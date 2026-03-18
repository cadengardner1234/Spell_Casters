#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>
#include <QString>
#include <QMap>
#include <../defs.h>
#include "shieldvisual.h"
#include "fireballvisual.h"
#include "megafireballvisual.h"
#include "boltvisual.h"
#include <QTimer>
#include <QKeyEvent>
#include <QNetworkProxy> // Lab Network Fix
#include <QKeyEvent>
#include <crosshair.h>
#include <QTimer>
#include <QGraphicsEllipseItem>

#define MAIN_MENU_IDX   0
#define LOBBY_SCR_IDX   1
#define GAME_SCR_IDX    2
#define SETTINGS_IDX    3

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    //bool eventFilter(QObject* watched, QEvent* event);

private:
    Ui::Dialog *ui;
    QTcpSocket* socket;
    QGraphicsScene* scene;
    QGraphicsItem* bg;
    Crosshair *crosshair = nullptr;
    void spawnCrosshair();
    QPushButton *cursorSpeed = nullptr;
    QSqlDatabase sl;
    QSqlDatabase lb;
    QSqlQueryModel qm;
    QMap<int, shieldvisual* > shields;
    QMap<int, FireballVisual* > fireballs;
    QMap<int, MegaFireballVisual* > megafireballs;
    QMap<int, BoltVisual* > bolts;
    void spawnShield(QString, QString, QString, QString, QString);
    void deleteShield(QString);
    void spawnFireball(QString, QString, QString, QString);
    void updateFireball(QString, QString, QString);
    void deleteFireball(QString);
    void spawnMegaFireball(QString, QString, QString, QString);
    void updateMegaFireball(QString, QString, QString);
    void deleteMegaFireball(QString);
    void spawnBolt(QString, QString, QString, QString);
    void updateBolt(QString, QString, QString);
    void deleteBolt(QString);
    void handleRemotePlayerDeath(int deadId);
    void recordWin();
    void recordLoss();
    void updateLobbyCount(int);
    void updateHealthBar(int player, int health);

    bool gameIsStarted = false;
    int deathsSeen = 0;
    QString playerName;
    int lobbyCount = 0;
    int gamePlayerCount = 0;   // players at the start of this game



    void processSingleMsg(QString);

    int clientId;

    // _____ Player Pixmaps _____ //

     QGraphicsPixmapItem* player1;
     QGraphicsPixmapItem* player2;
     QGraphicsPixmapItem* player3;
     QGraphicsPixmapItem* player4;

     QGraphicsPixmapItem* island1;
     QGraphicsPixmapItem* island2;
     QGraphicsPixmapItem* island3;
     QGraphicsPixmapItem* island4;

     // _________________________ //



private slots:
    void processMsg();
    void connectToServer();

    void initializeScene();
    // Socket State Slot Functions
    void SocketError();
    void SocketConnected();
    void SocketDisconnected();
    void sendStart();
    void startGame();
    void openSettings();
    void leaveGame();
    //settings button functions
    void statsAppend();
    void howtoAppend();
    void spellLibAppend();
    void configAppend();
    void returnToMenu();
    //void clearLayout(QLayout *layout);

    // Functions for Handling Sprite Implementations

    void playerJoinPixmap();
    void openSpellbook();

    //Player actions
    void castSpell(QPointF);

    // _____________________________________________
    void sendMsg(QByteArray);

};

#endif // DIALOG_H
