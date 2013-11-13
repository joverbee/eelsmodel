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
 * eelsmodel - gui/eelsmodeltab.cpp
 **/

#include "src/gui/eelsmodeltab.h"

#include <QActionGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>

#include "src/gui/imagedisplay.h"

EELSModelTab::EELSModelTab(Spectrum* spectrum, QWidget* parent)
: QMainWindow(parent),
  spectrum(spectrum)
{
  setAttribute(Qt::WA_DeleteOnClose); // delete this widget when tab is closed
  createToolBar();

  setCentralWidget(new QWidget(this));

  QHBoxLayout* columns = new QHBoxLayout;
  centralWidget()->setLayout(columns);
  QVBoxLayout* leftrows = new QVBoxLayout;
  QVBoxLayout* rightrows = new QVBoxLayout;
  columns->addLayout(leftrows);
  columns->addLayout(rightrows);

  // show image of spectrum
  QLabel* Imagedisplay_placeholder = new QLabel("This is were the image thingie goes.");
  Imagedisplay_placeholder->setAlignment(Qt::AlignCenter);
  Imagedisplay_placeholder->resize(100,100);
  leftrows->addWidget(Imagedisplay_placeholder);
  leftrows->addWidget(new Graph(spectrum));

  rightrows->addWidget(new QLabel("haha"));
}
//TODO
void EELSModelTab::createToolBar()
{
  // Exclusively grouped mouse cursor actions
  QActionGroup *group = new QActionGroup(this);
  group->setExclusive(true);

  //the normal mouse button
  QAction* normal = new QAction(QIcon(":/icons/previous.png"), tr("Normal"), group);
  normal->setStatusTip(tr("Normal mode"));
  normal->setWhatsThis(tr("Normal\n\nSets the cursor in the normal mode"));
  normal->setCheckable(true);
  normal->setChecked(true); // this should be the default option
  connect(normal, SIGNAL(triggered()), this, SIGNAL(toolbar_normal()));
  //the selection button
  QAction* selection = new QAction(QIcon(":/icons/rectangle.png"), tr("Region Select"), group);
  selection->setStatusTip(tr("Select a region in a graph"));
  selection->setWhatsThis(tr("Region Select\n\nSelect a region in a graph"));
  selection->setCheckable(true);
  connect(selection, SIGNAL(triggered()), this, SIGNAL(toolbar_selection()));

  //the zoom button
  QAction* zoom = new QAction(QIcon(":/icons/zoom.png"), tr("&Zoom"), group);
  zoom->setStatusTip(tr("Zoom in on a graph"));
  zoom->setWhatsThis(tr("Zoom\n\nZooms in on a graph"));
  zoom->setCheckable(true);
  connect(zoom, SIGNAL(triggered()), this, SIGNAL(toolbar_zoom()));

  // ungrouped buttons

  //the display home button
  QAction* home = new QAction(QIcon(":/icons/gohome.png"), tr("Home display"), this);
  home->setStatusTip(tr("Home display"));
  home->setWhatsThis(tr("Home display\n\nResets the graph arrea to show/n the original size"));
  home->setCheckable(false);
  connect(home, SIGNAL(triggered()), this, SIGNAL(toolbar_home()));

  //the link button
  QAction* link = new QAction(QIcon(":/icons/connect_established.png"), tr("Link"), this);
  link->setStatusTip(tr("Link parameters"));
  link->setWhatsThis(tr("Link\n\nLinks parameters together"));
  link->setCheckable(false);
  connect(link, SIGNAL(triggered()), this, SIGNAL(toolbar_link()));

  // The toolbar itself
  QToolBar* basicToolbar = addToolBar("basic tools");
  basicToolbar->addAction(normal);
  basicToolbar->addAction(selection);
  basicToolbar->addAction(zoom);
  basicToolbar->addSeparator(); // seperator between grouped and ungrouped buttons
  basicToolbar->addAction(home);
  basicToolbar->addAction(link);
}

void EELSModelTab::cut() {}
void EELSModelTab::copy() {}
void EELSModelTab::paste() {}
void EELSModelTab::undoSelection() {}
void EELSModelTab::exclude() {}
void EELSModelTab::resetExclude() {}
void EELSModelTab::newModel() {}
void EELSModelTab::editComponent() {}
void EELSModelTab::fitModel() {}
void EELSModelTab::setDetector() {}
