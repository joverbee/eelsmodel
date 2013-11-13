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
 * eelsmodel - gui/eelsmodeltab.h
 * One tab containing toolbar, spectrum, image, model and fitter widgets for one opened file.
 **/

#ifndef EELSMODELTAB_H
#define EELSMODELTAB_H

#include <QMainWindow>

#include <src/core/spectrum.h>

class EELSModelTab : public QMainWindow
{
  Q_OBJECT

public:
  explicit EELSModelTab(Spectrum* spectrum, QWidget *parent = 0);

  void createActions();
  void createToolBar();
  void createGraphs();

signals:

public slots:
  void cut(); // put the marked text/object into the clipboard and remove it from the document
  void copy(); // put the marked text/object into the clipboard
  void paste(); // paste the clipboard into the document
  void undoSelection(); // undo selection on a graph
  void exclude(); // exclude points from a selected area
  void resetExclude(); // reset an excluded area from a selection or from the whome spectrum

  //model slots
  void newModel();
  void editComponent(); // edit components in the model
  void fitModel(); // fit the model to the spectrum
  void setDetector();

private:
  Spectrum* spectrum; // the (multi)spectrum

  //edit actions
  QAction* editCut;
  QAction* editCopy;
  QAction* editPaste;
  QAction* editUndoSelection;
  QAction* editExclude;
  QAction* editResetExclude;
  //view actions
  QAction* viewToolBar;
  QAction* viewStatusBar;
  //model actions
  QAction* modelNew;
  QAction* modelFit;
  QAction* modelComponent;
  QAction* modelDETECTOR;
};

#endif // EELSMODELTAB_H
