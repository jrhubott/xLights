#include "BufferSizeDialog.h"

//(*InternalHeaders(BufferSizeDialog)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(BufferSizeDialog)
const long BufferSizeDialog::ID_STATICTEXT1 = wxNewId();
const long BufferSizeDialog::ID_SPINCTRL1 = wxNewId();
const long BufferSizeDialog::ID_STATICTEXT2 = wxNewId();
const long BufferSizeDialog::ID_SPINCTRL2 = wxNewId();
const long BufferSizeDialog::ID_STATICTEXT3 = wxNewId();
const long BufferSizeDialog::ID_SPINCTRL3 = wxNewId();
const long BufferSizeDialog::ID_STATICTEXT4 = wxNewId();
const long BufferSizeDialog::ID_SPINCTRL4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(BufferSizeDialog,wxDialog)
	//(*EventTable(BufferSizeDialog)
	//*)
END_EVENT_TABLE()

BufferSizeDialog::BufferSizeDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(BufferSizeDialog)
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, id, _("Buffer Size"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Top"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrl_Top = new wxSpinCtrl(this, ID_SPINCTRL1, _T("100"), wxDefaultPosition, wxDefaultSize, 0, 1, 200, 100, _T("ID_SPINCTRL1"));
	SpinCtrl_Top->SetValue(_T("100"));
	FlexGridSizer2->Add(SpinCtrl_Top, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Left"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrl_Left = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, 0, -100, 99, 0, _T("ID_SPINCTRL2"));
	SpinCtrl_Left->SetValue(_T("0"));
	FlexGridSizer2->Add(SpinCtrl_Left, 1, wxALL|wxEXPAND, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Bottom"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrl_Bottom = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxDefaultPosition, wxDefaultSize, 0, -100, 99, 0, _T("ID_SPINCTRL3"));
	SpinCtrl_Bottom->SetValue(_T("0"));
	FlexGridSizer2->Add(SpinCtrl_Bottom, 1, wxALL|wxEXPAND, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Right"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer2->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SpinCtrl_Right = new wxSpinCtrl(this, ID_SPINCTRL4, _T("100"), wxDefaultPosition, wxDefaultSize, 0, 1, 200, 100, _T("ID_SPINCTRL4"));
	SpinCtrl_Right->SetValue(_T("100"));
	FlexGridSizer2->Add(SpinCtrl_Right, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	FlexGridSizer1->Add(StdDialogButtonSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&BufferSizeDialog::OnSpinCtrl_TopChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&BufferSizeDialog::OnSpinCtrl_LeftChange);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&BufferSizeDialog::OnSpinCtrl_BottomChange);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&BufferSizeDialog::OnSpinCtrl_RightChange);
	//*)
}

BufferSizeDialog::~BufferSizeDialog()
{
	//(*Destroy(BufferSizeDialog)
	//*)
}

void BufferSizeDialog::SetSizes(int top, int left, int bottom, int right)
{
    SpinCtrl_Top->SetValue(top);
    SpinCtrl_Bottom->SetValue(bottom);
    if (SpinCtrl_Top->GetValue() <= SpinCtrl_Bottom->GetValue()) SpinCtrl_Bottom->SetValue(SpinCtrl_Bottom->GetValue() - 1);

    SpinCtrl_Left->SetValue(left);
    SpinCtrl_Right->SetValue(right);
    if (SpinCtrl_Left->GetValue() >= SpinCtrl_Right->GetValue()) SpinCtrl_Right->SetValue(SpinCtrl_Left->GetValue() + 1);
}

void BufferSizeDialog::OnSpinCtrl_TopChange(wxSpinEvent& event)
{
    if (SpinCtrl_Top->GetValue() <= SpinCtrl_Bottom->GetValue()) SpinCtrl_Bottom->SetValue(SpinCtrl_Top->GetValue() - 1);
}

void BufferSizeDialog::OnSpinCtrl_LeftChange(wxSpinEvent& event)
{
    if (SpinCtrl_Left->GetValue() >= SpinCtrl_Right->GetValue()) SpinCtrl_Right->SetValue(SpinCtrl_Left->GetValue() + 1);
}

void BufferSizeDialog::OnSpinCtrl_BottomChange(wxSpinEvent& event)
{
    if (SpinCtrl_Top->GetValue() <= SpinCtrl_Bottom->GetValue()) SpinCtrl_Top->SetValue(SpinCtrl_Bottom->GetValue() + 1);
}

void BufferSizeDialog::OnSpinCtrl_RightChange(wxSpinEvent& event)
{
    if (SpinCtrl_Left->GetValue() >= SpinCtrl_Right->GetValue()) SpinCtrl_Left->SetValue(SpinCtrl_Right->GetValue() - 1);
}
