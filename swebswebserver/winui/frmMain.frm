VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Stovell Web Server - Control Center"
   ClientHeight    =   6120
   ClientLeft      =   150
   ClientTop       =   540
   ClientWidth     =   7920
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   6120
   ScaleWidth      =   7920
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdOK 
      Caption         =   "&OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   5280
      TabIndex        =   2
      Top             =   5640
      Width           =   1215
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   375
      Left            =   6600
      TabIndex        =   1
      Top             =   5640
      Width           =   1215
   End
   Begin TabDlg.SSTab sstMain 
      Height          =   5415
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   7695
      _ExtentX        =   13573
      _ExtentY        =   9551
      _Version        =   393216
      Style           =   1
      Tab             =   1
      TabHeight       =   520
      TabCaption(0)   =   "Server Status"
      TabPicture(0)   =   "frmMain.frx":0CCA
      Tab(0).ControlEnabled=   0   'False
      Tab(0).Control(0)=   "lblStatus"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).ControlCount=   1
      TabCaption(1)   =   "Configuration"
      TabPicture(1)   =   "frmMain.frx":0CE6
      Tab(1).ControlEnabled=   -1  'True
      Tab(1).Control(0)=   "sstConfig"
      Tab(1).Control(0).Enabled=   0   'False
      Tab(1).ControlCount=   1
      TabCaption(2)   =   "Logs"
      TabPicture(2)   =   "frmMain.frx":0D02
      Tab(2).ControlEnabled=   0   'False
      Tab(2).Control(0)=   "lblLogs"
      Tab(2).Control(0).Enabled=   0   'False
      Tab(2).ControlCount=   1
      Begin TabDlg.SSTab sstConfig 
         Height          =   4335
         Left            =   120
         TabIndex        =   4
         Top             =   480
         Width           =   7335
         _ExtentX        =   12938
         _ExtentY        =   7646
         _Version        =   393216
         Style           =   1
         Tabs            =   4
         TabsPerRow      =   5
         TabHeight       =   520
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         TabCaption(0)   =   "Basic"
         TabPicture(0)   =   "frmMain.frx":0D1E
         Tab(0).ControlEnabled=   -1  'True
         Tab(0).Control(0)=   "lblServerName"
         Tab(0).Control(0).Enabled=   0   'False
         Tab(0).Control(1)=   "lblPort"
         Tab(0).Control(1).Enabled=   0   'False
         Tab(0).Control(2)=   "lblWebroot"
         Tab(0).Control(2).Enabled=   0   'False
         Tab(0).Control(3)=   "txtServerName"
         Tab(0).Control(3).Enabled=   0   'False
         Tab(0).Control(4)=   "txtPort"
         Tab(0).Control(4).Enabled=   0   'False
         Tab(0).Control(5)=   "txtWebroot"
         Tab(0).Control(5).Enabled=   0   'False
         Tab(0).ControlCount=   6
         TabCaption(1)   =   "Advanced"
         TabPicture(1)   =   "frmMain.frx":0D3A
         Tab(1).ControlEnabled=   0   'False
         Tab(1).Control(0)=   "lblMaxConnect"
         Tab(1).Control(0).Enabled=   0   'False
         Tab(1).Control(1)=   "lblIndexFiles"
         Tab(1).Control(1).Enabled=   0   'False
         Tab(1).Control(2)=   "lblAllowIndex"
         Tab(1).Control(2).Enabled=   0   'False
         Tab(1).Control(3)=   "lblLogFile"
         Tab(1).Control(3).Enabled=   0   'False
         Tab(1).Control(4)=   "txtMaxConnect"
         Tab(1).Control(4).Enabled=   0   'False
         Tab(1).Control(5)=   "txtIndexFiles"
         Tab(1).Control(5).Enabled=   0   'False
         Tab(1).Control(6)=   "txtAllowIndex"
         Tab(1).Control(6).Enabled=   0   'False
         Tab(1).Control(7)=   "txtLogFile"
         Tab(1).Control(7).Enabled=   0   'False
         Tab(1).ControlCount=   8
         TabCaption(2)   =   "vHosts"
         TabPicture(2)   =   "frmMain.frx":0D56
         Tab(2).ControlEnabled=   0   'False
         Tab(2).ControlCount=   0
         TabCaption(3)   =   "CGI Handlers"
         TabPicture(3)   =   "frmMain.frx":0D72
         Tab(3).ControlEnabled=   0   'False
         Tab(3).ControlCount=   0
         Begin VB.TextBox txtLogFile 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   -74880
            TabIndex        =   14
            Top             =   1560
            Width           =   2655
         End
         Begin VB.TextBox txtAllowIndex 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   -74880
            TabIndex        =   13
            Top             =   2160
            Width           =   2655
         End
         Begin VB.TextBox txtIndexFiles 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   -74880
            TabIndex        =   12
            Top             =   2760
            Width           =   2655
         End
         Begin VB.TextBox txtMaxConnect 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   -74880
            TabIndex        =   11
            Top             =   960
            Width           =   2655
         End
         Begin VB.TextBox txtWebroot 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   120
            TabIndex        =   7
            Top             =   2400
            Width           =   3735
         End
         Begin VB.TextBox txtPort 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   120
            TabIndex        =   6
            Top             =   1680
            Width           =   2535
         End
         Begin VB.TextBox txtServerName 
            Appearance      =   0  'Flat
            Height          =   285
            Left            =   120
            TabIndex        =   5
            Top             =   840
            Width           =   2535
         End
         Begin VB.Label lblLogFile 
            Caption         =   "Log File"
            Height          =   255
            Left            =   -74880
            TabIndex        =   18
            Top             =   1320
            Width           =   2775
         End
         Begin VB.Label lblAllowIndex 
            Caption         =   "Allow index"
            Height          =   255
            Left            =   -74880
            TabIndex        =   17
            Top             =   1920
            Width           =   3375
         End
         Begin VB.Label lblIndexFiles 
            Caption         =   "Index files"
            Height          =   255
            Left            =   -74880
            TabIndex        =   16
            Top             =   2520
            Width           =   3015
         End
         Begin VB.Label lblMaxConnect 
            Caption         =   "How many simultainious connections do you want to accept?"
            Height          =   495
            Left            =   -74880
            TabIndex        =   15
            Top             =   480
            Width           =   3735
         End
         Begin VB.Label lblWebroot 
            Caption         =   "Where is the root folder for your server?"
            Height          =   255
            Left            =   120
            TabIndex        =   10
            Top             =   2160
            Width           =   3015
         End
         Begin VB.Label lblPort 
            Caption         =   "What port do you want to use? (Default is 80)"
            Height          =   375
            Left            =   120
            TabIndex        =   9
            Top             =   1200
            Width           =   2535
         End
         Begin VB.Label lblServerName 
            Caption         =   "What is the name of your server?"
            Height          =   255
            Left            =   120
            TabIndex        =   8
            Top             =   600
            Width           =   2535
         End
      End
      Begin VB.Label lblLogs 
         Caption         =   "This will be used to view log files... at some point i'll make this afancy feature, until then it'll be"
         Height          =   375
         Left            =   -74400
         TabIndex        =   19
         Top             =   1680
         Width           =   6255
      End
      Begin VB.Label lblStatus 
         Caption         =   "This will be the main status page, from here you can start/stop the service, check various stats, etc..."
         Height          =   375
         Left            =   -74640
         TabIndex        =   3
         Top             =   1680
         Width           =   6855
      End
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileSave 
         Caption         =   "&Save Data..."
      End
      Begin VB.Menu mnuFileReload 
         Caption         =   "&Reload Data..."
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About..."
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'***************************************************************************
'
' SWEBS/WinUI
'
' Copyright (c) 2003 Adam Caudill.
'
' This program is free software; you can redistribute it and/or modify
' it under the terms of the GNU General Public License as published by
' the Free Software Foundation; either version 2 of the License, or
' (at your option) any later version.
'
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.
'
' You should have received a copy of the GNU General Public License
' along with this program; if not, write to the Free Software
' Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
'***************************************************************************

Option Explicit

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdOK_Click()
    If SaveConfigData(strConfigFile) = False Then
        MsgBox "Data was not saved, no idea why..."
    Else
        Unload Me
    End If
End Sub

Private Sub Form_Load()
Dim RetVal As Long
    If LoadConfigData(strConfigFile) = False Then
        RetVal = MsgBox("There was an error while loading your configuration data." & vbCrLf & vbCrLf & "Press 'Abort' to give up and exit, 'Retry' to try to load th data again," & vbCrLf & "or 'Ignore' to continue.", vbCritical + vbAbortRetryIgnore + vbApplicationModal)
        Select Case RetVal
            Case vbAbort
                End
            Case vbRetry
                If LoadConfigData(strConfigFile) = False Then
                    MsgBox "A second attempt to load your configuration data failed. Aborting." & vbCrLf & vbCrLf & "This application will now close.", vbApplicationModal + vbCritical
                    End
                End If
            Case vbIgnore
                MsgBox "NOTICE: You have chosen to proceed after a data error, this application may" & vbCrLf & "not function properly or you may loose data."
        End Select
    End If
End Sub

Private Function LoadConfigData(strCurConfigFile As String) As Boolean
'<CSCM>
'--------------------------------------------------------------------------------
' Project    :       WinUI
' Procedure  :       LoadConfigData
' Description:       This loads the data from the config XML file, returns true
'                    if the load is sucessful, otherwise returns false
' Created by :       Adam
' Date-Time  :       8/24/2003-3:01:42 PM
' Parameters :       strCurConfigFile (String)
'--------------------------------------------------------------------------------
'</CSCM>

Dim XML As CHILKATXMLLib.XmlFactory
Dim ConfigXML As CHILKATXMLLib.IChilkatXml
Dim Node As CHILKATXMLLib.IChilkatXml
    
    Set XML = New XmlFactory
    Set ConfigXML = XML.NewXml
    ConfigXML.LoadXmlFile strCurConfigFile
    
    '<ServerName>
    Set Node = ConfigXML.SearchForTag(Nothing, "ServerName")
    txtServerName.Text = Node.Content
    
    '<Port>
    Set Node = ConfigXML.SearchForTag(Nothing, "Port")
    txtPort.Text = Node.Content
    
    '<Webroot>
    Set Node = ConfigXML.SearchForTag(Nothing, "Webroot")
    txtWebroot.Text = Node.Content
    
    '<MaxConnections>
    Set Node = ConfigXML.SearchForTag(Nothing, "MaxConnections")
    txtMaxConnect.Text = Node.Content
    
    '<LogFile>
    Set Node = ConfigXML.SearchForTag(Nothing, "LogFile")
    txtLogFile.Text = Node.Content
    
    '<AllowIndex>
    Set Node = ConfigXML.SearchForTag(Nothing, "AllowIndex")
    txtAllowIndex.Text = Node.Content
    
    '<IndexFile>
    Set Node = ConfigXML.SearchForTag(Nothing, "IndexFile")
    txtIndexFiles.Text = ""
    Do While Not (Node Is Nothing)
        txtIndexFiles.Text = txtIndexFiles.Text & Node.Content & " "
        Set Node = ConfigXML.SearchForTag(Node, "IndexFile")
    Loop
    
    LoadConfigData = True
End Function

Private Sub mnuFileReload_Click()
Dim RetVal As Long
    RetVal = MsgBox("This will reset any changes you make." & vbCrLf & vbCrLf & "Do you want to continue?", vbYesNo + vbQuestion)
    If RetVal = vbYes Then
        If LoadConfigData(strConfigFile) = False Then
            RetVal = MsgBox("There was an error while loading your configuration data." & vbCrLf & vbCrLf & "Press 'Abort' to give up and exit, 'Retry' to try to load th data again," & vbCrLf & "or 'Ignore' to continue.", vbCritical + vbAbortRetryIgnore + vbApplicationModal)
            Select Case RetVal
                Case vbAbort
                    Unload Me
                Case vbRetry
                    If LoadConfigData(strConfigFile) = False Then
                        MsgBox "A second attempt to load your configuration data failed. Aborting.", vbApplicationModal + vbCritical
                    End If
                Case vbIgnore
                    MsgBox "NOTICE: You have chosen to proceed after a data error, this application may" & vbCrLf & "not function properly or you may loose data."
            End Select
        End If
    End If
End Sub

Private Sub mnuFileSave_Click()
    If SaveConfigData(strConfigFile) = False Then
        MsgBox "Data was not saved, no idea why..."
    End If
End Sub

Private Sub mnuHelpAbout_Click()
    MsgBox "This is going to be an about box someday."
End Sub

Private Function SaveConfigData(strCurConfigFile As String) As Boolean
'<CSCM>
'--------------------------------------------------------------------------------
' Project    :       WinUI
' Procedure  :       SaveConfigData
' Description:       this is where we save the changes to the config data.
'
'                    returns true on sucess
'
'                    does nothing for now, i'll fix it later :-P
' Created by :       Adam
' Date-Time  :       8/25/2003-1:12:28 AM
' Parameters :       strCurConfigFile (String)
'--------------------------------------------------------------------------------
'</CSCM>

    MsgBox "If this were a latter version this would save the changes you've made." & vbCrLf & vbCrLf & "But... This isn't a later version and all this does is show this pretty box." & vbCrLf & vbCrLf & ":-P", vbOKOnly + vbInformation

    SaveConfigData = True
End Function

Private Sub sstConfig_DblClick()

End Sub
