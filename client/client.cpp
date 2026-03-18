#include "client.h"
#include "ui_dialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(MAIN_MENU_IDX);
    initializeScene();

    socket = new QTcpSocket(this);
    socket->setProxy(QNetworkProxy::NoProxy);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Dialog::SocketError);
    connect(socket, &QTcpSocket::connected, this, &Dialog::SocketConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Dialog::SocketDisconnected);

    connect(ui->exitButton, &QPushButton::clicked, this, &Dialog::accept);
    connect(ui->connectButton, &QPushButton::clicked, this, &Dialog::connectToServer);
    connect(ui->settingsButton, &QPushButton::clicked, this, &Dialog::openSettings);

    connect(ui->statsButton, &QPushButton::clicked, this, &Dialog::statsAppend);
    connect(ui->howtoButton, &QPushButton::clicked, this, &Dialog::howtoAppend);
    connect(ui->spellLibButton, &QPushButton::clicked, this, &Dialog::spellLibAppend);
    connect(ui->configButton, &QPushButton::clicked, this, &Dialog::configAppend);

    connect(ui->startButton, &QPushButton::clicked, this, &Dialog::sendStart);
    //connect(ui->lobbyBackButton, &QPushButton::clicked, this, &Dialog::leaveGame);
    //connect(ui->startButton, &QPushButton::clicked, this, &Dialog::startGame); //Remove this line once server can start game

    //backbuttons returnToMenu
    connect(ui->backButton_2, &QPushButton::clicked, this, &Dialog::returnToMenu);
    connect(ui->backButton_4, &QPushButton::clicked, this, &Dialog::returnToMenu);
    connect(ui->quitButton, &QPushButton::clicked, this, &Dialog::returnToMenu);

    ui->startButton->setEnabled(false);
    QTimer* timer = new QTimer;
    timer->setInterval(24);
    connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    timer->start();

    QPixmap playerSprite1 = QPixmap(":/sprites/BlueCastle.png");
    QPixmap playerSprite2 = QPixmap(":/sprites/RedCastle.png");
    QPixmap playerSprite3 = QPixmap(":/sprites/YellowCastle.png");
    QPixmap playerSprite4 = QPixmap(":/sprites/GreenCastle.png");

    int spriteSide = PLAYER_WIDTH * 3;
    ui->playerSpriteLabel_1->setPixmap(playerSprite1.scaled(spriteSide, spriteSide, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->playerSpriteLabel_2->setPixmap(playerSprite2.scaled(spriteSide, spriteSide, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->playerSpriteLabel_3->setPixmap(playerSprite3.scaled(spriteSide, spriteSide, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->playerSpriteLabel_4->setPixmap(playerSprite4.scaled(spriteSide, spriteSide, Qt::KeepAspectRatio, Qt::SmoothTransformation));


    ui->playerSpriteLabel_1->setMinimumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_2->setMinimumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_3->setMinimumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_4->setMinimumSize(spriteSide,spriteSide);

    ui->playerSpriteLabel_1->setMaximumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_2->setMaximumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_3->setMaximumSize(spriteSide,spriteSide);
    ui->playerSpriteLabel_4->setMaximumSize(spriteSide,spriteSide);


    int NameLabelHeight = ui->playerNameLabel_1->height();
    ui->playerNameLabel_1->setMinimumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_2->setMinimumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_3->setMinimumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_4->setMinimumSize(spriteSide,NameLabelHeight);

    ui->playerNameLabel_1->setMaximumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_2->setMaximumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_3->setMaximumSize(spriteSide,NameLabelHeight);
    ui->playerNameLabel_4->setMaximumSize(spriteSide,NameLabelHeight);

    // ui->progressBar_1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // ui->progressBar_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // ui->progressBar_3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // ui->progressBar_4->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->healthbarLabel_1->setMaximumWidth(SCENE_WIDTH);
    ui->healthbarLabel_2->setMaximumWidth(SCENE_WIDTH);
    ui->healthbarLabel_3->setMaximumWidth(SCENE_WIDTH);
    ui->healthbarLabel_4->setMaximumWidth(SCENE_WIDTH);
    
    updateLobbyCount(0);
    updateHealthBar(0,100);
}

void Dialog::initializeScene()
{
    scene = new QGraphicsScene(-SCENE_WIDTH/2, -SCENE_HEIGHT/2,
                               SCENE_WIDTH, SCENE_HEIGHT);
    QPixmap bgpm = QPixmap(":/sprites/background.png");
    bg = scene->addPixmap(bgpm);
    bg->setPos(QPointF(-SCENE_WIDTH/2,-SCENE_HEIGHT/2));
    ui->gameView->setScene(scene);
    ui->gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setMinimumSize(SCENE_WIDTH,SCENE_HEIGHT);
    ui->gameView->installEventFilter(this);
    bg->setPos(-SCENE_WIDTH/2, -SCENE_HEIGHT/2);

}
void Dialog::howtoAppend()
{
    ui->settingsBrowser->clear();
    ui->settingsBrowser->show();
    ui->configFrame->hide();
    ui->settingsBrowser->append("Type in the text bar at the bottom to cast spells.\nType faster than everyone else to destroy your enemies before they destroy you.");
}
void Dialog::spellLibAppend()
{

    ui->settingsBrowser->clear();
    ui->settingsBrowser->show();
    ui->configFrame->hide();
    ui->settingsBrowser->append("attacks:\n\n");
    ui->settingsBrowser->append("'INCENDARIUM' - Fireball\n");
    ui->settingsBrowser->append("'INCENDARIUM AETERNALIS' - Mega Fireball\n");
    ui->settingsBrowser->append("'PEWPEWPEW' - standard bolt\n");

    /*sl = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName(":memory:");
    sl.setDatabaseName("custom.db");

    if ( !sl.open() )
    {
        qDebug() << "Error Opening Database: " << sl.lastError();
        exit(1);

    }
    QSqlQuery qprep;

    if ( !qprep.exec("CREATE TABLE IF NOT EXISTS spells(spell TEXT, desc TEXT)") )
    {
        qDebug() << "Error Creating Table: " << sl.lastError();
        return;
    }


    //qprep.exec("SELECT * FROM spells");

    ui->settingsBrowser->append("***** Prep Start *****");
    if (!qprep.exec("SELECT item, owner FROM spells"))
    {
        qDebug() << "Select failed:" << qprep.lastError();
    }


    while (qprep.next())
    {
        QString s = qprep.value(0).toString() + "  " + qprep.value(1).toString();
        ui->settingsBrowser->append(s);
    }
    ui->settingsBrowser->append("***** Prep Ends *****\n");
    */
}



//bool Dialog::eventFilter(QObject *watched, QEvent *event)
//{
//    if (watched == ui->gameView && event->type() == QEvent::MouseButtonPress)
//    {
//        QMouseEvent* me = static_cast<QMouseEvent*>(event);

//        if (me->button() == Qt::LeftButton)
//        {
//            QPointF scenePos = ui->gameView->mapToScene(me->pos());
//            castSpell(scenePos);
//            return true; // event handled
//        }
//    }

//    return QDialog::eventFilter(watched, event);
//}

void Dialog::connectToServer()
{
    qDebug() << "Connect2Server Call";
    QHostAddress ipAddress(ui->ipEnter->text());    //Convert the text entered in the line edit into QHostAddress
    qDebug() << ipAddress;
    socket->connectToHost(ipAddress, 5678); //Connect to the server
    connect(socket, &QTcpSocket::readyRead, this, &Dialog::processMsg);

}

void Dialog::sendStart()
{
    QString playerName = "PLAYER" + QString::number(clientId);
    sendMsg(playerName.toUtf8() + ",START\n");
}

void Dialog::startGame()
{
    gameIsStarted = true;
     deathsSeen = 0;
    spawnCrosshair();
    ui->stackedWidget->setCurrentIndex(GAME_SCR_IDX);
    openSpellbook();
    playerJoinPixmap();
    QTimer* timer = new QTimer;
       timer->setInterval(24);
       connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);

       timer->start();
        gamePlayerCount = lobbyCount;
}

void Dialog::leaveGame()
{
    qDebug() << "Leaving Game.";
    if (socket)
    {
        qDebug() << "Closing Socket.";
        socket->close();
    }
    ui->stackedWidget->setCurrentIndex(MAIN_MENU_IDX);
    ui->startButton->setEnabled(false);
}

void Dialog::openSettings()
{
    ui->stackedWidget->setCurrentIndex(SETTINGS_IDX);
    ui->configFrame->hide();
    ui->settingsBrowser->show();

}

void Dialog::returnToMenu()
{
    leaveGame();
}

void Dialog::statsAppend()
{
    ui->settingsBrowser->clear();
    ui->settingsBrowser->show();
    ui->configFrame->hide();

    lb = QSqlDatabase::addDatabase("QSQLITE");
    lb.setDatabaseName("custom.db");

    if (!lb.open())
    {
        qDebug() << "Error Opening Database: " << lb.lastError();
        return;
    }

    QSqlQuery qprep(lb);

    if (!qprep.exec("CREATE TABLE IF NOT EXISTS stats("
                    "id INTEGER PRIMARY KEY CHECK(id = 1), "
                    "win INT, "
                    "loss INT)"))
    {
        qDebug() << "Error Creating Table: " << lb.lastError();
        return;
    }

    // Ensure row exists
    if (!qprep.exec("INSERT OR IGNORE INTO stats(id, win, loss) VALUES(1, 0, 0)"))
    {
        qDebug() << "Insert default row failed:" << qprep.lastError();
        return;
    }

    ui->settingsBrowser->append("Stats for this computer:");
    ui->settingsBrowser->append("****************");

    if (!qprep.exec("SELECT win, loss FROM stats WHERE id = 1"))
    {
        qDebug() << "Select failed:" << qprep.lastError();
    }

    if (qprep.next())
    {
        QString s = "Wins: " + qprep.value(0).toString() +
                    "\nLosses: " + qprep.value(1).toString();
        ui->settingsBrowser->append(s);
    }
    else
    {
        ui->settingsBrowser->append("No games recorded yet.");
    }

    ui->settingsBrowser->append("****************");
}




void Dialog::configAppend()
{

    ui->settingsBrowser->hide();
    ui->configFrame->show();
    //QPushButton *cursorSpeed = new QPushButton("Config: Double Sensitivity", this);
    //ui->configLayout->insertWidget(0, cursorSpeed);
    //cursorSpeed->setGeometry()
}




void Dialog::spawnCrosshair()
{
    // Spawn custom Player object in center of scene
    if (crosshair)
    {
        delete crosshair;
    }
    crosshair = new Crosshair;
    crosshair->SetPlayer(clientId);
    QPointF p = crosshair->getCorner();

    if (HITBOXES_ON)
    {
        QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(p.rx() - crosshair->BOUNDING_RADIUS, p.ry()-crosshair->BOUNDING_RADIUS, 2*crosshair->BOUNDING_RADIUS, 2*crosshair->BOUNDING_RADIUS);
        QPen pen(Qt::yellow,1);
        circle->setPen(pen);
        //circle->setBrush(Qt::red);

        scene->addItem(circle);
    }
    qDebug() << p;
    if (p.rx() >= 0)
    {
        p.setX(p.rx() - crosshair->BOUNDING_RADIUS/2);
    }
    else
    {
        p.setX(p.rx() + crosshair->BOUNDING_RADIUS/2);
    }

    if (p.ry() >= 0)
    {
        p.setY(p.ry() - crosshair->BOUNDING_RADIUS/2);
    }
    else
    {
        p.setY(p.ry() + crosshair->BOUNDING_RADIUS/2);
    }

    //p.setX(SCENE_WIDTH/4);
    //p.setY(0);
    qDebug() << p;
    crosshair->setPos(p);
    scene->addItem(crosshair);
}

void Dialog::SocketError()
{
    qDebug() << "Socket Error.: " << socket->error();
    if (socket->error() == QAbstractSocket::ConnectionRefusedError)
    {
        qDebug() << "Connection Refused...";
        ui->debugBrowser->append("Connection Refused...");
    }
}

void Dialog::SocketConnected()
{
    qDebug() << "Socket Connected :)";
    ui->debugBrowser->append("Socked Connected :)");
    ui->stackedWidget->setCurrentIndex(LOBBY_SCR_IDX);
}

void Dialog::SocketDisconnected()
{
    qDebug() << "Socket Disconnected :(";
    ui->debugBrowser->append("Socket Disconnected :(");
    leaveGame();
}

void Dialog::keyPressEvent(QKeyEvent *e)
{
    bool spellArmed = true; // Eventually change this to be set to a function call.
    if (e && spellArmed && ui->stackedWidget->currentIndex() == GAME_SCR_IDX)
    {
        switch (e->key())
        {
        case Qt::Key_W:
        case Qt::Key_I:
            //crosshair.up
            crosshair->goUp();
        break;

        case Qt::Key_S:
        case Qt::Key_K:
            //crosshair.down
            crosshair->goDown();
        break;

        case Qt::Key_A:
        case Qt::Key_J:
            //crosshair.left
            crosshair->goLeft();
        break;

        case Qt::Key_D:
        case Qt::Key_L:
            //crosshair.right
            crosshair->goRight();
        break;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            //Use Spell?
            qDebug() << "Enter/Return Detected" << crosshair->pos();
            if (ui->spellBar->hasFocus())
            {
                castSpell(crosshair->pos().toPoint());
                ui->gameView->setFocus();
            } 
            else if (ui->gameView->hasFocus())
            {
                ui->spellBar->setFocus();
            } 
            else
            {
                qDebug() << "Neither spellBar nor gameView has the focus";
            }
        break;
        default:
        break;

        }
    }
    QDialog::keyPressEvent(e);
}


void Dialog::keyReleaseEvent(QKeyEvent *e)
{
    if (e && ui->stackedWidget->currentIndex() == GAME_SCR_IDX)
    {
        switch (e->key())
        {
        case Qt::Key_W:
        case Qt::Key_I:
            //if dy>0 -> dy=0
            qDebug() << "W released" << crosshair->getDy();
            if (crosshair->getDy() < 0)
            {
                crosshair->stopY();
            }
            break;

        case Qt::Key_S:
        case Qt::Key_K:
            //if dy<0 -> dy=0
            qDebug() << "S released" << crosshair->getDy();
            if (crosshair->getDy() > 0)
            {
                crosshair->stopY();
            }
            break;

        case Qt::Key_A:
        case Qt::Key_J:
            //if dx<0 -> dx=0
            if (crosshair->getDx() < 0)
            {
                crosshair->stopX();
            }
            break;

        case Qt::Key_D:
        case Qt::Key_L:
            //if dx>0 -> dx=0
            if (crosshair->getDx() > 0)
            {
                crosshair->stopX();
            }
            break;

        default:
            break;
        }
    }
    QDialog::keyReleaseEvent(e);
}

void Dialog::processMsg()
{
    QByteArray ba = socket->readAll();
    QString s(ba);
    QStringList messages = s.split("\n", QString::SkipEmptyParts);

    for(QString msg : messages)
    {
        processSingleMsg(msg);
    }
}

void Dialog::processSingleMsg(QString s)
{

    //qDebug() << "Processing Msg: " << s;
    QString player = "";
    QString messageType = "";
    QString spellType = "";
    QString x = "";
    QString y = "";
    QString rot = "";
    QString spellId = "";

    int isSpawnShield = 0;
    int isSpawnFireball = 0;
    int isSpawnMegaFireball = 0;
    int isUpdateFireball = 0;
    int isUpdateMegaFireball = 0;
    int isSpawnBolt = 0;
    int isUpdateBolt = 0;
    int isDeleteShield = 0;
    int isDeleteFireball = 0;
    int isDeleteMegaFireball = 0;
    int isDeleteBolt = 0;
    int isName = 0;
    int isPlayerCount = 0;
    int isHealthUpd = 0;

    qDebug() << "Incoming MSG: " << s;
    int iter = 0;
    for(int i = 0; i < s.length(); i++)
    {

        if(s[i] == ',')
        {
            iter++;
            qDebug() << "messageType: " << messageType;
            if(messageType == "HEALTH")
            {
                //call function here
            }


            if (messageType == "SPAWNSHIELD")
            {
                isSpawnShield = 1;
            }
            if(messageType == "SPAWNFIREBALL")
            {
                isSpawnFireball = 1;
            }

            if(messageType == "SPAWNMEGAFIREBALL")
            {
                isSpawnMegaFireball = 1;
            }

            if(messageType == "SPAWNBOLT")
            {
                isSpawnBolt = 1;
            }
            if(messageType == "DELETESHIELD")
            {
                isDeleteShield= 1;
            }
            if(messageType == "DELETEFIREBALL")
            {
                isDeleteFireball = 1;
            }
            if(messageType == "DELETEMEGAFIREBALL")
            {
                isDeleteMegaFireball = 1;
            }
            if(messageType == "DELETEBOLT")
            {
                isDeleteBolt = 1;
            }
            if(messageType == "DEFENSE")
            {
                //call function here
            }


            if(messageType == "UPDATEFIREBALL")
            {
                isUpdateFireball = 1;
            }
            if(messageType == "UPDATEMEGAFIREBALL")
            {
                isUpdateMegaFireball = 1;
            }
            if(messageType == "UPDATEBOLT")
            {
                isUpdateBolt = 1;
            }
            if(messageType == "NAME")
            {
                isName = 1;
            }
            if(messageType == "PLAYERCOUNT")
            {
                isPlayerCount = 1;
            }
            if(messageType == "HEALTHUPD")
            {
                isHealthUpd = 1;
            }
        }
            if(s[i]!= ',' && iter==0)
            {
                player.append(s[i]);
                //qDebug() << player;

            }

            else if(s[i]!= ',' && iter==1)
            {
                messageType.append(s[i]);
                //qDebug() << messageType;
            }


        //FOR SPAWNSHIELD
        else if(s[i]!= ',' && iter == 2 && isSpawnShield == 1)
        {
            spellId.append(s[i]);
        }
        else if(s[i]!= ',' && iter == 3 && isSpawnShield == 1)
        {
            x.append(s[i]);
        }
        else if(s[i]!= ',' && iter == 4 && isSpawnShield == 1)
        {
            y.append(s[i]);
        }
        else if(s[i]!= ',' && iter == 5 && isSpawnShield == 1)
        {
            rot.append(s[i]);
        }
        //END SPAWNSHIELD

        //FOR SPAWNFIREBALL
            else if(s[i]!= ',' && iter == 2 && isSpawnFireball == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isSpawnFireball == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isSpawnFireball == 1)
            {
                y.append(s[i]);
            }
        //END SPAWNFIREBALL

        //FOR SPAWNMEGAFIREBALL
            else if(s[i]!= ',' && iter == 2 && isSpawnMegaFireball == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isSpawnMegaFireball == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isSpawnMegaFireball == 1)
            {
                y.append(s[i]);
            }
        //END SPAWNMEGAFIREBALL

        //FOR SPAWNBOLT
            else if(s[i]!= ',' && iter == 2 && isSpawnBolt == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isSpawnBolt == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isSpawnBolt == 1)
            {
                y.append(s[i]);
            }
        //END SPAWNBOLT

        //FOR UPDATEFIREBALL
            else if(s[i]!= ',' && iter == 2 && isUpdateFireball == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isUpdateFireball == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isUpdateFireball == 1)
            {
                y.append(s[i]);
            }

        //END UPDATEFIREBALL

        //FOR UPDATEMEGAFIREBALL
            else if(s[i]!= ',' && iter == 2 && isUpdateMegaFireball == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isUpdateMegaFireball == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isUpdateMegaFireball == 1)
            {
                y.append(s[i]);
            }

        //END UPDATEMEGAFIREBALL

        //FOR UPDATEBOLT
            else if(s[i]!= ',' && iter == 2 && isUpdateBolt == 1)
            {
                spellId.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 3 && isUpdateBolt == 1)
            {
                x.append(s[i]);
            }
            else if(s[i]!= ',' && iter == 4 && isUpdateBolt == 1)
            {
                y.append(s[i]);
            }

        //END UPDATEBOLT

        //FOR DELETEFIREBALL
            else if(s[i]!= ',' && iter == 2 && (isDeleteShield == 1 || isDeleteFireball == 1 || isDeleteMegaFireball == 1 || isDeleteBolt == 1 || isName == 1 || isPlayerCount == 1 || isHealthUpd == 1))
            {
                spellId.append(s[i]);
            }

        //END DELETEFIREBALL
            
    }

    if(messageType == "PLAYERID")
    {
        //qDebug() << clientId;
        clientId = player.toInt();
        qDebug() << "clientId = " << clientId;
        if (ui->nameEdit->text() != "")
        {
            QString nameMsg = "PLAYER" + QString::number(clientId) + ",NAME," + ui->nameEdit->text() + "\n";
            qDebug() << "Sending Name Message: " << nameMsg;
            sendMsg(nameMsg.toUtf8());
        }
    }
    if (messageType == "DEATH" || messageType == "PLAYERDEAD")
    {
        int deadId = 0;

        // Server can send either "PLAYER1,DEATH" / "PLAYER2,DEATH" or "1,DEATH"
        if (player.startsWith("PLAYER"))
        {
            deadId = player.back().digitValue();
        }
        else
        {
            deadId = player.toInt();
        }

        qDebug() << "[CLIENT] Death message for player id:" << deadId
                 << "type:" << messageType;
        handleRemotePlayerDeath(deadId);
        return;    // Done with this message
    }
    if(messageType == "START")
    {
        startGame();
    }
    if(messageType == "HOST")
    {
        qDebug() << "Im the host";
        ui->startButton->setEnabled(true);
    }
    if(messageType == "NAME")
    {
        int playerNum = 0;
        QCharRef numChar = player.back();
        qDebug() << "Received Name message:" + player + ":" + messageType + ":." + spellId << "   Numchar="+numChar;

        if(numChar == '1' || numChar == '2' || numChar == '3' || numChar == '4')
        {
            playerNum = numChar.digitValue();
            qDebug() << "Changing Player Name for PLAYER" + QString::number(playerNum);
            if (playerNum == 1)
            {
                ui->playerNameLabel_1->setText(spellId);
                ui->healthbarLabel_1->setText(spellId);
            }
            else if (playerNum == 2)
            {
                ui->playerNameLabel_2->setText(spellId);
                ui->healthbarLabel_2->setText(spellId);
            }
            else if (playerNum == 3)
            {
                ui->playerNameLabel_3->setText(spellId);
                ui->healthbarLabel_3->setText(spellId);
            }
            else if (playerNum == 4)
            {
                ui->playerNameLabel_4->setText(spellId);
                ui->healthbarLabel_4->setText(spellId);
            }
            
        }
    }
    if (messageType == "PLAYERCOUNT")
    {
        updateLobbyCount(spellId.toInt());
    }
    if (messageType == "HEALTHUPD")
    {
        updateHealthBar(player.back().digitValue(), spellId.toInt());
    }

    if(messageType == "SPAWNSHIELD")
    {
        //qDebug() <<player << spellId << x << y;
        spawnShield(player, spellId, x, y, rot);
    }
    if(messageType == "SPAWNFIREBALL")
    {
        //qDebug() <<player << spellId << x << y;
        spawnFireball(player, spellId, x, y);
    }
    if(messageType == "UPDATEFIREBALL")
    {
        //qDebug() << spellId << x << y;
        updateFireball(spellId, x, y);

    }

    if(messageType == "DELETESHIELD")
    {
        deleteShield(spellId);
        qDebug() << spellId;
    }
    if(messageType == "DELETEFIREBALL")
    {
        deleteFireball(spellId);
        qDebug() << spellId;
    }

    if(messageType == "SPAWNMEGAFIREBALL")
    {
        //qDebug() <<player << spellId << x << y;
        spawnMegaFireball(player, spellId, x, y);
    }
    if(messageType == "UPDATEMEGAFIREBALL")
    {
        //qDebug() << spellId << x << y;
        updateMegaFireball(spellId, x, y);

    }
    if(messageType == "DELETEMEGAFIREBALL")
    {
        deleteMegaFireball(spellId);
    }

    if(messageType == "SPAWNBOLT")
    {
        //qDebug() <<player << spellId << x << y;
        spawnBolt(player, spellId, x, y);
    }
    if(messageType == "UPDATEBOLT")
    {
        //qDebug() << spellId << x << y;
        updateBolt(spellId, x, y);

    }
    if(messageType == "DELETEBOLT")
    {
        deleteBolt(spellId);
    }
    if(messageType == "TOOLATE")
    {
        ui->debugBrowser->append("The game has already started... Bye bye.");
    }
}

void Dialog::spawnShield(QString player, QString spellId, QString cursorx, QString cursory, QString rot)
{
    int id = spellId.toInt();
    double xPosition = cursorx.toInt();
    double yPosition = cursory.toInt();
    double rotation = rot.toDouble();
    shields[id] = new shieldvisual;
    shields[id]->setPos(xPosition, yPosition);
    shields[id]->setRotationVal(rotation);
    scene->addItem(shields[id]);
}


void Dialog::spawnFireball(QString player, QString spellId, QString cursorx, QString cursory)
{
    int id = spellId.toInt();
    double xPosition = cursorx.toInt();
    double yPosition = cursory.toInt();
    fireballs[id] = new FireballVisual;
    fireballs[id]->setPos(xPosition, yPosition);
    scene->addItem(fireballs[id]);

}

void Dialog::spawnMegaFireball(QString player, QString spellId, QString cursorx, QString cursory)
{
    int id = spellId.toInt();
    double xPosition = cursorx.toInt();
    double yPosition = cursory.toInt();
    megafireballs[id] = new MegaFireballVisual;
    megafireballs[id]->setPos(xPosition, yPosition);
    scene->addItem(megafireballs[id]);

}

void Dialog::spawnBolt(QString player, QString spellId, QString cursorx, QString cursory)
{
    int id = spellId.toInt();
    double xPosition = cursorx.toInt();
    double yPosition = cursory.toInt();
    bolts[id] = new BoltVisual;
    bolts[id]->setPos(xPosition, yPosition);
    scene->addItem(bolts[id]);

}
void Dialog::updateFireball(QString spellId, QString x, QString y)
{

    double xPosition = x.toDouble();
    double yPosition = y.toDouble();
    int id = spellId.toInt();



    fireballs[id]->x = xPosition;
    fireballs[id]->y = yPosition;
    fireballs[id]->setPos(xPosition, yPosition);


}

void Dialog::updateMegaFireball(QString spellId, QString x, QString y)
{

    double xPosition = x.toDouble();
    double yPosition = y.toDouble();
    int id = spellId.toInt();



    megafireballs[id]->x = xPosition;
    megafireballs[id]->y = yPosition;
    megafireballs[id]->setPos(xPosition, yPosition);


}

void Dialog::updateBolt(QString spellId, QString x, QString y)
{

    double xPosition = x.toDouble();
    double yPosition = y.toDouble();
    int id = spellId.toInt();



    bolts[id]->x = xPosition;
    bolts[id]->y = yPosition;
    bolts[id]->setPos(xPosition, yPosition);


}

void Dialog::deleteShield(QString spellId)
{
    int id = spellId.toInt();
    qDebug() << "deleting shield" << id;
    scene->removeItem(shields[id]);
    delete shields[id];
    shields.remove(id);
}

void Dialog::deleteFireball(QString spellId)
{
    int id = spellId.toInt();
    //qDebug() << id;
    scene->removeItem(fireballs[id]);
    delete fireballs[id];
    fireballs.remove(id);
    qDebug() << "ball deleted";
}

void Dialog::deleteMegaFireball(QString spellId)
{
    int id = spellId.toInt();
    qDebug() << "megafireball" << id;
    scene->removeItem(megafireballs[id]);
    delete megafireballs[id];
    megafireballs.remove(id);
}

void Dialog::deleteBolt(QString spellId)
{
    int id = spellId.toInt();
    qDebug() << "bolt" << id;
    scene->removeItem(bolts[id]);
    delete bolts[id];
    bolts.remove(id);
}
void Dialog::castSpell(QPointF cursorPoint)
{
    QString spellType = ui->spellBar->text();
    spellType = spellType.toUpper();
    ui->spellBar->clear();
    qDebug() << cursorPoint;
    QString playerName = "PLAYER" + QString::number(clientId);
    if(spellType == "INCENDARIUM"){
      sendMsg("DUMMY,SPELL,FIREBALL," + QByteArray::number(cursorPoint.rx()) + "," + QByteArray::number((cursorPoint.ry())) + '\n');
    }
    else if(spellType == "INCENDARIUM AETERNALIS"){
        sendMsg("DUMMY,SPELL,MEGAFIREBALL," + QByteArray::number(cursorPoint.rx()) + "," + QByteArray::number(cursorPoint.ry()) + '\n');
    }
    else if(spellType == "PEWPEWPEW"){
        sendMsg("DUMMY,SPELL,BOLT," + QByteArray::number(cursorPoint.rx()) + "," + QByteArray::number(cursorPoint.ry()) + '\n');
    }
    else if(spellType == "15HEALS") {
        sendMsg(playerName.toUtf8() + ",SPELL,HEAL\n");
    }
    else if(spellType == "HEALMENOWPLEASE...") {
        sendMsg(playerName.toUtf8() + ",SPELL,BIGHEAL\n");
    }
    else if(spellType == "HITASLURPJUICE") {
        sendMsg(playerName.toUtf8() + ",SPELL,REGEN\n");
    }else if(spellType == "STOPTHATNOW")
    {
        sendMsg("DUMMY,SPELL,SHIELD," + QByteArray::number(cursorPoint.rx()) + "," + QByteArray::number(cursorPoint.ry()) + '\n');
    }
}

void Dialog::updateHealthBar(int player, int health)
{
    qDebug() << "UpdateHealthBar Player" << player << " to health:" << health;
    if (player == 0)// Initialization
    {
        ui->progressBar_1->setValue(100);
        ui->progressBar_2->setValue(100);
        ui->progressBar_3->setValue(100);
        ui->progressBar_4->setValue(100);
    }
    if (player == 1)
    {
        ui->progressBar_1->setValue(health);
    }
    if (player == 2)
    {
        ui->progressBar_2->setValue(health);
    }
    if (player == 3)
    {
        ui->progressBar_3->setValue(health);
    }
    if (player == 4)
    {
        ui->progressBar_4->setValue(health);
    }
}

void Dialog::updateLobbyCount(int count)
{
    qDebug() << "Setting lobby to " + QString::number(count) + " players...";
    QLabel* lobbyLabels[] = {ui->playerNameLabel_1, ui->playerNameLabel_2, ui->playerNameLabel_3, ui->playerNameLabel_4};
    QLabel* lobbySprites[] = {ui->playerSpriteLabel_1, ui->playerSpriteLabel_2, ui->playerSpriteLabel_3, ui->playerSpriteLabel_4};
    QLabel* healthbarsLabels[] = {ui->healthbarLabel_1, ui->healthbarLabel_2, ui->healthbarLabel_3, ui->healthbarLabel_4};
    QProgressBar* healthbars[] = {ui->progressBar_1, ui->progressBar_2, ui->progressBar_3, ui->progressBar_4};
    
    lobbyCount = count;
    for (int i = 0; i < 4; i++)
    {
        if (i + 1 <= count)
        {
            lobbyLabels[i]->setVisible(true);
            lobbySprites[i]->setVisible(true);
            if (!gameIsStarted)
            {
                healthbarsLabels[i]->setVisible(true);
                healthbars[i]->setVisible(true);
            }
        }
        else
        {
            lobbyLabels[i]->setVisible(false);
            lobbySprites[i]->setVisible(false);
            if (!gameIsStarted)
            {
                healthbarsLabels[i]->setVisible(false);
                healthbars[i]->setVisible(false);
            }
        }
    }
    lobbyCount = count;
}

void Dialog::sendMsg(QByteArray ba)
{
    socket->write(ba);
}

// _______________ Draw PLayers ______________________ //

void Dialog::playerJoinPixmap()
{
   // Load player sprites
   QPixmap playerSprite1 = QPixmap(":/sprites/BlueCastle.png");
   QPixmap playerSprite2 = QPixmap(":/sprites/RedCastle.png");
   QPixmap playerSprite3 = QPixmap(":/sprites/YellowCastle.png");
   QPixmap playerSprite4 = QPixmap(":/sprites/GreenCastle.png");

   // Load island sprites
   QPixmap islandSprite1 = QPixmap(":/sprites/PlayerIsland.png");
   QPixmap islandSprite2 = QPixmap(":/sprites/PlayerIsland.png");
   QPixmap islandSprite3 = QPixmap(":/sprites/PlayerIsland.png");
   QPixmap islandSprite4 = QPixmap(":/sprites/PlayerIsland.png");

   // Scale the sprites down
   int playerWidth = 80;
   int playerHeight = 80;
   int islandWidth = 85;
   int islandHeight = 80;

   playerSprite1 = playerSprite1.scaled(playerWidth, playerHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   playerSprite2 = playerSprite2.scaled(playerWidth, playerHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   playerSprite3 = playerSprite3.scaled(playerWidth, playerHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   playerSprite4 = playerSprite4.scaled(playerWidth, playerHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

   islandSprite1 = islandSprite1.scaled(islandWidth, islandHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   islandSprite2 = islandSprite2.scaled(islandWidth, islandHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   islandSprite3 = islandSprite3.scaled(islandWidth, islandHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   islandSprite4 = islandSprite4.scaled(islandWidth, islandHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

   // Add islands to scene
   island1 = scene->addPixmap(islandSprite1);
   island2 = scene->addPixmap(islandSprite2);
   island3 = scene->addPixmap(islandSprite3);
   island4 = scene->addPixmap(islandSprite4);

   // Add players to scene
   player1 = scene->addPixmap(playerSprite1);
   player2 = scene->addPixmap(playerSprite2);
   player3 = scene->addPixmap(playerSprite3);
   player4 = scene->addPixmap(playerSprite4);

   // Position islands and players in corners
   int margin = 10;

   // Top Left - Island then Castle
   island1->setPos(-SCENE_WIDTH/2 + margin, -SCENE_HEIGHT/2 + margin + playerHeight - 40);
   player1->setPos(-SCENE_WIDTH/2 + margin + 5, -SCENE_HEIGHT/2 + margin - 15);
   //player1->setPos(-SCENE_WIDTH/2 + margin + 5, -SCENE_HEIGHT/2 + margin + 25);

   // Top Right - Island then Castle
   island2->setPos(SCENE_WIDTH/2 - islandWidth - margin, -SCENE_HEIGHT/2 + margin + playerHeight - 40);
   player2->setPos(SCENE_WIDTH/2 - playerWidth - margin, -SCENE_HEIGHT/2 + margin - 15);

   // Bottom Left - Castle then Island
   island3->setPos(-SCENE_WIDTH/2 + margin, SCENE_HEIGHT/2 - islandHeight - margin - 4);
   player3->setPos(-SCENE_WIDTH/2 + margin, SCENE_HEIGHT/2 - playerHeight - islandHeight - margin + 16);

   // Bottom Right - Castle then Island
   island4->setPos(SCENE_WIDTH/2 - islandWidth - margin, SCENE_HEIGHT/2 - islandHeight - margin - 4);
   player4->setPos(SCENE_WIDTH/2 - playerWidth - margin, SCENE_HEIGHT/2 - playerHeight - islandHeight - margin + 16);
}



// ___________________________________________________ //



//_________________ Player Spellbook _________________ //
void Dialog::openSpellbook(){
    QDialog* d = new QDialog;


    QVBoxLayout* layout = new QVBoxLayout(d);
    QLabel* spelllabel = new QLabel("Help");
    layout->addWidget(spelllabel);
    QString s = ":/sprites/spellbook.png";
    QPixmap spellbook = QPixmap(s);
    spellbook = spellbook.scaled(511, 630);

    spelllabel-> setPixmap(spellbook);

    d->setWindowTitle("Spellbook");


    d->show();
}
// ___________________________________________________ //

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::handleRemotePlayerDeath(int deadId)
{
    // Remove that player's castle + island sprites
    switch (deadId)
    {
    case 1:
        if (player1) { scene->removeItem(player1); delete player1; player1 = nullptr; }
        if (island1) { scene->removeItem(island1); delete island1; island1 = nullptr; }
        updateHealthBar(deadId, 0);
        break;
    case 2:
        if (player2) { scene->removeItem(player2); delete player2; player2 = nullptr; }
        if (island2) { scene->removeItem(island2); delete island2; island2 = nullptr; }
        updateHealthBar(deadId, 0);
        break;
    case 3:
        if (player3) { scene->removeItem(player3); delete player3; player3 = nullptr; }
        if (island3) { scene->removeItem(island3); delete island3; island3 = nullptr; }
        updateHealthBar(deadId, 0);
        break;
    case 4:
        if (player4) { scene->removeItem(player4); delete player4; player4 = nullptr; }
        if (island4) { scene->removeItem(island4); delete island4; island4 = nullptr; }
        updateHealthBar(deadId, 0);
        break;
    default:
        break;
    }

    // If YOU are the one who died -> LOSS
    if (deadId == clientId)
    {
        if (crosshair) {
            scene->removeItem(crosshair);
            delete crosshair;
            crosshair = nullptr;
        }

        recordLoss();  // update local SQL

        ui->debugBrowser->append("You died!");
        leaveGame();
        return;
    }

    // Someone else died → check if you're last alive
    int alive = 0;
    bool meAlive = false;

    if (gamePlayerCount == 1 && player1) { ++alive; if (clientId == 1) meAlive = true; }
        if (gamePlayerCount == 2 && player2) { ++alive; if (clientId == 2) meAlive = true; }
        if (gamePlayerCount == 3 && player3) { ++alive; if (clientId == 3) meAlive = true; }
        if (gamePlayerCount == 4 && player4) { ++alive; if (clientId == 4) meAlive = true; }

    // If exactly one player is left and it's you -> WIN
    if (meAlive && alive == 1)
        {
            if (gamePlayerCount > 1)
            {
                // Real win: there were multiple players at start
                recordWin();  // update local SQL
                ui->debugBrowser->append("YOU WIN!");
            }
            else
            {
                // Solo game -> don't record win/loss
                ui->debugBrowser->append("Game ended (solo player - no win/loss recorded).");
            }

            leaveGame();             // NEW: close socket & return to main menu
        }
}


void Dialog::recordLoss()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "lossConnection");
    db.setDatabaseName("custom.db");

    if (!db.open()) {
        qDebug() << "Error opening DB in recordLoss:" << db.lastError();
        return;
    }

    QSqlQuery q(db);

    if (!q.exec("CREATE TABLE IF NOT EXISTS stats("
                "id INTEGER PRIMARY KEY CHECK(id = 1), "
                "win INT, "
                "loss INT)")) {
        qDebug() << "Create table failed:" << q.lastError();
        db.close();
        return;
    }

    if (!q.exec("INSERT OR IGNORE INTO stats(id, win, loss) VALUES(1, 0, 0)")) {
        qDebug() << "Insert default row failed:" << q.lastError();
        db.close();
        return;
    }

    if (!q.exec("UPDATE stats SET loss = loss + 1 WHERE id = 1")) {
        qDebug() << "Update loss failed:" << q.lastError();
    }

    db.close();
}


void Dialog::recordWin()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "winConnection");
    db.setDatabaseName("custom.db");

    if (!db.open()) {
        qDebug() << "Error opening DB in recordWin:" << db.lastError();
        return;
    }

    QSqlQuery q(db);

    // Single local stats row: id always 1
    if (!q.exec("CREATE TABLE IF NOT EXISTS stats("
                "id INTEGER PRIMARY KEY CHECK(id = 1), "
                "win INT, "
                "loss INT)")) {
        qDebug() << "Create table failed:" << q.lastError();
        db.close();
        return;
    }

    // Make sure row exists
    if (!q.exec("INSERT OR IGNORE INTO stats(id, win, loss) VALUES(1, 0, 0)")) {
        qDebug() << "Insert default row failed:" << q.lastError();
        db.close();
        return;
    }

    // Increment win
    if (!q.exec("UPDATE stats SET win = win + 1 WHERE id = 1")) {
        qDebug() << "Update win failed:" << q.lastError();
    }

    db.close();
}
