#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#include <QObject>
#include <QSet>
#include <QVector>

//mine state of tiles
const int MINE_HERE      = -1;
const int NO_MINE_AROUND = 0;  //this tile has no mine ,and tiles around it have no mine.
//operate state of tiles
enum class TILE_OPER {
    TILE_UNCOVERED,
    TILE_MARKED,
    TILE_COVERED
};


class GameEngine : public QObject
{
    Q_OBJECT

public:
    GameEngine(QObject* parent = NULL) :
        m_iRowNum(0), m_iColNum(0), m_iMineNum(0),
        m_bInitialized(false), m_iUncoverTiles(0), QObject(parent) {}

    GameEngine(int iRowNum, int iColNum, int iMineNum, QObject* parent = NULL) :
        m_iRowNum(iRowNum), m_iColNum(iColNum), m_iMineNum(iMineNum),
        m_bInitialized(false), m_iUncoverTiles(0), QObject(parent) {
        m_vMineStateOfTiles.resize(m_iRowNum * m_iColNum);
        m_vOperStateOfTiles.resize(m_iRowNum * m_iColNum);
    }

    //when player select/diy a game, call this.
    void newGame(int iRowNum, int iColNum, int iMineNum);

    inline int totalRowNum() const { return m_iRowNum; }
    inline int totalColNum() const { return m_iColNum; }
    inline int totalMineNum() const { return m_iMineNum; }

    //when player click the first tile , call this function to initialize the game.
    void init(int iTile);
    void generateMines(int iTile);  //generate mines randomly
    void updateAroundMines();       //calc mine state of every tiles
    //check that is game initialized.
    inline bool initialized() const { return m_bInitialized; }

    void tileLeftClicked(int iTile);
    void tileRightClicked(int iTile);
    void tileBothClicked(int iTile);

    inline bool isMineTile(int iTile) const { return (isValidTile(iTile) && MINE_HERE == m_vMineStateOfTiles[iTile]); }
    inline bool isMarkTile(int iTile) const { return (isValidTile(iTile) && TILE_OPER::TILE_MARKED == m_vOperStateOfTiles[iTile]); }

private:
    void spreadTiles(int iTile);  //spread tiles safyly
    void checkAndSpread(int iTile);

    void trySpreadTilesAround(int iTile); //spread tiles forcely
    void openAroundTilesCovered(int iTile);
    void openCoveredTile(int iTile);

    inline bool checkFinish() const { return m_iMineNum + m_iUncoverTiles == m_vMineStateOfTiles.size(); }

    inline bool isValidTile(int iTile) const { return (0 <= iTile && iTile < m_vMineStateOfTiles.size()); }
    int  getMineNumAroundTile(int iTile) const;

signals:
    void boom(int iTile);
    void updateTileState(int iTile, TILE_OPER iOper, int iMine);
    void complete();
private:
    int m_iRowNum;
    int m_iColNum;
    int m_iMineNum;
    bool m_bInitialized;

    int m_iUncoverTiles;
    QVector<int> m_vMineStateOfTiles; //every tile has a value to show there're how many mines around it
                             //if this tile itself has a mine, then its value is -1
                             //if this tile has no mine, and all tiles around it (8 tiles normally) have no mine. then its value is 0
    QVector<TILE_OPER> m_vOperStateOfTiles; //if tile is uncovered
};

#endif // GAMEENGINE_H
