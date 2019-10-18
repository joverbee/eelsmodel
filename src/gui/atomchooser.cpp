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
 * eelsmodel - gui/atomchooser.cpp
 **/

#include "src/gui/atomchooser.h"

#include <iostream>
#include <sstream>

#include <QComboBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QMdiArea>
#include <math.h>
#include "src/gui/mendeleev.h" //defines table of mendeleev
#include "src/gui/energies.h"

Atomchooser::Atomchooser(QMdiArea *parent, const char *name,double Estart,double Estop, double E0, double beta, double alpha, double resolution,bool dofinestructure ) : QDialog(parent) {

 parent->addSubWindow(this); //add it explicitly to the workspace

this->setWindowTitle("Atom chooser");

//draw a table of Mendeleev
QGridLayout* lay = new QGridLayout(this);


size_t id=0;
buttonlist.clear();
for (size_t i=0;i<9;i++){
    for (size_t j=0;j<18;j++){

        //first line
        if ((i==0)&(j==1)){
            j=17;
        }
        //2nd
        if ((i==1)&(j==2)){
            j=12;
        }
        //3rd
        if ((i==2)&(j==2)){
            j=12;
        }
         //7th
        if ((i==6)&(j==7)){
            j=4;
            i=7;
        }
         if ((i==8)&(j==0)){
            j=4;
        }
        QPushButton *thisbutton = new QPushButton(mendeleev[id].c_str());
        buttonlist.push_back(thisbutton);
        thisbutton->setCheckable (true);
        thisbutton->setFixedSize(30,30);

        lay->addWidget(thisbutton,i,j);


        thisbutton->setEnabled(false);
        //check if we have a tabulated energy for each
        if (id<size_K){
                if ((energy_K[id]>Estart)&(energy_K[id]<Estop)){
                     thisbutton->setEnabled(true);
                }
        }
        if (id<size_L23){
                if ((energy_L23[id]>Estart)&(energy_L23[id]<Estop)){
                     thisbutton->setEnabled(true);
                }
        }
        id+=1;

        //connect to do something if clicked

    }
}

//convergence angle
valstring="%1"; //required by MacOSX otherwise empty text window
int fieldwidth=-1;
    char format='f';
    int precision=2;
    double_validator=new QDoubleValidator(this);
    alphaedit=new QLineEdit(valstring.arg (alpha/1.0e-3,fieldwidth,format,precision ));
    QLabel* alphalabel=new QLabel("alpha [mrad]");
    alphaedit->setValidator (double_validator);  //make sure only doubles are entered

    lay->addWidget(alphalabel,9,0,1,3);
    lay->addWidget(alphaedit,10,0,1,3);

//collection angle
    betaedit=new QLineEdit(valstring.arg (beta/1.0e-3,fieldwidth,format,precision ));
    QLabel* betalabel=new QLabel("beta [mrad]");
    betaedit->setValidator (double_validator);  //make sure only doubles are entered

    lay->addWidget(betalabel,9,3,1,3);
    lay->addWidget(betaedit,10,3,1,3);

//HT

    HTedit=new QLineEdit(valstring.arg (E0/1.0e3,fieldwidth,format,precision ));
    QLabel* HTlabel=new QLabel("E0 [kV]");
    HTedit->setValidator (double_validator);  //make sure only doubles are entered
    lay->addWidget(HTlabel,9,6,1,3);
    lay->addWidget(HTedit,10,6,1,3);

//energy resolution
 Resedit=new QLineEdit(valstring.arg (resolution,fieldwidth,format,precision ));
    QLabel* Reslabel=new QLabel("resolution [eV]");
    Resedit->setValidator (double_validator);  //make sure only doubles are entered
    lay->addWidget(Reslabel,9,9,1,3);
    lay->addWidget(Resedit,10,9,1,3);


//a radio button to swith fine structure
finestructbutton =new QRadioButton ("Add fine structure",this);
lay->addWidget(finestructbutton,10,12,1,3);
finestructbutton->setChecked(dofinestructure);



//OK and cancel buttons
//OK button

     QPushButton *OK = new QPushButton( "&Make cross sections");
     QPushButton *Cancel = new QPushButton( "&Cancel");
     lay->addWidget(OK,11,0,1,9);
     lay->addWidget(Cancel,11,9,1,9);
     connect( OK, SIGNAL( clicked()), this, SLOT( slot_OK() ) );
     connect( Cancel, SIGNAL( clicked()), this, SLOT( slot_Cancel() ) );

this->show();
 this->setMinimumSize(this->minimumSizeHint()); //needed since QT4 to make sure the widget is not shown minimized
}
Atomchooser::~Atomchooser(){
}


void Atomchooser::slot_OK(){
    //gather results when OK
    emit accept();
}
void Atomchooser::slot_Cancel(){
    //gather results when OK
    emit reject();
}

double Atomchooser::getalpha(){
    //get alpha in rad
    return getnumber(alphaedit->text())*1.0e-3;
}
double Atomchooser::getbeta(){
    //get Beta in rad
    return getnumber(betaedit->text())*1.0e-3;
}
double Atomchooser::getE0(){
    //get HT in eV
    return getnumber(HTedit->text())*1.0e3;
}
double Atomchooser::getresolution(){
    //get HT in eV
    return getnumber(Resedit->text());
}
bool Atomchooser::getdofine(){
    //do fine structure?
    return finestructbutton->isChecked();;
}
double Atomchooser::getnumber(const QString& valstring)const{
  bool ok;
  double result=valstring.toDouble(&ok);
  if (ok) return result;
  else return 0.0;
}

void Atomchooser::setweight(size_t Z, double weight,bool overlap){
    //set the weight of an element in the Mendeleev table
    //the user provides the weight calculate from a peak finding which indicates which atoms are likely to be present in the spectrum
    //the weight needs to be between 0.0 and 1.0 and determines the color of the button in the Mendeleev table

    //overlap indicates there is a high risk of overlap errors
    //this is indicated by adding red color
    //set color of button for atom Z

    if ((Z-1<buttonlist.size())&&(Z!=0)){
        const int g=200+int(pow(weight,0.5)*55.0);//make it nonlinear
        int r=200;
        const int b=200;
        if (overlap) r=255;
        QColor col = QColor(r,g,b);
        if(col.isValid()) {
            QString qss = QString("background-color: %1").arg(col.name());
            buttonlist[Z-1]->setStyleSheet(qss);
        }
    }
}

void Atomchooser::getZ(std::vector<size_t>* Zlist){
    //return vector with atom numbers
    Zlist->clear();
    //loop over the buttons and return the ones that are checked
    for (size_t i=0;i<buttonlist.size();i++){
        if (buttonlist[i]->isChecked()){
                Zlist->push_back(i+1); //the index starts from zero while atom numbers start from 1
            }
        }
}

void Atomchooser::getedges(std::vector<double>* Elist,int type){
    //return edge onsets for edges of type (0=K,1=L) of selected atoms, if we don't know the edge onset or if it is outside the range, we have 0
    Elist->clear();
    //loop over the buttons and return the ones that are checked
    for (size_t i=0;i<buttonlist.size();i++){
        if (buttonlist[i]->isChecked()){
            double energy=0.0;
            switch(type){
                case 0:
                    if (i<size_K){
                        energy=energy_K[i];
                    }
                    break;
                case 1:
                    if (i<size_L23){
                        energy=energy_L23[i];
                    }
            }
            Elist->push_back(energy);
        }

    }
}

void Atomchooser::getnamed(std::vector<std::string>* Namelist,int type){
    //return a list with names of the selected atoms
    Namelist->clear();
    //loop over the buttons and return the ones that are checked
    for (size_t i=0;i<buttonlist.size();i++){
        if (buttonlist[i]->isChecked()){
            std::string name;
            std::ostringstream s;
            switch(type){
                case 0:
                    if ((s << mendeleev[i]<<" K")){
                        // conversion worked
                        name=s.str();
                    }
                    break;
                case 1:
                    if ((s << mendeleev[i]<<" L23")){
                        // conversion worked
                        name=s.str();
                    }

            }
            Namelist->push_back(name);
        }
    }
}








