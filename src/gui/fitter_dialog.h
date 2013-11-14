/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - gui/fitter_dialog.h
 *
 * A dialog to control the fitter.
 **/

#ifndef FITTER_DIALOG_H
#define FITTER_DIALOG_H

#include <memory>
#include <string>

#include <QCheckBox>
#include <QComboBox>
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

class fitterWidget : public QWidget
{
  Q_OBJECT

public:
  fitterWidget(QWidget* parent = 0);

private:
  std::unique_ptr<Fitter> fitter; // set through fitterChooser

  void createLayout();

  QComboBox* fitterChooser;


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
