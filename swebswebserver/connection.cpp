
//---------------------------------------------------------------------------------------------
/*
			CONNECTION.CPP
			--------------
			This file contains the definitions of the CONNECTION class member functions.

			Upon creating a new connection the calling function hands the connection
			the Socket Descriptor that the connection is on, and a sockaddr_in 
			structure which contains information about the client. For example:

				CONNECTION * NewRequest = new CONNECTION (SFD, CLA);

			Where SFD is the Socket Descriptor, and CLA is a sockaddr_in structure.
			The constructor then recieves the request from the client, and breaks it up
			into all the possible values that might be used in the connection, such as
			the type of request, file requested, host, MIME types accepted by the client,
			etc. 

			Then, the constructor checks that the file requested by the client exists.
			If it does, it checks if it is a folder or a file. If its a file it gets the
			extension and cross references it with the appropriate MIME type. Then, it 
			checks if the file is a CGI script or a binary file, and sets the flags
			appropriately. If it is just a plain text file, neither flags are set.

			Update:
			A security bug was found which if the user was to use the URL:
			www.anyhost.com/../../windows

			Would list the windows directory. The same theory would work for all directories,
			thus giving full access to a system. 
*/
//---------------------------------------------------------------------------------------------
#pragma warning(disable:4786)
#include <windows.h>
#include <string>
#include <winsock.h>
#include <map>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <ctime>										
#include "headermap.hpp"															// Contains functions to map headers
#include "connection.hpp"															// Has the declaration of the CONNECTION class
#include "stats.hpp"

using namespace std;


// The following is what GetFileAttributes() returns when the file specified
//  does not exist. This should be in windows.h somewhere, but its not. I 
//  had to discover this value for myself. Don't lose it!


#define FILE_INVALID						4294967295
int CGICounter = 0;																	// Counter for every CGI script processed


//---------------------------------------------------------------------------------------------
//			Connection::CONNECTION
//---------------------------------------------------------------------------------------------
CONNECTION::CONNECTION(int SFD_SET, struct sockaddr_in CA)
{
	//-----------------------------------------------------------------------------------------
	//			Change settings
	//-----------------------------------------------------------------------------------------
	SFD = SFD_SET;																	// Set the socket descriptor
	ClientAddress = CA;																// Assign the client address
	UseVH = false;																	// Dont use a Virtual host by default
	IsFolder = false;																// By default its not a folder
	IsBinary = false;																// Nor is it binary
	IsScript = false;																// Or a CGI script
	IsAbsolute = false;																// And the URL is not an absolute URL
	Status = 200;																	// But the file is always served fine
	// Before we loop through keys/values, make sure HeaderMapInit is called
	HeaderMapInit();
}

CONNECTION::CONNECTION()
{
	
}
//---------------------------------------------------------------------------------------------
//			Connection::SetFileType
//---------------------------------------------------------------------------------------------
bool CONNECTION::SetFileType()
{
	// Get the files extension
	int X = RealFile.find_last_of('.') + 1;											// Get the last '.' in the string

	if (X > 0)
	{
		for (X; RealFile[X] != '\0'; X++)											// Copy everything after that to the extension
		{
			Extension+= RealFile[X];						
		}

		if (Options.CGI[Extension].length() > 1)									// If theres an enrty in the CGI map, its a script
		{
			IsScript = true;
			IsBinary = false;
		}
		else if (Options.Binary[Extension])											// Or if theres an entry in the Binary map, its binary
		{
			IsScript = false;
			IsBinary = true;
		}
		else																		// It must be text then
		{
			IsBinary = false;
			IsScript = false;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------------------------
//			Connection::ReadRequest
//---------------------------------------------------------------------------------------------
bool CONNECTION::ReadRequest()
{
	//-----------------------------------------------------------------------------------------
	//			Set request variables
	//-----------------------------------------------------------------------------------------
	// First, read in the whole request
	char Buffer[10000];
	recv(SFD, Buffer, 10000, 0);
	
	string Word;																	// Temporary place to store each word
	FullRequest = Buffer;

	//-----------------------------------------------------------------------------------------------------
	// Break off any POST data following a double newline
	int Position = FullRequest.find_last_of("\n\n") - 1;
	
	if (Position > 0)	
	{
		FullRequest[Position] = '\0';												// Put a \0 where the \n\n are.

		for ( int C = Position + 2; FullRequest[C] != '\0'; C++)
		{
			PostData += FullRequest[C];												// Copy everything after that to the PostData
		}
	}

	//-----------------------------------------------------------------------------------------------------
	// Split it into words
	istringstream IS(FullRequest);													// Create an istringstream class

	//-----------------------------------------------------------------------------------------------------
	IS >> Word;																		// The first word will be the request type
	
	//strupr(Word.c_str());										
	if (!( strcmpi(Word.c_str(), "POST") ||											// Check to see if its a method we support
		 strcmpi(Word.c_str(), "GET")  ||
		 strcmpi(Word.c_str(), "HEAD" )))
	{
		// Its not a request we like
		Status = 501;																// Send a 501 Not Implemented
		return false;
	}
	
	//-----------------------------------------------------------------------------------------------------
	RequestType = Word;																// The request type was ok, assign it
	IS >> FileRequested;															// The next word will be the requested file
	IS >> HTTPVersion;																// Then the HTTP version
	IS >> Word;
	//-----------------------------------------------------------------------------------------------------
	// Map keys to values
	while (Word.length())
	{
		// Loop through, mapping keys to values
		if (HeaderMap[Word] != NULL)												// Has there been a function assigned to this header?
		{
            HeaderMap[Word](IS, this);												// If there has, call it!
		}
		// Now, check if its a MIME type. If it DOES NOT have ':' and DOES have '/', then its probably mime
		else if (!strstr(Word.c_str(), ":") && strstr(Word.c_str(), "/"))
		{
			if (strstr(Word.c_str() , ","))
			{
				// There is a comma at the end, get rid of it
				int Y = Word.length() - 1;
				Word[Y] = '\0';
			}
			Accepts[Word] = true;
		}
		IS >> Word;
	}

	//-----------------------------------------------------------------------------------------------------
	// First, if the request is HTTP/1.1, there must be a host field
	if ( !strcmpi (HTTPVersion.c_str(), "HTTP/1.1") )
	{
		if (HostRequested.length() <= 0)
		{
			Status = 400;															// No host was specified. Send 400 Bad Request
			return false;
		}
	}
	
	//-----------------------------------------------------------------------------------------------------
	// Cut off absolute URL, making us able to serve all future HTTP versions.
	if (strstr( FileRequested.c_str() , "http://" ))								// If its an absolute URL
	{	 
		IsAbsolute = true;															// Start at the end of the http://
		int CurrLetter = FileRequested.find_first_of("http://") + 7;
		
		for (CurrLetter; FileRequested[CurrLetter] != '/'; CurrLetter++ )
		{
			HostRequested += FileRequested[CurrLetter];								// Copy to the new host.
		}
	} 
	
	//-----------------------------------------------------------------------------------------------------
	// Figue out the virtual host
	ThisHost = &VHI.Host[HostRequested];						
	if(ThisHost->Root.length() < 1)													// If there is an entry for the host in the VHI 
		UseVH = false;																//  then it is a virtual host
	else UseVH = true;

	//-----------------------------------------------------------------------------------------------------
	// Cut off query string
	int X = FileRequested.find_last_of('?') + 1;									// Get the last '?' in the string
	if (X > 0)
	{
		int Y = X;																	// Save position of X
		for (X; FileRequested[X] != '\0'; X++)										// Copy everything after that to the extension
		{
			QueryString+= FileRequested[X];						
		}
		FileRequested[Y - 1] = '\0';												// Chop it off at the '?'
	}
		
    //-----------------------------------------------------------------------------------------------------
    // URL Encoding
    // %20 = " "
    int S = 0;
    while ((S = FileRequested.find("%20", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, " ");
    }
    // %24 = "$"
    while ((S = FileRequested.find("%24", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "$");
    }
    // %26 = "&"
    while ((S = FileRequested.find("%26", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "&");
    }
    // %2B = "+"
    while ((S = FileRequested.find("%2B", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "+");
    }
    while ((S = FileRequested.find("%2C", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, ",");
    }
    while ((S = FileRequested.find("%2F", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "/");
    }
    while ((S = FileRequested.find("%3A", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, ":");
    }
    while ((S = FileRequested.find("%3B", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, ";");
    }
    while ((S = FileRequested.find("%3D", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "=");
    }
    while ((S = FileRequested.find("%3F", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "?");
    }
    while ((S = FileRequested.find("%40", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "@");
    }
    while ((S = FileRequested.find("%22", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "\"");
    }
    while ((S = FileRequested.find("%3C", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "<");
    }
    while ((S = FileRequested.find("%3E", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, ">");
    }
    while ((S = FileRequested.find("%23", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "#");
    } 
    while ((S = FileRequested.find("%25", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "%");
    }
    while ((S = FileRequested.find("%7B", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "{");
    }
    while ((S = FileRequested.find("%7D", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "}");
    }
    while ((S = FileRequested.find("%7C", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "|");
    }
    while ((S = FileRequested.find("%5C", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "\\");
    }
    while ((S = FileRequested.find("%5E", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "^");
    }
    while ((S = FileRequested.find("%7E", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "~");
    }
    while ((S = FileRequested.find("%5B", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "[");
    }
    while ((S = FileRequested.find("%5D", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "]");
    }
    while ((S = FileRequested.find("%60", 0)) != string::npos)
    {
        FileRequested.replace(S , 3, "`");
    }
	//-----------------------------------------------------------------------------------------------------
	// Change slashes from *nix to windows
	for (int Z = 0; FileRequested[Z] != '\0'; Z++)									// Replace / with \ 
	{
		if (FileRequested[Z] == '/') FileRequested[Z] = '\\';
	}
	
	//-----------------------------------------------------------------------------------------------------
	// Assign full path based on virtual hosts
	if (UseVH) RealFile = ThisHost->Root + FileRequested;
	else RealFile = Options.WebRoot + FileRequested;
		
	// Check for a "..", if found send a 404. Because this will allow them to go one folder back, and 
	//  then get files from there, effectively giving full access to the system - thanks to Adam for pointing this out!
	if (strstr(RealFile.c_str() , ".."))
	{
		Status = 404;
		return false;
	}

	//-----------------------------------------------------------------------------------------------------
	// Check if the file is a folder
	DWORD hFile = GetFileAttributes(RealFile.c_str());
	if (hFile == FILE_INVALID)
	{
		
		Status = 404;																// File does not exist. Return error 404
		return false;
	}
	
	//-----------------------------------------------------------------------------------------------------
	if (hFile & FILE_ATTRIBUTE_DIRECTORY)				
	{
		IsFolder = true;															// Is a folder
	}
	else	
	{								
		IsFolder = false;															// Is not folder
	}

	//-----------------------------------------------------------------------------------------------------	
	SetFileType();																	// Set whether the file is binary or a script
	Status = 200;																	// It passed all the tests, therefore its ok
	return true;
}

//---------------------------------------------------------------------------------------------
//			Connection::HandleRequest
//---------------------------------------------------------------------------------------------
bool CONNECTION::HandleRequest()
{
	// Get the time:
	time_t now;
	struct tm *tm_now;
	char buff[BUFSIZ];

	now = time ( NULL );
	tm_now = gmtime(&now);
	strftime ( buff, sizeof buff, "%a, %d %b %Y %H:%M:%S GMT", tm_now );
	Date = buff;

	//----------------------------------------------------------
	// Do the request
	if (Status == 200)
	{
		if (UseModDate == true)														// If we got a last modified header:
		{
			if (!ModifiedSince(ModifiedSinceStr))									// If the file has not been modifed
			{
				Status = 304;														// Send them a 304 not modifed
			}
		}
		if (UseUnModDate == true)													// If we got a last modified header:
		{
			if (!UnModifiedSince(UnModifiedSinceStr))								// If the file has not been modifed
			{
				Status = 402;														// Send them a 402 not modifed
			}
		}
		// Output the file
		if (IsFolder)																// Request was a folder
		{
			// Search for an index file
			int X = 0;
			bool Found = false;
			while ((X < Options.IndexFiles.size()) && !Found)						// While theres still index files there
			{
				string File = RealFile;
				File += "\\";
				File += Options.IndexFiles[X];
				ifstream hFile (File.c_str());
				if (!hFile)
					X++;															// Increase the counter
				else
					Found = true;													// Found one!
			}
			if (Found)
			{
				RealFile += "\\";
				RealFile += Options.IndexFiles[X];									// Make the file were looking for the appropriate index file
				IsFolder = false;									
			}																		// Not a folder anymore, break out and send as a file
			// If we are allowed to index:
			if (Options.AllowIndex == true && IsFolder)								// Ensure its still a folder
			{
				bool hResult = IndexFolder();										// Try to index the folder
				if (hResult == false)												// The folder could not be indexed. Report
				{
					Status = 404;													// Set status code
				}
			}
            SetFileType();
		}
		      
        // We are ready to process. Set CGI environment varaibles
        CGIVariables.CONTENT_LENGTH = PostData.length();
        CGIVariables.PATH_TRANSLATED = RealFile;
        CGIVariables.QUERY_STRING = QueryString;
        CGIVariables.REMOTE_ADDRESS = inet_ntoa(ClientAddress.sin_addr);
        CGIVariables.REQUEST_METHOD = RequestType;
        CGIVariables.SCRIPT_NAME = FileRequested;
        CGIVariables.SERVER_PORT = Options.Port;
        CGIVariables.SERVER_PROTOCOL = HTTPVersion;

        CGIVariables.HTTP_USER_AGENT = UserAgent;

        // Now process the request
		if (!IsFolder)																// Request was a file
		{
			if (IsBinary == true)
			{
				// The file is a binary file
				Headers = HTTPVersion;												// Send HTTP version
				Headers += " ";											
				Headers += IntToString(Status);										// Send status code
				Headers += " ";			
				Headers += "OK\n";													// Send OK msg
				Headers += "Server: ";
				Headers += Options.Servername;
				Headers += "\nConnection: ";										// Connection type
				Headers += "close\n";
				Headers += "Date: ";
				Headers += Date;
				Headers += "\nContent-type: ";										// Content type
				if (Options.MIMETypes[Extension].length() > 0)						// If we know the mime type
				{
					Headers += Options.MIMETypes[Extension];						// Send it
				}
				else																// Or if we don't know it
				{
					Headers += "image/jpeg";										// Send image/jpg
				}
				Headers += "\nContent-length: ";
				Headers += CalculateSize();
				Headers += "\n\n";													// Double newlines

				Send (SFD, Headers.c_str(), Headers.length(), 0);					// Send headers

				// Then, if its a GET of POST request, send the file requested
				if ( !strcmpi(RequestType.c_str(), "GET") || !strcmpi(RequestType.c_str(), "POST") )
					SendBinary();
			}
			else if (IsScript == true)
			{
				// The file is a CGI script
				Headers = HTTPVersion;												// Send HTTP version
				Headers += " ";											
				Headers += IntToString(Status);										// Send status code
				Headers += " ";			
				Headers += "OK\n";													// Send OK msg
				Headers += "Server: ";
				Headers += Options.Servername;
				Headers += "Date: ";
				Headers += Date;
				Headers += "\nConnection: close ";									// Connection type
				// Note: We do not send the content type OR the double newlines, the
				//  CGI interpreter must do that itself.	

				Send (SFD, Headers.c_str(), Headers.length(), 0);					// Send headers

				// Then, if its a GET of POST request, send the file requested
				if ( !strcmpi(RequestType.c_str(), "GET") || !strcmpi(RequestType.c_str(), "POST") )
					SendCGI();
			}
			else
			{
				// The file is plain text
				Headers = HTTPVersion;												// Send HTTP version
				Headers += " ";											
				Headers += IntToString(Status);										// Send status code
				Headers += " ";			
				Headers += "OK\n";													// Send OK msg
				Headers += "Server: ";
				Headers += Options.Servername;
				Headers += "\nConnection: ";										// Connection type
				Headers += "close\n";
				Headers += "Date: ";
				Headers += Date;
				Headers += "\nContent-type: ";										// Content type
                // Make EXTENSION into small letters
                char Ext[20];
                strcpy(Ext, Extension.c_str());
                strlwr(Ext);
				if (Options.MIMETypes[Ext].length() > 0)	    					// If we know the mime type
				{
					Headers += Options.MIMETypes[Ext];      						// Send it
				}
				else																// Or if we don't know it
				{
					Headers += "text/plain";										// Send text/plain
				}
				Headers += "\n\n";													// Double newlines
				Send (SFD, Headers.c_str(), Headers.length(), 0);					// Send headers
	
				// Then, if its a GET of POST request, send the file requested
				if ( !strcmpi(RequestType.c_str(), "GET") || !strcmpi(RequestType.c_str(), "POST"))
					SendText();
			}
		}
	}
	
    // Write to the stats file
    unsigned long Size = StringToInt(CalculateSize());
    SWEBSStats.TotalBytesSent += Size;
    SWEBSStats.TotalNumberOfRequests++;
    if (UseVH)
    {
        SWEBSStats.VirtualHosts[*ThisHost].BytesSent += Size;
        SWEBSStats.VirtualHosts[*ThisHost].NumberOfRequests++;
        SWEBSStats.VirtualHosts[*ThisHost].PageRequests[FileRequested] += 1;
    }
    else
    {
        SWEBSStats.BytesSent += Size;
        SWEBSStats.NumberOfRequests++;
        SWEBSStats.PageRequests[FileRequested] += 1;
    }

    // If we are logging, write the logs here:
    if (Options.Logfile.length() > 0)
    {
        LogText(inet_ntoa(ClientAddress.sin_addr));                                 // IP Address
        LogText(" - - [");                                                          // Normally the name goes here
        strftime ( buff, sizeof buff, "%d/%b/%Y:%H:%M:%S +0000", tm_now );          // Get the time (GMT)
	    LogText(buff);
        LogText("] \"");
        LogText(RequestType);                                                       // Request line
        LogText(" ");
        LogText(FileRequested);
        LogText(" ");
        LogText(HTTPVersion);
        LogText("\" ");
        LogText(IntToString(Status));                                               // Status code
        LogText(" ");
        LogText(IntToString(Size));                                                 // Size
        LogText(" \"");
        LogText(Referer);                                                           // Referer
        LogText("\" \"");
        LogText(UserAgent);                                                         // User agent
        LogText("\"\n");
    }

    // If the status wasn't 200 to start with, or it was somehow changed along the way:
	if (Status != 200)
	{
		SendError();																// Send the error
		return false;
	}
	return true;																	// No errors. Return true
}

//---------------------------------------------------------------------------------------------
//			Connection::SendText
//---------------------------------------------------------------------------------------------
bool CONNECTION::SendText()
{
	char Buffer[10000];																// Buffer to store data
	string Text;																	// String to store everything
	ifstream hFile (RealFile.c_str());		
	while (!hFile.eof())															// Go until the end
	{
		hFile.getline(Buffer, 10000);												// Read a full line
		Text += Buffer;																// Put it in the string
		Text += "\n";																// Remember to add the \n
	}
	hFile.close();																	// Close
																					// Send the data
	int Y = Send (SFD, Text.c_str(), Text.length(), 0);
	if (Y != 0)					
		return true;																// It sent fine
	else
		return false;																// Errors sending
}

//---------------------------------------------------------------------------------------------
//			Connection::SendBinary
//---------------------------------------------------------------------------------------------
bool CONNECTION::SendBinary()
{
	// WOOHOO! Do not lose this function, it took me ages to learn how to send binary files,
	//  and now it finally works!
    char Buffer[10000];
																                    // Open the file as binary
	ifstream hFile (RealFile.c_str(), ios::binary);
	int X = 0;
	while (!hFile.eof())                                                            // Keep reading it in
    {
        hFile.read(Buffer, 10000);
        X += send(SFD, Buffer, hFile.gcount(), 0);			                        // Send data as we read it
    }
    hFile.close();                                                                  // Close  
    
    SWEBSStats.TotalBytesSent += X;
    if (UseVH)
    {
        SWEBSStats.VirtualHosts[*ThisHost].BytesSent += X;
    }
    else
    {
        SWEBSStats.BytesSent += X;
    }
    
    return true;
}

//---------------------------------------------------------------------------------------------
//			Connection::SendCGI
//---------------------------------------------------------------------------------------------
bool CONNECTION::SendCGI()
{
	// This is where we will use the program provided by Volkan Uzun.
    string Message;
    Message = "Content-type: text/html\n\n";
    Message += "<font face='Verdana'><small><center>";
    Message += "<h1>Error 500 - Internal Server Error</h1><br>";
    Message += "At this stage the SWEBS Web Server cannot use CGI due to a problem with creating";
    Message += " named pipes as an NT service. We are working on a way around this issue,";
    Message += " and it will be working for version 1.0 of the server. Thankyou for your";
    Message += " patience and support.";
    Message += "</center></small></font>";

    return true;
}



//---------------------------------------------------------------------------------------------
//			Connection::IndexFolder()
//---------------------------------------------------------------------------------------------
bool CONNECTION::IndexFolder()
{
	// Check if we are allowed
	if (Options.AllowIndex == false)
	{
		Status = 404;
		return false;
	}
	else
	{	
		// Open and list folder contents
		HANDLE hFind;
		WIN32_FIND_DATA FindData;
		int ErrorCode;
		BOOL Continue = true;

		string FileIndex = RealFile + "\\*.*";										// List all files
		hFind = FindFirstFile(FileIndex.c_str(), &FindData);

		string Text;
		for (int Z = 0; FileRequested[Z] != '\0'; Z++)								// Replace \ with / 
		{
			if (FileRequested[Z] == '\\') FileRequested[Z] = '/';
		}

		//-----------------------------
		if(hFind == INVALID_HANDLE_VALUE)
		{
			Status = 404;
			return false;
		}
		else
		{	
			Headers = HTTPVersion;													// Send HTTP version
			Headers += " 200 OK\n";								
			Headers += "Server: SWS Stovell Web Server 2.0\n";						// Server name
			Headers += "Connection: close";
			Headers += "\nContent-type: text/html";									// Content type
			Headers += "\n\n";														// Double newlines
			Send (SFD, Headers.c_str(), Headers.length(), 0);						// Send headers

			// Most of this is all HTML bieng generated													
			Text = "<html>\n<head>\
\n<title>Index of ";
			Text += FileRequested;													// Insert the file requested					
			Text += "</title>\
\n</head>\
\
\n<body>\
\n<!-- Title -->\
\n<p align=\"center\">\
\n  <font face=\"Verdana\" size=\"6\">\
\n    Index of ";
			Text += FileRequested;													// Insert the file requested
			Text += "\n  </font>\
\n</p>\
\n\
\n<hr>\
\n\
\n<center>\
\n<table border=\"0\" width=\"75%\" height=\"6\" cellspacing=\"0\" cellpadding=\"4\">\
\n  <tr>\
\n    <td width=\"40%\" height=\"1\" bgcolor=\"#0080C0\"><p align=\"center\"><strong><small><font\
\n    face=\"Verdana\" color=\"#FFFFFF\">Name</font></small></strong></td>\
\n    <td width=\"23%\" height=\"1\" bgcolor=\"#0080C0\"><p align=\"center\"><font face=\"Verdana\"\
\n    color=\"#FFFFFF\"><strong><small>Size</small></strong></font></td>\
\n    \
\n  </tr>";
																					// Now, print the first entry
			Text += "  <tr>\
\n    <td width=\"40%\" height=\"0\" bgcolor=\"#E2E2E2\"><p align=\"left\"><small><font face=\"Verdana\">";
			Text += "<a href=\"";
			Text += FileRequested;													// Current folder
			Text += "/";
			Text += FindData.cFileName;												// Name of file
			Text += "\">";
			Text += FindData.cFileName;												// Name of file
			Text += "</a></font></small></td>\n";
			Text += "\n    <td width='23%' height='0' bgcolor='#C0C0C0'>\
\n    <p align='center'><small><font face='Verdana'>";
			if (IntToString(FindData.nFileSizeHigh)[0] != '0')
				Text += IntToString(FindData.nFileSizeHigh);
			if (IntToString(FindData.nFileSizeLow)[0] != '0')
				Text += IntToString(FindData.nFileSizeLow);
			Text += "\n</font></small></td>\
\n    </font></small></td>";
			Send(SFD, Text.c_str(), Text.length(), 0);
		}

		if (Continue)
		{
			while (FindNextFile(hFind, &FindData))
			{
				Text = "  <tr>\
\n    <td width=\"40%\" height=\"0\" bgcolor=\"#E2E2E2\"><p align=\"left\"><small><font face=\"Verdana\">";
			Text += "<a href=\"";
			if (strcmpi(FileRequested.c_str(), "/"))
				Text += FileRequested;												// Current folder
			Text += "/";
			Text += FindData.cFileName;												// Name of file
			Text += "\">";
			Text += FindData.cFileName;												// Name of file
			Text += "</a></font></small></td>\n";
			Text += "\n    <td width='23%' height='0' bgcolor='#C0C0C0'>\
\n    <p align='center'><small><font face='Verdana'>";
			
			if (IntToString(FindData.nFileSizeHigh)[0] != '0')
				Text += IntToString(FindData.nFileSizeHigh);
			if (IntToString(FindData.nFileSizeLow)[0] != '0')
				Text += IntToString(FindData.nFileSizeLow);
			Text += "\n</font></small></td>\
\n    </font></small></td>";
				Send(SFD, Text.c_str(), Text.length(), 0);
			}

			ErrorCode = GetLastError();

			if (ErrorCode == ERROR_NO_MORE_FILES)
			{
				Text = "</tr>\
\n</table>\
\n</div>\
\n\
\n<hr>\
\n\
\n<p align='center'><small><small><font face='Verdana'>Index produced automatically by <a\
\nhref='http://swebs.sourceforge.net'>SWS Web Server</a></font></small></small></p>\
\n</body>\
\n</html>";
				Send(SFD, Text.c_str(), Text.length(), 0);
			}

        FindClose(hFind);
       
		}
	return true;
	}
}

//---------------------------------------------------------------------------------------------
//			Connection::SendError()
//			Sends the appropriate error.
//---------------------------------------------------------------------------------------------
bool CONNECTION::SendError()
{	
	RealFile = Options.ErrorDirectory;												// Where the error files are kept
	RealFile += '\\';																// Add a backslash to be safe
	RealFile += IntToString(Status);												// Error code
	RealFile += ".html";															// Extension

	char Buffer[10000];																// Buffer to store data

	string ErrorPage;

	ifstream hFile (RealFile.c_str());	
	if (hFile)																		// The file exists
	{
		ErrorPage = "HTTP/1.1 ";													// Since we have an HTML file to send them, just send a 200
		ErrorPage += "200";
		ErrorPage += ' ';
		ErrorPage += "OK";
		ErrorPage += "\nContent-type: text/html\n\n";
		while (!hFile.eof())														// Go until the end
		{
			hFile.getline(Buffer, 10000);											// Read a full line
			ErrorPage += Buffer;													// Put it in the string
			ErrorPage += "\n";														// Remember to add the \n
		}
		hFile.close();																// Close
	}
	
	else																			// There was no custom error page for this code
	{					
		ErrorPage = "HTTP/1.1 ";								
		ErrorPage += IntToString(Status);											// Send the appropriate status code
		ErrorPage += ' ';
		ErrorPage += Options.ErrorCode[Status];
		ErrorPage += "\nContent-type: text/html\n\n";

		ErrorPage += "<html><body><center><b>";										// Send a basic and boring looking error page
		ErrorPage += IntToString(Status);
		ErrorPage += " ";
		ErrorPage += Options.ErrorCode[Status];
		ErrorPage += "</b></body></html>";
	}
	int Y = Send (SFD, ErrorPage.c_str(), ErrorPage.length(), 0);

	if (Y != 0)					
		return true;																// It sent fine
	else
		return false;																// Errors sending
	
	return false;
}

//---------------------------------------------------------------------------------------------
//			Connection::LogText()
//			Logs the string passed as an argument. This is just a temporary log, 
//			used for testing. A propper logging function must be written to replace this.
//---------------------------------------------------------------------------------------------
bool CONNECTION::LogText(string Text)
{
	FILE* log;
    if (UseVH)                                                                      // Check if this connection uses a VH
    {
	    log = fopen(ThisHost->Logfile.c_str(), "a+");                                       // It does, so open its log file
    }
    else 
    {
        log = fopen(Options.Logfile.c_str(), "a+");                                        // It doesn't, so use the default one
    }
	if (log == NULL)
    {
        return false;
    }
	fprintf(log, "%s", Text.c_str());                                               // Write the string
	fclose(log);                                                                    // Close it
	return true;
}

//---------------------------------------------------------------------------------------------
//			Connection::CalculateSize()
//			Calculates and returns the size of the file requested.
//---------------------------------------------------------------------------------------------
string CONNECTION::CalculateSize()
{
	string ReturnValue;
	unsigned long Size = 0;

	HANDLE hFile = CreateFile(
				RealFile.c_str(), 
				GENERIC_READ,														// Open for reading 
                FILE_SHARE_READ,													// Share for reading 
                NULL,																// No security 
                OPEN_EXISTING,														// Existing file only 
                FILE_ATTRIBUTE_NORMAL,												// Normal file 
                NULL);											

	if (hFile != INVALID_HANDLE_VALUE)												// If the file was opened
	{
		Size = GetFileSize(hFile, NULL);											// Get the size
	}
	
	CloseHandle(hFile);
	ReturnValue = IntToString(Size);												// Convert it to a printable string
	return ReturnValue;
}


//---------------------------------------------------------------------------------------------
//			Connection::ModifiedSince()
//			Checks if the file was modified since the given date
//---------------------------------------------------------------------------------------------
bool CONNECTION::ModifiedSince(string Date)
{
	//----------------------------------------------------------
	string Temp;
	time_t now;
	struct tm *tm_now = new tm;
	struct tm *tm_date = new tm;

	// Get the time the file was last modified:
	SYSTEMTIME myTime;																// Windows time structure
	HANDLE hFind;
    WIN32_FIND_DATA FindData;

	now = time ( NULL );															// Get the current time
	tm_date = localtime ( &now );
	
	hFind = FindFirstFile(RealFile.c_str(), &FindData);								// Open the file
	FileTimeToSystemTime(&FindData.ftLastWriteTime, &myTime);						// Get its time
	
	tm_now->tm_hour = myTime.wHour + 1;												// Copy the time settings
	tm_now->tm_min = myTime.wMinute;
	tm_now->tm_mday = myTime.wDay;
	tm_now->tm_mon = myTime.wMonth - 1;
	tm_now->tm_sec = myTime.wSecond;
	tm_now->tm_year = myTime.wYear;
	tm_now->tm_year -= 1900;

	//----------------------------------------------------------
	int Day;
	string Month;
	int Year;
	int Hour;
	int Minute;
	int Second;
	
	int Type = 0;

	istringstream DateString1(Date);
	DateString1 >> Temp;

	if (Temp[3] == ',')
	{
		Type = 1;
		//------------------------------------------------------
		for (int X = 0; X < Date.length(); X++)
		{
			if ( Date[X] == ':' )
				Date[X] = ' ';
		}

		istringstream DateString(Date);
		DateString >> Temp;
		//------------------------------------------------------
		DateString >> Day;
		DateString >> Month;
		DateString >> Year;
		DateString >> Hour;
		DateString >> Minute;
		DateString >> Second;
	}
	else if (Temp.length() > 3)
	{
		Type = 2;
		//------------------------------------------------------
		for (int X = 0; X < Date.length(); X++)
		{
			if ( Date[X] == ':' )
				Date[X] = ' ';
		}
		for (int Z = 0; Z < Date.length(); Z++)
		{
			if ( Date[Z] == '-' )
				Date[Z] = ' ';
		}
		istringstream DateString(Date);
		DateString >> Temp;
		//------------------------------------------------------
		DateString >> Day;
		DateString >> Month;
		DateString >> Year;
		Year += 2000;
		if (Year > 2050)
			return true;
		DateString >> Hour;
		DateString >> Minute;
		DateString >> Second;
	}
	else if (Temp[3] != ',')
	{
		Type = 3;
		//------------------------------------------------------
		for (int X = 0; X < Date.length(); X++)
		{
			if ( Date[X] == ':' )
				Date[X] = ' ';
		}

		istringstream DateString(Date);
		DateString >> Temp;
		//------------------------------------------------------
		
		DateString >> Month;
		DateString >> Day;
		DateString >> Hour;
		DateString >> Minute;
		DateString >> Second;
		DateString >> Year;
	}

	//----------------------------------------------------------
	tm_date->tm_hour = Hour;
	tm_date->tm_mday = Day;
	tm_date->tm_min = Minute;
	tm_date->tm_mon = CalcMonth(Month);
	tm_date->tm_sec = Second;
	tm_date->tm_year = Year - 1900;
	
	//----------------------------------------------------------
	
	int Y = difftime(mktime(tm_date), mktime(tm_now));
	delete tm_now;
	delete tm_date;
	if (Y < 0)
	{
		return true;
	}
	else return false;
}


//---------------------------------------------------------------------------------------------
//			Connection::UnModifiedSince()
//			Returns a negative of ModifiedSince()
//---------------------------------------------------------------------------------------------
bool CONNECTION::UnModifiedSince(string Date)
{
	bool X = ModifiedSince(Date);
	if (X)
	{
		return false;
	}
	else 
	{	
		return true;
	}
}

//---------------------------------------------------------------------------------------------
//			Connection::Send()
//          Our own version of send(), so that we can keep track of whats being sent
//---------------------------------------------------------------------------------------------
bool CONNECTION::Send(int SFD, string Text, int Length, int Nothing)
{
    // Some calls to send() have additional info that we want to get rid of, so we just ignore them here
    int NumSent = send (SFD, Text.c_str(), Length, 0);
    
    SWEBSStats.TotalBytesSent += NumSent;
    if (UseVH)
    {
        SWEBSStats.VirtualHosts[*ThisHost].BytesSent += NumSent;
    }
    else
    {
        SWEBSStats.BytesSent += NumSent;
    }

    if (NumSent <= 0)
        return false;
    else return true;
}

bool CONNECTION::Send(int SFD, string Text)
{
    // Send the data and write how much is sent to the stats file
    int NumSent = send (SFD, Text.c_str(), Text.length(), 0);
    
    SWEBSStats.TotalBytesSent += NumSent;
    if (UseVH)
    {
        SWEBSStats.VirtualHosts[*ThisHost].BytesSent += NumSent;
    }
    else
    {
        SWEBSStats.BytesSent += NumSent;
    }

    if (NumSent <= 0)
        return false;
    else return true;
}
//---------------------------------------------------------------------------------------------







