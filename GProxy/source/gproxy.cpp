#include <QApplication>
#include <QSound>
#include <QElapsedTimer>
#include <QDateTime>
#include <QTextStream>
#include <QTextCodec>
#include <QMutableListIterator>

#include "gproxy.h"
#include "util/Util.h"
#include "socket.h"
#include "commandpacket.h"
#include "bnetprotocol.h"
#include "bnet.h"
#include "gameprotocol.h"
#include "gpsprotocol.h"
#include "MainGUI.h"
#include "Config.h"
#include "thread/SleepThread.h"
#include "GhostGrazLogininformationDialog.h"

#include <signal.h>
#include <stdlib.h>

#ifdef WIN32
    #include <windows.h>
    #include <winsock.h>
#endif

#undef ERROR /* Undefine ERROR macro. Needed for ColoredMessage::ERROR. */

CGProxy* gproxy;
MainGUI* mainGUI;
QElapsedTimer timer;
QFile* logFile;

bool playerLoadingComplete[12];
int finishedLoadingCounter;
int teamNumber;
unsigned int countdownEndTime;

string parseTextline (string input)
{
    if (gproxy->m_LocalSocket && (input.find("$host$") != string::npos))
    {
        input.replace(input.find("$host$"), 6, gproxy->m_HostName);
    }

    return input;
};

/**
 * Returns the elapsed seconds since GProxy start.
 *
 * @return Elapsed seconds since GProxy start.
 */
unsigned long getElapsedSeconds ()
{
    return timer.elapsed() / 1000;
}

/**
 * Returns the elapsed milliseconds since GProxy start.
 *
 * @return Elapsed milliseconds since GProxy start.
 */
unsigned long getElapsedMilliseconds ()
{
    return timer.elapsed();
}

/**
 * Prints the logMessage to the gproxy log file, if logging is enabled.
 *
 * @param logMessage The log message.
 * @param printTimestamp If the current time should be displayed before the message. Default: <code>true</code>.
 * @param lineBreak If a line break should be appended to the message. Default: <code>true</code>.
 */
void LOG_Print (const QString& logMessage, bool printTimestamp, bool lineBreak)
{
    // If logging is disabled the log file is closed.
    if (logFile->isOpen() && logFile->isWritable())
    {
        QTextStream log(logFile);
        if (printTimestamp)
        {
            QString dateTime = QLocale().toString(QDateTime::currentDateTime(),
                    "[dddd dd-MM-yyyy hh:mm:ss] ");
            log << dateTime;
        }

        log << logMessage;

        if (lineBreak)
        {
            log << "\n";
        }
        log.flush();
    }
}

void CheckForGame (string gamename)
{
    if (getautosearch())
    {
        gproxy->m_BNET->SetSearchGameName(gamename); //old one, but it works..
        CONSOLE_Print(ColoredMessage("[GPROXY] Searching for [" + QString::fromStdString(gamename) + "].", ColoredMessage::GPROXY), true);
        gproxy->autosearch = false;
    }
}

/**
 * Displays the message at the output area of GProxy.
 *
 * @param coloredMessage The message to be displayed.
 * @param log If the message should be logged. Default: <code>true</code>.
 * @param printTimestamp If the current time should be displayed before the message. Default: <code>true</code>.
 * @param lineBreak If a line break should be appended to the message. Default: <code>true</code>.
 */
void CONSOLE_Print (const ColoredMessage& coloredMessage, bool log, bool printTimestamp, bool lineBreak)
{
    gproxy->addMessage(coloredMessage, log, printTimestamp, lineBreak);
}

bool fcfgfilterfirst ()// Phyton filter
{
    return gproxy->cfgfilterfirst;

}

string fcfgfilter ()// Phyton filter
{
    return gproxy->cfgfilter;
}

bool getautosearch () //pr0 autosearch
{
    return gproxy->autosearch;
}

void autosearch (bool autosearchNew) //pr0 autosearch
{
    gproxy->autosearch = autosearchNew;
}

bool cautosearch () //pr0 cautosearch
{
    return gproxy->cautosearch;
}

bool displayautocreated ()
{
    return gproxy->displayautocreated;
}

void displayautocreated (bool newone)
{
    gproxy->displayautocreated = newone;
}

void flisting_current_games (bool newone)
{
    gproxy->m_listing_current_games = newone;
}

bool flisting_current_games ()
{
    return gproxy->m_listing_current_games;
}

/**
 * The main function.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @return The exit code.
 */
int main (int argc, char** argv)
{
    QApplication app(argc, argv);

    gproxy = new CGProxy();

    mainGUI = new MainGUI(gproxy);
    mainGUI->show();

    gproxy->connectSignalsAndSlots();

    // For printing the date to the log file in English format.
    QLocale::setDefault(QLocale::English);
    // Support special characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    // For getting the elapsed time since application start.
    timer.start();

    logFile = new QFile("gproxy_log.txt");

    // Initialize config.
    Config* config = new Config("gproxy.cfg");
    gproxy->setConfig(config);

    // Load config.
    int statusCode = config->loadConfig();
    bool connect = gproxy->checkStatus(statusCode);

    // Init mainGUI.
    mainGUI->init();

    QObject::connect(config, SIGNAL(configSaved()),
            gproxy, SLOT(applyConfig()), Qt::QueuedConnection);

    gproxy->initConfigurations();

#ifdef WIN32
    // Initialize winsock
    CONSOLE_Print(ColoredMessage("[GPROXY] Starting winsock", ColoredMessage::GPROXY));
    WSADATA wsadata;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        gproxy->showErrorMessage("Error starting winsock!");
        return 1;
    }

    // Increase process priority
    CONSOLE_Print(ColoredMessage("[GPROXY] setting process priority to \"above normal\"", ColoredMessage::GPROXY));
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
#endif

    gproxy->showWelcomeMessages();

    // FIXME Remove unnecessary parameters.
    gproxy->initVariables("", "", true, false, connect);

    return app.exec();
}

/**
 * Empty Constructor.
 */
CGProxy::CGProxy () { }

/**
 * Deconstructor
 */
CGProxy::~CGProxy ()
{
    for (vector<CIncomingGameHost* > ::iterator i = m_Games.begin(); i != m_Games.end(); ++i)
    {
        m_UDPSocket->Broadcast(6112, m_GameProtocol->SEND_W3GS_DECREATEGAME(
                ((CIncomingGameHost*) (*i))->GetUniqueGameID()));
    }

    for (QList<Slot*>::iterator i = slotList.begin(); i != slotList.end(); ++i)
    {
        delete *i;
    }
    slotList.clear();

    for (QList<Player*> ::iterator i = players.begin(); i != players.end(); ++i)
    {
        delete *i;
    }
    players.clear();

    delete config;
    delete m_LocalServer;
    delete m_LocalSocket;
    delete m_RemoteSocket;
    delete m_UDPSocket;
    delete m_BNET;
    delete lastLeaver;

    for (vector<CIncomingGameHost*> ::iterator i = m_Games.begin(); i != m_Games.end(); i++)
    {
        delete *i;
    }
    m_Games.clear();

    delete m_GameProtocol;
    delete m_GPSProtocol;

    while (!m_LocalPackets.empty())
    {
        delete m_LocalPackets.front();
        m_LocalPackets.pop();
    }

    while (!m_RemotePackets.empty())
    {
        delete m_RemotePackets.front();
        m_RemotePackets.pop();
    }

    while (!m_PacketBuffer.empty())
    {
        delete m_PacketBuffer.front();
        m_PacketBuffer.pop();
    }
}

/**
 * Connect all necessary signals and slots.
 */
void CGProxy::connectSignalsAndSlots()
{
    qRegisterMetaType<Slot*>("Slot*");
    qRegisterMetaType< QList<Slot*> >("QList<Slot*>");
    qRegisterMetaType<Friend*>("Friend*");
    qRegisterMetaType< QList<Friend*> >("QList<Friend*>");
    qRegisterMetaType<ColoredMessage>("ColoredMessage");

    connect(this, SIGNAL(signal_startUpdateThread()),
            mainGUI, SLOT(startUpdateThread()), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_addMessage(ColoredMessage, bool, bool, bool)),
            mainGUI, SLOT(addMessage(ColoredMessage, bool, bool, bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_changeChannel(QString)),
            mainGUI, SLOT(changeChannel(QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_addChannelUser(QString, QString)),
            mainGUI, SLOT(addChannelUser(QString, QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_removeChannelUser(QString)),
            mainGUI, SLOT(removeChannelUser(QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_updateFriendlist(QList<Friend*>)),
            mainGUI, SLOT(updateFriendlist(QList<Friend*>)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_updateFriend(Friend*)),
            mainGUI, SLOT(updateFriend(Friend*)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_addFriend(Friend*)),
            mainGUI, SLOT(addFriend(Friend*)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_removeFriend(unsigned char)),
            mainGUI, SLOT(removeFriend(unsigned char)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_setGameslots(QList<Slot*>)),
            mainGUI, SLOT(setGameslots(QList<Slot*>)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_showErrorMessage(QString)),
            mainGUI, SLOT(showErrorMessage(QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_playerJoined(const ColoredMessage&)),
            mainGUI, SLOT(playerJoined(const ColoredMessage&)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_showConfigDialog(bool)),
            mainGUI, SLOT(showConfigDialog(bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_initConfigurations()),
            mainGUI, SLOT(initConfigurations()), Qt::QueuedConnection);

    connect(this, SIGNAL(signal_applyConfig()),
            mainGUI, SLOT(applyConfig()), Qt::QueuedConnection);
}

/**
 * Initializes all variables.
 *
 * @param cpublic
 * @param cfilter
 * @param temp_displayautocreated
 * @param listing_current_games
 * @param status
 */
void CGProxy::initVariables (string cpublic, string cfilter, bool temp_displayautocreated,
        bool listing_current_games, bool connect)
{
    m_Version = mainGUI->windowTitle().remove(0, 7).toStdString();
    m_LocalServer = new CTCPServer();
    m_LocalSocket = NULL;
    m_RemoteSocket = new CTCPClient();
    m_RemoteSocket->SetNoDelay(true);
    m_UDPSocket = new CUDPSocket();
    m_UDPSocket->SetBroadcastTarget("127.0.0.1");
    m_GameProtocol = new CGameProtocol(this);
    m_GPSProtocol = new CGPSProtocol();
    m_TotalPacketsReceivedFromLocal = 0;
    m_TotalPacketsReceivedFromRemote = 0;
    m_Exiting = false;
    m_TFT = !(this->getCDKeyTFT().isEmpty());
    m_War3Path = this->getWar3Path().toStdString();
    m_CDKeyROC = this->getCDKeyROC().toStdString();
    m_CDKeyTFT = this->getCDKeyTFT().toStdString();
    m_LastConnectionAttemptTime = 0;
    m_LastRefreshTime = 0;
    m_RemoteServerPort = 0;
    m_GameIsReliable = false;
    m_GameStarted = false;
    m_LeaveGameSent = false;
    m_ActionReceived = false;
    m_Synchronized = true;
    m_ReconnectPort = 0;
    m_PID = 255;
    m_ChatPID = 255;
    m_ReconnectKey = 0;
    m_NumEmptyActions = 0;
    m_NumEmptyActionsUsed = 0;
    m_LastAckTime = 0;
    m_LastActionTime = 0;
    m_BNET = new CBNET(this, mainGUI, server.toStdString(), string(), 0, 0,
            "USA", "United States", username.toStdString(), password.toStdString(),
            channel.toStdString(), war3version, exeversion, exeversionhash,
            passwordhashtype.toStdString(), 200);
    m_LocalServer->Listen(string(), port);
    cfgfilterfirst = true; // Phyton filter
    cfgfilter = cfilter; // Phyton filter
    cfgpublic = cpublic; // Phyton public
    autosearch = true; //pr0 autosearch
    displayautocreated = temp_displayautocreated;
    m_listing_current_games = listing_current_games;
    testvar = false;
    m_PlaySound = true; // manu
    m_joinStats = true;
    vShallCreate = false;
    vShallCreateQuiet = false;
    parrot = "";
    dotaMap = false;
    lastLeaver = new Player(username);

    if (connect)
    {
        emit signal_startUpdateThread();
    }

    CONSOLE_Print(ColoredMessage("[GPROXY] Customized GProxy++ Version " + QString::fromStdString(m_Version), ColoredMessage::GPROXY));
}

void CGProxy::initConfigurations()
{
    emit signal_initConfigurations();
    applyConfig();
}

/**
 * Cleanup function before exiting. (Deleting all pointers)
 */
void CGProxy::cleanup ()
{
#ifdef WIN32
    CONSOLE_Print(ColoredMessage("[GPROXY] shutting down winsock", ColoredMessage::GPROXY));
    WSACleanup();
#endif

    CONSOLE_Print(ColoredMessage("[GPROXY] shutting down", ColoredMessage::GPROXY));

    if (logFile->isOpen())
    {
        logFile->close();
    }
    delete logFile;

    delete mainGUI;
    delete gproxy;
}

/**
 * Initializes all variables from the config.
 */
void CGProxy::applyConfig ()
{
    if (!logFile->isOpen() && config->getBoolean("log"))
    {
        if (!logFile->open(QFile::Append | QFile::Text))
        {
            gproxy->showErrorMessage("Could not open logfile: " + logFile->errorString());
        }
    }

    gproxy->setWar3Path(config->getString("war3path"));
    gproxy->setCDKeyROC(config->getString("cdkeyroc"));
    gproxy->setCDKeyTFT(config->getString("cdkeytft"));
    gproxy->setServer(config->getString("server"));
    gproxy->setCdKeyOwner(config->getString("cdkeysowner"));
    
    gproxy->setPrivategamename(config->getString("privategamename"));
    gproxy->setBotprefix(config->getString("botprefix"));
    cautosearch = config->getBoolean("autosearch");
    gproxy->m_PlaySound = config->getBoolean("sound");
    gproxy->m_joinStats = config->getBoolean("joinstats");
    gproxy->setUsername(config->getString("username"));
    gproxy->setPassword(config->getString("password"));
    gproxy->setChannel(config->getString("channel"));
    gproxy->setWar3version(config->getInt("war3version"));
    gproxy->setPort(config->getInt("port"));
    gproxy->setExeversion(Util::extractNumbers(config->getString("exeversion").toStdString(), 4));
    gproxy->setExeversionhash(Util::extractNumbers(config->getString("exeversionhash").toStdString(), 4));
    gproxy->setPasswordhashtype(config->getString("passwordhashtype"));

    emit signal_applyConfig();
}

void CGProxy::showWelcomeMessages()
{
    CONSOLE_Print(ColoredMessage(""), false);
    CONSOLE_Print(ColoredMessage("  Welcome to GProxy++."), false);
    CONSOLE_Print(ColoredMessage("  Server: " + gproxy->getServer()), false);
    CONSOLE_Print(ColoredMessage("  Username: " + gproxy->getUsername()), false);
    CONSOLE_Print(ColoredMessage("  Channel: " + gproxy->getChannel()), false);
    CONSOLE_Print(ColoredMessage(""), false);
    CONSOLE_Print(ColoredMessage("  Type /help at any time for help."), false);
    CONSOLE_Print(ColoredMessage(""), false);

    CONSOLE_Print(ColoredMessage("Welcome to "), false, true, false);
    CONSOLE_Print(ColoredMessage("GProxy++", ColoredMessage::GPROXY), false, false, false);
    CONSOLE_Print(ColoredMessage(". This mod is by "), false, false, false);
    CONSOLE_Print(ColoredMessage("Phyton", ColoredMessage::WHISPER), false, false, false);
    CONSOLE_Print(ColoredMessage(", "), false, false, false);
    CONSOLE_Print(ColoredMessage("Pr0gm4n", ColoredMessage::INFO), false, false, false);
    CONSOLE_Print(ColoredMessage(", "), false, false, false);
    CONSOLE_Print(ColoredMessage("Noman(1)", ColoredMessage::ERROR), false, false, false);
    CONSOLE_Print(ColoredMessage(" and "), false, false, false);
    CONSOLE_Print(ColoredMessage("Manufactoring", ColoredMessage::GAMEINFO), false, false, false);
    CONSOLE_Print(ColoredMessage("."), false, false, true);
}

/**
 * Emits the signal <code>signal_addMessage</code>.
 * Displays the message at the output area of GProxy.
 *
 * @param coloredMessage The message to be displayed.
 * @param log If the message should be logged. Default: <code>true</code>.
 * @param printTimestamp If the current time should be displayed before the message. Default: <code>true</code>.
 * @param lineBreak If a line break should be appended to the message. Default: <code>true</code>.
 * @see MainGUI#addMessage
 */
void CGProxy::addMessage (const ColoredMessage& coloredMessage, bool log, bool printTimestamp, bool lineBreak)
{
    emit signal_addMessage(coloredMessage, log, printTimestamp, lineBreak);
}

/**
 * Emits the signal <code>signal_changeChannel</code>.
 *
 * @param channel
 * @see MainGUI#changeChannel
 */
void CGProxy::changeChannel (QString channel)
{
    this->m_GameStarted = false;
    emit signal_changeChannel(channel);
}

/**
 * Emits the signal <code>signal_addChannelUser</code>.
 *
 * @param username
 * @param clanTag
 * @see MainGUI#addChannelUser
 */
void CGProxy::addChannelUser (QString username, QString clanTag)
{
    // Phyton waitgame
    if (gproxy->getVShallCreate())
    {
        QString tname = username.toLower();
        QString tpre = gproxy->getBotprefix().toLower();

        if (gproxy->getBotprefix().size() > 0 && gproxy->getBotprefix()[0] != "^"[0])
            if (gproxy->getPrivategamename() != "" && tpre != "" && tname.mid(0, tpre.size()) == tpre)
            {
                gproxy->m_BNET->QueueChatCommand("/w " + username
                        + " !priv " + gproxy->getPrivategamename());
                autosearch = true;
                CheckForGame(gproxy->getPrivategamename().toStdString());
                if (!gproxy->getVShallCreateQuiet())
                {
                    gproxy->m_BNET->QueueChatCommand("gn: " + gproxy->getPrivategamename());
                }
                gproxy->setVShallCreate(false);
            }
            else;
        else
        {
            if (gproxy->getPrivategamename() != "" && tname == tpre.mid(1, tpre.size() - 1))
            {
                autosearch = true;
                gproxy->m_BNET->QueueChatCommand("/w " + username
                        + " !priv " + gproxy->getPrivategamename());
                CheckForGame(gproxy->getPrivategamename().toStdString());
                if (!gproxy->getVShallCreateQuiet())
                {
                    gproxy->m_BNET->QueueChatCommand("gn: " + gproxy->getPrivategamename());
                }
                gproxy->setVShallCreate(false);
            }
        }
    }

    emit signal_addChannelUser(username, clanTag);
}

/**
 * Emits the signal <code>removeChannelUser</code>.
 *
 * @param username
 * @see MainGUI#removeChannelUser
 */
void CGProxy::removeChannelUser (QString username)
{
    emit signal_removeChannelUser(username);
}

/**
 * Emits the signal <code>updateFriendlist</code>.
 *
 * @param friends List of friends.
 */
void CGProxy::updateFriendlist(QList<Friend*> friends)
{
    emit signal_updateFriendlist(friends);
}

void CGProxy::updateFriend(Friend* f)
{
    emit signal_updateFriend(f);
}

void CGProxy::addFriend(Friend* f)
{
    emit signal_addFriend(f);
}

void CGProxy::removeFriend(unsigned char entryNumber)
{
    emit signal_removeFriend(entryNumber);
}

/**
 * Emits the signal <code>signal_showErrorMessage</code>.
 *
 * @see MainGUI#showErrorMessage
 */
void CGProxy::showErrorMessage (QString errorMessage)
{
    emit signal_showErrorMessage(errorMessage);
}

bool CGProxy::Update (long usecBlock)
{
    unsigned int NumFDs = 0;

    // take every socket we own and throw it in one giant select statement so we can block on all sockets

    int nfds = 0;
    fd_set fd;
    fd_set send_fd;
    FD_ZERO(&fd);
    FD_ZERO(&send_fd);

    // 1. the battle.net socket

    NumFDs += m_BNET->SetFD(&fd, &send_fd, &nfds);

    // 2. the local server

    m_LocalServer->SetFD(&fd, &send_fd, &nfds);
    NumFDs++;

    // 3. the local socket

    if (m_LocalSocket)
    {
        m_LocalSocket->SetFD(&fd, &send_fd, &nfds);
        NumFDs++;
    }

    // 4. the remote socket

    if (!m_RemoteSocket->HasError() && m_RemoteSocket->GetConnected())
    {
        m_RemoteSocket->SetFD(&fd, &send_fd, &nfds);
        NumFDs++;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usecBlock;

    struct timeval send_tv;
    send_tv.tv_sec = 0;
    send_tv.tv_usec = 0;

#ifdef WIN32
    select(1, &fd, NULL, NULL, &tv);
    select(1, NULL, &send_fd, NULL, &send_tv);
#else
    select(nfds + 1, &fd, NULL, NULL, &tv);
    select(nfds + 1, NULL, &send_fd, NULL, &send_tv);
#endif

    if (NumFDs == 0)
    {
        SleepThread::msleep(50);
    }

    if (m_BNET->Update(&fd, &send_fd))
        return true;

    //
    // accept new connections
    //

    CTCPSocket *NewSocket = m_LocalServer->Accept(&fd);

    if (NewSocket)
    {
        if (m_LocalSocket)
        {
            // someone's already connected, reject the new connection
            // we only allow one person to use the proxy at a time

            delete NewSocket;
        }
        else
        {
            CONSOLE_Print(ColoredMessage("[GPROXY] local player connected", ColoredMessage::GPROXY));
            m_LocalSocket = NewSocket;
            m_LocalSocket->SetNoDelay(true);
            m_TotalPacketsReceivedFromLocal = 0;
            m_TotalPacketsReceivedFromRemote = 0;
            m_GameIsReliable = false;
            m_GameStarted = false;
            m_LeaveGameSent = false;
            m_ActionReceived = false;
            m_Synchronized = true;
            m_ReconnectPort = 0;
            m_PID = 255;
            m_ChatPID = 255;
            m_ReconnectKey = 0;
            m_NumEmptyActions = 0;
            m_NumEmptyActionsUsed = 0;
            m_LastAckTime = 0;
            m_LastActionTime = 0;
            m_JoinedName.clear();
            m_HostName.clear();

            while (!m_PacketBuffer.empty())
            {
                delete m_PacketBuffer.front();
                m_PacketBuffer.pop();
            }
        }
    }

    if (m_LocalSocket)
    {
        //
        // handle proxying (reconnecting, etc...)
        //

        if (m_LocalSocket->HasError() || !m_LocalSocket->GetConnected())
        {
            CONSOLE_Print(ColoredMessage("[GPROXY] local player disconnected", ColoredMessage::GPROXY));

            if (m_BNET->GetInGame())
                m_BNET->QueueEnterChat();

            delete m_LocalSocket;
            m_LocalSocket = NULL;

            // ensure a leavegame message was sent, otherwise the server may wait for our reconnection which will never happen
            // if one hasn't been sent it's because Warcraft III exited abnormally

            if (m_GameIsReliable && !m_LeaveGameSent)
            {
                // note: we're not actually 100% ensuring the leavegame message is sent, we'd need to check that DoSend worked, etc...

                BYTEARRAY LeaveGame;
                LeaveGame.push_back(0xF7);
                LeaveGame.push_back(0x21);
                LeaveGame.push_back(0x08);
                LeaveGame.push_back(0x00);
                Util::appendByteArray(LeaveGame, (uint32_t) PLAYERLEAVE_GPROXY, false);
                m_RemoteSocket->PutBytes(LeaveGame);
                m_RemoteSocket->DoSend(&send_fd);
            }

            m_RemoteSocket->Reset();
            m_RemoteSocket->SetNoDelay(true);
            m_RemoteServerIP.clear();
            m_RemoteServerPort = 0;
        }
        else
        {
            m_LocalSocket->DoRecv(&fd);
            ExtractLocalPackets();
            ProcessLocalPackets();

            if (!m_RemoteServerIP.empty())
            {
                if (m_GameIsReliable && m_ActionReceived
                        && getElapsedSeconds() - m_LastActionTime >= 60)
                {
                    if (m_NumEmptyActionsUsed < m_NumEmptyActions)
                    {
                        SendEmptyAction();
                        m_NumEmptyActionsUsed++;
                    }
                    else
                    {
                        SendLocalChat("GProxy++ ran out of time to reconnect, Warcraft III will disconnect soon.");
                        CONSOLE_Print(ColoredMessage("[GPROXY] ran out of time to reconnect", ColoredMessage::GPROXY));
                    }

                    m_LastActionTime = getElapsedSeconds();
                }

                if (m_RemoteSocket->HasError())
                {
                    CONSOLE_Print(ColoredMessage("[GPROXY] disconnected from remote server due to socket error", ColoredMessage::GPROXY));

                    if (m_GameIsReliable && m_ActionReceived && m_ReconnectPort > 0)
                    {
                        SendLocalChat("You have been disconnected from the server due to a socket error.");
                        uint32_t TimeRemaining = (m_NumEmptyActions
                                - m_NumEmptyActionsUsed + 1) * 60
                                - (getElapsedSeconds() - m_LastActionTime);

                        if (getElapsedSeconds() - m_LastActionTime
                                > (((unsigned int) m_NumEmptyActions)
                                - ((unsigned int) m_NumEmptyActionsUsed) + 1) * 60)
                        {
                            TimeRemaining = 0;
                        }

                        SendLocalChat("GProxy++ is attempting to reconnect... (" + QString::number(TimeRemaining) + " seconds remain)");
                        CONSOLE_Print(ColoredMessage("[GPROXY] attempting to reconnect", ColoredMessage::GPROXY));
                        m_RemoteSocket->Reset();
                        m_RemoteSocket->SetNoDelay(true);
                        m_RemoteSocket->Connect(string(), m_RemoteServerIP, m_ReconnectPort);
                        m_LastConnectionAttemptTime = getElapsedSeconds();
                    }
                    else
                    {
                        if (m_BNET->GetInGame())
                            m_BNET->QueueEnterChat();

                        m_LocalSocket->Disconnect();
                        delete m_LocalSocket;
                        m_LocalSocket = NULL;
                        m_RemoteSocket->Reset();
                        m_RemoteSocket->SetNoDelay(true);
                        m_RemoteServerIP.clear();
                        m_RemoteServerPort = 0;
                        return false;
                    }
                }

                if (!m_RemoteSocket->GetConnecting() && !m_RemoteSocket->GetConnected())
                {
                    CONSOLE_Print(ColoredMessage("[GPROXY] disconnected from remote server", ColoredMessage::GPROXY));

                    if (m_GameIsReliable && m_ActionReceived && m_ReconnectPort > 0)
                    {
                        SendLocalChat("You have been disconnected from the server.");
                        uint32_t TimeRemaining = (m_NumEmptyActions
                                - m_NumEmptyActionsUsed + 1) * 60
                                - (getElapsedSeconds() - m_LastActionTime);

                        if (getElapsedSeconds() - m_LastActionTime
                                > (((unsigned int) m_NumEmptyActions)
                                - ((unsigned int) m_NumEmptyActionsUsed) + 1) * 60)
                        {
                            TimeRemaining = 0;
                        }

                        SendLocalChat("GProxy++ is attempting to reconnect... (" + QString::number(TimeRemaining) + " seconds remain)");
                        CONSOLE_Print(ColoredMessage("[GPROXY] attempting to reconnect", ColoredMessage::GPROXY));
                        m_RemoteSocket->Reset();
                        m_RemoteSocket->SetNoDelay(true);
                        m_RemoteSocket->Connect(string(), m_RemoteServerIP, m_ReconnectPort);
                        m_LastConnectionAttemptTime = getElapsedSeconds();
                    }
                    else
                    {
                        if (m_BNET->GetInGame())
                            m_BNET->QueueEnterChat();

                        m_LocalSocket->Disconnect();
                        delete m_LocalSocket;
                        m_LocalSocket = NULL;
                        m_RemoteSocket->Reset();
                        m_RemoteSocket->SetNoDelay(true);
                        m_RemoteServerIP.clear();
                        m_RemoteServerPort = 0;
                        return false;
                    }
                }

                if (m_RemoteSocket->GetConnected())
                {
                    if (m_GameIsReliable && m_ActionReceived && m_ReconnectPort > 0
                            && getElapsedSeconds() - m_RemoteSocket->GetLastRecv() >= 20)
                    {
                        CONSOLE_Print(ColoredMessage("[GPROXY] disconnected from remote server due to 20 second timeout", ColoredMessage::GPROXY));
                        SendLocalChat("You have been timed out from the server.");
                        uint32_t TimeRemaining = (m_NumEmptyActions
                                - m_NumEmptyActionsUsed + 1) * 60
                                - (getElapsedSeconds() - m_LastActionTime);

                        if (getElapsedSeconds() - m_LastActionTime
                                > ((unsigned int) (m_NumEmptyActions
                                - m_NumEmptyActionsUsed + 1)) * 60)
                        {
                            TimeRemaining = 0;
                        }

                        SendLocalChat("GProxy++ is attempting to reconnect... (" + QString::number(TimeRemaining) + " seconds remain)");
                        CONSOLE_Print(ColoredMessage("[GPROXY] attempting to reconnect", ColoredMessage::GPROXY));
                        m_RemoteSocket->Reset();
                        m_RemoteSocket->SetNoDelay(true);
                        m_RemoteSocket->Connect(string(), m_RemoteServerIP, m_ReconnectPort);
                        m_LastConnectionAttemptTime = getElapsedSeconds();
                    }
                    else
                    {
                        m_RemoteSocket->DoRecv(&fd);
                        ExtractRemotePackets();
                        ProcessRemotePackets();

                        if (m_GameIsReliable && m_ActionReceived && m_ReconnectPort > 0
                                && getElapsedSeconds() - m_LastAckTime >= 10)
                        {
                            m_RemoteSocket->PutBytes(m_GPSProtocol->SEND_GPSC_ACK(m_TotalPacketsReceivedFromRemote));
                            m_LastAckTime = getElapsedSeconds();
                        }

                        m_RemoteSocket->DoSend(&send_fd);
                    }
                }

                if (m_RemoteSocket->GetConnecting())
                {
                    // we are currently attempting to connect

                    if (m_RemoteSocket->CheckConnect())
                    {
                        // the connection attempt completed

                        if (m_GameIsReliable && m_ActionReceived)
                        {
                            // this is a reconnection, not a new connection
                            // if the server accepts the reconnect request it will send a GPS_RECONNECT back requesting a certain number of packets

                            SendLocalChat("GProxy++ reconnected to the server!");
                            SendLocalChat("==================================================");
                            CONSOLE_Print(ColoredMessage("[GPROXY] reconnected to remote server", ColoredMessage::GPROXY));

                            // note: even though we reset the socket when we were disconnected, we haven't been careful to ensure we never queued any data in the meantime
                            // therefore it's possible the socket could have data in the send buffer
                            // this is bad because the server will expect us to send a GPS_RECONNECT message first
                            // so we must clear the send buffer before we continue
                            // note: we aren't losing data here, any important messages that need to be sent have been put in the packet buffer
                            // they will be requested by the server if required

                            m_RemoteSocket->ClearSendBuffer();
                            m_RemoteSocket->PutBytes(m_GPSProtocol->SEND_GPSC_RECONNECT(m_PID, m_ReconnectKey, m_TotalPacketsReceivedFromRemote));

                            // we cannot permit any forwarding of local packets until the game is synchronized again
                            // this will disable forwarding and will be reset when the synchronization is complete

                            m_Synchronized = false;
                        }
                        else
                            CONSOLE_Print(ColoredMessage("[GPROXY] connected to remote server", ColoredMessage::GPROXY));
                    }
                    else if (getElapsedSeconds() - m_LastConnectionAttemptTime >= 10)
                    {
                        // the connection attempt timed out (10 seconds)

                        CONSOLE_Print(ColoredMessage("[GPROXY] connect to remote server timed out", ColoredMessage::GPROXY));

                        if (m_GameIsReliable && m_ActionReceived && m_ReconnectPort > 0)
                        {
                            uint32_t TimeRemaining = (m_NumEmptyActions
                                    - m_NumEmptyActionsUsed + 1) * 60
                                    - (getElapsedSeconds() - m_LastActionTime);

                            if (getElapsedSeconds() - m_LastActionTime
                                    > ((unsigned int) (m_NumEmptyActions
                                    - m_NumEmptyActionsUsed + 1)) * 60)
                            {
                                TimeRemaining = 0;
                            }

                            SendLocalChat("GProxy++ is attempting to reconnect... (" + QString::number(TimeRemaining) + " seconds remain)");
                            CONSOLE_Print(ColoredMessage("[GPROXY] attempting to reconnect", ColoredMessage::GPROXY));
                            m_RemoteSocket->Reset();
                            m_RemoteSocket->SetNoDelay(true);
                            m_RemoteSocket->Connect(string(), m_RemoteServerIP, m_ReconnectPort);
                            m_LastConnectionAttemptTime = getElapsedSeconds();
                        }
                        else
                        {
                            if (m_BNET->GetInGame())
                                m_BNET->QueueEnterChat();

                            m_LocalSocket->Disconnect();
                            delete m_LocalSocket;
                            m_LocalSocket = NULL;
                            m_RemoteSocket->Reset();
                            m_RemoteSocket->SetNoDelay(true);
                            m_RemoteServerIP.clear();
                            m_RemoteServerPort = 0;
                            return false;
                        }
                    }
                }
            }

            m_LocalSocket->DoSend(&send_fd);
        }
    }
    else
    {
        //
        // handle game listing
        //

        if (getElapsedSeconds() - m_LastRefreshTime >= 2)
        {
            for (vector<CIncomingGameHost*> ::iterator i = m_Games.begin(); i != m_Games.end();)
            {
                // expire games older than 60 seconds *

                if (getElapsedSeconds() - ((CIncomingGameHost*) (*i))->GetReceivedTime() >= 60)
                {
                    // don't forget to remove it from the LAN list first

                    m_UDPSocket->Broadcast(6112, m_GameProtocol->SEND_W3GS_DECREATEGAME(((CIncomingGameHost*) (*i))->GetUniqueGameID()));
                    delete *i;
                    i = m_Games.erase(i);
                    continue;
                }

                BYTEARRAY MapGameType;
                Util::appendByteArray(MapGameType, ((CIncomingGameHost*) (*i))->GetGameType(), false);
                Util::appendByteArray(MapGameType, ((CIncomingGameHost*) (*i))->GetParameter(), false);
                BYTEARRAY MapFlags = Util::createByteArray(((CIncomingGameHost*) (*i))->GetMapFlags(), false);
                BYTEARRAY MapWidth = Util::createByteArray(((CIncomingGameHost*) (*i))->GetMapWidth(), false);
                BYTEARRAY MapHeight = Util::createByteArray(((CIncomingGameHost*) (*i))->GetMapHeight(), false);
                string GameName = ((CIncomingGameHost*) (*i))->GetGameName();

                // colour reliable game names so they're easier to pick out of the list

                if (((CIncomingGameHost*) (*i))->GetMapWidth() == 1984 &&
                        ((CIncomingGameHost*) (*i))->GetMapHeight() == 1984)
                {
                    if (GameName == gproxy->getPrivategamename().toStdString())
                    {
                        GameName = "|cFFFF0000" + GameName;
                    }
                    else if (((CIncomingGameHost*) (*i))->GetHostName().substr(0, 9) == "GhostGraz")
                    {
                        // Phyton ghostgrazgame orange
                        GameName = "|cFFFF9F40" + GameName;
                    }
                    else if (GameName.substr(0, 3) == "thr")
                    {
                        if (GameName.substr(0, 4) == "thr-")
                        {
                            GameName = "|cFF00FF00" + GameName;
                        }
                        else if (GameName.substr(0, 5) == "thr2-")
                        {
                            GameName = "|cFFCCCC00" + GameName;
                        }
                        else if (GameName.substr(0, 5) == "thr3-")
                        {
                            GameName = "|cFFFF0000" + GameName;
                        }
                    }
                    else
                    {
                        GameName = "|cFF4080C0" + GameName;
                    }

                    // unfortunately we have to truncate them
                    // is this acceptable?

                    if (GameName.size() > 31)
                        GameName = GameName.substr(0, 31);
                }

                m_UDPSocket->Broadcast(6112, m_GameProtocol->SEND_W3GS_GAMEINFO(m_TFT, war3version, MapGameType, MapFlags, MapWidth, MapHeight, GameName, (*i)->GetHostName(), (*i)->GetElapsedTime(), (*i)->GetMapPath(), (*i)->GetMapCRC(), 12, 12, port, (*i)->GetUniqueGameID(), (*i)->GetUniqueGameID()));
                i++;
            }

            m_LastRefreshTime = getElapsedSeconds();
        }
    }

    return m_Exiting;
}

void CGProxy::ExtractLocalPackets ()
{
    if (!m_LocalSocket)
        return;

    string *RecvBuffer = m_LocalSocket->GetBytes();
    BYTEARRAY Bytes = Util::createByteArray((unsigned char *) RecvBuffer->c_str(), RecvBuffer->size());

    // a packet is at least 4 bytes so loop as long as the buffer contains 4 bytes
    while (Bytes.size() >= 4)
    {
        // byte 0 is always 247
        if (Bytes[0] == W3GS_HEADER_CONSTANT)
        {
            // bytes 2 and 3 contain the length of the packet
            uint16_t Length = Util::byteArrayToUInt16(Bytes, false, 2);

            if (Length >= 4)
            {
                if (Bytes.size() >= Length)
                {
                    // we have to do a little bit of packet processing here
                    // this is because we don't want to forward any chat messages that start with a "/" as these may be forwarded to battle.net instead
                    // in fact we want to pretend they were never even received from the proxy's perspective

                    bool Forward = true;
                    BYTEARRAY Data = BYTEARRAY(Bytes.begin(), Bytes.begin() + Length);

                    if (Bytes[1] == CGameProtocol::W3GS_CHAT_TO_HOST)
                    {
                        if (Data.size() >= 5)
                        {
                            unsigned int i = 5;
                            unsigned char Total = Data[4];

                            BYTEARRAY toPIDs = BYTEARRAY(Data.begin() + i, Data.begin() + i + Total);
                            //                            int FromPID = (int) Data[i];

                            if (Total > 0 && Data.size() >= i + Total)
                            {
                                i += Total;
                                unsigned char Flag = Data[i + 1];
                                i += 2;

                                BYTEARRAY ExtraFlags;
                                string MessageString;

                                if (Flag == 16)
                                {
                                    // Chat message
                                    BYTEARRAY Message = Util::extractNTCString(Data, i);
                                    MessageString = string(Message.begin(), Message.end());
                                }
                                else if (Flag == 32)
                                {
                                    // Extra flags
                                    ExtraFlags = BYTEARRAY(Data.begin() + i, Data.begin() + i + 4);
                                    BYTEARRAY Message = Util::extractNTCString(Data, i + 4);
                                    MessageString = string(Message.begin(), Message.end());
                                }

                                if (Flag == 16 || Flag == 32) // Chat message
                                {
                                    if (Flag == 16)
                                    {
                                        CONSOLE_Print(ColoredMessage("[LOBBY]["), true, true, false);
                                        CONSOLE_Print(ColoredMessage(username, ColoredMessage::USERCOLOR), true, false, false);
                                        CONSOLE_Print(ColoredMessage("] " + QString::fromStdString(MessageString)), true, false, true);
                                    }
                                    else // GameStarted
                                    {
                                        // ExtraFlags[0] stores the type of chat message as defined by Warcraft III.
                                        if (ExtraFlags[0] == 0) // 0 is an All message
                                        {
                                            CONSOLE_Print(ColoredMessage("[ALL][" + username + "] " + QString::fromStdString(MessageString)));
                                        }
                                        else if (ExtraFlags[0] == 1) // 1 is an Allies message
                                        {
                                            CONSOLE_Print(ColoredMessage("[ALLY][" + username + "] " + QString::fromStdString(MessageString)));
                                        }
                                        else if (ExtraFlags[0] == 2) // 2 is an Observer/Referee message
                                        {
                                            CONSOLE_Print(ColoredMessage("[OBSERVER][" + username + "] " + QString::fromStdString(MessageString)));
                                        }
                                        else if (ExtraFlags[0] >= 3) // 3+ are private messages
                                        {
                                            // Based on limited testing it seems that
                                            // the extra flags' first byte contains 3 plus the recipient's colour to denote a private message.
                                            // Due to the fact that the Datapacket also contains the FromPID, we do not need to extract the color.

                                            CONSOLE_Print(ColoredMessage("[PRIVATE][TO][" + username + "] " + QString::fromStdString(MessageString)));
                                        }
                                    }
                                }

                                Forward = CheckForwarding(QString::fromStdString(MessageString));
                            }
                        }
                    }

                    if (Forward)
                    {
                        m_LocalPackets.push(new CCommandPacket(W3GS_HEADER_CONSTANT, Bytes[1], Data));
                        m_PacketBuffer.push(new CCommandPacket(W3GS_HEADER_CONSTANT, Bytes[1], Data));
                        m_TotalPacketsReceivedFromLocal++;
                    }

                    *RecvBuffer = RecvBuffer->substr(Length);
                    Bytes = BYTEARRAY(Bytes.begin() + Length, Bytes.end());
                }
                else
                    return;
            }
            else
            {
                CONSOLE_Print(ColoredMessage("[GPROXY] received invalid packet from local player (bad length)", ColoredMessage::GPROXY));
                m_Exiting = true;
                return;
            }
        }
        else
        {
            CONSOLE_Print(ColoredMessage("[GPROXY] received invalid packet from local player (bad header constant)", ColoredMessage::GPROXY));
            m_Exiting = true;
            return;
        }
    }
}

bool CGProxy::CheckForwarding (QString message)
{
    bool forward = true;
    QString command = message.toLower();

    if (command.length() >= 1 && command.startsWith("/"))
    {
        forward = false;

        if (command.startsWith("/p ") || command.startsWith("/phrase "))
        {
            string filePath = "phrase/";

            if (command.startsWith("/p "))
            {
                filePath.append(command.mid(3).toStdString());
            }
            else
            {
                filePath.append(command.mid(8).toStdString());
            }

            if (!QString::fromStdString(filePath).endsWith(".txt"))
            {
                filePath.append(".txt");
            }

            char help[100];
            unsigned int i;
            for (i = 0; i < 100; i++)
                help[i] = NULL;
            for (i = 0; i < filePath.size(); i++)
                help[i] = filePath[i];
            fstream infile(help);

            if (infile) // File exists
            {
                char messageText[224]; // Maximum chat text is 224 bytes.
                do //while( !infile.eof() )
                {
                    infile.getline(messageText, 224);
                    string tmp = parseTextline(messageText);

                    if (gproxy->m_BNET->GetInGame() && (messageText[0] != "#"[0]))
                    {
                        if ((messageText[0] != "%"[0]))
                        {
                            if (messageText[0] != "~"[0])
                            {
                                gproxy->sendGamemessage(QString::fromStdString(tmp));
                            }
                            else
                            {
                                // Sleep(50);
                            }
                        }
                        else
                        {
                            gproxy->m_BNET->QueueChatCommand(QString::fromStdString(tmp.substr(1)));
                        }
                    }
                    else if (messageText[0] != "#"[0])
                        gproxy->m_BNET->QueueChatCommand(QString::fromStdString(tmp));
                }
                while (!infile.eof());

                infile.close();
            }
            else
            {
                CONSOLE_Print(ColoredMessage("[ERROR] File \"" + QString::fromStdString(help) + "\" does not exist!", ColoredMessage::ERROR));
                SendLocalChat("File \"" + QString::fromStdString(help) + "\" does not exist!");
            }
        }
        else if (command.length() >= 5 && command.startsWith("/re "))
        {
            if (m_BNET->GetLoggedIn())
            {
                if (!m_BNET->GetReplyTarget().empty())
                {
                    m_BNET->QueueChatCommand(message.mid(4), m_BNET->GetReplyTarget(), true);
                    SendLocalChat("Whispered to " + QString::fromStdString(m_BNET->GetReplyTarget()) + ": " + message.mid(4));
                }
                else
                {
                    SendLocalChat("Nobody has whispered you yet.");
                }
            }
            else
            {
                SendLocalChat("You are not connected to battle.net.");
            }
        }
        else if (command == "/sc" || command == "/spoof"
                || command == "/spoofcheck" || command == "/spoof check")
        {
            if (m_BNET->GetLoggedIn())
            {
                if (!m_GameStarted)
                {
                    m_BNET->QueueChatCommand("spoofcheck", m_HostName, true);
                    SendLocalChat("Whispered to " + QString::fromStdString(m_HostName) + ": spoofcheck");
                }
                else
                {
                    SendLocalChat("The game has already started.");
                }
            }
            else
            {
                SendLocalChat("You are not connected to battle.net.");
            }
        }
        else if (command.startsWith("/wh "))
        {
            QString msg = command.mid(4, command.length() - 4);
            m_BNET->QueueChatCommand(msg, m_HostName, true);
            SendLocalChat("Whispered to host [" + QString::fromStdString(m_HostName) + "] " + msg);
        }
        else if (command.startsWith("/whs "))
        {
            QString msg = command.mid(5, command.length() - 5);
            m_BNET->QueueChatCommand("!say " + msg, m_HostName, true);
            SendLocalChat("Whispered to host [" + QString::fromStdString(m_HostName) + "] !say " + msg);
        }
        else if (command.startsWith("/cmd "))
        {
            m_BNET->QueueChatCommand(command.mid(5, command.length() - 5));
        }
        else if (command == "/host")
        {
            SendLocalChat("Hosting player/bot is [" + QString::fromStdString(m_HostName) + "]. (use '/wh <message>' to whisper to him)");
        }
        else if (command == "/status")
        {
            if (m_LocalSocket)
            {
                if (m_GameIsReliable && m_ReconnectPort > 0)
                {
                    SendLocalChat("GProxy++ disconnect protection: Enabled");
                }
                else
                {
                    SendLocalChat("GProxy++ disconnect protection: Disabled");
                }

                if (m_BNET->GetLoggedIn())
                {
                    SendLocalChat("battle.net: Connected");
                }
                else
                {
                    SendLocalChat("battle.net: Disconnected");
                }

                if (mainGUI->getStatspage()->isLoggedIn())
                {
                    SendLocalChat("GhostGraz statspage: Available");
                }
                else
                {
                    SendLocalChat("GhostGraz statspage: Not available");
                }
            }
        }
        else if (command.length() >= 4 && command.startsWith("/w "))
        {
            if (m_BNET->GetLoggedIn())
            {
                int messageBeginIndex = command.indexOf(" ", 4);

                if (messageBeginIndex != -1 && messageBeginIndex != 0)
                {
                    m_BNET->QueueChatCommand(message);
                    SendLocalChat("Whisper to " + message.mid(3, messageBeginIndex - 3)
                            + ": " + message.mid(messageBeginIndex));
                }
            }
            else
            {
                SendLocalChat("You are not connected to battle.net.");
            }
        }
        else if (command == "/s" || command.startsWith("/s "))
        {
            if (command == "/s")
            {
                gproxy->sendGamemessage("!stats");
            }
            else
            {
                gproxy->sendGamemessage("!stats "+message.mid(3));
            }
        }
        else if (command == "/sd" || command.startsWith("/sd "))
        {
            if (command == "/sd")
            {
                gproxy->sendGamemessage("!statsdota");
            }
            else
            {
                gproxy->sendGamemessage("!statsdota "+message.mid(4));
            }
        }
        else if (command == "/statslast" || command == "/sl")
        {
            gproxy->sendGamemessage("!stats " + lastLeaver->getName().getMessage());
        }
        else
        {
            gproxy->m_BNET->QueueChatCommand(message, false);
        }
    } // if (command.length() >= 1 && command.startsWith("/"))
    else if (command.length() >= 1 && command.startsWith("!"))
    {
        if (command.startsWith("!stats") || command.startsWith("!statsdota"))
        {
            return true;
        }
        else if (command == "!s" || command.startsWith("!s "))
        {
            if (command == "!s")
            {
                gproxy->sendGamemessage("!stats");
            }
            else
            {
                gproxy->sendGamemessage("!stats "+message.mid(3));
            }
        }
        else if (command == "!sd" || command.startsWith("!sd "))
        {
            if (command == "!sd")
            {
                gproxy->sendGamemessage("!statsdota");
            }
            else if (command.mid(3, 1) == " ")
            {
                gproxy->sendGamemessage("!statsdota "+message.mid(4));
            }
        }
        else if (command == "!statslast" || command == "!sl")
        {
            gproxy->sendGamemessage("!stats " + lastLeaver->getName().getMessage());
        }
    } // if (command.length() >= 1 && command.startsWith("!"))

    return forward;
}

void CGProxy::ProcessLocalPackets ()
{
    if (!m_LocalSocket)
        return;

    while (!m_LocalPackets.empty())
    {
        CCommandPacket *Packet = m_LocalPackets.front();
        m_LocalPackets.pop();
        BYTEARRAY Data = Packet->GetData();

        if (Packet->GetPacketType() == W3GS_HEADER_CONSTANT)
        {
            if (Packet->GetID() == CGameProtocol::W3GS_REQJOIN)
            {
                if (Data.size() >= 20)
                {
                    uint32_t EntryKey = Util::byteArrayToUInt32(Data, false, 8);
                    unsigned char Unknown = Data[12];
                    uint16_t ListenPort = Util::byteArrayToUInt16(Data, false, 13);
                    uint32_t PeerKey = Util::byteArrayToUInt32(Data, false, 15);
                    BYTEARRAY Name = Util::extractNTCString(Data, 19);
                    string NameString = string(Name.begin(), Name.end());
                    BYTEARRAY Remainder = BYTEARRAY(Data.begin() + Name.size() + 20, Data.end());

                    if(( Remainder.size( ) == 18 && config->getInt("war3version") <= 28 )||( Remainder.size( ) == 19 && config->getInt("war3version") >= 29 ))
                    {
                        // lookup the game in the main list

                        bool GameFound = false;

                        for (vector<CIncomingGameHost*> ::iterator i = m_Games.begin(); i != m_Games.end(); i++)
                        {
                            if (((CIncomingGameHost*) (*i))->GetUniqueGameID() == EntryKey)
                            {
                                CONSOLE_Print(ColoredMessage("[GPROXY] local player requested game name [" + QString::fromStdString(((CIncomingGameHost*) (*i))->GetGameName()) + "]", ColoredMessage::GPROXY));

                                if (NameString != username.toStdString())
                                    CONSOLE_Print(ColoredMessage("[GPROXY] using battle.net name [" + username + "] instead of requested name [" + QString::fromStdString(NameString) + "]", ColoredMessage::GPROXY));

                                CONSOLE_Print(ColoredMessage("[GPROXY] connecting to remote server [" + QString::fromStdString(((CIncomingGameHost*) (*i))->GetIPString()) + "] on port " + QString::number(((CIncomingGameHost*) (*i))->GetPort()), ColoredMessage::GPROXY));
                                m_RemoteServerIP = ((CIncomingGameHost*) (*i))->GetIPString();
                                m_RemoteServerPort = ((CIncomingGameHost*) (*i))->GetPort();
                                m_RemoteSocket->Reset();
                                m_RemoteSocket->SetNoDelay(true);
                                m_RemoteSocket->Connect(string(), m_RemoteServerIP, m_RemoteServerPort);
                                m_LastConnectionAttemptTime = getElapsedSeconds();
                                m_GameIsReliable = (((CIncomingGameHost*) (*i))->GetMapWidth() == 1984 && ((CIncomingGameHost*) (*i))->GetMapHeight() == 1984);
                                m_GameStarted = false;

                                // rewrite packet

                                BYTEARRAY DataRewritten;
                                DataRewritten.push_back(W3GS_HEADER_CONSTANT);
                                DataRewritten.push_back(Packet->GetID());
                                DataRewritten.push_back(0);
                                DataRewritten.push_back(0);
                                Util::appendByteArray(DataRewritten, (*i)->GetHostCounter(), false);
                                Util::appendByteArray(DataRewritten, (uint32_t) 0, false);
                                DataRewritten.push_back(Unknown);
                                Util::appendByteArray(DataRewritten, ListenPort, false);
                                Util::appendByteArray(DataRewritten, PeerKey, false);
                                Util::appendByteArray(DataRewritten, username.toStdString());
                                Util::appendByteArrayFast(DataRewritten, Remainder);
                                BYTEARRAY LengthBytes;
                                LengthBytes = Util::createByteArray((uint16_t) DataRewritten.size(), false);
                                DataRewritten[2] = LengthBytes[0];
                                DataRewritten[3] = LengthBytes[1];
                                Data = DataRewritten;

                                // tell battle.net we're joining a game (for automatic spoof checking)
                                m_BNET->QueueJoinGame(((CIncomingGameHost*) (*i))->GetGameName());

                                // save the hostname for later (for manual spoof checking)
                                m_JoinedName = NameString;
                                m_HostName = ((CIncomingGameHost*) (*i))->GetHostName();
                                GameFound = true;

                                // Manufactorings work
                                m_GameName = ((CIncomingGameHost*) (*i))->GetGameName();
                                gproxy->changeChannel(QString::fromStdString(m_GameName));

                                break;
                            }
                        }

                        if (!GameFound)
                        {
                            CONSOLE_Print(ColoredMessage("[GPROXY] local player requested unknown game (expired?)", ColoredMessage::GPROXY));
                            m_LocalSocket->Disconnect();
                        }
                    }
                    else
                        CONSOLE_Print(ColoredMessage("[GPROXY] received invalid join request from local player (invalid remainder)", ColoredMessage::GPROXY));
                }
                else
                    CONSOLE_Print(ColoredMessage("[GPROXY] received invalid join request from local player (too short)", ColoredMessage::GPROXY));
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_LEAVEGAME)
            {
                m_LeaveGameSent = true;
                m_LocalSocket->Disconnect();
                gproxy->m_BNET->QueueChatCommand("/whois " + QString::fromStdString(gproxy->m_HostName));
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_CHAT_TO_HOST)
            {
                // handled in ExtractLocalPackets (yes, it's ugly)
            }
        }

        // warning: do not forward any data if we are not synchronized (e.g. we are reconnecting and resynchronizing)
        // any data not forwarded here will be cached in the packet buffer and sent later so all is well

        if (m_RemoteSocket && m_Synchronized)
            m_RemoteSocket->PutBytes(Data);

        delete Packet;
    }
}

void CGProxy::ExtractRemotePackets ()
{
    string *RecvBuffer = m_RemoteSocket->GetBytes();
    BYTEARRAY Bytes = Util::createByteArray((unsigned char *) RecvBuffer->c_str(), RecvBuffer->size());

    // a packet is at least 4 bytes so loop as long as the buffer contains 4 bytes

    while (Bytes.size() >= 4)
    {
        if (Bytes[0] == W3GS_HEADER_CONSTANT || Bytes[0] == GPS_HEADER_CONSTANT)
        {
            // bytes 2 and 3 contain the length of the packet

            uint16_t Length = Util::byteArrayToUInt16(Bytes, false, 2);

            if (Length >= 4)
            {
                if (Bytes.size() >= Length)
                {
                    m_RemotePackets.push(new CCommandPacket(Bytes[0], Bytes[1], BYTEARRAY(Bytes.begin(), Bytes.begin() + Length)));

                    if (Bytes[0] == W3GS_HEADER_CONSTANT)
                        m_TotalPacketsReceivedFromRemote++;

                    *RecvBuffer = RecvBuffer->substr(Length);
                    Bytes = BYTEARRAY(Bytes.begin() + Length, Bytes.end());
                }
                else
                    return;
            }
            else
            {
                CONSOLE_Print(ColoredMessage("[GPROXY] received invalid packet from remote server (bad length)", ColoredMessage::GPROXY));
                m_Exiting = true;
                return;
            }
        }
        else
        {
            CONSOLE_Print(ColoredMessage("[GPROXY] received invalid packet from remote server (bad header constant)", ColoredMessage::GPROXY));
            m_Exiting = true;
            return;
        }
    }
}

void CGProxy::ProcessRemotePackets ()
{
    if (!m_LocalSocket || !m_RemoteSocket)
        return;

    while (!m_RemotePackets.empty())
    {
        CCommandPacket *Packet = m_RemotePackets.front();
        m_RemotePackets.pop();

        if (Packet->GetPacketType() == W3GS_HEADER_CONSTANT)
        {
            if (Packet->GetID() == CGameProtocol::W3GS_CHAT_FROM_HOST)
            {
                BYTEARRAY Data = Packet->GetData();
                unsigned int i = 5;

                unsigned char toPID_Count = Data[4];
                BYTEARRAY toPIDs = BYTEARRAY(Data.begin() + i, Data.begin() + i + toPID_Count);

                i += toPID_Count;

                int FromPID = (int) Data[i];
                unsigned char Flag = Data[i + 1];
                i += 2;
                BYTEARRAY ExtraFlags;
                QString message;

                if (Flag == 16)
                {
                    // Chat message
                    message = Util::extractNTString(Data, i);
                }
                else if (Flag >= 17 && Flag <= 20)
                {
                    // team/colour/race/handicap change request
                }
                else if (Flag == 32)
                {
                    // Extra flags
                    ExtraFlags = BYTEARRAY(Data.begin() + i, Data.begin() + i + 4);
                    message = Util::extractNTString(Data, i + 4);
                }

                if (Flag == 16 || Flag == 32) // Chat message
                {
                    ColoredMessage coloredPlayer;
                    foreach(Player* player, players)
                    {
                        if(player->getPlayerId() == FromPID)
                        {
                            coloredPlayer = player->getName();
                            break;
                        }
                    }

                    // If the message is a autogenerated ban message
                    if (message.contains("Player [")
                            && message.contains("] was banned by player [")
                            && message.contains("] on server ["))
                    {
                        QSound::play("sounds/player_banned.wav");
                    }

                    // If the message is a autogenerated "same ip" message
                    if ((message.contains("Player [")
                            && message.contains("] has the same IP address as: "))
                            || message.contains("same IPs:"))
                    {
                        QSound::play("sounds/same_ip.wav");
                    }

                    if (Flag == 16)
                    {
                        CONSOLE_Print(ColoredMessage("[LOBBY]["), true, true, false);
                        CONSOLE_Print(coloredPlayer, true, false, false);
                        CONSOLE_Print(ColoredMessage("] " + message), true, false, true);
                    }
                    else // GameStarted
                    {
                        // ExtraFlags[0] stores the type of chat message as defined by Warcraft III.
                        if (ExtraFlags[0] == 0) // 0 is an All message
                        {
                            CONSOLE_Print(ColoredMessage("[ALL]["), true, true, false);
                            CONSOLE_Print(coloredPlayer, true, false, false);
                            CONSOLE_Print(ColoredMessage("] " + message), true, false, true);
                        }
                        else if (ExtraFlags[0] == 1) // 1 is an Allies message
                        {
                            CONSOLE_Print(ColoredMessage("[ALLY]["), true, true, false);
                            CONSOLE_Print(coloredPlayer, true, false, false);
                            CONSOLE_Print(ColoredMessage("] " + message), true, false, true);
                        }
                        else if (ExtraFlags[0] == 2) // 2 is an Observer/Referee message
                        {
                            CONSOLE_Print(ColoredMessage("[OBSERVER]["), true, true, false);
                            CONSOLE_Print(coloredPlayer, true, false, false);
                            CONSOLE_Print(ColoredMessage("] " + message), true, false, true);
                        }
                        else if (ExtraFlags[0] >= 3) // 3+ are private messages
                        {
                            // Based on limited testing it seems that
                            // the extra flags' first byte contains 3 plus the recipient's colour to denote a private message.
                            // Due to the fact that the Datapacket also contains the FromPID, we do not need to extract the color.

                            CONSOLE_Print(ColoredMessage("[PRIVATE][FROM]["), true, true, false);
                            CONSOLE_Print(coloredPlayer, true, false, false);
                            CONSOLE_Print(ColoredMessage("] " + message), true, false, true);
                        }
                    }
                }
            }
            if (Packet->GetID() == CGameProtocol::W3GS_PLAYERLEAVE_OTHERS)
            {
                BYTEARRAY Data = Packet->GetData();

                int playerId = ((int) Data[4]);
                ColoredMessage playername;

                for (int i = 0; i < players.count(); i++)
                {
                    if(players.at(i)->getPlayerId() == playerId)
                    {
                        playername = players.at(i)->getName();
                        delete lastLeaver;
                        lastLeaver = players.at(i);
                        players.removeAt(i);
                        break;
                    }
                }

                if (gproxy->m_GameStarted)
                {
                    if (gproxy->m_PlaySound)
                    {
                        QSound::play("sounds/leaver_detected.wav");
                    }
                }

                CONSOLE_Print(ColoredMessage("[GAMEINFO] ", ColoredMessage::GAMEINFO), true, true, false);
                CONSOLE_Print(ColoredMessage(playername), true, false, false);
                CONSOLE_Print(ColoredMessage(" has left the game.", ColoredMessage::GAMEINFO), true, false, true);
            }
            if (Packet->GetID() == CGameProtocol::W3GS_SLOTINFOJOIN)
            {
                for(int i = 0; i < players.count(); i++)
                {
                    delete players[i];
                }
                players.clear();

                for (QList<Slot*>::iterator it = slotList.begin(); it != slotList.end(); ++it)
                {
                    delete *it;
                }
                slotList.clear();

                BYTEARRAY Data = Packet->GetData();

                if (Data.size() >= 6)
                {
                    uint16_t SlotInfoSize = Util::byteArrayToUInt16(Data, false, 4);

                    if (Data.size() >= 7 + (unsigned) SlotInfoSize)
                    {
                        m_ChatPID = Data[6 + SlotInfoSize];
                    }

                    // send a GPS_INIT packet
                    // if the server doesn't recognize it (e.g. it isn't GHost++) we should be kicked
                    CONSOLE_Print(ColoredMessage("[GPROXY] join request accepted by remote server", ColoredMessage::GPROXY));
                    if (m_GameIsReliable)
                    {
                        CONSOLE_Print(ColoredMessage("[GPROXY] detected reliable game, starting GPS handshake", ColoredMessage::GPROXY));
                        m_RemoteSocket->PutBytes(m_GPSProtocol->SEND_GPSC_INIT(1));
                    }
                    else
                        CONSOLE_Print(ColoredMessage("[GPROXY] detected standard game, disconnect protection disabled", ColoredMessage::GPROXY));

                    unsigned char numberOfSlots = Data[6];

                    int i = 7;
                    for (int j = 0; j < numberOfSlots; j++)
                    {
                        Slot* slot = new Slot();
                        slot->setPlayerId(Data[i++]);
                        slot->setDownloadProgress(Data[i++]);
                        slot->setSlotStatus(Slot::SlotStatus(Data[i++]));
                        slot->setComputerStatus(Slot::ComputerStatus(Data[i++]));
                        slot->setTeam(Data[i++]);
                        slot->setColor(Slot::SlotColor(Data[i++]));
                        slot->setRace(Slot::Race(Data[i++]));
                        slot->setComputerType(Slot::ComputerType(Data[i++]));
                        slot->setHandicap(Data[i++]);

                        if (slot->getPlayerId() == m_ChatPID)
                        {
                            teamNumber = slot->getTeam();
                        }

                        slotList.append(slot);
                    }

                    Player* player = new Player();
                    player->setPlayerId(m_ChatPID);
                    player->setName(ColoredMessage(username, ColoredMessage::USERCOLOR));
                    players.append(player);

                    emit signal_playerJoined(ColoredMessage(username, ColoredMessage::USERCOLOR));
                }
                else
                {
                    CONSOLE_Print(ColoredMessage("[SLOTINFOJOIN] Small data recieved!", ColoredMessage::ERROR));
                }
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_SLOTINFO)
            {
                BYTEARRAY Data = Packet->GetData();

                uint16_t SlotInfoSize = Util::byteArrayToUInt16(Data, false, 4);

                if (Data.size() >= 7 + (unsigned) SlotInfoSize)
                {
                    m_ChatPID = Data[6 + SlotInfoSize];
                }

                unsigned char numberOfSlots = Data[6];

                int i = 7;
                for (int j = 0; j < numberOfSlots; j++)
                {
                    foreach(Slot* slot, slotList)
                    {
                        if (slot->getColor() == Data[i + 5])
                        {
                            slot->setPlayerId(Data[i++]);
                            slot->setDownloadProgress(Data[i++]);
                            slot->setSlotStatus(Slot::SlotStatus(Data[i++]));
                            slot->setComputerStatus(Slot::ComputerStatus(Data[i++]));
                            slot->setTeam(Data[i++]);
                            slot->setColor(Slot::SlotColor(Data[i++]));
                            slot->setRace(Slot::Race(Data[i++]));
                            slot->setComputerType(Slot::ComputerType(Data[i++]));
                            slot->setHandicap(Data[i++]);

                            bool playerFound = false;
                            foreach(Player* player, players)
                            {
                                if(player->getPlayerId() == slot->getPlayerId())
                                {
                                    slot->setPlayer(player);
                                    playerFound = true;
                                    break;
                                }
                            }
                            if (!playerFound)
                            {
                                slot->setPlayer(NULL);
                            }

                            if (slot->getPlayerId() == m_ChatPID)
                            {
                                teamNumber = slot->getTeam();
                            }

                            break;
                        }
                    }
                }

                emit signal_setGameslots(slotList);
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_PLAYERINFO)
            {
                BYTEARRAY data = Packet->GetData();

                int playerId = (int) data[8];
                QString playername = Util::extractNTString(data, 9);
                ColoredMessage name(playername, ColoredMessage::USERCOLOR);
                if(playername.startsWith("|c", Qt::CaseInsensitive))
                {
                    name.setMessage(name.getMessage().mid(10));
                    if (playername.endsWith("|r", Qt::CaseInsensitive))
                    {
                        name.setMessage(name.getMessage().left(name.getMessage().length() - 2));
                    }

                    name.setColorType(ColoredMessage::CUSTOM);
                    name.setColor(Util::toColor(playername.left(10)));
                }

                Player* player = new Player();
                player->setPlayerId(playerId);
                player->setName(name);
                players.append(player);

                emit signal_playerJoined(name);
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_MAPCHECK)
            {
                BYTEARRAY data = Packet->GetData();

                BYTEARRAY path = Util::extractNTCString(data, 8);
                string filePath = string(path.begin(), path.end());

                transform(filePath.begin(), filePath.end(), filePath.begin(), (int(*)(int))tolower);
                if (filePath.find("dota") != string::npos)
                    setDotaMap(true);
                else
                    setDotaMap(false);
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_GAMELOADED_OTHERS)
            {
                uint32_t loadingTime = getElapsedMilliseconds() - countdownEndTime;
                uint32_t sec = loadingTime / 1000;
                uint32_t dsec = (loadingTime / 100) % 10;
                ostringstream finishedLoading;

                if (((int) Packet->GetData()[4]) == m_ChatPID)
                {
                    CONSOLE_Print(ColoredMessage("[GAMEINFO] You have finished loading. "
                            "Loading time: " + QString::number(sec) + "," + QString::number(dsec) + " seconds.", ColoredMessage::GAMEINFO));
                }
                else
                {
                    ColoredMessage playername;
                    foreach(Player *player, players)
                    {
                        if(player->getPlayerId() == ((int) Packet->GetData()[4]))
                        {
                            playername = player->getName();
                            break;
                        }
                    }

                    CONSOLE_Print(ColoredMessage("[GAMEINFO] ", ColoredMessage::GAMEINFO), true, true, false);
                    CONSOLE_Print(playername, true, false, false);
                    CONSOLE_Print(ColoredMessage(" has finished loading. Loading time: "
                            + QString::number(sec) + "," + QString::number(dsec) + " seconds.",
                            ColoredMessage::GAMEINFO), true, false, true);
                }

                playerLoadingComplete[ ((int) Packet->GetData()[4]) ] = true;
                finishedLoadingCounter++;

                int playerCount = 0;
                foreach(Slot* slot, slotList)
                {
                    if (slot->getSlotStatus() == Slot::OCCUPIED
                            && slot->getComputerStatus() == Slot::NO_COMPUTER)
                    {
                        playerCount++;
                    }
                }

                if (playerCount == finishedLoadingCounter)
                {
                    for (int i = 0; i < 12; i++)
                    {
                        playerLoadingComplete[i] = false;
                    }

                    finishedLoadingCounter = 0;

                    if (gproxy->m_PlaySound)
                    {
                        QSound::play("sounds/game_started.wav");
                    }
                }
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_COUNTDOWN_END)
            {
                if (m_GameIsReliable && m_ReconnectPort > 0)
                    CONSOLE_Print(ColoredMessage("[GPROXY] game started, disconnect protection enabled", ColoredMessage::GPROXY));
                else
                {
                    if (m_GameIsReliable)
                        CONSOLE_Print(ColoredMessage("[GPROXY] game started but GPS handshake not complete, disconnect protection disabled", ColoredMessage::GPROXY));
                    else
                        CONSOLE_Print(ColoredMessage("[GPROXY] game started", ColoredMessage::GPROXY));
                }

                m_GameStarted = true;
                countdownEndTime = getElapsedMilliseconds();

                emit signal_setGameslots(slotList);
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_REJECTJOIN)
            {
                BYTEARRAY Data = Packet->GetData();

                if (Data.size() < 5)
                    return;

                uint32_t reason = Util::byteArrayToUInt32(Data, false, 4);

                switch (reason)
                {
                    case 0x07:
                        CONSOLE_Print(ColoredMessage("[ERROR] there was an error in the request", ColoredMessage::ERROR));
                        break;
                    case 0x09:
                        CONSOLE_Print(ColoredMessage("[ERROR] gamelobby is full", ColoredMessage::ERROR));
                        break;
                    case 0x10:
                        CONSOLE_Print(ColoredMessage("[ERROR] game has been started", ColoredMessage::ERROR));
                        break;
                    case 0x27:
                        CONSOLE_Print(ColoredMessage("[ERROR] wrong password", ColoredMessage::ERROR));
                        break;
                    default:
                        CONSOLE_Print(ColoredMessage("[ERROR] Join rejected with invalid error code: " + QString::number(reason), ColoredMessage::ERROR));
                }
                return;
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_INCOMING_ACTION)
            {
                if (m_GameIsReliable)
                {
                    // we received a game update which means we can reset the number of empty actions we have to work with
                    // we also must send any remaining empty actions now
                    // note: the lag screen can't be up right now otherwise the server made a big mistake, so we don't need to check for it

                    BYTEARRAY EmptyAction;
                    EmptyAction.push_back(0xF7);
                    EmptyAction.push_back(0x0C);
                    EmptyAction.push_back(0x06);
                    EmptyAction.push_back(0x00);
                    EmptyAction.push_back(0x00);
                    EmptyAction.push_back(0x00);

                    for (unsigned char i = m_NumEmptyActionsUsed; i < m_NumEmptyActions; i++)
                        m_LocalSocket->PutBytes(EmptyAction);

                    m_NumEmptyActionsUsed = 0;
                }

                m_ActionReceived = true;
                m_LastActionTime = getElapsedSeconds();
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_START_LAG)
            {
                if (m_GameIsReliable)
                {
                    BYTEARRAY Data = Packet->GetData();

                    if (Data.size() >= 5)
                    {
                        unsigned char NumLaggers = Data[4];

                        if (Data.size() == 5 + ((unsigned int) NumLaggers) * 5)
                        {
                            for (unsigned char i = 0; i < NumLaggers; i++)
                            {
                                bool LaggerFound = false;

                                for (vector<unsigned char> ::iterator j = m_Laggers.begin(); j != m_Laggers.end(); j++)
                                {
                                    if (*j == Data[5 + i * 5])
                                        LaggerFound = true;
                                }

                                if (LaggerFound)
                                    CONSOLE_Print(ColoredMessage("[WARNING] Received start_lag on known lagger", ColoredMessage::WARNING));
                                else
                                    m_Laggers.push_back(Data[5 + i * 5]);
                            }
                        }
                        else
                            CONSOLE_Print(ColoredMessage("[WARNING] Unhandled start_lag (2)", ColoredMessage::WARNING));
                    }
                    else
                        CONSOLE_Print(ColoredMessage("[WARNING] Unhandled start_lag (1)", ColoredMessage::WARNING));
                }
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_STOP_LAG)
            {
                if (m_GameIsReliable)
                {
                    BYTEARRAY Data = Packet->GetData();

                    if (Data.size() == 9)
                    {
                        bool LaggerFound = false;

                        for (vector<unsigned char> ::iterator i = m_Laggers.begin(); i != m_Laggers.end();)
                        {
                            if (*i == Data[4])
                            {
                                i = m_Laggers.erase(i);
                                LaggerFound = true;
                            }
                            else
                                i++;
                        }

                        if (!LaggerFound)
                            CONSOLE_Print(ColoredMessage("[WARNING] Received stop_lag on unknown lagger", ColoredMessage::WARNING));
                    }
                    else
                        CONSOLE_Print(ColoredMessage("[GPROXY] Unhandled stop_lag", ColoredMessage::WARNING));
                }
            }
            else if (Packet->GetID() == CGameProtocol::W3GS_INCOMING_ACTION2)
            {
                if (m_GameIsReliable)
                {
                    // we received a fractured game update which means we cannot use any empty actions until we receive the subsequent game update
                    // we also must send any remaining empty actions now
                    // note: this means if we get disconnected right now we can't use any of our buffer time, which would be very unlucky
                    // it still gives us 60 seconds total to reconnect though
                    // note: the lag screen can't be up right now otherwise the server made a big mistake, so we don't need to check for it

                    BYTEARRAY EmptyAction;
                    EmptyAction.push_back(0xF7);
                    EmptyAction.push_back(0x0C);
                    EmptyAction.push_back(0x06);
                    EmptyAction.push_back(0x00);
                    EmptyAction.push_back(0x00);
                    EmptyAction.push_back(0x00);

                    for (unsigned char i = m_NumEmptyActionsUsed; i < m_NumEmptyActions; i++)
                        m_LocalSocket->PutBytes(EmptyAction);

                    m_NumEmptyActionsUsed = m_NumEmptyActions;
                }
            }


            // forward the data

            m_LocalSocket->PutBytes(Packet->GetData());

            // we have to wait until now to send the status message since otherwise the slotinfojoin itself wouldn't have been forwarded

            if (Packet->GetID() == CGameProtocol::W3GS_SLOTINFOJOIN)
            {
                if (m_JoinedName != username.toStdString())
                    SendLocalChat("Using battle.net name \"" + username + "\" instead of LAN name \"" + QString::fromStdString(m_JoinedName) + "\".");

                if (m_GameIsReliable)
                    SendLocalChat("This is a reliable game. Requesting GProxy++ disconnect protection from server...");
                else
                    SendLocalChat("This is an unreliable game. GProxy++ disconnect protection is disabled.");
            }
        }
        else if (Packet->GetPacketType() == GPS_HEADER_CONSTANT)
        {
            if (m_GameIsReliable)
            {
                BYTEARRAY Data = Packet->GetData();

                if (Packet->GetID() == CGPSProtocol::GPS_INIT && Data.size() == 12)
                {
                    m_ReconnectPort = Util::byteArrayToUInt16(Data, false, 4);
                    m_PID = Data[6];
                    m_ReconnectKey = Util::byteArrayToUInt32(Data, false, 7);
                    m_NumEmptyActions = Data[11];
                    SendLocalChat("GProxy++ disconnect protection is ready (" + QString::number((m_NumEmptyActions + 1) * 60) + " second buffer).");
                    CONSOLE_Print(ColoredMessage("[GPROXY] handshake complete, disconnect protection ready (" + QString::number((m_NumEmptyActions + 1) * 60) + " second buffer)", ColoredMessage::GPROXY));
                }
                else if (Packet->GetID() == CGPSProtocol::GPS_RECONNECT && Data.size() == 8)
                {
                    uint32_t LastPacket = Util::byteArrayToUInt32(Data, false, 4);
                    uint32_t PacketsAlreadyUnqueued = m_TotalPacketsReceivedFromLocal - m_PacketBuffer.size();

                    if (LastPacket > PacketsAlreadyUnqueued)
                    {
                        uint32_t PacketsToUnqueue = LastPacket - PacketsAlreadyUnqueued;

                        if (PacketsToUnqueue > m_PacketBuffer.size())
                        {
                            CONSOLE_Print(ColoredMessage("[GPROXY] received GPS_RECONNECT with last packet > total packets sent", ColoredMessage::GPROXY));
                            PacketsToUnqueue = m_PacketBuffer.size();
                        }

                        while (PacketsToUnqueue > 0)
                        {
                            delete m_PacketBuffer.front();
                            m_PacketBuffer.pop();
                            PacketsToUnqueue--;
                        }
                    }

                    // send remaining packets from buffer, preserve buffer
                    // note: any packets in m_LocalPackets are still sitting at the end of this buffer because they haven't been processed yet
                    // therefore we must check for duplicates otherwise we might (will) cause a desync

                    queue<CCommandPacket *> TempBuffer;

                    while (!m_PacketBuffer.empty())
                    {
                        if (m_PacketBuffer.size() > m_LocalPackets.size())
                            m_RemoteSocket->PutBytes(((CCommandPacket *) m_PacketBuffer.front())->GetData());

                        TempBuffer.push(m_PacketBuffer.front());
                        m_PacketBuffer.pop();
                    }

                    m_PacketBuffer = TempBuffer;

                    // we can resume forwarding local packets again
                    // doing so prior to this point could result in an out-of-order stream which would probably cause a desync

                    m_Synchronized = true;
                }
                else if (Packet->GetID() == CGPSProtocol::GPS_ACK && Data.size() == 8)
                {
                    uint32_t LastPacket = Util::byteArrayToUInt32(Data, false, 4);
                    uint32_t PacketsAlreadyUnqueued = m_TotalPacketsReceivedFromLocal - m_PacketBuffer.size();

                    if (LastPacket > PacketsAlreadyUnqueued)
                    {
                        uint32_t PacketsToUnqueue = LastPacket - PacketsAlreadyUnqueued;

                        if (PacketsToUnqueue > m_PacketBuffer.size())
                        {
                            CONSOLE_Print(ColoredMessage("[GPROXY] received GPS_ACK with last packet > total packets sent", ColoredMessage::GPROXY));
                            PacketsToUnqueue = m_PacketBuffer.size();
                        }

                        while (PacketsToUnqueue > 0)
                        {
                            delete m_PacketBuffer.front();
                            m_PacketBuffer.pop();
                            PacketsToUnqueue--;
                        }
                    }
                }
                else if (Packet->GetID() == CGPSProtocol::GPS_REJECT && Data.size() == 8)
                {
                    uint32_t Reason = Util::byteArrayToUInt32(Data, false, 4);

                    if (Reason == REJECTGPS_INVALID)
                        CONSOLE_Print(ColoredMessage("[GPROXY] rejected by remote server: invalid data", ColoredMessage::GPROXY));
                    else if (Reason == REJECTGPS_NOTFOUND)
                        CONSOLE_Print(ColoredMessage("[GPROXY] rejected by remote server: player not found in any running games", ColoredMessage::GPROXY));

                    m_LocalSocket->Disconnect();
                }

            }
        }

        delete Packet;
    }
}

bool CGProxy::AddGame (CIncomingGameHost* game)
{
    // check for duplicates and rehosted games

    bool DuplicateFound = false;
    uint32_t OldestReceivedTime = getElapsedSeconds();

    for (vector<CIncomingGameHost*> ::iterator i = m_Games.begin(); i != m_Games.end(); i++)
    {
        if (game->GetIP() == ((CIncomingGameHost*) (*i))->GetIP() && game->GetPort() == ((CIncomingGameHost*) (*i))->GetPort())
        {
            // duplicate or rehosted game, delete the old one and add the new one
            // don't forget to remove the old one from the LAN list first

            m_UDPSocket->Broadcast(6112, m_GameProtocol->SEND_W3GS_DECREATEGAME(((CIncomingGameHost*) (*i))->GetUniqueGameID()));
            delete *i;
            *i = game;
            DuplicateFound = true;
            break;
        }

        if (game->GetGameName() != m_BNET->GetSearchGameName() && game->GetReceivedTime() < OldestReceivedTime)
            OldestReceivedTime = game->GetReceivedTime();
    }

    if (!DuplicateFound)
    {
        m_Games.push_back(game);
        gproxy->autosearch = true; //pr0 autosearch
    }

    // the game list cannot hold more than 20 games (warcraft 3 doesn't handle it properly and ignores any further games)
    // if this game puts us over the limit, remove the oldest game
    // don't remove the "search game" since that's probably a pretty important game
    // note: it'll get removed automatically by the 60 second timeout in the main loop when appropriate

    if (m_Games.size() > 20)
    {
        for (vector<CIncomingGameHost*> ::iterator i = m_Games.begin(); i != m_Games.end(); i++)
        {
            if (game->GetGameName() != m_BNET->GetSearchGameName() && game->GetReceivedTime() == OldestReceivedTime)
            {
                m_UDPSocket->Broadcast(6112, m_GameProtocol->SEND_W3GS_DECREATEGAME(((CIncomingGameHost*) (*i))->GetUniqueGameID()));
                delete *i;
                m_Games.erase(i);
                break;
            }
        }
    }

    return !DuplicateFound;
}

void CGProxy::sendGamemessage (QString message, bool alliesOnly)
{
    if (message.isEmpty() || !gproxy->m_BNET->GetInGame())
    {
        return;
    }

    // Warcraft can only display a total of 127 characters.
    // Sending more characters disconnects every player
    if (message.length() > 127)
    {
        message = message.left(127);
    }

    BYTEARRAY toPIDs;
    BYTEARRAY packet;

    if (!gproxy->m_GameStarted)
    {
        foreach(Slot* slot, slotList)
        {
            if (slot->getSlotStatus() == Slot::OCCUPIED && slot->getComputerStatus() == Slot::NO_COMPUTER)
            {
                toPIDs.push_back(slot->getPlayerId());
            }
        }

        packet = gproxy->m_GameProtocol->SEND_W3GS_CHAT_TO_HOST(gproxy->m_ChatPID, toPIDs, 16, BYTEARRAY(), message);
    }
    else if (alliesOnly)
    {
        foreach(Slot* slot, slotList)
        {
            if (slot->getTeam() == teamNumber && slot->getSlotStatus() == Slot::OCCUPIED
                    && slot->getComputerStatus() == Slot::NO_COMPUTER)
            {
                toPIDs.push_back(slot->getPlayerId());
            }
        }

        BYTEARRAY extraFlags;
        extraFlags.push_back(1);
        extraFlags.push_back(0);
        extraFlags.push_back(0);
        extraFlags.push_back(0);

        packet = gproxy->m_GameProtocol->SEND_W3GS_CHAT_TO_HOST(gproxy->m_ChatPID, toPIDs, 32, extraFlags, message);
    }
    else
    {
        foreach (Slot* slot, slotList)
        {
            if (slot->getSlotStatus() == Slot::OCCUPIED && slot->getComputerStatus() == Slot::NO_COMPUTER)
            {
                toPIDs.push_back(slot->getPlayerId());
            }
        }

        BYTEARRAY extraFlags;
        extraFlags.push_back(0);
        extraFlags.push_back(0);
        extraFlags.push_back(0);
        extraFlags.push_back(0);

        packet = gproxy->m_GameProtocol->SEND_W3GS_CHAT_TO_HOST(gproxy->m_ChatPID, toPIDs, 32, extraFlags, message);
    }

    if (CheckForwarding(message))
    {
        m_PacketBuffer.push(new CCommandPacket(W3GS_HEADER_CONSTANT, packet[1], packet));
        m_LocalPackets.push(new CCommandPacket(W3GS_HEADER_CONSTANT, packet[1], packet));
        m_TotalPacketsReceivedFromLocal++;
    }
}

void CGProxy::changeTeam (unsigned char team)
{
    BYTEARRAY toPIDs;

    foreach (Slot* slot, slotList)
    {
        if (slot->getSlotStatus() == Slot::OCCUPIED
                && slot->getComputerStatus() == Slot::NO_COMPUTER)
        {
            toPIDs.push_back(slot->getPlayerId());
        }
    }

    BYTEARRAY Packet = gproxy->m_GameProtocol->SEND_W3GS_TEAMCHANGE(gproxy->m_ChatPID, toPIDs, team);

    m_PacketBuffer.push(new CCommandPacket(W3GS_HEADER_CONSTANT, Packet[1], Packet));
    m_LocalPackets.push(new CCommandPacket(W3GS_HEADER_CONSTANT, Packet[1], Packet));
    m_TotalPacketsReceivedFromLocal++;
}

void CGProxy::leaveLobby ()
{
    m_LeaveGameSent = true;
    BYTEARRAY packet = m_GameProtocol->SEND_W3GS_LEAVEGAME(PLAYERLEAVE_LOBBY);
    m_RemoteSocket->PutBytes(packet);
    
    m_BNET->QueueEnterChat();

    m_LocalSocket->Disconnect();
    delete m_LocalSocket;
    m_LocalSocket = NULL;
}

void CGProxy::SendLocalChat (QString message)
{
    if (m_LocalSocket)
    {
        if (m_GameStarted)
        {
            if (message.length() > 127)
            {
                message = message.left(127);
            }

            m_LocalSocket->PutBytes(m_GameProtocol->SEND_W3GS_CHAT_FROM_HOST(m_ChatPID, Util::createByteArray(m_ChatPID), 32, Util::createByteArray((uint32_t) 0, false), message));
        }
        else
        {
            if (message.size() > 254)
            {
                message = message.left(254);
            }

            m_LocalSocket->PutBytes(m_GameProtocol->SEND_W3GS_CHAT_FROM_HOST(m_ChatPID, Util::createByteArray(m_ChatPID), 16, BYTEARRAY(), message));
        }
    }
}

void CGProxy::SendEmptyAction ()
{
    // we can't send any empty actions while the lag screen is up
    // so we keep track of who the lag screen is currently showing (if anyone) and we tear it down, send the empty action, and put it back up

    for (vector<unsigned char> ::iterator i = m_Laggers.begin(); i != m_Laggers.end(); i++)
    {
        BYTEARRAY StopLag;
        StopLag.push_back(0xF7);
        StopLag.push_back(0x11);
        StopLag.push_back(0x09);
        StopLag.push_back(0);
        StopLag.push_back(*i);
        Util::appendByteArray(StopLag, (uint32_t) 60000, false);
        m_LocalSocket->PutBytes(StopLag);
    }

    BYTEARRAY EmptyAction;
    EmptyAction.push_back(0xF7);
    EmptyAction.push_back(0x0C);
    EmptyAction.push_back(0x06);
    EmptyAction.push_back(0x00);
    EmptyAction.push_back(0x00);
    EmptyAction.push_back(0x00);
    m_LocalSocket->PutBytes(EmptyAction);

    if (!m_Laggers.empty())
    {
        BYTEARRAY StartLag;
        StartLag.push_back(0xF7);
        StartLag.push_back(0x10);
        StartLag.push_back(0);
        StartLag.push_back(0);
        StartLag.push_back(m_Laggers.size());

        for (vector<unsigned char> ::iterator i = m_Laggers.begin(); i != m_Laggers.end(); i++)
        {
            // using a lag time of 60000 ms means the counter will start at zero
            // hopefully warcraft 3 doesn't care about wild variations in the lag time in subsequent packets

            StartLag.push_back(*i);
            Util::appendByteArray(StartLag, (uint32_t) 60000, false);
        }

        BYTEARRAY LengthBytes;
        LengthBytes = Util::createByteArray((uint16_t) StartLag.size(), false);
        StartLag[2] = LengthBytes[0];
        StartLag[3] = LengthBytes[1];
        m_LocalSocket->PutBytes(StartLag);
    }
}

bool CGProxy::checkStatus (int statusCode)
{
    switch (statusCode)
    {
        case 0:
        {
            CONSOLE_Print(ColoredMessage("[GPROXY] Configuration file successfully loaded", ColoredMessage::GPROXY));
            return true;
        }
        case 1:
        {
            emit signal_showConfigDialog(true);
            return false;
        }
        case 2:
        {
            CONSOLE_Print(ColoredMessage("[ERROR] Could not access the configuration file.\n"
                    "Maybe you dont have permissions to write to that directory.\n"
                    "Try to move your folder outside \"C:\\Program Files\" or run as administrator."
                    "\nShutting down GProxy.", ColoredMessage::ERROR));
            return false;
        }
        default:
        {
            CONSOLE_Print(ColoredMessage("[ERROR] Unknown error while loading configuration file.\n"
                    "ErrorCode: " + QString::number(statusCode), ColoredMessage::ERROR));
            return false;
        }
    }
}

/**
 * Sends "spoofcheck" as whisper to the host to spoofcheck.
 */
void CGProxy::spoofcheck ()
{
    if (gproxy->m_BNET->GetLoggedIn())
    {
        gproxy->m_BNET->QueueChatCommand("spoofcheck", gproxy->m_HostName, true);
    }
}

void CGProxy::setServer (const QString &server)
{
    this->server = server;
}

void CGProxy::setUsername (const QString &username)
{
    this->username = username;
}

void CGProxy::setPassword (const QString &password)
{
    this->password = password;
}

void CGProxy::setWar3version (const uint32_t &war3version)
{
    this->war3version = war3version;
}

void CGProxy::setPort (const uint16_t &port)
{
    this->port = port;
}

void CGProxy::setExeversion (const BYTEARRAY &exeversion)
{
    this->exeversion = exeversion;
}

void CGProxy::setExeversionhash (const BYTEARRAY &exeversionhash)
{
    this->exeversionhash = exeversionhash;
}

void CGProxy::setPasswordhashtype (const QString &passwordhashtype)
{
    this->passwordhashtype = passwordhashtype;
}

void CGProxy::setChannel (const QString &channel)
{
    this->channel = channel;
}

QString CGProxy::getServer ()
{
    return this->server;
}

QString CGProxy::getUsername ()
{
    return this->username;
}

QString CGProxy::getPassword ()
{
    return this->password;
}

uint32_t CGProxy::getWar3version ()
{
    return this->war3version;
}

uint16_t CGProxy::getPort ()
{
    return this->port;
}

BYTEARRAY CGProxy::getExeversion ()
{
    return this->exeversion;
}

BYTEARRAY CGProxy::getExeversionhash ()
{
    return this->exeversionhash;
}

QString CGProxy::getPasswordhashtype ()
{
    return this->passwordhashtype;
}
QString CGProxy::getChannel ()
{
    return this->channel;
}


QList<Player*> CGProxy::getPlayers()
{
    return this->players;
}

void CGProxy::setPlayers(const QList<Player*> &players)
{
    this->players = players;
}

Player* CGProxy::getLastLeaver()
{
    return this->lastLeaver;
}

void CGProxy::setSlotList(const QList<Slot*> slotList)
{
    this->slotList = slotList;
}

QList<Slot*> CGProxy::getSlotList()
{
    return this->slotList;
}

#define ERROR 0 /* Redefine ERROR macro */