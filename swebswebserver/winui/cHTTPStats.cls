VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "cHTTPStats"
Attribute VB_GlobalNameSpace = True
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
'CSEH: WinUI - Custom
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

'local variable(s) to hold property value(s)
Private mvarLastRestart As Date
Private mvarRequestCount As Long
Private mvarTotalBytesSent As Double
Private mvarFile As String

Public Sub Setup()
    '<EhHeader>
    On Error GoTo Setup_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.Setup")
    '</EhHeader>
100     mvarFile = WinUI.Util.GetRegistryString(&H80000002, "SOFTWARE\SWS", "StatsFile")
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Sub

Setup_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.Setup", Erl, False
    Resume Next
    '</EhFooter>
End Sub

Public Property Get File() As String
    '<EhHeader>
    On Error GoTo File_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.File")
    '</EhHeader>
100     File = mvarFile
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Property

File_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.File", Erl, False
    Resume Next
    '</EhFooter>
End Property

Public Sub Reload()
    '<EhHeader>
    On Error GoTo Reload_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.Reload")
    '</EhHeader>
    Dim XML As CHILKATXMLLib.XmlFactory
    Dim StatsXML As CHILKATXMLLib.IChilkatXml
    Dim Node As CHILKATXMLLib.IChilkatXml
    
100     Set XML = New XmlFactory
104     Set StatsXML = XML.NewXml
108     If Dir$(WinUI.Server.HTTP.Stats.File) <> "" Then
112         WinUI.EventLog.AddEvent "SWEBS_WinUI_DLL.cStats.Reload", "Loading Stats File: " & mvarFile
116         StatsXML.LoadXmlFile mvarFile
        Else
120         WinUI.EventLog.AddEvent "SWEBS_WinUI_DLL.cStats.Reload", "Stats File not found."
            Exit Sub
        End If
    
        '<TotalBytesSent>
124     Set Node = StatsXML.SearchForTag(Nothing, "TotalBytesSent")
128     If Node Is Nothing Then
132         mvarTotalBytesSent = 0
        Else
136         mvarTotalBytesSent = Node.Content
        End If
    
        '<LastRestart>
140     Set Node = StatsXML.SearchForTag(Nothing, "LastRestart")
144     If Node Is Nothing Then
148         mvarLastRestart = CDate(Now)
        Else
152         mvarLastRestart = CDate(Node.Content)
        End If
    
        '<RequestCount>
156     Set Node = StatsXML.SearchForTag(Nothing, "RequestCount")
160     If Node Is Nothing Then
164         mvarRequestCount = 0
        Else
168         mvarRequestCount = Val(Node.Content)
        End If
    
        'clean up
172     Set XML = Nothing
176     Set StatsXML = Nothing
180     Set Node = Nothing
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Sub

Reload_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.Reload", Erl, False
    Resume Next
    '</EhFooter>
End Sub

Public Property Get TotalBytesSent() As Double
    '<EhHeader>
    On Error GoTo TotalBytesSent_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.TotalBytesSent")
    '</EhHeader>
100     TotalBytesSent = mvarTotalBytesSent
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Property

TotalBytesSent_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.TotalBytesSent", Erl, False
    Resume Next
    '</EhFooter>
End Property

Public Property Get RequestCount() As Long
    '<EhHeader>
    On Error GoTo RequestCount_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.RequestCount")
    '</EhHeader>
100     RequestCount = mvarRequestCount
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Property

RequestCount_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.RequestCount", Erl, False
    Resume Next
    '</EhFooter>
End Property

Public Property Get LastRestart() As Date
    '<EhHeader>
    On Error GoTo LastRestart_Err
    WinUI.Debuger.CallStack.Push ("SWEBS_WinUI.cHTTPStats.LastRestart")
    '</EhHeader>
100     LastRestart = mvarLastRestart
    '<EhFooter>
    WinUI.Debuger.CallStack.Pop
    Exit Property

LastRestart_Err:
    DisplayErrMsg Err.Description, "SWEBS_WinUI.cHTTPStats.LastRestart", Erl, False
    Resume Next
    '</EhFooter>
End Property


