#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QList>
#include <QNetworkInterface>
#include <QtDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QVector>
#include <algorithm>
#include <QGraphicsScene>
#include <QMap>
#include <QRandomGenerator>
#include <QStringList>
#include <QTimer>
#include <../defs.h>
#include "shield.h"
#include "fireball.h"
#include "megafireball.h"
#include "bolt.h"
#include "playerstate.h"
#include <QNetworkProxy> // Lab Network Fix


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    bool gameStarted = false;

    ~Dialog();

signals:
    void logMessage(const QString &msg);    //message logger

private:
    Ui::Dialog *ui;
    QTcpServer server;
    QTcpSocket* incomingClient = nullptr;
    QVector<QTcpSocket*> playerSockets;
    QVector<int> toDeleteSH;
    QVector<int> toDeleteFB;
    QVector<int> toDeleteMFB;
    QVector<int> toDeleteB;
    QGraphicsScene* scene = nullptr;
    int playerCount;
    QMap<int, shield* > shields;
    QMap<int, Fireball* > fireballs;
    QMap<int, MegaFireball*> megafireballs;
    QMap<int, Bolt* > bolts;
    QMap<QTcpSocket*, PlayerState*> playerIds;
    void castShield(QString player, QString cursorx, QString cursory);
    void castFireball(QString, QString,QString);
    void castMegaFireball(QString, QString, QString);
    void castBolt(QString, QString,QString);
    void castHealType(QString, QString);
    void processSingleMsg(QString);
    void sendFireballUpdates();
    void sendMegaFireballUpdates();
    void sendBoltUpdates();
    void handlePlayerDeath(int index);
    void setName(QString player, QString NewName);
    void updateNames();
    void sendHealth(QTcpSocket* sock);


private slots:
    void initializeScene();
    void acceptConnection();
    void onDisconnection();
    void onReadyRead();
    void sendMsg(QByteArray);
    void sendProjectileUpdates();
    void regenCheck();
};

#endif // DIALOG_H
