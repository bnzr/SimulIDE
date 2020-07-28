/***************************************************************************
*   Copyright (C) 2016 by santiago González                               *
*   santigoro@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
*                                                                         *
***************************************************************************/

#ifndef CUSTOMPLOTWIDGET_H
#define CUSTOMPLOTWIDGET_H

#include <QPixmap>
#include <QtWidgets>
#include <QHBoxLayout>

#include <qcustomplot.h>

class CustomPlotWidget : public QWidget
{
    Q_OBJECT

    public:
    CustomPlotWidget(int width, int height);

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

	QCustomPlot* getCustomPlot();
	void setTimeplotWidget (QWidget *t);

	bool resizing();
	void replot();

    signals:
	void customPlotWidgetAboutToClose();

	
    protected:
        //void paintEvent( QPaintEvent *event );
	void resizeEvent (QResizeEvent *event );
	void closeEvent (QCloseEvent *event );

   private slots:
	void onMouseMove (QMouseEvent *event);
	void timeoutResizing ();
	
   private:
        int m_width;
        int m_height;
        QCustomPlot* customPlot;
	QHBoxLayout* gridLayout;
	QWidget* timeplot;
	QCPItemStraightLine *mouseMarkerX;
	QCPItemStraightLine *mouseMarkerY;
	QPen crosshairpen;
	QCPItemText *textItem;
	bool m_resizing;
	QTimer *timer_resizing;
};

#endif

