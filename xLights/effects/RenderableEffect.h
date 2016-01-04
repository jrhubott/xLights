#ifndef RENDERABLEEFFECT_H
#define RENDERABLEEFFECT_H

#include <string>

class wxPanel;
class wxWindow;
class wxBitmap;
class ModelClass;
class SequenceElements;

class RenderableEffect
{
    public:
        RenderableEffect(int id, std::string n);
        virtual ~RenderableEffect();
    
        virtual const std::string &Name() const { return name;};
        virtual const wxBitmap &GetEffectIcon(int size, bool exact = true) const;
        virtual int GetId() const { return id; }

        virtual void SetSequenceElements(SequenceElements *els) {mSequenceElements = els;}

        wxPanel *GetPanel(wxWindow *parent);
        virtual void SetDefaultParameters(ModelClass *cls) {}

        //virtual void Render() = 0;

    protected:
        virtual wxPanel *CreatePanel(wxWindow *parent) = 0;
        std::string name;
        int id;
        wxPanel *panel;
        SequenceElements *mSequenceElements;
    private:
};

#endif // RENDERABLEEFFECT_H