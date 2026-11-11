#pragma once
#include <vector>
#include <algorithm>

// Shared CSV reader for FAI / Cosmetic. Never modifies CSV files or alarm data.
static void OCAPHistory_Split(const CString& sLine, std::vector<CString>& cells)
{
	cells.clear();
	CString sCell;
	bool bQuoted = false;
	for (int i=0; i<sLine.GetLength(); i++) {
		TCHAR ch = sLine[i];
		if (ch == '"') {
			if (bQuoted && i+1<sLine.GetLength() && sLine[i+1] == '"') { sCell += '"'; i++; }
			else bQuoted = !bQuoted;
		} else if (ch == ',' && !bQuoted) { cells.push_back(sCell); sCell.Empty(); }
		else sCell += ch;
	}
	cells.push_back(sCell);
}

struct OCAPHistory_Row
{
	CString sKey;
	std::vector<CString> cells;
};
static bool OCAPHistory_Newer(const OCAPHistory_Row& a, const OCAPHistory_Row& b)
{
	return a.sKey.Compare(b.sKey) > 0;
}

// Same rate highlighting as the original in-memory history display.
// These colors indicate threshold exceedance, not a saved alarm-event flag.
static COLORREF OCAPHistory_Color(int nTotal, int nCount, int nMinimum,
	double dIndividual, const double* pCommon, int nType)
{
	if (nTotal <= 0 || nTotal < nMinimum || nCount <= 0 || dIndividual == 0.0)
		return RGB(255, 255, 255);
	double dRate = nCount * 100.0 / nTotal;
	int nFirst = nType == 0 ? 2 : 3;
	int nLast = nType == 0 ? 0 : 3;
	for (int i=nFirst; i>=nLast; i--) {
		double dLimit = dIndividual == -1.0 ? pCommon[i] : dIndividual;
		if (dRate >= dLimit) {
			if (i >= 2) return RGB(255, 0, 0);
			if (i == 1) return RGB(0, 0, 255);
			return RGB(0, 255, 0);
		}
	}
	return RGB(255, 255, 255);
}

static void OCAPHistory_Display(CGridCS* pGrid, const CString& sFolder, CString* pCodes, const CString& sType, const double* pCodeDefect, const double* pCommon, int nMinimum)
{
	BOOL bCosmetic = sType == "Cosmetic";
	int nBase = bCosmetic ? 13 : 16;
	// Three calendar months, inclusive of the cutoff date; clamp month-end days.
	SYSTEMTIME now;
	GetLocalTime(&now);
	int nYear = now.wYear, nMonth = now.wMonth - 3;
	if (nMonth <= 0) { nMonth += 12; nYear--; }
	int nDays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if (nYear%4 == 0 && (nYear%100 != 0 || nYear%400 == 0)) nDays[1] = 29;
	int nDay = now.wDay < nDays[nMonth-1] ? now.wDay : nDays[nMonth-1];
	CString sCutoff, sToday;
	sCutoff.Format("%04d%02d%02d", nYear, nMonth, nDay);
	sToday.Format("%04d%02d%02d", now.wYear, now.wMonth, now.wDay);

	std::vector<OCAPHistory_Row> rows;
	CFileFind finder;
	BOOL bMore = finder.FindFile(sFolder + "\\*.csv");
	while (bMore) {
		bMore = finder.FindNextFile();
		if (finder.IsDirectory()) continue;
		CString sDate = finder.GetFileName().Left(8);
		BOOL bDate = sDate.GetLength() == 8;
		for (int k=0; k<sDate.GetLength(); k++) if (sDate[k] < '0' || sDate[k] > '9') bDate = FALSE;
		if (!bDate) continue;
		if (sDate.Compare(sCutoff) < 0 || sDate.Compare(sToday) > 0) continue;
		CStdioFile file;
		if (!file.Open(finder.GetFilePath(), CFile::modeRead | CFile::typeText | CFile::shareDenyNone)) continue;
		try {
			CString sLine;
			std::vector<CString> header, data;
			if (!file.ReadString(sLine)) { file.Close(); continue; }
			OCAPHistory_Split(sLine, header);
			if (header.size() < 20 || header[0] != "Time") { file.Close(); continue; }
			BOOL bCodeOnly = bCosmetic && finder.GetFileName().Find("OCAPCosmeticCodeV2_") >= 0;
			int nHeaderBase = bCodeOnly ? 20 : 23;
			int nMap[400];
			for (int j=0; j<400; j++) nMap[j] = -1;
			// Match the recorded code, not the current ordinal or editable name.
			for (int nType=0; nType<2; nType++) {
				CString sPrefix = nType == 0 ? sType + "-" : CString("ERR-");
				for (size_t j=nHeaderBase; j+1<header.size(); j+=2) {
					CString sTitle = header[j];
					if (bCodeOnly) {
						for (int k=0; k<100; k++) {
							if (pCodes[k].IsEmpty()) continue;
							CString sExpected = nType == 0 ? pCodes[k] : "ERR " + pCodes[k];
							if (sTitle == sExpected && ((j-nHeaderBase)/200) == nType) {
								nMap[nType*200+k*2] = (int)j;
								nMap[nType*200+k*2+1] = (int)j+1;
							}
						}
						continue;
					}
					if (sTitle.Left(sPrefix.GetLength()) != sPrefix) continue;
					int nSpace = sTitle.Find(' ');
					if (nSpace < 0) continue;
					CString sRecorded = sTitle.Mid(nSpace+1);
					// Prefer the longest matching code when one code prefixes another.
					int nMatch = -1, nLength = 0;
					for (int k=0; k<100; k++) {
						int nLen = pCodes[k].GetLength();
						if (nLen > nLength && sRecorded.Left(nLen+1) == pCodes[k] + " ") { nMatch = k; nLength = nLen; }
					}
					if (nMatch >= 0) {
						nMap[nType*200+nMatch*2] = (int)j;
						nMap[nType*200+nMatch*2+1] = (int)j+1;
					}
				}
			}
			while (file.ReadString(sLine)) {
				OCAPHistory_Split(sLine, data);
				// Ignore a partial final record while the logger is appending it.
				if (data.size() < header.size() || data[0].GetLength() < 8) continue;
				OCAPHistory_Row row;
				row.sKey = sDate + data[0];
				row.cells.resize(nBase+400);
				row.cells[0] = sDate.Left(4) + "/" + sDate.Mid(4,2) + "/" + sDate.Mid(6,2);
				row.cells[1] = data[0]; row.cells[2] = data[1];
				for (int j=3; j<nBase; j++) row.cells[j] = data[j+7];
				for (int j=0; j<400; j++)
					if (nMap[j] >= 0 && nMap[j] < (int)data.size()) row.cells[j+nBase] = data[nMap[j]];
				rows.push_back(row);
			}
			file.Close();
		} catch (CFileException* pEx) { pEx->Delete(); }
	}
	finder.Close();
	std::stable_sort(rows.begin(), rows.end(), OCAPHistory_Newer);
	// WM_SETREDRAW(TRUE) adds WS_VISIBLE even to a hidden window.
	// Only suspend redraw when the grid is already intended to be visible.
	BOOL bVisible = (pGrid->GetStyle() & WS_VISIBLE) != 0;
	if (bVisible) pGrid->SetRedraw(FALSE);
	// Reset the previous display so expired rows cannot remain visible.
	pGrid->Set_RowCount(1);
	int nRows = (int)rows.size()+1;
	if (nRows < 51) nRows = 51;
	pGrid->Set_RowCount(nRows);
	pGrid->Set_ColWidth(0, 100);
	pGrid->Set_ColWidth(1, 105);
	for (int i=1; i<nRows; i++) {
		pGrid->Set_RowHeight(i, 26);
		for (int j=0; j<nBase+400; j++) {
			CString sText;
			if (i <= (int)rows.size()) sText = rows[i-1].cells[j];
			pGrid->Set_CellText(i, j, sText);
			if (j >= nBase) {
				COLORREF crBack = RGB(255, 255, 255);
				if (i <= (int)rows.size()) {
					const OCAPHistory_Row& row = rows[i-1];
					int nCode = ((j-nBase) % 200) / 2;
					int nCountCol = nBase + ((j-nBase) / 2) * 2;
					if (!row.cells[nCountCol].IsEmpty() && !pCodes[nCode].IsEmpty())
						crBack = OCAPHistory_Color(_ttoi(row.cells[3]), _ttoi(row.cells[nCountCol]),
							nMinimum, pCodeDefect[nCode], pCommon, (j-nBase) / 200);
				}
				pGrid->Set_CellBackClr(i, j, crBack);
				// White text remains legible on the original blue highlight.
				pGrid->Set_CellTextClr(i, j, crBack == RGB(0, 0, 255) ? RGB(255, 255, 255) : RGB(0, 0, 0));
			}
		}
	}
	if (bVisible) {
		pGrid->SetRedraw(TRUE);
		pGrid->Invalidate(FALSE);
	}
}
