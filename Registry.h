// Registry.h : header file
// Copyright (C) 1997 Rick A. Eichhorn

#ifndef __REGISTRY_H__
#define __REGISTRY_H__

/////////////////////////////////////////////////////////////////////////////
// CRegistry

static const TCHAR szSoftware[] = _T("Software");

class CRegistry {
private:
	HKEY m_hAppKey;

public:
	enum regType {
		Machine,
		User
	};
public:
	CRegistry(regType nType, LPCSTR sCompany, LPCSTR sProduct, LPCSTR sKey = NULL);
	~CRegistry();
	BOOL WriteInt(LPCSTR sSection, LPCSTR sEntry, int nValue);
	BOOL WriteString(LPCSTR sSection, LPCSTR sEntry, LPCSTR sValue);
    BOOL WriteBinary(LPCSTR sSection, LPCSTR sEntry, BYTE* sValue, int nLen);
	UINT GetInt(LPCSTR sSection, LPCSTR sEntry, int nDefault);
	CString GetString(LPCSTR sSection, LPCSTR sEntry, LPCSTR sDefault = NULL);
    int GetBinary(LPCSTR sSection, LPCSTR sEntry, BYTE* sValue, int nMax);

};
#endif  // __REGISTRY_H