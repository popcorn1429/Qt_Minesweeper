#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <tilelabel.h>

namespace Ui {
class MainWindow;
}

class GameEngine;
enum class TILE_OPER;

enum class GameState {
    NOT_INITIALIZED,
    RUNNING,
    PAUSE,
    MISSION_FAILED,
    MISSION_COMPLETED
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resetGame(int iRowNum, int iColNum, int iMineNum);
    void resetTiles(int iRowNum, int iColNum);

public slots:
    //deal signals from menu
    void onActionRestart();
    void onActionEasyMode();
    void onActionMiddleMode();
    void onActionHardMode();
    void onActionCustomMode();

    //deal signals from TileLabel
    void onTileLeftClicked(int iRow, int iCol);
    void onTileRightClicked(int iRow, int iCol);
    void onTileBothClicked(int iRow, int iCol);
    void onTilePressed(int iRow, int iCol);

    //deal signals from GameEngine
    void onBoom(int iTile);
    void onUpdateTileState(int iTile, TILE_OPER iOper, int iMine);
    void onComplete();

    //timer
    void showSeconds();
    void checkGameState();

private:
    Ui::MainWindow *ui;
    QVector<TileLabel*> tiles;

    GameState       gameState;
    GameEngine     *engine;
    int             curColNum;
    int             curRowNum;
    int             curMineNum;

    QTimer         *timer;
    int             elapseSeconds;

    QTimer         *checkTimer;
};

#endif // MAINWINDOW_H
