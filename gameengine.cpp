#include "gameengine.h"
#include <cstdlib>
using std::rand;

void GameEngine::newGame(int iRowNum, int iColNum, int iMineNum) {
    qDebug("GameEngine::newGame %d, %d, %d", iRowNum, iColNum, iMineNum);
    m_iRowNum = iRowNum;
    m_iColNum = iColNum;
    m_iMineNum = iMineNum;
    m_vMineStateOfTiles.resize(m_iRowNum * m_iColNum);
    m_vOperStateOfTiles.resize(m_iRowNum * m_iColNum);
    for (int iTile = 0; iTile < m_vMineStateOfTiles.size(); ++iTile) {
        m_vMineStateOfTiles[iTile] = NO_MINE_AROUND;
        m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_COVERED;
    }
    m_bInitialized = false;
    m_iUncoverTiles = 0;
}

void GameEngine::init(int iTile) {
    //initialize
    generateMines(iTile);
    updateAroundMines();
    m_bInitialized = true;

    //the first click on a tile
    tileLeftClicked(iTile);
}

void GameEngine::generateMines(int iTile) {
    //set mines randomly
    //note that pos(iRow, iCol) should not have a mine!
    int iMines = totalMineNum();
    int iTiles = totalRowNum() * totalColNum() - 1;
    int i = 0;
    while (iMines > 0) {
        //make sure that current tile has no mine!
        if (i == iTile) {
            ++i;
            continue;
        }

        if (rand()%iTiles < iMines) {
            if (!isValidTile(i)) {
                qDebug("generateMines ERROR!!%d too large, vector size is %zu.!!!", i, m_vMineStateOfTiles.size());
                break;
            }
            m_vMineStateOfTiles[i] = MINE_HERE;
            --iMines;
        }
        --iTiles;
        ++i;
    }
}

void GameEngine::updateAroundMines() {
    qDebug("updateAroundMines");
    int iTiles = totalRowNum() * totalColNum();
    for (int iTile = 0; iTile < iTiles; ++iTile) {
        if (!isValidTile(iTile)) {
            qDebug("updateAroundMines ERROR %d out of range %zu!!", iTile, m_vMineStateOfTiles.size());
            break;
        }

        if (m_vMineStateOfTiles[iTile] != MINE_HERE) {
            m_vMineStateOfTiles[iTile] = getMineNumAroundTile(iTile);
        }
    }
}

void GameEngine::tileLeftClicked(int iTile) {
    if (!isValidTile(iTile))
        return;

    //if this tile is already uncovered, nothing to do
    if (TILE_OPER::TILE_UNCOVERED == m_vOperStateOfTiles[iTile]) {
        return;
    }

    //open this tile, oh! boom!!
    m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_UNCOVERED;
    if (m_vMineStateOfTiles[iTile] == MINE_HERE) {
        emit boom(iTile); //game over !!
        return;
    }

    //open this tile, and spread tiles around if neccesary, and check game complete or not.
    if (m_vMineStateOfTiles[iTile] == NO_MINE_AROUND) {
        emit updateTileState(iTile, TILE_OPER::TILE_UNCOVERED, NO_MINE_AROUND);
        spreadTiles(iTile);
    }
    else {
        emit updateTileState(iTile, TILE_OPER::TILE_UNCOVERED, m_vMineStateOfTiles[iTile]);
    }
    ++m_iUncoverTiles;

    if (checkFinish()) {
        emit complete();
    }
}

void GameEngine::tileRightClicked(int iTile) {
    if (!isValidTile(iTile))
        return;

    //if this tile is already uncovered, nothing to do
    if (TILE_OPER::TILE_UNCOVERED == m_vOperStateOfTiles[iTile]) {
        return;
    }

    if (TILE_OPER::TILE_MARKED == m_vOperStateOfTiles[iTile]) {
        m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_COVERED;
        emit updateTileState(iTile, TILE_OPER::TILE_COVERED, m_vMineStateOfTiles[iTile]);
    }
    else {
        m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_MARKED;
        emit updateTileState(iTile, TILE_OPER::TILE_MARKED, m_vMineStateOfTiles[iTile]);
    }
}

void GameEngine::tileBothClicked(int iTile) {
    if (!isValidTile(iTile))
        return;

    //current tile must be uncovered!
    if (TILE_OPER::TILE_UNCOVERED != m_vOperStateOfTiles[iTile]) {
        return;
    }

    trySpreadTilesAround(iTile);
}

//when current tile has no mine and all tiles around it have no mine. call this function.
void GameEngine::spreadTiles(int iTile) {
    //surely, current tile has no mine.(otherwise this function would not be called)
    //check every tile around this one, if the tile has no mine and 0 mines around it, then spread this tile.
    //note that this function is recursive.
    // iTile-m_iColNum-1, iTile-m_iColNum, iTile-m_iColNum+1
    // iTile-1,            iTile,          iTile+1
    // iTile+m_iColNum-1, iTile+m_iColNum, iTile+m_iColNum+1

    //1 left_up
    checkAndSpread(iTile-m_iColNum-1);
    //2 middle_up
    checkAndSpread(iTile-m_iColNum);
    //3 right_up
    checkAndSpread(iTile-m_iColNum+1);
    //4 left_middle
    checkAndSpread(iTile-1);
    //5 right_middle
    checkAndSpread(iTile+1);
    //6 left_down
    checkAndSpread(iTile+m_iColNum-1);
    //7 middle_down
    checkAndSpread(iTile+m_iColNum);
    //8 right_down
    checkAndSpread(iTile+m_iColNum+1);
}

void GameEngine::checkAndSpread(int iTile) {
    //qDebug("checkAndSpread(%d of %zu [%zu]) 0", iTile, m_vMineStateOfTiles.size(), m_vOperStateOfTiles.size());
    if (!isValidTile(iTile)) {
        return;
    }

    //qDebug("checkAndSpread(%d of %zu [%zu]) 1", iTile, m_vMineStateOfTiles.size(), m_vOperStateOfTiles.size());
    //if tile already uncovered or it's marked, do nothing
    if (TILE_OPER::TILE_COVERED != m_vOperStateOfTiles[iTile]) {
        return;
    }

    int iMineAround = m_vMineStateOfTiles[iTile];
    if (iMineAround == MINE_HERE) {
        emit boom(iTile);
        return;
    }

    m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_UNCOVERED;
    emit updateTileState(iTile, TILE_OPER::TILE_UNCOVERED, iMineAround);
    ++m_iUncoverTiles;
    if (iMineAround == NO_MINE_AROUND) {
        spreadTiles(iTile);
    }
}

void GameEngine::trySpreadTilesAround(int iTile) {
    if (!isValidTile(iTile))
        return;

    int iMineNum = m_vMineStateOfTiles[iTile];
    int iMarkNum = (isMarkTile(iTile-m_iColNum-1)?1:0)
            + (isMarkTile(iTile-m_iColNum)?1:0)
            + (isMarkTile(iTile-m_iColNum+1)?1:0)
            + (isMarkTile(iTile-1)?1:0)
            + (isMarkTile(iTile+1)?1:0)
            + (isMarkTile(iTile+m_iColNum-1)?1:0)
            + (isMarkTile(iTile+m_iColNum)?1:0)
            + (isMarkTile(iTile+m_iColNum+1)?1:0);

    if (iMarkNum == iMineNum) {
        openAroundTilesCovered(iTile);
    }
}

void GameEngine::openAroundTilesCovered(int iTile) {
    openCoveredTile(iTile-m_iColNum-1);
    openCoveredTile(iTile-m_iColNum);
    openCoveredTile(iTile-m_iColNum+1);
    openCoveredTile(iTile-1);
    openCoveredTile(iTile+1);
    openCoveredTile(iTile+m_iColNum-1);
    openCoveredTile(iTile+m_iColNum);
    openCoveredTile(iTile+m_iColNum+1);
}

void GameEngine::openCoveredTile(int iTile) {
    if (!isValidTile(iTile))
        return;

    if (TILE_OPER::TILE_COVERED == m_vOperStateOfTiles[iTile]) {
        m_vOperStateOfTiles[iTile] = TILE_OPER::TILE_UNCOVERED;
        if (m_vMineStateOfTiles[iTile] == MINE_HERE) {
            emit boom(iTile); //game over !!
            return;
        }
        emit updateTileState(iTile, TILE_OPER::TILE_UNCOVERED, m_vMineStateOfTiles[iTile]);
    }
}

int GameEngine::getMineNumAroundTile(int iTile) const {
    int sum = (isMineTile(iTile-m_iColNum-1)?1:0)
            + (isMineTile(iTile-m_iColNum)?1:0)
            + (isMineTile(iTile-m_iColNum+1)?1:0)
            + (isMineTile(iTile-1)?1:0)
            + (isMineTile(iTile+1)?1:0)
            + (isMineTile(iTile+m_iColNum-1)?1:0)
            + (isMineTile(iTile+m_iColNum)?1:0)
            + (isMineTile(iTile+m_iColNum+1)?1:0);
    return sum;
}




