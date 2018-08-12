#include "Product.h"

Product::Product () { }

Product::Product(const QString& productCode)
{
    this->setProductCode(productCode);
}

Product::Product(const Product& orig)
{
    this->setProductCode(orig.getProductCode());
}

Product& Product::operator= (const Product& orig)
{
    if (this == &orig)
    {
        return *this;
    }

    this->setProductCode(orig.getProductCode());

    return *this;
}

Product::~Product () { }

// <editor-fold defaultstate="collapsed" desc="Getters and setters">
QString Product::getName() const
{
    if (productCode == "CHAT")
    {
        return "Telnet";
    }
    else if (productCode == "SSHR")
    {
        return "Starcraft Shareware";
    }
    else if (productCode == "JSTR")
    {
        return "Starcraft Japanese";
    }
    else if (productCode == "STAR")
    {
        return "Starcraft";
    }
    else if (productCode == "SEXP")
    {
        return "Starcraft Broodwar";
    }
    else if (productCode == "DSHR")
    {
        return "Diablo Shareware";
    }
    else if (productCode == "DRTL")
    {
        return "Diablo I";
    }
    else if (productCode == "D2DV")
    {
        return "Diablo II";
    }
    else if (productCode == "D2XP")
    {
        return "Diablo II LOD";
    }
    else if (productCode == "W2BN")
    {
        return "Warcraft II";
    }
    else if (productCode == "W3DM")
    {
        return "Warcraft III Demo";
    }
    else if (productCode == "WAR3")
    {
        return "Warcraft III Reign of Chaos";
    }
    else if (productCode == "W3XP")
    {
        return "Warcraft III The Frozen Throne";
    }
    else
    {
        return "Unknown Produckt: " + productCode;
    }
}

QString Product::getProductCode() const
{
    return productCode;
}

void Product::setProductCode(const QString& productCode)
{
    this->productCode = productCode;
}
// </editor-fold>