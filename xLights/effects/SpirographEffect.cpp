#include "SpirographEffect.h"
#include "SpirographPanel.h"

#include "../sequencer/Effect.h"
#include "../RenderBuffer.h"
#include "../UtilClasses.h"


#include "../../include/spirograph-16.xpm"
#include "../../include/spirograph-24.xpm"
#include "../../include/spirograph-32.xpm"
#include "../../include/spirograph-48.xpm"
#include "../../include/spirograph-64.xpm"

SpirographEffect::SpirographEffect(int id) : RenderableEffect(id, "Spirograph", spirograph_16, spirograph_24, spirograph_32, spirograph_48, spirograph_64)
{
    //ctor
}

SpirographEffect::~SpirographEffect()
{
    //dtor
}
wxPanel *SpirographEffect::CreatePanel(wxWindow *parent) {
    return new SpirographPanel(parent);
}

void SpirographEffect::SetDefaultParameters()
{
    SpirographPanel *sp = (SpirographPanel*)panel;
    if (sp == nullptr) {
        return;
    }

    sp->BitmapButton_Spirograph_AnimateVC->SetActive(false);
    sp->BitmapButton_Spirograph_LengthVC->SetActive(false);
    sp->BitmapButton_Spirograph_dVC->SetActive(false);
    sp->BitmapButton_Spirograph_RVC->SetActive(false);
    sp->BitmapButton_Spirograph_rVC->SetActive(false);
    sp->BitmapButton_Spirograph_SpeedrVC->SetActive(false);

    SetSliderValue(sp->Slider_Spirograph_Speed, 10);
    SetSliderValue(sp->Slider_Spirograph_R, 20);
    SetSliderValue(sp->Slider_Spirograph_r, 10);
    SetSliderValue(sp->Slider_Spirograph_d, 30);
    SetSliderValue(sp->Slider_Spirograph_Animate, 0);
    SetSliderValue(sp->Slider_Spirograph_Length, 20);
}

void SpirographEffect::Render(Effect *effect, SettingsMap &SettingsMap, RenderBuffer &buffer) {

    float oset = buffer.GetEffectTimeIntervalPosition();

    int int_R   = GetValueCurveInt("Spirograph_R", 20, SettingsMap, oset, SPIROGRAPH_R_MIN, SPIROGRAPH_R_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int int_r   = GetValueCurveInt("Spirograph_r", 10, SettingsMap, oset, SPIROGRAPH_r_MIN, SPIROGRAPH_r_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int int_d   = GetValueCurveInt("Spirograph_d", 30, SettingsMap, oset, SPIROGRAPH_d_MIN, SPIROGRAPH_d_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int Animate = GetValueCurveInt("Spirograph_Animate", 0, SettingsMap, oset, SPIROGRAPH_ANIMATE_MIN, SPIROGRAPH_ANIMATE_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int sspeed  = GetValueCurveInt("Spirograph_Speed", 10, SettingsMap, oset, SPIROGRAPH_SPEED_MIN, SPIROGRAPH_SPEED_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int length  = GetValueCurveInt("Spirograph_Length", 20, SettingsMap, oset, SPIROGRAPH_LENGTH_MIN, SPIROGRAPH_LENGTH_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());

    int d_mod;
    HSVValue hsv,hsv0,hsv1;
    size_t colorcnt=buffer.GetColorCount();

    int state = (buffer.curPeriod - buffer.curEffStartPer) * sspeed * buffer.frameTimeInMs / 50;
    double animateState = double((buffer.curPeriod - buffer.curEffStartPer) * Animate * buffer.frameTimeInMs) / 5000.0;

    length = length * 18;

    int xc = (int)(buffer.BufferWi/2); // 20x100 flex strips with 2 fols per strip = 40x50
    int yc = (int)(buffer.BufferHt/2);
    float R = xc*(int_R/100.0);   //  Radius of the large circle just fits in the width of model
    float r = xc*(int_r/100.0); // start little circle at 1/4 of max width
    if(r>R) r=R;
    float d = xc*(int_d/100.0);

    //  palette.GetHSV(1, hsv1);
    //
    //    A hypotrochoid is a roulette traced by a point attached to a circle of radius r rolling around the inside of a fixed circle of radius R, where the point is a distance d from the center of the interior circle.
    //The parametric equations for a hypotrochoid are:[citation needed]
    //
    //  more info: http://en.wikipedia.org/wiki/Hypotrochoid
    //
    //x(t) = (R-r) * cos t + d*buffer.cos ((R-r/r)*t);
    //y(t) = (R-r) * sin t + d*buffer.sin ((R-r/r)*t);

    int mod1440 = state%1440;
    float d_orig = d;
    if (Animate) d = d_orig + animateState * d_orig; // should we modify the distance variable each pass through?
    for(int i = 1; i<=length; i++)
    {
        float t = (i+mod1440)*M_PI/180;
        int x = (R-r) * buffer.cos (t) + d*buffer.cos (((R-r)/r)*t) + xc;
        int y = (R-r) * buffer.sin (t) + d*buffer.sin (((R-r)/r)*t) + yc;

        if(colorcnt>0) d_mod = (int) buffer.BufferWi/colorcnt;
        else d_mod=1;

        double x2 = pow ((double)(x-xc),2);
        double y2 = pow ((double)(y-yc),2);
        double hyp = (sqrt(x2 + y2)/buffer.BufferWi) * 100.0;
        int ColorIdx = (int)(hyp / d_mod); // Select random numbers from 0 up to number of colors the user has checked. 0-5 if 6 boxes checked

        if(ColorIdx>=colorcnt) ColorIdx=colorcnt-1;

        buffer.palette.GetHSV(ColorIdx, hsv); // Now go and get the hsv value for this ColorIdx


        buffer.palette.GetHSV(0, hsv0);
        ColorIdx=(state+rand()) % colorcnt; // Select random numbers from 0 up to number of colors the user has checked. 0-5 if 6 boxes checked
        buffer.palette.GetHSV(ColorIdx, hsv1); // Now go and get the hsv value for this ColorIdx

        buffer.SetPixel(x,y,hsv);
        //        if(i<=state360) SetPixel(x,y,hsv); // Turn pixel on
        //        else SetPixel(x,y,hsv1);
    }
}
