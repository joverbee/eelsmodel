/***************************************************************************
                          component.h  -  description
component is a spectrum which can create its own content by an internal function
component is used to derive several specific components which have their own way to
calculate like e.g. a lorentz peak
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

#ifndef COMPONENT_H
#define COMPONENT_H
#include "src/core/spectrum.h"
#include <valarray>
#include "src/core/parameter.h"
#include <vector>
#include "src/core/slice_iter.h"
//#define COMPONENT_DEBUG

/**
  *@author Jo Verbeeck
  */
class Equalizer;

class Component : public Spectrum {
//member data
private:
std::vector<Parameter*> parametervector; //a vector with the different parameters of the component
std::string name;         //name of the component is used in the user interface to label this component, should be a UNIQUE name
std::string typname;      //a name the user can give
std::string description;  //description of the component,can be used in the user interface help function
bool canconvolute;        //to make it possible that some components are not convoluted
std::vector<bool> hasgradient;         //has analytical gradient or not, per parameter

bool convolutor;          //if the component is a convolution which output is the whole result, not just a part of the model
bool shifter;             //if the component is a shifter which output is the whole result, not just a part of the model
bool multiplier;          //wether the component is a special multiplier component, the spectrum of this component is used to multiply with another component and is not used by itself in the model
bool hasmultiplier;       //whether the component is pointing to a multiplier that it uses in the final step of calculation
Component* multiplierptr; //pointer to the multiplier component that is used to calculate the final multiplication
bool visible; //defines whether a component can show graphs or not
//switch off during calculations to increase speed, switch on afterwards to
//shows subresults, like eg. the DOS graph in a DOS component
Equalizer* myequalizer; //a pointer to an equalizer
//exception classes
public:class Componenterr{
    public:class bad_index{unsigned int n1;};
    class bad_alloc{};
    class not_same_size{int n1,n2;};
    class unable_to_create{};
    class unable_to_calculate{};
    };

public:
Spectrum gradient;       //a spectrum containing the gradient wrt to some parameter

//constructors
Component();
Component(int,double,double,std::vector<Parameter*>* parameterlistptr=0);

//destructor
virtual ~Component();
virtual void setoptions(){}; //show a window to change the options, only needs implemetation if you want it

//public modifiers
virtual void calculate()=0;   //each component has to define how to do this: pure virtual class, you can not create a component
void setdisplayname(std::string s);
void setunchanged();          //set all parameters in the component to unchanged
void setchanged();
Parameter* getparameter(unsigned int index);
const Parameter* getparameter(unsigned int index)const;

std::vector<Parameter*>::iterator beginparameter(){return parametervector.begin();}
std::vector<Parameter*>::iterator endparameter(){return parametervector.end();}
virtual Component* clone()const=0;       //pure virtual cloning functions
virtual Component* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const=0;
//virtual void getgradient(size_t j,Slice_iter<double> array)=0; //get analytical partial derivative to parameter j and copy it in array via itterators
virtual Spectrum* getgradient(size_t j)=0; //get pointer to analytical partial derivative to parameter j
virtual void orderchanged(){};//to do when order of component changes, it is up to the component to reimplement this
void seteshift(double e); //overloaded from spectrum class
void setvisible(bool b);
virtual void show(); //show the contents of the component in a separate window if visible
virtual void updatemonitors(){}; //call some code if needed to calculate the contents of a monitor, usually called after fitting
virtual void estimateparams(){}; //determine value for parameters automatically, eg a fit to a power law, used for first estimates
//public inspectors
std::string getname()const;
std::string getdescription()const;
std::string getdisplayname()const;
size_t get_nr_of_parameters()const{return parametervector.size();};
bool get_has_gradient(size_t j)const{return hasgradient[j];} //return wether the component has an analytical gradient for parameter j
bool getcanconvolute()const{return canconvolute;}
bool getconvolutor()const{return convolutor;}
bool getshifter()const{return shifter;}
bool indexOK(unsigned int index)const;
bool get_ismultiplier()const{return multiplier;};
bool get_hasmultiplier()const{return hasmultiplier;};
Component* getmultiplierptr();
void setmultiplierptr(Component* comp);
void releasemultiplier();
bool isvisible(){ return visible;};
bool hasparameter(Parameter* p);
void showequalizer(); //shows an equalizer window to graphically control the parameter values

protected:
//protected accessors, only derived components can use these
void setconvolutor(bool b){convolutor=b;}
void setshifter(bool b){shifter=b;}
void setname(std::string s);
void setdescription(std::string s);
void addparameter(Parameter* p);
void pullparameter();
void setcanconvolute(bool b);
void sethasgradient(size_t j,bool b);
void set_ismultiplier(bool b);
};

#endif
