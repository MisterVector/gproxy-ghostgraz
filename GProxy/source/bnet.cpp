/*

   Copyright 2010 Trevor Hogan

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

 */

#include "gproxy.h"
#include "util/Util.h"
#include "socket.h"
#include "commandpacket.h"
#include "bncsutilinterface.h"
#include "bnetprotocol.h"
#include "bnet.h"
#include "gameprotocol.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QSound>

#undef ERROR /* Undefine ERROR macro. Needed for ColoredMessage::ERROR. */

using namespace std;

uint32_t lastgames = getElapsedSeconds() - 7;

CBNET::CBNET (CGProxy *nGProxy, MainGUI *p_mainGUI, string nServer, string nBNLSServer, uint16_t nBNLSPort, uint32_t nBNLSWardenCookie, string nCountryAbbrev, string nCountry, string nUserName, string nUserPassword, string nFirstChannel, unsigned char nWar3Version, BYTEARRAY nEXEVersion, BYTEARRAY nEXEVersionHash, string nPasswordHashType, uint32_t nMaxMessageLength)
{
    m_GProxy = nGProxy;
    mainGUI = p_mainGUI;

    m_SearchGameCount=0;//phy [20]
	m_Socket = new CTCPClient( );
	m_Protocol = new CBNETProtocol( );
	// m_BNLSClient = NULL;
	m_BNCSUtil = new CBNCSUtilInterface( nUserName, nUserPassword );
	m_Server = nServer;
	m_repeat_display_games = 0;

	if( nPasswordHashType == "pvpgn" && !nBNLSServer.empty( ) )
	{
		CONSOLE_Print(ColoredMessage("[BNET] pvpgn connection found with a configured BNLS server, ignoring BNLS server", ColoredMessage::BNET) );
		nBNLSServer.clear( );
		nBNLSPort = 0;
		nBNLSWardenCookie = 0;
	}

	m_BNLSServer = nBNLSServer;
	m_BNLSPort = nBNLSPort;
	m_BNLSWardenCookie = nBNLSWardenCookie;
	m_CDKeyROC = m_GProxy->getCDKeyROC().toStdString();
	m_CDKeyTFT = m_GProxy->getCDKeyTFT().toStdString();

	// remove dashes from CD keys and convert to uppercase

	m_CDKeyROC.erase( remove( m_CDKeyROC.begin( ), m_CDKeyROC.end( ), '-' ), m_CDKeyROC.end( ) );
	m_CDKeyTFT.erase( remove( m_CDKeyTFT.begin( ), m_CDKeyTFT.end( ), '-' ), m_CDKeyTFT.end( ) );
	transform( m_CDKeyROC.begin( ), m_CDKeyROC.end( ), m_CDKeyROC.begin( ), (int(*)(int))toupper );
	transform( m_CDKeyTFT.begin( ), m_CDKeyTFT.end( ), m_CDKeyTFT.begin( ), (int(*)(int))toupper );

	if( m_CDKeyROC.size( ) != 26 )
		CONSOLE_Print(ColoredMessage("[BNET] warning - your ROC CD key is not 26 characters long and is probably invalid", ColoredMessage::BNET) );

	if( m_GProxy->m_TFT && m_CDKeyTFT.size( ) != 26 )
		CONSOLE_Print(ColoredMessage("[BNET] warning - your TFT CD key is not 26 characters long and is probably invalid", ColoredMessage::BNET) );

	m_CountryAbbrev = nCountryAbbrev;
	m_Country = nCountry;
	m_UserName = nUserName;
	m_UserPassword = nUserPassword;
	joiningChannel = QString::fromStdString(nFirstChannel);
	m_ListPublicGames = false;
	m_SearchGameNameTime = 0;
	m_War3Version = nWar3Version;
	m_EXEVersion = nEXEVersion;
	m_EXEVersionHash = nEXEVersionHash;
	m_PasswordHashType = nPasswordHashType;
	m_MaxMessageLength = nMaxMessageLength;
	m_LastDisconnectedTime = 0;
	m_LastConnectionAttemptTime = 0;
	m_LastNullTime = 0;
	m_LastOutPacketTicks = 0;
	m_LastOutPacketSize = 0;
	m_LastGetPublicListTime = 0;
	m_LastGetSearchGameTime = 0;
	m_FirstConnect = true;
	m_WaitingToConnect = true;
	m_LoggedIn = false;
	m_InChat = false;
	m_InGame = false;
}

int GETlengthofgame (string gamename)//phy autosearch "creating public game...
{
    int i = 22;
    while ((i < 100) && (gamename.substr(i, 2) != "]."))
    {
        i = i + 1;
    }
    return i - 22;
}

CBNET::~CBNET ()
{
    delete m_Socket;
    delete m_Protocol;
    // delete m_BNLSClient;

    while (!m_Packets.empty())
    {
        delete m_Packets.front();
        m_Packets.pop();
    }

    delete m_BNCSUtil;
}

BYTEARRAY CBNET::GetUniqueName ()
{
    return m_Protocol->GetUniqueName();
}

unsigned int CBNET::SetFD (void *fd, void *send_fd, int *nfds)
{
    unsigned int NumFDs = 0;

    if (!m_Socket->HasError() && m_Socket->GetConnected())
    {
        m_Socket->SetFD((fd_set *) fd, (fd_set *) send_fd, nfds);
        NumFDs++;

        /* if( m_BNLSClient )
                NumFDs += m_BNLSClient->SetFD( fd, send_fd, nfds ); */
    }

    return NumFDs;
}

bool CBNET::Update (void *fd, void *send_fd)
{
    // we return at the end of each if statement so we don't have to deal with errors related to the order of the if statements
    // that means it might take a few ms longer to complete a task involving multiple steps (in this case, reconnecting) due to blocking or sleeping
    // but it's not a big deal at all, maybe 100ms in the worst possible case (based on a 50ms blocking time)

    if (m_Socket->HasError())
    {
        // the socket has an error

        CONSOLE_Print(ColoredMessage("[BNET] disconnected from battle.net due to socket error", ColoredMessage::BNET));

        if (m_Socket->GetError() == ECONNRESET
                && getElapsedSeconds() - m_LastConnectionAttemptTime <= 15)
        {
            CONSOLE_Print(ColoredMessage("[BNET] warning - you are probably temporarily IP banned from battle.net", ColoredMessage::BNET));
        }
        else
        {
            if (m_GProxy->m_LocalSocket)
                m_GProxy->SendLocalChat("Disconnected from battle.net.");
        }

        CONSOLE_Print(ColoredMessage("[BNET] waiting 90 seconds to reconnect", ColoredMessage::BNET));
        /* delete m_BNLSClient;
        m_BNLSClient = NULL; */
        m_BNCSUtil->Reset(m_UserName, m_UserPassword);
        m_Socket->Reset();
        m_LastDisconnectedTime = getElapsedSeconds();
        m_LoggedIn = false;
        m_InChat = false;
        m_InGame = false;
        m_WaitingToConnect = true;
        return false;
    }

    if (!m_Socket->GetConnecting() && !m_Socket->GetConnected() && !m_WaitingToConnect)
    {
        // the socket was disconnected

        CONSOLE_Print(ColoredMessage("[BNET] disconnected from battle.net", ColoredMessage::BNET));

        if (m_GProxy->m_LocalSocket)
            m_GProxy->SendLocalChat("Disconnected from battle.net.");

        CONSOLE_Print(ColoredMessage("[BNET] waiting 90 seconds to reconnect", ColoredMessage::BNET));
        m_BNCSUtil->Reset(m_UserName, m_UserPassword);
        m_Socket->Reset();
        m_LastDisconnectedTime = getElapsedSeconds();
        m_LoggedIn = false;
        m_InChat = false;
        m_InGame = false;
        m_WaitingToConnect = true;
        return false;
    }

    if (m_Socket->GetConnected())
    {
        // the socket is connected and everything appears to be working properly
        m_Socket->DoRecv((fd_set *) fd);
        ExtractPackets();
        ProcessPackets();

        // request the public game list every 15 seconds
        if (!m_GProxy->m_LocalSocket && m_ListPublicGames
                && getElapsedSeconds() - m_LastGetPublicListTime >= 15
                && m_OutPackets.size() <= 2)
        {
            // request 20 games (note: it seems like 20 is the maximum, requesting more doesn't result in more results returned)

            QueueGetGameList(20);
            m_LastGetPublicListTime = getElapsedSeconds();
        }

        if (!m_SearchGameName.empty()
                && getElapsedSeconds() - m_SearchGameNameTime >= 120)
        {
            CONSOLE_Print(ColoredMessage("[BNET] stopped searching for game \"" + QString::fromStdString(m_SearchGameName) + "\"", ColoredMessage::BNET));
            autosearch(true);
            m_SearchGameNameTime = getElapsedSeconds();
            m_SearchGameName.clear();
        }

        if (!m_GProxy->m_LocalSocket && !m_SearchGameName.empty()
                && getElapsedSeconds() - m_LastGetSearchGameTime >= 7
                && m_OutPackets.size() <= 2)
        {
            QueueGetGameList(m_SearchGameName);
            m_LastGetSearchGameTime = getElapsedSeconds();
        }

        // check if at least one packet is waiting to be sent and if we've waited long enough to prevent flooding
        // this formula has changed many times but currently we wait 1 second if the last packet was "small", 3.5 seconds if it was "medium", and 4 seconds if it was "big"

        uint32_t WaitTicks = 0;

        if (m_LastOutPacketSize < 10)
            WaitTicks = 1000;
        else if (m_LastOutPacketSize < 100)
            WaitTicks = 3500;
        else
            WaitTicks = 4000;

        if (!m_OutPackets.empty()
                && getElapsedMilliseconds() - m_LastOutPacketTicks >= WaitTicks)
        {
            if (m_OutPackets.size() > 7)
                CONSOLE_Print(ColoredMessage("[BNET] packet queue warning - there are " + QString::fromStdString(Util::toString(m_OutPackets.size())) + " packets waiting to be sent", ColoredMessage::BNET));

            m_Socket->PutBytes(m_OutPackets.front());
            m_LastOutPacketSize = ((BYTEARRAY) m_OutPackets.front()).size();
            m_OutPackets.pop();
            m_LastOutPacketTicks = getElapsedMilliseconds();
        }

        // send a null packet every 60 seconds to detect disconnects
        if (getElapsedSeconds() - m_LastNullTime >= 60
                && getElapsedMilliseconds() - m_LastOutPacketTicks >= 60000)
        {
            m_Socket->PutBytes(m_Protocol->SEND_SID_NULL());
            m_LastNullTime = getElapsedSeconds();
        }

        m_Socket->DoSend((fd_set *) send_fd);
        return false;
    }

    if (m_Socket->GetConnecting())
    {
        // we are currently attempting to connect to battle.net
        if (m_Socket->CheckConnect())
        {
            // the connection attempt completed

            CONSOLE_Print(ColoredMessage("[BNET] connected", ColoredMessage::BNET));
            m_Socket->PutBytes(m_Protocol->SEND_PROTOCOL_INITIALIZE_SELECTOR());
            m_Socket->PutBytes(m_Protocol->SEND_SID_AUTH_INFO(m_War3Version, m_GProxy->m_TFT, m_CountryAbbrev, m_Country));
            m_Socket->DoSend((fd_set *) send_fd);
            m_LastNullTime = getElapsedSeconds();
            m_LastOutPacketTicks = getElapsedMilliseconds();

            while (!m_OutPackets.empty())
                m_OutPackets.pop();

            return false;
        }
        else if (getElapsedSeconds() - m_LastConnectionAttemptTime >= 15)
        {
            // the connection attempt timed out (15 seconds)
            CONSOLE_Print(ColoredMessage("[BNET] connect timed out", ColoredMessage::BNET));
            CONSOLE_Print(ColoredMessage("[BNET] waiting 90 seconds to reconnect", ColoredMessage::BNET));
            m_Socket->Reset();
            m_LastDisconnectedTime = getElapsedSeconds();
            m_WaitingToConnect = true;
            return false;
        }
    }

    if (!m_Socket->GetConnecting() && !m_Socket->GetConnected()
            && (m_FirstConnect || getElapsedSeconds() - m_LastDisconnectedTime >= 90))
    {
        // attempt to connect to battle.net
        m_FirstConnect = false;

        CONSOLE_Print(ColoredMessage("[BNET] connecting to server [" + QString::fromStdString(m_Server) + "] on port 6112", ColoredMessage::BNET));

        if (m_ServerIP.empty())
        {
            m_Socket->Connect("", m_Server, 6112);

            if (!m_Socket->HasError())
            {
                m_ServerIP = m_Socket->GetIPString();
                CONSOLE_Print(ColoredMessage("[BNET] resolved and cached server IP address " + QString::fromStdString(m_ServerIP), ColoredMessage::BNET));
            }
        }
        else
        {
            // use cached server IP address since resolving takes time and is blocking
            CONSOLE_Print(ColoredMessage("[BNET] using cached server IP address " + QString::fromStdString(m_ServerIP), ColoredMessage::BNET));
            m_Socket->Connect("", m_ServerIP, 6112);
        }

        m_WaitingToConnect = false;
        m_LastConnectionAttemptTime = getElapsedSeconds();
        return false;
    }

    return false;
}

void CBNET::ExtractPackets ()
{
    // extract as many packets as possible from the socket's receive buffer and put them in the m_Packets queue
    string *RecvBuffer = m_Socket->GetBytes();
    BYTEARRAY Bytes = Util::createByteArray((unsigned char *) RecvBuffer->c_str(), RecvBuffer->size());

    // a packet is at least 4 bytes so loop as long as the buffer contains 4 bytes
    while (Bytes.size() >= 4)
    {
        // byte 0 is always 255
        if (Bytes[0] == BNET_HEADER_CONSTANT)
        {
            // bytes 2 and 3 contain the length of the packet
            uint16_t Length = Util::byteArrayToUInt16(Bytes, false, 2);

            if (Length >= 4)
            {
                if (Bytes.size() >= Length)
                {
                    m_Packets.push(new CCommandPacket(BNET_HEADER_CONSTANT, Bytes[1], BYTEARRAY(Bytes.begin(), Bytes.begin() + Length)));
                    *RecvBuffer = RecvBuffer->substr(Length);
                    Bytes = BYTEARRAY(Bytes.begin() + Length, Bytes.end());
                }
                else
                    return;
            }
            else
            {
                CONSOLE_Print(ColoredMessage("[BNET] error - received invalid packet from battle.net (bad length), disconnecting", ColoredMessage::BNET));
                m_Socket->Disconnect();
                return;
            }
        }
        else
        {
            CONSOLE_Print(ColoredMessage("[BNET] error - received invalid packet from battle.net (bad header constant), disconnecting", ColoredMessage::BNET));
            m_Socket->Disconnect();
            return;
        }
    }
}

void CBNET::ProcessPackets ()
{
    vector<CIncomingGameHost *> Games;
    uint32_t GamesReceived = 0;
    uint32_t OldReliableGamesReceived = 0;
    uint32_t NewReliableGamesReceived = 0;
    CIncomingChatEvent *ChatEvent = NULL;
    BYTEARRAY WardenData;

    // process all the received packets in the m_Packets queue
    // this normally means sending some kind of response

    while (!m_Packets.empty())
    {
        CCommandPacket *Packet = m_Packets.front();
        m_Packets.pop();

        if (Packet->GetPacketType() == BNET_HEADER_CONSTANT)
        {
            switch (Packet->GetID())
            {
                case CBNETProtocol::SID_NULL:
                    // warning: we do not respond to NULL packets with a NULL packet of our own
                    // this is because PVPGN servers are programmed to respond to NULL packets so it will create a vicious cycle of useless traffic
                    // official battle.net servers do not respond to NULL packets

                    m_Protocol->RECEIVE_SID_NULL(Packet->GetData());
                    break;

                case CBNETProtocol::SID_GETADVLISTEX:
                    Games = m_Protocol->RECEIVE_SID_GETADVLISTEX(Packet->GetData());

                    // check for reliable games
                    // GHost++ uses specific invalid map dimensions (1984) to indicated reliable games

                    GamesReceived = Games.size();

                    for (vector<CIncomingGameHost *> ::iterator i = Games.begin(); i != Games.end();)
                    {
                        if (!m_PublicGameFilter.empty() && ((CIncomingGameHost *) (*i))->GetGameName() != m_SearchGameName)
                        {
                            string FilterLower = m_PublicGameFilter;
                            string GameNameLower = ((CIncomingGameHost *) (*i))->GetGameName();
                            transform(FilterLower.begin(), FilterLower.end(), FilterLower.begin(), (int(*)(int))tolower);
                            transform(GameNameLower.begin(), GameNameLower.end(), GameNameLower.begin(), (int(*)(int))tolower);

                            if (GameNameLower.find(FilterLower) == string::npos)
                            {
                                delete *i;
                                i = Games.erase(i);
                                continue;
                            }
                        }

                        if (m_GProxy->AddGame(*i))
                        {
                            NewReliableGamesReceived++;
                            /*
                                (DWORD) Host Counter (Game ID)
                                (DWORD) Entry Key (used in LAN)
                                (BYTE) Unknown
                                (WORD) Listen Port
                                (DWORD) Peer Key
                                (STRING) Player name
                                (DWORD) Unknown
                                (WORD) Internal Port
                                (DWORD) Internal IP
                             */
//                            if(!GetInGame())
//                            {
//                                m_GProxy->m_RemoteSocket->Reset();
//                                m_GProxy->m_RemoteSocket->SetNoDelay(true);
//
//                                BYTEARRAY test = (*i)->GetIP();
//                                BYTEARRAY ip_cstr = Util::ExtractCString(test, 0);
//                                string ip = string(ip_cstr.begin(), ip_cstr.end());
//
//                                m_GProxy->m_RemoteSocket->Connect(string(), ip, (*i)->GetPort());
//                                m_GProxy->m_RemoteSocket->PutBytes(
//                                m_GProxy->m_GameProtocol->SEND_W3GS_REQJOIN(
//                                        (*i)->GetHostCounter(),
//                                        (uint32_t) 0, (unsigned char) 0,
//                                        (uint16_t) 6126, (uint32_t) 2,
//                                        "Manufactoring", (uint32_t) 0,
//                                        (uint16_t) 0, (uint32_t) 0));
//                            }
                        }
                        else
                        {
                            OldReliableGamesReceived++;
                        }

                        i++;
                    }

                    /* if( GamesReceived > 0 )
                            CONSOLE_Print( "[BNET] sifted game list, found " + Util::ToString( OldReliableGamesReceived + NewReliableGamesReceived ) + "/" + Util::ToString( GamesReceived ) + " reliable games (" + Util::ToString( OldReliableGamesReceived ) + " duplicates)" ); */

                    break;

                case CBNETProtocol::SID_ENTERCHAT:
                    if (m_Protocol->RECEIVE_SID_ENTERCHAT(Packet->GetData()))
                    {
                        CONSOLE_Print(ColoredMessage("[BNET] joining channel [" + joiningChannel + "]", ColoredMessage::BNET));
                        m_InChat = true;
                        m_InGame = false;
                        m_Socket->PutBytes(m_Protocol->SEND_SID_JOINCHANNEL(CBNETProtocol::JCF_FORCED_JOIN, joiningChannel.toStdString()));
                        m_Socket->PutBytes(m_Protocol->SEND_SID_FRIENDSLIST());
                    }

                    break;

                case CBNETProtocol::SID_CHATEVENT:
                    ChatEvent = m_Protocol->RECEIVE_SID_CHATEVENT(Packet->GetData());

                    if (ChatEvent)
                        ProcessChatEvent(ChatEvent);

                    delete ChatEvent;
                    ChatEvent = NULL;
                    break;

                case CBNETProtocol::SID_CHECKAD:
                    m_Protocol->RECEIVE_SID_CHECKAD(Packet->GetData());
                    break;

                case CBNETProtocol::SID_STARTADVEX3:
                    if (m_Protocol->RECEIVE_SID_STARTADVEX3(Packet->GetData()))
                    {
                        m_InChat = false;
                        m_InGame = true;
                    }
                    else
                        CONSOLE_Print(ColoredMessage("[BNET] startadvex3 failed", ColoredMessage::BNET));

                    break;

                case CBNETProtocol::SID_PING:
                    m_Socket->PutBytes(m_Protocol->SEND_SID_PING(m_Protocol->RECEIVE_SID_PING(Packet->GetData())));
                    break;

                case CBNETProtocol::SID_AUTH_INFO:
                    if (m_Protocol->RECEIVE_SID_AUTH_INFO(Packet->GetData()))
                    {
                        if (m_BNCSUtil->HELP_SID_AUTH_CHECK(m_GProxy->m_TFT, m_GProxy->getWar3Path().toStdString(), m_CDKeyROC, m_CDKeyTFT, m_Protocol->GetValueStringFormulaString(), m_Protocol->GetIX86VerFileNameString(), m_Protocol->GetClientToken(), m_Protocol->GetServerToken()))
                        {
                            // override the exe information generated by bncsutil if specified in the config file
                            // apparently this is useful for pvpgn users

                            if (m_EXEVersion.size() == 4)
                            {
                                CONSOLE_Print(ColoredMessage("[BNET] using custom exe version bnet_custom_exeversion = " + QString::number(m_EXEVersion[0]) + " " + QString::number(m_EXEVersion[1]) + " " + QString::number(m_EXEVersion[2]) + " " + QString::number(m_EXEVersion[3]), ColoredMessage::BNET));
                                m_BNCSUtil->SetEXEVersion(m_EXEVersion);
                            }

                            if (m_EXEVersionHash.size() == 4)
                            {
                                CONSOLE_Print(ColoredMessage("[BNET] using custom exe version hash bnet_custom_exeversionhash = " + QString::number(m_EXEVersionHash[0]) + " " + QString::number(m_EXEVersionHash[1]) + " " + QString::number(m_EXEVersionHash[2]) + " " + QString::number(m_EXEVersionHash[3]), ColoredMessage::BNET));
                                m_BNCSUtil->SetEXEVersionHash(m_EXEVersionHash);
                            }

                            if (m_GProxy->m_TFT)
                            {
                                CONSOLE_Print(ColoredMessage("[BNET] attempting to auth as Warcraft III: The Frozen Throne", ColoredMessage::BNET));
                            }
                            else
                            {
                                CONSOLE_Print(ColoredMessage("[BNET] attempting to auth as Warcraft III: Reign of Chaos", ColoredMessage::BNET));
                            }

                            m_Socket->PutBytes(m_Protocol->SEND_SID_AUTH_CHECK(m_GProxy->m_TFT, m_Protocol->GetClientToken(), m_BNCSUtil->GetEXEVersion(), m_BNCSUtil->GetEXEVersionHash(), m_BNCSUtil->GetKeyInfoROC(), m_BNCSUtil->GetKeyInfoTFT(), m_BNCSUtil->GetEXEInfo(), m_GProxy->getCdKeyOwner().toStdString()));

                            // the Warden seed is the first 4 bytes of the ROC key hash
                            // initialize the Warden handler

                            /*

                            if( !m_BNLSServer.empty( ) )
                            {
                                    CONSOLE_Print( "[BNET] creating BNLS client" );
                                    delete m_BNLSClient;
                                    m_BNLSClient = new CBNLSClient( m_BNLSServer, m_BNLSPort, m_BNLSWardenCookie );
                                    m_BNLSClient->QueueWardenSeed( Util::ByteArrayToUInt32( m_BNCSUtil->GetKeyInfoROC( ), false, 16 ) );
                            }

                             */
                        }
                        else
                        {
                            CONSOLE_Print(ColoredMessage("[BNET] logon failed - bncsutil key hash failed (check your Warcraft 3 path and cd keys), disconnecting", ColoredMessage::BNET));
                            m_Socket->Disconnect();
                            delete Packet;
                            return;
                        }
                    }

                    break;

                case CBNETProtocol::SID_AUTH_CHECK:
                    if (m_Protocol->RECEIVE_SID_AUTH_CHECK(Packet->GetData()))
                    {
                        // cd keys accepted

                        CONSOLE_Print(ColoredMessage("[BNET] cd keys accepted", ColoredMessage::BNET));
                        m_BNCSUtil->HELP_SID_AUTH_ACCOUNTLOGON();
                        m_Socket->PutBytes(m_Protocol->SEND_SID_AUTH_ACCOUNTLOGON(m_BNCSUtil->GetClientKey(), m_UserName));
                    }
                    else
                    {
                        // cd keys not accepted

                        switch (Util::byteArrayToUInt32(m_Protocol->GetKeyState(), false))
                        {
                            case CBNETProtocol::KR_ROC_KEY_IN_USE:
                                CONSOLE_Print(ColoredMessage("[BNET] logon failed - ROC CD key in use by user [" + QString::fromStdString(m_Protocol->GetKeyStateDescription()) + "], disconnecting", ColoredMessage::BNET));
                                break;
                            case CBNETProtocol::KR_TFT_KEY_IN_USE:
                                CONSOLE_Print(ColoredMessage("[BNET] logon failed - TFT CD key in use by user [" + QString::fromStdString(m_Protocol->GetKeyStateDescription()) + "], disconnecting", ColoredMessage::BNET));
                                break;
                            case CBNETProtocol::KR_OLD_GAME_VERSION:
                                CONSOLE_Print(ColoredMessage("[BNET] logon failed - game version is too old, disconnecting", ColoredMessage::BNET));
                                break;
                            case CBNETProtocol::KR_INVALID_VERSION:
                                CONSOLE_Print(ColoredMessage("[BNET] logon failed - game version is invalid, disconnecting", ColoredMessage::BNET));
                                break;
                            default:
                                CONSOLE_Print(ColoredMessage("[BNET] logon failed - cd keys not accepted, disconnecting", ColoredMessage::BNET));
                                break;
                        }

                        m_Socket->Disconnect();
                        delete Packet;
                        return;
                    }

                    break;

                case CBNETProtocol::SID_AUTH_ACCOUNTLOGON:
                    if (m_Protocol->RECEIVE_SID_AUTH_ACCOUNTLOGON(Packet->GetData()))
                    {
                        CONSOLE_Print(ColoredMessage("[BNET] username [" + QString::fromStdString(m_UserName) + "] accepted", ColoredMessage::BNET));

                        if (m_PasswordHashType == "pvpgn")
                        {
                            // pvpgn logon

                            CONSOLE_Print(ColoredMessage("[BNET] using pvpgn logon type (for pvpgn servers only)", ColoredMessage::BNET));
                            m_BNCSUtil->HELP_PvPGNPasswordHash(m_UserPassword);
                            m_Socket->PutBytes(m_Protocol->SEND_SID_AUTH_ACCOUNTLOGONPROOF(m_BNCSUtil->GetPvPGNPasswordHash()));
                        }
                        else
                        {
                            // battle.net logon

                            CONSOLE_Print(ColoredMessage("[BNET] using battle.net logon type (for official battle.net servers only)", ColoredMessage::BNET));
                            m_BNCSUtil->HELP_SID_AUTH_ACCOUNTLOGONPROOF(m_Protocol->GetSalt(), m_Protocol->GetServerPublicKey());
                            m_Socket->PutBytes(m_Protocol->SEND_SID_AUTH_ACCOUNTLOGONPROOF(m_BNCSUtil->GetM1()));
                        }
                    }
                    else
                    {
                        CONSOLE_Print(ColoredMessage("[BNET] logon failed - invalid username, disconnecting", ColoredMessage::BNET));
                        m_Socket->Disconnect();
                        delete Packet;
                        return;
                    }

                    break;

                case CBNETProtocol::SID_AUTH_ACCOUNTLOGONPROOF:
                    if (m_Protocol->RECEIVE_SID_AUTH_ACCOUNTLOGONPROOF(Packet->GetData()))
                    {
                        // logon successful

                        CONSOLE_Print(ColoredMessage("[BNET] logon successful", ColoredMessage::BNET));
                        m_LoggedIn = true;
                        m_Socket->PutBytes(m_Protocol->SEND_SID_NETGAMEPORT(6112));
                        m_Socket->PutBytes(m_Protocol->SEND_SID_ENTERCHAT());
                        m_Socket->PutBytes(m_Protocol->SEND_SID_JOINCHANNEL(CBNETProtocol::JCF_FIRST_JOIN, joiningChannel.toStdString()));

                        if (m_GProxy->m_LocalSocket)
                            m_GProxy->SendLocalChat("Connected to battle.net.");

                    }
                    else
                    {
                        CONSOLE_Print(ColoredMessage("[BNET] logon failed - invalid password, disconnecting", ColoredMessage::BNET));

                        // try to figure out if the user might be using the wrong logon type since too many people are confused by this

                        string Server = m_Server;
                        transform(Server.begin(), Server.end(), Server.begin(), (int(*)(int))tolower);

                        if (m_PasswordHashType == "pvpgn" && (Server == "useast.battle.net" || Server == "uswest.battle.net" || Server == "asia.battle.net" || Server == "europe.battle.net"))
                        {
                            CONSOLE_Print(ColoredMessage("[BNET] it looks like you're trying to connect to a battle.net server using a pvpgn logon type, check your config file's \"battle.net custom data\" section", ColoredMessage::BNET));
                        }
                        else if (m_PasswordHashType != "pvpgn" && (Server != "useast.battle.net" && Server != "uswest.battle.net" && Server != "asia.battle.net" && Server != "europe.battle.net"))
                        {
                            CONSOLE_Print(ColoredMessage("[BNET] it looks like you're trying to connect to a pvpgn server using a battle.net logon type, check your config file's \"battle.net custom data\" section", ColoredMessage::BNET));
                        }

                        m_Socket->Disconnect();
                        delete Packet;
                        return;
                    }

                    break;

                case CBNETProtocol::SID_WARDEN:
                    WardenData = m_Protocol->RECEIVE_SID_WARDEN(Packet->GetData());

                    /* if( m_BNLSClient )
                            m_BNLSClient->QueueWardenRaw( WardenData );
                    else */
                    CONSOLE_Print(ColoredMessage("[BNET] warning - received warden packet but no BNLS server is available, you will be kicked from battle.net soon", ColoredMessage::BNET));

                    break;

                case CBNETProtocol::SID_FRIENDSLIST:
                    m_GProxy->updateFriendlist(m_Protocol->RECEIVE_SID_FRIENDSLIST(Packet->GetData()));
                    break;

                case CBNETProtocol::SID_FRIENDSUPDATE:
                    m_GProxy->updateFriend(m_Protocol->RECEIVE_SID_FRIENDSUPDATE(Packet->GetData()));
                    break;

                case CBNETProtocol::SID_FRIENDSADD:
                    m_GProxy->addFriend(m_Protocol->RECEIVE_SID_FRIENDSADD(Packet->GetData()));
                    break;

                case CBNETProtocol::SID_FRIENDSREMOVE:
                    m_GProxy->removeFriend(m_Protocol->RECEIVE_SID_FRIENDSREMOVE(Packet->GetData()));
                    break;

                case CBNETProtocol::SID_CLANMEMBERLIST:
                    break;
            }
        }

        delete Packet;
    }
}

void CBNET::ProcessChatEvent (CIncomingChatEvent * chatEvent)
{
    CBNETProtocol::IncomingChatEvent Event = chatEvent->GetChatEvent();
    QString User = chatEvent->GetUser();
    QString Message = chatEvent->GetMessage();

    if (Event == CBNETProtocol::EID_SHOWUSER
            || Event == CBNETProtocol::EID_JOIN)
    {
        //PX3W 1N3W 0 virP
        QString statString = Message;
        QStringList statStringList = statString.split(" ");
        QString productID;
        QString icon;
        QString level;
        QString clanTag;

        for (int i = 0; i < statStringList.count(); i++)
        {
            switch (i)
            {
                case 0:
                {
                    QByteArray ba = statStringList.at(i).toAscii();
                    char *d = ba.data();
                    std::reverse(d, d + statStringList.at(i).length());
                    productID = QString(d);
                    break;
                }
                case 1:
                {
                    QByteArray ba = statStringList.at(i).toAscii();
                    char *d = ba.data();
                    std::reverse(d, d + statStringList.at(i).length());
                    icon = QString(d);
                    break;
                }
                case 2:
                {
                    level = statStringList.at(i);
                    break;
                }
                case 3:
                {
                    QByteArray ba = statStringList.at(i).toLocal8Bit();
                    char *d = ba.data();
                    std::reverse(d, d + statStringList.at(i).length());
                    clanTag = QString(d);
                    break;
                }
            }

            if (productID != "W3XP")
            {
                break;
            }
        }

        m_GProxy->addChannelUser(User, clanTag);
    }
    else if (Event == CBNETProtocol::EID_LEAVE)
    {
        m_GProxy->removeChannelUser(User);
    }
    else if (Event == CBNETProtocol::EID_WHISPER)
    {
        m_ReplyTarget = User.toStdString();

        m_GProxy->SendLocalChat(User + " whispers: " + Message);
        CONSOLE_Print(ColoredMessage("[WHISPER FROM] [" + User + "] " + Message, ColoredMessage::WHISPER));

        if (Message.mid(0, 30) == "Spoof check by replying to thi")
        {
            CONSOLE_Print(ColoredMessage("[INFO] Trying to autospoofcheck...", ColoredMessage::GAMEINFO));
            m_GProxy->spoofcheck();
        }

        if (m_GProxy->m_PlaySound)
        {
            QSound::play("sounds/whisper.wav");
        }
    }
    else if (Event == CBNETProtocol::EID_TALK)
    {
        QString s = User;
        //phy hide autocreated games
        bool isover = true;
        if (Message.size() > 16)
        {
            if (Message.mid(0, 6) == "Game [" && Message.mid(Message.length() - 10, 10) == "] is over.")
                isover = false;
        }
        if (((Message.mid(0, 22) != "Creating public game [") && (isover)))
        {
            if (m_GProxy->getParrot() != "Ignore ignore" || !Message.startsWith("[PARROT]"))
            {
                CONSOLE_Print(ColoredMessage("["), true, true, false);
                CONSOLE_Print(ColoredMessage(User, ColoredMessage::USERCOLOR), true, false, false);
                CONSOLE_Print(ColoredMessage("] " + Message), true, false, true);
            }
        }

        if (s != "")
        {
            s = s.toLower();
        }
        if (s == m_GProxy->getParrot() && !Message.mid(0, 8).startsWith("[PARROT]"))
        {
            QueueChatCommand("[PARROT]" + Message);
        }
        else if ("Parrot parrot" == m_GProxy->getParrot() && Message.mid(0, 8) != "[PARROT]")
        {
            QueueChatCommand("[PARROT]" + Message);
        }
        if (Message.mid(0, 22) == "Creating public game [" /*&& autodetect()*/)//phy autosearch
        {
            if(getautosearch() && cautosearch())
            {
                CheckForGame(Message.mid(22, GETlengthofgame(Message.toStdString())).toStdString());
            }
        }
        else if (Message.mid(0, 3) == "gn " && Message.length() <= 34 /*&& autodetect()*/)//phy autosearch
        {
            if(getautosearch() && cautosearch())
            {
                CheckForGame(Message.mid(3, Message.length() - 3).toStdString());
            }
        }
        else if (Message.mid(0, 4) == "gn: " && Message.length() <= 35/*&& autodetect()*/)//phy autosearch
        {
            if(getautosearch() && cautosearch())
            {
                CheckForGame(Message.mid(4, Message.length() - 4).toStdString());
            }
        }
    }
    else if (Event == CBNETProtocol::EID_BROADCAST)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[BROADCAST] " + Message, ColoredMessage::DEFAULT));
    }
    else if (Event == CBNETProtocol::EID_CHANNEL)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[BNET] joined channel [" + Message + "]", ColoredMessage::BNET));
        currentChannel = Message;
        m_GProxy->changeChannel(currentChannel);
    }
    else if (Event == CBNETProtocol::EID_CHANNELFULL)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[BNET] channel is full", ColoredMessage::BNET));
    }
    else if (Event == CBNETProtocol::EID_CHANNELDOESNOTEXIST)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[BNET] channel does not exist", ColoredMessage::BNET));
    }
    else if (Event == CBNETProtocol::EID_CHANNELRESTRICTED)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[BNET] channel restricted", ColoredMessage::BNET));
    }
    else if (Event == CBNETProtocol::EID_INFO)
    {
        m_GProxy->SendLocalChat("[INFO] " + Message);
        CONSOLE_Print(ColoredMessage("[INFO] " + Message, ColoredMessage::INFO)); //phy autosearch
        int index = Message.indexOf(" ");
        if (Message.mid(index + 1, 48) == "is using Warcraft III The Frozen Throne in game ")
        {
            CheckForGame(Message.mid(index + 49, Message.length() - index - 50).toStdString());
        }
    }
    else if (Event == CBNETProtocol::EID_ERROR)
    {
        m_GProxy->SendLocalChat("[ERROR] " + Message);
        CONSOLE_Print(ColoredMessage("[ERROR] " + Message, ColoredMessage::ERROR));
    }
    else if (Event == CBNETProtocol::EID_EMOTE)
    {
        m_GProxy->SendLocalChat("[EMOTE] [" + User + "] " + Message);
        CONSOLE_Print(ColoredMessage("[EMOTE] [" + User + "] " + Message, ColoredMessage::EMOTE));
    }
}

void CBNET::updateFriend(const unsigned char& entryNumber)
{
    SendPacket(m_Protocol->SEND_SID_FRIENDSUPDATE(entryNumber));
}

void CBNET::QueueEnterChat ()
{
    if (m_LoggedIn)
        m_OutPackets.push(m_Protocol->SEND_SID_ENTERCHAT());
}

void CBNET::QueueChatCommand (QString chatCommand, bool showMessage)
{
    if (chatCommand.isEmpty())
    {
        return;
    }

    if (m_LoggedIn)
    {
        if (m_PasswordHashType == "pvpgn" && (unsigned int) chatCommand.length() > m_MaxMessageLength)
        {
            chatCommand = chatCommand.mid(0, m_MaxMessageLength);
        }

        if (chatCommand.size() > 255)
        {
            chatCommand = chatCommand.mid(0, 255);
        }

        if (m_OutPackets.size() > 10)
        {
            CONSOLE_Print(ColoredMessage("[BNET] attempted to queue chat command ["
                    + chatCommand + "] but there are too many ("
                    + QString::number(m_OutPackets.size()) + ") packets queued, discarding", ColoredMessage::BNET));
        }
        else
        {
            m_OutPackets.push(m_Protocol->SEND_SID_CHATCOMMAND(chatCommand));

            if (!showMessage)
            {
                return;
            }

            if (chatCommand.startsWith("/w ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO] " + chatCommand.mid(3, chatCommand.length() - 3), ColoredMessage::WHISPER));
            }
            else if (chatCommand.startsWith("/whisper ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO] " + chatCommand.mid(9, chatCommand.length() - 9), ColoredMessage::WHISPER));
            }
            else if (chatCommand.startsWith("/f m ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO ALL FRIENDS] " + chatCommand.mid(5, chatCommand.length() - 5), ColoredMessage::WHISPER));
            }
            else if (chatCommand.startsWith("/f msg ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO ALL FRIENDS] " + chatCommand.mid(7, chatCommand.length() - 7), ColoredMessage::WHISPER));
            }
            else if (chatCommand.startsWith("/friends m ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO ALL FRIENDS] " + chatCommand.mid(11, chatCommand.length() - 11), ColoredMessage::WHISPER));
            }
            else if (chatCommand.startsWith("/friends msg ", Qt::CaseInsensitive))
            {
                CONSOLE_Print(ColoredMessage("[WHISPER TO ALL FRIENDS] " + chatCommand.mid(13, chatCommand.length() - 13), ColoredMessage::WHISPER));
            }
            else
            {
                CONSOLE_Print(ColoredMessage("["), true, true, false);
                CONSOLE_Print(ColoredMessage(m_GProxy->getUsername(), ColoredMessage::USERCOLOR), true, false, false);
                CONSOLE_Print(ColoredMessage("] " + chatCommand), true, false, true);
            }
        }
    }
}

void CBNET::QueueChatCommand (QString chatCommand, string user, bool whisper)
{
    if (chatCommand.isEmpty())
    {
        return;
    }

    if (whisper)
    {
        QueueChatCommand("/w " + QString::fromStdString(user) + " " + chatCommand);
    }
    else
    {
        QueueChatCommand(chatCommand);
    }
}

void CBNET::QueueGetGameList (uint32_t numGames)// phyZZ
{
    if (m_LoggedIn)
        m_OutPackets.push(m_Protocol->SEND_SID_GETADVLISTEX(string(), numGames));
}

void CBNET::QueueGetGameList (string gameName)
{
    if (m_LoggedIn)
        m_OutPackets.push(m_Protocol->SEND_SID_GETADVLISTEX(gameName, m_SearchGameCount));
}

void CBNET::QueueJoinGame (string gameName)
{
    if (m_LoggedIn)
    {
        m_OutPackets.push(m_Protocol->SEND_SID_NOTIFYJOIN(gameName));
        m_InChat = false;
        m_InGame = true;
    }
}

void CBNET::SendPacket (BYTEARRAY Packet)
{
    if (m_LoggedIn)
        m_OutPackets.push(Packet);
}

#define ERROR 0 /* Redefine ERROR macro */