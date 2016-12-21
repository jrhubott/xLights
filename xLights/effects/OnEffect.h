#ifndef ONEFFECT_H
#define ONEFFECT_H

#include "RenderableEffect.h"


class OnEffect : public RenderableEffect
{
    public:
        OnEffect(int id);
        virtual ~OnEffect();
        virtual bool CanBeRandom() override {return false;}
    
        virtual void Render(Effect *effect, const SettingsMap &settings, RenderBuffer &buffer) override;
        virtual int DrawEffectBackground(const Effect *e, int x1, int y1, int x2, int y2, DrawGLUtils::xlVertexColorAccumulator &backgrounds) override;
        virtual bool SupportsLinearColorCurves(const SettingsMap &SettingsMap) override { return true; }

        virtual void SetDefaultParameters(Model *cls) override;
        virtual std::string GetEffectString() override;

    protected:
        virtual void RemoveDefaults(const std::string &version, Effect *effect) override;
        virtual wxPanel *CreatePanel(wxWindow *parent) override;
    private:
};

#endif // ONEFFECT_H
