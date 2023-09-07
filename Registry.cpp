// Registry.cpp - Class to read and write registry entries
// Copyright (C) 1997 Rick A. Eichhorn.  All rights reserved.
//

#include "stdafx.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Registry
CRegistry::CRegistry(regType nType, LPCSTR sCompany, LPCSTR sProduct, LPCSTR sKey)
{
	// Return a key for either LOCAL_MACHINE or CURRENT_USER depending 
	// on regType.  If the key does not exist, create it if we have the
    // necessary access rights.
	HKEY hType;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	HKEY hProductKey = NULL;
    REGSAM userAccess = KEY_WRITE|KEY_READ;
    long nReturn;

	m_hAppKey = NULL;

	if (nType == Machine)
		hType = HKEY_LOCAL_MACHINE;
	else
		hType = HKEY_CURRENT_USER;

    nReturn = RegOpenKeyEx(hType, szSoftware, 0, userAccess, &hSoftKey);
    if (nReturn != ERROR_SUCCESS)
    {
        userAccess = KEY_READ;
        nReturn = RegOpenKeyEx(hType, szSoftware, 0, KEY_READ, &hSoftKey);
    }

	if (nReturn == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, sCompany, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, userAccess, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, sProduct, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, userAccess, NULL,
				&hProductKey, &dw);
		}
		if (hProductKey != NULL && sKey != NULL)
		{
			RegCreateKeyEx(hProductKey, sKey, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, userAccess, NULL,
				&m_hAppKey, &dw);
		}
        else
            m_hAppKey = hProductKey;
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);
	if (hProductKey != NULL && sKey != NULL)
		RegCloseKey(hProductKey);

	return;
}

CRegistry::~CRegistry()
{
	if (m_hAppKey != NULL)
		RegCloseKey(m_hAppKey);
}

BOOL CRegistry::WriteInt(LPCSTR sSection, LPCSTR sEntry, int nValue)
{
	// Write an entry to HKEY_LOCAL_MACHINE.  If the key does not
	// exist, create it.  Return TRUE if successful.
	HKEY hSectionKey = NULL;
	HKEY hEntryKey = NULL;
	long lResult = ERROR_SUCCESS + 1;

	DWORD dw;
	if (RegCreateKeyEx(m_hAppKey, sSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw) == ERROR_SUCCESS)
	{
		lResult = ::RegSetValueEx(hSectionKey, sEntry, 0, REG_DWORD,
			(BYTE*) &nValue, sizeof(nValue));
	}

	if (hSectionKey != NULL)
		::RegCloseKey(hSectionKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

BOOL CRegistry::WriteString(LPCSTR sSection, LPCSTR sEntry, LPCSTR sValue)
{
	// Write an entry to HKEY_LOCAL_MACHINE.  If the key does not
	// exist, create it.  Return TRUE if successful.
	HKEY hSectionKey = NULL;
	HKEY hEntryKey = NULL;
	long lResult = ERROR_SUCCESS + 1;

	DWORD dw;
	if (RegCreateKeyEx(m_hAppKey, sSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw) == ERROR_SUCCESS)
	{
		lResult = ::RegSetValueEx(hSectionKey, sEntry, 0, REG_SZ,
			(BYTE*) sValue, (DWORD) strlen(sValue));
	}

	if (hSectionKey != NULL)
		::RegCloseKey(hSectionKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

BOOL CRegistry::WriteBinary(LPCSTR sSection, LPCSTR sEntry, BYTE* sValue, int nLen)
{
	// Write an entry to HKEY_LOCAL_MACHINE.  If the key does not
	// exist, create it.  Return TRUE if successful.
	HKEY hSectionKey = NULL;
	HKEY hEntryKey = NULL;
	long lResult = ERROR_SUCCESS + 1;

	DWORD dw;
	if (RegCreateKeyEx(m_hAppKey, sSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw) == ERROR_SUCCESS)
	{
		lResult = ::RegSetValueEx(hSectionKey, sEntry, 0, REG_BINARY,
			(BYTE*) sValue, nLen);
	}

	if (hSectionKey != NULL)
		::RegCloseKey(hSectionKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

UINT CRegistry::GetInt(LPCSTR sSection, LPCSTR sEntry, int nDefault)
{
	// Read an entry from HKEY_LOCAL_MACHINE.
	HKEY hStartKey;
	int nReturn;
	int nValue;
	DWORD lSize = sizeof(nValue);
	long lError;
    
	lError = ::RegOpenKeyEx(m_hAppKey, sSection, 0,
		KEY_READ, &hStartKey);
	if (lError == ERROR_SUCCESS)
		lError = ::RegQueryValueEx(hStartKey, sEntry, 
			NULL, NULL, (BYTE*) &nValue, &lSize);
	if (hStartKey != NULL)
		::RegCloseKey(hStartKey);

	if (lError == ERROR_SUCCESS)
		nReturn = (int) nValue;
	else
		nReturn = nDefault;

	return nReturn;
}

CString CRegistry::GetString(LPCSTR sSection, LPCSTR sEntry, LPCSTR sDefault)
{
	// Read an entry from HKEY_LOCAL_MACHINE. 
	CString sReturn;
	HKEY hStartKey = NULL;
	BYTE sValue[150];
	DWORD lSize = 150;
	long lError;
    
	lError = ::RegOpenKeyEx(m_hAppKey, sSection, 0,
		KEY_READ, &hStartKey);
	if (lError == ERROR_SUCCESS)
		lError = ::RegQueryValueEx(hStartKey, sEntry, 
			NULL, NULL, sValue, &lSize);
	if (hStartKey != NULL)
		::RegCloseKey(hStartKey);

	if (lError == ERROR_SUCCESS)
		sReturn = sValue;
	else
		sReturn = sDefault;

	return sReturn;
}

int CRegistry::GetBinary(LPCSTR sSection, LPCSTR sEntry, BYTE* sValue, int nMax)
{
	// Read an entry from HKEY_LOCAL_MACHINE. 
    int nReturn;
    DWORD lSize = nMax;
	HKEY hStartKey = NULL;
	long lError;
    
	lError = ::RegOpenKeyEx(m_hAppKey, sSection, 0,
		KEY_READ, &hStartKey);
	if (lError == ERROR_SUCCESS)
		lError = ::RegQueryValueEx(hStartKey, sEntry, 
			NULL, NULL, sValue, &lSize);
	if (hStartKey != NULL)
		::RegCloseKey(hStartKey);

	if (lError == ERROR_SUCCESS)
		nReturn = (int) lSize;
	else
		nReturn = -1;

	return nReturn;
}
