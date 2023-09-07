// Utils.h : Assorted useful functions
// Copyright (C) 2004 Rick A. Eichhorn

#pragma once

CString PathName(LPCTSTR sFullName);
CString FullName(LPCTSTR sDir, LPCTSTR sName);
CString FileName(LPCTSTR sFullName);
BOOL MkDir(CString sPath);

int GenMod(LPCSTR sKey);
int GetKey(CString* sUser, CString* sCompany, CString* sKey);
bool LeaveDropping(LPCSTR sAppName, bool bValid = true);
bool GetDropping(LPCSTR sAppName, COleDateTime* pDate);
