#include "Slot.h"

Slot::Slot () { }

Slot::Slot (const Slot& orig)
{
    this->setPlayerId(orig.getPlayerId());
    this->setDownloadProgress(orig.getDownloadProgress());
    this->setSlotStatus(orig.getSlotStatus());
    this->setComputerStatus(orig.getComputerStatus());
    this->setTeam(orig.getTeam());
    this->setColor(orig.getColor());
    this->setRace(orig.getRace());
    this->setComputerType(orig.getComputerType());
    this->setHandicap(orig.getHandicap());
    this->setPlayer(orig.getPlayer());
}

Slot& Slot::operator= (const Slot& orig)
{
    if (this == &orig)
    {
        return *this;
    }

    this->setPlayerId(orig.getPlayerId());
    this->setDownloadProgress(orig.getDownloadProgress());
    this->setSlotStatus(orig.getSlotStatus());
    this->setComputerStatus(orig.getComputerStatus());
    this->setTeam(orig.getTeam());
    this->setColor(orig.getColor());
    this->setRace(orig.getRace());
    this->setComputerType(orig.getComputerType());
    this->setHandicap(orig.getHandicap());
    this->setPlayer(orig.getPlayer());

    return *this;
}

Slot::~Slot () { }

// <editor-fold defaultstate="collapsed" desc="Getters and setters">
Slot::SlotColor Slot::getColor () const
{
    return color;
}

void Slot::setColor (const SlotColor& color)
{
    this->color = color;
}

Slot::ComputerStatus Slot::getComputerStatus () const
{
    return computerStatus;
}

void Slot::setComputerStatus (const ComputerStatus& computerStatus)
{
    this->computerStatus = computerStatus;
}

Slot::ComputerType Slot::getComputerType () const
{
    return computerType;
}

void Slot::setComputerType (const ComputerType& computerType)
{
    this->computerType = computerType;
}

unsigned char Slot::getDownloadProgress () const
{
    return downloadProgress;
}

void Slot::setDownloadProgress (const unsigned char& downloadProgress)
{
    this->downloadProgress = downloadProgress;
}

unsigned char Slot::getHandicap () const
{
    return handicap;
}

void Slot::setHandicap (const unsigned char& handicap)
{
    this->handicap = handicap;
}

Player* Slot::getPlayer () const
{
    return player;
}

void Slot::setPlayer (Player *player)
{
    this->player = player;
}

unsigned char Slot::getPlayerId () const
{
    return playerId;
}

void Slot::setPlayerId (const unsigned char& playerId)
{
    this->playerId = playerId;
}

Slot::Race Slot::getRace () const
{
    return race;
}

void Slot::setRace (const Race& race)
{
    this->race = race;
}

Slot::SlotStatus Slot::getSlotStatus () const
{
    return slotStatus;
}

void Slot::setSlotStatus (const SlotStatus& slotStatus)
{
    this->slotStatus = slotStatus;
}

unsigned char Slot::getTeam () const
{
    return team;
}

void Slot::setTeam (const unsigned char& team)
{
    this->team = team;
}
// </editor-fold>
