/***************************************************************
 * Name:      RgbEffects.cpp
 * Purpose:   Implements RGB effects
 * Author:    Matt Brown (dowdybrown@yahoo.com)
 * Created:   2012-12-23
 * Copyright: 2012 by Matt Brown
 * License:
     This file is part of xLights.

    xLights is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    xLights is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with xLights.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************/
#include <cmath>
#ifdef _MSC_VER
	// required so M_PI will be defined by MSC
	#define _USE_MATH_DEFINES
	#include <math.h>
#endif
#include "RenderBuffer.h"
#include "sequencer/Effect.h"
#include "xLightsMain.h"
#include "xLightsXmlFile.h"




#ifdef __WXMSW__
#define USE_GRAPHICS_CONTEXT_FOR_TEXT 0
#else
#define USE_GRAPHICS_CONTEXT_FOR_TEXT 1
#endif

EffectRenderCache::EffectRenderCache() {}
EffectRenderCache::~EffectRenderCache() {}
void RenderBuffer::SetAllowAlphaChannel(bool a) { allowAlpha = a; }
void RenderBuffer::SetFrameTimeInMs(int i) { frameTimeInMs = i; }

inline void unshare(wxObject &o) {
    if (o.GetRefData() != nullptr) {
        o.UnShare();
    }
}
inline void unshare(const wxObject &o2) {
    wxObject *o = (wxObject*)&o2;
    if (o->GetRefData() != nullptr) {
        o->UnShare();
    }
}

AudioManager* RenderBuffer::GetMedia()
{
	if (xLightsFrame::CurrentSeqXmlFile == nullptr)
	{
		return NULL;
	}
	return xLightsFrame::CurrentSeqXmlFile->GetMedia();
}

inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }


DrawingContext::DrawingContext(int BufferWi, int BufferHt, bool allowShared, bool alpha) : nullBitmap(wxNullBitmap)
{
    unshare(nullBitmap);
    image = new wxImage(BufferWi > 0 ? BufferWi : 1, BufferHt > 0 ? BufferHt : 1);
    if (alpha) {
        image->SetAlpha();
        for(wxCoord x=0; x<BufferWi; x++) {
            for(wxCoord y=0; y<BufferHt; y++) {
                image->SetAlpha(x, y, wxIMAGE_ALPHA_TRANSPARENT);
            }
        }
    }
    bitmap = new wxBitmap(*image);
    dc = new wxMemoryDC(*bitmap);

    if (!allowShared) {
        //make sure we UnShare everything that is being held onto
        //also use "non-normal" defaults to avoid "==" issue that
        //would keep it from using the non-shared versions
        wxFont font(*wxITALIC_FONT);
        unshare(font);
        dc->SetFont(font);

        wxBrush brush(*wxYELLOW_BRUSH);
        unshare(brush);
        dc->SetBrush(brush);
        dc->SetBackground(brush);

        wxPen pen(*wxGREEN_PEN);
        unshare(pen);
        dc->SetPen(pen);

        unshare(dc->GetBrush());
        unshare(dc->GetBackground());
        unshare(dc->GetFont());
        unshare(dc->GetPen());
        unshare(dc->GetTextForeground());
        unshare(dc->GetTextBackground());
    #ifndef LINUX
        wxColor c(12, 25, 3);
        unshare(c);
        dc->SetTextBackground(c);

        wxColor c2(0, 35, 5);
        unshare(c2);
        dc->SetTextForeground(c2);
    #endif
    }

    dc->SelectObject(nullBitmap);
    delete bitmap;
    bitmap = nullptr;
    gc = nullptr;
}

DrawingContext::~DrawingContext() {
    if (gc != nullptr) {
        delete gc;
    }
    if (dc != nullptr) {
        delete dc;
    }
    if (bitmap != nullptr) {
        delete bitmap;
    }
    if (image != nullptr) {
        delete image;
    }
}


PathDrawingContext::PathDrawingContext(int BufferWi, int BufferHt, bool allowShared)
    : DrawingContext(BufferWi, BufferHt, allowShared, true)
{
}
PathDrawingContext::~PathDrawingContext() {
}

TextDrawingContext::TextDrawingContext(int BufferWi, int BufferHt, bool allowShared)
#ifdef __WXMSW__
    : DrawingContext(BufferWi, BufferHt, allowShared, false)
#else
    : DrawingContext(BufferWi, BufferHt, allowShared, true)
#endif
{
}
TextDrawingContext::~TextDrawingContext() {
}


void DrawingContext::ResetSize(int BufferWi, int BufferHt) {
    if (bitmap != nullptr) {
        delete bitmap;
        bitmap = nullptr;
    }
    if (image != nullptr) {
        delete image;
    }
    image = new wxImage(BufferWi > 0 ? BufferWi : 1, BufferHt > 0 ? BufferHt : 1);
    if (AllowAlphaChannel()) {
        image->SetAlpha();
        for(wxCoord x=0; x<BufferWi; x++) {
            for(wxCoord y=0; y<BufferHt; y++) {
                image->SetAlpha(x, y, wxIMAGE_ALPHA_TRANSPARENT);
            }
        }
    }
}

void DrawingContext::Clear() {
    dc->SelectObject(nullBitmap);
    if (bitmap != nullptr) {
        delete bitmap;
    }
    image->Clear();

    if (AllowAlphaChannel()) {
        image->SetAlpha();
        for(wxCoord x=0; x<image->GetWidth(); x++) {
            for(wxCoord y=0; y<image->GetHeight(); y++) {
                image->SetAlpha(x, y, wxIMAGE_ALPHA_TRANSPARENT);
            }
        }
        bitmap = new wxBitmap(*image, 32);
    } else {
        bitmap = new wxBitmap(*image);
    }
    dc->SelectObject(*bitmap);
}
void PathDrawingContext::Clear() {
    if (gc != nullptr) {
        delete gc;
        gc = nullptr;
    }
    DrawingContext::Clear();
#ifdef LINUX
    gc = wxGraphicsContext::Create(*image);
#else
    gc = wxGraphicsContext::Create(*dc);
#endif
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->SetInterpolationQuality(wxInterpolationQuality::wxINTERPOLATION_FAST);
    gc->SetCompositionMode(wxCompositionMode::wxCOMPOSITION_SOURCE);
    //gc->SetCompositionMode(wxCompositionMode::wxCOMPOSITION_OVER);
}
void TextDrawingContext::Clear() {
    if (gc != nullptr) {
        delete gc;
        gc = nullptr;
    }
    DrawingContext::Clear();

#if USE_GRAPHICS_CONTEXT_FOR_TEXT
#ifndef __WXOSX__
    gc = wxGraphicsContext::Create(*image);
#else
    gc = wxGraphicsContext::Create(*dc);
#endif
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    gc->SetInterpolationQuality(wxInterpolationQuality::wxINTERPOLATION_FAST);
    gc->SetCompositionMode(wxCompositionMode::wxCOMPOSITION_SOURCE);
    //gc->SetCompositionMode(wxCompositionMode::wxCOMPOSITION_OVER);
#endif
}
bool TextDrawingContext::AllowAlphaChannel() {
#ifdef __WXMSW__
    return false;
#else
    return true;
#endif
}


wxImage *DrawingContext::FlushAndGetImage() {
    if (gc != nullptr) {
        gc->Flush();
        delete gc;
        gc = nullptr;
    }
#ifndef LINUX
    dc->SelectObject(nullBitmap);
    *image = bitmap->ConvertToImage();
    dc->SelectObject(*bitmap);
#endif
    return image;
}

void PathDrawingContext::SetPen(wxPen &pen) {
    gc->SetPen(pen);
}
void TextDrawingContext::SetPen(wxPen &pen) {
    if (gc != nullptr) {
        gc->SetPen(pen);
    } else {
        dc->SetPen(pen);
    }
}

wxGraphicsPath PathDrawingContext::CreatePath()
{
    return gc->CreatePath();
}

void PathDrawingContext::StrokePath(wxGraphicsPath& path)
{
    gc->StrokePath(path);
}


void TextDrawingContext::SetFont(wxFontInfo &font, const xlColor &color) {
    if (gc != nullptr) {
        int style = wxFONTFLAG_NOT_ANTIALIASED;
        if (font.GetWeight() == wxFONTWEIGHT_BOLD) {
            style |= wxFONTFLAG_BOLD;
        }
        if (font.GetWeight() == wxFONTWEIGHT_LIGHT) {
            style |= wxFONTFLAG_LIGHT;
        }
        if (font.GetStyle() == wxFONTSTYLE_ITALIC) {
            style |= wxFONTFLAG_ITALIC;
        }
        if (font.GetStyle() == wxFONTSTYLE_SLANT) {
            style |= wxFONTFLAG_SLANT;
        }
        if (font.IsUnderlined()) {
            style |= wxFONTFLAG_UNDERLINED;
        }
        if (font.IsStrikethrough()) {
            style |= wxFONTFLAG_STRIKETHROUGH;
        }

        if (style != fontStyle
            || font.GetPixelSize().y != fontSize
            || font.GetFaceName() != fontName
            || color != fontColor) {
            this->font = gc->CreateFont(font.GetPixelSize().y, font.GetFaceName(), style, color.asWxColor());

            fontStyle = style;
            fontSize = font.GetPixelSize().y;
            fontName = font.GetFaceName();
            fontColor = color;
        }
        gc->SetFont(this->font);
    } else {
        wxFont f(font);
    #ifdef __WXMSW__
        /*
         Here is the format for NativeFontInfo on Windows (taken from the source)
         We want to change lfQuality from 2 to 3 - this disables antialiasing
         s.Printf(wxS("%d;%ld;%ld;%ld;%ld;%ld;%d;%d;%d;%d;%d;%d;%d;%d;%s"),
         0, // version, in case we want to change the format later
         lf.lfHeight,
         lf.lfWidth,
         lf.lfEscapement,
         lf.lfOrientation,
         lf.lfWeight,
         lf.lfItalic,
         lf.lfUnderline,
         lf.lfStrikeOut,
         lf.lfCharSet,
         lf.lfOutPrecision,
         lf.lfClipPrecision,
         lf.lfQuality,
         lf.lfPitchAndFamily,
         lf.lfFaceName);*/
        wxString s = f.GetNativeFontInfoDesc();
        s.Replace(";2;",";3;",false);
        f.SetNativeFontInfo(s);
    #endif
        dc->SetFont(f);
        dc->SetTextForeground(color.asWxColor());
    }
}

void TextDrawingContext::DrawText(const wxString &msg, int x, int y, double rotation) {
    if (gc != nullptr) {
        gc->DrawText(msg, x, y, DegToRad(rotation));
    } else {
        dc->DrawRotatedText(msg, x, y, rotation);
    }
}

void TextDrawingContext::DrawText(const wxString &msg, int x, int y) {
    if (gc != nullptr) {
        gc->DrawText(msg, x, y);
    } else {
        dc->DrawText(msg, x, y);
    }
}

void TextDrawingContext::GetTextExtent(const wxString &msg, double *width, double *height) {
    if (gc != nullptr) {
        gc->GetTextExtent(msg, width, height);
    } else {
        wxSize size = dc->GetTextExtent(msg);
        *width = size.GetWidth();
        *height = size.GetHeight();
    }
}

RenderBuffer::RenderBuffer(xLightsFrame *f, bool b) : frame(f)
{
    frameTimeInMs = 50;
    textDrawingContext = nullptr;
    pathDrawingContext = nullptr;
    tempInt = tempInt2 = 0;
    onlyOnMain = b;
    isTransformed = false;
}

RenderBuffer::~RenderBuffer()
{
    //dtor
    if (textDrawingContext != nullptr) {
        delete textDrawingContext;
    }
    if (pathDrawingContext != nullptr) {
        delete pathDrawingContext;
    }
    for (std::map<int, EffectRenderCache*>::iterator i = infoCache.begin(); i != infoCache.end(); i++) {
        delete i->second;
    }
}

void RenderBuffer::InitBuffer(int newBufferHt, int newBufferWi, const std::string& bufferTransform)
{
    if (pathDrawingContext == nullptr) {
        pathDrawingContext = new PathDrawingContext(newBufferWi, newBufferHt, onlyOnMain);
    } else if (BufferHt != newBufferHt || BufferWi != newBufferWi) {
        pathDrawingContext->ResetSize(newBufferWi, newBufferHt);
    }
    if (textDrawingContext == nullptr) {
        textDrawingContext = new TextDrawingContext(newBufferWi, newBufferHt, onlyOnMain);
    } else if (BufferHt != newBufferHt || BufferWi != newBufferWi) {
        textDrawingContext->ResetSize(newBufferWi, newBufferHt);
    }
    BufferHt=newBufferHt;
    BufferWi=newBufferWi;
    int NumPixels=BufferHt * BufferWi;
    pixels.resize(NumPixels);
    tempbuf.resize(NumPixels);
    isTransformed = (bufferTransform != "None");
}

void RenderBuffer::Clear()
{
    // KW This is massively faster
    if (pixels.size() > 0)
    {
        memset(&pixels[0], 0x00, sizeof(xlColor) * pixels.size());
    }
    //for(size_t i=0; i<pixels.size(); i++)
    //{
    //    pixels[i]=bgColor;
    //}
}

void RenderBuffer::SetPalette(xlColorVector& newcolors, xlColorCurveVector& newcc)
{
    palette.Set(newcolors, newcc);
}

size_t RenderBuffer::GetColorCount()
{
    return palette.Size();
}

// return a random number between 0 and 1 inclusive
double RenderBuffer::rand01()
{
    return (double)rand()/(double)RAND_MAX;
}


class SinTable {
public:
    static constexpr float precision = 300.0f; // gradations per Pi, 942 entries of size float is under 4K or less than a memory page
    static constexpr int modulus = (int)(M_PI * precision) + 1;
    static constexpr int modulus2 = modulus * 2;

    SinTable() {
        for (int i = 0; i<modulus; i++) {
            float f = i;
            f /= precision;
            table[i]=sinf(f);
        }
        /*
        for (int x = -720; x <= 720; x++) {
            float s = ((float)x)*3.14159f/180.0f;
            printf("%d:\t%f\t%f\n", x, this->cos(s), cosf(s));
        }
         */
    }
    ~SinTable() {
    }
    float table[modulus]; // lookup table

    float sinLookup(int a) {
        if (a >= 0) {
            int idx = a%(modulus2);
            if (idx >= modulus) {
                idx -= modulus;
                return -table[idx];
            }
            return table[idx];
        }
        int idx = -a%(modulus2);
        if (idx >= modulus) {
            idx -= modulus;
            return table[idx];
        }
        return -table[idx];
    }
    float sin(float rad) {
        float idx = rad * precision + 0.5f;
        return sinLookup((int)idx);
    }
    float cos(float a) {
        return this->sin(a + M_PI_2);
    }
};
static SinTable sinTable;

float RenderBuffer::sin(float rad)
{
    return sinTable.sin(rad);
}
float RenderBuffer::cos(float rad)
{
    return sinTable.cos(rad);
}

// generates a random number between num1 and num2 inclusive
double RenderBuffer::RandomRange(double num1, double num2)
{
    double hi,lo;
    if (num1 < num2)
    {
        lo = num1;
        hi = num2;
    }
    else
    {
        lo = num2;
        hi = num1;
    }
    return rand01()*(hi-lo)+ lo;
}

void RenderBuffer::Color2HSV(const xlColor& color, HSVValue& hsv)
{
    color.toHSV(hsv);
}

// sets newcolor to a random color between hsv1 and hsv2
void RenderBuffer::SetRangeColor(const HSVValue& hsv1, const HSVValue& hsv2, HSVValue& newhsv)
{
    newhsv.hue=RandomRange(hsv1.hue,hsv2.hue);
    newhsv.saturation=RandomRange(hsv1.saturation,hsv2.saturation);
    newhsv.value=1.0;
}

// return a value between c1 and c2
uint8_t RenderBuffer::ChannelBlend(uint8_t c1, uint8_t c2, float ratio)
{
    return c1 + floor(ratio*(c2-c1)+0.5);
}

void RenderBuffer::Get2ColorBlend(int coloridx1, int coloridx2, float ratio, xlColor &color)
{
    color = palette.GetColor(coloridx1);
    const xlColor &c2 = palette.GetColor(coloridx2);
    Get2ColorBlend(color, c2, ratio);
}

void RenderBuffer::Get2ColorBlend(xlColor& color, xlColor color2, float ratio)
{
    color.Set(ChannelBlend(color.Red(), color2.Red(), ratio), ChannelBlend(color.Green(), color2.Green(), ratio), ChannelBlend(color.Blue(), color2.Blue(), ratio));
}

void RenderBuffer::Get2ColorAlphaBlend(const xlColor& c1, const xlColor& c2, float ratio, xlColor &color)
{
    color.Set(ChannelBlend(c1.Red(),c2.Red(),ratio), ChannelBlend(c1.Green(),c2.Green(),ratio), ChannelBlend(c1.Blue(),c2.Blue(),ratio));
}

HSVValue RenderBuffer::Get2ColorAdditive(HSVValue& hsv1, HSVValue& hsv2)
{
    xlColor rgb;
    xlColor rgb1(hsv1);
    xlColor rgb2(hsv2);
    rgb.red = rgb1.red + rgb2.red;
    rgb.green = rgb1.green + rgb2.green;
    rgb.blue = rgb1.blue + rgb2.blue;
    return rgb.asHSV();
}
// 0 <= n < 1
void RenderBuffer::GetMultiColorBlend(float n, bool circular, xlColor &color)
{
    size_t colorcnt=GetColorCount();
    if (colorcnt <= 1)
    {
        palette.GetColor(0,color);
        return;
    }
    if (n >= 1.0) n=0.99999;
    if (n < 0.0) n=0.0;
    float realidx=circular ? n*colorcnt : n*(colorcnt-1);
    int coloridx1=floor(realidx);
    int coloridx2=(coloridx1+1) % colorcnt;
    float ratio=realidx-float(coloridx1);
    Get2ColorBlend(coloridx1,coloridx2,ratio,color);
}


// 0,0 is lower left
void RenderBuffer::SetPixel(int x, int y, const xlColor &color, bool wrap)
{
    if (wrap) {
        while (x < 0) {
            x += BufferWi;
        }
        while (y < 0) {
            y += BufferHt;
        }
        while (x > BufferWi) {
            x -= BufferWi;
        }
        while (y > BufferHt) {
            y -= BufferHt;
        }
    }
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        pixels[y*BufferWi+x] = color;
    }
}

void RenderBuffer::ProcessPixel(int x_pos, int y_pos, const xlColor &color, bool wrap_x)
{
    int x_value = x_pos;
    if( wrap_x )  // if set wrap image at boundary
    {
        x_value %= BufferWi;
        x_value = (x_value >= 0) ? (x_value) : (BufferWi + x_value);
    }
    SetPixel(x_value,y_pos,color);
}

// 0,0 is lower left
void RenderBuffer::SetPixel(int x, int y, const HSVValue& hsv, bool wrap)
{
    if (wrap) {
        while (x < 0) {
            x += BufferWi;
        }
        while (y < 0) {
            y += BufferHt;
        }
        while (x > BufferWi) {
            x -= BufferWi;
        }
        while (y > BufferHt) {
            y -= BufferHt;
        }
    }
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        pixels[y*BufferWi+x] = hsv;
    }
}

//copy src to dest: -DJ
void RenderBuffer::CopyPixel(int srcx, int srcy, int destx, int desty)
{
    if ((srcx >= 0) && (srcx < BufferWi) && (srcy >= 0) && (srcy < BufferHt))
        if ((destx >= 0) && (destx < BufferWi) && (desty >= 0) && (desty < BufferHt))
            pixels[desty * BufferWi + destx] = pixels[srcy * BufferWi + srcx];
}

void RenderBuffer::DrawHLine(int y, int xstart, int xend, const xlColor &color, bool wrap) {
    if (xstart > xend) {
        int i = xstart;
        xstart = xend;
        xend = i;
    }
    for (int x = xstart; x <= xend; x++) {
        SetPixel(x, y, color, wrap);
    }
}
void RenderBuffer::DrawVLine(int x, int ystart, int yend, const xlColor &color, bool wrap) {
    if (ystart > yend) {
        int i = ystart;
        ystart = yend;
        yend = i;
    }
    for (int y = ystart; y <= yend; y++) {
        SetPixel(x, y, color, wrap);
    }
}
void RenderBuffer::DrawBox(int x1, int y1, int x2, int y2, const xlColor& color, bool wrap) {
    if (y1 > y2) {
        int i = y1;
        y1 = y2;
        y2 = i;
    }
    if (x1 > x2) {
        int i = x1;
        x1 = x2;
        x2 = i;
    }
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            SetPixel(x, y, color, wrap);
        }
    }
}

// Bresenham's line algorithm
void RenderBuffer::DrawLine( const int x0_, const int y0_, const int x1_, const int y1_, const xlColor& color )
{
    int x0 = x0_;
    int x1 = x1_;
    int y0 = y0_;
    int y1 = y1_;

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;){
    SetPixel(x0,y0, color);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

void RenderBuffer::DrawThickLine( const int x0_, const int y0_, const int x1_, const int y1_, const xlColor& color, bool direction )
{
    int x0 = x0_;
    int x1 = x1_;
    int y0 = y0_;
    int y1 = y1_;
    int lastx = x0;
    int lasty = y0;

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;){
    SetPixel(x0,y0, color);
    if( (x0 != lastx) && (y0 != lasty) && (x0_ != x1_) && (y0_ != y1_) )
    {
        int fix = 0;
        if( x0 > lastx ) fix += 1;
        if( y0 > lasty ) fix += 2;
        if( direction  ) fix += 4;
        switch (fix)
        {
        case 2:
        case 4:
            if( x0 < BufferWi -2 ) SetPixel(x0+1,y0, color);
            break;
        case 3:
        case 5:
            if( x0 > 0 ) SetPixel(x0-1,y0, color);
            break;
        case 0:
        case 1:
            if( y0 < BufferHt -2 )SetPixel(x0,y0+1, color);
            break;
        case 6:
        case 7:
            if( y0 > 0 )SetPixel(x0,y0-1, color);
            break;
        }
    }
    lastx = x0;
    lasty = y0;
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

void RenderBuffer::DrawFadingCircle(int x0, int y0, int radius, const xlColor& rgb, bool wrap)
{
    HSVValue hsv(rgb);
    xlColor color(rgb);
    int r = radius;
    if (allowAlpha) {
        while(r >= 0)
        {
            color.alpha = (double)rgb.alpha * (1.0 - (double)(r) / (double)radius);
            DrawCircle(x0, y0, r, color, wrap);
            r--;
        }
    } else {
        double full_brightness = hsv.value;
        while(r >= 0)
        {
            hsv.value = full_brightness * (1.0 - (double)(r) / (double)radius);
            if( hsv.value > 0.0 )
            {
                color = hsv;
                DrawCircle(x0, y0, r, color, wrap);
            }
            r--;
        }
    }
}

void RenderBuffer::DrawCircle(int x0, int y0, int radius, const xlColor& rgb, bool filled, bool wrap)
{
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    while(x >= y) {
        if (!filled) {
            SetPixel(x + x0, y + y0, rgb, wrap);
            SetPixel(y + x0, x + y0, rgb, wrap);
            SetPixel(-x + x0, y + y0, rgb, wrap);
            SetPixel(-y + x0, x + y0, rgb, wrap);
            SetPixel(-x + x0, -y + y0, rgb, wrap);
            SetPixel(-y + x0, -x + y0, rgb, wrap);
            SetPixel(x + x0, -y + y0, rgb, wrap);
            SetPixel(y + x0, -x + y0, rgb, wrap);
        } else {
            DrawVLine(x0 - x, y0 - y, y0 + y, rgb, wrap);
            DrawVLine(x0 + x, y0 - y, y0 + y, rgb, wrap);
            DrawVLine(x0 - y, y0 - x, y0 + x, rgb, wrap);
            DrawVLine(x0 + y, y0 - x, y0 + x, rgb, wrap);
        }
        y++;
        if (radiusError<0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x) + 1;
        }
    }
}


// 0,0 is lower left
void RenderBuffer::GetPixel(int x, int y, xlColor &color)
{
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        color=pixels[y*BufferWi+x];
    }
}
const xlColor &RenderBuffer::GetPixel(int x, int y) {
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        return pixels[y*BufferWi+x];
    }
    return xlBLACK;
}

// 0,0 is lower left
void RenderBuffer::SetTempPixel(int x, int y, const xlColor &color)
{
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        tempbuf[y*BufferWi+x]=color;
    }
}

void RenderBuffer::SetTempPixel(int x, int y, const xlColor & color, int alpha)
{
    xlColor c(color.Red(), color.Green(), color.Blue(), alpha);

    SetTempPixel(x, y, c);
}

// 0,0 is lower left
void RenderBuffer::GetTempPixel(int x, int y, xlColor &color)
{
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        color=tempbuf[y*BufferWi+x];
    }
}
const xlColor &RenderBuffer::GetTempPixel(int x, int y) {
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        return tempbuf[y*BufferWi+x];
    }
    return xlBLACK;
}


const xlColor& RenderBuffer::GetTempPixelRGB(int x, int y)
{
    if (x >= 0 && x < BufferWi && y >= 0 && y < BufferHt)
    {
        return tempbuf[y*BufferWi+x];
    }
    return xlBLACK;
}

void RenderBuffer::SetState(int period, bool ResetState, const std::string& model_name)
{
    if (ResetState)
    {
        needToInit = true;
    }
    curPeriod = period;
    cur_model = model_name;
    curPeriod = period;
    palette.UpdateForProgress(GetEffectTimeIntervalPosition());
}
void RenderBuffer::ClearTempBuf()
{
    for (size_t i=0; i < tempbuf.size(); i++)
    {
        tempbuf[i].Set(0, 0, 0, 0);
    }
}
float RenderBuffer::GetEffectTimeIntervalPosition(float cycles) {
    if (curEffEndPer == curEffStartPer) {
        return 0.0f;
    }
    float retval = (float)(curPeriod-curEffStartPer)/(float)(curEffEndPer-curEffStartPer);
    retval *= cycles;
    while (retval > 1.0) {
        retval -= 1.0;
    }
    return retval;
}
float RenderBuffer::GetEffectTimeIntervalPosition()
{
    if (curEffEndPer == curEffStartPer) {
        return 0.0;
    }
    return (float)(curPeriod-curEffStartPer)/(float)(curEffEndPer-curEffStartPer);
}

void RenderBuffer::SetEffectDuration(int startMsec, int endMsec)
{
    curEffStartPer = startMsec / frameTimeInMs;
    curEffEndPer = (endMsec - 1) / frameTimeInMs;
}

void RenderBuffer::GetEffectPeriods( int& start, int& endp)
{
    start = curEffStartPer;
    endp = curEffEndPer;
}

void RenderBuffer::SetDisplayListHRect(Effect *eff, int idx, float x1, float y1, float x2, float y2,
                                     const xlColor &c1, const xlColor &c2) {
    SetDisplayListRect(eff, idx, x1, y1, x2, y2, c1, c1, c2, c2);
}
void RenderBuffer::SetDisplayListVRect(Effect *eff, int idx, float x1, float y1, float x2, float y2,
                                     const xlColor &c1, const xlColor &c2) {
    SetDisplayListRect(eff, idx, x1, y1, x2, y2, c1, c2, c1, c2);
}
void RenderBuffer::SetDisplayListRect(Effect *eff, int idx, float x1, float y1, float x2, float y2,
                                    const xlColor &cx1y1, const xlColor &cx1y2,
                                    const xlColor &cx2y1, const xlColor &cx2y2) {
    eff->GetBackgroundDisplayList()[idx].color = cx1y1;
    eff->GetBackgroundDisplayList()[idx+1].color = cx1y2;
    eff->GetBackgroundDisplayList()[idx+2].color = cx2y2;
    eff->GetBackgroundDisplayList()[idx+3].color = cx2y2;
    eff->GetBackgroundDisplayList()[idx+4].color = cx2y1;
    eff->GetBackgroundDisplayList()[idx+5].color = cx1y1;

    eff->GetBackgroundDisplayList()[idx].x = x1;
    eff->GetBackgroundDisplayList()[idx+1].x = x1;
    eff->GetBackgroundDisplayList()[idx+2].x = x2;
    eff->GetBackgroundDisplayList()[idx+3].x = x2;
    eff->GetBackgroundDisplayList()[idx+4].x = x2;
    eff->GetBackgroundDisplayList()[idx+5].x = x1;

    eff->GetBackgroundDisplayList()[idx].y = y1;
    eff->GetBackgroundDisplayList()[idx+1].y = y2;
    eff->GetBackgroundDisplayList()[idx+2].y = y2;
    eff->GetBackgroundDisplayList()[idx+3].y = y2;
    eff->GetBackgroundDisplayList()[idx+4].y = y1;
    eff->GetBackgroundDisplayList()[idx+5].y = y1;
}
void RenderBuffer::CopyPixelsToDisplayListX(Effect *eff, int row, int sx, int ex, int inc) {
    std::lock_guard<std::recursive_mutex> lock(eff->GetBackgroundDisplayList().lock);
    int count = curEffEndPer - curEffStartPer + 1;


    int total = curEffEndPer - curEffStartPer + 1;
    float x = float(curPeriod - curEffStartPer) / float(total);
    float x2 = (curPeriod - curEffStartPer + 1.0) / float(total);
    xlColor c;

    int cur = 0;
    for (int p = sx; p <= ex; p += inc) {
        float y = float(p - sx) / float(ex - sx + 1.0);
        float y2 = float(p - sx + 1.0) / float(ex - sx + 1.0);
        GetPixel(p, row, c);

        int idx = cur * count + (curPeriod - curEffStartPer);
        cur++;
        SetDisplayListHRect(eff, idx*6, x, y, x2, y2, c, c);
    }
}

double RenderBuffer::calcAccel(double ratio, double accel)
{
    if( accel == 0 ) return ratio;

    double pct_accel = (std::abs(accel) - 1.0) / 9.0;
    double new_accel1 = pct_accel * 5 + (1.0 - pct_accel) * 1.5;
    double new_accel2 = 1.5 + (ratio * new_accel1);
    double final_accel = pct_accel * new_accel2 + (1.0 - pct_accel) * new_accel1;

    if( accel > 0 )
    {
        return std::pow(ratio, final_accel);
    }
    else
    {
        return (1.0 - std::pow(1.0 - ratio, new_accel1));
    }
}

double RenderBuffer::GetStepAngle(int width, int height)
{
    double step = 0.5;
    int biggest = std::max(width, height);
    if( biggest > 50 ) {
        step = 0.4;
    }
    if( biggest > 150 ) {
        step = 0.3;
    }
    if( biggest > 250 ) {
        step = 0.2;
    }
    if( biggest > 350 ) {
        step = 0.1;
    }
    return step;
}

// create a copy of the buffer suitable only for copying out pixel data
RenderBuffer::RenderBuffer(RenderBuffer& buffer)
{
    BufferHt = buffer.BufferHt;
    BufferWi = buffer.BufferWi;

    pixels = buffer.pixels;
    textDrawingContext = NULL;
    pathDrawingContext = NULL;
}
