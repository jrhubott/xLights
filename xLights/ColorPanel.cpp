#include "ColorPanel.h"
#include "xLightsMain.h"

#include "../include/padlock16x16-green.xpm" //-DJ
#include "../include/padlock16x16-red.xpm" //-DJ
#include "../include/padlock16x16-blue.xpm" //-DJ


class xLightsFrame;
//(*InternalHeaders(ColorPanel)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(ColorPanel)
const long ColorPanel::ID_CHECKBOX_Palette1 = wxNewId();
const long ColorPanel::ID_CHECKBOX_Palette2 = wxNewId();
const long ColorPanel::ID_CHECKBOX_Palette3 = wxNewId();
const long ColorPanel::ID_CHECKBOX_Palette4 = wxNewId();
const long ColorPanel::ID_CHECKBOX_Palette5 = wxNewId();
const long ColorPanel::ID_CHECKBOX_Palette6 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette1 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette2 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette3 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette4 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette5 = wxNewId();
const long ColorPanel::ID_BUTTON_Palette6 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette1 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette2 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette3 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette4 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette5 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_BUTTON_Palette6 = wxNewId();
const long ColorPanel::ID_STATICTEXT24 = wxNewId();
const long ColorPanel::ID_SLIDER_SparkleFrequency = wxNewId();
const long ColorPanel::ID_TEXTCTRL5 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_SLIDER_SparkleFrequency = wxNewId();
const long ColorPanel::ID_STATICTEXT127 = wxNewId();
const long ColorPanel::ID_SLIDER_Brightness = wxNewId();
const long ColorPanel::ID_TEXTCTRL6 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_SLIDER_Brightness = wxNewId();
const long ColorPanel::ID_STATICTEXT128 = wxNewId();
const long ColorPanel::ID_SLIDER_Contrast = wxNewId();
const long ColorPanel::ID_TEXTCTRL7 = wxNewId();
const long ColorPanel::ID_BITMAPBUTTON_SLIDER_Contrast = wxNewId();
const long ColorPanel::ID_SCROLLED_ColorScroll = wxNewId();
const long ColorPanel::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ColorPanel,wxPanel)
	//(*EventTable(ColorPanel)
	//*)
END_EVENT_TABLE()

ColorPanel::ColorPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(ColorPanel)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(1, 1, 0, 0);
	Panel_Sizer = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Panel_Sizer->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	ColorScrollWindow = new wxScrolledWindow(Panel_Sizer, ID_SCROLLED_ColorScroll, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLED_ColorScroll"));
	ColorScrollWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer4 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer5->AddGrowableCol(0);
	FlexGridSizer5->AddGrowableCol(1);
	FlexGridSizer5->AddGrowableCol(2);
	FlexGridSizer5->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer_Palette = new wxFlexGridSizer(0, 6, 0, 0);
	FlexGridSizer_Palette->AddGrowableCol(0);
	FlexGridSizer_Palette->AddGrowableCol(1);
	FlexGridSizer_Palette->AddGrowableCol(2);
	FlexGridSizer_Palette->AddGrowableCol(3);
	FlexGridSizer_Palette->AddGrowableCol(4);
	FlexGridSizer_Palette->AddGrowableCol(5);
	CheckBox_Palette1 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette1"));
	CheckBox_Palette1->SetValue(true);
	FlexGridSizer_Palette->Add(CheckBox_Palette1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	CheckBox_Palette2 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette2"));
	CheckBox_Palette2->SetValue(true);
	FlexGridSizer_Palette->Add(CheckBox_Palette2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	CheckBox_Palette3 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette3"));
	CheckBox_Palette3->SetValue(false);
	FlexGridSizer_Palette->Add(CheckBox_Palette3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	CheckBox_Palette4 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette4"));
	CheckBox_Palette4->SetValue(false);
	FlexGridSizer_Palette->Add(CheckBox_Palette4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	CheckBox_Palette5 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette5"));
	CheckBox_Palette5->SetValue(false);
	FlexGridSizer_Palette->Add(CheckBox_Palette5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	CheckBox_Palette6 = new wxCheckBox(ColorScrollWindow, ID_CHECKBOX_Palette6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_Palette6"));
	CheckBox_Palette6->SetValue(false);
	FlexGridSizer_Palette->Add(CheckBox_Palette6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	Button_Palette1 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette1, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette1"));
	Button_Palette1->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette1, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	Button_Palette2 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette2, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette2"));
	Button_Palette2->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette2, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	Button_Palette3 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette3, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette3"));
	Button_Palette3->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette3, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	Button_Palette4 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette4, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette4"));
	Button_Palette4->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette4, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	Button_Palette5 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette5, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette5"));
	Button_Palette5->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette5, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	Button_Palette6 = new wxBitmapButton(ColorScrollWindow, ID_BUTTON_Palette6, wxNullBitmap, wxDefaultPosition, wxSize(21,21), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON_Palette6"));
	Button_Palette6->SetDefault();
	FlexGridSizer_Palette->Add(Button_Palette6, 1, wxALIGN_LEFT|wxALIGN_TOP, 2);
	BitmapButton_Palette1 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette1, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette1"));
	BitmapButton_Palette1->SetDefault();
	BitmapButton_Palette1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BitmapButton_Palette2 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette2, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette2"));
	BitmapButton_Palette2->SetDefault();
	BitmapButton_Palette2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BitmapButton_Palette3 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette3, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette3"));
	BitmapButton_Palette3->SetDefault();
	BitmapButton_Palette3->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BitmapButton_Palette4 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette4, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette4"));
	BitmapButton_Palette4->SetDefault();
	BitmapButton_Palette4->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BitmapButton_Palette5 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette5, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette5"));
	BitmapButton_Palette5->SetDefault();
	BitmapButton_Palette5->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BitmapButton_Palette6 = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_BUTTON_Palette6, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_BUTTON_Palette6"));
	BitmapButton_Palette6->SetDefault();
	BitmapButton_Palette6->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	FlexGridSizer_Palette->Add(BitmapButton_Palette6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	FlexGridSizer5->Add(FlexGridSizer_Palette, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	FlexGridSizer5->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4->Add(FlexGridSizer5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2 = new wxFlexGridSizer(0, 4, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText22 = new wxStaticText(ColorScrollWindow, ID_STATICTEXT24, _("Sparkles"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
	FlexGridSizer2->Add(StaticText22, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_SparkleFrequency = new wxSlider(ColorScrollWindow, ID_SLIDER_SparkleFrequency, 200, 10, 200, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER_SparkleFrequency"));
	FlexGridSizer2->Add(Slider_SparkleFrequency, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	txtCtrlSparkleFreq = new wxTextCtrl(ColorScrollWindow, ID_TEXTCTRL5, _("200"), wxDefaultPosition, wxSize(32,20), wxTE_PROCESS_ENTER|wxTAB_TRAVERSAL, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	FlexGridSizer2->Add(txtCtrlSparkleFreq, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BitmapButton_SparkleFrequency = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_SLIDER_SparkleFrequency, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_SparkleFrequency"));
	BitmapButton_SparkleFrequency->SetDefault();
	BitmapButton_SparkleFrequency->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	BitmapButton_SparkleFrequency->SetToolTip(_("Lock/Unlock. If Locked then a \"Create Random Effects\" will NOT change this value."));
	FlexGridSizer2->Add(BitmapButton_SparkleFrequency, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText126 = new wxStaticText(ColorScrollWindow, ID_STATICTEXT127, _("Brightness"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT127"));
	FlexGridSizer2->Add(StaticText126, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Brightness = new wxSlider(ColorScrollWindow, ID_SLIDER_Brightness, 100, 0, 400, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Brightness"));
	FlexGridSizer2->Add(Slider_Brightness, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	txtCtlBrightness = new wxTextCtrl(ColorScrollWindow, ID_TEXTCTRL6, _("100"), wxDefaultPosition, wxSize(32,20), wxTE_PROCESS_ENTER|wxTAB_TRAVERSAL, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	FlexGridSizer2->Add(txtCtlBrightness, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BitmapButton_Brightness = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_SLIDER_Brightness, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_Brightness"));
	BitmapButton_Brightness->SetDefault();
	BitmapButton_Brightness->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	BitmapButton_Brightness->SetToolTip(_("Lock/Unlock. If Locked then a \"Create Random Effects\" will NOT change this value."));
	FlexGridSizer2->Add(BitmapButton_Brightness, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText127 = new wxStaticText(ColorScrollWindow, ID_STATICTEXT128, _("Contrast"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT128"));
	FlexGridSizer2->Add(StaticText127, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	Slider_Contrast = new wxSlider(ColorScrollWindow, ID_SLIDER_Contrast, 0, 0, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER_Contrast"));
	FlexGridSizer2->Add(Slider_Contrast, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	txtCtlContrast = new wxTextCtrl(ColorScrollWindow, ID_TEXTCTRL7, _("0"), wxDefaultPosition, wxSize(32,20), wxTE_PROCESS_ENTER|wxTAB_TRAVERSAL, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	FlexGridSizer2->Add(txtCtlContrast, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BitmapButton_Contrast = new wxBitmapButton(ColorScrollWindow, ID_BITMAPBUTTON_SLIDER_Contrast, padlock16x16_blue_xpm, wxDefaultPosition, wxSize(13,13), wxBU_AUTODRAW|wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON_SLIDER_Contrast"));
	BitmapButton_Contrast->SetDefault();
	BitmapButton_Contrast->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	BitmapButton_Contrast->SetToolTip(_("Lock/Unlock. If Locked then a \"Create Random Effects\" will NOT change this value."));
	FlexGridSizer2->Add(BitmapButton_Contrast, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	ColorScrollWindow->SetSizer(FlexGridSizer4);
	FlexGridSizer4->Fit(ColorScrollWindow);
	FlexGridSizer4->SetSizeHints(ColorScrollWindow);
	FlexGridSizer3->Add(ColorScrollWindow, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel_Sizer->SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(Panel_Sizer);
	FlexGridSizer3->SetSizeHints(Panel_Sizer);
	FlexGridSizer1->Add(Panel_Sizer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_CHECKBOX_Palette1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_CHECKBOX_Palette2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_CHECKBOX_Palette3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_CHECKBOX_Palette4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_CHECKBOX_Palette5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_CHECKBOX_Palette6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ColorPanel::OnCheckBox_PaletteClick);
	Connect(ID_BUTTON_Palette1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BUTTON_Palette2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BUTTON_Palette3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BUTTON_Palette4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BUTTON_Palette5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BUTTON_Palette6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnButton_PaletteNumberClick);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette1Click);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette2Click);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette3Click);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette4Click);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette5Click);
	Connect(ID_BITMAPBUTTON_BUTTON_Palette6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ColorPanel::OnBitmapButton_Palette6Click);
	Connect(wxEVT_PAINT,(wxObjectEventFunction)&ColorPanel::OnPaint);
	Connect(wxEVT_SIZE,(wxObjectEventFunction)&ColorPanel::OnResize);
	//*)
    
    
    SetDefaultPalette();
}

ColorPanel::~ColorPanel()
{
	//(*Destroy(ColorPanel)
	//*)
}


void ColorPanel::SetButtonColor(wxBitmapButton* btn, const wxColour* c)
{
    btn->SetBackgroundColour(*c);
    int test=c->Red()*0.299 + c->Green()*0.587 + c->Blue()*0.114;
    btn->SetForegroundColour(test < 186 ? *wxWHITE : *wxBLACK);
    
    wxImage image(15, 15);
    image.SetRGB(wxRect(0, 0, 15, 15),
                 c->Red(), c->Green(), c->Blue());
    wxBitmap bmp(image);
    
    btn->SetBitmap(bmp);
}

wxColour ColorPanel::GetPaletteColor(int idx)
{
    switch (idx)
    {
    case 1:
        return Button_Palette1->GetBackgroundColour();
    case 2:
        return Button_Palette2->GetBackgroundColour();
    case 3:
        return Button_Palette3->GetBackgroundColour();
    case 4:
        return Button_Palette4->GetBackgroundColour();
    case 5:
        return Button_Palette5->GetBackgroundColour();
    case 6:
        return Button_Palette6->GetBackgroundColour();
    }
    return *wxBLACK;
}

// idx is 1-6
void ColorPanel::SetPaletteColor(int idx, const wxColour* c)
{
    switch (idx)
    {
    case 1:
        SetButtonColor(Button_Palette1,c);
        break;
    case 2:
        SetButtonColor(Button_Palette2,c);
        break;
    case 3:
        SetButtonColor(Button_Palette3,c);
        break;
    case 4:
        SetButtonColor(Button_Palette4,c);
        break;
    case 5:
        SetButtonColor(Button_Palette5,c);
        break;
    case 6:
        SetButtonColor(Button_Palette6,c);
        break;
    }
}

//allow array-like access to checkboxes also -DJ
wxCheckBox* ColorPanel::GetPaletteCheckbox(int idx)
{
    switch (idx)   // idx is 1-6
    {
    case 1:
        return CheckBox_Palette1;
    case 2:
        return CheckBox_Palette2;
    case 3:
        return CheckBox_Palette3;
    case 4:
        return CheckBox_Palette4;
    case 5:
        return CheckBox_Palette5;
    case 6:
        return CheckBox_Palette6;
    }
    return CheckBox_Palette1; //0;
}

//allow array-like access to buttons also -DJ
wxButton* ColorPanel::GetPaletteButton(int idx)
{
    switch (idx)   // idx is 1-6
    {
    case 1:
        return Button_Palette1;
    case 2:
        return Button_Palette2;
    case 3:
        return Button_Palette3;
    case 4:
        return Button_Palette4;
    case 5:
        return Button_Palette5;
    case 6:
        return Button_Palette6;
    }
    return Button_Palette1; //0;
}

void ColorPanel::SetDefaultPalette()
{
    SetButtonColor(Button_Palette1,wxRED);
    SetButtonColor(Button_Palette2,wxGREEN);
    SetButtonColor(Button_Palette3,wxBLUE);
    SetButtonColor(Button_Palette4,wxYELLOW);
    SetButtonColor(Button_Palette5,wxWHITE);
    SetButtonColor(Button_Palette6,wxBLACK);
}




void ColorPanel::OnCheckBox_PaletteClick(wxCommandEvent& event)
{
    PaletteChanged=true;
}

void ColorPanel::OnBitmapButton_Palette2Click(wxCommandEvent& event)
{
}

void ColorPanel::OnBitmapButton_Palette1Click(wxCommandEvent& event)
{
}

void ColorPanel::OnBitmapButton_Palette3Click(wxCommandEvent& event)
{
}

void ColorPanel::OnBitmapButton_Palette4Click(wxCommandEvent& event)
{
}

void ColorPanel::OnBitmapButton_Palette5Click(wxCommandEvent& event)
{
}

void ColorPanel::OnBitmapButton_Palette6Click(wxCommandEvent& event)
{
}

void ColorPanel::OnButton_PaletteNumberClick(wxCommandEvent& event)
{
    wxBitmapButton* w=(wxBitmapButton*)event.GetEventObject();
    wxColour color = w->GetBackgroundColour();
    colorData.SetColour(color);
    wxColourDialog dialog(this, &colorData);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour color = retData.GetColour();
        SetButtonColor(w, &color);
        PaletteChanged=true;
    }

}

void ColorPanel::OnResize(wxSizeEvent& event)
{
    wxSize s = GetSize();
    Panel_Sizer->SetSize(s);
    Panel_Sizer->SetMinSize(s);
    Panel_Sizer->SetMaxSize(s);
    Panel_Sizer->Refresh();

    ColorScrollWindow->SetSize(s);
    ColorScrollWindow->SetMinSize(s);
    ColorScrollWindow->SetMaxSize(s);

    ColorScrollWindow->FitInside();
    ColorScrollWindow->SetScrollRate(5, 5);
    ColorScrollWindow->Refresh();

}

void ColorPanel::OnPaint(wxPaintEvent& event)
{
}
