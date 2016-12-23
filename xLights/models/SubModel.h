#ifndef SUBMODEL_H
#define SUBMODEL_H

#include "Model.h"

class SubModel : public Model {
public:
    SubModel(Model *p, wxXmlNode *n);
    virtual ~SubModel() {}
    
    virtual std::string GetFullName() const override { return parent->GetFullName() + "/" + name;}
    
    virtual const ModelScreenLocation &GetModelScreenLocation() const override { return parent->GetModelScreenLocation(); }
    virtual ModelScreenLocation &GetModelScreenLocation() override { return parent->GetModelScreenLocation(); };
    
    virtual const std::string &GetLayoutGroup() const override { return parent->GetLayoutGroup(); }

    virtual void AddProperties(wxPropertyGridInterface *grid) override;

    
    Model* GetParent() const { return parent; }
private:
    Model *parent;
};


#endif // SUBMODEL_H
