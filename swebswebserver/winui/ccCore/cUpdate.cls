VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "cUpdate"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
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
Private mvarIsAvailable As Boolean 'local copy
Private mvarVersion As String 'local copy
Private mvarReleaseDate As String 'local copy
Private mvarInfoURL As String 'local copy
Private mvarDownloadURL As String 'local copy
Private mvarDescription As String 'local copy
Private mvarUpdateLevel As String 'local copy
Private mvarFileSize As Long 'local copy

Public Sub Check()
Dim strData As String
Dim strNewVer() As String
Dim strCurVer() As String
Dim i As Long

    If gCore.Debuger.DisableUpdate <> True Then
        If gCore.Net.IsOnline = True Then
            strData = Replace(gCore.Net.PageSource("http://swebs.sf.net/upgrade.xml"), vbLf, vbCrLf)
        End If
        If InStr(1, strData, "Server at swebs.sourceforge.net Port 80") = 0 And strData <> "" Then
            gCore.EventLog.AddEvent "SWEBS_WinUI_DLL.cUpdate.Check", "Update Data Found, Processing."
            mvarReleaseDate = Util.GetTaggedData(strData, "Date")
            mvarDescription = Util.GetTaggedData(strData, "Description")
            mvarDownloadURL = Util.GetTaggedData(strData, "DownloadURL")
            mvarInfoURL = Util.GetTaggedData(strData, "InfoURL")
            mvarVersion = Util.GetTaggedData(strData, "Version")
            mvarUpdateLevel = Util.GetTaggedData(strData, "UpgradeLevel")
            mvarFileSize = Val(Util.GetTaggedData(strData, "FileSize"))
            
            'check to see if this is newer
            'this is a bad way to do things, needs to be fixed, again
            strNewVer() = Split(mvarVersion, ".")
            ReDim Preserve strNewVer(0 To 6)
            strCurVer() = Split(gCore.Version, ".")
            ReDim Preserve strCurVer(0 To 6)
            For i = 0 To UBound(strNewVer)
                Debug.Print "New: " & Val(strNewVer(i)) & " Old: " & Val(strCurVer(i))
                If Val(strNewVer(i)) > Val(strCurVer(i)) Then
                    mvarIsAvailable = True
                    gCore.EventLog.AddEvent "SWEBS_WinUI_DLL.cUpdate.Check", "Update Available. Old Version: " & gCore.Version & "; New Version: " & gCore.Update.Version
                    Exit For
                ElseIf Val(strNewVer(i)) < Val(strCurVer(i)) Then
                    Exit For
                End If
            Next
        ElseIf mvarIsAvailable = True Then
            gCore.EventLog.AddEvent "SWEBS_WinUI_DLL.cUpdate.Check", "Update status already true."
        Else
            mvarIsAvailable = False
            gCore.EventLog.AddEvent "SWEBS_WinUI_DLL.cUpdate.Check", "No update data or update file not found."
        End If
    End If
End Sub

Public Property Get FileSize() As Long
    FileSize = mvarFileSize
End Property

Public Property Get UpdateLevel() As String
    UpdateLevel = mvarUpdateLevel
End Property

Public Property Get Description() As String
    Description = mvarDescription
End Property

Public Property Get DownloadURL() As String
    DownloadURL = mvarDownloadURL
End Property

Public Property Get InfoURL() As String
    InfoURL = mvarInfoURL
End Property

Public Property Get ReleaseDate() As String
    ReleaseDate = mvarReleaseDate
End Property

Public Property Get Version() As String
    Version = mvarVersion
End Property

Public Property Get IsAvailable() As Boolean
    IsAvailable = mvarIsAvailable
End Property



