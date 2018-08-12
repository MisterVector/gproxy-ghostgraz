#include "Player.h"

Player::Player ()
{
    // Set stay ratio to temporary to 100%. Will be updated by the statspage.
    setWins(1);
    setLosses(0);
    setGamesPlayed(1);
}

Player::Player (const ColoredMessage& name)
{
    this->name = name;

    // Set stay ratio to temporary to 100%. Will be updated by the statspage.
    setWins(1);
    setLosses(0);
    setGamesPlayed(1);
}

Player::Player (const Player& orig)
{
    this->setPlayerId(orig.getPlayerId());
    this->setName(orig.getName());
    this->setKills(orig.getKills());
    this->setDeaths(orig.getDeaths());
    this->setAssists(orig.getAssists());
    this->setKillDeathRatio(orig.getKillDeathRatio());
    this->setGamesPlayed(orig.getGamesPlayed());
    this->setWins(orig.getWins());
    this->setLosses(orig.getLosses());
    this->setWinPercent(orig.getWinPercent());
    this->setScore(orig.getScore());
    this->setCreepKills(orig.getCreepKills());
    this->setCreepDenies(orig.getCreepDenies());
    this->setTowerKills(orig.getTowerKills());
    this->setRaxKills(orig.getRaxKills());
    this->setCourierKills(orig.getCourierKills());
}

Player& Player::operator= (const Player& orig)
{
    if (this == &orig)
    {
        return *this;
    }

    this->setPlayerId(orig.getPlayerId());
    this->setName(orig.getName());
    this->setKills(orig.getKills());
    this->setDeaths(orig.getDeaths());
    this->setAssists(orig.getAssists());
    this->setKillDeathRatio(orig.getKillDeathRatio());
    this->setGamesPlayed(orig.getGamesPlayed());
    this->setWins(orig.getWins());
    this->setLosses(orig.getLosses());
    this->setWinPercent(orig.getWinPercent());
    this->setScore(orig.getScore());
    this->setCreepKills(orig.getCreepKills());
    this->setCreepDenies(orig.getCreepDenies());
    this->setTowerKills(orig.getTowerKills());
    this->setRaxKills(orig.getRaxKills());
    this->setCourierKills(orig.getCourierKills());

    return *this;
}

Player::~Player () { }

// <editor-fold defaultstate="collapsed" desc="Getters and setters">
double Player::getStayPercent ()
{
    if (gamesPlayed == 0)
    {
        return 0;
    }
    else
    {
        return (double) ((wins + losses) * 100) / gamesPlayed;
    }
}

unsigned char Player::getPlayerId () const
{
    return playerId;
}

void Player::setPlayerId (const unsigned char& playerId)
{
    this->playerId = playerId;
}

int Player::getAssists () const
{
    return assists;
}

void Player::setAssists (const int& assits)
{
    this->assists = assits;
}

int Player::getCourierKills () const
{
    return courierKills;
}

void Player::setCourierKills (const int& courierKills)
{
    this->courierKills = courierKills;
}

int Player::getCreepDenies () const
{
    return creepDenies;
}

void Player::setCreepDenies (const int& creepDenies)
{
    this->creepDenies = creepDenies;
}

int Player::getCreepKills () const
{
    return creepKills;
}

void Player::setCreepKills (const int& creepKills)
{
    this->creepKills = creepKills;
}

int Player::getDeaths () const
{
    return deaths;
}

void Player::setDeaths (const int& deaths)
{
    this->deaths = deaths;
}

int Player::getGamesPlayed () const
{
    return gamesPlayed;
}

void Player::setGamesPlayed (const int& gamesPlayed)
{
    this->gamesPlayed = gamesPlayed;
}

double Player::getKillDeathRatio () const
{
    return killDeathRatio;
}

void Player::setKillDeathRatio (const double& killDeathRatio)
{
    this->killDeathRatio = killDeathRatio;
}

int Player::getKills () const
{
    return kills;
}

void Player::setKills (const int& kills)
{
    this->kills = kills;
}

ColoredMessage Player::getName () const
{
    return name;
}

void Player::setName (const ColoredMessage& name)
{
    this->name = name;
}

int Player::getRaxKills () const
{
    return raxKills;
}

void Player::setRaxKills (const int& raxKills)
{
    this->raxKills = raxKills;
}

double Player::getScore () const
{
    return score;
}

void Player::setScore (const double& score)
{
    this->score = score;
}

int Player::getTowerKills () const
{
    return towerKills;
}

void Player::setTowerKills (const int& towerKills)
{
    this->towerKills = towerKills;
}

int Player::getWins () const
{
    return wins;
}

void Player::setWins (const int& wins)
{
    this->wins = wins;
}

int Player::getLosses () const
{
    return losses;
}

void Player::setLosses (const int& losses)
{
    this->losses = losses;
}

double Player::getWinPercent () const
{
    return winPercent;
}

void Player::setWinPercent (const double& winPercent)
{
    this->winPercent = winPercent;
}
// </editor-fold>
