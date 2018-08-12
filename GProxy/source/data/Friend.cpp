#include "Friend.h"

Friend::Friend ()
{
    entryNumber = 255;
}

Friend::Friend (const Friend& orig)
{
    this->setEntryNumber(orig.getEntryNumber());
    this->setName(orig.getName());
    this->setLocation(orig.getLocation());
    this->setStatus(orig.getStatus());
    this->setProduct(orig.getProduct());
    this->setLocationName(orig.getLocationName());
}

Friend& Friend::operator= (const Friend& orig)
{
    if (this == &orig)
    {
        return *this;
    }

    this->setEntryNumber(orig.getEntryNumber());
    this->setName(orig.getName());
    this->setLocation(orig.getLocation());
    this->setStatus(orig.getStatus());
    this->setProduct(orig.getProduct());
    this->setLocationName(orig.getLocationName());

    return *this;
}

Friend::~Friend () { }

// <editor-fold defaultstate="collapsed" desc="Getters and setters">
unsigned char Friend::getEntryNumber() const
{
    return entryNumber;
}

void Friend::setEntryNumber(const unsigned char& entryNumber)
{
    this->entryNumber = entryNumber;
}

Friend::Location Friend::getLocation() const
{
    return location;
}

void Friend::setLocation(const Friend::Location& location)
{
    this->location = location;
}

void Friend::setLocation(const unsigned char& location)
{
    switch(location)
    {
        case 0x00:
            this->location = Friend::NOT_AVAILABLE;
            break;
        case 0x01:
            this->location = Friend::NOT_IN_CHANNEL;
            break;
        case 0x02:
            this->location = Friend::IN_CHANNEL;
            break;
        case 0x03:
            this->location = Friend::IN_PUBLIC_GAME;
            break;
        case 0x04:
            this->location = Friend::IN_UNKNOWN_PRIVATE_GAME;
            break;
        case 0x05:
            this->location = Friend::IN_PRIVATE_GAME;
            break;
    }
}

QString Friend::getLocationName() const
{
    return locationName;
}

void Friend::setLocationName(const QString& locationName)
{
    this->locationName = locationName;
}

QString Friend::getName() const
{
    return name;
}

void Friend::setName(const QString& name)
{
    this->name = name;
}

Product Friend::getProduct() const
{
    return product;
}

void Friend::setProduct(const Product& product)
{
    this->product = product;
}

Friend::Status Friend::getStatus() const
{
    return status;
}

void Friend::setStatus(const Friend::Status& status)
{
    this->status = status;
}

void Friend::setStatus(const unsigned char& status)
{
    switch(status)
    {
        case 0:
            this->status = Friend::OFFLINE;
            break;
        case 1:
            this->status = Friend::MUTUAL;
            break;
        case 2:
            this->status = Friend::DND;
            break;
        case 3:
            this->status = Friend::MUTUAL_DND;
            break;
        case 4:
            this->status = Friend::AWAY;
            break;
        case 5:
            this->status = Friend::MUTUAL_AWAY;
            break;
        case 6:
            this->status = Friend::DND_AWAY;
            break;
        case 7:
            this->status = Friend::MUTUAL_DND_AWAY;
            break;
    }
}
// </editor-fold>