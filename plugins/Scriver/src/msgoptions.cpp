/*
Scriver

Copyright (c) 2000-12 Miranda ICQ/IM project,

all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "stdafx.h"

INT_PTR CALLBACK DlgProcOptions1(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcOptions2(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcOptionsPopup(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define FONTF_BOLD   1
#define FONTF_ITALIC 2

struct
{
	const wchar_t *szDescr;
	COLORREF defColour;
	const wchar_t *szDefFace;
	BYTE defStyle;
	char defSize;
	const wchar_t *szBkgName;
}
static const fontOptionsList[] =
{
	{ LPGENW("Outgoing messages"), RGB(106, 106, 106), L"Arial", 0, -12, LPGENW("Outgoing background")},
	{ LPGENW("Incoming messages"), RGB(0, 0, 0), L"Arial", 0, -12, LPGENW("Incoming background")},
	{ LPGENW("Outgoing name"), RGB(89, 89, 89), L"Arial", FONTF_BOLD, -12, LPGENW("Outgoing background")},
	{ LPGENW("Outgoing time"), RGB(0, 0, 0), L"Terminal", FONTF_BOLD, -9, LPGENW("Outgoing background")},
	{ LPGENW("Outgoing colon"), RGB(89, 89, 89), L"Arial", 0, -11, LPGENW("Outgoing background")},
	{ LPGENW("Incoming name"), RGB(215, 0, 0), L"Arial", FONTF_BOLD, -12, LPGENW("Incoming background")},
	{ LPGENW("Incoming time"), RGB(0, 0, 0), L"Terminal", FONTF_BOLD, -9, LPGENW("Incoming background")},
	{ LPGENW("Incoming colon"), RGB(215, 0, 0), L"Arial", 0, -11, LPGENW("Incoming background")},
	{ LPGENW("Message area"), RGB(0, 0, 0), L"Arial", 0, -12, LPGENW("Input area background")},
	{ LPGENW("Notices"), RGB(90, 90, 160), L"Arial", 0, -12, LPGENW("Incoming background")},
	{ LPGENW("Unused"), RGB(0, 0, 255), L"Arial", 0, -12, LPGENW("Outgoing background")},
	{ LPGENW("Unused"), RGB(0, 0, 255), L"Arial", 0, -12, LPGENW("Incoming background")},
	{ LPGENW("Info bar contact name"), RGB(0, 0, 0), L"Arial", FONTF_BOLD, -19, LPGENW("Info bar background")},
	{ LPGENW("Info bar status message"), RGB(50, 50, 50), L"Arial", FONTF_ITALIC, -11, LPGENW("Info bar background")}
};

int fontOptionsListSize = _countof(fontOptionsList);

struct
{
	const wchar_t *szName;
	const char *szSettingName;
	COLORREF defColour;
	int systemColor;
}
static const colourOptionsList[] =
{
	{ LPGENW("Background"), SRMSGSET_BKGCOLOUR, 0, COLOR_WINDOW},
	{ LPGENW("Input area background"), SRMSGSET_INPUTBKGCOLOUR, 0, COLOR_WINDOW},
	{ LPGENW("Incoming background"), SRMSGSET_INCOMINGBKGCOLOUR, 0, COLOR_WINDOW},
	{ LPGENW("Outgoing background"), SRMSGSET_OUTGOINGBKGCOLOUR, 0, COLOR_WINDOW},
	{ LPGENW("Info bar background"), SRMSGSET_INFOBARBKGCOLOUR, 0, COLOR_3DLIGHT},
	{ LPGENW("Line between messages"), SRMSGSET_LINECOLOUR, 0, COLOR_3DLIGHT},
};

int FontServiceFontsChanged(WPARAM, LPARAM)
{
	LoadMsgLogIcons();
	LoadInfobarFonts();
	Srmm_Broadcast(DM_OPTIONSAPPLIED, 0, 0);
	return 0;
}

void RegisterFontServiceFonts()
{
	FontIDW fid = {};
	wcsncpy_s(fid.group, LPGENW("Messaging"), _TRUNCATE);
	wcsncpy_s(fid.backgroundGroup, LPGENW("Messaging"), _TRUNCATE);
	strncpy(fid.dbSettingsGroup, SRMM_MODULE, _countof(fid.dbSettingsGroup));
	fid.flags = FIDF_DEFAULTVALID | FIDF_DEFAULTVALID;
	for (int i = 0; i < _countof(fontOptionsList); i++) {
		fid.order = i;

		char szTemp[100];
		mir_snprintf(szTemp, "SRMFont%d", i);
		strncpy(fid.setting, szTemp, _countof(fid.setting));
		wcsncpy(fid.name, fontOptionsList[i].szDescr, _countof(fid.name));
		fid.deffontsettings.colour = fontOptionsList[i].defColour;
		fid.deffontsettings.size = fontOptionsList[i].defSize;
		fid.deffontsettings.style = fontOptionsList[i].defStyle;
		fid.deffontsettings.charset = DEFAULT_CHARSET;
		wcsncpy(fid.deffontsettings.szFace, fontOptionsList[i].szDefFace, _countof(fid.deffontsettings.szFace));
		wcsncpy(fid.backgroundName, fontOptionsList[i].szBkgName, _countof(fid.backgroundName));
		g_plugin.addFont(&fid);
	}

	ColourIDW cid = {};
	wcsncpy_s(cid.group, LPGENW("Messaging"), _TRUNCATE);
	strncpy(cid.dbSettingsGroup, SRMM_MODULE, _countof(fid.dbSettingsGroup));
	cid.flags = 0;
	for (int i = 0; i < _countof(colourOptionsList); i++) {
		cid.order = i;
		wcsncpy(cid.name, colourOptionsList[i].szName, _countof(cid.name));
		if (colourOptionsList[i].systemColor != -1)
			cid.defcolour = GetSysColor(colourOptionsList[i].systemColor);
		else
			cid.defcolour = colourOptionsList[i].defColour;

		strncpy(cid.setting, colourOptionsList[i].szSettingName, _countof(cid.setting));
		g_plugin.addColor(&cid);
	}
}

int IconsChanged(WPARAM, LPARAM)
{
	ReleaseIcons();
	LoadGlobalIcons();
	FreeMsgLogIcons();
	LoadMsgLogIcons();
	Srmm_Broadcast(DM_REMAKELOG, 0, 0);
	Srmm_Broadcast(DM_CHANGEICONS, 0, 1);
	return 0;
}

int SmileySettingsChanged(WPARAM wParam, LPARAM)
{
	Srmm_Broadcast(DM_REMAKELOG, wParam, 0);
	return 0;
}

void LoadMsgDlgFont(int i, LOGFONT *lf, COLORREF *colour)
{
	char str[32];

	if (colour) {
		mir_snprintf(str, "%s%dCol", "SRMFont", i);
		*colour = g_plugin.getDword(str, fontOptionsList[i].defColour);
	}

	if (lf) {
		mir_snprintf(str, "%s%dSize", "SRMFont", i);
		lf->lfHeight = (char)g_plugin.getByte(str, fontOptionsList[i].defSize);
		lf->lfWidth = 0;
		lf->lfEscapement = 0;
		lf->lfOrientation = 0;
		mir_snprintf(str, "%s%dSty", "SRMFont", i);
		int style = g_plugin.getByte(str, fontOptionsList[i].defStyle);
		lf->lfWeight = style & FONTF_BOLD ? FW_BOLD : FW_NORMAL;
		lf->lfItalic = style & FONTF_ITALIC ? 1 : 0;
		lf->lfUnderline = 0;
		lf->lfStrikeOut = 0;
		lf->lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf->lfQuality = DEFAULT_QUALITY;
		lf->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		mir_snprintf(str, "%s%d", "SRMFont", i);

		ptrW tszFace(g_plugin.getWStringA(str));
		if (tszFace == nullptr)
			mir_wstrcpy(lf->lfFaceName, fontOptionsList[i].szDefFace);
		else
			wcsncpy(lf->lfFaceName, tszFace, _countof(lf->lfFaceName));

		mir_snprintf(str, "%s%dSet", "SRMFont", i);
		lf->lfCharSet = g_plugin.getByte(str, DEFAULT_CHARSET);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

class CBaseOptionDlg : public CDlgBase
{
	void ApplyOptions(void*)
	{
		ReloadGlobals();
		WindowList_Broadcast(g_dat.hParentWindowList, DM_OPTIONSAPPLIED, 0, 0);
		Srmm_Broadcast(DM_OPTIONSAPPLIED, 0, 0);
		Chat_UpdateOptions();
	}

public:
	CBaseOptionDlg(int dlgId) :
		CDlgBase(g_plugin, dlgId)
	{
		m_OnFinishWizard = Callback(this, &CBaseOptionDlg::ApplyOptions);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

struct CheckBoxValues_t
{
	DWORD style;
	const wchar_t *szDescr;
};

static const struct CheckBoxValues_t statusValues[] =
{
	{ MODEF_OFFLINE, LPGENW("Offline") },
	{ PF2_ONLINE, LPGENW("Online") },
	{ PF2_SHORTAWAY, LPGENW("Away") },
	{ PF2_LONGAWAY, LPGENW("Not available") },
	{ PF2_LIGHTDND, LPGENW("Occupied") },
	{ PF2_HEAVYDND, LPGENW("Do not disturb") },
	{ PF2_FREECHAT, LPGENW("Free for chat") },
	{ PF2_INVISIBLE, LPGENW("Invisible") }
};

class CMainOptionsDlg : public CBaseOptionDlg
{
	void FillCheckBoxTree(const struct CheckBoxValues_t *values, int nValues, DWORD style)
	{
		TVINSERTSTRUCT tvis;
		tvis.hParent = nullptr;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_STATE;
		for (int i = 0; i < nValues; i++) {
			tvis.item.lParam = values[i].style;
			tvis.item.pszText = TranslateW(values[i].szDescr);
			tvis.item.stateMask = TVIS_STATEIMAGEMASK;
			tvis.item.state = INDEXTOSTATEIMAGEMASK((style & tvis.item.lParam) != 0 ? 2 : 1);
			m_tree.InsertItem(&tvis);
		}
	}

	DWORD MakeCheckBoxTreeFlags()
	{
		DWORD flags = 0;
		TVITEMEX tvi;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_STATE;
		tvi.hItem = m_tree.GetRoot();
		while (tvi.hItem) {
			m_tree.GetItem(&tvi);
			if (((tvi.state & TVIS_STATEIMAGEMASK) >> 12 == 2))
				flags |= tvi.lParam;
			tvi.hItem = m_tree.GetNextSibling(tvi.hItem);
		}
		return flags;
	}

	CCtrlCheck chkAutoMin, chkAutoPopup, chkCascade, chkSavePerContact, chkStayMinimized;
	CCtrlCheck chkSaveDrafts, chkDelTemp, chkHideContainer;
	CCtrlCombo cmbSendMode;
	CCtrlTreeView m_tree;

public:
	CMainOptionsDlg() :
		CBaseOptionDlg(IDD_OPT_MSGDLG),
		m_tree(this, IDC_POPLIST),
		chkAutoMin(this, IDC_AUTOMIN),
		chkCascade(this, IDC_CASCADE),
		chkDelTemp(this, IDC_DELTEMP),
		cmbSendMode(this, IDC_SENDMODE),
		chkAutoPopup(this, IDC_AUTOPOPUP),
		chkSaveDrafts(this, IDC_SAVEDRAFTS),
		chkHideContainer(this, IDC_HIDECONTAINERS),
		chkStayMinimized(this, IDC_STAYMINIMIZED),
		chkSavePerContact(this, IDC_SAVEPERCONTACT)
	{
		CreateLink(chkCascade, g_plugin.bCascade);
		CreateLink(chkAutoMin, g_plugin.bAutoMin);
		CreateLink(chkAutoPopup, g_plugin.bAutoPopup);
		CreateLink(chkSaveDrafts, g_plugin.bSaveDrafts);
		CreateLink(chkHideContainer, g_plugin.bHideContainer);
		CreateLink(chkStayMinimized, g_plugin.bStayMinimized);
		CreateLink(chkSavePerContact, g_plugin.bSavePerContact);

		chkCascade.OnChange = Callback(this, &CMainOptionsDlg::onChange_Cascade);
		chkAutoPopup.OnChange = Callback(this, &CMainOptionsDlg::onChange_AutoPopup);
		chkSavePerContact.OnChange = Callback(this, &CMainOptionsDlg::onChange_SavePerContact);
	}

	bool OnInitDialog() override
	{
		SetWindowLongPtr(m_tree.GetHwnd(), GWL_STYLE, (GetWindowLongPtr(m_tree.GetHwnd(), GWL_STYLE) & ~WS_BORDER) | TVS_NOHSCROLL | TVS_CHECKBOXES);
		FillCheckBoxTree(statusValues, _countof(statusValues), g_plugin.getDword(SRMSGSET_POPFLAGS, SRMSGDEFSET_POPFLAGS));

		SendDlgItemMessage(m_hwnd, IDC_SECONDSSPIN, UDM_SETRANGE, 0, MAKELONG(60, 4));
		SendDlgItemMessage(m_hwnd, IDC_SECONDSSPIN, UDM_SETPOS, 0, g_plugin.getDword(SRMSGSET_MSGTIMEOUT, SRMSGDEFSET_MSGTIMEOUT) / 1000);

		cmbSendMode.AddString(TranslateT("Enter"));
		cmbSendMode.AddString(TranslateT("Double 'Enter'"));
		cmbSendMode.AddString(TranslateT("Ctrl+Enter"));
		cmbSendMode.AddString(TranslateT("Shift+Enter"));
		cmbSendMode.SetCurSel(g_dat.sendMode);

		onChange_AutoPopup(0);
		return true;
	}

	bool OnApply() override
	{
		g_plugin.setDword(SRMSGSET_POPFLAGS, MakeCheckBoxTreeFlags());

		g_plugin.setDword(SRMSGSET_MSGTIMEOUT, (DWORD)SendDlgItemMessage(m_hwnd, IDC_SECONDSSPIN, UDM_GETPOS, 0, 0) * 1000);

		g_plugin.setByte(SRMSGSET_SENDMODE, cmbSendMode.GetCurSel());
		return true;
	}

	void onChange_AutoPopup(CCtrlCheck*)
	{
		bool bChecked = chkAutoPopup.GetState();
		m_tree.Enable(bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_STAYMINIMIZED), bChecked);
	}

	void onChange_Cascade(CCtrlCheck*)
	{
		chkSavePerContact.SetState(!chkCascade.GetState());
	}

	void onChange_SavePerContact(CCtrlCheck*)
	{
		chkCascade.SetState(!chkSavePerContact.GetState());
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class CTabsOptionsDlg : public CBaseOptionDlg
{
	CCtrlCheck chkUseTabs, chkLimitTabs, chkLimitChatTabs, chkLimitNames, chkSeparateChats;
	CCtrlCheck chkTabCloseButton, chkHideOneTab, chkTabsAtBottom, chkSwitchToActive;
	CCtrlSpin  spinNames, spinTabs, spinChatTabs;

public:
	CTabsOptionsDlg() :
		CBaseOptionDlg(IDD_OPT_MSGTABS),
		chkUseTabs(this, IDC_USETABS),
		chkLimitTabs(this, IDC_LIMITTABS),
		chkLimitNames(this, IDC_LIMITNAMES),
		chkLimitChatTabs(this, IDC_LIMITCHATSTABS),
		chkHideOneTab(this, IDC_HIDEONETAB),
		chkSeparateChats(this, IDC_SEPARATECHATSCONTAINERS),
		chkTabsAtBottom(this, IDC_TABSATBOTTOM),
		chkSwitchToActive(this, IDC_SWITCHTOACTIVE),
		chkTabCloseButton(this, IDC_TABCLOSEBUTTON),
		spinNames(this, IDC_LIMITNAMESLENSPIN, 100),
		spinTabs(this, IDC_LIMITTABSNUMSPIN, 100, 1),
		spinChatTabs(this, IDC_LIMITCHATSTABSNUMSPIN, 100, 1)
	{
		CreateLink(chkUseTabs, g_plugin.bUseTabs);
		CreateLink(chkLimitTabs, g_plugin.bLimitTabs);
		CreateLink(chkLimitNames, g_plugin.bLimitNames);
		CreateLink(chkLimitChatTabs, g_plugin.bLimitChatTabs);
		CreateLink(chkHideOneTab, g_plugin.bHideOneTab);
		CreateLink(chkSeparateChats, g_plugin.bSeparateChats);
		CreateLink(chkTabsAtBottom, g_plugin.bTabsAtBottom);
		CreateLink(chkSwitchToActive, g_plugin.bSwitchToActive);
		CreateLink(chkTabCloseButton, g_plugin.bTabCloseButton);
		
		CreateLink(spinNames, g_plugin.iLimitNames);
		CreateLink(spinTabs, g_plugin.iLimitTabs);
		CreateLink(spinChatTabs, g_plugin.iLimitChatTabs);

		chkUseTabs.OnChange = Callback(this, &CTabsOptionsDlg::onChange_UseTabs);
		chkLimitTabs.OnChange = Callback(this, &CTabsOptionsDlg::onChange_LimitTabs);
		chkLimitNames.OnChange = Callback(this, &CTabsOptionsDlg::onChange_LimitNames);
		chkLimitChatTabs.OnChange = Callback(this, &CTabsOptionsDlg::onChange_LimitChatTabs);
		chkSeparateChats.OnChange = Callback(this, &CTabsOptionsDlg::onChange_SeparateChats);
	}

	bool OnInitDialog() override
	{
		onChange_UseTabs(0);
		return true;
	}

	void onChange_UseTabs(CCtrlCheck*)
	{
		int bChecked = chkUseTabs.GetState();
		chkHideOneTab.Enable(bChecked);
		chkTabsAtBottom.Enable(bChecked);
		chkSwitchToActive.Enable(bChecked);
		chkTabCloseButton.Enable(bChecked);
		chkLimitNames.Enable(bChecked);
		chkSeparateChats.Enable(bChecked);
		chkLimitTabs.Enable(bChecked);

		onChange_LimitTabs(0);
		onChange_LimitNames(0);
		onChange_SeparateChats(0);
	}

	void onChange_LimitTabs(CCtrlCheck*)
	{
		int bChecked = chkUseTabs.GetState() && chkLimitTabs.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_LIMITTABSNUM), bChecked);
		spinTabs.Enable(bChecked);
	}

	void onChange_SeparateChats(CCtrlCheck*)
	{
		int bChecked = chkUseTabs.GetState() && chkSeparateChats.GetState();
		chkLimitChatTabs.Enable(bChecked);

		onChange_LimitChatTabs(0);
	}

	void onChange_LimitChatTabs(CCtrlCheck*)
	{
		int bChecked = chkUseTabs.GetState() && chkSeparateChats.GetState() && chkLimitChatTabs.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_LIMITCHATSTABSNUM), bChecked);
		spinChatTabs.Enable(bChecked);
	}

	void onChange_LimitNames(CCtrlCheck*)
	{
		int bChecked = chkUseTabs.GetState() && chkLimitNames.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_LIMITNAMESLEN), bChecked);
		spinNames.Enable(bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_CHARS), bChecked);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class CLayoutOptionsDlg : public CBaseOptionDlg
{
	CCtrlCheck chkTransparency, chkShowTitlebar, chkShowStatusBar, chkShowToolbar, chkShowInfobar, chkShowProgress, chkShowAvatar;

public:
	CLayoutOptionsDlg() : 
		CBaseOptionDlg(IDD_OPT_LAYOUT),
		chkTransparency(this, IDC_TRANSPARENCY),
		chkShowAvatar(this, IDC_AVATARSUPPORT),
		chkShowInfobar(this, IDC_SHOWINFOBAR),
		chkShowToolbar(this, IDC_SHOWTOOLBAR),
		chkShowTitlebar(this, IDC_SHOWTITLEBAR),
		chkShowProgress(this, IDC_SHOWPROGRESS),
		chkShowStatusBar(this, IDC_SHOWSTATUSBAR)
	{
		CreateLink(chkShowAvatar, g_plugin.bShowAvatar);
		CreateLink(chkShowInfobar, g_plugin.bShowInfoBar);
		CreateLink(chkShowToolbar, g_plugin.bShowToolBar);
		CreateLink(chkShowTitlebar, g_plugin.bShowTitleBar);
		CreateLink(chkShowProgress, g_plugin.bShowProgress);
		CreateLink(chkShowStatusBar, g_plugin.bShowStatusBar);

		chkTransparency.OnChange = Callback(this, &CLayoutOptionsDlg::onChange_Transparency);
		chkShowTitlebar.OnChange = Callback(this, &CLayoutOptionsDlg::onChange_ShowTitlebar);
	}

	bool OnInitDialog() override
	{
		SetWindowText(GetDlgItem(m_hwnd, IDC_TITLEFORMAT), g_dat.wszTitleFormat);
		CheckDlgButton(m_hwnd, IDC_TRANSPARENCY, g_plugin.getByte(SRMSGSET_USETRANSPARENCY, SRMSGDEFSET_USETRANSPARENCY) ? BST_CHECKED : BST_UNCHECKED);
		SendDlgItemMessage(m_hwnd, IDC_ATRANSPARENCYVALUE, TBM_SETRANGE, FALSE, MAKELONG(0, 255));
		SendDlgItemMessage(m_hwnd, IDC_ATRANSPARENCYVALUE, TBM_SETPOS, TRUE, g_plugin.getDword(SRMSGSET_ACTIVEALPHA, SRMSGDEFSET_ACTIVEALPHA));
		SendDlgItemMessage(m_hwnd, IDC_ITRANSPARENCYVALUE, TBM_SETRANGE, FALSE, MAKELONG(0, 255));
		SendDlgItemMessage(m_hwnd, IDC_ITRANSPARENCYVALUE, TBM_SETPOS, TRUE, g_plugin.getDword(SRMSGSET_INACTIVEALPHA, SRMSGDEFSET_INACTIVEALPHA));

		char str[10];
		mir_snprintf(str, "%d%%", (int)(100 * SendDlgItemMessage(m_hwnd, IDC_ATRANSPARENCYVALUE, TBM_GETPOS, 0, 0) / 255));
		SetDlgItemTextA(m_hwnd, IDC_ATRANSPARENCYPERC, str);

		mir_snprintf(str, "%d%%", (int)(100 * SendDlgItemMessage(m_hwnd, IDC_ITRANSPARENCYVALUE, TBM_GETPOS, 0, 0) / 255));
		SetDlgItemTextA(m_hwnd, IDC_ITRANSPARENCYPERC, str);

		SendDlgItemMessage(m_hwnd, IDC_INPUTLINESSPIN, UDM_SETRANGE, 0, MAKELONG(100, 1));
		SendDlgItemMessage(m_hwnd, IDC_INPUTLINESSPIN, UDM_SETPOS, 0, g_plugin.getWord(SRMSGSET_AUTORESIZELINES, SRMSGDEFSET_AUTORESIZELINES));

		onChange_Transparency(0);
		onChange_ShowTitlebar(0);

		CheckDlgButton(m_hwnd, IDC_AVATARSUPPORT, g_dat.flags.bShowAvatar);
		return true;
	}

	bool OnApply() override
	{
		GetWindowText(GetDlgItem(m_hwnd, IDC_TITLEFORMAT), g_dat.wszTitleFormat, _countof(g_dat.wszTitleFormat));
		g_plugin.setWString(SRMSGSET_WINDOWTITLE, g_dat.wszTitleFormat);

		g_plugin.setByte(SRMSGSET_USETRANSPARENCY, (BYTE)IsDlgButtonChecked(m_hwnd, IDC_TRANSPARENCY));
		g_plugin.setDword(SRMSGSET_ACTIVEALPHA, SendDlgItemMessage(m_hwnd, IDC_ATRANSPARENCYVALUE, TBM_GETPOS, 0, 0));
		g_plugin.setDword(SRMSGSET_INACTIVEALPHA, SendDlgItemMessage(m_hwnd, IDC_ITRANSPARENCYVALUE, TBM_GETPOS, 0, 0));

		g_plugin.setWord(SRMSGSET_AUTORESIZELINES, (WORD)SendDlgItemMessage(m_hwnd, IDC_INPUTLINESSPIN, UDM_GETPOS, 0, 0));
		LoadInfobarFonts();
		return true;
	}

	void onChange_Transparency(CCtrlCheck*)
	{
		int bChecked = IsDlgButtonChecked(m_hwnd, IDC_TRANSPARENCY);
		EnableWindow(GetDlgItem(m_hwnd, IDC_ATRANSPARENCYVALUE), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_ATRANSPARENCYPERC), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_ITRANSPARENCYVALUE), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_ITRANSPARENCYPERC), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_TRANSPARENCYTEXT1), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_TRANSPARENCYTEXT2), bChecked);
	}

	void onChange_ShowTitlebar(CCtrlCheck*)
	{
		bool bChecked = chkShowTitlebar.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_TITLEFORMAT), bChecked);
	}

	INT_PTR DlgProc(UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == WM_HSCROLL) {
			char str[10];
			mir_snprintf(str, "%d%%", (int)(100 * SendDlgItemMessage(m_hwnd, IDC_ATRANSPARENCYVALUE, TBM_GETPOS, 0, 0) / 256));
			SetDlgItemTextA(m_hwnd, IDC_ATRANSPARENCYPERC, str);
			
			mir_snprintf(str, "%d%%", (int)(100 * SendDlgItemMessage(m_hwnd, IDC_ITRANSPARENCYVALUE, TBM_GETPOS, 0, 0) / 256));
			SetDlgItemTextA(m_hwnd, IDC_ITRANSPARENCYPERC, str);
			SendMessage(GetParent(m_hwnd), PSM_CHANGED, 0, 0);
		}

		return CDlgBase::DlgProc(msg, wParam, lParam);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class CLogOptionsDlg : public CBaseOptionDlg
{
	CCtrlCheck chkLoadUnread, chkLoadCount, chkLoadTime;
	CCtrlCheck chkShowIcons, chkShowTime, chkShowSecs, chkShowDate, chkLongDate, chkRelativeDate;
	CCtrlCheck chkGroupMsg, chkIndentText, chkHideNames, chkMarkFollowups, chkMsgOnNewline, chkDrawLines;
	CCtrlRichEdit m_log;
	CCtrlHyperlink m_fonts;

	void OnChange() override
	{
		m_log.SetText(L"");

		struct GlobalMessageData gdat = {};
		gdat.flags.bShowIcons = chkShowIcons.GetState();
		gdat.flags.bHideNames = chkHideNames.GetState();
		gdat.flags.bShowTime = chkShowTime.GetState();
		gdat.flags.bShowSeconds = chkShowSecs.GetState();
		gdat.flags.bShowDate = chkShowDate.GetState();
		gdat.flags.bLongDate = chkLongDate.GetState();
		gdat.flags.bRelativeDate = chkRelativeDate.GetState();
		gdat.flags.bGroupMessages = chkGroupMsg.GetState();
		gdat.flags.bMarkFollowups = chkMarkFollowups.GetState();
		gdat.flags.bMsgOnNewline = chkMsgOnNewline.GetState();
		gdat.flags.bDrawLines = chkDrawLines.GetState();
		gdat.flags.bIndentText = chkIndentText.GetState();
		gdat.indentSize = (int)SendDlgItemMessage(m_hwnd, IDC_INDENTSPIN, UDM_GETPOS, 0, 0);

		PARAFORMAT2 pf2;
		pf2.cbSize = sizeof(pf2);
		pf2.dwMask = PFM_OFFSET;
		pf2.dxOffset = (gdat.flags.bIndentText) ? gdat.indentSize * 1440 / g_dat.logPixelSX : 0;
		m_log.SendMsg(EM_SETPARAFORMAT, 0, (LPARAM)&pf2);

		StreamInTestEvents(m_log.GetHwnd(), &gdat);
	}

public:
	CLogOptionsDlg() :
		CBaseOptionDlg(IDD_OPT_MSGLOG),
		m_log(this, IDC_SRMM_LOG),
		m_fonts(this, IDC_FONTSCOLORS),
		chkShowTime(this, IDC_SHOWTIMES),
		chkShowSecs(this, IDC_SHOWSECONDS),
		chkShowDate(this, IDC_SHOWDATES),
		chkLongDate(this, IDC_USELONGDATE),
		chkRelativeDate(this, IDC_USERELATIVEDATE),
		chkGroupMsg(this, IDC_GROUPMESSAGES),
		chkMarkFollowups(this, IDC_MARKFOLLOWUPS),
		chkDrawLines(this, IDC_DRAWLINES),
		chkShowIcons(this, IDC_SHOWLOGICONS),
		chkIndentText(this, IDC_INDENTTEXT),
		chkHideNames(this, IDC_HIDENAMES),
		chkMsgOnNewline(this, IDC_MESSAGEONNEWLINE),
		chkLoadTime(this, IDC_LOADTIME),
		chkLoadCount(this, IDC_LOADCOUNT),
		chkLoadUnread(this, IDC_LOADUNREAD)
	{
		m_fonts.OnClick = Callback(this, &CLogOptionsDlg::onClick_Fonts);

		CreateLink(chkShowTime, g_plugin.bShowTime);
		CreateLink(chkShowSecs, g_plugin.bShowSeconds);
		CreateLink(chkShowDate, g_plugin.bShowDate);
		CreateLink(chkLongDate, g_plugin.bLongDate);
		CreateLink(chkGroupMsg, g_plugin.bGroupMessages);
		CreateLink(chkShowIcons, g_plugin.bShowIcons);
		CreateLink(chkHideNames, g_plugin.bHideNames);
		CreateLink(chkDrawLines, g_plugin.bDrawLines);
		CreateLink(chkIndentText, g_plugin.bIndentText);
		CreateLink(chkMsgOnNewline, g_plugin.bMsgOnNewline);
		CreateLink(chkRelativeDate, g_plugin.bRelativeDate);
		CreateLink(chkMarkFollowups, g_plugin.bMarkFollowups);

		chkLoadTime.OnChange = chkLoadCount.OnChange = chkLoadUnread.OnChange = Callback(this, &CLogOptionsDlg::onChange_Time);
		chkShowDate.OnChange = Callback(this, &CLogOptionsDlg::onChange_Dates);
		chkShowTime.OnChange = Callback(this, &CLogOptionsDlg::onChange_Times);
		chkGroupMsg.OnChange = Callback(this, &CLogOptionsDlg::onChange_GroupMsg);
		chkIndentText.OnChange = Callback(this, &CLogOptionsDlg::onChange_IndentText);
	}

	bool OnInitDialog() override
	{
		switch (g_plugin.getByte(SRMSGSET_LOADHISTORY, SRMSGDEFSET_LOADHISTORY)) {
		case LOADHISTORY_UNREAD:
			CheckDlgButton(m_hwnd, IDC_LOADUNREAD, BST_CHECKED);
			break;
		case LOADHISTORY_COUNT:
			CheckDlgButton(m_hwnd, IDC_LOADCOUNT, BST_CHECKED);
			break;
		case LOADHISTORY_TIME:
			CheckDlgButton(m_hwnd, IDC_LOADTIME, BST_CHECKED);
			break;
		}
		onChange_Time(0);

		SendDlgItemMessage(m_hwnd, IDC_LOADCOUNTSPIN, UDM_SETRANGE, 0, MAKELONG(100, 0));
		SendDlgItemMessage(m_hwnd, IDC_LOADCOUNTSPIN, UDM_SETPOS, 0, g_plugin.getWord(SRMSGSET_LOADCOUNT, SRMSGDEFSET_LOADCOUNT));
		SendDlgItemMessage(m_hwnd, IDC_LOADTIMESPIN, UDM_SETRANGE, 0, MAKELONG(12 * 60, 0));
		SendDlgItemMessage(m_hwnd, IDC_LOADTIMESPIN, UDM_SETPOS, 0, g_plugin.getWord(SRMSGSET_LOADTIME, SRMSGDEFSET_LOADTIME));

		CheckDlgButton(m_hwnd, IDC_USEIEVIEW, g_plugin.getByte(SRMSGSET_USEIEVIEW, SRMSGDEFSET_USEIEVIEW) ? BST_CHECKED : BST_UNCHECKED);
		if (!g_dat.ieviewInstalled)
			EnableWindow(GetDlgItem(m_hwnd, IDC_USEIEVIEW), FALSE);

		onChange_Times(0);
		onChange_Dates(0);
		onChange_GroupMsg(0);
		onChange_IndentText(0);

		SendDlgItemMessage(m_hwnd, IDC_INDENTSPIN, UDM_SETRANGE, 0, MAKELONG(999, 0));
		SendDlgItemMessage(m_hwnd, IDC_INDENTSPIN, UDM_SETPOS, 0, g_plugin.getWord(SRMSGSET_INDENTSIZE, SRMSGDEFSET_INDENTSIZE));

		PARAFORMAT2 pf2;
		memset(&pf2, 0, sizeof(pf2));
		pf2.cbSize = sizeof(pf2);
		pf2.dwMask = PFM_OFFSETINDENT | PFM_RIGHTINDENT;
		pf2.dxStartIndent = 30;
		pf2.dxRightIndent = 30;
		m_log.SendMsg(EM_SETPARAFORMAT, 0, (LPARAM)&pf2);
		m_log.SendMsg(EM_SETEDITSTYLE, SES_EXTENDBACKCOLOR, SES_EXTENDBACKCOLOR);
		m_log.SendMsg(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(0, 0));
		m_log.SendMsg(EM_AUTOURLDETECT, TRUE, 0);
		m_log.SetReadOnly(true);

		OnChange();
		return true;
	}

	bool OnApply() override
	{
		if (IsDlgButtonChecked(m_hwnd, IDC_LOADCOUNT))
			g_plugin.setByte(SRMSGSET_LOADHISTORY, LOADHISTORY_COUNT);
		else if (IsDlgButtonChecked(m_hwnd, IDC_LOADTIME))
			g_plugin.setByte(SRMSGSET_LOADHISTORY, LOADHISTORY_TIME);
		else
			g_plugin.setByte(SRMSGSET_LOADHISTORY, LOADHISTORY_UNREAD);
		g_plugin.setWord(SRMSGSET_LOADCOUNT, (WORD)SendDlgItemMessage(m_hwnd, IDC_LOADCOUNTSPIN, UDM_GETPOS, 0, 0));
		g_plugin.setWord(SRMSGSET_LOADTIME, (WORD)SendDlgItemMessage(m_hwnd, IDC_LOADTIMESPIN, UDM_GETPOS, 0, 0));

		g_plugin.setByte(SRMSGSET_USEIEVIEW, (BYTE)IsDlgButtonChecked(m_hwnd, IDC_USEIEVIEW));
		g_plugin.setWord(SRMSGSET_INDENTSIZE, (WORD)SendDlgItemMessage(m_hwnd, IDC_INDENTSPIN, UDM_GETPOS, 0, 0));

		FreeMsgLogIcons();
		LoadMsgLogIcons();
		return true;
	}

	void onClick_Fonts(CCtrlHyperlink*)
	{
		g_plugin.openOptions(L"Customize", L"Fonts and colors");
	}

	void onChange_Time(CCtrlCheck*)
	{
		int bChecked = IsDlgButtonChecked(m_hwnd, IDC_LOADCOUNT);
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADCOUNTN), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADCOUNTSPIN), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADCOUNTTEXT2), bChecked);

		bChecked = IsDlgButtonChecked(m_hwnd, IDC_LOADTIME);
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADTIMEN), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADTIMESPIN), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_STMINSOLD), bChecked);
	}

	void onChange_Times(CCtrlCheck*)
	{
		int bChecked = chkShowTime.GetState();
		chkShowSecs.Enable(bChecked);
		chkShowDate.Enable(bChecked);

		onChange_Dates(0);
	}

	void onChange_Dates(CCtrlCheck*)
	{
		int bChecked = chkShowDate.GetState() && chkShowTime.GetState();
		chkLongDate.Enable(bChecked);
		chkRelativeDate.Enable(bChecked);
	}

	void onChange_GroupMsg(CCtrlCheck*)
	{
		chkMarkFollowups.Enable(chkGroupMsg.GetState());
	}

	void onChange_IndentText(CCtrlCheck*)
	{
		int bChecked = chkIndentText.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_INDENTSIZE), bChecked);
		EnableWindow(GetDlgItem(m_hwnd, IDC_INDENTSPIN), bChecked);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

static void ResetCList(HWND hwndDlg)
{
	if (!db_get_b(0, "CList", "UseGroups", SETTING_USEGROUPS_DEFAULT))
		SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETUSEGROUPS, FALSE, 0);
	else
		SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETUSEGROUPS, TRUE, 0);
	SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETHIDEEMPTYGROUPS, 1, 0);
}

static void RebuildList(HWND hwndDlg, HANDLE hItemNew, HANDLE hItemUnknown)
{
	BYTE defType = g_plugin.getByte(SRMSGSET_TYPINGNEW, SRMSGDEFSET_TYPINGNEW);
	if (hItemNew && defType)
		SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETCHECKMARK, (WPARAM)hItemNew, 1);

	if (hItemUnknown && g_plugin.getByte(SRMSGSET_TYPINGUNKNOWN, SRMSGDEFSET_TYPINGUNKNOWN))
		SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETCHECKMARK, (WPARAM)hItemUnknown, 1);

	for (auto &hContact : Contacts()) {
		HANDLE hItem = (HANDLE)SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_FINDCONTACT, hContact, 0);
		if (hItem && g_plugin.getByte(hContact, SRMSGSET_TYPING, defType))
			SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_SETCHECKMARK, (WPARAM)hItem, 1);
	}
}

static void SaveList(HWND hwndDlg, HANDLE hItemNew, HANDLE hItemUnknown)
{
	if (hItemNew)
		g_plugin.setByte(SRMSGSET_TYPINGNEW, (BYTE)(SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_GETCHECKMARK, (WPARAM)hItemNew, 0) ? 1 : 0));

	if (hItemUnknown)
		g_plugin.setByte(SRMSGSET_TYPINGUNKNOWN, (BYTE)(SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_GETCHECKMARK, (WPARAM)hItemUnknown, 0) ? 1 : 0));

	for (auto &hContact : Contacts()) {
		HANDLE hItem = (HANDLE)SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_FINDCONTACT, hContact, 0);
		if (hItem)
			g_plugin.setByte(hContact, SRMSGSET_TYPING, (BYTE)(SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_GETCHECKMARK, (WPARAM)hItem, 0) ? 1 : 0));
	}
}

static INT_PTR CALLBACK DlgProcTypeOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hItemNew, hItemUnknown;

	switch (msg) {
	case WM_INITDIALOG:
		TranslateDialogDefault(hwndDlg);
		{
			CLCINFOITEM cii = { sizeof(cii) };
			cii.flags = CLCIIF_GROUPFONT | CLCIIF_CHECKBOX;
			cii.pszText = (wchar_t*)TranslateT("** New contacts **");
			hItemNew = (HANDLE)SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_ADDINFOITEM, 0, (LPARAM)&cii);
			cii.pszText = (wchar_t*)TranslateT("** Unknown contacts **");
			hItemUnknown = (HANDLE)SendDlgItemMessage(hwndDlg, IDC_CLIST, CLM_ADDINFOITEM, 0, (LPARAM)&cii);
		}

		SetWindowLongPtr(GetDlgItem(hwndDlg, IDC_CLIST), GWL_STYLE, GetWindowLongPtr(GetDlgItem(hwndDlg, IDC_CLIST), GWL_STYLE) | CLS_SHOWHIDDEN | CLS_NOHIDEOFFLINE);
		ResetCList(hwndDlg);

		CheckDlgButton(hwndDlg, IDC_SHOWNOTIFY, g_plugin.getByte(SRMSGSET_SHOWTYPING, SRMSGDEFSET_SHOWTYPING) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_TYPEWIN, g_plugin.getByte(SRMSGSET_SHOWTYPINGWIN, SRMSGDEFSET_SHOWTYPINGWIN) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_TYPETRAY, g_plugin.getByte(SRMSGSET_SHOWTYPINGNOWIN, SRMSGDEFSET_SHOWTYPINGNOWIN) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_NOTIFYTRAY, g_plugin.getByte(SRMSGSET_SHOWTYPINGCLIST, SRMSGDEFSET_SHOWTYPINGCLIST) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_NOTIFYBALLOON, !g_plugin.getByte(SRMSGSET_SHOWTYPINGCLIST, SRMSGDEFSET_SHOWTYPINGCLIST) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_TYPINGSWITCH, g_plugin.getByte(SRMSGSET_SHOWTYPINGSWITCH, SRMSGDEFSET_SHOWTYPINGSWITCH) ? BST_CHECKED : BST_UNCHECKED);
		EnableWindow(GetDlgItem(hwndDlg, IDC_TYPEWIN), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
		EnableWindow(GetDlgItem(hwndDlg, IDC_TYPETRAY), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
		EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYTRAY), IsDlgButtonChecked(hwndDlg, IDC_TYPETRAY));
		EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYBALLOON), IsDlgButtonChecked(hwndDlg, IDC_TYPETRAY));
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_TYPETRAY:
			if (IsDlgButtonChecked(hwndDlg, IDC_TYPETRAY)) {
				EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYTRAY), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYBALLOON), TRUE);
			}
			else {
				EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYTRAY), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYBALLOON), FALSE);
			}
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;

		case IDC_SHOWNOTIFY:
			EnableWindow(GetDlgItem(hwndDlg, IDC_TYPEWIN), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
			EnableWindow(GetDlgItem(hwndDlg, IDC_TYPETRAY), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
			EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYTRAY), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
			EnableWindow(GetDlgItem(hwndDlg, IDC_NOTIFYBALLOON), IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
			__fallthrough;

		case IDC_TYPEWIN:
		case IDC_NOTIFYTRAY:
		case IDC_NOTIFYBALLOON:
		case IDC_TYPINGSWITCH:
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		}
		break;
	
	case WM_NOTIFY:
		switch (((NMHDR *)lParam)->idFrom) {
		case IDC_CLIST:
			switch (((NMHDR *)lParam)->code) {
			case CLN_OPTIONSCHANGED:
				ResetCList(hwndDlg);
				break;
			case CLN_CHECKCHANGED:
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
				break;
			case CLN_LISTREBUILT:
				RebuildList(hwndDlg, hItemNew, hItemUnknown);
				break;
			}
			break;
		
		case 0:
			switch (((LPNMHDR)lParam)->code) {
			case PSN_APPLY:
				SaveList(hwndDlg, hItemNew, hItemUnknown);
				g_plugin.setByte(SRMSGSET_SHOWTYPING, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_SHOWNOTIFY));
				g_plugin.setByte(SRMSGSET_SHOWTYPINGWIN, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_TYPEWIN));
				g_plugin.setByte(SRMSGSET_SHOWTYPINGNOWIN, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_TYPETRAY));
				g_plugin.setByte(SRMSGSET_SHOWTYPINGCLIST, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_NOTIFYTRAY));
				g_plugin.setByte(SRMSGSET_SHOWTYPINGSWITCH, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_TYPINGSWITCH));
				ReloadGlobals();
				Srmm_Broadcast(DM_OPTIONSAPPLIED, 0, 0);
			}
			break;
		}
		break;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////

int OptInitialise(WPARAM wParam, LPARAM)
{
	OPTIONSDIALOGPAGE odp = {};
	odp.position = 910000000;
	odp.szTitle.a = LPGEN("Message sessions");
	odp.flags = ODPF_BOLDGROUPS;

	odp.pDialog = new CMainOptionsDlg();
	odp.szTab.a = LPGEN("General");
	g_plugin.addOptions(wParam, &odp);

	odp.pDialog = new CTabsOptionsDlg();
	odp.szTab.a = LPGEN("Tabs");
	g_plugin.addOptions(wParam, &odp);

	odp.pDialog = new CLayoutOptionsDlg();
	odp.szTab.a = LPGEN("Layout");
	g_plugin.addOptions(wParam, &odp);

	odp.pDialog = new CLogOptionsDlg();
	odp.szTab.a = LPGEN("Event log");
	g_plugin.addOptions(wParam, &odp);

	////////////////////////////////////////////////////////////////////////////////////////
	odp.pDialog = nullptr;
	odp.szGroup.a = LPGEN("Message sessions");
	odp.szTitle.a = LPGEN("Group chats");

	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS1);
	odp.pfnDlgProc = DlgProcOptions1;
	odp.szTab.a = LPGEN("General");
	g_plugin.addOptions(wParam, &odp);

	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS2);
	odp.pfnDlgProc = DlgProcOptions2;
	odp.szTab.a = LPGEN("Event log");
	g_plugin.addOptions(wParam, &odp);

	////////////////////////////////////////////////////////////////////////////////////////
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_MSGTYPE);
	odp.szTitle.a = LPGEN("Typing notify");
	odp.pfnDlgProc = DlgProcTypeOptions;
	odp.szTab.a = nullptr;
	g_plugin.addOptions(wParam, &odp);

	////////////////////////////////////////////////////////////////////////////////////////
	odp.position = 910000002;
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONSPOPUP);
	odp.szGroup.a = LPGEN("Popups");
	odp.szTitle.a = LPGEN("Messaging");
	odp.pfnDlgProc = DlgProcOptionsPopup;
	odp.flags = ODPF_BOLDGROUPS;
	g_plugin.addOptions(wParam, &odp);
	return 0;
}
