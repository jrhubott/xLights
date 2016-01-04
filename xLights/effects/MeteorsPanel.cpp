#include "MeteorsPanel.h"
#include "../../include/padlock16x16-blue.xpm"
#include "EffectPanelUtils.h"

//(*InternalHeaders(MeteorsPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/slider.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(MeteorsPanel)
const long MeteorsPanel::ID_STATICTEXT39 = wxNewId();
const long MeteorsPanel::ID_CHOICE_Meteors_Type = wxNewId();
const long MeteorsPanel::ID_BITMAPBUTTON_CHOICE_Meteors_Type = wxNewId();
const long MeteorsPanel::ID_STATICTEXT129 = wxNewId();
const long MeteorsPanel::ID_CHOICE_Meteors_Effect = wxNewId();
const long MeteorsPanel::ID_BITMAPBUTTON_CHOICE_Meteors_Effect = wxNewId();
const long MeteorsPanel::ID_STATICTEXT41 = wxNewId();
const long MeteorsPanel::ID_SLIDER_Meteors_Count = wxNewId();
const long MeteorsPanel::IDD_TEXTCTRL_Meteors_Count = wxNewId();
const long MeteorsPanel::ID_BITMAPBUTTON_SLIDER_Meteors_Count = wxNewId();
const long MeteorsPanel::ID_STATICTEXT43 = wxNewId();
const long MeteorsPanel::ID_SLIDER_Meteors_Length = wxNewId();
const long MeteorsPanel::IDD_TEXTCTRL_Meteors_Length = wxNewId();
const long MeteorsPanel::ID_BITMAPBUTTON_SLIDER_Meteors_Length = wxNewId();
const long MeteorsPanel::ID_STATICTEXT131 = wxNewId();
const long MeteorsPanel::ID_SLIDER_Meteors_Swirl_Intensity = wxNewId();
const long MeteorsPanel::IDD_TEXTCTRL_Meteors_Swirl_Intensity = wxNewId();
const long MeteorsPanel::ID_BITMAPBUTTON_SLIDER_Meteors_Swirl_Intensity = wxNewId();
const long MeteorsPanel::ID_SLIDER_Meteors_Speed = wxNewId();
const long MeteorsPanel::IDD_TEXTCTRL_Meteors_Speed = wxNewId();
//*)

BEGIN_EVENT_TABLE(MeteorsPanel,wxPanel)
	//(*EventTable(MeteorsPanel)
	//*)
END_EVENT_TABLE()

MeteorsPanel::MeteorsPanel(wxWindow* parent)
{
	//(*Initialize(MeteorsPanel)
	wxFlexGridSizer* FlexGridSizer41;
	wxChoice* Choice_Meteors_Effect;
	wxSlider* Slider19;
	wxTextCtrl* TextCtrl50;
	wxTextCtrl* TextCtrl52;
	wxTextCtrl* TextCtrl51;
	wxStaticText* StaticText4;
	wxTextCtrl* TextCtrl49;
	wxChoice* Choice_Meteors_Type;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	FlexGridSizer41 = new wxFlexGridSizer(0, 4, 0, 0);
	FlexGridSizer41->AddGrowableCol(1);
	StaticText39 = new wxStaticText(this, ID_STATICTEXT39, _("Colors"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT39"));
	FlexGridSizer41->Add(StaticText39, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Choice_Meteors_Type = new wxChoice(this, ID_CHOICE_Meteors_Type, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_Meteors_Type"));
	Choice_Meteors_Type->SetSelection( Choice_Meteors_Type->Append(_("Rainbow")) );
	Choice_Meteors_Type->Append(_("Range"));
	Choice_Meteors_Type->Append(_("Palette"));
	FlexGridSizer41->Add(Choice_Meteors_Type, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer41->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton_MeteorsType = new wxBitmapButton(this, ID_BITMAPBUTTON_CHOICE_Meteors_Type, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_CHOICE_Meteors_Type"));
	BitmapButton_MeteorsType->SetDefault();
	BitmapButton_MeteorsType->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer41->Add(BitmapButton_MeteorsType, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText128 = new wxStaticText(this, ID_STATICTEXT129, _("Effect"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT129"));
	FlexGridSizer41->Add(StaticText128, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Choice_Meteors_Effect = new wxChoice(this, ID_CHOICE_Meteors_Effect, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_Meteors_Effect"));
	Choice_Meteors_Effect->SetSelection( Choice_Meteors_Effect->Append(_("Down")) );
	Choice_Meteors_Effect->Append(_("Up"));
	Choice_Meteors_Effect->Append(_("Left"));
	Choice_Meteors_Effect->Append(_("Right"));
	Choice_Meteors_Effect->Append(_("Implode"));
	Choice_Meteors_Effect->Append(_("Explode"));
	Choice_Meteors_Effect->Append(_("Icicles"));
	Choice_Meteors_Effect->Append(_("Icicles + bkg"));
	FlexGridSizer41->Add(Choice_Meteors_Effect, 1, wxTOP|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer41->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton_MeteorsEffect = new wxBitmapButton(this, ID_BITMAPBUTTON_CHOICE_Meteors_Effect, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_CHOICE_Meteors_Effect"));
	BitmapButton_MeteorsEffect->SetDefault();
	BitmapButton_MeteorsEffect->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer41->Add(BitmapButton_MeteorsEffect, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText41 = new wxStaticText(this, ID_STATICTEXT41, _("Count"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT41"));
	FlexGridSizer41->Add(StaticText41, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Meteors_Count = new wxSlider(this, ID_SLIDER_Meteors_Count, 10, 1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Meteors_Count"));
	FlexGridSizer41->Add(Slider_Meteors_Count, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	TextCtrl49 = new wxTextCtrl(this, IDD_TEXTCTRL_Meteors_Count, _("10"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(20,-1)), 0, wxDefaultValidator, _T("IDD_TEXTCTRL_Meteors_Count"));
	TextCtrl49->SetMaxLength(4);
	FlexGridSizer41->Add(TextCtrl49, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton_MeteorsCount = new wxBitmapButton(this, ID_BITMAPBUTTON_SLIDER_Meteors_Count, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_Meteors_Count"));
	BitmapButton_MeteorsCount->SetDefault();
	BitmapButton_MeteorsCount->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer41->Add(BitmapButton_MeteorsCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText43 = new wxStaticText(this, ID_STATICTEXT43, _("Trail Length"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT43"));
	FlexGridSizer41->Add(StaticText43, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Meteors_Length = new wxSlider(this, ID_SLIDER_Meteors_Length, 25, 1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Meteors_Length"));
	FlexGridSizer41->Add(Slider_Meteors_Length, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	TextCtrl50 = new wxTextCtrl(this, IDD_TEXTCTRL_Meteors_Length, _("25"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(20,-1)), 0, wxDefaultValidator, _T("IDD_TEXTCTRL_Meteors_Length"));
	TextCtrl50->SetMaxLength(4);
	FlexGridSizer41->Add(TextCtrl50, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton_MeteorsLength = new wxBitmapButton(this, ID_BITMAPBUTTON_SLIDER_Meteors_Length, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_Meteors_Length"));
	BitmapButton_MeteorsLength->SetDefault();
	BitmapButton_MeteorsLength->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer41->Add(BitmapButton_MeteorsLength, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText130 = new wxStaticText(this, ID_STATICTEXT131, _("Swirl Intensity"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT131"));
	FlexGridSizer41->Add(StaticText130, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Meteors_Swirl_Intensity = new wxSlider(this, ID_SLIDER_Meteors_Swirl_Intensity, 0, 0, 20, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Meteors_Swirl_Intensity"));
	FlexGridSizer41->Add(Slider_Meteors_Swirl_Intensity, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	TextCtrl51 = new wxTextCtrl(this, IDD_TEXTCTRL_Meteors_Swirl_Intensity, _("0"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(20,-1)), 0, wxDefaultValidator, _T("IDD_TEXTCTRL_Meteors_Swirl_Intensity"));
	TextCtrl51->SetMaxLength(3);
	FlexGridSizer41->Add(TextCtrl51, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BitmapButton_MeteorsSwirlIntensity = new wxBitmapButton(this, ID_BITMAPBUTTON_SLIDER_Meteors_Swirl_Intensity, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_Meteors_Swirl_Intensity"));
	BitmapButton_MeteorsSwirlIntensity->SetDefault();
	BitmapButton_MeteorsSwirlIntensity->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer41->Add(BitmapButton_MeteorsSwirlIntensity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText4 = new wxStaticText(this, wxID_ANY, _("Speed"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	FlexGridSizer41->Add(StaticText4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	Slider19 = new wxSlider(this, ID_SLIDER_Meteors_Speed, 10, 0, 50, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Meteors_Speed"));
	FlexGridSizer41->Add(Slider19, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl52 = new wxTextCtrl(this, IDD_TEXTCTRL_Meteors_Speed, _("10"), wxDefaultPosition, wxDLG_UNIT(this,wxSize(20,-1)), 0, wxDefaultValidator, _T("IDD_TEXTCTRL_Meteors_Speed"));
	TextCtrl52->SetMaxLength(3);
	FlexGridSizer41->Add(TextCtrl52, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer41->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer41);
	FlexGridSizer41->Fit(this);
	FlexGridSizer41->SetSizeHints(this);

	Connect(ID_BITMAPBUTTON_CHOICE_Meteors_Type,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MeteorsPanel::OnLockButtonClick);
	Connect(ID_BITMAPBUTTON_CHOICE_Meteors_Effect,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MeteorsPanel::OnLockButtonClick);
	Connect(ID_SLIDER_Meteors_Count,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedTextCtrl);
	Connect(IDD_TEXTCTRL_Meteors_Count,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedSlider);
	Connect(ID_BITMAPBUTTON_SLIDER_Meteors_Count,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MeteorsPanel::OnLockButtonClick);
	Connect(ID_SLIDER_Meteors_Length,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedTextCtrl);
	Connect(IDD_TEXTCTRL_Meteors_Length,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedSlider);
	Connect(ID_BITMAPBUTTON_SLIDER_Meteors_Length,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MeteorsPanel::OnLockButtonClick);
	Connect(ID_SLIDER_Meteors_Swirl_Intensity,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedTextCtrl);
	Connect(IDD_TEXTCTRL_Meteors_Swirl_Intensity,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedSlider);
	Connect(ID_BITMAPBUTTON_SLIDER_Meteors_Swirl_Intensity,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MeteorsPanel::OnLockButtonClick);
	Connect(ID_SLIDER_Meteors_Speed,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedTextCtrl);
	Connect(IDD_TEXTCTRL_Meteors_Speed,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MeteorsPanel::UpdateLinkedSlider);
	//*)
    SetName("ID_PANEL_METEORS");
}

MeteorsPanel::~MeteorsPanel()
{
	//(*Destroy(MeteorsPanel)
	//*)
}

PANEL_EVENT_HANDLERS(MeteorsPanel)