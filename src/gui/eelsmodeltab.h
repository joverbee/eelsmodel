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

#include <memory>

#include <QMainWindow>

#include "src/core/model.h"
#include "src/core/spectrum.h"

#include "src/fitters/fitter.h"

class QLabel;

class Image;

class EELSModelTab : public QMainWindow
{
  Q_OBJECT

public:
  explicit EELSModelTab(Spectrum* spectrum, QWidget *parent = 0);
  explicit EELSModelTab(Image* spectrum, QWidget *parent = 0);

signals:

public slots:
  // Edit slots
  void cut(); // put the marked text/object into the clipboard and remove it from the document
  void copy(); // put the marked text/object into the clipboard
  void paste(); // paste the clipboard into the document
  void undoSelection(); // undo selection on a graph
  void exclude(); // exclude points from a selected area
  void resetExclude(); // reset an excluded area from a selection or from the whome spectrum

  // Model slots
  void newModel(); // create new model for spectrum in tab
  void setDetector();

  // ToolBar slots
  void setNormalMode();
  void setZoomMode();
  void setSelectMode();

private:
  void createMenuBar();
  void createLayout();
  void createToolBar();

  bool multi; //TODO get rid of this which says if below spectrum is actually a multispectrum
  std::unique_ptr<Spectrum> spectrum; // the (multi)spectrum from file
  Model model; // the model, might need specializations for (multi)spectrum

  // QWidgets in the tab
  Imagedisplay* imagedisplay;
  Graph* spectrum_graph;
  Graph* model_graph;

  // Edit menu actions
  QAction* editCut;
  QAction* editCopy;
  QAction* editPaste;
  QAction* editUndoSelection;
  QAction* editExclude;
  QAction* editResetExclude;

  // View menu actions
  QAction* viewToggleToolBar;
  QAction* viewToggleStatusBar;

  // Model actions
  QAction* modelNew;
  QAction* modelFit;
  QAction* modelComponent;
  QAction* modelDETECTOR;

  //toolbar actions
  QAction* toolbarSelection;
  QAction* toolbarZoom;
  QAction* toolbarLink;
  QAction* toolbarHome;
  QAction* toolbarNormal;
};

#endif // EELSMODELTAB_H
