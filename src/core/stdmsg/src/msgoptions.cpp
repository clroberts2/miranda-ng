/*

Copyright 2000-12 Miranda IM, 2012-18 Miranda NG team,
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

#include "m_fontservice.h"

int ChatOptionsInitialize(WPARAM);

struct FontOptionsList
{
	const wchar_t* szDescr;
	COLORREF     defColour;
	const wchar_t* szDefFace;
	BYTE         defStyle;
	char         defSize;
}
static const fontOptionsList[] =
{
	{ LPGENW("Outgoing messages"), RGB(106, 106, 106), L"Arial",    0, -12},
	{ LPGENW("Incoming messages"), RGB(0, 0, 0),       L"Arial",    0, -12},
	{ LPGENW("Outgoing name"),     RGB(89, 89, 89),    L"Arial",    DBFONTF_BOLD, -12},
	{ LPGENW("Outgoing time"),     RGB(0, 0, 0),       L"Terminal", DBFONTF_BOLD, -9},
	{ LPGENW("Outgoing colon"),    RGB(89, 89, 89),    L"Arial",    0, -11},
	{ LPGENW("Incoming name"),     RGB(215, 0, 0),     L"Arial",    DBFONTF_BOLD, -12},
	{ LPGENW("Incoming time"),     RGB(0, 0, 0),       L"Terminal", DBFONTF_BOLD, -9},
	{ LPGENW("Incoming colon"),    RGB(215, 0, 0),     L"Arial",    0, -11},
	{ LPGENW("Message area"),      RGB(0, 0, 0),       L"Arial",    0, -12},
	{ LPGENW("Other events"),      RGB(90, 90, 160),   L"Arial",    0, -12},
};

static BYTE MsgDlgGetFontDefaultCharset(const wchar_t*)
{
  return DEFAULT_CHARSET;
}

bool LoadMsgDlgFont(int i, LOGFONT* lf, COLORREF * colour)
{
	if (i >= _countof(fontOptionsList))
		return false;

	char str[32];

	if (colour) {
		mir_snprintf(str, "SRMFont%dCol", i);
		*colour = db_get_dw(0, SRMMMOD, str, fontOptionsList[i].defColour);
	}
	if (lf) {
		mir_snprintf(str, "SRMFont%dSize", i);
		lf->lfHeight = (char)db_get_b(0, SRMMMOD, str, fontOptionsList[i].defSize);
		lf->lfWidth = 0;
		lf->lfEscapement = 0;
		lf->lfOrientation = 0;
		mir_snprintf(str, "SRMFont%dSty", i);
		int style = db_get_b(0, SRMMMOD, str, fontOptionsList[i].defStyle);
		lf->lfWeight = style & DBFONTF_BOLD ? FW_BOLD : FW_NORMAL;
		lf->lfItalic = style & DBFONTF_ITALIC ? 1 : 0;
		lf->lfUnderline = 0;
		lf->lfStrikeOut = 0;
		lf->lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf->lfQuality = DEFAULT_QUALITY;
		lf->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		mir_snprintf(str, "SRMFont%d", i);

		ptrW wszFontFace(db_get_wsa(0, SRMMMOD, str));
		if (wszFontFace == nullptr)
			wcsncpy_s(lf->lfFaceName, fontOptionsList[i].szDefFace, _TRUNCATE);
		else
			mir_wstrncpy(lf->lfFaceName, wszFontFace, _countof(lf->lfFaceName));

		mir_snprintf(str, "SRMFont%dSet", i);
		lf->lfCharSet = db_get_b(0, SRMMMOD, str, MsgDlgGetFontDefaultCharset(lf->lfFaceName));
	}
	return true;
}

void RegisterSRMMFonts(void)
{
	char idstr[10];

	FontIDW fontid = { sizeof(fontid) };
	fontid.flags = FIDF_ALLOWREREGISTER | FIDF_DEFAULTVALID;
	for (int i = 0; i < _countof(fontOptionsList); i++) {
		strncpy_s(fontid.dbSettingsGroup, SRMMMOD, _TRUNCATE);
		wcsncpy_s(fontid.group, LPGENW("Message log"), _TRUNCATE);
		wcsncpy_s(fontid.name, fontOptionsList[i].szDescr, _TRUNCATE);
		mir_snprintf(idstr, "SRMFont%d", i);
		strncpy_s(fontid.prefix, idstr, _TRUNCATE);
		fontid.order = i;

		fontid.flags &= ~FIDF_CLASSMASK;
		fontid.flags |= (fontOptionsList[i].defStyle == DBFONTF_BOLD) ? FIDF_CLASSHEADER : FIDF_CLASSGENERAL;

		fontid.deffontsettings.colour = fontOptionsList[i].defColour;
		fontid.deffontsettings.size = fontOptionsList[i].defSize;
		fontid.deffontsettings.style = fontOptionsList[i].defStyle;
		wcsncpy_s(fontid.deffontsettings.szFace, fontOptionsList[i].szDefFace, _TRUNCATE);
		fontid.deffontsettings.charset = MsgDlgGetFontDefaultCharset(fontOptionsList[i].szDefFace);
		wcsncpy_s(fontid.backgroundGroup, LPGENW("Message log"), _TRUNCATE);
		wcsncpy_s(fontid.backgroundName, LPGENW("Background"), _TRUNCATE);
		Font_RegisterW(&fontid);
	}

	ColourIDW colourid = { sizeof(colourid) };
	strncpy_s(colourid.dbSettingsGroup, SRMMMOD, _TRUNCATE);
	strncpy_s(colourid.setting, SRMSGSET_BKGCOLOUR, _TRUNCATE);
	colourid.defcolour = SRMSGDEFSET_BKGCOLOUR;
	wcsncpy_s(colourid.name, LPGENW("Background"), _TRUNCATE);
	wcsncpy_s(colourid.group, LPGENW("Message log"), _TRUNCATE);
	Colour_RegisterW(&colourid);
}

/////////////////////////////////////////////////////////////////////////////////////////

struct CheckBoxValues_t
{
	DWORD  style;
	wchar_t* szDescr;
}
statusValues[] =
{
	{ MODEF_OFFLINE, LPGENW("Offline") },
	{ PF2_ONLINE, LPGENW("Online") },
	{ PF2_SHORTAWAY, LPGENW("Away") },
	{ PF2_LONGAWAY, LPGENW("Not available") },
	{ PF2_LIGHTDND, LPGENW("Occupied") },
	{ PF2_HEAVYDND, LPGENW("Do not disturb") },
	{ PF2_FREECHAT, LPGENW("Free for chat") },
	{ PF2_INVISIBLE, LPGENW("Invisible") },
	{ PF2_OUTTOLUNCH, LPGENW("Out to lunch") },
	{ PF2_ONTHEPHONE, LPGENW("On the phone") }
};

class COptionMainDlg : public CPluginDlgBase
{
	CCtrlEdit  edtNFlash, edtAvatarH, edtSecs;
	CCtrlCheck chkAutoMin, chkAutoClose, chkSavePerContact, chkDoNotStealFocus;
	CCtrlCheck chkDelTemp, chkCascade, chkCharCount, chkStatusWin, chkCtrlSupport;
	CCtrlCheck chkAvatar, chkLimitAvatar;
	CCtrlCheck chkSendOnEnter, chkSendOnDblEnter, chkShowSend, chkShowButtons;

	CCtrlTreeView tree;

	void FillCheckBoxTree(DWORD style)
	{
		TVINSERTSTRUCT tvis;
		tvis.hParent = nullptr;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
		for (auto &it : statusValues) {
			tvis.item.lParam = it.style;
			tvis.item.pszText = TranslateW(it.szDescr);
			tvis.item.stateMask = TVIS_STATEIMAGEMASK;
			tvis.item.iImage = (style & it.style) != 0;
			tree.InsertItem(&tvis);
		}
	}

	DWORD MakeCheckBoxTreeFlags()
	{
		DWORD flags = 0;

		TVITEMEX tvi;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE;
		tvi.hItem = tree.GetRoot();
		while (tvi.hItem) {
			tree.GetItem(&tvi);
			if (tvi.iImage)
				flags |= tvi.lParam;
			tvi.hItem = tree.GetNextSibling(tvi.hItem);
		}
		return flags;
	}

public:
	COptionMainDlg() :
		CPluginDlgBase(g_hInst, IDD_OPT_MSGDLG, SRMMMOD),
		tree(this, IDC_POPLIST),
		edtSecs(this, IDC_SECONDS),
		edtNFlash(this, IDC_NFLASHES),
		edtAvatarH(this, IDC_AVATARHEIGHT),
		chkAvatar(this, IDC_AVATARSUPPORT),
		chkDelTemp(this, IDC_DELTEMP),
		chkAutoMin(this, IDC_AUTOMIN),
		chkCascade(this, IDC_CASCADE),
		chkShowSend(this, IDC_SHOWSENDBTN),
		chkAutoClose(this, IDC_AUTOCLOSE),
		chkStatusWin(this, IDC_STATUSWIN),
		chkCharCount(this, IDC_CHARCOUNT),
		chkLimitAvatar(this, IDC_LIMITAVATARH),
		chkShowButtons(this, IDC_SHOWBUTTONLINE),
		chkCtrlSupport(this, IDC_CTRLSUPPORT),
		chkSendOnEnter(this, IDC_SENDONENTER),
		chkSendOnDblEnter(this, IDC_SENDONDBLENTER),
		chkSavePerContact(this, IDC_SAVEPERCONTACT),
		chkDoNotStealFocus(this, IDC_DONOTSTEALFOCUS)
	{
		tree.SetFlags(MTREE_CHECKBOX);

		chkAvatar.OnChange = Callback(this, &COptionMainDlg::onChange_Avatar);
		chkAutoMin.OnChange = Callback(this, &COptionMainDlg::onChange_AutoMin);
		chkAutoClose.OnChange = Callback(this, &COptionMainDlg::onChange_AutoClose);
		chkLimitAvatar.OnChange = Callback(this, &COptionMainDlg::onChange_LimitAvatar);
		chkSendOnEnter.OnChange = Callback(this, &COptionMainDlg::onChange_SendOnEnter);
		chkSendOnDblEnter.OnChange = Callback(this, &COptionMainDlg::onChange_SendOnDblEnter);
		chkSavePerContact.OnChange = Callback(this, &COptionMainDlg::onChange_SavePerContact);

		CreateLink(edtNFlash, g_dat.nFlashMax);
		CreateLink(edtAvatarH, g_dat.iAvatarHeight);

		CreateLink(chkAvatar, g_dat.bShowAvatar);
		CreateLink(chkLimitAvatar, g_dat.bLimitAvatarHeight);

		CreateLink(chkDelTemp, g_dat.bDeleteTempCont);
		CreateLink(chkCascade, g_dat.bCascade);
		CreateLink(chkAutoMin, g_dat.bAutoMin);
		CreateLink(chkAutoClose, g_dat.bAutoClose);
		CreateLink(chkShowSend, g_dat.bSendButton);
		CreateLink(chkCharCount, g_dat.bShowReadChar);
		CreateLink(chkStatusWin, g_dat.bUseStatusWinIcon);
		CreateLink(chkShowButtons, g_dat.bShowButtons);
		CreateLink(chkSendOnEnter, g_dat.bSendOnEnter);
		CreateLink(chkSendOnDblEnter, g_dat.bSendOnDblEnter);
		CreateLink(chkSavePerContact, g_dat.bSavePerContact);
		CreateLink(chkDoNotStealFocus, g_dat.bDoNotStealFocus);
	}

	virtual void OnInitDialog() override
	{
		FillCheckBoxTree(g_dat.popupFlags);

		DWORD msgTimeout = g_dat.msgTimeout;
		edtSecs.SetInt((msgTimeout >= 5000) ? msgTimeout / 1000 : 5);
		
		onChange_Avatar(nullptr);

		chkCascade.Enable(!g_dat.bSavePerContact);
		chkCtrlSupport.Enable(!g_dat.bAutoClose);
	}

	virtual void OnApply() override
	{
		g_dat.popupFlags = MakeCheckBoxTreeFlags();

		DWORD msgTimeout = edtSecs.GetInt() * 1000;
		if (msgTimeout < 5000)
			msgTimeout = 5000;
		g_dat.msgTimeout = msgTimeout;

		Srmm_Broadcast(DM_OPTIONSAPPLIED, TRUE, 0);
	}

	void onChange_AutoMin(CCtrlCheck*)
	{
		chkAutoClose.SetState(false);
		chkCtrlSupport.Enable(!chkAutoClose.GetState());
	}

	void onChange_AutoClose(CCtrlCheck*)
	{
		chkAutoMin.SetState(false);
		chkCtrlSupport.Enable(!chkAutoClose.GetState());
	}

	void onChange_SendOnEnter(CCtrlCheck*)
	{
		chkSendOnDblEnter.SetState(false);
	}

	void onChange_SendOnDblEnter(CCtrlCheck*)
	{
		chkSendOnEnter.SetState(false);
	}

	void onChange_SavePerContact(CCtrlCheck*)
	{
		chkCascade.Enable(!chkSavePerContact.GetState());
	}

	void onChange_Avatar(CCtrlCheck*)
	{
		bool bEnabled = chkAvatar.GetState();
		edtAvatarH.Enable(bEnabled);
		chkLimitAvatar.Enable(bEnabled);
	}

	void onChange_LimitAvatar(CCtrlCheck*)
	{
		edtAvatarH.Enable(chkLimitAvatar.GetState());
	}

	void onChange_Tree(CCtrlTreeView::TEventInfo*)
	{
		NotifyChange();
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionLogDlg : public CPluginDlgBase
{
	HBRUSH hBkgColourBrush;

	CCtrlCheck chkLoadUnread, chkLoadCount, chkLoadTime, chkDate, chkTime, chkSecs, chkIcons;
	CCtrlCheck chkShowNames, chkFormat;
	CCtrlSpin  spinCount, spinTime;

public:
	COptionLogDlg() :
		CPluginDlgBase(g_hInst, IDD_OPT_MSGLOG, SRMMMOD),
		chkSecs(this, IDC_SHOWSECS),
		chkDate(this, IDC_SHOWDATES),
		chkTime(this, IDC_SHOWTIMES),
		chkIcons(this, IDC_SHOWLOGICONS),
		chkFormat(this, IDC_SHOWFORMATTING),
		chkLoadTime(this, IDC_LOADTIME),
		chkShowNames(this, IDC_SHOWNAMES),
		chkLoadCount(this, IDC_LOADCOUNT),
		chkLoadUnread(this, IDC_LOADUNREAD),
		
		spinTime(this, IDC_LOADTIMESPIN),
		spinCount(this, IDC_LOADCOUNTSPIN)
	{
		chkTime.OnChange = Callback(this, &COptionLogDlg::onChange_Time);
		chkLoadUnread.OnChange = chkLoadCount.OnChange = chkLoadTime.OnChange = Callback(this, &COptionLogDlg::onChange_Load);

		CreateLink(chkSecs, g_dat.bShowSecs);
		CreateLink(chkDate, g_dat.bShowDate);
		CreateLink(chkTime, g_dat.bShowTime);
		CreateLink(chkIcons, g_dat.bShowIcons);
		CreateLink(chkFormat, g_dat.bShowFormat);
		CreateLink(chkShowNames, g_dat.bShowNames);
	}
	
	virtual void OnInitDialog() override
	{
		switch (g_dat.iLoadHistory) {
		case LOADHISTORY_UNREAD:
			chkLoadUnread.SetState(true);
			break;
		case LOADHISTORY_COUNT:
			chkLoadCount.SetState(true);
			EnableWindow(GetDlgItem(m_hwnd, IDC_LOADCOUNTN), TRUE);
			spinCount.Enable();
			break;
		case LOADHISTORY_TIME:
			chkLoadTime.SetState(true);
			EnableWindow(GetDlgItem(m_hwnd, IDC_LOADTIMEN), TRUE);
			spinTime.Enable();
			EnableWindow(GetDlgItem(m_hwnd, IDC_STMINSOLD), TRUE);
			break;
		}
		spinCount.SetRange(100);
		spinCount.SetPosition(g_dat.nLoadCount);
		spinTime.SetRange(12 * 60);
		spinTime.SetPosition(g_dat.nLoadTime);

		onChange_Time(nullptr);
	}

	virtual void OnApply() override
	{
		if (chkLoadCount.GetState())
			g_dat.iLoadHistory = LOADHISTORY_COUNT;
		else if (chkLoadTime.GetState())
			g_dat.iLoadHistory = LOADHISTORY_TIME;
		else
			g_dat.iLoadHistory = LOADHISTORY_UNREAD;
		g_dat.nLoadCount = spinCount.GetPosition();
		g_dat.nLoadTime = spinTime.GetPosition();

		FreeMsgLogIcons();
		LoadMsgLogIcons();
		Srmm_Broadcast(DM_OPTIONSAPPLIED, TRUE, 0);
	}

	virtual void OnDestroy() override
	{
		DeleteObject(hBkgColourBrush);
	}

	void onChange_Load(CCtrlCheck*)
	{
		bool bEnabled = chkLoadCount.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADCOUNTN), bEnabled);
		spinCount.Enable(bEnabled);

		bEnabled = chkLoadTime.GetState();
		EnableWindow(GetDlgItem(m_hwnd, IDC_LOADTIMEN), bEnabled);
		spinTime.Enable(bEnabled);
		EnableWindow(GetDlgItem(m_hwnd, IDC_STMINSOLD), bEnabled);
	}

	void onChange_Time(CCtrlCheck*)
	{
		bool bEnabled = chkTime.GetState();
		chkSecs.Enable(bEnabled);
		chkDate.Enable(bEnabled);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionTypingDlg : public CPluginDlgBase
{
	HANDLE hItemNew, hItemUnknown;

	CCtrlClc clist;
	CCtrlCheck chkType, chkTypeWin, chkTypeTray, chkTypeClist, chkTypeBalloon;

public:
	COptionTypingDlg() :
		CPluginDlgBase(g_hInst, IDD_OPT_MSGTYPE, SRMMMOD),
		clist(this, IDC_CLIST),
		chkType(this, IDC_SHOWNOTIFY),
		chkTypeWin(this, IDC_TYPEWIN),
		chkTypeTray(this, IDC_TYPETRAY),
		chkTypeClist(this, IDC_NOTIFYTRAY),
		chkTypeBalloon(this, IDC_NOTIFYBALLOON)
	{
		chkType.OnChange = Callback(this, &COptionTypingDlg::onChange_ShowNotify);
		chkTypeTray.OnChange = Callback(this, &COptionTypingDlg::onChange_Tray);

		CreateLink(chkType, g_dat.bShowTyping);
		CreateLink(chkTypeWin, g_dat.bShowTypingWin);
		CreateLink(chkTypeTray, g_dat.bShowTypingTray);
		CreateLink(chkTypeClist, g_dat.bShowTypingClist);
	}

	void ResetCList(CCtrlClc::TEventInfo* = nullptr)
	{
		clist.SetUseGroups(db_get_b(0, "CList", "UseGroups", SETTING_USEGROUPS_DEFAULT));
		clist.SetHideEmptyGroups(1);
	}

	void RebuildList(CCtrlClc::TEventInfo* = nullptr)
	{
		BYTE defType = g_dat.bTypingNew;
		if (hItemNew && defType)
			clist.SetCheck(hItemNew, 1);

		if (hItemUnknown && g_dat.bTypingUnknown)
			clist.SetCheck(hItemUnknown, 1);

		for (auto &hContact : contact_iter()) {
			HANDLE hItem = clist.FindContact(hContact);
			if (hItem && db_get_b(hContact, SRMMMOD, SRMSGSET_TYPING, defType))
				clist.SetCheck(hItem, 1);
		}
	}

	void SaveList()
	{
		if (hItemNew)
			g_dat.bTypingNew = clist.GetCheck(hItemNew);

		if (hItemUnknown)
			g_dat.bTypingUnknown = clist.GetCheck(hItemUnknown);

		for (auto &hContact : contact_iter()) {
			HANDLE hItem = clist.FindContact(hContact);
			if (hItem)
				db_set_b(hContact, SRMMMOD, SRMSGSET_TYPING, clist.GetCheck(hItem));
		}
	}

	virtual void OnInitDialog() override
	{
		CLCINFOITEM cii = { sizeof(cii) };
		cii.flags = CLCIIF_GROUPFONT | CLCIIF_CHECKBOX;
		cii.pszText = TranslateT("** New contacts **");
		hItemNew = clist.AddInfoItem(&cii);
		cii.pszText = TranslateT("** Unknown contacts **");
		hItemUnknown = clist.AddInfoItem(&cii);

		SetWindowLongPtr(clist.GetHwnd(), GWL_STYLE, GetWindowLongPtr(clist.GetHwnd(), GWL_STYLE) | (CLS_SHOWHIDDEN) | (CLS_NOHIDEOFFLINE));
		ResetCList();

		clist.OnListRebuilt = Callback(this, &COptionTypingDlg::RebuildList);
		clist.OnCheckChanged = Callback(this, &COptionTypingDlg::onChange_Clist);
		clist.OnOptionsChanged = Callback(this, &COptionTypingDlg::ResetCList);

		onChange_ShowNotify(nullptr);
	}

	virtual void OnApply() override
	{
		SaveList();
		Srmm_Broadcast(DM_OPTIONSAPPLIED, TRUE, 0);
	}

	void onChange_Clist(CCtrlClc::TEventInfo*)
	{
		NotifyChange();
	}

	void onChange_Tray(CCtrlCheck*)
	{
		bool bStatus = chkTypeTray.GetState() != 0;
		chkTypeClist.Enable(bStatus);
		chkTypeBalloon.Enable(bStatus);
	}

	void onChange_ShowNotify(CCtrlCheck*)
	{
		bool bStatus = chkType.GetState();
		chkTypeWin.Enable(bStatus);
		chkTypeTray.Enable(bStatus);

		bool bTrayStatus = chkTypeTray.GetState();
		chkTypeClist.Enable(bStatus && bTrayStatus);
		chkTypeBalloon.Enable(bStatus && bTrayStatus);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionsTabDlg : public CDlgBase
{
	CCtrlCheck m_chkTabs, m_chkTabsBottom, m_chkTabsClose, m_chkTabsRestore;

public:
	COptionsTabDlg() :
		CDlgBase(g_hInst, IDD_OPT_TABS),
		m_chkTabs(this, IDC_USETABS),
		m_chkTabsBottom(this, IDC_TABSBOTTOM),
		m_chkTabsClose(this, IDC_CLOSETABS),
		m_chkTabsRestore(this, IDC_RESTORETABS)
	{
		m_chkTabs.OnChange = Callback(this, &COptionsTabDlg::onChange_Tabs);
	}

	virtual void OnInitDialog() override
	{
		m_chkTabs.SetState(db_get_b(0, CHAT_MODULE, "Tabs", 1));
		m_chkTabsBottom.SetState(db_get_b(0, CHAT_MODULE, "TabBottom", 1));
		m_chkTabsClose.SetState(db_get_b(0, CHAT_MODULE, "TabCloseOnDblClick", 1));
		m_chkTabsRestore.SetState(db_get_b(0, CHAT_MODULE, "TabRestore", 1));
		onChange_Tabs(&m_chkTabs);
	}

	virtual void OnApply() override
	{
		BYTE bOldValue = db_get_b(0, CHAT_MODULE, "Tabs", 1);

		db_set_b(0, CHAT_MODULE, "Tabs", m_chkTabs.GetState());
		db_set_b(0, CHAT_MODULE, "TabBottom", m_chkTabsBottom.GetState());
		db_set_b(0, CHAT_MODULE, "TabCloseOnDblClick", m_chkTabsClose.GetState());
		db_set_b(0, CHAT_MODULE, "TabRestore", m_chkTabsRestore.GetState());

		pci->ReloadSettings();

		if (bOldValue != db_get_b(0, CHAT_MODULE, "Tabs", 1)) {
			pci->SM_BroadcastMessage(nullptr, WM_CLOSE, 0, 1, FALSE);
			g_Settings.bTabsEnable = db_get_b(0, CHAT_MODULE, "Tabs", 1) != 0;
		}
		else Chat_UpdateOptions();
	}

	void onChange_Tabs(CCtrlCheck *pCheck)
	{
		bool bEnabled = pCheck->GetState() != 0;
		m_chkTabsBottom.Enable(bEnabled);
		m_chkTabsClose.Enable(bEnabled);
		m_chkTabsRestore.Enable(bEnabled);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

static int OptInitialise(WPARAM wParam, LPARAM)
{
	OPTIONSDIALOGPAGE odp = {};
	odp.position = 910000000;
	odp.szTab.a = LPGEN("Messaging");
	odp.flags = ODPF_BOLDGROUPS;

	odp.szTitle.a = LPGEN("Message sessions");
	odp.pDialog = new COptionMainDlg();
	Options_AddPage(wParam, &odp);

	odp.szTab.a = LPGEN("Messaging log");
	odp.pDialog = new COptionLogDlg();
	Options_AddPage(wParam, &odp);

	odp.szTab.a = LPGEN("Typing notify");
	odp.pDialog = new COptionTypingDlg();
	Options_AddPage(wParam, &odp);

	odp.szTab.a = LPGEN("Tabs");
	odp.pDialog = new COptionsTabDlg();
	Options_AddPage(wParam, &odp);

	ChatOptionsInitialize(wParam);
	return 0;
}

void InitOptions(void)
{
	HookEvent(ME_OPT_INITIALISE, OptInitialise);
}
