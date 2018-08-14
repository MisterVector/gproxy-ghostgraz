#include "MainGUI.h"
#include "gproxy.h"
#include "bnet.h"
#include "delegate/ChannellistDelegate.h"
#include "delegate/GamelistDelegate.h"
#include "socket.h"
#include "ConfigGUI.h"
#include "GhostGrazLogininformationDialog.h"
#include "util/Util.h"
#include "util/VariantUtil.h"
#include "bnetprotocol.h"

#include <QDesktopWidget>
#include <QScrollBar>
#include <QIcon>
#include <QMessageBox>
#include <QIcon>
#include <QProcess>
#include <QSound>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <QClipboard>
#include <QFileInfo>
#include <QRegExp>
#include <QTextFrame>

#undef ERROR /* Undefine ERROR macro. Needed for ColoredMessage::ERROR. */

MainGUI::MainGUI (CGProxy *p_gproxy)
{
    widget.setupUi(this);
    gproxy = p_gproxy;
}

MainGUI::~MainGUI ()
{
    delete statspage;
    delete gameListDownloader;
    gproxyUpdateThread->stop();
    delete gproxyUpdateThread;
}

void MainGUI::onClose ()
{
    Config* config = gproxy->getConfig();
    config->setInt("width", this->width());
    config->setInt("height", this->height());
    config->commit();

    gproxy->cleanup();
}

void MainGUI::onRestart()
{
    this->hide();
    QString applicationName = QFileInfo(QApplication::applicationFilePath()).fileName();
    QProcess::startDetached(applicationName , QStringList());
    QApplication::quit();
}

void MainGUI::startUpdateThread()
{
    gproxyUpdateThread->start(QThread::HighPriority);
}

void MainGUI::init ()
{
    gameListDownloader = new GameListDownloader("http://0.static.ghostgraz.com/currentgames.txt");
    gameListDownloader->startDownloadInterval(5000);
    gproxyUpdateThread = new GproxyUpdateThread(gproxy);

    widget.channelList->setItemDelegate(new ChannellistDelegate(this));
    widget.gameList->setItemDelegate(new GamelistDelegate());

    widget.outputTextArea->setConfig(gproxy->getConfig());
    
    // Install the event filter to filter key press events.
    widget.inputTextArea->installEventFilter(this);

    QAction *startWarcraftAction = widget.menuBar->addAction("Start Warcraft");
    connect(startWarcraftAction, SIGNAL(activated()),
            this, SLOT(startWarcraft()));

    initStatspage();
    initLayout();
    initSlots();
}

void MainGUI::initStatspage ()
{
    statspage = new Statspage();

    connect(statspage, SIGNAL(loginFinished()),
            this, SLOT(statspageLoginFinished()));
    connect(statspage, SIGNAL(playerInformationReplyFinished(Player *)),
            this, SLOT(receivedPlayerInformation(Player *)));
    connect(statspage, SIGNAL(adminlistReplyFinished(QStringList)),
            this, SLOT(onAdminlistReceived(QStringList)));

    Config* config = gproxy->getConfig();
    QString ghostgrazUsername = config->getString("ghostgrazUsername");
    QString ghostgrazPassword = config->getString("ghostgrazPassword");

    if (ghostgrazUsername.isEmpty() && ghostgrazPassword.isEmpty()
            && config->hasRequiredValues())
    {
        GhostGrazLogininformationDialog *dialog =
                new GhostGrazLogininformationDialog(gproxy->getConfig(), statspage);
        dialog->setModal(true);
        dialog->show();
    }
    else
    {
        statspage->login(ghostgrazUsername, ghostgrazPassword);
    }
}

void MainGUI::initLayout ()
{
    int width = this->width();
    int height = this->height() - widget.menuBar->height();
    
    int inputFieldHeight = 40;
    int channelfieldWidth = 200;
    int channelfieldHeight = 25;
    int gamelistWidth = 220;

    widget.refreshButton->setMinimumSize(gamelistWidth, channelfieldHeight);
    widget.refreshButton->setMaximumSize(gamelistWidth, channelfieldHeight);
    widget.refreshButton->move(width - channelfieldWidth - gamelistWidth, 0);

    widget.titleLabel->setMinimumSize(channelfieldWidth, channelfieldHeight);
    widget.titleLabel->setMaximumSize(channelfieldWidth, channelfieldHeight);
    widget.titleLabel->move(width - channelfieldWidth, 0);

    widget.channelList->setMinimumSize(channelfieldWidth,
            (height - inputFieldHeight - channelfieldHeight) / 2);
    widget.channelList->setMaximumSize(channelfieldWidth,
            (height - inputFieldHeight - channelfieldHeight) / 2);
    widget.channelList->move(width - channelfieldWidth, channelfieldHeight);

    widget.friendList->setMinimumSize(channelfieldWidth,
            (height - inputFieldHeight - channelfieldHeight) / 2);
    widget.friendList->setMaximumSize(channelfieldWidth,
            (height - inputFieldHeight - channelfieldHeight) / 2);
    widget.friendList->move(width - channelfieldWidth, channelfieldHeight +
            (height - inputFieldHeight - channelfieldHeight) / 2);

    widget.gameList->setMinimumSize(gamelistWidth, height - inputFieldHeight - channelfieldHeight);
    widget.gameList->setMaximumSize(gamelistWidth, height - inputFieldHeight - channelfieldHeight);
    widget.gameList->move(width - channelfieldWidth - gamelistWidth, channelfieldHeight);

    widget.inputTextArea->setMinimumSize(width, inputFieldHeight);
    widget.inputTextArea->setMaximumSize(width, inputFieldHeight);
    widget.inputTextArea->move(0, height - inputFieldHeight);

    widget.outputTextArea->setMinimumSize(width - channelfieldWidth - gamelistWidth, height - inputFieldHeight);
    widget.outputTextArea->setMaximumSize(width - channelfieldWidth - gamelistWidth, height - inputFieldHeight);
    widget.outputTextArea->move(0, 0);
}

void MainGUI::initSlots ()
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(onClose()));
    connect(gameListDownloader, SIGNAL(downloadFinished(QList<GameListEntry>)), 
            this, SLOT(onGameListDownloadFinished(QList<GameListEntry>)));
}

void MainGUI::initAdminlist ()
{
    statspage->getAdminlist();
}

/**
 * Configuration options which are only applied once after startup.
 */
void MainGUI::initConfigurations()
{
    Config* config = gproxy->getConfig();

    QDesktopWidget* desktop = QApplication::desktop();

    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int appWidth = config->getInt("width");
    int appHeight = config->getInt("height");

    // Resize if possible. App would crash if the window size is too small.
    if (appWidth >= 500 && appHeight >= 200)
    {
        this->resize(appWidth, appHeight);
    }
    // Center the window.
    this->move((screenWidth - this->width()) / 2, (screenHeight - this->height()) / 2 - 50);

    applyConfig();
}

/**
 * Configuration options which are reloaded every time the config has changed and once after startup.
 */
void MainGUI::applyConfig()
{
    Config* config = gproxy->getConfig();

    setColor("backgroundcolor", config->getColor("backgroundcolor"));
    setColor("_foregroundcolor", QColor::Invalid);
    setFont("outputarea", config->getFont("outputareaFont"));
}

/**
 * Slot for changing the color.
 *
 * @param area The area where the color change should be applied.
 * @param color The new color.
 */
void MainGUI::setColor(const QString& area, const QColor& color)
{
    if (area == "backgroundcolor")
    {
        QPalette palette = widget.centralwidget->palette();
        palette.setColor(QPalette::Base, color);
        widget.centralwidget->setPalette(palette);
        widget.titleLabel->setPalette(palette);
    }
    else if (area.endsWith("_foregroundcolor"))
    {
        // Removing "_foregroundcolor".
        QString element = area.left(area.length() - 16);

        widget.outputTextArea->repaint(element, color);

        if (element.toLower() == "default")
        {
            QPalette palette = widget.titleLabel->palette();
            palette.setColor(QPalette::Text, color);
            widget.titleLabel->setPalette(palette);
        }
    }
}

/**
 * Slot for changing the font.
 *
 * @param area The area where the font change should be applied.
 * @param font The new font.
 */
void MainGUI::setFont(const QString& area, const QFont& font)
{
    if (area == "outputarea")
    {
        widget.outputTextArea->setFont(font);
    }
}

/**
 * Event for the window resize request.
 *
 * @param event QResizeEvent*
 */
void MainGUI::resizeEvent (QResizeEvent* event)
{
    // Resize the window.
    QWidget::resizeEvent(event);
    // Apply layout changes.
    initLayout();
}

bool MainGUI::eventFilter(QObject* watchedObject, QEvent* event)
{
    if (watchedObject == widget.inputTextArea)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = (QKeyEvent*) event;
            return onInputTextAreaKeyPressed(keyEvent);
        }
    }
    
    // Delegate events
    return false;
}

/**
 * A key on the input text area was pressed.
 * 
 * @param event The occurred key event.
 * @return Wheter this event should be delegated to the text area.
 */
bool MainGUI::onInputTextAreaKeyPressed (QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        if (widget.inputTextArea->toPlainText().length() == 0)
        {
            // Do not delegate this event (add text) to the text area.
            return true;
        }
        
        if (event->modifiers() != Qt::ShiftModifier)
        {
            QString input = widget.inputTextArea->toPlainText();
            widget.inputTextArea->clear();

            QStringList lines = input.split('\n');
            
            // Fix of not sending whispers correctly when you have \n on the input text
            // Detect Whisper
            if(lines.first().toLower().startsWith("/w ") and lines.first().size() > 3 ){
                // Add /w [NAME] command on each line
                for(int count = 1; count < lines.size(); ++count)           
                lines[count] = lines.first().mid(0,lines.first().indexOf(' ',lines.first().indexOf(' ',0) + 1) ) + ' ' +  lines[count];
            }
            
            foreach(QString line, lines)
            {
                if (!line.isEmpty())
                {
                    processInput(line);
                }
            }
            
            // Do not delegate this event (add text) to the text area.
            return true;
        }
    }
    else if (event->key() == Qt::Key_Space 
            && widget.inputTextArea->toPlainText().toLower() == "/r"
            && !gproxy->m_BNET->GetReplyTarget().empty())
    {
        widget.inputTextArea->setPlainText("/w "
                + QString::fromStdString(gproxy->m_BNET->GetReplyTarget()));
        widget.inputTextArea->moveCursor(QTextCursor::End);
    }
    
    return false;
}

/**
 * The slot which is emitted after the text of the titleLabel has changed.
 * Used to resize the text to fit into the label.
 */
void MainGUI::onTitleLabelTextChanged ()
{
    QString text = widget.titleLabel->text();
    QFont font = widget.titleLabel->font();
    font.setPointSize(12);
    QFontMetrics *fm = new QFontMetrics(font);
    int width = widget.titleLabel->width();

    while (fm->boundingRect(text).width() >= width)
    {
        font.setPointSize(font.pointSize() - 1);
        fm = new QFontMetrics(font);
    }

    widget.titleLabel->setFont(font);
}

/**
 * Slot to build the requested context menu for the channellist.
 *
 * @param pos The position of the mouse event.
 */
void MainGUI::onChannelContextMenu (const QPoint& pos)
{
    QPoint globalPos = widget.channelList->mapToGlobal(pos);
    QListWidgetItem* item = widget.channelList->itemAt(pos);

    if (!item)
    {
        return;
    }

    QString user = item->data(ChannellistDelegate::USER).toString();

    QMenu menu;

    if (gproxy->m_BNET->GetInGame())
    {
        if (user == "The Sentinel" || user == "The Scourge" || user.startsWith("Team "))
        {
            menu.addAction(QIcon(":/images/Play.png"), "Change team");
        }
        else if (item->data(ChannellistDelegate::SLOT_STATUS).toInt() == 2
                && item->data(ChannellistDelegate::SLOT_COMPUTER_STATUS).toInt() == 0)
        {
            menu.addAction(QIcon(":/images/Mail.png"), "Whisper");
            menu.addAction(QIcon(":/images/Add.png"), "Add as friend");
            menu.addAction(QIcon(":/images/Applications.png"), "Copy name");
            menu.addAction(QIcon(":/images/Colors.png"), "Change color");
            menu.addAction(QIcon(":/images/Edit.png"), "!stats");
            menu.addAction(QIcon(":/images/Edit Alt.png"), "!statsdota");
        }
    }
    else
    {
        menu.addAction(QIcon(":/images/Mail.png"), "Whisper");
        menu.addAction(QIcon(":/images/User.png"), "Whois");
        menu.addAction(QIcon(":/images/Applications.png"), "Copy name");
        menu.addAction(QIcon(":/images/Add.png"), "Add as friend");
        menu.addAction(QIcon(":/images/Colors.png"), "Change color");
    }

    QAction* action = menu.exec(globalPos);
    if (action)
    {
        if (action->text() == "Whisper")
        {
            widget.inputTextArea->setPlainText("/w " + user + " ");
            widget.inputTextArea->setFocus();
            widget.inputTextArea->moveCursor(QTextCursor::End);
        }
        else if (action->text() == "Whois")
        {
            gproxy->m_BNET->QueueChatCommand("/whois " + user, false);
        }
        else if (action->text() == "!stats" || action->text() == "!statsdota")
        {
            gproxy->sendGamemessage(action->text() + " " + user);
        }
        else if (action->text() == "Change team")
        {
            int team;

            if (user == "The Sentinel")
            {
                team = 0;
            }
            else if (user == "The Scourge")
            {
                team = 1;
            }
            else
            {
                team = user.mid(5).toInt();
            }

            gproxy->changeTeam(team);
        }
        else if (action->text() == "Copy name")
        {
            QApplication::clipboard()->setText(user);
        }
        else if (action->text() == "Add as friend")
        {
            gproxy->m_BNET->QueueChatCommand("/friends add " + user, false);
        }
        else if (action->text() == "Change color")
        {
            MColorDialog* colorDialog = new MColorDialog(gproxy->getConfig()->getUserColor(user), this);
            colorDialog->setObjectName(user + "ColorDialog");
            colorDialog->setProperty("listItem", VariantUtil<QListWidgetItem>::toQVariant(item));

            connect(colorDialog, SIGNAL(currentColorChanged(MColorDialog*)),
                    this, SLOT(userColorChanged(MColorDialog*)), Qt::QueuedConnection);

            if (colorDialog->exec() == QDialog::Accepted)
            {
                QColor color = colorDialog->selectedColor();
                if (color.isValid())
                {
                    gproxy->getConfig()->setUserColor(user, color);
                    gproxy->getConfig()->commit();
                    item->setData(ChannellistDelegate::COLOR_USER, color);
                }
            }
            else
            {
                colorDialog->setVisible(false);
                userColorChanged(colorDialog);
            }

            delete colorDialog;
        }
        else
        {
            addMessage(ColoredMessage("[ERROR] Not yet implemented!", ColoredMessage::ERROR), false);
        }
    }
}

/**
 * Slot to build the requested context menu for the friendlist.
 *
 * @param pos The position of the mouse event.
 */
void MainGUI::onFriendsContextMenu (const QPoint& pos)
{
    QPoint globalPos = widget.friendList->mapToGlobal(pos);
    QListWidgetItem* item = widget.friendList->itemAt(pos);

    if (!item)
    {
        return;
    }

    QMenu menu;
    menu.addAction(QIcon(":/images/Mail.png"), "Whisper");
    menu.addAction(QIcon(":/images/User.png"), "Whois");
    menu.addAction(QIcon(":/images/Chat Bubble.png"), "Whisper all");
    menu.addAction(QIcon(":/images/Applications.png"), "Copy name");
    menu.addAction(QIcon(":/images/Remove.png"), "Remove friend");

    if (gproxy->m_BNET->GetInGame())
    {
        menu.addAction(QIcon(":/images/Edit.png"), "!stats");
        menu.addAction(QIcon(":/images/Edit Alt.png"), "!statsdota");
    }

    QAction* action = menu.exec(globalPos);
    if (action)
    {
        if (action->text() == "Whisper")
        {
            widget.inputTextArea->setPlainText("/w " + item->text() + " ");
            widget.inputTextArea->setFocus();
            widget.inputTextArea->moveCursor(QTextCursor::End);
        }
        else if (action->text() == "Whois")
        {
            gproxy->m_BNET->QueueChatCommand("/whois " + item->text(), false);
        }
        else if (action->text() == "Whisper all")
        {
            widget.inputTextArea->setPlainText("/f m ");
            widget.inputTextArea->setFocus();
            widget.inputTextArea->moveCursor(QTextCursor::End);
        }
        else if (action->text() == "Copy name")
        {
            QApplication::clipboard()->setText(item->text());
        }
        else if (action->text() == "!stats" || action->text() == "!statsdota")
        {
            gproxy->sendGamemessage(action->text() + " " + item->text());
        }
        else if (action->text() == "Remove friend")
        {
            gproxy->m_BNET->QueueChatCommand("/friends remove "
                    + item->text(), false);
        }
        else
        {
            addMessage(ColoredMessage("[ERROR] Not yet implemented!", ColoredMessage::ERROR), false);
        }
    }
}

/**
 * The slot which is emitted a click event on the gamelist occured.
 *
 * @param mouseEvent The MouseEvent*.
 */
void MainGUI::onGameListItemClicked (QMouseEvent *mouseEvent)
{
    widget.inputTextArea->setFocus();

    QListWidgetItem *item = widget.gameList->itemAt(mouseEvent->pos());
    if (!item)
    {
        return;
    }

    QString gamename = item->data(GamelistDelegate::GAMENAME).toString();

    if (mouseEvent->button() == Qt::LeftButton && !gamename.isEmpty()
            && gamename != "-" && gamename != "in channel ghostgraz")
    {
        gproxy->m_BNET->SetSearchGameName(gamename.toStdString());
        autosearch(false);
        addMessage(ColoredMessage("[BNET] looking for a game named \"" + gamename
                + "\" for up to two minutes", ColoredMessage::BNET));
    }
    else if (mouseEvent->button() == Qt::RightButton)
    {
        QApplication::clipboard()->setText(gamename);
    }
}

/**
 * Slot which is emitted when the refresh button was clicked.
 */
void MainGUI::onRefreshButtonClicked ()
{
    widget.refreshButton->setEnabled(false);

    gameListDownloader->startDownload();

    widget.refreshButton->setText("Refresh (10)");
    QTimer::singleShot(1000, this, SLOT(updateRefreshButton()));
}

/**
 * Updates the text of the refresh button and enables it if the wait time has elapsed.
 */
void MainGUI::updateRefreshButton ()
{
    QString text = widget.refreshButton->text();
    int secondsLeft = text.mid(9, text.length() - 10).toInt();

    if (secondsLeft != 1)
    {
        widget.refreshButton->setText("Refresh ("
                + QString::number(secondsLeft - 1) + ")");
        QTimer::singleShot(1000, this, SLOT(updateRefreshButton()));
    }
    else
    {
        widget.refreshButton->setText("Refresh");
        widget.refreshButton->setEnabled(true);
    }
}

/**
 * Processed the input of the inputTextArea. E.g: Execute commands or send text to Battle.net.
 *
 * @param input The user input text.
 */
void MainGUI::processInput (const QString& input)
{
    // Variable holding the input text in lower case for command comparison.
    QString command = input.toLower();

    if (command == "/commands")
    {
        addMessage(ColoredMessage(">>> /commands"), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  In the GProxy++ console:"), false);
        addMessage(ColoredMessage("   /commands           : show command list"), false);
        addMessage(ColoredMessage("   /exit or /quit      : close GProxy++"), false);
        addMessage(ColoredMessage("   /filter <f>         : start filtering public game names for <f>"), false);
        addMessage(ColoredMessage("   /filteroff          : stop filtering public game names"), false);
        addMessage(ColoredMessage("   /game <gamename>    : look for a specific game named <gamename>"), false);
        addMessage(ColoredMessage("   /help               : show help text"), false);
        addMessage(ColoredMessage("   /public             : enable listing of public games"), false);
        addMessage(ColoredMessage("   /publicoff          : disable listing of public games"), false);
        addMessage(ColoredMessage("   /r <message>        : reply to the last received whisper"), false);
        addMessage(ColoredMessage("   /pgn <privgamename> : sets privategamename temporary to <privgamename> (Alias: /privategamename)"), false);
        addMessage(ColoredMessage("   /waitgame <option>  : waits for a bot joining channel with the name-prefix set in gproxy.cfg and hosts"), false);
        addMessage(ColoredMessage("                         a private game named <value of privategamename> (Alias: /wg <option>"), false);
        addMessage(ColoredMessage("   Optons:   quiet     : doesn't display the message to other users in the channel (Alias: q)"), false);
        addMessage(ColoredMessage("             off       : stops waiting for a bot to host a game"), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("   /botprefix <prefix> : sets <value of botprefix> temporary to <prefix>"), false);
        addMessage(ColoredMessage("   /parrot <plname>    : repeats anything that Player <plname> sais in Chat with [PARROT] <Player's message>"), false);
        addMessage(ColoredMessage("   /parrotall          : repeats anything that Players say in Chat with [PARROT] <Player's message>"), false);
        addMessage(ColoredMessage("   /parrotoff          : stops /parrot and /parrotall"), false);
        addMessage(ColoredMessage("   /start              : start warcraft 3"), false);
        addMessage(ColoredMessage("   /version            : show version text"), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  In game:"), false);
        addMessage(ColoredMessage("   /re <message>       : reply to the last received whisper"), false);
        addMessage(ColoredMessage("   /sc                 : whispers \"spoofcheck\" to the game host"), false);
        addMessage(ColoredMessage("   /status             : show status information"), false);
        addMessage(ColoredMessage("   /w <user> <message> : whispers <message> to <user>"), false);
        addMessage(ColoredMessage(""), false);
    }
    else if (command == "/exit" || command == "/quit")
    {
        close();
        return;
    }
    else if (command == "/filteroff" || command == "/filter off")
    {
        gproxy->m_BNET->SetPublicGameFilter(string());
        addMessage(ColoredMessage("[BNET] stopped filtering public game names", ColoredMessage::BNET));
    }
    else if (command.length() >= 9 && command.startsWith("/filter "))
    {
        QString filter = input.mid(8);

        if (!filter.isEmpty())
        {
            filter = filter.left(31);
            gproxy->m_BNET->SetPublicGameFilter(filter.toStdString());
            addMessage(ColoredMessage("[BNET] started filtering public game names for \"" + filter + "\""), ColoredMessage::BNET);
        }
    }
    else if (command.startsWith("/priv ") && command.length() >= 7
            && !gproxy->getPrivategamename().isEmpty())
    {
        QString botname = input.mid(6);
        if (!botname.isEmpty() && botname.length() <= 15)
        {
            QString pgn = gproxy->getPrivategamename();
            if (botname.length() > 2)
            {
                gproxy->m_BNET->QueueChatCommand("/w " + botname + " !priv " + pgn);
            }
            else
            {
                gproxy->m_BNET->QueueChatCommand("/w " + gproxy->getBotprefix()
                        + botname + " !priv " + pgn);
            }
            gproxy->m_BNET->SetSearchGameName(pgn.toStdString());
            autosearch(true);
            addMessage(ColoredMessage("[BNET] try to create a private game named ["
                    + pgn + "] at Bot ["
                    + botname + "]."), ColoredMessage::BNET);
        }
    }
    else if (command.startsWith("/autosearch") || command.startsWith("/as"))
    {
        if (command == "/autosearch" || command == "/as")
        {
            if (gproxy->autosearch)
            {
                addMessage(ColoredMessage("[GPROXY] 'autosearch' is on.", ColoredMessage::GPROXY));
            }
            else
            {
                addMessage(ColoredMessage("[GPROXY] 'autosearch' is off.", ColoredMessage::GPROXY));
            }
        }
        else if (command == "/autosearch on" || command == "/as on")
        {
            autosearch(true);
            addMessage(ColoredMessage("[GPROXY] Autosearch enabled.", ColoredMessage::GPROXY));
        }
        else if (command == "/autosearch off" || command == "/as off")
        {
            autosearch(false);
            addMessage(ColoredMessage("[GPROXY] Autosearch disabled.", ColoredMessage::GPROXY));
        }
        else
        {
            addMessage(ColoredMessage("[ERROR] Invalid parameter! Use \"/as\", \"/as on\" or \"/as off\"", ColoredMessage::ERROR));
        }
    }
    else if (command == "/privategamename" || command == "/pgn")
    {
        addMessage(ColoredMessage("[GPROXY] The value of 'privategamename' is: ["
                + gproxy->getPrivategamename() + "]. Change it with the input "
                + "'/privategamename <value>' alias '/pgn <value>'.", ColoredMessage::GPROXY));
    }
    else if (command.startsWith("/privategamename ") || command.startsWith("/pgn "))
    {
        if (command.startsWith("/privategamename "))
        {
            gproxy->setPrivategamename(input.mid(17, (input.length() - 17)));
        }
        else
        {
            gproxy->setPrivategamename(input.mid(5, (input.size() - 5)));
        }

        addMessage(ColoredMessage("[GPROXY] Change value of 'privategamename' to ["
                + gproxy->getPrivategamename() + "].", ColoredMessage::GPROXY));

    }
    else if (command == "/botprefix")
    {
        addMessage(ColoredMessage("[GPROXY] Value of 'botprefix' is ["
                + gproxy->getBotprefix()
                + "]. You can change it with '/botprefix <value>'.", ColoredMessage::GPROXY));
    }
    else if (command.startsWith("/botprefix "))
    {
        gproxy->setBotprefix(input.mid(11, input.size() - 11));
        addMessage(ColoredMessage("[GPROXY] Change value of 'botprefix' to ["
                + gproxy->getBotprefix() + "].", ColoredMessage::GPROXY));
    }
    else if (command.startsWith("/wg"))
    {
        if (command == "/wg")
        {
            gproxy->setVShallCreateQuiet(false);
            gproxy->setVShallCreate(true);
            addMessage(ColoredMessage("[GPROXY] Waiting for a bot and then trying to create a game "
                    "and say [gn: " + gproxy->getPrivategamename() + "].", ColoredMessage::GPROXY));
            gproxy->m_BNET->QueueChatCommand("[GProxy++] Waiting for a bot "
                    "and create a game with the name ["
                    + gproxy->getPrivategamename() + "].");
        }
        else if (command == "/wg off")
        {
            addMessage(ColoredMessage("[GPROXY] stopped waiting for a bot to create a game.", ColoredMessage::GPROXY));
            gproxy->setVShallCreate(false);
        }
        else if (command == "/wg quiet" || command == "/wg q")
        {
            gproxy->setVShallCreate(true);
            gproxy->setVShallCreateQuiet(true);
            addMessage(ColoredMessage("[GPROXY] waiting for a bot and then trying to create a game [quiet].", ColoredMessage::GPROXY));
        }
    }
    else if (command.startsWith("/waitgame") && gproxy->getBotprefix() != "")
    {
        if (command == "/waitgame")
        {
            gproxy->setVShallCreateQuiet(false);
            gproxy->setVShallCreate(true);
            addMessage(ColoredMessage("[GPROXY] waiting for a bot and then trying to create a game "
                    "and say [gn: " + gproxy->getPrivategamename() + "].", ColoredMessage::GPROXY));
            gproxy->m_BNET->QueueChatCommand("[GProxy++][Phyton] Waiting for a bot "
                    "and create a game with the name ["
                    + gproxy->getPrivategamename() + "].");
        }
        else if (command == "/waitgame off")
        {
            addMessage(ColoredMessage("[GPROXY] stopped waiting for a bot to create a game.", ColoredMessage::GPROXY));
            gproxy->setVShallCreate(false);
        }
        else if (command == "/waitgame quiet")
        {
            gproxy->setVShallCreate(true);
            gproxy->setVShallCreateQuiet(true);
            addMessage(ColoredMessage("[GPROXY] waiting for a bot and then trying to create a game [quiet].", ColoredMessage::GPROXY));
        }
    }
    else if (command.startsWith("/parrot "))
    {
        gproxy->setParrot(input.mid(8, input.length() - 8));
        addMessage(ColoredMessage("[GPROXY] Parrot [" + gproxy->getParrot() + "]!!!111", ColoredMessage::GPROXY));
    }
    else if (command == "/parrotoff")
    {
        gproxy->setParrot("");
        addMessage(ColoredMessage("[GPROXY] Parrot off", ColoredMessage::GPROXY));
    }
    else if (input == "/parrotall")
    {
        gproxy->setParrot("Parrot parrot");
    }
    else if (input == "/parrotignore")
    {
        gproxy->setParrot("Ignore ignore");
    }
    else if (command.length() >= 7 && command.startsWith("/game "))
    {
        string GameName = input.mid(6).toStdString();

        if (!GameName.empty() && GameName.size() <= 31)
        {
            gproxy->m_BNET->SetSearchGameName(GameName);
            autosearch(false);
            addMessage(ColoredMessage("[BNET] looking for a game named \"" + QString::fromStdString(GameName)
                    + "\" for up to two minutes", ColoredMessage::BNET));
        }
    }
    else if (command == "/help")
    {
        addMessage(ColoredMessage(">>> /help"), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  GProxy++ connects to battle.net and looks for games for you to join."), false);
        addMessage(ColoredMessage("  If GProxy++ finds any they will be listed on the Warcraft III LAN screen."), false);
        addMessage(ColoredMessage("  To join a game, simply open Warcraft III and wait at the LAN screen."), false);
        addMessage(ColoredMessage("  Standard games will be white and GProxy++ enabled games will be blue."), false);
        addMessage(ColoredMessage("  Note: You must type \"/public\" to enable listing of public games."), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  If you want to join a specific game, type \"/game <gamename>\"."), false);
        addMessage(ColoredMessage("  GProxy++ will look for that game for up to two minutes before giving up."), false);
        addMessage(ColoredMessage("  This is useful for private games."), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  Please note:"), false);
        addMessage(ColoredMessage("  GProxy++ will join the game using your battle.net name, not your LAN name."), false);
        addMessage(ColoredMessage("  Other players will see your battle.net name even if you choose another name."), false);
        addMessage(ColoredMessage("  This is done so that you can be automatically spoof checked."), false);
        addMessage(ColoredMessage(""), false);
        addMessage(ColoredMessage("  Type \"/commands\" for a full command list."), false);
        addMessage(ColoredMessage(""), false);
    }
    else if (command == "/public" || command == "/publicon" || command == "/public on"
            || command == "/list" || command == "/liston" || command == "/list on")
    {
        gproxy->m_BNET->SetListPublicGames(true);
        addMessage(ColoredMessage("[BNET] listing of public games enabled", ColoredMessage::BNET));
    }
    else if (command == "/publicoff" || command == "/public off"
            || command == "/listoff" || command == "/list off")
    {
        gproxy->m_BNET->SetListPublicGames(false);
        addMessage(ColoredMessage("[BNET] listing of public games disabled", ColoredMessage::BNET));
    }
    else if (command.length() >= 4 && command.startsWith("/r "))
    {
        if (!gproxy->m_BNET->GetReplyTarget().empty())
        {
            gproxy->m_BNET->QueueChatCommand(input.mid(3),
                    gproxy->m_BNET->GetReplyTarget(), true);
        }
        else
            addMessage(ColoredMessage("[BNET] nobody has whispered you yet", ColoredMessage::BNET));
    }
    else if (command == "/start")
    {
        startWarcraft();
    }
    else if (command == "/version")
    {
        addMessage(ColoredMessage("[GPROXY] Customized "
                "GProxy++ Version " + QString::fromStdString(gproxy->m_Version), ColoredMessage::GPROXY), false);
        addMessage(ColoredMessage("[GPROXY] ", ColoredMessage::GPROXY), false, true, false);
        addMessage(ColoredMessage("This mod is by "), false, false, false);
        addMessage(ColoredMessage("Phyton", ColoredMessage::WHISPER), false, false, false);
        addMessage(ColoredMessage(", "), false, false, false);
        addMessage(ColoredMessage("Pr0gm4n", ColoredMessage::INFO), false, false, false);
        addMessage(ColoredMessage(", "), false, false, false);
        addMessage(ColoredMessage("Noman(1)", ColoredMessage::ERROR), false, false, false);
        addMessage(ColoredMessage(" and "), false, false, false);
        addMessage(ColoredMessage("Manufactoring", ColoredMessage::GAMEINFO), false, false, false);
        addMessage(ColoredMessage("."), false, false, true);
    }
    else if (command.startsWith("/test"))
    {
        // Isn't it obvious?
    }
    else if (command.startsWith("/p ") || command.startsWith("/phrase "))
    {
        QString filePath = "phrase/";

        if (command.startsWith("/p "))
        {
            filePath.append(command.mid(3));
        }
        else if (command.startsWith("/phrase "))
        {
            filePath.append(command.mid(8));
        }

        if (!filePath.endsWith(".txt"))
        {
            filePath.append(".txt");
        }

        QFile* phraseFile = new QFile(filePath);

        if (phraseFile->open(QFile::ReadOnly))
        {
            QStringList lines = QString(phraseFile->readAll()).split('\n');

            foreach(QString line, lines)
            {
                if (gproxy->m_BNET->GetInGame() && !line.startsWith('#'))
                {
                    if (!line.startsWith('%'))
                    {
                        if (!line.startsWith('~'))
                        {
                            gproxy->sendGamemessage(line);
                        }
                        else
                        {
                            // Sleep
                        }
                    }
                    else
                    {
                        gproxy->m_BNET->QueueChatCommand(line.mid(1));
                    }
                }
                else if (!line.startsWith('#'))
                {
                    gproxy->m_BNET->QueueChatCommand(line);
                }
            }
        }
        else
        {
            addMessage(ColoredMessage("[ERROR] File \"" + filePath + "\" does not exist!", ColoredMessage::ERROR));
            gproxy->SendLocalChat("File \"" + filePath + "\" does not exist!");
        }
    }
    else if (command == "/leave")
    {
        if (gproxy->m_BNET->GetInGame())
        {
            if (gproxy->m_GameStarted)
            {
                addMessage(ColoredMessage("[ERROR] Leaving a running game is disabled on purpose.", 
                        ColoredMessage::ERROR));
            }
            else
            {
                gproxy->leaveLobby();
            }
        }
        else
        {
            addMessage(ColoredMessage("[ERROR] No game to leave found.", ColoredMessage::ERROR));
        }
    }
    else if (gproxy->m_BNET->GetInGame())
    {
        if (command == "/statslast" || command == "/sl"
                || command == "!statslast" || command == "!sl")
        {
            gproxy->sendGamemessage("!stats " + gproxy->getLastLeaver()->getName().getMessage());
        }
        else if (command.startsWith("/allies "))
        {
            gproxy->sendGamemessage(input.mid(8), true);
        }
        else
        {
            gproxy->sendGamemessage(input);
        }
    }
    else
    {
        gproxy->m_BNET->QueueChatCommand(input);
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
void MainGUI::addMessage (const ColoredMessage& coloredMessage, bool log, bool printTimestamp, bool lineBreak)
{
    ColoredMessage cm = ColoredMessage(coloredMessage);

    if (log)
    {
        LOG_Print(cm.getMessage(), printTimestamp, lineBreak);
    }

    if (printTimestamp)
    {
        QString dateTimeString = QLocale().toString(QDateTime::currentDateTime(), "[hh:mm:ss] ");
        cm.setMessage(cm.getMessage().prepend(dateTimeString));
    }

    if (lineBreak)
    {
        cm.setMessage(cm.getMessage().append('\n'));
    }

    widget.outputTextArea->appendColoredMessage(cm);
    widget.inputTextArea->setFocus();
}

/**
 * Adds color data to the QListWidgetItem.
 *
 * @param item QListWidgetItem*.
 */
void MainGUI::addColor (QListWidgetItem *item)
{
    QString username = item->data(ChannellistDelegate::USER).toString();
    QColor color = gproxy->getConfig()->getUserColor(username);

    item->setData(ChannellistDelegate::COLOR_USER, color);
}

/**
 * Adds tooltip information to the QListWidgetItem.
 *
 * @param item QListWidgetItem*.
 */
void MainGUI::addTooltip(QListWidgetItem* item)
{
    QString username = item->data(ChannellistDelegate::USER).toString();

    if (username == "baerli_graz" || username == "klingone_graz")
    {
        item->setToolTip("Creator of GhostGraz");
    }
    else if (username == "Phyton" || username == "Manufactoring")
    {
        item->setToolTip("Creator of GProxy GhostGraz");
    }
    else if (username == "Noman(1)")
    {
        item->setToolTip("Little hepler of Gproxy GhostGraz");
    }
    else if (username.startsWith("GhostGraz"))
    {
        item->setToolTip("GhostGraz bot");
    }
    else if (isAdmin(username))
    {
        item->setToolTip("GhostGraz admin");
    }
}

/**
 * Adds a user to the channellist.
 *
 * @param username The name of the user.
 * @param clanTag The clantag of the user.
 */
void MainGUI::addChannelUser (QString username, QString clanTag)
{
    for (int i = 0; i < widget.channelList->count(); i++)
    {
        QListWidgetItem *item = widget.channelList->item(i);
        if (item->data(ChannellistDelegate::USER).toString() == username)
        {
            if (item->data(ChannellistDelegate::CLAN_TAG).toString() == clanTag)
            {
                return;
            }
            else
            {
                // Update
                QListWidgetItem *newItem = new QListWidgetItem();
                newItem->setData(ChannellistDelegate::USER, username);
                newItem->setData(ChannellistDelegate::CLAN_TAG, clanTag);
                addColor(newItem);
                addTooltip(newItem);
                delete widget.channelList->takeItem(i);
                widget.channelList->addItem(newItem);
                return;
            }
        }
    }

    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setData(ChannellistDelegate::USER, username);
    if (!clanTag.isEmpty())
    {

        newItem->setData(ChannellistDelegate::CLAN_TAG, clanTag);
    }
    addColor(newItem);
    addTooltip(newItem);
    widget.channelList->addItem(newItem);
}

/**
 * Removes a user with the given username from the channellist.
 *
 * @param username The name of the user.
 */
void MainGUI::removeChannelUser (QString username)
{
    for (int i = 0; i < widget.channelList->count(); i++)
    {
        QListWidgetItem *item = widget.channelList->item(i);
        if (item->data(ChannellistDelegate::USER).toString() == username)
        {
            item = widget.channelList->takeItem(i);
            delete item;

            return;
        }
    }
}

/**
 * Slot which is emitted if the channel has been changed.
 *
 * @param channel The new channel name.
 */
void MainGUI::changeChannel (QString channel)
{
    widget.channelList->clear();
    widget.titleLabel->setText(channel);
}

/**
 * Slot which is emiited when the friendlist gets updated.
 *
 * @param friends The list of friends.
 */
void MainGUI::updateFriendlist(QList<Friend*> friends)
{
    for (int i = 0; i < widget.friendList->count(); ++i)
    {
        delete VariantUtil<Friend>::toPointer(widget.friendList->item(i)->data(ROLE_FRIEND));
    }
    widget.friendList->clear();

    foreach(Friend* f, friends)
    {
        addFriend(f);
    }
}

/**
 * Slot which is emiited when a new friend information was received.
 *
 * @param f The friend.
 */
void MainGUI::updateFriend(Friend* f)
{
    for (int i = 0; i < widget.friendList->count(); ++i)
    {
        Friend* storedFriend = VariantUtil<Friend>::toPointer(widget.friendList->item(i)->data(ROLE_FRIEND));
        if (storedFriend->getEntryNumber() == f->getEntryNumber())
        {
            storedFriend->setLocation(f->getLocation());
            storedFriend->setStatus(f->getStatus());
            storedFriend->setProduct(f->getProduct());
            storedFriend->setLocationName(f->getLocationName());

            // Readd the friend to display the updates.
            delete widget.friendList->takeItem(i);
            addFriend(storedFriend);
            break;
        }
    }

    delete f;
}

void MainGUI::addFriend(Friend* f)
{
    if (f->getEntryNumber() == 255)
    {
        // A new friend was added to the list.
        f->setEntryNumber(widget.friendList->count());
        gproxy->m_BNET->updateFriend(f->getEntryNumber());
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setData(ROLE_FRIEND, VariantUtil<Friend>::toQVariant(f));
    item->setText(f->getName());

    switch (f->getLocation())
    {
        case Friend::IN_CHANNEL:
            if (f->getLocationName().isNull() || f->getLocationName().isEmpty())
            {
                item->setToolTip("In channel.");
            }
            else
            {
                item->setToolTip("In channel: " + f->getLocationName());
            }
            break;
        case Friend::IN_PUBLIC_GAME:
            item->setToolTip("In public game: " + f->getLocationName());
            break;
        case Friend::IN_UNKNOWN_PRIVATE_GAME:
            item->setToolTip("In private game.");
            break;
        case Friend::IN_PRIVATE_GAME:
            item->setToolTip("In private game: " + f->getLocationName());
            break;
        default:
            // No location details available. Do nothing.
            break;
    }

    if (f->getStatus() != Friend::OFFLINE)
    {
        item->setForeground(Qt::green);
        widget.friendList->insertItem(0, item);
    }
    else
    {
        item->setForeground(Qt::red);
        widget.friendList->addItem(item);
    }
}

void MainGUI::removeFriend(unsigned char entryNumber)
{
    for (int i = 0; i < widget.friendList->count(); ++i)
    {
        Friend* storedFriend = VariantUtil<Friend>::toPointer(widget.friendList->item(i)->data(ROLE_FRIEND));
        if (storedFriend->getEntryNumber() == entryNumber)
        {
            delete storedFriend;
            delete widget.friendList->takeItem(i);
            // Do not break this loop!
        }
        else if (storedFriend->getEntryNumber() > entryNumber)
        {
            // Move friends with a higher entry number up. (Decrease their entry numbers)
            storedFriend->setEntryNumber(storedFriend->getEntryNumber() - 1);
        }
    }
}

void MainGUI::addFriendInformation(Friend* f, QListWidgetItem* item)
{

}

/**
 * Sets the new slotlist of the game.
 *
 * @param slotList The new slotlist.
 */
void MainGUI::setGameslots (QList<Slot*> slotList)
{
    widget.channelList->clear();

    int currentTeam = -1;
    foreach (Slot* slot, slotList)
    {
        if (currentTeam != slot->getTeam())
        {
            currentTeam = slot->getTeam();

            QListWidgetItem* teamItem = new QListWidgetItem();

            if (gproxy->isDotaMap())
            {
                if (currentTeam == 0)
                {
                    teamItem->setData(ChannellistDelegate::USER, "The Sentinel");
                    teamItem->setData(ChannellistDelegate::SLOT_TEAM, 0);
                    teamItem->setData(ChannellistDelegate::COLOR_USER, QColor(255, 0, 0));
                }
                else
                {
                    teamItem->setData(ChannellistDelegate::USER, "The Scourge");
                    teamItem->setData(ChannellistDelegate::SLOT_TEAM, 1);
                    teamItem->setData(ChannellistDelegate::COLOR_USER, QColor(0, 255, 0));
                }
            }
            else
            {
                teamItem->setData(ChannellistDelegate::USER, "Team " + QString::number(currentTeam + 1));
                teamItem->setData(ChannellistDelegate::SLOT_TEAM, currentTeam);
            }

            widget.channelList->addItem(teamItem);
        }

        QListWidgetItem* playerItem = new QListWidgetItem();
        playerItem->setData(ChannellistDelegate::SLOT_PID, slot->getPlayerId());
        playerItem->setData(ChannellistDelegate::SLOT_DOWNLOAD_STATUS, slot->getDownloadProgress());
        playerItem->setData(ChannellistDelegate::SLOT_STATUS, slot->getSlotStatus());
        playerItem->setData(ChannellistDelegate::SLOT_COMPUTER_STATUS, slot->getComputerStatus());
        playerItem->setData(ChannellistDelegate::SLOT_TEAM, slot->getTeam());
        playerItem->setData(ChannellistDelegate::SLOT_COLOR, slot->getColor());
        playerItem->setData(ChannellistDelegate::SLOT_RACE, slot->getRace());
        playerItem->setData(ChannellistDelegate::SLOT_COMPUTER_TYPE, slot->getComputerType());
        playerItem->setData(ChannellistDelegate::SLOT_HANDICAP, slot->getHandicap());

        if (slot->getPlayer())
        {
            playerItem->setData(ChannellistDelegate::USER, slot->getPlayer()->getName().getMessage());
            playerItem->setData(ChannellistDelegate::COLOR_USER, slot->getPlayer()->getName().getColor());
        }
        else
        {
            playerItem->setData(ChannellistDelegate::USER, "");
        }
        widget.channelList->addItem(playerItem);
    }
}
#ifdef WIN32
/**
 * Starts Warcraft 3 Reign of Chaos or The Frozen Throne (depending on the configuration) as detached process.
 */
void MainGUI::startWarcraft ()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof( pi ) );
    QString exePath = gproxy->getWar3Path();
    std::string exePathStr = exePath.toUtf8().constData();
    std::cout << exePathStr;
    string War3EXE;
    
    
    War3EXE = exePathStr + "Warcraft III.exe";
    BOOL hProcess = CreateProcessA( War3EXE.c_str( ), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, exePathStr.c_str( ), LPSTARTUPINFOA( &si ), &pi );
    
    if( !hProcess )
    {
        War3EXE = exePathStr + "war3.exe";
        BOOL hProcess = CreateProcessA( War3EXE.c_str( ), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, exePathStr.c_str( ), LPSTARTUPINFOA( &si ), &pi );
    }  else if( !hProcess )
    {
        War3EXE = exePathStr + "warcraft.exe";
        BOOL hProcess = CreateProcessA( War3EXE.c_str( ), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, exePathStr.c_str( ), LPSTARTUPINFOA( &si ), &pi );
    }  else
    {
        CONSOLE_Print( QString::fromStdString( "[GPROXY] started Warcraft 3" ) );
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }
}
#endif
/**
 * Displays a errormessage dialog.
 *
 * @param errorMessage The error as QString.
 */
void MainGUI::showErrorMessage (QString errorMessage)
{
    addMessage(ColoredMessage("[ERROR] " + errorMessage, ColoredMessage::ERROR));

    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/images/Error.png"));
    msgBox.setWindowTitle("Error");
    msgBox.setText(errorMessage);
    msgBox.exec();
}

/**
 * Displays the Options dialog.
 * If exitOnReject is true GProxy will be restarted or closed when the dialog returns.
 *
 * @param exitOnReject Determines if GProxy should restarted/closed after the dialog returns.
 */
void MainGUI::showConfigDialog (bool exitOnReject)
{
    ConfigGUI* configGUI = new ConfigGUI(this);
    int exitCode = configGUI->exec();
    if (exitOnReject)
    {
        if (exitCode == QDialog::Accepted)
        {
            // Restart GProxy.
            showErrorMessage("Restarting GProxy...");
            onRestart();
        }
        else
        {
            showErrorMessage("GProxy cannot run without configuration.\nGood luck next time!");
            QApplication::quit();
        }
    }

    delete configGUI;
}

/**
 * Slot which is emitted when a player joins the game.
 * Will play a sound if a VIP joins. A player is considered VIP if he is admin or on the friendlist.
 * Also requests additional player information from the statspage.
 *
 * @param playername The name of player.
 */
void MainGUI::playerJoined (const ColoredMessage& playername)
{
    addMessage(ColoredMessage("[GAMEINFO] ", ColoredMessage::GAMEINFO), true, true, false);
    addMessage(ColoredMessage(playername), true, false, false);
    addMessage(ColoredMessage(" has joined the game.", ColoredMessage::GAMEINFO), true, false, true);

    // If the player has a colored name (hostbot), return.
    if (playername.getColorType() == ColoredMessage::CUSTOM)
    {
        return;
    }

    statspage->getPlayerInformation(playername.getMessage());

    if (gproxy->getUsername() != playername.getMessage() && isAdmin(playername.getMessage()))
    {
        QSound::play("sounds/vip_joins.wav");
    }
    else
    {
        // If the player is a friend -> play sound.
        for (int i = 0; i < widget.friendList->count(); i++)
        {
            if (widget.friendList->item(i)->text() == playername.getMessage())
            {
                QSound::play("sounds/vip_joins.wav");
                break;
            }
        }
    }
}

/**
 * Slot which is emitted when the channellist was clicked.
 *
 * @param mouseEvent QMouseEvent* holding additional information.
 */
void MainGUI::onChannellistItemClicked (QMouseEvent *mouseEvent)
{
    widget.inputTextArea->setFocus();

    QListWidgetItem* item = widget.channelList->itemAt(mouseEvent->pos());
    if (!item)
    {
        return;
    }

    if (mouseEvent->button() == Qt::LeftButton)
    {
        QString user = item->data(ChannellistDelegate::USER).toString();
        int slotStatus = item->data(ChannellistDelegate::SLOT_STATUS).toInt();
        int computerStatus = item->data(ChannellistDelegate::SLOT_COMPUTER_STATUS).toInt();
        int team = item->data(ChannellistDelegate::SLOT_TEAM).toInt();

        if (gproxy->m_BNET->GetInGame())
        {
            if (user == "The Sentinel" || user == "The Scourge"
                    || user.startsWith("Team "))
            {
                gproxy->changeTeam(team);
            }
            else if (slotStatus == 0)
            {
                if (gproxy->isDotaMap())
                {
                    return; // Still buggy.

                    int requestedSlotNumber = 1;
                    for (int i = 0; i < widget.channelList->count(); i++)
                    {
                        if (item == widget.channelList->item(i))
                        {
                            requestedSlotNumber = i;
                            break;
                        }
                    }

                    int currentSlotNumber = 1;
                    foreach (Slot* slot, gproxy->getSlotList())
                    {
                        if (slot->getPlayer()) // Check for existing player.
                        {
                            if (slot->getPlayer()->getPlayerId() == gproxy->m_ChatPID)
                            {
                                currentSlotNumber = slot->getColor();
                                break;
                            }
                        }
                    }

                    if ((currentSlotNumber < 6 && requestedSlotNumber < 6)
                            || (currentSlotNumber > 6 && requestedSlotNumber > 6))
                    {
                        for (int i = currentSlotNumber + 1; i != requestedSlotNumber +1; i++)
                        {
                            if (team == 0 && i > 5)
                            {
                                i = 1;
                            }
                            else if (i > 11)
                            {
                                i = 7;
                            }

                            if (!gproxy->getSlotList().at(i)->getPlayer())
                            {
                                gproxy->changeTeam(team);
                            }
                        }
                    }
                    else
                    {
                        int i;
                        if (team == 0)
                        {
                            i = 1;
                        }
                        else
                        {
                            i = 7;
                        }

                        while (i != requestedSlotNumber)
                        {
                            if (!gproxy->getSlotList().at(i)->getPlayer())
                            {
                                gproxy->changeTeam(team);
                            }

                            i++;
                        }

                        /**
                         * SENT
                         * 1
                         * 2
                         * 3
                         * 4
                         * 5
                         * SCOU
                         * 7
                         * 8
                         * 9
                         * 10
                         * 11
                         */
                    }
                }
            }
            else if (slotStatus == 2 && computerStatus == 0)
            {
                widget.inputTextArea->setPlainText("/w " + user + " ");
                widget.inputTextArea->moveCursor(QTextCursor::End);
            }
        }
        else
        {
            widget.inputTextArea->setPlainText("/w " + user + " ");
            widget.inputTextArea->moveCursor(QTextCursor::End);
        }
    }
}

/**
 * Slot which is emitted when the friendlist was clicked.
 *
 * @param mouseEvent QMouseEvent* holding additional information.
 */
void MainGUI::onFriendlistItemClicked (QMouseEvent *mouseEvent)
{
    widget.inputTextArea->setFocus();

    QListWidgetItem *item = widget.friendList->itemAt(mouseEvent->pos());
    if (!item)
    {
        return;
    }

    if (mouseEvent->button() == Qt::LeftButton)
    {
        widget.inputTextArea->setPlainText("/w " + item->text() + " ");
        widget.inputTextArea->moveCursor(QTextCursor::End);
    }
}

/**
 * Slot which is emitted when the statspage login was finished.
 */
void MainGUI::statspageLoginFinished ()
{
    if (statspage->isLoggedIn())
    {
        addMessage(ColoredMessage("[GPROXY] Statspage available.", ColoredMessage::GPROXY));
        initAdminlist();
    }
    else
    {
        addMessage(ColoredMessage("[ERROR] Statspage unavailable. "
                "You might have entered wrong username or password.", ColoredMessage::ERROR));
    }
}

/**
 * Slot which is emitted when the statspage finished parsing the playerinformation.
 *
 * @param player The player with every available information from the statspage.
 */
void MainGUI::receivedPlayerInformation (Player *player)
{
    QList<Player*> players = gproxy->getPlayers();
    for (int i = 0; i < players.count(); i++)
    {
        if (players.at(i)->getName() == player->getName())
        {
            player->setPlayerId(players.at(i)->getPlayerId());
            players[i] = player;

            if (players[i]->getGamesPlayed() == 0)
            {
                addMessage(ColoredMessage("[WARNING] ", ColoredMessage::WARNING), true, true, false);
                addMessage(players[i]->getName(), true, false, false);
                addMessage(ColoredMessage(" hasn't played any games on GhostGraz yet.",
                        ColoredMessage::WARNING), true, false, true);
                gproxy->SendLocalChat("[WARNING] " + players[i]->getName().getMessage()
                        + " hasn't played any games with the bot yet.");
            }
            else if (players[i]->getStayPercent() < 80)
            {
                addMessage(ColoredMessage("[WARNING] ", ColoredMessage::WARNING), true, true, false);
                addMessage(players[i]->getName(), true, false, false);
                addMessage(ColoredMessage(" has a stay ratio below 80%. ("
                        + QString::number(players[i]->getStayPercent(), 'f', 2) + "%)",
                        ColoredMessage::WARNING), true, false, true);
                gproxy->SendLocalChat("WARNING: " + players[i]->getName().getMessage()
                        + " has a stay ratio below 80%. ("
                        + QString::number(players[i]->getStayPercent(), 'f', 2) + "%)");
            }
            if(gproxy->getConfig()->getBoolean("joinstats")){
                addMessage(ColoredMessage("[Statspage] ", ColoredMessage::GPROXY),true, true, false);
                addMessage(players[i]->getName(), true, false, false);
                addMessage(ColoredMessage(" has stats of: " +  QString::number(players[i]->getGamesPlayed()) + " g with " +  QString::number(players[i]->getWinPercent(),'f',2) + "% of win. Stay of: " +  QString::number(players[i]->getStayPercent(),'f',2) + "% "),true, false, true);
                gproxy->SendLocalChat("[Statspage]: " + players[i]->getName().getMessage() + " has stats of: " + QString::number(players[i]->getGamesPlayed()) + " g with " + QString::number(players[i]->getWinPercent(),'f',2) + "% of win. Stay of: " + QString::number(players[i]->getStayPercent(),'f',2) + "%");
            }
            break;
        }
    }

    gproxy->setPlayers(players);
    widget.channelList->repaint();
}

/**
 * Slot: Executed when the adminlist was downloaded and parsed successfully.
 *
 * @param admins The adminlist.
 */
void MainGUI::onAdminlistReceived (QStringList admins)
{
    this->admins = admins;
    gproxy->getConfig()->setAdmins(admins);
    for (int i = 0; i < widget.channelList->count(); ++i)
    {
        addColor(widget.channelList->item(i));
        addTooltip(widget.channelList->item(i));
    }
    widget.channelList->repaint();
}

/**
 * Returns true if the name is the name of an GhostGraz admin.
 *
 * @param name Possible admin name.
 * @return True if the name is the name of an admin.
 */
bool MainGUI::isAdmin (const QString& username)
{
    if (admins.contains(username, Qt::CaseInsensitive))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Slot: Executed when the a usercolor has changed.
 *
 * @param colorDialog The color dialog for the user color.
 */
void MainGUI::userColorChanged(MColorDialog* colorDialog)
{
    // Example name is "ManufactoringColorDialog". Expected username is "Manufactoring".
    // Removing "ColorDialog".
    QString username = colorDialog->objectName().left(colorDialog->objectName().length() - 11);
    QColor color = colorDialog->currentColor();
    QListWidgetItem* item = VariantUtil<QListWidgetItem>::toPointer(colorDialog->property("listItem"));

    if (!colorDialog->isVisible())
    {
        color = gproxy->getConfig()->getUserColor(username);
    }

    item->setData(ChannellistDelegate::COLOR_USER, color);
    widget.outputTextArea->repaint("usercolor", color, username);
}

/**
 * Slot: Executed when the GameListDownloser has successfully downloded 
 * and parsed the game list.
 * 
 * @param gameList The list with game list entries.
 */
void MainGUI::onGameListDownloadFinished(QList<GameListEntry> gameList)
{
    widget.gameList->clear();
    
    foreach (GameListEntry gameListEntry, gameList)
    {
        QListWidgetItem* gameListItem = new QListWidgetItem();
    
        gameListItem->setData(GamelistDelegate::BOTNAME, gameListEntry.getHostname());
        gameListItem->setData(GamelistDelegate::GAMENAME, gameListEntry.getGamename());
        gameListItem->setData(GamelistDelegate::OPEN_SLOTS, gameListEntry.getOpenSlotCount());

        widget.gameList->addItem(gameListItem);
    }
}

CGProxy* MainGUI::getGproxy ()
{
    return gproxy;
}

Statspage* MainGUI::getStatspage ()
{
    return statspage;
}

#define ERROR 0 /* Redefine ERROR macro */
