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
 * eelsmodel - core/main.cpp
 **/

#include <ctime>
#include <iostream>

#include <QApplication>
#include <QWorkspace>
#include <QWidget>
#include <QFont>
#include <QString>
#include <QTextCodec>
#include <QTranslator>

//#include <gsl/gsl_linalg.h> //for matrixtest to compare with gsl
//#include <gsl/gsl_blas.h> //for matrixtest

#include "src/core/eelsmodel.h"

#include "src/gui/hello.h"
#include "src/gui/menu.h"

//TEST TEST TEST
//#include  <mpatrol.h>

//global vars keeping a pointer to eelsmodel and to the qt workspace
Eelsmodel* global_eelsmodelptr=0;
QWorkspace* global_workspaceptr=0;
MenuEelsmodel* global_menueelsmodel=0;
//QString globaldirname=""; //global variable holding the current directory

Eelsmodel* geteelsmodelptr(){
return global_eelsmodelptr;
}

QWorkspace* getworkspaceptr(){
return global_workspaceptr;
}

MenuEelsmodel* getmenuptr(){
return global_menueelsmodel;
}




/*
void spectrumtest(){
//test of spectrum class
   Spectrum* s1=0;
  try{s1 = new Spectrum(11,200,0.3);}
  catch(Spectrum::Spectrumerr::bad_alloc){std::cerr << "unable to allocate memory for spectrum\n";}
Spectrum* s2 = new Spectrum(10,500,0.3);
s1->setdatapoint(2,1.0,2.0,3.0);
 try {s2->setdatapoint(20,1.5,2.5,3.5);}
 catch(Spectrum::Spectrumerr::bad_index) { std::cerr <<"index out of bounds\n";}
 catch(...){std::cerr <<"Another error\n";}

 try{*s1+=*s2;}
 catch(Spectrum::Spectrumerr::not_same_size){std::cerr << "don't know how to add spectra with different size\n";}
double energy,counts,err;

s1->getdatapoint(1,&energy,&counts,&err);
 std::cout <<"s1.counts "<< counts<<" energy: "<<energy<< "\n";
delete s1;
delete s2;

//try the bad_alloc
 for(;;){
 try{s1 = new Spectrum(110000,200,0.3);}
  catch(Spectrum::Spectrumerr::bad_alloc){std::cerr << "unable to allocate memory for spectrum\n";}
}
}

void matrixtest(){
//test of CurveMatrix class
   CurveMatrix* m=new CurveMatrix(5);
   std::cout<<"after init\n";
   m->debugdisplay();
   //put random numbers in the matrix
   for (unsigned int i=0;i<m->dim1();i++){
    for (unsigned int j=0;j<m->dim2();j++){
      (*m)[i][j]=((double(rand())/RAND_MAX)*1000.0)-500.0;
      }
    }
   std::cout<<"after random numbers \n";
   m->debugdisplay();
   m->inv_inplace();
   std::cout<<"inverted \n";
   m->debugdisplay();
   m->inv_inplace();
   std::cout<<"inverted again\n";
   m->debugdisplay();
}
void chisquaretest(){
//test of the cumulative chi square distribution
  Spectrum* chiplot=new Spectrum(1024,0.0,1.0);
  //fill out chi square values * 100
  int nu=500;
  for (unsigned int i=0;i<chiplot->getnpoints();i++){
    double x=(double)i;
    double value= 10000.0*cumulative_chisquare(x,nu);
    chiplot->setcounts(i,value);
  }
  chiplot->setname("Cumulative chi square");
  chiplot->display();
}
void parametertest(){
//test the parameter class
Parameter p1=123.5;
 std::cout <<" the value is "<<p1.getvalue()<<"\n";
p1.setvalue(500.236);
 std::cout <<" the value is "<<p1.getvalue()<<"and changed is:"<<p1.changed()<<"\n";

p1.setchangeable(false);
p1.setvalue(400.06);
std::cout <<" the value is " << p1.getvalue()<<"\n";

 if (!p1.setboundaries(-100,500)){std::cout <<"the boundaries are not set\n";};
std::cout <<" the upperbound is "<<p1.getupperbound()<<"\n";
std::cout <<" the lowerbound is "<<p1.getlowerbound()<<"\n";
p1.setchangeable(true);
p1.setvalue(100.06);
std::cout <<" the value is "<<p1.getvalue()<<"\n";
if (!p1.setboundaries(-100,500)){std::cout <<"the boundaries are not set\n";};

std::cout <<" the upperbound is "<<p1.getupperbound()<<"\n";
std::cout <<" the lowerbound is "<<p1.getlowerbound()<<"\n";

p1.setvalue(700.06);
std::cout <<" the value is "<<p1.getvalue()<<"\n";


//second type of constructor
Parameter p2("size",12.5);
 std::cout << " the value of " << p2.getname() << " is " <<p2.getvalue()<< "\n";
 //third type of constructor
Parameter p3("parameter3", 5.55 ,false);
p3.setvalue(100.0);
 std::cout << " the value of " << p3.getname() << " is " << p3.getvalue() << "changeable: "<<p3.ischangeable()<<"\n";


//test destructor of parameter

  {
    //create a parameter
    Parameter p("size",0.0);
    Spectrum s(10,500.0,0.3);
  }
  //now p is out of scope so destructor should have been called
}
*/


int main(int argc, char *argv[])
{
//  debug_new::PrintTrace PrintingOn; // merely activates logging of new/delete

    //print arguments
    std::cout << "Argument list with "<<argc<<" arguments\n";
    for (int i=0;i<argc;i++)
    {
      std::cout << argv[i] <<"\n";
    }
    //first argument is the filename of this program containing the current working directory
    //second argument is *.mod filename which needs to be opened at startup


    //testmatrix();

    QApplication a( argc, argv );
    //a.QApplication::setGraphicsSystem("raster"); //important to fix a bug in QT with scaling of pen sizes

    //QTranslator tor( 0 );

    // set the location where your .qm files are in load() below as the last parameter instead of "."
    // for development, use "/" to use the english original as
    // .qm files are stored in the base project directory.

    //tor.load( QString("eelsmodel.") + QTextCodec::locale(), "." );

  //  a.installTranslator( &tor );
    /* uncomment the following line, if you want a Windows 95 look*/
    //a.setStyle(QApplication::WindowsStyle);

    //create menu, toolbar and init the workspace
    MenuEelsmodel* mymainwindow=new MenuEelsmodel();
    global_menueelsmodel=mymainwindow;
    //store the workspace
    global_workspaceptr=mymainwindow->getworkspaceptr();
    //init the eelsmodel class
    Eelsmodel* myeelsmodel=new Eelsmodel(global_workspaceptr,"version 3.0");
    //and store a global pointer
    global_eelsmodelptr=myeelsmodel;

    //show the window
    //a.setMainWidget( mymainwindow );
    mymainwindow->show();
    mymainwindow->showMaximized();

    //make connections between menu and eelsmodel program
    QObject::connect(mymainwindow, SIGNAL(file_new()),myeelsmodel, SLOT(newspectrum()) );
    QObject::connect(mymainwindow, SIGNAL(model_componentmaintenance()),myeelsmodel, SLOT(componentmaintenance()) );
    QObject::connect(mymainwindow, SIGNAL(model_newmodel()),myeelsmodel, SLOT(newmodel()) );
    QObject::connect(mymainwindow, SIGNAL(model_detector()),myeelsmodel, SLOT(slot_model_detector()) );

    QObject::connect(mymainwindow, SIGNAL(file_open_msa()),myeelsmodel, SLOT(openmsa()) );
    QObject::connect(mymainwindow, SIGNAL(file_open_DM()),myeelsmodel, SLOT(openDM()) );

    QObject::connect(mymainwindow, SIGNAL(model_iterativefit()),myeelsmodel, SLOT(iterativefit()) );
    QObject::connect(mymainwindow, SIGNAL(edit_undoselection()),myeelsmodel, SLOT(undoselection()) );
    QObject::connect(mymainwindow, SIGNAL(edit_exclude()),myeelsmodel, SLOT(exclude()) );
    QObject::connect(mymainwindow, SIGNAL(edit_resetexclude()),myeelsmodel, SLOT(resetexclude()) );

    QObject::connect(mymainwindow, SIGNAL(file_project_open()),myeelsmodel, SLOT(slot_open_project()) );
    QObject::connect(mymainwindow, SIGNAL(file_project_save()),myeelsmodel, SLOT(slot_save_project()) );
    QObject::connect(mymainwindow, SIGNAL(file_model_save()),myeelsmodel, SLOT(slot_save_model()) );
    QObject::connect(mymainwindow, SIGNAL(file_report_save()),myeelsmodel, SLOT((slot_save_report())) );
    QObject::connect(mymainwindow, SIGNAL(file_save_as()),myeelsmodel, SLOT(slot_save_as()) );
    QObject::connect(myeelsmodel, SIGNAL(enablemodel(bool)),mymainwindow, SLOT(slot_enable_model(bool)));




    Hello myhello;
    //=new Hello();
    //(void*) myhello;//supress unused warning
    if (argc>1){
      //get a *.mod file from the arguments
      std::string arg1=argv[0]; //contains program directory+program name
      std::string arg2=argv[1]; //contains argument ideally a *.mod filename to open
      global_eelsmodelptr->slot_open_project_from_filename(arg2);
    }






    const int result=a.exec();
    //delete the pointers we have
    //if (myhello!=0) delete(myhello); //the hello window is deleted by itself when pressing done

    //if (mymainwindow!=0) delete(mymainwindow);
    //if (myeelsmodel!=0) delete(myeelsmodel);



    return result;
}



