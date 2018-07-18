#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameengine.h"
#include "customdialog.h"
#include <QMessageBox>
#include <QTimer>

//easy mode:
const int EASY_ROWNUM = 10;
const int EASY_COLNUM = 20;
const int EASY_MINENUM = 40;
//middle mode:
const int MIDDLE_ROWNUM = 15;
const int MIDDLE_COLNUM = 25;
const int MIDDLE_MINENUM = 90;
//hard mode:
const int HARD_ROWNUM = 25;
const int HARD_COLNUM = 40;
const int HARD_MINENUM = 300;
//custom mode limitations:
const int CUSTOM_ROWNUM_MIN = 5;
const int CUSTOM_COLNUM_MIN = 10;
const int CUSTOM_ROWNUM_MAX = 40;
const int CUSTOM_COLNUM_MAX = 80;

//window:
const int WINDOW_MARGIN = 12;

//tile size:
const int TILE_WIDTH = 30;
const int TILE_HEIGHT = TILE_WIDTH;

//tile margin
const int TILE_MARGIN = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gameState(GameState::NOT_INITIALIZED),
    elapseSeconds(0) {
    ui->setupUi(this);
    //connect menu with window
    connect(ui->actionRestart, &QAction::triggered, this, &MainWindow::onActionRestart);
    connect(ui->actionEasy, &QAction::triggered, this, &MainWindow::onActionEasyMode);
    connect(ui->actionMiddle, &QAction::triggered, this, &MainWindow::onActionMiddleMode);
    connect(ui->actionHard, &QAction::triggered, this, &MainWindow::onActionHardMode);
    connect(ui->actionCustom, &QAction::triggered, this, &MainWindow::onActionCustomMode);

    //lcd
    ui->lcdNumber_timer->setSegmentStyle(QLCDNumber::Flat);
    QPalette plt = ui->lcdNumber_timer->palette();
    plt.setColor(QPalette::Normal,QPalette::WindowText,Qt::red);
    ui->lcdNumber_timer->setPalette(plt);
    ui->lcdNumber_mines->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber_mines->setPalette(plt);

    engine = new GameEngine(this);
    //connect engine with window
    connect(engine, &GameEngine::boom, this, &MainWindow::onBoom);
    connect(engine, &GameEngine::updateTileState, this, &MainWindow::onUpdateTileState);
    connect(engine, &GameEngine::complete, this, &MainWindow::onComplete);

    //timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::showSeconds);
    timer->stop();
    checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, &MainWindow::checkGameState);
    checkTimer->start(50);

    //default game mode (easy mode)
    onActionEasyMode();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resetGame(int iRowNum, int iColNum, int iMineNum) {
    //view
    setFixedSize(TILE_WIDTH*iColNum + TILE_MARGIN*iColNum-TILE_MARGIN + WINDOW_MARGIN*2,
                 WINDOW_MARGIN*2 + 110 + TILE_HEIGHT*iRowNum + TILE_MARGIN*iRowNum - TILE_MARGIN);
    ui->lcdNumber_mines->display(iMineNum);
    timer->stop();
    elapseSeconds = 0;
    ui->lcdNumber_timer->display(elapseSeconds);
    resetTiles(iRowNum, iColNum);

    gameState = GameState::NOT_INITIALIZED;

    //engine
    engine->newGame(iRowNum, iColNum, iMineNum);
}

void MainWindow::resetTiles(int iRowNum, int iColNum) {
    //clear old
    QList<QLabel*> lstTiles = ui->frame->findChildren<QLabel*>();
    foreach (QLabel* label, lstTiles) {
        delete label;
    }
    lstTiles.clear();
    tiles.clear();

    //add new tiles
    TileLabel::setRowNum(iRowNum);
    TileLabel::setColNum(iColNum);
    for (int iRow = 0; iRow < iRowNum; ++iRow) {
        for (int iCol = 0; iCol < iColNum; ++iCol) {
            TileLabel* tile = new TileLabel(ui->frame, iRow, iCol);
            tile->setGeometry(iCol*TILE_MARGIN + iCol*TILE_WIDTH,
                              iRow*TILE_MARGIN + iRow*TILE_HEIGHT,
                              TILE_WIDTH, TILE_HEIGHT);
            tile->setStyleSheet(QStringLiteral("QLabel {border-radius:4px; border:2px solid #5F92B2; background-color: #a6ddff;}"));
            tile->setAlignment(Qt::AlignCenter);
            //connect tile with window
            connect(tile, &TileLabel::tileLeftClicked, this, &MainWindow::onTileLeftClicked);
            connect(tile, &TileLabel::tileBothClicked, this, &MainWindow::onTileBothClicked);
            connect(tile, &TileLabel::tileRightClicked, this, &MainWindow::onTileRightClicked);

            tiles.push_back(tile);
        }
    }

    lstTiles = ui->frame->findChildren<QLabel*>();
    foreach (QLabel* label, lstTiles) {
        label->show();
    }
    lstTiles.clear();
}

void MainWindow::onActionRestart() {
    resetGame(curRowNum, curColNum, curMineNum);
}

void MainWindow::onActionEasyMode() {
    curRowNum = EASY_ROWNUM;
    curColNum = EASY_COLNUM;
    curMineNum = EASY_MINENUM;
    resetGame(curRowNum, curColNum, curMineNum);
}

void MainWindow::onActionMiddleMode() {
    curRowNum = MIDDLE_ROWNUM;
    curColNum = MIDDLE_COLNUM;
    curMineNum = MIDDLE_MINENUM;
    resetGame(curRowNum, curColNum, curMineNum);
}

void MainWindow::onActionHardMode() {
    curRowNum = HARD_ROWNUM;
    curColNum = HARD_COLNUM;
    curMineNum = HARD_MINENUM;
    resetGame(curRowNum, curColNum, curMineNum);
}

void MainWindow::onActionCustomMode() {
    CustomDialog dlg(this);
    if (QDialog::Accepted == dlg.exec()) {
        dlg.getCustomConf(curRowNum, curColNum, curMineNum);
        resetGame(curRowNum, curColNum, curMineNum);
    }
}

void MainWindow::onTileLeftClicked(int iRow, int iCol) {
    if (GameState::RUNNING != gameState && GameState::NOT_INITIALIZED != gameState) {
        return ;
    }

    int iTile = iRow * engine->totalColNum() + iCol;
    //first tile click make the game initialized.
    if (!engine->initialized()) {
        engine->init(iTile);
        //reset timer
        elapseSeconds = 0;
        timer->start(1000);
        gameState = GameState::RUNNING;
    }
    else {
        qDebug("onTileLeftClicked %d,%d", iRow, iCol);
        engine->tileLeftClicked(iTile);
    }
}

void MainWindow::onTileRightClicked(int iRow, int iCol) {
    if (GameState::RUNNING != gameState) {
        return ;
    }
    int iTile = iRow * engine->totalColNum() + iCol;
    engine->tileRightClicked(iTile);
}

void MainWindow::onTileBothClicked(int iRow, int iCol) {
    if (GameState::RUNNING != gameState) {
        return ;
    }

    int iTile = iRow * engine->totalColNum() + iCol;
    engine->tileBothClicked(iTile);
}

void MainWindow::onBoom(int iTile) {
    TileLabel* tile = tiles[iTile];
    tile->clear();
    tile->setStyleSheet(QStringLiteral("QLabel {border-radius:4px; border:2px solid #5F92B2; background-image: url(:/game/pic/boom.png);}"));
    gameState = GameState::MISSION_FAILED;
}

void MainWindow::onUpdateTileState(int iTile, TILE_OPER iOper, int iMine) {
    TileLabel* tile = tiles[iTile];
    switch (iOper) {
    case TILE_OPER::TILE_UNCOVERED:
        tile->setStyleSheet(QStringLiteral("QLabel {border-radius:4px; border:2px solid #5F92B2; background-color: #95e3a7;}"));
        if (iMine != NO_MINE_AROUND) {
            //uncover with no number
            tile->setText("<font size=\"5\" color=\"red\">" + QString::number(iMine) + "</font>");
        }
        break;
    case TILE_OPER::TILE_MARKED:
        tile->setStyleSheet(QStringLiteral("QLabel {border-radius:4px; border:2px solid #5F92B2; background-color: #a6ddff;}"));
        tile->setText("<font size=\"5\" color=\"yellow\">?</font>");
        break;
    case TILE_OPER::TILE_COVERED:
        tile->setStyleSheet(QStringLiteral("QLabel {border-radius:4px; border:2px solid #5F92B2; background-color: #a6ddff;}"));
        tile->clear();
        break;
    default:
        break;
    }
}

void MainWindow::onComplete() {
    gameState = GameState::MISSION_COMPLETED;
}

void MainWindow::showSeconds() {
    ++elapseSeconds;
    ui->lcdNumber_timer->display(elapseSeconds);
}

void MainWindow::checkGameState() {
    switch (gameState) {
    case GameState::NOT_INITIALIZED:
        if (timer->isActive()) {
            timer->stop();
        }
        break;
    case GameState::RUNNING:
        if (!timer->isActive()) {
            timer->start(1000);
        }
        break;
    case GameState::PAUSE:
        if (timer->isActive()) {
            timer->stop();
        }
        break;
    case GameState::MISSION_COMPLETED:
        //change state :
        gameState = GameState::PAUSE;
        //show a msg box
        if (QMessageBox::Yes == QMessageBox::warning(this, QStringLiteral("Congratulations"), QStringLiteral(":-D Well done, soldier! Restart new Game?"), QMessageBox::Yes, QMessageBox::No)) {
            gameState = GameState::NOT_INITIALIZED;
            resetGame(curRowNum,curColNum,curMineNum);
        }
        else {
            qApp->quit();
        }
        break;
    case GameState::MISSION_FAILED:
        //change state :
        gameState = GameState::PAUSE;
        //show a msg box
        if (QMessageBox::Yes == QMessageBox::warning(this, QStringLiteral("Game Over"), QStringLiteral(":-( Now you're dead... Restart?"), QMessageBox::Yes, QMessageBox::No)) {
            gameState = GameState::NOT_INITIALIZED;
            resetGame(curRowNum,curColNum,curMineNum);
        }
        else {
            qApp->quit();
        }
        break;
    default:
        break;
    }
}

