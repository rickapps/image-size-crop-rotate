#if !defined DIRECTORY_H
#define DIRECTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectory  
{
public:
	void LimitSize(unsigned nMin, unsigned nMax);
	BOOL NextFile(CString& sFileName);
	BOOL SetFirst(CString& sFileName);
	void FirstFile(LPCTSTR sPattern, BOOL bDirectory = FALSE);
	BOOL NextMatch(CString& sFileName);
	int GetCount(LPCTSTR sPattern, BOOL bDirectory = FALSE);
	void SetFileMask(LPCTSTR sMask);
	CDirectory(LPCTSTR sPath = "/");
	virtual ~CDirectory();

private:
	BOOL SetNextMask(CString& sMask);

private:
	CString m_sPath;
	CString m_sMask;
	CString m_sPattern;
	BOOL m_bDirectory;
	WIN32_FIND_DATA m_FileData;
	HANDLE m_hDirectory;
	BOOL m_bFirst;          // Used by NextFile;
	unsigned m_nMinSize;
	unsigned m_nMaxSize;
};

#endif 

