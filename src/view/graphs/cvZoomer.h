/*
 * cvZoomer.h
 *
 *  Created on: 06/09/2008
 *      Author: paulista
 */

#ifndef CVZOOMER_H_
#define CVZOOMER_H_
#include <qwt/qwt_plot_zoomer.h>

class cvZoomer: public QwtPlotZoomer
{
public:
    cvZoomer(QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerText(const QwtDoublePoint &pos) const
    {
        QColor bg(Qt::white);
       bg.setAlpha(200);

        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};

#endif /* CVZOOMER_H_ */