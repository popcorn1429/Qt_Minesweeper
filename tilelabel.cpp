#include "tilelabel.h"

const int click_interval = 250;

int TileLabel::s_iMaxRow = 0;
int TileLabel::s_iMaxCol = 0;

TileLabel::TileLabel(QWidget *parent, int iRow, int iCol)
    : QLabel(parent), m_iRow(iRow), m_iCol(iCol),
      m_tPress(0), m_tRelease(0),
      m_bLeftPressed(false), m_bRightPressed(false),
      m_bPartlyReleased(false)
{
}

void TileLabel::mousePressEvent(QMouseEvent *ev) {
    Qt::MouseButtons pressedBtns = ev->buttons();
    //qDebug("TileLabel mouse press %d ", pressedBtns);
    m_bLeftPressed = (0 != (pressedBtns & Qt::LeftButton));
    m_bRightPressed = (0 != (pressedBtns & Qt::RightButton));

    if (m_bRightPressed && m_bLeftPressed) {
        //qDebug("both pressed");
        m_bPartlyReleased = false;
    }

    m_tPress = clock();

    QLabel::mousePressEvent(ev);
}

void TileLabel::mouseReleaseEvent(QMouseEvent *ev) {
    //qDebug("TileLabel mouse release");
    m_tRelease = clock();
    //qDebug("TileLabel mouse release %ld - %ld = %ld ", m_tRelease, m_tPress, m_tRelease-m_tPress);
    if ((m_tRelease - m_tPress)*1000/CLOCKS_PER_SEC < click_interval) {
        //qDebug("TileLabel mouse clicked!  %d", ev->button());
        if (Qt::LeftButton == ev->button()) {
            if (!m_bRightPressed) {
                qDebug("emit tileLeftClicked");
                QLabel::mouseReleaseEvent(ev);
                m_tPress = m_tRelease;
                m_tRelease = 0;
                emit tileLeftClicked(m_iRow, m_iCol);
            }
            else if (m_bPartlyReleased) {
                //qDebug("click left & right");
                QLabel::mouseReleaseEvent(ev);
                m_tPress = m_tRelease;
                m_tRelease = 0;
                emit tileBothClicked(m_iRow, m_iCol);
            }
            else {
                //qDebug("release partly");
                m_bPartlyReleased = true;
                m_tPress = m_tRelease;
                m_tRelease = 0;
                QLabel::mouseReleaseEvent(ev);
            }
        }
        else if (ev->button() == Qt::RightButton) {
            if (!m_bLeftPressed) {
                //qDebug("click right");
                QLabel::mouseReleaseEvent(ev);
                m_tPress = m_tRelease;
                m_tRelease = 0;
                emit tileRightClicked(m_iRow, m_iCol);
            }
            else if (m_bPartlyReleased) {
                //qDebug("click left & right");
                m_tPress = m_tRelease;
                m_tRelease = 0;
                QLabel::mouseReleaseEvent(ev);
                emit tileBothClicked(m_iRow, m_iCol);
            }
            else {
                //qDebug("release partly");
                m_bPartlyReleased = true;
                m_tPress = m_tRelease;
                m_tRelease = 0;
                QLabel::mouseReleaseEvent(ev);
            }
        }
    }
}
