#ifndef PLAYER_H
#define	PLAYER_H

#include "ColoredMessage.h"

#include <QObject>
#include <QMetaType>
#include <QString>

class Player : public QObject
{
    Q_OBJECT

public:
    Player();

    Player(const ColoredMessage& name);
    Player(const Player& orig);
    Player& operator= (const Player& orig);
    virtual ~Player();

    double getStayPercent();

    // <editor-fold defaultstate="collapsed" desc="Getters and setters">
    unsigned char getPlayerId() const;
    void setPlayerId(const unsigned char& playerId);
    int getAssists() const;
    void setAssists(const int& assists);
    int getCourierKills() const;
    void setCourierKills(const int& courierKills);
    int getCreepDenies() const;
    void setCreepDenies(const int& creepDenies);
    int getCreepKills() const;
    void setCreepKills(const int& creepKills);
    int getDeaths() const;
    void setDeaths(const int& deaths);
    int getGamesPlayed() const;
    void setGamesPlayed(const int& gamesPlayed);
    double getKillDeathRatio() const;
    void setKillDeathRatio(const double& killDeathRatio);
    int getKills() const;
    void setKills(const int& kills);
    ColoredMessage getName() const;
    void setName(const ColoredMessage& name);
    int getRaxKills() const;
    void setRaxKills(const int& raxKills);
    double getScore() const;
    void setScore(const double& score);
    int getTowerKills() const;
    void setTowerKills(const int& towerKills);
    int getWins() const;
    void setWins(const int& wins);
    int getLosses() const;
    void setLosses(const int& losses);
    double getWinPercent() const;
    void setWinPercent(const double& winPercent);
    // </editor-fold>

private:
    unsigned char playerId;
    ColoredMessage name;
    int kills;
    int deaths;
    int assists;
    double killDeathRatio;
    int gamesPlayed;
    int wins;
    int losses;
    double winPercent;
    double score;
    int creepKills;
    int creepDenies;
    int towerKills;
    int raxKills;
    int courierKills;
};

Q_DECLARE_METATYPE(Player)

#endif	/* PLAYER_H */

