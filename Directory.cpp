#include "stdafx.h"
#include "Directory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// CDirectory Class
//////////////////////////////////////////////////////////////////////
// Pretty much the same as CFileFind, but gives us more control

CDirectory::CDirectory(LPCTSTR sPath)
{
	m_hDirectory = NULL;
	m_sPath = sPath;
	m_bDirectory = FALSE;
	m_nMinSize = 0;
	m_nMaxSize = 0;
	m_sMask = "*";
}

CDirectory::~CDirectory()
{
	if (m_hDirectory != NULL)
		FindClose(m_hDirectory);
}

void CDirectory::SetFileMask(LPCTSTR sMask)
{
	// A file mask can contain several extensions separated by
	// spaces or semicolons. Example: '*.jpg;*.gif;*.bmp'
	m_sMask = sMask;

}

int CDirectory::GetCount(LPCTSTR sPattern, BOOL bDir)
{
	CString sName;
	int nCount = 0;
	FirstFile(sPattern, bDir);
	while (NextMatch(sName))
		nCount++;

	return nCount;

}

BOOL CDirectory::SetFirst(CString &sFileName)
{
	// Lower level function to iterate through a directory.
	// Call this function before calling NextFile.
	// You may want to use FirstFile() instead.
	BOOL bInd;
	CString sFilter;

	if (m_hDirectory != NULL)
		FindClose(m_hDirectory);

	sFilter = m_sPath + "\\" + m_sMask;
	m_hDirectory = FindFirstFile(sFilter, &m_FileData);
	if (m_hDirectory != INVALID_HANDLE_VALUE) 
	{
		sFileName = m_FileData.cFileName;
		bInd = TRUE;
	}
	else
		bInd = FALSE;

	return bInd;

}

BOOL CDirectory::NextFile(CString &sFileName)
{
	BOOL bInd;
	
	bInd = FindNextFile(m_hDirectory, &m_FileData);
	if (bInd)
		sFileName = m_FileData.cFileName;

	return bInd;

}

void CDirectory::LimitSize(unsigned int nMin, unsigned int nMax)
{
	m_nMinSize = nMin;
	m_nMaxSize = nMax;
}

void CDirectory::FirstFile(LPCTSTR sPattern, BOOL bDirectory)
{
	// Call this function before calling NextMatch
	m_sPattern = sPattern;
	m_bDirectory = bDirectory;
	m_bFirst = SetNextMask(m_sMask);

}

BOOL CDirectory::NextMatch(CString& sFileName)
{
	CString sName;
	BOOL bInd;
	BOOL bDirectory;
	BOOL bDone = FALSE;

	while (!bDone)
	{
		if (m_bFirst)
		{
			bInd = SetFirst(sName);
			m_bFirst = FALSE;
		}
		else
			bInd = NextFile(sName);

		if (bInd)
		{
			// Test if the file is a directory. If m_bDirectory is TRUE, we
			// want files that are directories.
			if (m_FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				CString sName = m_FileData.cFileName;
				if (sName != "." && sName != "..")
					bDirectory = TRUE;   // Ignore these two directories
				else
					bDirectory = FALSE;
			}
			else
				bDirectory = FALSE;
			if (m_bDirectory == bDirectory)
				bDone = TRUE;
			
			// Filter out system files and hidden files
			if (m_FileData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM)
				bDone = FALSE;
			if (m_FileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)
				bDone = FALSE;

			// If we are not looking for directories, filter on size too
			if (!m_bDirectory)
			{
				if (m_nMaxSize > 0)
					if (m_FileData.nFileSizeLow > m_nMaxSize && m_FileData.nFileSizeHigh == 0)
						bDone = FALSE;
				if (m_nMinSize > 0)
					if (m_FileData.nFileSizeLow < m_nMinSize && m_FileData.nFileSizeHigh == 0)
						bDone = FALSE;
			}

			if (bDone)
				sFileName = sName;
		}
		else
		{
			// SetNextMask returns FALSE if it fails
			 m_bFirst = SetNextMask(m_sMask);
			 bDone = !m_bFirst;
		}
	}

	return bInd && bDone;
}

BOOL CDirectory::SetNextMask(CString& sMask)
{
	// Parse a pattern into masks. A pattern is a collection of masks separated
	// by ';' or ' '. Do not use more than one separator between masks.
	BOOL bInd;

	int nIndex = m_sPattern.FindOneOf("; ");

	if (nIndex > 0)
	{
		sMask = m_sPattern.Left(nIndex);
		m_sPattern = m_sPattern.Right(m_sPattern.GetLength() - nIndex - 1);
	}
	else
	{
		sMask = m_sPattern;
		m_sPattern = "";
	}

	if (sMask.GetLength() > 0)
		bInd = TRUE;
	else
		bInd = FALSE;

	return bInd;
}
