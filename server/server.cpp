#include "server.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    playerCount = 0;

    QString myIP;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    for(int index = 0; index < list.count(); index++)
    {
        if( !list[index].isLoopback() )
        {
            if ( list[index].protocol() == QAbstractSocket::IPv4Protocol )
            {
                myIP = list[index].toString();
            }
        }
    }

    this->setWindowTitle(myIP);    //Places local IP into title bar of program window
    QHostAddress ipAddress(myIP);   //Convert string myIP into QHostAddress
    if (!server.listen(ipAddress, 5678)) //Listen for new connections
    {
        qDebug() << "No listen";
        //ui->debugBrowser->append("No listen");
    }
    connect(&server, &QTcpServer::newConnection, this, &Dialog::acceptConnection);  //When server detects new connection, call acceptConnection function
    QTimer* timer = new QTimer;
    timer->setInterval(24);
    connect(timer, &QTimer::timeout, this, &Dialog::sendProjectileUpdates);
    connect(timer, &QTimer::timeout, this, &Dialog::regenCheck);
    timer->start();

 }

void Dialog::initializeScene()
{
    scene = new QGraphicsScene(-SCENE_WIDTH/2, -SCENE_HEIGHT/2,
                               SCENE_WIDTH, SCENE_HEIGHT);
    QTimer* timer = new QTimer;
    timer->setInterval(24);
    connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    timer->start();


}


void Dialog::acceptConnection()
{
    qDebug() << "Accept Connection";
    emit logMessage("Accept Connection");   //emits message to be recieved in client

    incomingClient = server.nextPendingConnection();
    if (gameStarted)
    {
        incomingClient->write("DUMMY,TOOLATE");
        incomingClient->close();
        return;
    }
    incomingClient->setProxy(QNetworkProxy::NoProxy);
    playerSockets.push_back(incomingClient);    //Add socket to player list
    qDebug() << "New player has connected. ";
    //ui->debugBrowser->append("New player has connected. ");
    playerCount++;
    qDebug() << "Current Player Count: " << playerCount;
    //ui->debugBrowser->append("Current Player Count: "+playerCount);

    if(playerCount == 1){
        playerIds[incomingClient] = new PlayerState;
        playerIds[incomingClient]->name = "PLAYER1";
        incomingClient->write("1,PLAYERID\n");
        incomingClient->write("1,HOST\n");
    }
    else if(playerCount == 2){
        playerIds[incomingClient] = new PlayerState;
        playerIds[incomingClient]->name = "PLAYER2";
        incomingClient->write("2,PLAYERID\n");
    }
    else if(playerCount == 3){
        playerIds[incomingClient] = new PlayerState;
        playerIds[incomingClient]->name = "PLAYER3";
        incomingClient->write("3,PLAYERID\n");
    }
    else{
        playerIds[incomingClient] = new PlayerState;
        playerIds[incomingClient]->name = "PLAYER4";
        incomingClient->write("4,PLAYERID\n");
    }
    QString newMsg = "DUMMY,PLAYERCOUNT," + QString::number(playerCount) + "\n";
    sendMsg(newMsg.toUtf8());
    connect(incomingClient, &QTcpSocket::disconnected, this, &Dialog::onDisconnection);   //When client disconnects
    connect(incomingClient, &QTcpSocket::readyRead, this, &Dialog::onReadyRead);    //When client sends a message

    updateNames();
}

void Dialog::onDisconnection()
{
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if(disconnectedSocket)
    {
        if (!gameStarted){
            for (int i = playerSockets.indexOf(disconnectedSocket)+1; i  < playerCount; i++) {
                qDebug() << playerIds[playerSockets[i]]->name << " is swapping to PLAYER" << i;
                playerIds[playerSockets[i]]->name = "PLAYER" + QString::number(i);
                QString newId = QString::number(i)+",PLAYERID\n";
                playerSockets[i]->write(newId.toUtf8());
            }
        }
        playerSockets.removeOne(disconnectedSocket);    //Remove socket from the playerSockets list
        playerIds.remove(disconnectedSocket);  //Remove player from playerIds map
        //playerIds.remove(disconnectedSocket);
        disconnectedSocket->deleteLater();  //Delete the socket
        qDebug() << "Player disconnected";
        //ui->debugBrowser->append("Player disconnected");
        playerCount--;
        qDebug() << "Current Player Count: " << playerCount;
            QString newMsg = "DUMMY,PLAYERCOUNT," + QString::number(playerCount) + "\n";
        sendMsg(newMsg.toUtf8());
        //ui->debugBrowser->append("Current Player Count " + playerCount);
        if (!gameStarted && playerCount > 0)
        {
            playerSockets[0]->write("1,HOST\n");
        }
    }
    else
    {
        qDebug() << "Error in onDisconnection: could not cast sender to QTcpSocket*";
    }
}

void Dialog::onReadyRead()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    QByteArray ba = sock->readAll();
    QString s(ba);
    QStringList messages = s.split("\n", QString::SkipEmptyParts);

    for(QString msg : messages)
    {
        processSingleMsg(msg);
    }
}

void Dialog::processSingleMsg(QString s)
{
    qDebug() << "Processing Msg: " << s;
    QString player = "";
    QString messageType = "";
    QString spellType = "";
    QString cursorx = "";
    QString cursory = "";
    int iter = 0;
    int isSpell = 0;
    for(int i = 0; i < s.length(); i++)
    {
        if(s[i] == ',')
        {
            iter++;
            if(messageType == "SPELL")
            {
                isSpell = 1;

            }
            else if (messageType == "NAME")
            {
                isSpell = 1; // WARNING: Im using this to make name work without redoing our system this late.
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

            else if(s[i]!= ',' && iter == 2 && isSpell == 1)
            {
                spellType.append(s[i]);
            }

            else if(s[i]!= ',' && iter == 3 && isSpell == 1)
            {
                cursorx.append(s[i]);
            }

            else if(s[i]!= ',' && iter == 4 && isSpell == 1)
            {
                cursory.append(s[i]);
            }



    }
    if(player == "DUMMY" || player == "PLAYER1" || player == "PLAYER2" || player == "PLAYER3" || player == "PLAYER4")  //
    {
        player = playerIds[qobject_cast<QTcpSocket*>(sender())]->name;
    }
    //qDebug() << messageType;
    if(messageType ==  "SPELL")
    {
        qDebug() << "Casting: " << spellType;
        if(spellType == "FIREBALL")
        {
            castFireball(player, cursorx, cursory);
        }
        else if(spellType == "MEGAFIREBALL")
        {
            castMegaFireball(player, cursorx, cursory);
        }
        else if(spellType == "BOLT")
        {
            castBolt(player, cursorx, cursory);
        }
        else if(spellType == "HEAL")
        {
            castHealType(player, spellType);
        }
        else if(spellType == "BIGHEAL")
        {
            castHealType(player, spellType);
        }
        else if(spellType == "REGEN")
        {
            castHealType(player, spellType);
        }
        else if (spellType == "SHIELD")
        {
            castShield(player, cursorx, cursory);
        }
    }
    else if (messageType == "START")
    {
        gameStarted = true;
        sendMsg("DUMMY,START\n");
    } else if (messageType == "NAME")
    {
        qDebug() << "Name message received.";
        setName(player,spellType);
        updateNames();
    }

}

void Dialog::sendHealth(QTcpSocket* sock)
{

    QString msg = playerIds[sock]->name + ",HEALTHUPD," + QString::number(playerIds[sock]->health) + "\n";
    sendMsg(msg.toUtf8());
}

void Dialog::setName(QString player, QString NewName)
{
    int playerNum = 0;
    QCharRef numChar = player.back();
    if(numChar == '1' || numChar == '2' || numChar == '3' || numChar == '4')
    {
        playerNum = numChar.digitValue();
    }
    else
    {
        qDebug() << "SetName error with player:" << player;
        return;
    }
    qDebug() << player << " is now named " << NewName;
    if(playerCount >= playerNum)
    {
        playerIds[playerSockets[playerNum-1]]->governmentName = NewName;
    }
}

void Dialog::updateNames()
{
    qDebug() << "Sending Name Updates...";
    for (int i = 0; i < playerCount; i++)
    {
        QTcpSocket* sock = playerSockets[i];
        QString nameMsg;
        if (playerIds[sock]->governmentName != "")
        {
            nameMsg = playerIds[sock]->name + ",NAME," + playerIds[sock]->governmentName + "\n";
        }
        else
        {
            nameMsg = playerIds[sock]->name + ",NAME," + playerIds[sock]->name + "\n";
        }
        sendMsg(nameMsg.toUtf8());
    }
}

void Dialog::sendProjectileUpdates()
{
    sendFireballUpdates();
    sendMegaFireballUpdates();
    sendBoltUpdates();
}

void Dialog::castShield(QString player, QString cursorx, QString cursory)
{
    int xPosition = cursorx.toInt();
    int yPosition = cursory.toInt();
    static int id = 0;
    id++;
    double dx;
    double dy;
    if(player == "PLAYER1")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else if(player == "PLAYER2")
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else if(player == "PLAYER3")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    //qDebug() << xPosition << yPosition;
    shields[id] = new shield;
    shields[id]->x = xPosition;
    shields[id]->y = yPosition;
    int rotation = shields[id]->setRotation(dx, dy);
    qDebug() << "Shield rotation: " << rotation;
    sendMsg(player.toUtf8() + "," + "SPAWNSHIELD" + "," + QByteArray::number(id) + "," + cursorx.toUtf8() + "," + cursory.toUtf8() +  "," + QString::number(rotation).toUtf8() + "\n");
}

void Dialog::castFireball(QString player, QString cursorx, QString cursory)
{
    int xPosition = cursorx.toInt();
    int yPosition = cursory.toInt();
    static int id = 0;
    id++;
    double dx;
    double dy;
    if(player == "PLAYER1")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*2;
        dy /= length;
        dy = dy*2;
    }
    else if(player == "PLAYER2")
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*2;
        dy /= length;
        dy = dy*2;
    }
    else if(player == "PLAYER3")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*2;
        dy /= length;
        dy = dy*2;
    }
    else
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*2;
        dy /= length;
        dy = dy*2;
    }
    //qDebug() << xPosition << yPosition;
    fireballs[id] = new Fireball;
    fireballs[id]->x = xPosition;
    fireballs[id]->y = yPosition;
    fireballs[id]->dx = dx;
    fireballs[id]->dy = dy;
    sendMsg(player.toUtf8() + "," + "SPAWNFIREBALL" + "," + QByteArray::number(id) + "," + cursorx.toUtf8() + "," + cursory.toUtf8() + '\n');
}

void Dialog::castMegaFireball(QString player, QString cursorx, QString cursory)
{
    int xPosition = cursorx.toInt();
    int yPosition = cursory.toInt();
    static int id = 0;
    id++;
    double dx;
    double dy;
    if(player == "PLAYER1")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else if(player == "PLAYER2")
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else if(player == "PLAYER3")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    else
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dy /= length;
    }
    //qDebug() << xPosition << yPosition;
    megafireballs[id] = new MegaFireball;
    megafireballs[id]->x = xPosition;
    megafireballs[id]->y = yPosition;
    megafireballs[id]->dx = dx;
    megafireballs[id]->dy = dy;

    sendMsg(player.toUtf8() + "," + "SPAWNMEGAFIREBALL" + "," + QByteArray::number(id) + "," + cursorx.toUtf8() + "," + cursory.toUtf8() + '\n');
}
void Dialog::castBolt(QString player, QString cursorx, QString cursory)
{
    int xPosition = cursorx.toInt();
    int yPosition = cursory.toInt();
    static int id = 0;
    id++;
    double dx;
    double dy;
    if(player == "PLAYER1")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*4;
        dy /= length;
        dy = dy*4;
    }
    else if(player == "PLAYER2")
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (-SCENE_HEIGHT/2 + PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*4;
        dy /= length;
        dy = dy*4;
    }
    else if(player == "PLAYER3")
    {
        dx = xPosition - (-SCENE_WIDTH/2 + PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*4;
        dy /= length;
        dy = dy*4;
    }
    else
    {
        dx = xPosition - (SCENE_WIDTH/2 - PLAYER_WIDTH/2);
        dy = yPosition - (SCENE_HEIGHT/2 - PLAYER_HEIGHT/2);
        double length = sqrt(dx*dx + dy*dy);
        dx /= length;
        dx = dx*4;
        dy /= length;
        dy = dy*4;
    }
    //qDebug() << xPosition << yPosition;
    bolts[id] = new Bolt;
    bolts[id]->x = xPosition;
    bolts[id]->y = yPosition;
    bolts[id]->dx = dx;
    bolts[id]->dy = dy;

    sendMsg(player.toUtf8() + "," + "SPAWNBOLT" + "," + QByteArray::number(id) + "," + cursorx.toUtf8() + "," + cursory.toUtf8() + '\n');
}
void Dialog::castHealType(QString player, QString type)
{
    int playerNum = 0;
    QCharRef numChar = player.back();
    if(numChar == '1' || numChar == '2' || numChar == '3' || numChar == '4')
    {
        playerNum = numChar.digitValue();
    }
    else
    {
        qDebug() << "CastHealType error with player:" << player << " - " << numChar;
        return;
    }
    qDebug() << player << "cast heal";
    if(playerCount >= playerNum)
    {
        if (type == "REGEN")
        {
            playerIds[playerSockets[playerNum-1]]->castRegen();
            sendHealth(playerSockets[playerNum-1]);
        }
        else if (type == "HEAL")
        {
            playerIds[playerSockets[playerNum-1]]->heal(PlayerState::HEAL_AMT);
            sendHealth(playerSockets[playerNum-1]);
        }
        else if (type == "BIGHEAL")
        {
            playerIds[playerSockets[playerNum-1]]->heal(PlayerState::BIG_HEAL_AMT);
            sendHealth(playerSockets[playerNum-1]);
        }
    }
    
   
}

void Dialog::regenCheck()
{
    for (QTcpSocket* sock : playerIds.keys()) {
        playerIds[sock]->regenTimerCb();
        sendHealth(sock);
    }
}

void Dialog::sendFireballUpdates()
{

    for(int id : fireballs.keys())
    {

        fireballs[id]->x = (fireballs[id]->x) + (fireballs[id]->dx);
        fireballs[id]->y = (fireballs[id]->y) + (fireballs[id]->dy);
        double xPos = fireballs[id]->x;
        double yPos = fireballs[id]->y;
        //qDebug() << fireballs[id]->dx;
        //qDebug() << xPos << yPos;


        bool shieldHit = false;
        for (int ShieldID : shields.keys()) {
            double shieldX = shields[ShieldID]->x;
            double shieldY = shields[ShieldID]->y;
            double distX = xPos - shieldX;
            double distY = yPos - shieldY;
            double distance = sqrt(distX*distX + distY*distY);
            if (distance < 20) {
                sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
                toDeleteFB.push_back(id);
                sendMsg("DUMMY,DELETESHIELD," + QByteArray::number(ShieldID) + '\n');
                toDeleteSH.push_back(ShieldID);
                qDebug() << "Shield hit id=" << ShieldID;
                shieldHit = true;
                break;
            }
        }
        if (shieldHit){
        }
        //qDebug() << fireballs[id]->dx << fireballs[id]->dy;
        //If fireball goes off edge of screen
        else if(xPos < (-SCENE_WIDTH/2) || xPos > (SCENE_WIDTH/2) || yPos < (-SCENE_HEIGHT/2) || yPos > (SCENE_HEIGHT/2))
        {
            sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteFB.push_back(id);
            //qDebug() << "offscreen";
        }

        //If player 1 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteFB.push_back(id);
            qDebug() << "1 hit";
            if(playerCount >= 1){
            playerIds[playerSockets[0]]->health -= 15;
            sendHealth(playerSockets[0]);
            PlayerState* ps = playerIds[playerSockets[0]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(0);
                    }

            }
        }

        //If player 2 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteFB.push_back(id);
            qDebug() << "2 hit";
            if(playerCount >= 2){
            playerIds[playerSockets[1]]->health -= 15;
            sendHealth(playerSockets[1]);
            PlayerState* ps = playerIds[playerSockets[1]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(1);
                    }

            }
        }

        //If player 3 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteFB.push_back(id);
            qDebug() << "3 hit";
            if(playerCount >= 3){
            playerIds[playerSockets[2]]->health -= 15;
            sendHealth(playerSockets[2]);
            PlayerState* ps = playerIds[playerSockets[2]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(2);
                    }

            }
        }

        //If player 4 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteFB.push_back(id);
            qDebug() << "4 hit";
            if(playerCount >= 4){
            playerIds[playerSockets[3]]->health -= 15;
            sendHealth(playerSockets[3]);
            PlayerState* ps = playerIds[playerSockets[3]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(3);
                    }

            }
        }
        else{
            //qDebug() << "update";
        sendMsg("DUMMY,UPDATEFIREBALL," + QByteArray::number(id) + "," + QByteArray::number(xPos) + "," + QByteArray::number(yPos) + '\n');
        }
        for(int deleteid : toDeleteFB)
        {
            qDebug() << "delete called" << deleteid;
            delete fireballs[deleteid];
            fireballs.remove(deleteid);

        }
        for(int deleteid : toDeleteSH)
        {
            delete shields[deleteid];
            shields.remove(deleteid);
        }
        toDeleteSH.clear();
        toDeleteFB.clear();
    }
}

void Dialog::sendMegaFireballUpdates()
{

    for(int id : megafireballs.keys())
    {

        megafireballs[id]->x = (megafireballs[id]->x) + (megafireballs[id]->dx);
        megafireballs[id]->y = (megafireballs[id]->y) + (megafireballs[id]->dy);
        double xPos = megafireballs[id]->x;
        double yPos = megafireballs[id]->y;
        //qDebug() << fireballs[id]->dx << fireballs[id]->dy;
        
        bool shieldHit = false;
        for (int ShieldID : shields.keys()) {
            double shieldX = shields[ShieldID]->x;
            double shieldY = shields[ShieldID]->y;
            double distX = xPos - shieldX;
            double distY = yPos - shieldY;
            double distance = sqrt(distX*distX + distY*distY);
            if (distance < 20) {
                sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
                toDeleteMFB.push_back(id);
                sendMsg("DUMMY,DELETESHIELD," + QByteArray::number(ShieldID) + '\n');
                toDeleteSH.push_back(ShieldID);
                qDebug() << "Shield hit id=" << ShieldID;
                shieldHit = true;
                break;
            }
        }
        if (shieldHit){
        }
        //If fireball goes off edge of screen
        else if(xPos < (-SCENE_WIDTH/2) || xPos > (SCENE_WIDTH/2) || yPos < (-SCENE_HEIGHT/2) || yPos > (SCENE_HEIGHT/2))
        {
            sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteMFB.push_back(id);
        }

        //If player 1 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteMFB.push_back(id);
            if(playerCount >= 1){
            playerIds[playerSockets[0]]->health -= 25;
            sendHealth(playerSockets[0]);
            PlayerState* ps = playerIds[playerSockets[0]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(0);
                    }
            }
        }

        //If player 2 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteMFB.push_back(id);
            if(playerCount >= 2){
            playerIds[playerSockets[1]]->health -= 25;
            sendHealth(playerSockets[1]);
            PlayerState* ps = playerIds[playerSockets[1]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(1);
                    }
            }
        }

        //If player 3 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteMFB.push_back(id);
            if(playerCount >= 3){
            playerIds[playerSockets[2]]->health -= 25;
            sendHealth(playerSockets[2]);
            PlayerState* ps = playerIds[playerSockets[2]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(2);
                    }
            }
        }

        //If player 4 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEMEGAFIREBALL," + QByteArray::number(id) + '\n');
            toDeleteMFB.push_back(id);
            if(playerCount >= 4){
            playerIds[playerSockets[3]]->health -= 25;
            sendHealth(playerSockets[3]);
            PlayerState* ps = playerIds[playerSockets[3]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(3);
                    }
            }
        }
        else{
             sendMsg("DUMMY,UPDATEMEGAFIREBALL," + QByteArray::number(id) + "," + QByteArray::number(xPos) + "," + QByteArray::number(yPos) + '\n');
        }
        for(int deleteid : toDeleteMFB)
        {
            delete megafireballs[deleteid];
            megafireballs.remove(deleteid);
        }
        for(int deleteid : toDeleteSH)
        {
            delete shields[deleteid];
            shields.remove(deleteid);
        }
        toDeleteSH.clear();
        toDeleteMFB.clear();
    }
}

void Dialog::sendBoltUpdates()
{

    for(int id : bolts.keys())
    {

        bolts[id]->x = (bolts[id]->x) + (bolts[id]->dx);
        bolts[id]->y = (bolts[id]->y) + (bolts[id]->dy);
        double xPos = bolts[id]->x;
        double yPos = bolts[id]->y;
        //qDebug() << fireballs[id]->dx << fireballs[id]->dy;


        bool shieldHit = false;
        for (int ShieldID : shields.keys()) {
            double shieldX = shields[ShieldID]->x;
            double shieldY = shields[ShieldID]->y;
            double distX = xPos - shieldX;
            double distY = yPos - shieldY;
            double distance = sqrt(distX*distX + distY*distY);
            if (distance < 20) {
                sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
                toDeleteB.push_back(id);
                sendMsg("DUMMY,DELETESHIELD," + QByteArray::number(ShieldID) + '\n');
                toDeleteSH.push_back(ShieldID);
                qDebug() << "Shield hit id=" << ShieldID;
                shieldHit = true;
                break;
            }
        }
        if (shieldHit){
        }

        //If bolt goes off edge of scene
        else if(xPos < (-SCENE_WIDTH/2) || xPos > (SCENE_WIDTH/2) || yPos < (-SCENE_HEIGHT/2) || yPos > (SCENE_HEIGHT/2))
        {
            sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
            toDeleteB.push_back(id);

        }

        //If player 1 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
            toDeleteB.push_back(id);
            if(playerCount >= 1){
            playerIds[playerSockets[0]]->health -= 7;
            sendHealth(playerSockets[0]);
            PlayerState* ps = playerIds[playerSockets[0]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(0);
                    }
            }
        }

        //If player 2 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos < ((-SCENE_HEIGHT/2) + 90))
        {
            sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
            toDeleteB.push_back(id);
            if(playerCount >= 2){
            playerIds[playerSockets[1]]->health -= 7;
            sendHealth(playerSockets[1]);
            PlayerState* ps = playerIds[playerSockets[1]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(1);
                    }
            }
        }

        //If player 3 hit
        else if(xPos < ((-SCENE_WIDTH/2) + 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
            toDeleteB.push_back(id);
            if(playerCount >= 3){
            playerIds[playerSockets[2]]->health -= 7;
            sendHealth(playerSockets[2]);
            PlayerState* ps = playerIds[playerSockets[2]];
            if (ps->getHealth() <= 0) {
                 // 0 = player 1, 1 = player 2, etc.
                 handlePlayerDeath(2);
             }
            }
        }

        //If player 4 hit
        else if(xPos > ((SCENE_WIDTH/2) - 90) && yPos > ((SCENE_HEIGHT/2) - 140))
        {
            sendMsg("DUMMY,DELETEBOLT," + QByteArray::number(id) + '\n');
            toDeleteB.push_back(id);
            if(playerCount >= 4){
            playerIds[playerSockets[3]]->health -= 7;
            sendHealth(playerSockets[3]);
            PlayerState* ps = playerIds[playerSockets[3]];
            if (ps->getHealth() <= 0) {
                        // 0 = player 1, 1 = player 2, etc.
                        handlePlayerDeath(3);
                    }
            }
        }
        else{
            sendMsg("DUMMY,UPDATEBOLT," + QByteArray::number(id) + "," + QByteArray::number(xPos) + "," + QByteArray::number(yPos) + '\n');
        }
        for(int deleteid : toDeleteB)
        {
            delete bolts[deleteid];
            bolts.remove(deleteid);

        }
        for(int deleteid : toDeleteSH)
        {
            delete shields[deleteid];
            shields.remove(deleteid);
        }
        toDeleteSH.clear();
        toDeleteB.clear();
    }
}
void Dialog::sendMsg(QByteArray ba)
{
    //qDebug() <<  "Sending Msg: " << ba;
    for(QTcpSocket* sock : playerSockets)
    {
    sock->write(ba);
    }
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::handlePlayerDeath(int index)
{
    if (index < 0 || index >= playerSockets.size())
        return;

    QTcpSocket* sock = playerSockets[index];
    PlayerState* ps = playerIds.value(sock, nullptr);
    if (!ps)
        return;

    int id = index + 1;

    QByteArray deadMsg = QByteArray::number(id) + ",DEATH\n";
    sendMsg(deadMsg);

    //sock->disconnectFromHost();
}
