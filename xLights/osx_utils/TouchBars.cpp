//
//  TouchBars.cpp
//  xLights

#include "TouchBars.h"
#include "../sequencer/MainSequencer.h"
#include "../effects/EffectManager.h"
#include "../effects/RenderableEffect.h"

#ifdef __WXOSX__
void *initializeTouchBarSuppor(wxWindow *w);
void setActiveTouchbar(void *controller, xlTouchBar *tb);
#else 
void *initializeTouchBarSuppor(wxWindow *w) { return nullptr;}
void setActiveTouchbar(void *controller, xlTouchBar *tb) {}
#endif


xlTouchBarSupport::xlTouchBarSupport() : window(nullptr), parent(nullptr), controller(nullptr) {
}
xlTouchBarSupport::~xlTouchBarSupport() {
}
void xlTouchBarSupport::Init(wxWindow *w) {
    controller = initializeTouchBarSuppor(w);
    if (controller) {
        parent = new wxPanel(w->GetParent());
        parent->Hide();
    }
}
void xlTouchBarSupport::SetActive(xlTouchBar *tb) {
    setActiveTouchbar(controller, tb);
    currentBar = tb;
}


wxControlTouchBarItem::wxControlTouchBarItem(wxWindow *c) : TouchBarItem(c->GetName().ToStdString()), control(c) {
    
}


xlTouchBar::xlTouchBar(xlTouchBarSupport &s) : support(s) {
}
xlTouchBar::~xlTouchBar() {
    for (int x = 0; x < items.size(); x++) {
        delete items[x];
    }
    items.clear();
}



EffectGridTouchBar::EffectGridTouchBar(xlTouchBarSupport &support, const EffectManager &m,
                                       MainSequencer *mainSequencer, ColorPanelTouchBar *cb)
: xlTouchBar(support), manager(m), colorBar(cb) {
    ButtonTouchBarItem *colorsButton = new ButtonTouchBarItem([support, cb]() {
        cb->SetActive();
    }, "NSTouchBarColorPickerFill", "Colors");
    items.push_back(colorsButton);
    
    for (auto it = manager.begin(); it != manager.end(); it++) {
        wxBitmap ico = (*it)->GetEffectIcon(16, false);
        
        wxButton *b = new wxButton(support.GetControlParent(), wxID_ANY,
                                   "",
                                   wxDefaultPosition,
                                   wxDefaultSize,
                                   0,
                                   wxDefaultValidator,
                                   (*it)->Name());
        b->SetBitmap(ico);
        b->Connect(wxEVT_BUTTON, (wxObjectEventFunction)&MainSequencer::TouchButtonEvent, nullptr, mainSequencer);
        
        items.push_back(new wxControlTouchBarItem(b));
    }
}
EffectGridTouchBar::~EffectGridTouchBar() {
}


void ColorPickerItem::SetColor(const wxBitmap &b, wxColor &c) {
    bmp = b;
    color = c;
}

void SliderItem::SetValue(int i) {
    value = i;
}


ColorPanelTouchBar::ColorPanelTouchBar(ColorChangedFunction f,
                                       SliderItemChangedFunction spark,
                                       xlTouchBarSupport &support)
    : xlTouchBar(support), colorCallback(f), sparkCallback(spark), inCallback(false)
{
    xlTouchBarSupport *sp = &support;
    ButtonTouchBarItem *doneButton = new ButtonTouchBarItem([sp, this]() {
        sp->SetActive(lastBar);
    }, "Done", "Done");
    doneButton->SetBackgroundColor(xlBLUE);
    items.push_back(doneButton);

    for (char x = '1'; x <= '8'; x++) {
        std::string name = "Color ";
        name += x;
        items.push_back(new ColorPickerItem([this, x](xlColor c) {
            this->inCallback = true;
            this->colorCallback(x - '1', c);
            this->inCallback = false;
        }, name));
    }
        
    items.push_back(new SliderItem([this](int i) {
        this->inCallback = true;
        this->sparkCallback(i);
        this->inCallback = false;
    }, "Sparkles", 0, 200));
}
ColorPanelTouchBar::~ColorPanelTouchBar() {
}
void ColorPanelTouchBar::SetActive() {
    lastBar = support.GetCurrentBar();
    support.SetActive(this);
}
void ColorPanelTouchBar::SetSparkles(int v) {
    if (inCallback) {
        return;
    }
    SliderItem *item = (SliderItem*)items[9];
    item->SetValue(v);
    if (support.IsActive(this)) {
        support.SetActive(this);
    }
}

void ColorPanelTouchBar::SetColor(int idx, const wxBitmap &bmp, wxColor &c) {
    if (inCallback) {
        return;
    }
    ColorPickerItem *item = (ColorPickerItem*)items[idx + 1];  //count for the Done button
    item->SetColor(bmp, c);
    if (support.IsActive(this)) {
        support.SetActive(this);
    }
}
