// Utils.cpp : Assorted useful functions
// Copyright (C) 2004 Rick A. Eichhorn

#include "stdafx.h"
#include <direct.h>   //For mkdir function
#include <errno.h>    //For mkdir function
#include "registry.h"
#include "utils.h"

CString FullName(LPCTSTR sDir, LPCTSTR sName)
{
	// Combine sName with sDir to generate a full file name
	CString sFullName;

	// Check the directory to see if it ends with a slash
	CString oDir = sDir;
	int nLoc = oDir.ReverseFind('\\') + 1;
	if (nLoc != oDir.GetLength())
		oDir = oDir + '\\';

	sFullName = oDir + sName;

	return sFullName;
}

CString PathName(LPCTSTR sFullName)
{
	// Retrieve the directory of the given file or dir
	CString sPathName;

	CString oFullName = sFullName;
	int nLoc = oFullName.ReverseFind('\\');
	if (nLoc < 0)
	{
		nLoc = oFullName.ReverseFind(':');
		// If the colon is the last char, ignore it
		if (nLoc == oFullName.GetLength() - 1)
			nLoc = -1;
	}

	if (nLoc < 0)
		sPathName = "";
	else
		sPathName = oFullName.Left(nLoc);

	return sPathName;
}

CString FileName(LPCTSTR sFullName)
{
	// Retrieve the file name of the given file or dir
	CString sFileName;

	CString oFullName = sFullName;
	int nLoc = oFullName.ReverseFind('\\');
	if (nLoc < 0)
		nLoc = oFullName.ReverseFind(':');

	if (nLoc < 0)
		sFileName = sFullName;
	else
		sFileName = oFullName.Right(oFullName.GetLength() - nLoc - 1);

	return sFileName;
}

BOOL MkDir(CString sPath)
{
	// Create the directory named by sPath.  Return TRUE
	// if the directory name is valid. This function hangs if passed the root directory
	BOOL bInd;
    CString sSubPath;

	if (sPath == "")
		return FALSE;

    // If we have a root directory, just say everything is ok
    // Note - you better not pass a non-root path that ends with a slash
    sSubPath = FileName(sPath);
    if (sSubPath.GetLength() <= 0)
        return TRUE;

    // If the directory exists this returns true. It messes up on the root directory
    // The root directory is not a directory, it is a volume.
	if (_mkdir((LPCTSTR) sPath) == 0)
		return TRUE;

	int nErr = errno;
	if (nErr == EEXIST)
		return TRUE;

	// The directory could not be created.
	// Try moving down the tree
	sSubPath = PathName(sPath);
	bInd = MkDir(sSubPath);
	if (bInd)
		bInd = MkDir(sPath);

	return bInd;
}

// Verify that the key has not been mistyped
int GenMod(LPCSTR sKey)
{
    // Checks 3,7,12,16,21,25,30,34,36
    int x = 0;         // Position
    int xx = 0;        // Sum for inside chars
    int xy = 0;        // Sum for last char
    int x1 = 0;        // Check value - should be nine
    char yx[100];      // Substring to check
    strcpy_s(yx, sKey);
    // Length of our substring
    x = (int) (strchr(sKey, '-') - sKey + 1);
    while (x > 0) {
        xx = yx[0];
        if (x > 1)
            xx+=yx[1];
        if (x > 2)
            xx+=yx[2];
        if (x > 3)
            if (xx%26+65==yx[3])
                    x1++;
        xy+=xx;
        if (x > 7)
        {
            xx=yx[4]+yx[5]+yx[6];
            if (xx%26+65==yx[7])
                x1++;
        }
        xy+=xx;
        // Get next substring
        strcpy_s(yx, &yx[x]);
        // Get substring length
        x = (int) (strchr(yx, '-') - yx + 1);
    }
    if (xy%26+65==yx[0])
        x1++;
    return x1;
}

int GetKey(CString* sUser, CString* sCompany, CString* sKey)
{
    BYTE sConvert[150];
    LPSTR sBuffer;
    char sName[MAX_COMPUTERNAME_LENGTH+1];
    DWORD nSize = MAX_COMPUTERNAME_LENGTH+1;;
    if (::GetComputerName(sName, &nSize) == 0)
    {
        strcpy_s(sName, "Unknown");
        nSize = 7;
    }

    // Read our keys
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    *sUser = oReg.GetString("License", "User");
    *sCompany = oReg.GetString("License", "Company");
    int nLen = oReg.GetBinary("License", "Key", sConvert, 150);

    // Transform our key
    sBuffer = sKey->GetBuffer(nLen+1);
    for (int i = 0; i<nLen; i++)
        sBuffer[i] = (sConvert[i] ^ sName[i%nSize]);
    sBuffer[nLen] = '\0';
    sKey->ReleaseBuffer();

    return nLen;
}

// Leave last run time marking. If bValid is false,
// corrupt the file.
bool LeaveDropping(LPCSTR sAppName, bool bValid)
{
    CString szPath;
    CString sFile;
    LPSTR sBuffer;
    FILETIME ft;
    SYSTEMTIME st;
    CFile oFile;
    bool bInd = false;
    // Try for the common app data folder. Not supported by all Windows versions
    try {
    bInd = SUCCEEDED(SHGetFolderPath(NULL, 
                                CSIDL_COMMON_APPDATA, 
                                NULL, 
                                SHGFP_TYPE_DEFAULT, 
                                szPath.GetBuffer(MAX_PATH)));
    }
    catch (CNotSupportedException* e)
    {
        e->Delete();
    }
    szPath.ReleaseBuffer();

    try {
    if (szPath.GetLength() <= 0)
    {
        sBuffer = szPath.GetBuffer(MAX_PATH);
        bInd = SUCCEEDED(SHGetFolderPath(NULL, 
                                    CSIDL_WINDOWS, 
                                    NULL, 
                                    SHGFP_TYPE_CURRENT, 
                                    sBuffer));
        szPath.ReleaseBuffer();
        if (bInd)
            szPath = FullName(szPath, "Application Data");
    }
    }
    catch (CNotSupportedException* e)
    {
        szPath.ReleaseBuffer();
        e->Delete();
    }
    if (szPath.GetLength() <= 0)
        szPath = "C:\\Windows\\Application Data";

    if (bInd)
    {
        szPath = FullName(szPath, sAppName);
        sFile = FullName(szPath, "License.reg");
        bInd = (oFile.Open(sFile, CFile::modeCreate   
                               | CFile::modeWrite| CFile::shareExclusive) == TRUE);
    }
    if (bInd)
    {
        ::GetSystemTime(&st);
        ::SystemTimeToFileTime(&st, &ft);
        if (!bValid)
        {
            ft.dwHighDateTime = -1;
            ft.dwLowDateTime = 0;
        }
        char sBuff[80];
        sprintf_s(sBuff, "%d\t%d\0", ft.dwHighDateTime, ft.dwLowDateTime);
        oFile.Write(sBuff, (UINT) strlen(sBuff));
        oFile.Close();
    }
    return bInd;
}

bool GetDropping(LPCSTR sAppName, COleDateTime* pDate)
{
    CString szPath;
    CString sFile;
    LPSTR sBuffer;
    FILETIME ft;
    CFile oFile;
    bool bInd = false;
    // Try for the common app data folder. Not supported by all Windows versions
    try {
    bInd = SUCCEEDED(SHGetFolderPath(NULL, 
                                CSIDL_COMMON_APPDATA, 
                                NULL, 
                                SHGFP_TYPE_DEFAULT, 
                                szPath.GetBuffer(MAX_PATH)));
    }
    catch (CNotSupportedException* e)
    {
        e->Delete();
    }
    szPath.ReleaseBuffer();

    try {
    if (szPath.GetLength() <= 0)
    {
        sBuffer = szPath.GetBuffer(MAX_PATH);
        bInd = SUCCEEDED(SHGetFolderPath(NULL, 
                                    CSIDL_WINDOWS, 
                                    NULL, 
                                    SHGFP_TYPE_CURRENT, 
                                    sBuffer));
        szPath.ReleaseBuffer();
        if (bInd)
            szPath = FullName(szPath, "Application Data");
    }
    }
    catch (CNotSupportedException* e)
    {
        szPath.ReleaseBuffer();
        e->Delete();
    }
    if (szPath.GetLength() <= 0)
        szPath = "C:\\Windows\\Application Data";

    if (bInd)
    {
        szPath = FullName(szPath, sAppName);
        sFile = FullName(szPath, "License.reg");
        bInd = (oFile.Open(sFile, CFile::modeRead | CFile::shareExclusive) == TRUE);
    }

    if (bInd)
    {
        char buff[80];
        if (oFile.Read(buff, 80) <= 0)
            bInd = false;
        oFile.Close();
        if (sscanf_s(buff, "%d\t%d", &ft.dwHighDateTime, &ft.dwLowDateTime) != 2)
            bInd = false;
    }
    if (bInd)
    {
        COleDateTime oTime(ft);
        *pDate = oTime;
    }

    return bInd;
}
