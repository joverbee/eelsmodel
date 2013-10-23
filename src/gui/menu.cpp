/***************************************************************************
                          menu.cpp  -  description
                             -------------------
    begin                : Sat Oct 12 2002
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <QPixmap>
#include <QActionGroup>
#include <QResizeEvent>
#include <QMenu>
#include <qaction.h>
#include "src/gui/menu.h"
#include "./icons/filesave.xpm"
#include "./icons/fileopen.xpm"
#include "./icons/filenew.xpm"
#include "./icons/previous.xpm"
#include "./icons/rectangle.xpm"
#include "./icons/zoom.xpm"
#include "./icons/gohome.xpm"
#include "./icons/connect_established.xpm"
#include "src/gui/hello.h"
#include <QSignalMapper>
#include <QWorkspace>
#include <qmenubar.h>
#include <qtoolbar.h>



MenuEelsmodel::MenuEelsmodel(){
 setWindowTitle(tr("Eelsmodel 3.3") );
  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initWorkspace();
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();

//TODO : windows are on top of selection buttons, this makes them almost invisible
  //TODO: reload window positions from file

  viewToolBar->setChecked(true);
  viewStatusBar->setChecked(true);
}
MenuEelsmodel::~MenuEelsmodel(){
}
void MenuEelsmodel::initActions(){
  //all actions of the application

  QPixmap openIcon, saveIcon, newIcon;
  newIcon = QPixmap(filenew);
  openIcon = QPixmap(fileopen);
  saveIcon = QPixmap(filesave);



  //file actions
  //  fileNew = new QAction(tr("New File"), newIcon, tr("&New"), QAccel::stringToKey(tr("Ctrl+N")), this);
  //fileNew->setStatusTip(tr("Creates a new document"));
  //fileNew->setWhatsThis(tr("New File\n\nCreates a new document"));
  //connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  fileOpen = new QAction(openIcon,tr("Open Spectrum"), this);
  fileOpen->setStatusTip(tr("Opens an existing spectrum"));
  fileOpen->setWhatsThis(tr("Open Spectrum\n\nOpens an existing spectrum"));
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpenMSA()));

  fileProjectOpen = new QAction(openIcon,tr("Open Project"), this);
  fileProjectOpen->setStatusTip(tr("Opens an existing project"));
  fileProjectOpen->setWhatsThis(tr("Open Project\n\nOpens an existing project"));
  connect(fileProjectOpen, SIGNAL(activated()), this, SLOT(slotFileProjectOpen()));

  fileOpenDM = new QAction(openIcon,tr("Open DM3"), this);
  fileOpenDM->setStatusTip(tr("Opens a Digital Micrograph 3 image"));
  fileOpenDM->setWhatsThis(tr("Open DM3\n\nOpens a Digital Micrograph 3 image"));
  connect(fileOpenDM, SIGNAL(activated()), this, SLOT(slotFileOpenDM()));

  fileSave = new QAction(saveIcon,tr("Save Model as txt"), this);
  fileSave->setStatusTip(tr("Saves the model as a tab delimited text file"));
  fileSave->setWhatsThis(tr("Save Model as txt.\n\nSaves model and its components as a tab delimitted text file"));
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));

  fileProjectSave = new QAction(saveIcon, tr("Save Project"), this);
  fileProjectSave->setStatusTip(tr("Saves the actual project"));
  fileProjectSave->setWhatsThis(tr("Save Project.\n\nSaves the actual project"));
  connect(fileProjectSave, SIGNAL(activated()), this, SLOT(slotFileProjectSave()));

  fileReportSave = new QAction(saveIcon, tr("Save Report"), this);
  fileReportSave->setStatusTip(tr("Saves a report"));
  fileReportSave->setWhatsThis(tr("Save report.\n\nSaves the parameter values to a file\n for easy reading in a spreadsheet"));
  connect(fileReportSave, SIGNAL(activated()), this, SLOT(slotFileReportSave()));

  fileSaveAs = new QAction(saveIcon,tr("Save Spectrum as"), this);
  fileSaveAs->setStatusTip(tr("Saves the spectrum as a dat file"));
  fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the spectrum as a dat file"));
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));


  fileQuit = new QAction(tr("Exit"), this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), this, SLOT(slotFileQuit()));



  editUndoSelection = new QAction(tr("&Undo selection"), this);
  editUndoSelection->setStatusTip(tr("Undo selection"));
  editUndoSelection->setWhatsThis(tr("Undo selection"));
  connect(editUndoSelection, SIGNAL(activated()), this, SLOT(slotEditUndoSelection()));

  editExclude = new QAction(tr("Exclude points"), this);
  editExclude->setStatusTip(tr("Exlude selected points from fitting"));
  editExclude->setWhatsThis(tr("Exlude selected points from fitting"));
  connect(editExclude, SIGNAL(activated()), this, SLOT(slotEditExclude()));

  editResetExclude = new QAction(tr("Reset Exclude points"), this);
  editResetExclude->setStatusTip(tr("Reset exclude region, \n either on a selection or on the\n whole spectrum"));
  editResetExclude->setWhatsThis(tr("Reset exclude region, \n either on a selection or on the\n whole spectrum"));
  connect(editResetExclude, SIGNAL(activated()), this, SLOT(slotEditResetExclude()));

  //view actions
  viewToolBar = new QAction(tr("Toolbar"), this);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  viewToolBar->setCheckable(true);
  connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("Statusbar"), this);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
  viewStatusBar->setCheckable(true);
  connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

  //help actions
  helpAboutApp = new QAction(tr("About"), this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

  helpAboutQt = new QAction(tr("About Qt"), this);
  helpAboutQt->setStatusTip(tr("About Qt"));
  helpAboutQt->setWhatsThis(tr("About Qt\n\nAbout Qt"));
  connect(helpAboutQt, SIGNAL(activated()), this, SLOT(slotHelpAboutQt()));

  //model actions
  modelNew = new QAction(tr("New Model"), this);
  modelNew->setStatusTip(tr("New Model"));
  modelNew->setWhatsThis(tr("New Model\n\nCreate a new model"));
  connect(modelNew, SIGNAL(activated()), this, SLOT(slotModelNew()));

  modelComponent = new QAction(tr("Component"), this);
  modelComponent->setStatusTip(tr("Component maintenance"));
  modelComponent->setWhatsThis(tr("Component\n\nMaintain components in the model"));
  connect(modelComponent, SIGNAL(activated()), this, SLOT(slotModelComponent()));
  modelComponent->setEnabled(false);

  modelFit = new QAction(tr("Fit"), this);
  modelFit->setStatusTip(tr("Fit the model"));
  modelFit->setWhatsThis(tr("Fit\n\nFit the model"));
  connect(modelFit, SIGNAL(activated()), this, SLOT(slotModelFit()));
  modelFit->setEnabled(false);

  modelDETECTOR = new QAction(tr("choose detector"), this);
  modelDETECTOR->setStatusTip(tr("set the detector properties"));
  modelDETECTOR->setWhatsThis(tr("set the detector properties"));
  connect(modelDETECTOR, SIGNAL(activated()), this, SLOT(slotModelDETECTOR()));
  modelDETECTOR->setEnabled(false);

  //toolbar actions put them in an exclusive group

  QActionGroup *toolbargrp = new QActionGroup( this );
  toolbargrp->setExclusive( TRUE );
  //the normal mouse button
  QPixmap normalIcon(previous_xpm);
  toolbarNormal = new QAction(normalIcon, tr("Normal"), toolbargrp);
  toolbarNormal->setStatusTip(tr("Normal mode"));
  toolbarNormal->setWhatsThis(tr("Normal\n\nSets the cursor in the normal mode"));
  connect(toolbarNormal, SIGNAL(activated()), this, SIGNAL(toolbar_normal()));
  toolbarNormal->setCheckable( TRUE );
  toolbarNormal->setChecked( TRUE ); //the default

  //the selection button
  QPixmap selectionIcon(rectangle_xpm);
  toolbarSelection = new QAction(selectionIcon, tr("Region Select"), toolbargrp);
  toolbarSelection->setStatusTip(tr("Select a region in a graph"));
  toolbarSelection->setWhatsThis(tr("Region Select\n\nSelect a region in a graph"));
  connect(toolbarSelection, SIGNAL(activated()), this, SIGNAL(toolbar_selection()));
  toolbarSelection->setCheckable( TRUE );

  //the zoom button
  QPixmap zoomIcon(zoom_xpm);
  toolbarZoom = new QAction(zoomIcon, tr("&Zoom"), toolbargrp);
  toolbarZoom->setStatusTip(tr("Zoom in on a graph"));
  toolbarZoom->setWhatsThis(tr("Zoom\n\nZooms in on a graph"));
  connect(toolbarZoom, SIGNAL(activated()), this, SIGNAL(toolbar_zoom()));
  toolbarZoom->setCheckable( TRUE );

  //the display home button
  QPixmap homeIcon(gohome_xpm);
  toolbarHome = new QAction(homeIcon, tr("Home display"), this);
  toolbarHome->setStatusTip(tr("Home display"));
  toolbarHome->setWhatsThis(tr("Home display\n\nResets the graph arrea to show/n the original size"));
  connect(toolbarHome, SIGNAL(activated()), this, SIGNAL(toolbar_home()));
  toolbarHome->setCheckable( false );

  //the link button
  QPixmap linkIcon(connect_established_xpm);
  toolbarLink = new QAction(linkIcon, tr("Link"), this);
  toolbarLink->setStatusTip(tr("Link parameters"));
  toolbarLink->setWhatsThis(tr("Link\n\nLinks parameters together"));
  connect(toolbarLink, SIGNAL(activated()), this, SIGNAL(toolbar_link()));
  toolbarLink->setCheckable( false );

}

void MenuEelsmodel::initMenuBar(){
///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu
  fileMenu=new QMenu("File");
  //fileNew->addTo(fileMenu);
  fileMenu->addAction(fileOpen);
  fileMenu->addAction(fileProjectOpen);
  fileMenu->addAction(fileOpenDM);
  //fileClose->addAction(fileMenu);
  fileMenu->insertSeparator(fileSave);
  fileMenu->addAction(fileSave);
  fileMenu->addAction(fileProjectSave);
  fileMenu->addAction(fileReportSave);
  fileMenu->addAction(fileSaveAs);

  //fileProjectSaveAs->addAction(fileMenu);
  //fileMenu->insertSeparator();
  //filePrint->addAction(fileMenu);
  fileMenu->insertSeparator(fileQuit);
  fileMenu->addAction(fileQuit);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu=new QMenu("Edit");
  //editCut->addAction(editMenu);
  //editCopy->addAction(editMenu);
  //editPaste->addAction(editMenu);
  editMenu->addAction(editUndoSelection);
  editMenu->addAction(editExclude);
  editMenu->addAction(editResetExclude);

  //not implemented yet
  //editCut->setEnabled(FALSE );
  //editCopy->setEnabled(FALSE );
  //editPaste->setEnabled(FALSE );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu=new QMenu("View");
  //viewMenu->setCheckable(true);
  viewMenu->addAction(viewToolBar);
  viewMenu->addAction(viewStatusBar);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry modelMenu
  modelMenu=new QMenu("Model");
  //modelMenu->setCheckable(true);
  modelMenu->addAction(modelNew);
  modelMenu->addAction(modelComponent);
  modelMenu->addAction(modelFit);
  modelMenu->addAction(modelDETECTOR);

  slot_enable_model(false);


  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
  helpMenu=new QMenu("Help");
  helpMenu->addAction(helpAboutApp);
  helpMenu->addAction(helpAboutQt);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION

  menuBar()-> addMenu(fileMenu);
  menuBar()-> addMenu(editMenu);
  menuBar()-> addMenu(viewMenu);
  menuBar()-> addMenu(modelMenu);
  menuBar()->addSeparator();
  menuBar()-> addMenu(helpMenu);

//    setMinimumSize( 100, 80 );
}

void MenuEelsmodel::initToolBar(){
  //create a toolbar
  mytoolbar=new QToolBar ("basic tools",myworkspace);
  mytoolbar->setWindowTitle("basic tools");
  mytoolbar->addAction(toolbarNormal);
  mytoolbar->addAction(toolbarSelection);
  mytoolbar->addAction(toolbarZoom);
  mytoolbar->addAction(toolbarHome);
  mytoolbar->addAction(toolbarLink);
  mytoolbar->addSeparator();
  //QWhatsThis::whatsThisButton(mytoolbar);

  //not implemented yet

  toolbarHome->setEnabled(FALSE );
  toolbarLink->setEnabled(FALSE );
}

void MenuEelsmodel::initStatusBar(){
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR


  statusBar()->showMessage(tr("Ready."), 2000);
}

void MenuEelsmodel::initWorkspace()
{
  ////////////////////////////////////////////////////////////////////
  // init the workspace
  myworkspace=new QWorkspace (this);
  myworkspace->setWindowTitle("EELSMODEL");
  setCentralWidget(myworkspace);
  myworkspace->show();
  myworkspace->tile();
  //
  connect(myworkspace, SIGNAL(windowActivated(QWidget *)),this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget *)),myworkspace, SLOT(setActiveWindow(QWidget *)));
}



bool MenuEelsmodel::queryExit()
{
  int exit=QMessageBox::question(this, tr("Quit..."),
                                    tr("Do your really want to quit?"),
                                    QMessageBox::Ok, QMessageBox::Cancel);
  return (exit==QMessageBox::Ok ); //return true if OK
}

////////////////////////////////////////////////////////////////////
// Slot definitions
void MenuEelsmodel::slotViewToolBar(bool toggle)
{
  statusBar()->showMessage(tr("Toggle toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if (toggle== false)
  {
    mytoolbar->hide();
  }
  else
  {
    mytoolbar->show();
  };

  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotViewStatusBar(bool toggle)
{
  statusBar()->showMessage(tr("Toggle statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if (toggle == false)
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  statusBar()->showMessage(tr("Ready."));
}


void MenuEelsmodel::slotEditCut()
{
  statusBar()->showMessage(tr("Cutting selection..."));

  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotEditCopy()
{
  statusBar()->showMessage(tr("Copying selection to clipboard..."));

  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotEditPaste()
{
  statusBar()->showMessage(tr("Inserting clipboard contents..."));

  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotEditUndoSelection(){
  statusBar()->showMessage(tr("Undoing selection..."));
  emit(edit_undoselection());
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotEditExclude(){
  statusBar()->showMessage(tr("Excluding points..."));
  emit(edit_exclude());
  statusBar()->showMessage(tr("Ready."));
}
void MenuEelsmodel::slotEditResetExclude(){
  statusBar()->showMessage(tr("Resetting exclude region..."));
  emit(edit_resetexclude());
  statusBar()->showMessage(tr("Ready."));
}





//file menu slots
void MenuEelsmodel::slotFileNew()
{
  statusBar()->showMessage(tr("Creating new spectrum..."));
  emit file_new();
  statusBar()->showMessage(tr("Ready."));
}
void MenuEelsmodel::slotFileOpenMSA()
{
  statusBar()->showMessage(tr("Opening file..."));
  emit file_open_msa();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileProjectOpen()
{
  statusBar()->showMessage(tr("Opening project..."));
  emit file_project_open();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileOpenDM()
{
  statusBar()->showMessage(tr("Opening DM image..."));
  emit file_open_DM();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileSave()
{
  statusBar()->showMessage(tr("Saving file..."));
  emit file_model_save();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileProjectSave()
{
  statusBar()->showMessage(tr("Saving project..."));
  emit file_project_save();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileReportSave()
{
  statusBar()->showMessage(tr("Saving report..."));
  emit file_report_save();
  statusBar()->showMessage(tr("Ready."));
}
void MenuEelsmodel::slotFileSaveAs()
{
  statusBar()->showMessage(tr("Saving spectrum as dat..."));
  emit file_save_as();
  statusBar()->showMessage(tr("Ready."));
}



void MenuEelsmodel::slotFileProjectSaveAs()
{
  statusBar()->showMessage(tr("Saving project under new filename..."));
  emit file_project_save();
  statusBar()->showMessage(tr("Ready."));
}

void MenuEelsmodel::slotFileClose()
{
  statusBar()->showMessage(tr("Closing file..."));
   emit file_close();
  statusBar()->showMessage(tr("Ready."));
}
void MenuEelsmodel::slotFilePrint()
{
 /* statusBar()->showMessage(tr("Printing..."));
  QPrinter printer;
  if (printer.setup(this))
  {
    QPainter painter;
    painter.begin(&printer);

    ///////////////////////////////////////////////////////////////////
    // TODO: Define printing by using the QPainter methods here

    painter.end();
  };

  statusBar()->showMessage(tr("Ready."));
  */
}
void MenuEelsmodel::slotFileQuit()
{
  statusBar()->showMessage(tr("Exiting application..."));
  ///////////////////////////////////////////////////////////////////
  // exits the Application
  if(queryExit())
    {
      qApp->quit();
    }
  statusBar()->showMessage(tr("Ready."));
}

//model menu slots
void MenuEelsmodel::slotModelNew()
{
    statusBar()->showMessage(tr("Creating a new model"));
    //activate component and detector (fit is not sensible without components
    modelComponent->setEnabled(TRUE );
    modelDETECTOR->setEnabled(TRUE);
    emit model_newmodel();
}
void MenuEelsmodel::slotModelComponent()
{
    statusBar()->showMessage(tr("Adding removing components to the model"));
    modelFit->setEnabled(TRUE );
    emit model_componentmaintenance();
}

void MenuEelsmodel::slotModelFit()
{
    statusBar()->showMessage(tr("Fitting the model"));
    emit model_iterativefit();
}

void MenuEelsmodel::slotModelDETECTOR()
{
    statusBar()->showMessage(tr("Setting the DETECTOR"));
    emit model_detector();
}

//help menu slots
void MenuEelsmodel::slotHelpAbout()
{
  Hello* hellobox=new Hello();
  (void) hellobox; //get rid of unused variable warnings
}
void MenuEelsmodel::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this,tr("Eelsmodel\nVersion VERSION") );
}


void MenuEelsmodel::resizeEvent( QResizeEvent * )
{
    //label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
}

void MenuEelsmodel::slot_enable_model(bool b)
{
  modelComponent->setEnabled(b);
  modelFit->setEnabled(b);
  modelDETECTOR->setEnabled(b);
}
