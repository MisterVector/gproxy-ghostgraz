#ifndef FRIEND_H
#define	FRIEND_H

#include "Product.h"

#include <QObject>
#include <QMetaType>
#include <QString>

class Friend : public QObject
{
    Q_OBJECT
    Q_ENUMS(Location)
    Q_ENUMS(Status)

public:
    enum Location
    {
        NOT_AVAILABLE = 0x00,
        NOT_IN_CHANNEL = 0x01,
        IN_CHANNEL = 0x02,
        IN_PUBLIC_GAME = 0x03,
        IN_UNKNOWN_PRIVATE_GAME = 0x04, // Friend is in a private game, but you are not that person's friend.
        IN_PRIVATE_GAME = 0x05          // Friend is in a private game and you are that person's friend
    };

    enum Status
    {
        OFFLINE = 0x00,
        MUTUAL = 0x01,
        DND = 0x02,
        MUTUAL_DND = 0x03,
        AWAY = 0x04,
        MUTUAL_AWAY = 0x05,
        DND_AWAY = 0x06,
        MUTUAL_DND_AWAY = 0x07
    };

    Friend();
    Friend(const Friend& orig);
    Friend& operator= (const Friend& orig);
    virtual ~Friend();

    // <editor-fold defaultstate="collapsed" desc="Getters and setters">
    unsigned char getEntryNumber() const;
    void setEntryNumber(const unsigned char& entryNumber);
    Location getLocation() const;
    void setLocation(const Location& location);
    void setLocation(const unsigned char& location);
    QString getLocationName() const;
    void setLocationName(const QString& locationName);
    QString getName() const;
    void setName(const QString& name);
    Product getProduct() const;
    void setProduct(const Product& product);
    Status getStatus() const;
    void setStatus(const Status& status);
    void setStatus(const unsigned char& status);
    // </editor-fold>

private:
    /**
     * The internal identifier of the friend holding the display position.
     * Note that GProxy will sort online users on top, so this position most likely isn't the same.
     */
    unsigned char entryNumber;

    /**
     * The friends username.
     */
    QString name;

    /**
     * The current location of the friend.
     */
    Location location;

    /**
     * Friends status.
     */
    Status status;

    /**
     * The friends product when he logged on.
     */
    Product product;

    /**
     * Detailed location description. May be null/empty.
     */
    QString locationName;
};

Q_DECLARE_METATYPE(Friend)

#endif	/* FRIEND_H */

