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
 * eelsmodel - gui/equalizer.cpp
 **/

#include "src/gui/equalizer.h"

#include <QSlider>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QWorkspace>
#include <QApplication>

#include "src/core/component.h"
#include "src/core/parameter.h"

#include "src/gui/eelsmodel.h"

EELSModel* geteelsmodelptr();

Equalizer::Equalizer(QWorkspace *parent, const char *name,Component* cp)
: QWidget(parent),sliderlist() {
     parent->addWindow(this);
      this-> setWindowTitle(name);
   //draw a set of sliders
   comp=cp;
   nparam=comp->get_nr_of_parameters();
   //setup sliders
   slidersetup();
   this->show();
   this->setMinimumSize(500,100); //needed since QT4 to make sure the widget is not shown minimized

}
void Equalizer::slidersetup(){
    sliderlist.clear();
    QHBoxLayout *hbox1 = new QHBoxLayout(this);
    hbox1->setMargin( 5 );
    for (size_t i=0;i<nparam;i++){
       //draw a slider for every parameter
       QSlider* myslider=new QSlider ( Qt::Vertical,this);
        hbox1->addWidget( myslider);
        sliderlist.push_back(myslider);
        maxval=100;
        myslider->setMaximum(maxval);
        myslider->setMinimum(-maxval);
        myslider->setTracking(true);
        //set current value
        Parameter* p=comp->getparameter(i);
        myslider->setValue(getscaledvalue(p));
        //set grey when locked
        myslider->setDisabled(!(p->ischangeable()));
        //connect(myslider,SIGNAL(sliderReleased()),this,SLOT(slot_sliderrelease()) );
        //connect( myslider, SIGNAL ( valueChanged (int)), this, SLOT(slot_sliderchange()) );
   }
}
int Equalizer::getscaledvalue(Parameter* p){
    //get the parameter and scale it to int taking into account the boundaries
    const double val=p->getvalue();
double max=0.0;
double min=0.0;

     if (p->isbound()){
            //set min to lowest boundary
            max=p->getupperbound();
            min=p->getlowerbound();
        }
        else{
            max=val*2.0;
            min=val*0.5;
        }
        double scaledval=0.0;
        if (max!=min){
            scaledval=-(double)maxval+(double)(2*maxval)*((val-min)/(max-min));
        }

        int intval= (int) scaledval;
        if (abs(intval)>maxval){
            intval=0;
            }
        return intval;
    }


void Equalizer::setscaledvalue(Parameter* p,int value){
    //set the parameter and scale it to int taking into account the boundaries

    //carefull! this causes rounding errors...do this only on bound parameters
    //and only on the ones which where really changed by the user!
    const double val=p->getvalue();
double max=0.0;
double min=0.0;

     if (p->isbound()){
            //set min to lowest boundary
            max=p->getupperbound();
            min=p->getlowerbound();
        }
        else{
            max=val*2.0;
            min=val*0.5;
        }
        const double ratio=(double)(value+maxval)/((double)2*maxval);
        const double scaledval=min+(max-min)*ratio;

        p->setvalue(scaledval);
}



void Equalizer::paintEvent( QPaintEvent * e){

 for (size_t i=0;i<nparam;i++){
        QSlider* myslider=sliderlist[i];
        //set current value
       Parameter* p=comp->getparameter(i);

       //switch off connection so to not trigger a parameter update
       disconnect(myslider,SIGNAL(sliderReleased()),this,SLOT(slot_sliderrelease()) );
       disconnect( myslider, SIGNAL ( valueChanged (int)), this, SLOT(slot_sliderchange()) );

       myslider->setValue(getscaledvalue(p));
       //set grey when locked
       myslider->setDisabled(!(p->ischangeable()));

       //switch connection back on
       connect(myslider,SIGNAL(sliderReleased()),this,SLOT(slot_sliderrelease()) );
       connect( myslider, SIGNAL ( valueChanged (int)), this, SLOT(slot_sliderchange()) );


    }
    this->QWidget::paintEvent(e);
}


Equalizer::~Equalizer(){
}

void Equalizer::slot_sliderrelease() {
    //force a recalc and redisplay of the model
    geteelsmodelptr()->componentmaintenance_updatescreen();
}

void Equalizer::slot_sliderchange() {
    //if slider changes value, update the parameter
    QSlider* myslider = (QSlider*)sender(); //do this on the first line of a slot!, otherwise undefined behaviour
    //find this QSlider in the list
    size_t i=0;
    for (i=0;i<nparam;i++){
        if (myslider==sliderlist[i]) break;
    }
    if (i<nparam) setscaledvalue(comp->getparameter(i),myslider->value()); //ONLY if really changed by the user
}

size_t Equalizer::activesliderindex(){
    size_t i=0;
    for (i=0;i<nparam;i++){
        //if ((sliderlist[i])->hasMouse()) break;
        }
    return i;
}
