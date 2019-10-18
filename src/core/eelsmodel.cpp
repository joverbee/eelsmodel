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
 * eelsmodel - core/eelsmodel.cpp
 **/

//#define DEBUG_EELSMODEL
#define GSLFITTER

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <QMessageBox>
#include <QMdiArea>
#include <QApplication>
#include <QDateTime>

#include "src/components/powerlaw.h"

#include "src/core/component.h"
#include "src/core/eelsmodel.h"
#include "src/core/image.h"
#include "src/core/model.h"
#include "src/core/monitor.h"
#include "src/core/multispectrum.h"
#include "src/core/spectrum.h"

#include "src/fitters/fitter.h"
#include "src/fitters/levenbergmarquardt.h"
#include "src/fitters/lsqfitter.h"
#include "src/fitters/mlfitter.h"
#include "src/fitters/mlfitter_gsl.h"
#include "src/fitters/wlsqfitter.h"

#include "src/gui/componentmaintenance.h"
#include "src/gui/detectorchooser.h"
#include "src/gui/fitterchooser.h"
#include "src/gui/fitter_dialog.h"
#include "src/gui/getfilename.h"
#include "src/gui/graph.h"
#include "src/gui/imagedisplay.h"
#include "src/gui/realinput.h"
#include "src/gui/saysomething.h"

QMdiArea* getworkspaceptr();

Eelsmodel::Eelsmodel(QWidget* parent, const char *name) : QWidget(0)
{
getworkspaceptr()->show();
myfitter=0;
mymodel=0;
mymaintain=0;
mydialog=0;
projectfilename="";
}

Eelsmodel::~Eelsmodel()
{
if (myfitter!=0) delete myfitter;
if (mymodel!=0) delete mymodel;
if (mymaintain!=0) delete mymaintain;
if (mydialog!=0) delete mydialog;
}

void Eelsmodel::newgraph(){
  //  Graph* mygraph =new Graph(0,"Graph",1024);                             // create widget
   // mygraph->show();
  //  QMessageBox::about( this,"info","Graph added\n" );
}
void Eelsmodel::newspectrum(){
    Spectrum* s1 =new Spectrum(1024);                             // create spectrum
    s1->display(getworkspaceptr());
}


//opens a file in emsa/mas format if cancelled or a load error occurs, no spectrum is created
void Eelsmodel::openmsa(){
    loadmsa l;
    Spectrum* s1=0;
    try{
    s1 =new Spectrum(l,"");                             // create spectrum from disc
    }
    catch(Spectrum::Spectrumerr::load_error& e){
      this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error",e.msgptr,true);
      return;
      }
    catch(Spectrum::Spectrumerr::load_cancelled){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
        return;}
    catch(...){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error","An unexpected error has occured during emsa load",true);
      return;}

    //reset the mouse pointer in case we had an exception
    QApplication::restoreOverrideCursor(); // we're done

      s1->display(getworkspaceptr());
      //s1->display(0);

}

Multispectrum* Eelsmodel::openDM(std::string filename,bool silent)const{
   //open the image and convert to mspec
   Image* myimage=0;
   Multispectrum* mymspec=0;
   try{
      myimage=new Image(getworkspaceptr(),"Image",filename,silent);
      //now create an mspec
      mymspec=myimage->getmspec();
      mymspec->display(getworkspaceptr());
    }
    catch(...){
      //something went wrong

      myimage=0;
      mymspec=0;
    }
  //reset the mouse pointer in case we had an exception
  QApplication::restoreOverrideCursor(); // we're done
   return mymspec;
}

void Eelsmodel::componentmaintenance(){


    //Model* mymodel=gettopmodel();
    if (mymodel!=0){
      //draw the maintenance window
     mymaintain=new Componentmaintenance(getworkspaceptr(),"Component maintenance"
                                                              ,(mymodel->getcomponentvector())
                                                              ,(mymodel->getallcomponentvector()));

     //make a connection with componentmaintenance slots
     connect( mymaintain, SIGNAL(remove_components(int) ),this, SLOT( componentmaintenance_remove_components(int) ) );
     connect( mymaintain, SIGNAL(add_components(int) ), this, SLOT( componentmaintenance_add_components(int) ) );
     connect( this, SIGNAL(componentmaintenance_update() ), mymaintain, SLOT( slot_update() ) );
     connect( mymaintain, SIGNAL(signal_update() ),this, SLOT( componentmaintenance_updatescreen() ) );
     connect( mymaintain, SIGNAL(componentmaintenance_died() ),this, SLOT(slot_componentmaintenance_died() ) );

     mymaintain->show();
      }
    else{
     Saysomething mysay(0,"Error","You have to select a valid model window first",true);
     return;
      }
}


void Eelsmodel::newmodel(){
    Multispectrum* m1=gettopmultispectrum();
    if (m1==0){
      Spectrum* s1 =gettopspectrum();
      if (s1==0) {
        Saysomething mysay(0,"Error","You have to select a valid spectrum first",true);
        return;
      }
      else{
        //we have a normal spectrum
        if (mymodel!=0){
          switch( QMessageBox::warning( this, "EELSMODEL",
          "A model already exists. Continuing will delete this model.\n"
          "Continue? ",
          "Yes",
          "No", 0, 0, 1 ) )
          {
          case 1: // The user clicked no
            return;
          case 0: // The user clicked yes or pressed enter
            delete(mymodel);
            break;
          }
        }
        mymodel = new Model(s1);
      }
    }
    else{
      //we have a multispectrum
      if (mymodel!=0){
          switch( QMessageBox::warning( this, "EELSMODEL",
          "A model already exists. Continuing will delete this model.\n"
          "Continue? ",
          "Yes",
          "No", 0, 0, 1 ) )
          {
          case 1: // The user clicked no
            return;
          case 0: // The user clicked yes or pressed enter
            delete(mymodel);
            break;
          }
        }
      mymodel = new Model(m1);
    }


    mymodel->calculate();
    mymodel->display(getworkspaceptr());
    mymodel->printcomponents();
    emit enablemodel(true);

    //if spectrum is a multispectrum, make connection to update model whenever the current
    //spectrum changes
    if (m1!=0){
      const Imagedisplay* myimdisplay=m1->getimageptr();
      connect( myimdisplay, SIGNAL(curr_spec_update() ),this, SLOT( slot_model_update() ) );
    }
}

void Eelsmodel::slot_model_update(){
  if (mymodel!=0) {
      mymodel->retrieveuserparams(); //make sure the model is calculated and we have the right parameters
      mymodel->updateHL(); //no need for init here
  }
}

void Eelsmodel::slot_componentmaintenance_updatemonitors(){
     if (mymaintain!=0){
        mymaintain->slot_update_monitors();
    }
}

void Eelsmodel::componentmaintenance_remove_components(int index){
    #ifdef DEBUG_EELSMODEL
    std::cout <<"Eelsmodel::componentmaintenance_remove_component(" << index << ")\n";
    #endif
    //Model* mymodel=gettopmodel();
    if (mymodel!=0){
      Component* multiplierptr=mymodel->getcomponent(index)->getmultiplierptr();
      mymodel->removecomponent(index);
      if (multiplierptr!=0){
        //also remove the multiplier
          mymodel->removecomponent(mymodel->getcomponentindexbypointer(multiplierptr));
      }
      mymodel->calculate();
      mymodel->display(getworkspaceptr());
      emit componentmaintenance_update();
      }

}
void Eelsmodel::componentmaintenance_add_components(int index){
    #ifdef DEBUG_EELSMODEL
    std::cout <<"Eelsmodel::componentmaintenance_add_component(" << index << ")\n";
    #endif
    //Model* mymodel=gettopmodel();
    if (mymodel!=0){
      mymodel->addcomponent(index);
      mymodel->calculate();
      mymodel->display(getworkspaceptr());
      emit componentmaintenance_update();
      }
}

void Eelsmodel::componentmaintenance_doupdate(){
    emit componentmaintenance_update();
}

void Eelsmodel::componentmaintenance_updatescreen(){
     #ifdef DEBUG_EELSMODEL
     std::cout <<"Eelsmodel::componentmaintenance_updatescreen\n";
     #endif
      if (mymodel!=0){
      mymodel->calculate();
      mymodel->display(getworkspaceptr());
      mymodel->setchanged(true); //if we are here something might have changed to the parameter
      std::string result="";
      if (myfitter!=0){
        result.append(myfitter->goodness_of_fit_string());
        result.append("\t");
        result.append(myfitter->LRtestconfidence_string());
      }
      mymodel->storeuserparams(result);
      //this could cause a change in the number of free parameters and the fitter
      //has to recreate its knowledge over the model
      }
}


void Eelsmodel::iterativefit(){
  //ask user for a type of fitter
  if (mymodel!=0){
    int result;
    FitterChooser* mychooser=new FitterChooser(getworkspaceptr(),"Fitter Chooser",result);
    (void) mychooser;//get rid of unused variable warning
    #ifdef DEBUG_EELSMODEL
    std::cout <<"Eelsmodel::fitter chooser returned(" << result << ")\n";
    #endif
    switch(result){
    case -1:
      return;  //the user pressed cancel
    case 0:
      if (myfitter!=0) delete(myfitter);
      myfitter=new LSQFitter(mymodel);//least squares
      break;
    case 1:
      if (myfitter!=0) delete(myfitter);
      myfitter=new WLSQFitter(mymodel);//weighted least squares
      break;
    case 2:
      if (myfitter!=0) delete(myfitter);
      myfitter=new MLFitter(mymodel);//maximum likelyhood
      break;
      //disable GSL fitter for non linux

       #ifdef GSLFITTER
    case 3:
      if (myfitter!=0) delete(myfitter);
     
      myfitter=new MLFitterGSL(mymodel);//GSL fitter least squares
     
      break;
      #endif

    case 4:
      if (myfitter!=0) delete(myfitter);
      
      myfitter=new LevenbergMarquardt(mymodel);//LevenbergMarquardt for Poisson
      
      break;

    default: //unexpected
      return;
    }

    //show a dialog with fitter options and controls
    fitter_dialog();
  }
  else{
    Saysomething mysay(0,"Error","You have to select a valid model window first",true);
  }
}

void Eelsmodel::fitter_dialog(){
  //causes a segmentation fault, because signalling that fitter_dialog died doesn't work
  //so fitter dialog is already gone but the pointer!=0, deleting it twice...
  //if (mydialog!=0){
    //kill the old one
  //  delete(mydialog);
   //}
  //create a new one
  mydialog=new  Fitter_dialog(getworkspaceptr(),"Fitter dialog",myfitter);
  mydialog->show();
  connect( mydialog, SIGNAL(update() ),this, SLOT( fitter_updatescreen() ) );
  connect( mydialog, SIGNAL(signal_start_multi_report()),this, SLOT(( slot_start_multi_report()) ) );
  connect( mydialog, SIGNAL(signal_add_multi_report() ),this, SLOT( slot_add_multi_report() ) );
  connect( mydialog, SIGNAL(signal_fitterdialog_died() ),this, SLOT( slot_fitterdialog_died() ) );
}

void Eelsmodel::fitter_updatescreen(){
  if (mymodel!=0){
    mymodel->display(getworkspaceptr());
    }
  //also redraw the component maintenance window to update the monitors
   if (mymaintain!=0){
    mymaintain->slot_update_monitors();
    }
}

void Eelsmodel::undoselection(){
//remove selection from the top graph spectrum
Graph* topgraph=gettopgraph();
if (topgraph!=0){
    topgraph->resetselection();
  }
else{
   Saysomething mysay(0,"Error","The top window is not a spectrum graph",true);
   return;
  }
}

void Eelsmodel::exclude(){
Graph* topgraph=gettopgraph();
Spectrum* topspectrum=gettopspectrum();
if ((topgraph!=0)&&(topspectrum!=0)){
    if (topgraph->getselected()){//if something selected
      //exclude this region
      topspectrum->setexcluderegion(topgraph->getstartindex(),topgraph->getendindex());
      //repaint the graph to show
    //  topgraph->repaint();
      topgraph->updategraph(0,topspectrum);
      }
    else{
      Saysomething mysay(0,"Info","Select a region first");
      return;
      }
  }
else{
   Saysomething mysay(0,"Error","The top window is not a spectrum graph",true);
   return;
  }
}

void Eelsmodel::resetexclude(){
Graph* topgraph=gettopgraph();
Spectrum* topspectrum=gettopspectrum();
if ((topgraph!=0)&&(topspectrum!=0)){
    if (topgraph->getselected()){//if something selected
      //reset exclude in this region
      topspectrum->resetexcluderegion(topgraph->getstartindex(),topgraph->getendindex());
      //repaint the graph to show
      topgraph->updategraph(0,topspectrum);
      //topgraph->repaint();
      }
    else{
      //reset all excludes
      topspectrum->resetexcluderegion(0,((topspectrum->getnpoints())-1));
      //repaint the graph to show
       topgraph->updategraph(0,topspectrum);
      //topgraph->repaint();
      }
  }
else{
   Saysomething mysay(0,"Error","The top window is not a spectrum graph",true);
   return;
  }
}



Spectrum* Eelsmodel::gettopspectrum(){
  Graph* topgraph=gettopgraph();
  if (topgraph!=0){
  //ask the graph for a pointer to its spectrum
    return topgraph->getspectrumptr();
  }
  else{
    return 0;
  }
}

Multispectrum* Eelsmodel::gettopmultispectrum(){
  Graph* topgraph=gettopgraph();
  if (topgraph!=0){
    //ask the graph for a pointer to its spectrum
    return topgraph->getmultispectrumptr();
  }
  else{
    return 0;
  }
}

Multispectrum* Eelsmodel::gettopmultispectrumfromimdisplay(){
  Imagedisplay* topim=gettopimdisplay();
  if (topim!=0){
    //ask the graph for a pointer to its spectrum
    return topim->getmultispectrumptr();
  }
  else{
    return 0;
  }
}

Model* Eelsmodel::gettopmodel(){
	Model* topmodel=dynamic_cast<Model*>(gettopspectrum());
	return topmodel;
}

Graph* Eelsmodel::gettopgraph(){
   //get a pointer to the active window
   //QWidget* topw=getworkspaceptr()->activeWindow();
   QWidget* topw=QApplication::activeWindow();
   Graph*  topgraph=dynamic_cast<Graph*>(topw); //make sure to enable rtti in BCC32 , otherwise an error about polymorphism is generated
   return topgraph;
}

Imagedisplay* Eelsmodel::gettopimdisplay(){
   //get a pointer to the active window
   //QWidget* topw=getworkspaceptr()->activeWindow();
   QWidget* topw=QApplication::activeWindow();
   Imagedisplay*  topdisplay=dynamic_cast<Imagedisplay*>(topw); //make sure to enable rtti in BCC32 , otherwise an error about polymorphism is generated
   return topdisplay;
}

void Eelsmodel::slot_componentmaintenance_died(){
  //reset the pointer since apparently the componentmaintenance window has gone
  mymaintain=0;
}

void Eelsmodel::slot_fitterdialog_died(){
  //reset the pointer since apparently the fitterdialog window has gone
  mydialog=0;
  //also kill the fitter with it
  if (myfitter!=0) delete(myfitter);
  myfitter=0;
}

void Eelsmodel::slot_save_project(){
  try{
  save_project(mymodel,myfitter);
  }
  catch(EelsmodelErr::save_error& e){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error",e.msgptr);
      return;
      }
  catch(EelsmodelErr::save_cancelled){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
        return;}
  catch(...){
      this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error","An unexpected error has occured during project save",true);
      return;}
}

void Eelsmodel::slot_save_as(){
     //save the frontmost spectrum or multispectrum in dat format

     //get the frontmost window and check if it is a spectrum or multispectrum
     Multispectrum* m1=gettopmultispectrumfromimdisplay();
     Spectrum* s1 =gettopspectrum();
     if ((s1==0)&&(m1==0)) {
        Saysomething mysay(0,"Error","You have to select a valid spectrum first",true);
        return;
     }
     //open a file for writing
      Getfilename myfile(0,"Select a file to save");
      //myfile.setdirname("~");
      myfile.setfilter("Dat files (*.dat *.DAT)");
      myfile.setcaption("Select a filename to save");
      std::string filename;
      filename=myfile.save();
      if (filename=="") return;
      #ifdef DEBUG_EELSMODEL
      std::cout <<"the filename is: "<<filename <<"\n";
      #endif
      //try to open for reading to see if it exists
      std::ifstream dummy(filename.c_str());
      if (dummy) {
         switch( QMessageBox::warning( this, "EELSMODEL",
        "This file already exists.\n"
        "Overwrite? ",
        "Yes",
        "No", 0, 0, 1 ) )
        {
        case 1: // The user clicked no
             return;
        case 0: // The user clicked yes or pressed enter
             break;
        }
      }

      if (m1!=0){
           //write a multipsectrum
           m1->savedat(filename);
      }
      if (s1!=0){
           //write a normal spectrum
           s1->savedat(filename);


      }
}


void Eelsmodel::slot_open_project(){
try{
  load_project(mymodel,myfitter,"");
  }
  catch(EelsmodelErr::load_error& e){
      this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error",e.msgptr);
      return;
      }
  catch(EelsmodelErr::load_cancelled){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
        return;}

  catch(...){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error","An unexpected error has occured during project load",true);
      return;}
}
void Eelsmodel::slot_open_project_from_filename(std::string filename){
try{
  load_project(mymodel,myfitter,filename);
  }
  catch(EelsmodelErr::load_error& e){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error",e.msgptr);
      return;
      }
  catch(EelsmodelErr::load_cancelled){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
        return;}
  catch(...){
        this->setCursor(Qt::ArrowCursor); //set to normal cursor
      Saysomething mysay(0,"Error","An unexpected error has occured during project load",true);
      return;}
}

void Eelsmodel::slot_model_detector(){
  //launches a window that lets the user choose between different detectors
  //it then fills in these data in the model
    DetectorChooser * mychooser=new DetectorChooser(0,"Monitor Chooser",mymodel);
    (void) mychooser; //get rid of unused variable warning

}


void Eelsmodel::save_project(Model* mymodel,Fitter* myfitter){
//***************************************
//*      save the whole project
//***************************************
//model, fitter etc

//check if a model exists
  if (mymodel==0) throw EelsmodelErr::save_error("No model found, a project\n is only meaningful when you have at least a model");
//start to get a filename to save
  Getfilename myfile(0,"Select a file to save");
  //myfile.setdirname("~");
  myfile.setfilter("Projects (*.mod *.MOD)");
  myfile.setcaption("Select an eelsmodel project file to save");
  std::string filename;
  filename=myfile.save();
  if (filename=="") throw EelsmodelErr::save_cancelled();//cancelled by user
  #ifdef DEBUG_EELSMODEL
  std::cout <<"the filename is: "<<filename <<"\n";
  #endif
  //try to open for reading to see if it exists
  std::ifstream dummy(filename.c_str());
  if (dummy) {
    switch( QMessageBox::warning( this, "EELSMODEL",
        "This file already exists.\n"
        "Overwrite? ",
        "Yes",
        "No", 0, 0, 1 ) )
      {
      case 1: // The user clicked no
        return;
      case 0: // The user clicked yes or pressed enter
        break;
      }
    }
    //open this file for writing ascii
    std::ofstream projectfile(filename.c_str());
    if (!projectfile) {
      throw EelsmodelErr::save_error("Can not open file for writing");
    }
this->setCursor(Qt::WaitCursor); //set to wait cursor
    //save the model
    projectfile <<"EELSMODEL PROJECT FILE\n";
    projectfile <<"#MODEL\n";
    //save model specific data
    //also save the type of this file
    if (mymodel->ismulti()) {
      projectfile <<"#MODELHLFILENAME:"<< (mymodel->getmultispectrumptr())->getfilename();
      projectfile <<":MULTI"<<"\n";
    }
    else {
      projectfile <<"#MODELHLFILENAME:"<< (mymodel->getHLptr())->getfilename();
      projectfile <<":MSA"<<"\n";
    }
    //detector settings
    projectfile <<"#DETECTORNAME:"<< mymodel->getdetectorname()<<"\n";
    projectfile <<"#DETECTORGAIN:"<< mymodel->getdetectorgain()<<"\n";
    projectfile <<"#DETECTORCORRFACTOR:"<< mymodel->getdetectorcorrfactor()<<"\n";


    //save model components and their parameters
    projectfile <<"#COMPONENTLIST\n";
    for (size_t i=0;i<(mymodel->getcomponentsnr());i++){
      Component* mycomponent=mymodel->getcomponent(i);
      projectfile <<"COMPONENTNAME:"<<mycomponent->getname()<<"\n";
      projectfile <<"COMPONENTDISPLAYNAME:"<<mycomponent->getdisplayname()<<"\n";

      //list the parameters
      int compnr=mycomponent->get_nr_of_parameters();
      for (int j=0;j<compnr;j++){
        Parameter* myparameter=mycomponent->getparameter(j);
        projectfile <<"PARAMETERNAME:"<<myparameter->getpurename()<<"\n";
        projectfile <<"PARAMETERVALUE:"<<myparameter->getvalue()<<"\n";
        projectfile <<"PARAMETERLOWERBOUND:"<<myparameter->getlowerbound()<<"\n";
        projectfile <<"PARAMETERUPPERBOUND:"<<myparameter->getupperbound()<<"\n";
        projectfile <<"PARAMETERCHANGEABLE:"<<myparameter->ischangeable()<<"\n";
        projectfile <<"PARAMETERBOUND:"<<myparameter->isbound()<<"\n";
       }
       projectfile << "ENDOFCOMPONENT:\n";
    }
    projectfile <<"#ENDOFCOMPONENTLIST\n";
    //save exclude regions
    projectfile <<"#EXCLUDELIST\n";
    for (unsigned int i=0;i<(mymodel->getnpoints());i++){
      if (mymodel->isexcluded(i)) projectfile << i <<"\n";
    }
    projectfile <<"#ENDOFEXCLUDELIST\n";

    //save parameter coupling
    projectfile <<"#COUPLINGLIST\n";
    int modelnr=mymodel->getcomponentsnr();
    for (int i=0;i<modelnr;i++){
      Component* mycomponent=mymodel->getcomponent(i);
      int compnr=mycomponent->get_nr_of_parameters();
      for (int j=0;j<compnr;j++){
        Parameter* myparameter=mycomponent->getparameter(j);
        if (myparameter->iscoupledslave()){//if a parameter is coupled as slave
          //get the number of the parameter that is master
          Parameter* masterparameter=myparameter->getmaster();
          //now you have a pointer to master, now get the number in the modellist
          //quick and dirty, just check all parameters of all components
          for (int k=0;k<modelnr;k++){
            Component* mastercomponent=mymodel->getcomponent(k);
            int compnr2=mastercomponent->get_nr_of_parameters();
            for (int l=0;l<compnr2;l++){
              if (masterparameter==mastercomponent->getparameter(l)){
                  projectfile <<"slavecomponent:"<< i <<"slaveparameter:"<< j <<",mastercomponent:"<< k <<"masterparameter:"<< l <<"\n";
              }
            }
          }
        }
      }
    }
    projectfile <<"#ENDOFCOUPLINGLIST\n";
    //save monitors
    projectfile <<"#MONITORLIST\n";
    for (int i=0;i<modelnr;i++){
      Component* mycomponent=mymodel->getcomponent(i);
      int compnr=mycomponent->get_nr_of_parameters();
      for (int j=0;j<compnr;j++){
        Parameter* myparameter=mycomponent->getparameter(j);
        if (myparameter->ismonitored()){
          //get a pointer to the second paramter of the monitor
          Monitor* mymonitor=myparameter->getmonitor();
          Parameter* p2=mymonitor->getpar2();
          //now you have a pointer to this parameter, now get the number in the modellist
          //quick and dirty, just check all parameters of all components
          for (int k=0;k<modelnr;k++){
            Component* p2component=mymodel->getcomponent(k);
            int compnr2=p2component->get_nr_of_parameters();
            for (int l=0;l<compnr2;l++){
              if (p2==(p2component->getparameter(l))){
                int type=mymonitor->gettype();
                projectfile <<"p2component:"<< k<<"p2:"<< l <<",p1component:"<< i <<"p1:"<< j <<"type:"<< type <<"\n";
              }
            }
          }
        }
      }
    }
    projectfile <<"#ENDOFMONITORLIST\n";

    projectfile <<"#ENDOFMODEL\n";

    //save the fitter settings
    if (myfitter!=0){
      projectfile <<"#FITTER\n";
      projectfile <<"FITTERTYPE:"<<myfitter->gettype()<<"\n";
      projectfile <<"FRACTION:"<<myfitter->getfraction()<<"\n";
      projectfile <<"MAXSTEP:"<<myfitter->getmaxstep()<<"\n";
      projectfile <<"MINSTEP:"<<myfitter->getminstep()<<"\n";
      projectfile <<"TOLERANCE:"<<myfitter->gettolerance()<<"\n";
      projectfile <<"USEGRADIENTS:"<<myfitter->getusegradients()<<"\n";
      projectfile <<"#ENDOFFITTER\n";
    }

/*
    //save positions of all open windows
    if (mymaintain!=0){
      int xpos;
      int ypos;
      int w,h;
      mymaintain->getposition(xpos,ypos,w,h);
      projectfile <<"#COMPONENTWINDOW:"<<xpos<<" : "<<ypos<<"\n";
       projectfile <<"W x H:"<<w<<" : "<<h<<"\n";
    }
    if (mydialog!=0){
      int xpos;
      int ypos;
      mydialog->getposition(xpos,ypos);
      projectfile <<"#FITTERWINDOW:"<<xpos<<" : "<<ypos<<"\n";
    }
*/

    //save all current parameter values in a multifit to be able to come back to the current fitting position
    if (mymodel->ismulti()){
        projectfile <<"#ALLPARAMS\n";
        for (size_t k=0;k<mymodel->getnrofspectra();k++){
            if (mymodel->hasstored(k)){
                for (size_t i=0;i<mymodel->getnrofallparams();i++){
                    projectfile <<mymodel->getuservalue(k,i)<<"\t"; //a fast way to get the parameters from the user storage
                    projectfile <<mymodel->getusersigma(k,i)<<"\t"; //a fast way to get the parameters from the user storage
                }
                projectfile <<"\n";
            }else{
                projectfile <<"no stored values for this spectrum\n"; //put also a tab there ,the reading is easier then...
            }
        }
    }

this->setCursor(Qt::ArrowCursor); //set to normal cursor
}

void Eelsmodel::load_project(Model*& mymodel,Fitter*& myfitter,std::string filename){
//***************************************
//*      load the whole project
//***************************************
//model, fitter etc


//check if a model exists
  if (mymodel!=0){
     switch( QMessageBox::warning( this, "EELSMODEL",
        "Loading a project overwrites the existing model.\n"
        "Continue? ",
        "Yes",
        "No", 0, 0, 1 ) )
      {
      case 1: // The user clicked no
        return;
      case 0: // The user clicked yes or pressed enter
        break;
      }
    }
//clean up all old mess
  if (mymaintain!=0) delete(mymaintain);
  mymaintain=0;
  if (mydialog!=0) delete(mydialog);
  mydialog=0;
  if (myfitter!=0) delete(myfitter);
  myfitter=0;
  if (mymodel!=0) delete(mymodel);
  mymodel=0;
  if (filename==""){
    //start to get a filename to load
    Getfilename myfile(0,"Select a file to load");
    //myfile.setdirname("~");
    myfile.setfilter("Projects (*.mod *.MOD)");
    myfile.setcaption("Select an eelsmodel project file to load");
#ifdef DEBUG_EELSMODEL
    std::cout <<"before getfilename open \n";
#endif
    filename=myfile.open();
    if (filename=="") throw EelsmodelErr::load_cancelled();//cancelled by user
  }
#ifdef DEBUG_EELSMODEL
  std::cout <<"the filename is: "<<filename <<"\n";
  #endif
  //try to open for reading to see if it exists
  std::ifstream projectfile(filename.c_str());
  if (!projectfile) {
         throw EelsmodelErr::load_error("Can not open file for reading");
      }
  projectfilename=filename; //store the project filename

//change to busy mouse indicator
this->setCursor(Qt::WaitCursor); //set to wait cursor



//set some variables
  std::string myline;
  std::string::size_type mypos;
  char eol='\n'; //eol, in future it should be determined from the file because it can be platform specific
  char cr='\r';
  Spectrum* HLspectrum=0;
  Multispectrum* HLmultispectrum=0;
  Model* newmodel=0;
  //reset fitter pointer
  if (myfitter!=0) delete(myfitter);
  myfitter=0;

  //load the model line by line
  //look for special keywords
  while (std::getline(projectfile,myline,eol)){
#ifdef DEBUG_EELSMODEL
    std::cout <<"myline: "<<myline<<"\n";
#endif

    if (myline.find("#MODEL")!=std::string::npos){
        //read the model part
         while (std::getline(projectfile,myline,eol)){
           #ifdef DEBUG_EELSMODEL
    std::cout <<"myline: "<<myline<<"\n";
#endif
          if (myline.find("#MODELHLFILENAME:")!=std::string::npos){
             mypos=myline.find("#MODELHLFILENAME:")+1+16; //careful filename contains ":" in win
             std::string nameandtype=myline.substr(mypos,std::string::npos);
             mypos=nameandtype.find_last_of(":")+1; //search from the backside because win contains ":" chars in filename
             std::string type=nameandtype.substr(mypos,std::string::npos);
             std::string HLfilename=nameandtype.substr(0,mypos-1);
             if (type.find(cr)!=std::string::npos)  type.erase(filename.find(cr));
             //load the HL spectrum
             #ifdef DEBUG_EELSMODEL
              std::cout <<"the filename is: "<<HLfilename <<" the type is"<<type<<"\n";
             #endif
             try{
              if (type=="MSA") {
                //load MSA file
                loadmsa l;
                HLspectrum=new Spectrum(l,HLfilename);
                }
              else if (type=="MULTI") {
                  //load a multispectrum in DM3 format
                  //open the DM3 image and convert to mspec
                  myimage=new Image(getworkspaceptr(),"Image",HLfilename);
                  //now take the mspec from the image
                  HLmultispectrum=myimage->getmspec();
                  }
                  else{
                   //oops a type that we don't know of
                   throw EelsmodelErr::load_error("Unknown HL file type during project load");
                  }

             }
             catch(Spectrum::Spectrumerr::load_cancelled()){
                this->setCursor(Qt::ArrowCursor); //set to normal cursor
                return; //cancelled by user
             }
             catch(Spectrum::Spectrumerr::load_error()){
                  this->setCursor(Qt::ArrowCursor); //set to normal cursor
                throw EelsmodelErr::load_error("Unexpected error during opening of spectrum");
                return;
             }

             if ((HLspectrum==0)&&(HLmultispectrum==0))  throw EelsmodelErr::load_error("No spectrum created, error during load");
             //open a graph for this spectrum
            if (HLspectrum!=0) HLspectrum->display(getworkspaceptr());
            if (HLmultispectrum!=0) HLmultispectrum->display(getworkspaceptr());
            //create a new model for this spectrum
            try{
              if (HLspectrum!=0){
                newmodel= new Model(HLspectrum);
                }
              else {
                newmodel= new Model(HLmultispectrum);
                //make connection to update model whenever the current
                //spectrum changes
                const Imagedisplay* myimdisplay=HLmultispectrum->getimageptr();
                connect( myimdisplay, SIGNAL(curr_spec_update() ),this, SLOT( slot_model_update() ) );
              }
            }
            catch(...){
                this->setCursor(Qt::ArrowCursor); //set to normal cursor
              throw EelsmodelErr::load_error("Unable to create new model");
            }
            //delete old model if new is created and old existed
            if ((newmodel!=0)&&(mymodel!=0)){
              delete(mymodel); //throw away old one

              mymodel=newmodel; //point to new one
              }
            if(newmodel==0) throw EelsmodelErr::load_error("Unable to create new model");
            if (mymodel==0) mymodel=newmodel;
            //show the model graph
            mymodel->display(getworkspaceptr());
            if (mymaintain!=0) delete(mymaintain);
            emit enablemodel(true);
            //open the componentmaintenance for the new model
            this->componentmaintenance();

            }
          if (myline.find("#PPPC:")!=std::string::npos){
          //for compatibility with older projects
             //read pppc
             mypos=myline.find(":")+1;
             std::string pppcstring=myline.substr(mypos,std::string::npos);
             double pppc=atof(pppcstring.c_str()); //convert to double
             if (mymodel!=0){
               mymodel->setdetectorgain(pppc);
             }
           }

          if (myline.find("#DETECTORNAME:")!=std::string::npos){
          //for compatibility with older projects
             //read pppc
             mypos=myline.find(":")+1;
             std::string detname=myline.substr(mypos,std::string::npos);
             if (mymodel!=0){
               mymodel->setdetectorname(detname);

             }
           }
            if (myline.find("#DETECTORGAIN:")!=std::string::npos){
          //new name for PPPC
             //read pppc
             mypos=myline.find(":")+1;
             std::string pppcstring=myline.substr(mypos,std::string::npos);
             double pppc=atof(pppcstring.c_str()); //convert to double
             if (mymodel!=0){
               mymodel->setdetectorgain(pppc);

             }
           }

           if (myline.find("#DETECTORCORRFACTOR:")!=std::string::npos){
          //for compatibility with older projects
             //read pppc
             mypos=myline.find(":")+1;
             std::string corrfactorstring=myline.substr(mypos,std::string::npos);
             double corrfactor=atof(corrfactorstring.c_str()); //convert to double
             if (mymodel!=0){
               mymodel->setdetectorcorrfactor(corrfactor);
             }
           }


          if (myline.find("#COMPONENTLIST")!=std::string::npos){
            //read components
            while (std::getline(projectfile,myline,eol)){
              if (myline.find("COMPONENTNAME:")!=std::string::npos){
                 mypos=myline.find(":")+1;
                 std::string compname=myline.substr(mypos,std::string::npos);

		 //remove CR character at end if needed
                 if (compname.find(cr)!=std::string::npos)  compname.erase(filename.find(cr));

		 //backward compatibility for new name for DOS, as suggested by G. Botton
		 if (compname=="Density of States"){
		   compname="Fine Structure (DOS)";
		 }

                 //create a new component by name
                 int index=mymodel->getcomponentindexbyname(compname);
                 if (index<0){
                     switch( QMessageBox::warning( this, "EELSMODEL",
                      "Trying to load an unknown component.\n"
                      "What should I do? ",
                      "Skip",
                      "Exit load project", 0, 0, 1 ) ){
                        case 1: // The user clicked no
                          return;
                        case 0: // The user clicked yes or pressed enter
                          break;
                        }
                    }
                  else{
                    //read the display name
                    std::string displayname="";
                    if (std::getline(projectfile,myline,eol)){
                       if (myline.find("COMPONENTDISPLAYNAME:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         displayname=myline.substr(mypos,std::string::npos);
                         }
                    }
                    //add the new component
                    std::vector<Parameter*> parameterlist;
                    //create the parameter list
                    Parameter* currentparameter=0;
                    while (std::getline(projectfile,myline,eol)){
                      if (myline.find("PARAMETERNAME:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parname=myline.substr(mypos,std::string::npos);
                         currentparameter=new Parameter(parname,0.0);
                         //put it in the vector (make a copy)
                         parameterlist.push_back(currentparameter);
                         }
                      else if (myline.find("PARAMETERVALUE:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parval=myline.substr(mypos,std::string::npos);
                         double value=atof(parval.c_str()); //convert to double
                         currentparameter->setvalue(value);
                         }
                      else if (myline.find("PARAMETERLOWERBOUND:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parval=myline.substr(mypos,std::string::npos);
                         double lbound=atof(parval.c_str()); //convert to double
                         currentparameter->setlowerbound(lbound);
                         }
                      else if (myline.find("PARAMETERUPPERBOUND:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parval=myline.substr(mypos,std::string::npos);
                         double ubound=atof(parval.c_str()); //convert to double
                         currentparameter->setupperbound(ubound);
                         }
                      else if (myline.find("PARAMETERCHANGEABLE:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parval=myline.substr(mypos,std::string::npos);
                         if (parval=="1") currentparameter->setchangeable(true);
                         else currentparameter->setchangeable(false);
                         }
                      else if (myline.find("PARAMETERBOUND:")!=std::string::npos){
                         mypos=myline.find(":")+1;
                         std::string parval=myline.substr(mypos,std::string::npos);
                         if (parval=="1") currentparameter->setbound(true);
                         else currentparameter->setbound(false);
                         }
                      else if (myline.find("ENDOFCOMPONENT:")!=std::string::npos){
                          break;//go to the next component
                         }
                      }
                    const int oldnr=mymodel->getcomponentsnr();
                    mymodel->addcomponent(index,&parameterlist);
                    const int newnr=mymodel->getcomponentsnr();
                    if (newnr>oldnr){
                      //only us it when it was really created
                      Component* mycomponent=mymodel->getlastcomponent();
                      mycomponent->setdisplayname(displayname);
                    }
                    parameterlist.clear(); //make clean for next component
                    this->componentmaintenance_updatescreen();
                    emit componentmaintenance_update();
                    }
                 }
               if (myline.find("#ENDOFCOMPONENTLIST")!=std::string::npos) break;
               }
              if (myline.find("#ENDOFMODEL")!=std::string::npos) break;
              }

    if (myline.find("#EXCLUDELIST")!=std::string::npos){
       //read the exclude regions
        while (std::getline(projectfile,myline,eol)){
          int index=atoi(myline.c_str()); //convert to int
          mymodel->setexclude(index,true);
          if (myline.find("#ENDOFEXCLUDELIST")!=std::string::npos) break;
          }
        }


      if (myline.find("#COUPLINGLIST")!=std::string::npos){
       //read coupling of parameters
        while (std::getline(projectfile,myline,eol)){
          //split the line in four parts
          std::string  sc="slavecomponent:";
          std::string  sp="slaveparameter:";
          std::string  mc="mastercomponent:";
          std::string  mp="masterparameter:";



          std::string::size_type pos1=myline.find(sc);
          std::string::size_type afterpos1=pos1+sc.length();
          std::string::size_type pos2=myline.find(sp);
          std::string::size_type afterpos2=pos2+sp.length();
          std::string::size_type pos3=myline.find(mc);
          std::string::size_type afterpos3=pos3+mc.length();
          std::string::size_type pos4=myline.find(mp);
          std::string::size_type afterpos4=pos4+mp.length();
          int slavecomp=0;
          int slavepar=0;
          int mastercomp=0;
          int masterpar=0;

          if ((pos1!=std::string::npos)&&(pos2!=std::string::npos)&&(pos3!=std::string::npos)&&(pos4!=std::string::npos)){
            std::string slavecompstring=myline.substr(afterpos1,pos2);
            slavecomp=atoi(slavecompstring.c_str()); //convert to int
            std::string slaveparstring=myline.substr(afterpos2,pos3);
            slavepar=atoi(slaveparstring.c_str()); //convert to int
            std::string mastercompstring=myline.substr(afterpos3,pos4);
            mastercomp=atoi(mastercompstring.c_str()); //convert to int
            std::string masterparstring=myline.substr(afterpos4,std::string::npos);
            masterpar=atoi(masterparstring.c_str()); //convert to int
            #ifdef DEBUG_EELSMODEL
            std::cout <<"coupling part of model\n";
            std::cout <<"slavecomponent string :"<< slavecompstring  << " to int:"   <<slavecomp <<"\n";
            std::cout <<"slaveparameter string :"<< slaveparstring   << " to int:"   <<slavepar  <<"\n";
            std::cout <<"mastercomponent string:"<< mastercompstring << " to int:"   <<mastercomp<<"\n";
            std::cout <<"masterparameter string:"<< masterparstring  << " to int:"   << masterpar<<"\n";
            #endif

            //now apply the coupling
            Component* slavecompptr=0;
            Component* mastercompptr=0;
            Parameter* slaveparptr=0;
            Parameter* masterparptr=0;
            try{
              slavecompptr=mymodel->getcomponent(slavecomp);
              mastercompptr=mymodel->getcomponent(mastercomp);
              slaveparptr=slavecompptr->getparameter(slavepar);
              masterparptr=mastercompptr->getparameter(masterpar);
              slaveparptr->couple(masterparptr,1.0);

              }
            catch(...){
                this->setCursor(Qt::ArrowCursor); //set to normal cursor
              //something went wrong, just skip the coupling part
              Saysomething mysay(0,"Error","Error adding coupling, coupling skipped",true);
              break;
              }
            }
          if (myline.find("#ENDOFCOUPLINGLIST")!=std::string::npos) {
            //update to show the right coupled icons...
            this->componentmaintenance_updatescreen();
            emit componentmaintenance_update();
            break;
            }
          }
        }

      if (myline.find("#MONITORLIST")!=std::string::npos){
       //read the monitors
        while (std::getline(projectfile,myline,eol)){
          //split the line in four parts
          std::string  sc="p2component:";
          std::string  sp="p2:";
          std::string  mc="p1component:";
          std::string  mp="p1:";
          std::string  tp="type:";

          std::string::size_type pos1=myline.find(sc);
          std::string::size_type afterpos1=pos1+sc.length();
          std::string::size_type pos2=myline.find(sp);
          std::string::size_type afterpos2=pos2+sp.length();
          std::string::size_type pos3=myline.find(mc);
          std::string::size_type afterpos3=pos3+mc.length();
          std::string::size_type pos4=myline.find(mp);
          std::string::size_type afterpos4=pos4+mp.length();
          std::string::size_type pos5=myline.find(tp);
          std::string::size_type afterpos5=pos5+tp.length();

          int p2comp=0;
          int p2par=0;
          int p1comp=0;
          int p1par=0;
          int type=0;

          if ((pos1!=std::string::npos)&&(pos2!=std::string::npos)&&(pos3!=std::string::npos)&&(pos4!=std::string::npos)&&(pos5!=std::string::npos)){
            std::string p2compstring=myline.substr(afterpos1,pos2);
            p2comp=atoi(p2compstring.c_str()); //convert to int
            std::string p2parstring=myline.substr(afterpos2,pos3);
            p2par=atoi(p2parstring.c_str()); //convert to int
            std::string p1compstring=myline.substr(afterpos3,pos4);
            p1comp=atoi(p1compstring.c_str()); //convert to int
            std::string p1parstring=myline.substr(afterpos4,pos5);
            p1par=atoi(p1parstring.c_str()); //convert to int
            std::string typestring=myline.substr(afterpos5,std::string::npos);
            type=atoi(typestring.c_str()); //convert to int


            #ifdef DEBUG_EELSMODEL
            std::cout <<"monitor part of model\n";
            std::cout <<"p2component string :"<< p2compstring  << " to int:"   <<p2comp <<"\n";
            std::cout <<"p2parameter string :"<< p2parstring   << " to int:"   <<p2par  <<"\n";
            std::cout <<"p1component string:"<< p1compstring << " to int:"   <<p1comp<<"\n";
            std::cout <<"p1parameter string:"<< p1parstring  << " to int:"   << p1par<<"\n";
            std::cout <<"type string:"<< typestring  << " to int:"   << type<<"\n";
            #endif

            //now apply the monitor
            Component* p1compptr=0;
            Component* p2compptr=0;
            Parameter* p1parptr=0;
            Parameter* p2parptr=0;
            try{
              p1compptr=mymodel->getcomponent(p1comp);
              p2compptr=mymodel->getcomponent(p2comp);
              p1parptr=p1compptr->getparameter(p1par);
              p2parptr=p2compptr->getparameter(p1par);
              Monitor* mymonitor=new Monitor(p1parptr,p2parptr,type);
              (void) mymonitor;
              //p1parptr->setmonitor(mymonitor);
              }
            catch(...){
                this->setCursor(Qt::ArrowCursor); //set to normal cursor
              //something went wrong, just skip the coupling part
              Saysomething mysay(0,"Error","Error adding monitor, skipped",true);
              break;
              }
            }
          if (myline.find("#ENDOFMONITORLIST")!=std::string::npos) {
            //update to show the right icons...
            this->componentmaintenance_updatescreen();
            emit componentmaintenance_update();
            break;
            }
          }
        }




    if (myline.find("#FITTER")!=std::string::npos){
       //read the fitter part

       while (std::getline(projectfile,myline,eol)){
          if (myline.find("FITTERTYPE:")!=std::string::npos){
              mypos=myline.find(":")+1;
              std::string type=myline.substr(mypos,std::string::npos);
              //create a fitter of type
              //a bit awkward but...
              Fitter* fitter1=new MLFitter(mymodel);
              Fitter* fitter2=new LSQFitter(mymodel);
              Fitter* fitter3=new WLSQFitter(mymodel);

              //disable GSL fitter in non-unix

              #ifdef GSLFITTER
              Fitter* fitter4=new MLFitterGSL(mymodel);
#endif
              Fitter* fitter5=new LevenbergMarquardt(mymodel);

              std::string f1type=fitter1->gettype();
              std::string f2type=fitter2->gettype();
              std::string f3type=fitter3->gettype();
               #ifdef GSLFITTER
              std::string f4type=fitter4->gettype();
#endif
              std::string f5type=fitter5->gettype();

              if (f1type==type){
                 myfitter=fitter1;
                 delete(fitter2);
                 delete(fitter3);
                      #ifdef GSLFITTER
                 delete(fitter4);
                 #endif
                  delete(fitter5);
                 }
              if (f2type==type){
                 myfitter=fitter2;
                 delete(fitter1);
                 delete(fitter3);
                  #ifdef GSLFITTER
                 delete(fitter4);
                 #endif
                 delete(fitter5);
                 }
              if (f3type==type){
                 myfitter=fitter3;
                 delete(fitter1);
                 delete(fitter2);
                  #ifdef GSLFITTER
                 delete(fitter4);
                 #endif
                 delete(fitter5);
                 }
 #ifdef GSLFITTER
               if (f4type==type){
                 myfitter=fitter4;
                 delete(fitter1);
                 delete(fitter2);
                 delete(fitter3);
                 delete(fitter5);
                 }
#endif
               if (f5type==type){
                 myfitter=fitter5;
                 delete(fitter1);
                 delete(fitter2);
                 delete(fitter3);
                  #ifdef GSLFITTER
                 delete(fitter4);
                 #endif

                 }


              if (myfitter==0) throw EelsmodelErr::load_error("Unable to create new fitter");
              //myfitter exists now

              }
          if (myline.find("FRACTION:")!=std::string::npos){
               mypos=myline.find(":")+1;
               std::string fractionstring=myline.substr(mypos,std::string::npos);
               double fraction=atof(fractionstring.c_str()); //convert to double
               myfitter->setfraction(fraction);
              }
          if (myline.find("MAXSTEP:")!=std::string::npos){
               mypos=myline.find(":")+1;
               std::string maxstepstring=myline.substr(mypos,std::string::npos);
               double maxstep=atof(maxstepstring.c_str()); //convert to double
               myfitter->setmaxstep(maxstep);
              }
          if (myline.find("MINSTEP:")!=std::string::npos){
               mypos=myline.find(":")+1;
               std::string minstepstring=myline.substr(mypos,std::string::npos);
               double minstep=atof(minstepstring.c_str()); //convert to double
               myfitter->setminstep(minstep);
              }
          if (myline.find("TOLERANCE:")!=std::string::npos){
               mypos=myline.find(":")+1;
               std::string tolerancestring=myline.substr(mypos,std::string::npos);
               double tolerance=atof(tolerancestring.c_str()); //convert to double
               myfitter->settolerance(tolerance);
              }
          if (myline.find("USEGRADIENTS:")!=std::string::npos){
               mypos=myline.find(":")+1;
               std::string usegradientstring=myline.substr(mypos,std::string::npos);
               if (usegradientstring=="1") myfitter->setusegradients(true);
               else myfitter->setusegradients(false);
              }
          if (myline.find("#ENDOFFITTER")!=std::string::npos) {
            if (myfitter!=0){
              fitter_dialog();//open the fitter dialog
              emit fitter_updatescreen();
            }
          break;
          }
        }
      }

    //read positions of windows
    if (myline.find("#COMPONENTWINDOW:")!=std::string::npos){
          mypos=myline.find(":")+1;
	      std::string::size_type mypos2=myline.rfind(":")+1;
          std::string xstring=myline.substr(mypos,mypos2);
	      std::string ystring=myline.substr(mypos2,std::string::npos);
	      const int x=atoi(xstring.c_str()); //convert to int
	      const int y=atoi(ystring.c_str()); //convert to int
	      //read the w and h
	      std::getline(projectfile,myline,eol);
	      mypos=myline.find(":")+1;
	      mypos2=myline.rfind(":")+1;
          xstring=myline.substr(mypos,mypos2);
	      ystring=myline.substr(mypos2,std::string::npos);
	      const int w=atoi(xstring.c_str()); //convert to int
	      const int h=atoi(ystring.c_str()); //convert to int
	      if (mymaintain!=0){
		mymaintain->slot_setposition(x,y,w,h);
		mymaintain->update();
	      }
    }

    if (myline.find("#FITTERWINDOW:")!=std::string::npos){
          mypos=myline.find(":")+1;
	      std::string::size_type mypos2=myline.rfind(":")+1;
          std::string xstring=myline.substr(mypos,mypos2);
	      std::string ystring=myline.substr(mypos2,std::string::npos);
	      const int x=atoi(xstring.c_str()); //convert to int
	      const int y=atoi(ystring.c_str()); //convert to int
	      if (mydialog!=0){
		  mydialog->slot_setposition(x,y);
	      }
    }

    //load all parameters in a multifit
    if ((myline.find("#ALLPARAMS")!=std::string::npos)&&(mymodel->ismulti())){
        #ifdef DEBUG_EELSMODEL
        std::cout <<" reading allparams\n";
        #endif
        for (size_t k=0;k<mymodel->getnrofspectra();k++){
            mymodel->setcurrentspectrum(k,false); //set spectrum but don't redraw
            std::getline(projectfile,myline,eol); //read the line
            std::istringstream linestream(myline);
            std::string token;
            std::string token_sigma;
            if (myline!="no stored values for this spectrum"){
                for (size_t i=0;i<mymodel->getnrofallparams();i++){ //read all parameters
                    //read part of the string up to the first tab
                    //second part is the sigma value
                    std::getline(linestream, token, '\t');
                    std::getline(linestream, token_sigma, '\t');
                    //convert token to value
                    const double value=atof(token.c_str()); //convert to double
                    const double sigma=atof(token_sigma.c_str()); //convert to double
                    mymodel->setuservalue(k,i,value);
                    mymodel->setusersigma(k,i,sigma);
                    mymodel->storeresultstring("as loaded\t",k);
                    mymodel->storemonitorstring(k,"?\t?");

                    #ifdef DEBUG_EELSMODEL
                    std::cout <<"token:"<<token<<" converted to float="<<value<<"\n";
                    std::cout <<"token_sigma:"<<token_sigma<<" converted to float="<<sigma<<"\n";
                    #endif
                }
                std::getline(linestream, token, eol); //read away the eol
            }
            else{
                #ifdef DEBUG_EELSMODEL
                std::cout <<"line skipped\n";
                #endif
            }
        }

        mymodel->setcurrentspectrum(0,true); //set to spectrum 0 and redraw
        mymodel->updatemonitors(); //fill in the correct monitor
      }

   }
  }
 }
 //do the model, to make sure the display is correct
 #ifdef DEBUG_EELSMODEL
        std::cout <<" end of load model\n";
 #endif
  this->setCursor(Qt::ArrowCursor); //set to normal cursor
}
void Eelsmodel::slot_save_model(){
//***************************************
//*      saves the model and its components as a tab delimitted file
//*      easily readable in a spreadsheet
//***************************************
//model, fitter etc

//check if a model exists
  if (mymodel==0) throw EelsmodelErr::save_error("No model found, a report\n is only meaningful when you have at least a model");
//start to get a filename to save
    Getfilename myfile(0,"Select a file to save");
    //myfile.setdirname("~");
    myfile.setfilter("Reports (*.txt *.TXT)");
    myfile.setcaption("Select an eelsmodel report file to save");
    std::string filename=myfile.save();
  if (filename=="") return;//cancelled by user

  #ifdef DEBUG_EELSMODEL
  std::cout <<"the filename is: "<<filename <<"\n";
  #endif
  //try to open for reading to see if it exists
  std::ifstream dummy(filename.c_str());
  if (dummy) {
    switch( QMessageBox::warning( this, "EELSMODEL",
        "This file already exists.\n"
        "Overwrite? ",
        "Yes",
        "No", 0, 0, 1 ) )
      {
      case 1: // The user clicked no
        return;
      case 0: // The user clicked yes or pressed enter
        break;
      }
   }
  //open this file for writing ascii
  std::ofstream projectfile(filename.c_str());
  if (!projectfile) {
  throw EelsmodelErr::save_error("Can not open file for writing");
  }
this->setCursor(Qt::WaitCursor); //set to wait cursor
  //make a short note on which file this model was for
  projectfile <<"EELSMODEL model file\n";
  QDateTime now=QDateTime::currentDateTime();
  projectfile <<now.toString().toStdString()<<"\n";
  projectfile <<"FILENAME of HL spectrum:"<< (mymodel->getHLptr())->getfilename() <<"\n";
  projectfile <<"NAME of HL spectrum:"<< (mymodel->getHLptr())->getname() <<"\n";



  //set the precision of numbers
  //8 digits in engineering format
  projectfile << std::ofstream::scientific;
  projectfile.precision(8);

  //print the names of the different components
  projectfile <<"\n";

  projectfile << "energy\texperiment\tmodel";

  //run over all components
  for (size_t j=0;j<(mymodel->getcomponentsnr());j++){
    //print the name of all components
    const Component* mycomponent=mymodel->getcomponent(j);
    std::string namestring=mycomponent->getdisplayname();
    if (namestring==""){
      namestring=mycomponent->getname();
    }
    projectfile << "\t"<<namestring;
  }
  projectfile << "\n";

  //print the values
  //run over all multispectra
  size_t currspec=mymodel->getcurrspecnr(); //get current spectrum number (0 if this is not a multispectrum)

  for (size_t k=0;k<(mymodel->getnrofspectra());k++) {
  mymodel->setcurrentspectrum(k,false); //select spectrum but don't update graphs, has no effect if we only have 1 spectrum
  for (size_t i=0;i<(mymodel->getnpoints());i++){
    projectfile << mymodel->getenergy(i)<<"\t"<<mymodel->getconstHLptr()->getcounts(i) <<"\t"<<mymodel->getcounts(i);
    for (size_t j=0;j<(mymodel->getcomponentsnr());j++){
      const Component* mycomponent=mymodel->getcomponent(j);
      projectfile << "\t"<<mycomponent->getcounts(i);
    }
    projectfile << "\n";
  }
}
  mymodel->setcurrentspectrum(currspec,true); //set back to spectrum that we had
this->setCursor(Qt::ArrowCursor); //set to normal cursor
}









