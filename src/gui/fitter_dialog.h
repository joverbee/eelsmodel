/***************************************************************************
                          fitter_dialog.h  -  description
A dialog to control the fitter
                             -------------------
    begin                : Sat Nov 9 2002
    copyright            : (C) 2002 by Jo Verbeeck
    email                : jo@localhost.localdomain
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FITTER_DIALOG_H
#define FITTER_DIALOG_H

#include <string>

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QString>
#include <QValidator>
#include <QWidget>

#include "src/fitters/fitter.h"

class QWorkspace;

class Fitter_dialog : public QWidget  {
   Q_OBJECT
    QLineEdit* minstepedit;
    QLineEdit* maxstepedit;
    QLineEdit* fractionedit;
    QLineEdit* toleranceedit;
    QLineEdit* nmaxedit;
    QCheckBox* check_gradients;
    QLabel* chisqlabel;
    QLabel* confidencelabel;
    QProgressBar* pbar;
    QPushButton* stopbutton;
    double minstep,maxstep,fraction,chisq,tolerance;
    int nmax;
    QDoubleValidator * double_validator;
    QIntValidator * int_validator;
    Fitter* fitterptr;
    bool stop,update_allways,usegradients,previous,residual,redrawfit,startfromcurrent;

public:
	Fitter_dialog(QWorkspace *parent, const char *name,Fitter* myfitter);
	~Fitter_dialog();
double getnumber(const QString&)const;
bool getstop(){return stop;};
void getposition(int& xpos,int& ypos)const; //return the position of the window
void iterate_this_spectrum();
void save_params(bool dat,bool crlb);

public slots:
void slot_change_minstep(const QString&);
void slot_change_maxstep(const QString&);
void slot_change_fraction(const QString&);
void slot_change_tolerance(const QString&);
void slot_change_nmax(const QString&);
void quit(){delete this;};
//void slot_change_progress(int n);
void slot_iterations();
void slot_stop();
void slot_update();
void slot_set_update_allways(bool);
void slot_set_usegradients(bool);
void slot_set_residual(bool);
void slot_set_multi(bool);
void slot_set_lintrick(bool);
void slot_set_previous(bool);
void slot_set_startfromcurrent(bool b);
void slot_set_redrawfit(bool);
void slot_undo();
void slot_setposition(int x, int y); //changes position of window on screen
void slot_params();
void slot_params_save_dat();
void slot_model_save_dat();
void slot_params_save_txt();
void slot_params_save_crlb_dat();

signals:
void signal_progressbar(int n);
void update();
void signal_start_multi_report();
void signal_add_multi_report();
void signal_fitterdialog_died();
};

#endif
