/***************************************************************************
                          menu.h  -  description
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

#ifndef MENU_H
#define MENU_H

#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QResizeEvent>
#include <QStatusBar>
#include <QString>
#include <QToolButton>
#include <QWhatsThis>
#include <QWidget>

class QWorkspace;
class QSignalMapper;
class QMenuBar;
class QToolBar;
class QLabel;

class MenuEelsmodel : public QMainWindow
{
    Q_OBJECT
public:

    /** construtor */
    MenuEelsmodel ();
    /** destructor */
    ~MenuEelsmodel();
    /** initializes all QActions of the application */
    void initActions();
    /** initMenuBar creates the menu_bar and inserts the menuitems */
    void initMenuBar();
    /** this creates the toolbars. Change the toolbar look and add new toolbars in this
     * function */
    void initToolBar();
    /** setup the statusbar */
    void initStatusBar();
    /** setup the workspace*/
    void initWorkspace();


    /** overloaded for Message box on last window exit */
    bool queryExit();
    /** get the workspace pointer*/
    QWorkspace* getworkspaceptr(){return myworkspace;};
    bool zoomMode(){return toolbarZoom->isChecked();};
    bool selectMode(){return toolbarSelection->isChecked();}

public slots:
   /** generate a new document in the actual view */
    void slotFileNew();
    /** open a document */
    void slotFileOpenMSA();
    /** open a DM file */
    void slotFileOpenDM();
    /** open a project */
    void slotFileProjectOpen();
    /** save a document */
    void slotFileSave();
    /** save a project*/
    void slotFileProjectSave();
    /** save a report*/
    void slotFileReportSave();
    /** save a document under a different filename*/
    void slotFileSaveAs();
     /** save a project under a different filename*/

    void slotFileProjectSaveAs();
    /** close the actual file */
    void slotFileClose();
    /** print the actual file */
    void slotFilePrint();
    /** exits the application */
    void slotFileQuit();
    /** put the marked text/object into the clipboard and remove
     * it from the document */
    void slotEditCut();
    /** put the marked text/object into the clipboard*/
    void slotEditCopy();
    /** paste the clipboard into the document*/
    void slotEditPaste();
    /** undo selection on a graph **/
    void slotEditUndoSelection();
    /** exclude points from a selected arrea **/
    void slotEditExclude();
    /** reset an excluded area from a selection or from the whome spectrum **/
    void slotEditResetExclude();

    /** toggle the toolbar*/
    void slotViewToolBar(bool toggle);
    /** toggle the statusbar*/
    void slotViewStatusBar(bool toggle);

    /** shows an about dlg*/
    void slotHelpAbout();

    /** shows an about dlg*/
    void slotHelpAboutQt();

//model slots
    /** create a new model*/
    void    slotModelNew();
    /** edit components in the model*/
    void    slotModelComponent();
    /** fit the model to the spectrum*/
    void    slotModelFit();
    /** set detector*/
    void    slotModelDETECTOR();

//model enabled slot
    void slot_enable_model(bool b);
protected:
    void    resizeEvent( QResizeEvent * );

signals:
    void    explain( const QString& );
//file
    void    file_new();
    void    file_open_msa();
    void    file_open_DM();
    void    file_project_open();
    void    file_close();
    void    file_project_save();
    void    file_model_save();
    void    file_report_save();
    void    file_save_as();

//edit
    void    edit_undoselection();
    void    edit_exclude();
    void    edit_resetexclude();

//model
    void    model_newmodel();
    void    model_componentmaintenance();
    void    model_fitbyhand();
    void    model_iterativefit();
    void    model_fitteroptions();
    void    model_detector();

//toolbar
    void  toolbar_normal();
    void  toolbar_selection();
    void  toolbar_zoom();
    void  toolbar_home();
    void  toolbar_link();

private:
    QMenuBar *menu;
    QToolBar* mytoolbar;
    QLabel   *label;
    bool isBold;
    bool isUnderline;
    int boldID, underlineID,componentID,fitbyhandID,itterativefitID;
    //All the actions

    //file actions
    QAction* fileNew;
    QAction* fileOpen;
    QAction* fileOpenDM;
    QAction* fileProjectOpen;
    QAction* fileClose;
    QAction* fileSave;
    QAction* fileProjectSave;
    QAction* fileReportSave;
    QAction* fileSaveAs;

    QAction* fileProjectSaveAs;
    QAction* filePrint;
    QAction* fileQuit;
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
    //help actions
    QAction* helpAboutApp;
    QAction* helpAboutQt;
    //toolbar actions
    QAction* toolbarSelection;
    QAction* toolbarZoom;
    QAction* toolbarLink;
    QAction* toolbarHome;
    QAction* toolbarNormal;
    //popup menus
    QMenu* fileMenu;
    QMenu* viewMenu;
    QMenu* editMenu;
    QMenu* helpMenu;
    QMenu* modelMenu;
    //the workspace pointer
    QWorkspace * myworkspace;
    QSignalMapper *windowMapper;


};


#endif // MENU_H
