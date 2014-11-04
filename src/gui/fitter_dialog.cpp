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
 * eelsmodel - gui/fitter_dialog.cpp
 **/

#include "src/gui/fitter_dialog.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPoint>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QToolTip>
#include <QValidator>
#include <QVBoxLayout>
#include <QWorkspace>

#include "src/core/eelsmodel.h"
#include "src/core/multispectrum.h"

#include "src/fitters/fitter.h"

#include "src/gui/getfilename.h"

Eelsmodel* geteelsmodelptr();

Fitter_dialog::Fitter_dialog(QWorkspace *parent, const char *name,Fitter* myfitter)
: QWidget(parent)
{
    this->setWindowTitle(name);
    parent->addWindow(this); //add it explicitly to the workspace
    previous=false;
    startfromcurrent=false;
    redrawfit=true;
    //fill out initial values of the parameter
    fitterptr=myfitter;
    minstep=myfitter->getminstep();
    maxstep=myfitter->getmaxstep();
    fraction=myfitter->getfraction();
    tolerance=myfitter->gettolerance();
    usegradients=myfitter->getusegradients();
    chisq=0.0;
    // the way numbers are displayed
    int fieldwidth=-1;
    char format='e';
    int precision=6;

    //create a fill out box with parameter values
    std::string s="Fitter options: "+myfitter->gettype();
    this->setWindowTitle(s.c_str());

    //validator check for numeric input and check bounds
    double_validator=new QDoubleValidator(this);
    int_validator=new QIntValidator(this);

    // Create Widgets which allow easy layouting
    QGridLayout *lay = new QGridLayout( this );


    //box1->addWidget( bgrp2 );
    lay->setMargin(5);
    lay->setSpacing(2);

    QString valstring;
    valstring="%1"; //needed for MacosX
    //chooser for the type of fitter

    //label for displaying goodness of fit
    chisqlabel=new QLabel(valstring.arg (chisq,fieldwidth,format,precision ));
    chisqlabel->setText((fitterptr->goodness_of_fit_string()).c_str());
    lay->addWidget(chisqlabel,0,0,1,3);

    confidencelabel=new QLabel(valstring.arg (chisq,fieldwidth,format,precision ));
    confidencelabel->setText((fitterptr->LRtestconfidence_string()).c_str());
    lay->addWidget(confidencelabel,1,0,1,3);

    //minstep edit

    QLineEdit* minstepedit=new QLineEdit(valstring.arg (minstep,fieldwidth,format,precision ));
    QLabel* minsteplabel=new QLabel("Smallest parameter change step");
    lay->addWidget(minstepedit,2,0);
    lay->addWidget(minsteplabel,2,1,1,2);
    minstepedit->setValidator (double_validator);  //make sure only doubles are entered

    //maxstep edit
    QLineEdit* maxstepedit=new QLineEdit(valstring.arg (maxstep,fieldwidth,format,precision ));
    QLabel* maxsteplabel=new QLabel("Largest parameter change step");
    (void) maxsteplabel;//get rid of unused variable warning
    maxstepedit->setValidator (double_validator);  //make sure only doubles are entered
    lay->addWidget(maxstepedit,3,0);
    lay->addWidget(maxsteplabel,3,1,1,2);

    //fraction edit
    QLineEdit* fractionedit=new QLineEdit(valstring.arg (fraction,fieldwidth,format,precision ));
    QLabel* fractionlabel=new QLabel("Fraction of param. for numerical derivative");
    (void) fractionlabel;//get rid of unused variable warning
    fractionedit->setValidator (double_validator);  //make sure only doubles are entered
    lay->addWidget(fractionedit,4,0);
    lay->addWidget(fractionlabel,4,1,1,2);

    //tolerance edit
    QLineEdit* toleranceedit=new QLineEdit(valstring.arg (tolerance,fieldwidth,format,precision ));
    QLabel* tolerancelabel=new QLabel("Tolerance");
    (void) tolerancelabel;//get rid of unused variable warning
    toleranceedit->setValidator (double_validator);  //make sure only doubles are entered
    lay->addWidget(toleranceedit,5,0);
    lay->addWidget(tolerancelabel,5,1,1,2);


    //number of itterations edit
    nmax=fitterptr->getnmax();
    QLineEdit* nmaxedit=new QLineEdit(valstring.arg ((double)nmax,fieldwidth,format,precision ));
    QLabel* nmaxlabel=new QLabel("Max. number of itterations");
    (void) nmaxlabel;//get rid of unused variable warning
    nmaxedit->setValidator (int_validator);  //make sure only doubles are entered
    lay->addWidget(nmaxedit,6,0);
    lay->addWidget(nmaxlabel,6,1,1,2);



    // insert checkboxes
    check_gradients=new QCheckBox( "Use analytical gradients" );
    QCheckBox * check_update = new QCheckBox( "Update graph each iteration" );
    QCheckBox * check_multi = new QCheckBox( "Fit all in multispectrum");       
    QCheckBox * check_previous = new QCheckBox( "Start from previous (multifit)" );
    
    
    QCheckBox * check_residual = new QCheckBox( "Generate residuals" );
    QCheckBox * check_redrawfit = new QCheckBox( "Redraw during multifit" );
    QCheckBox * check_lintrick = new QCheckBox( "Use Linear fit if possible" );
    
    QCheckBox * check_startfromcurrent = new QCheckBox( "Start multifit from current spectrum" );
    
    lay->addWidget(check_gradients,7,0);
    lay->addWidget(check_update,8,0);
    lay->addWidget(check_multi,9,0);
    lay->addWidget(check_previous,10,0);
    lay->addWidget(check_residual,7,1);
    lay->addWidget(check_redrawfit,8,1);
    lay->addWidget(check_lintrick,9,1);
    lay->addWidget(check_startfromcurrent,10,1);

    
    
    check_gradients->setChecked( usegradients );
    check_multi->setChecked(false);
    check_previous->setChecked(false);
    check_startfromcurrent->setChecked(false);
    check_residual->setChecked(false);
    check_redrawfit->setChecked(redrawfit);
    check_lintrick->setChecked(myfitter->getdolintrick());
    check_lintrick->setEnabled(myfitter->candolintrick());
    //only enable multi and previous when it is a multispectrum model
    check_multi->setEnabled(myfitter->getmulti());
    check_previous->setEnabled(myfitter->getmulti());
    check_startfromcurrent->setEnabled(myfitter->getmulti());
    check_redrawfit->setEnabled(myfitter->getmulti());

    update_allways=false;
    check_update->setChecked( update_allways );

     //OK button

     //QPushButton *OK = new QPushButton( "&OK", buttonbox );
     QPushButton *iterations = new QPushButton( "Do Iterations" );
     QPushButton *orig = new QPushButton( "Restore original values" );
     lay->addWidget(iterations,11,0);
     lay->addWidget(orig,11,1);


     stopbutton = new QPushButton( "Stop fitting");
     stopbutton->setEnabled(false);
     lay->addWidget(stopbutton,11,2);


     QPushButton *params = new QPushButton( "All params to current");
     params->setToolTip("set all parameters of all spectra to the parameters of the current fit.\n this is a good idea before starting a fresh fit.");

     QPushButton *paramsave = new QPushButton( "Save params as .dat");
     paramsave->setToolTip("stores all parameters in a binary file, \ncan be loaded by DM with import 8byte real\n AxBxC with A,B dimension of original SI (for 2D SI) \n and C the number of parameters (read on top of model list)");

     QPushButton *paramsavetxt = new QPushButton( "Save params as .txt" );
     paramsavetxt->setToolTip("stores all parameters in a tab separated txt file");


     QPushButton *crlbsave = new QPushButton( "Save crlb" );
     crlbsave->setToolTip("stores the estimated precision of the parameters in a binary file, \ncan be loaded by DM with import 8byte real\n AxBxC with A,B dimension of original SI (for 2D SI) \n and C the number of parameters (read on top of model list)");

     
     QPushButton *modelsave = new QPushButton( "Save model as .dat" );
     modelsave->setToolTip("stores the calculated model in a binary file, \ncan be loaded by DM with import 8byte real\n AxBxC with A,B,C dimension of original SI (for 2D SI) or A,B for 1D SI");

       
     
    lay->addWidget(params,12,0);
    lay->addWidget(paramsave,12,1);
    lay->addWidget(paramsavetxt,12,2);
    lay->addWidget(modelsave,13,0);
    
     //progress bar

     pbar=new QProgressBar ();
     pbar->setMinimum(0);
     lay->addWidget(pbar,14,0,1,3);
     pbar->reset();

    //connections
     connect( minstepedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_minstep(const QString&) ) );
     connect( maxstepedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_maxstep(const QString&) ) );
     connect( fractionedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_fraction(const QString&) ) );
     connect( toleranceedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_tolerance(const QString&) ) );
     connect( nmaxedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_nmax(const QString&) ) );
     connect( check_update, SIGNAL( toggled(bool)), this, SLOT( slot_set_update_allways(bool) ) );
     connect( check_gradients, SIGNAL( toggled(bool)), this, SLOT( slot_set_usegradients(bool) ) );
     connect( check_residual, SIGNAL( toggled(bool)), this, SLOT( slot_set_residual(bool) ) );
     connect( check_multi, SIGNAL( toggled(bool)), this, SLOT( slot_set_multi(bool) ) );
     connect( check_lintrick, SIGNAL( toggled(bool)), this, SLOT( slot_set_lintrick(bool) ) );
     connect( check_previous, SIGNAL( toggled(bool)), this, SLOT( slot_set_previous(bool) ) );
     connect( check_startfromcurrent, SIGNAL( toggled(bool)), this, SLOT( slot_set_startfromcurrent(bool) ) );
     connect( check_redrawfit, SIGNAL( toggled(bool)), this, SLOT( slot_set_redrawfit(bool) ) );
     connect( iterations, SIGNAL( clicked()), this, SLOT( slot_iterations() ) );
     connect( params, SIGNAL( clicked()), this, SLOT( slot_params() ) );

     connect( paramsave, SIGNAL( clicked()), this, SLOT(slot_params_save_dat() ) );
     connect( paramsavetxt, SIGNAL( clicked()), this, SLOT(slot_params_save_txt() ) );
     connect( crlbsave, SIGNAL( clicked()), this, SLOT( slot_params_save_crlb_dat() ) );
     connect( modelsave, SIGNAL( clicked()), this, SLOT( slot_model_save_dat() ) );
     
     connect( stopbutton, SIGNAL( clicked()), this, SLOT( slot_stop() ) );
     connect( orig, SIGNAL( clicked()), this, SLOT( slot_undo() ) );
     connect (this,SIGNAL(signal_progressbar(int)),pbar,SLOT(setValue(int)));
     this->show();
     this->setMinimumSize(this->minimumSizeHint()); //needed since QT4 to make sure the widget is not shown minimized
}
Fitter_dialog::~Fitter_dialog(){
  //tell eelsmodel that we are off
   emit signal_fitterdialog_died();
}



void Fitter_dialog::slot_change_minstep(const QString& valstring){
  minstep=getnumber(valstring);
  fitterptr->setminstep(minstep);
}
void Fitter_dialog::slot_change_maxstep(const QString& valstring){
  maxstep=getnumber(valstring);
  fitterptr->setmaxstep(maxstep);
}
void Fitter_dialog::slot_change_fraction(const QString& valstring){
  fraction=getnumber(valstring);
  fitterptr->setfraction(fraction);
}

void Fitter_dialog::slot_change_tolerance(const QString& valstring){
  tolerance=getnumber(valstring);
  fitterptr->settolerance(tolerance);
}

void Fitter_dialog::slot_change_nmax(const QString& valstring){
  int n=int(getnumber(valstring));
  if (n<1) {
    n=1;
  }
  fitterptr->setnmax((unsigned int)n);
}

double Fitter_dialog::getnumber(const QString& valstring)const{
  bool ok;
  double result=valstring.toDouble(&ok);
  if (ok) return result;
  else return 0.0;
}

void Fitter_dialog::slot_stop(){
stop=true;
stopbutton->setEnabled(false);
stopbutton->repaint();
pbar->reset();
this->setWindowTitle("Stopped by user");
confidencelabel->setText((fitterptr->LRtestconfidence_string()).c_str());
  confidencelabel->repaint();
}
void Fitter_dialog::slot_update(){
QString valstring;
  // the way numbers are displayed
    int fieldwidth=-1;
    char format='e';
    int precision=6;
  fitterptr->createmodelinfo();
valstring="%1"; //required by MacOSX otherwise empty text window
  minstep=fitterptr->getminstep();
  minstepedit->setText(valstring.arg (minstep,fieldwidth,format,precision ));

  maxstep=fitterptr->getmaxstep();
  maxstepedit->setText(valstring.arg (maxstep,fieldwidth,format,precision ));

  fraction=fitterptr->getfraction();
  fractionedit->setText(valstring.arg (fraction,fieldwidth,format,precision ));

  tolerance=fitterptr->gettolerance();
  toleranceedit->setText(valstring.arg (tolerance,fieldwidth,format,precision ));

  usegradients=fitterptr->getusegradients();
  check_gradients->setChecked( usegradients );

  confidencelabel->setText((fitterptr->LRtestconfidence_string()).c_str());
  confidencelabel->repaint();
}
void Fitter_dialog::slot_set_update_allways(bool b){
update_allways=b;
}
void Fitter_dialog::slot_set_usegradients(bool b){
usegradients=b;
fitterptr->setusegradients(usegradients);
}
void Fitter_dialog::slot_set_residual(bool b){
residual=b;
fitterptr->setdoresidual(residual);
}
void Fitter_dialog::slot_set_multi(bool b){
fitterptr->setdomulti(b);
}

void Fitter_dialog::slot_set_lintrick(bool b){
    fitterptr->dolintrick(b);
}

void Fitter_dialog::slot_set_startfromcurrent(bool b){
    startfromcurrent=b;
}

void Fitter_dialog::slot_set_previous(bool b){
previous=b;
}


void Fitter_dialog::slot_undo(){
  //get the parameters back that where valid before doing the itteration
  (fitterptr->modelptr)->retrieveparams();
  (fitterptr->modelptr)->updateHL(); //and redraw
}

void Fitter_dialog::iterate_this_spectrum(){

    //itteration loop for 1 spectrum

    //do the GUI stuff
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    std::string s="Fitter options: "+fitterptr->gettype();
    this->setWindowTitle(s.c_str());

    int n=1;
    if (fitterptr->getdolintrick()){
        n=fitterptr->getnumberofspectra(); //only one itteration in case of linear fitting
    }else{
        pbar->reset();
        n=fitterptr->getnmax(); //max number of itterations
    }
    pbar->setMaximum(n);
    QString valstring;
    stop=false;
    stopbutton->setEnabled(true);

    //stop button doesn't work!!!!!!!!!!!!!!!!
    stopbutton->setAutoRepeat(true);
    stopbutton->repaint();

    //prepare the model for fitting
    Model* mptr=(fitterptr->getmodelptr());
    mptr->updateHLnodraw(); //force the model to update its HL pointer, but don't draw, only draw when finished
    if (mptr->ismulti()){
        (mptr->getmultispectrumptr())->update();//advance the line tool on the multispectrum
    }
    //fitterptr->createmodelinfo(); //make sure the fitter has up to date info from the model
    fitterptr->initfitter(); //make sure the fitter has up to date info from the model
    mptr->storeparams(); //to be able to come back in case of divergence

    //do the fitting
    for (int i=0;(i<n)&&(!this->getstop());i++){
        fitterptr->iterate(1);
        chisqlabel->setText((fitterptr->goodness_of_fit_string()).c_str());
        chisqlabel->repaint(); //update the chisquare value
        if (fitterptr->getdolintrick()){
            pbar->setValue((fitterptr->getmodelptr())->getcurrspecnr()+1);
        }
        else{
            pbar->setValue(i+1);
        }
        this->setWindowTitle((fitterptr->getstatus()).c_str()); //show in caption what the fitter has to say
        if (update_allways) emit(update());
        qApp->processEvents();
        if (stop) break;
        if (fitterptr->getdolintrick()) break; //only one iterration for linear fitting
        if (fitterptr->converged()){
            //tolerance reached
            pbar->setValue(n);
            this->setWindowTitle("Tolerance reached");
            break;
        }
    }

    //store the current parameters in the user storage, and update the model and display to the final converged model
    fitterptr->CRLB(); //fill in the precision estimates before storing values and sigmas

    if (redrawfit) {
        mptr->updateHL();
        emit(update()); //update the screen after each fit
    }
    else{
         mptr->updateHLnodraw();
    }
    //fitterptr->updatemonitors(); //update sigma for the report and for the monitors in the componentmaintneance
     geteelsmodelptr()->slot_componentmaintenance_updatemonitors(); //and update the monitors
     if (fitterptr->getdoresidual()){
        fitterptr->updateresidual(); //calculate the residuals
    }


    std::string result="";
    std::ostringstream s1;
    if (s1 << fitterptr->goodness_of_fit()<<"\t"<< fitterptr->LRtestconfidence()){
	     // conversion worked
	     result=s1.str();
    }
    mptr->storeuserparams(result);//keep parameters for later saving etc, or as starting value for next fit

    //show the result

    confidencelabel->setText((fitterptr->LRtestconfidence_string()).c_str());
    confidencelabel->repaint();

   //reset GUI to normal state
   stopbutton->setEnabled(false);
   stopbutton->repaint();
   confidencelabel->setText((fitterptr->LRtestconfidence_string()).c_str());
   confidencelabel->repaint();
   QApplication::restoreOverrideCursor(); // we're done
}

void Fitter_dialog::slot_iterations(){
//total itteration loop
//either multifit or a single fit
	
Model* mptr=fitterptr->getmodelptr();
if (mptr->getnroffreeparameters()==0){
	QMessageBox::warning( this, "EELSMODEL","No free parameters to fit","OK", 0, 0, 1 );	      
	return;
}
    fitterptr->createmodelinfo(); //make sure the fitter has up to date info from the model
    if (fitterptr->getdomulti()){
      int nspec=mptr->getnrofspectra();
      //store the current parameters
      const size_t currentindex=(mptr->getmultispectrumptr())->getcurrentslice(); //keep index of current spectrum
      size_t startindex=0;
      //ask the user if he wants to start from current or from zero
      if (startfromcurrent) startindex=currentindex;
      
      for (size_t j=startindex;j<nspec;j++){
        if (previous) {
          //change current spec nr to the new spectrum, but don't change anything else
          (mptr->getmultispectrumptr())->setcurrentslice(j);
          //store the current parameters for this new spectru
          std::string result="";
          result.append(fitterptr->goodness_of_fit_string());
          result.append("\t");
          result.append(fitterptr->LRtestconfidence_string());
          mptr->storeuserparams(result);
          //the next step will then take these parameters
        }

        //select the spectrum we want and do the itterations on it
        mptr->setcurrentspectrum(j,false); //no redraw
        this->iterate_this_spectrum(); //signal first if j=0
        if (stop) break;
      }
      emit(update()); //update the screen after each fit
      //come back to the original state
      //mptr->setcurrentspectrum(currentindex,true); //with redraw
      //(mptr->getmultispectrumptr())->update();//update the line tool on the multispectrum
    }
    else{ //a single fit
        this->iterate_this_spectrum();
    }
}

void Fitter_dialog::getposition(int& xpos,int& ypos)const{
//return the position of the window
  QPoint p=this->pos();
  xpos=p.x();
  ypos=p.y();

}
void Fitter_dialog::slot_setposition(int x, int y){
//changes position of window on screen
//only when it fits on the screen
 // if (((qApp->desktop())->width()>x)&&((qApp->desktop())->height()>y)){
    const QPoint p=QPoint(x,y);
    this->move(p);
 // }
  this->show();
}


void Fitter_dialog::slot_params(){
//set all parameters to current
 (fitterptr->modelptr)->setalluserparamstocurrent();
}

void Fitter_dialog::save_params(bool dat,bool crlb){
    //save all user stored parameters to a binary file
    //this can be read easily by DM with the import function
    //especially convenient for 2D SI's
    //for 1D the text output file might be more convenient

    //open a file for writing
      Getfilename myfile(0,"Select a file to save");
      //myfile.setdirname("~");
      if (dat){
        myfile.setfilter("Dat files (*.dat *.DAT)");
      }
      else{
        myfile.setfilter("txt files (*.txt *.TXT)");
      }
      myfile.setcaption("Select a filename to save");
      std::string filename;
      filename=myfile.save();
      if (filename=="") return; //canceled by user
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
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    (fitterptr->getmodelptr())->save_params(filename,dat,crlb,fitterptr);
    QApplication::restoreOverrideCursor(); // we're done
}


   




void Fitter_dialog::slot_params_save_dat(){
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    save_params(true,false);
    QApplication::restoreOverrideCursor(); // we're done

}

void Fitter_dialog::slot_model_save_dat(){
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));    
    //save the calculated model for all paprameter to a binary file
    //this can be read easily by DM with the import function
    //especially convenient for 2D SI's
    //for 1D the text output file might be more convenient
    
    //open a file for writing
    Getfilename myfile(0,"Select a file to save");
    //myfile.setdirname("~");        
    myfile.setfilter("Dat files (*.dat *.DAT)");        
    myfile.setcaption("Select a filename to save");
    std::string filename;
    filename=myfile.save();
    if (filename=="") return; //canceled by user
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
    (fitterptr->getmodelptr())->save_model(filename,fitterptr);               
    QApplication::restoreOverrideCursor(); // we're done
}

void Fitter_dialog::slot_params_save_txt(){
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    save_params(false,false);
    QApplication::restoreOverrideCursor(); // we're done
}
void Fitter_dialog::slot_params_save_crlb_dat(){
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    save_params(true,true);
    QApplication::restoreOverrideCursor(); // we're done
}

void Fitter_dialog::slot_set_redrawfit(bool r){
    redrawfit=r;
}
