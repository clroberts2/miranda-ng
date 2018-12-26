#include "stdafx.h"

extern BOOL bPopupService;

struct {
	int idc;
	int flag;
}
static const checkboxes[] = {
	{ IDC_COPYID, VF_CID },
	{ IDC_STATUSMSG, VF_STAT },
	{ IDC_COPYIP, VF_CIP },
	{ IDC_COPYMIRVER, VF_CMV },
	{ IDC_VIS, VF_VS },
	{ IDC_SHOWALPHAICONS, VF_SAI },
	{ IDC_HIDE, VF_HFL },
	{ IDC_IGNORE, VF_IGN },
	{ IDC_IGNOREHIDE, VF_IGNH },
	{ IDC_PROTOS, VF_PROTO },
	{ IDC_ADDED, VF_ADD },
	{ IDC_AUTHREQ, VF_REQ },
	{ IDC_SHOWID, VF_SHOWID },
	{ IDC_COPYIDNAME, VF_CIDN },
	{ IDC_RECVFILES, VF_RECV },
	{ IDC_SMNAME, VF_SMNAME },
	{ IDC_TRIMID, VF_TRIMID }
};

COptDialog::COptDialog() :
	CDlgBase(g_plugin, IDD_OPTIONS),
	m_chkVis(this, IDC_VIS),
	m_chkAlpha(this, IDC_SHOWALPHAICONS),
	m_chkHide(this, IDC_HIDE),
	m_chkIgnore(this, IDC_IGNORE),
	m_chkProtos(this, IDC_PROTOS),
	m_chkAdded(this, IDC_ADDED),
	m_chkAuthReq(this, IDC_AUTHREQ),
	m_chkRecvFiles(this, IDC_RECVFILES),
	m_chkCopyIP(this, IDC_COPYIP),
	m_chkCopyMirver(this, IDC_COPYMIRVER),
	m_chkStatusMsg(this, IDC_STATUSMSG),
	m_chkSMName(this, IDC_SMNAME),
	m_chkCopyID(this, IDC_COPYID),
	m_chkCopyIDName(this, IDC_COPYIDNAME),
	m_chkShowID(this, IDC_SHOWID),
	m_chkTrimID(this, IDC_TRIMID),
	m_lblHint(this, IDC_HINT1),
	m_chkIgnoreHide(this, IDC_IGNOREHIDE)
{
	m_flags = g_plugin.getDword("flags", vf_default);

	m_chkVis.OnChange = Callback(this, &COptDialog::OnVisChange);
	m_chkIgnore.OnChange = Callback(this, &COptDialog::OnIgnoreChange);
	m_chkCopyID.OnChange = Callback(this, &COptDialog::OnCopyIDChange);
	m_chkStatusMsg.OnChange = Callback(this, &COptDialog::OnStatusMsgChange);
	m_chkShowID.OnChange = Callback(this, &COptDialog::OnShowIDChange);
}

bool COptDialog::OnInitDialog()
{
	for (auto &it : checkboxes) {
		CCtrlCheck &item = *(CCtrlCheck*)FindControl(it.idc);
		item.SetState(m_flags & it.flag);
	}

	if (bPopupService) {
		for (int i = 0; i < 4; i++) {
			CCtrlCheck &item = *(CCtrlCheck*)FindControl(checkboxes[i].idc);
			CMStringW buffer;
			buffer.Format(L"%s *", item.GetText());
			item.SetText(buffer);
		}
	}
	else
		m_lblHint.Hide();

	EnableWindowChecks();
	return true;
}

bool COptDialog::OnApply()
{
	DWORD mod_flags = 0;

	for (auto &it : checkboxes) {
		CCtrlCheck &item = *(CCtrlCheck*)FindControl(it.idc);
		mod_flags |= item.GetState() ? it.flag : 0;
	}

	g_plugin.setDword("flags", mod_flags);
	return true;
}

void COptDialog::EnableWindowChecks()
{
	m_chkAlpha.Enable(m_chkVis.GetState());
	m_chkIgnoreHide.Enable(m_chkIgnore.GetState());
	m_chkCopyIDName.Enable(m_chkCopyID.GetState());
	m_chkShowID.Enable(m_chkCopyID.GetState());
	m_chkTrimID.Enable(m_chkCopyID.GetState() && m_chkShowID.GetState());
	m_chkSMName.Enable(m_chkStatusMsg.GetState());
}

void COptDialog::OnVisChange(CCtrlBase*)
{
	EnableWindowChecks();
}

void COptDialog::OnIgnoreChange(CCtrlBase*)
{
	EnableWindowChecks();
}

void COptDialog::OnCopyIDChange(CCtrlBase*)
{
	EnableWindowChecks();
}

void COptDialog::OnStatusMsgChange(CCtrlBase*)
{
	EnableWindowChecks();
}

void COptDialog::OnShowIDChange(CCtrlBase*)
{
	EnableWindowChecks();
}

int OptionsInit(WPARAM wparam, LPARAM)
{
	OPTIONSDIALOGPAGE odp = {};
	odp.szTitle.a = MODULENAME;
	odp.pDialog = new COptDialog;
	odp.szGroup.a = LPGEN("Customize");
	odp.flags = ODPF_BOLDGROUPS;
	g_plugin.addOptions(wparam, &odp);
	return 0;
}
