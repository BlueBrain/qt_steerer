/*----------------------------------------------------------------------------
    Header file for HistoryPlot class for QT steerer GUI

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the copyright holder, nor the
    University of Manchester offer any warranties or representations,
    nor do they accept any liabilities with respect to this software.

    This software must not be used for commercial gain without the
    written permission of the authors.

    This software must not be redistributed without the written
    permission of the authors.

    Permission is granted to modify this software, provided any
    modifications are made freely available to the original authors.

    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL

    WWW:    http://www.sve.man.ac.uk
    email:  sve@man.ac.uk
    Tel:    +44 161 275 6095
    Fax:    +44 161 275 6800

    Initial version by: M Riding, 24.06.2003

---------------------------------------------------------------------------*/

#ifndef HISTORYPLOT_H
#define HISTORYPLOT_H

#include "qlayout.h"
#include "qframe.h"
#include "qmenubar.h"
#include "qfiledialog.h"
#include "qthread.h"
#include "qpixmap.h"

#include <qwt_plot.h>
#include "types.h"




class ParameterHistory;
class QMenuBar;
class QPopupMenu;

/** The plotter class is the extended qwt widget.
 *  This deals with the drawing of the graph.
 */

class HistoryPlotter : public QwtPlot
{
  Q_OBJECT

private:
public:
  HistoryPlotter(QWidget *p=0, const char *name=0):QwtPlot(p, name){}
  ~HistoryPlotter(){}
};

/** The history plot class is the main window for the
 *  graph, with the extra functionality of menus etc.
 */
class HistoryPlot : public QFrame
{
  Q_OBJECT
  
private:
    QMenuBar *mMenuBar;
    QPopupMenu *mFileMenu;
    ParameterHistory *mParamHist;
    HistoryPlotter *mPlotter;
    char lLabel[kCHKPT_PARAM_LEN];
    int paramID;
        
    void doPlot();

public slots:
    void updateSlot(ParameterHistory *mParamHist, const int paramID);
    void filePrint();
    void fileSave();
    void fileQuit();

public:
    HistoryPlot(ParameterHistory *mParamHist, const char *lLabel, const int paramID);
    ~HistoryPlot();

};

/////////////////////////////////////////////////////////

/** This class is necessary to give QT time to update the widget properly before
 *  taking a screenshot. Seemingly, standard calls to refresh() redraw() update()
 *  and the like only cause a refresh after all current events have been dealt with.
 *  Hence this class.
 */
 
class ScreenGrabThread: public QThread
{
  private:
    QWidget *mWidget;
    QString mFileName;
  public:
    ScreenGrabThread(QWidget *lWidget, QString &lFileName){
      mWidget = lWidget;
      mFileName = lFileName;
    }
    virtual void run(){
      wait(500);

      // Grab the window contents rather than the widget
      // since we seemingly can't force a refresh immediately - spawn a thread to do this
      QPixmap lPixmap = QPixmap::grabWindow(mWidget->winId());
      lPixmap.save(mFileName, "JPEG");
    }
};

/////////////////////////////////////////////////////////
  

#endif
