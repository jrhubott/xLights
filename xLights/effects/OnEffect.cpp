#include "OnEffect.h"
#include "OnPanel.h"

#include "../sequencer/Effect.h"
#include "../RenderBuffer.h"
#include "../UtilClasses.h"
#include "../xLightsMain.h" //xLightsFrame
#include "../models/DmxModel.h"

static const std::string TEXTCTRL_Eff_On_Start("TEXTCTRL_Eff_On_Start");
static const std::string TEXTCTRL_Eff_On_End("TEXTCTRL_Eff_On_End");
static const std::string CHECKBOX_On_Shimmer("CHECKBOX_On_Shimmer");
static const std::string TEXTCTRL_On_Cycles("TEXTCTRL_On_Cycles");

#include "../../include/On.xpm"

#include <sstream>


OnEffect::OnEffect(int i) : RenderableEffect(i, "On", On, On, On, On, On)
{
    //ctor
}

OnEffect::~OnEffect()
{
    //dtor
}

wxPanel *OnEffect::CreatePanel(wxWindow *parent) {
    return new OnPanel(parent);
}

void OnEffect::SetDefaultParameters(Model *cls) {
    OnPanel *p = (OnPanel*)panel;
    p->CheckBoxShimmer->SetValue(false);
    p->TextCtrlStart->SetValue("100");
    p->TextCtrlEnd->SetValue("100");
    p->TextCtrlCycles->SetValue("1.0");
    
    
    p->BitmapButton_On_Transparency->SetActive(false);
    
    SetSliderValue(p->Slider_On_Transparency, 0);
}
std::string OnEffect::GetEffectString() {
    OnPanel *p = (OnPanel*)panel;
    std::stringstream ret;
    if (100 != p->SliderStart->GetValue()) {
        ret << "E_TEXTCTRL_Eff_On_Start=";
        ret << p->TextCtrlStart->GetValue().ToStdString();
        ret << ",";
    }
    if (100 != p->SliderEnd->GetValue()) {
        ret << "E_TEXTCTRL_Eff_On_End=";
        ret << p->TextCtrlEnd->GetValue().ToStdString();
        ret << ",";
    }
    if (10 != p->SliderCycles->GetValue()) {
        ret << "E_TEXTCTRL_On_Cycles=";
        ret << p->TextCtrlCycles->GetValue().ToStdString();
        ret << ",";
    }
    if (p->CheckBoxShimmer->GetValue()) {
        ret << "E_CHECKBOX_On_Shimmer=1,";
    }
    if (p->BitmapButton_On_Transparency->GetValue()->IsActive()) {
        ret << "E_VALUECURVE_On_Transparency=";
        ret << p->BitmapButton_On_Transparency->GetValue()->Serialise();
        ret << ",";
    } else if (p->Slider_On_Transparency->GetValue() > 0) {
        ret << "E_TEXTCTRL_On_Transparency=";
        ret << p->TextCtrlOnTransparency->GetValue();
        ret << ",";
    }
    return ret.str();
}


void GetOnEffectColors(const Effect *e, xlColor &start, xlColor &end) {
    int starti = e->GetSettings().GetInt("E_TEXTCTRL_Eff_On_Start", 100);
    int endi = e->GetSettings().GetInt("E_TEXTCTRL_Eff_On_End", 100);
    xlColor newcolor;
    newcolor = e->GetPalette()[0];
    if (starti == 100 && endi == 100) {
        start = end = newcolor;
    } else {
        HSVValue hsv = newcolor.asHSV();
        hsv.value = (hsv.value * starti) / 100;
        start = hsv;
        hsv = newcolor.asHSV();
        hsv.value = (hsv.value * endi) / 100;
        end = hsv;
    }
}
int OnEffect::DrawEffectBackground(const Effect *e, int x1, int y1, int x2, int y2,
                                   DrawGLUtils::xlVertexColorAccumulator &bg) {
    if (e->HasBackgroundDisplayList()) {
        DrawGLUtils::DrawDisplayList(x1, y1, x2-x1, y2-y1, e->GetBackgroundDisplayList(), bg);
        return e->GetBackgroundDisplayList().iconSize;
    }
    xlColor start;
    xlColor end;
    GetOnEffectColors(e, start, end);
    bg.AddVertex(x1, y1, start);
    bg.AddVertex(x1, y2, start);
    bg.AddVertex(x2, y2, end);

    bg.AddVertex(x2, y2, end);
    bg.AddVertex(x2, y1, end);
    bg.AddVertex(x1, y1, start);
    return 2;
}

void OnEffect::RemoveDefaults(const std::string &version, Effect *effect) {
    SettingsMap &settingsMap = effect->GetSettings();
    if (settingsMap.Get("E_TEXTCTRL_Eff_On_Start", "") == "100") {
        settingsMap.erase("E_TEXTCTRL_Eff_On_Start");
    }
    if (settingsMap.Get("E_TEXTCTRL_Eff_On_End", "") == "100") {
        settingsMap.erase("E_TEXTCTRL_Eff_On_End");
    }
    if (settingsMap.Get("E_CHECKBOX_On_Shimmer", "") == "0") {
        settingsMap.erase("E_CHECKBOX_On_Shimmer");
    }
    if (settingsMap.Get("E_TEXTCTRL_On_Cycles", "") == "1.0") {
        settingsMap.erase("E_TEXTCTRL_On_Cycles");
    }
    RenderableEffect::RemoveDefaults(version, effect);
}


void OnEffect::Render(Effect *eff, const SettingsMap &SettingsMap, RenderBuffer &buffer) {
    int start = SettingsMap.GetInt(TEXTCTRL_Eff_On_Start, 100);
    int end = SettingsMap.GetInt(TEXTCTRL_Eff_On_End, 100);
    bool shimmer = SettingsMap.GetInt(CHECKBOX_On_Shimmer, 0);
    float cycles = SettingsMap.GetDouble(TEXTCTRL_On_Cycles, 1.0);
    int x,y;
    int cidx = 0;
    if (shimmer) {
        int tot = buffer.curPeriod - buffer.curEffStartPer;
        if (tot % 2) {
            if (buffer.palette.Size() <= 1) {
                return;
            }
            cidx = 1;
        }
    }

    bool spatialcolour = buffer.palette.IsSpatial(cidx);

    double adjust = buffer.GetEffectTimeIntervalPosition(cycles);

    xlColor color;
    if (start == 100 && end == 100) {
        buffer.palette.GetColor(cidx, color);
    } else {
        HSVValue hsv;
        buffer.palette.GetHSV(cidx,hsv);
        double d = adjust;
        d = start + (end - start) * d;
        d = d / 100.0;
        hsv.value = hsv.value * d;
        color = hsv;
    }
    
    int transparency = GetValueCurveInt("On_Transparency", 0, SettingsMap, adjust);
    if (transparency) {
        transparency *= 255;
        transparency /= 100;
        color.alpha = 255 - transparency;
    }

    ///////////////////////// DMX Support ////////////////////////
    // if the model is a DMX model this will write the color into
    // the proper red, green, and blue channels.
    //////////////////////////////////////////////////////////////
    if (buffer.cur_model != "") {
        Model* model_info = buffer.frame->AllModels[buffer.cur_model];
        if (model_info != nullptr) {
            if( model_info->GetDisplayAs() == "DMX" ) {
                xlColor c;
                DmxModel* dmx = (DmxModel*)model_info;
                int red_channel = dmx->GetRedChannel();
                int grn_channel = dmx->GetGreenChannel();
                int blu_channel = dmx->GetBlueChannel();
                if( red_channel != 0 ) {
                    c.red = color.red;
                    c.green = color.red;
                    c.blue = color.red;
                    buffer.SetPixel(red_channel-1, 0, c);
                }
                if( grn_channel != 0 ) {
                    c.red = color.green;
                    c.green = color.green;
                    c.blue = color.green;
                    buffer.SetPixel(grn_channel-1, 0, c);
                }
                if( blu_channel != 0 ) {
                    c.red = color.blue;
                    c.green = color.blue;
                    c.blue = color.blue;
                    buffer.SetPixel(blu_channel-1, 0, c);
                }
                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////
    ///////////////////// End DMX Support ////////////////////////
    //////////////////////////////////////////////////////////////

    //Every Node set to selected color
    for (x=0; x<buffer.BufferWi; x++)
    {
        for (y=0; y<buffer.BufferHt; y++)
        {
            if (spatialcolour)
            {
                buffer.palette.GetSpatialColor(cidx, (float)x / (float)buffer.BufferWi, (float)y / (float)buffer.BufferHt, color);
                if (start == 100 && end == 100) {
                }
                else {
                    HSVValue hsv = color.asHSV();
                    double d = adjust;
                    d = start + (end - start) * d;
                    d = d / 100.0;
                    hsv.value = hsv.value * d;
                    color = hsv;
                }
                if (transparency) {
                    color.alpha = 255 - transparency;
                }
            }

            buffer.SetPixel(x,y,color);
        }
    }

    if (shimmer || cycles != 1.0) {
        std::lock_guard<std::recursive_mutex> lock(eff->GetBackgroundDisplayList().lock);
        eff->GetBackgroundDisplayList().resize((buffer.curEffEndPer - buffer.curEffStartPer + 1) * 6);
        buffer.CopyPixelsToDisplayListX(eff, 0, 0, 0);
    } else if (buffer.needToInit) {
        std::lock_guard<std::recursive_mutex> lock(eff->GetBackgroundDisplayList().lock);
        eff->GetBackgroundDisplayList().resize(6);
        if (start == 100 && end == 100) {
            buffer.palette.GetColor(0, color);
            buffer.SetDisplayListHRect(eff, 0, 0.0, 0.0, 1.0, 1.0, color, color);
        } else {
            HSVValue hsv;
            buffer.palette.GetHSV(cidx,hsv);
            hsv.value = hsv.value * start / 100.0;
            color = hsv;

            buffer.palette.GetHSV(cidx,hsv);
            hsv.value = hsv.value * end / 100.0;
            buffer.SetDisplayListHRect(eff, 0, 0.0, 0.0, 1.0, 1.0, color, xlColor(hsv));
        }
        buffer.needToInit = false;
    }
}
