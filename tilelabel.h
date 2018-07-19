#ifndef TILELABEL_H
#define TILELABEL_H
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <ctime>
using std::clock_t;
using std::clock;

class TileLabel : public QLabel
{
    Q_OBJECT
public:
    TileLabel(QWidget* parent, int iRow, int iCol);

    static void setRowNum(int iRowNum) { s_iMaxRow = iRowNum; }
    static void setColNum(int iColNum) { s_iMaxCol = iColNum; }

public:
    static int s_iMaxCol;
    static int s_iMaxRow;

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void tileBothPressed(int iRow, int iCol);
    void tileBothReleased(int iRow, int iCol);

    void tileLeftClicked(int iRow, int iCol);
    void tileRightClicked(int iRow, int iCol);

private:
    //tile position
    int m_iCol;
    int m_iRow;

    //button event
    clock_t m_tPress;
    clock_t m_tRelease;
    bool m_bLeftPressed;
    bool m_bRightPressed;
    bool m_bPartlyReleased;  //both buttons pressed before, and now one of them released

};

#endif // TILELABEL_H
