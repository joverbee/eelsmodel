/***************************************************************************
                          eelsmodel.h  -  description
                             -------------------
    begin                : Sat Oct 12 12:25:14 CEST 2002
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

#ifndef EELSMODEL_H
#define EELSMODEL_H

//#define DEBUG_EELSMODEL //print some debugging info

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include <kapp.h>
#include <qwidget.h>
#include <vector>
class Component;
class Fitter;
class Spectrum;
class Multispectrum;
class Imagedisplay;
class QWorkspace;
#include "src/core/model.h"
class QWorkspace;
class Graph;
class Componentmaintenance;
class Fitter_dialog;
class Image;
/** Eelsmodel is the base class of the project */
class Eelsmodel : public QWidget
{
  Q_OBJECT

  Model* mymodel;
  Image* myimage;
  Fitter* myfitter;
  Componentmaintenance* mymaintain;
  Fitter_dialog* mydialog;
  std::string reportfilename;
  std::string projectfilename;
    public:class EelsmodelErr{   //class for exeptions
    public:
    class bad_alloc{};
    class load_error{ public: const char* msgptr;load_error(const char* e){msgptr=e;}};
    class save_error{ public: const char* msgptr;save_error(const char* e){msgptr=e;}};
    class load_cancelled{};
    class save_cancelled{};
  };

  public:
    /** construtor */
    Eelsmodel(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~Eelsmodel();
    const Model* getmodel()const{return mymodel;}
    Model* getmodel_nonconst()const{return mymodel;}
    std::string getprojectfilename()const{return projectfilename;};
  private:
  Spectrum* gettopspectrum();
  Multispectrum* gettopmultispectrum();
  Model* gettopmodel();
  Graph* gettopgraph();
  Imagedisplay* gettopimdisplay();
  Multispectrum* gettopmultispectrumfromimdisplay();
  void save_project(Model* mymodel,Fitter* myfitter);
  std::string save_report(Model* mymodel,Fitter* myfitter,std::string filename,bool append);
  void load_project(Model*& mymodel,Fitter*& myfitter,std::string filename);
  void fitter_dialog();

  public slots:
    void newgraph();
    void openmsa();
    Multispectrum* openDM(std::string filename="",bool silent=false)const;
    void newspectrum();
    void componentmaintenance();
    void componentmaintenance_remove_components(int);
    void componentmaintenance_add_components(int);
    void componentmaintenance_updatescreen();
    void componentmaintenance_doupdate();
    void newmodel();
    void iterativefit();
    void fitter_updatescreen();
    void undoselection();
    void exclude();
    void resetexclude();
    void slot_save_project();
    void slot_save_model();
    //void slot_save_report();
    void slot_save_as();
    //void slot_start_multi_report(); //start a report for a multispectrum fitting
    //void slot_add_multi_report(); //add to this report
    void slot_open_project();
    void slot_open_project_from_filename(std::string filename);
    void slot_model_update();
    void slot_model_detector();
    void slot_componentmaintenance_died();
    void slot_fitterdialog_died();
    void slot_componentmaintenance_updatemonitors();
public:
signals:
    void componentmaintenance_update();
    void enablemodel(bool b);
};

#endif
