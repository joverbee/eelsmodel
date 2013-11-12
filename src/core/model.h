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
 * eelsmodel - core/model.h
 *
 * Model class is a spectrum that is able to calculate itself
 * it creates its own spectrum data by using a number of Components
 * (like lorentz peaks, gaussian peaks, hydrogenic K-edges etc.).
 * You should start with a pointer to the core loss spectrum as argument.
 **/

#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "src/core/spectrum.h"

class Component;
class Fitter;
class Graph;
class Imagedisplay;
class Multispectrum;
class Parameter;

class QWorkspace;

class Model : public Spectrum{

//member data
Graph* graphptr;
std::vector<Component*> componentptrvector;
std::vector<Component*> ptr_to_all_components;
Spectrum* HLptr; //pointer to HL spectrum
Spectrum* dummyspec;//pointer to a temporary spectrum for calculation

std::vector<Parameter*> paramvector;
std::vector<Spectrum*> plotspecvector; //vector with spectrum for each parameter that is displayed. that contains the plot of the parameter value for each spectrum in the SI
std::vector<Imagedisplay*> imdisplayvector;
std::vector<Parameter*> freeparamvector; //vector with pointers to the free parameters of the model
std::vector<Parameter*> freelinparamvector; //vector with pointers to the free linear parameters of the model
std::vector<Parameter*> freenonlinparamvector; //vector with pointers to the free linear parameters of the model

std::vector<Component*> freeparamcomponentvector; //vector with pointers to the components of the free parameters of the model
std::vector<int> freeparamindex; //vector the index of the parameter in the component
std::vector<int> freecomponentindex; //vector the index of the component of a certain parameter in the model
std::vector<int> freenonlinid;
std::vector<int> freelinid;
std::vector<double> originalvalues;//vector with original values of all parameters
std::vector<double> originalvalues_sigma;//vector with original values of all parameters
std::vector<double> uservalues;//vector with user stored values of all parameters
std::vector<double> uservalues_sigma;//vector with user stored values of all parameters
std::vector<std::string> uservalues_resultstring;
std::vector<std::string> uservalues_monitorstring;


std::vector<bool> stored;
std::vector<bool> userstored;
size_t freeparameters; //number of free parameters in the model
size_t freelinparameters; //number of free linear parameters in the model
size_t freenonlinparameters; //number of free linear parameters in the model

size_t nrofspectra; //the number of spectra in the multispectrum or 1 for a normal spectrum
//vectors holding info for the free parameters of the model



Multispectrum* Multiptr;
bool locked;//if the model is locked, no components can be added or removed
bool changed;//needed to see if the model was changed
bool multi;//indicates wether the model is for a multispectrum or a normal spectrum
double detectorgain;
std::string detectorname;
double detectorcorrfactor;

//member functions
public:
Model(Spectrum* HL);
Model(Multispectrum* HL);
~Model();
void maintaincomponents();
Component* getcomponent(int index);
const Component* getcomponent(int index)const;
Component* getlastcomponent();
void componentshow();
size_t getcomponentsnr()const{return componentptrvector.size();}
void calculate();
void display(QWorkspace* parent=0); //override of Spectrum::display
void seteshift(double e); //override of Spectrum::eshift
void printcomponents();
void resetsigmas();
std::vector<Component*>& getcomponentvector();
std::vector<Component*>& getallcomponentvector();
void addcomponent(int index,std::vector<Parameter*>* parameterlistptr=0);
void removecomponent(int index);
const Spectrum* getconstHLptr()const{return HLptr;}
Spectrum* getHLptr()const{return HLptr;}
Spectrum getspectrum(){return *this;}
Spectrum* getspectrumptr(){return this;}
Multispectrum* getmultispectrumptr(){return Multiptr;}
const Multispectrum* getconstmultispectrumptr()const{return Multiptr;}
const Spectrum* getconstspectrumptr()const{return this;}
void setlocked(bool b){locked=b;}
bool is_locked()const{return locked;}
bool has_changed(){return changed;}
void setchanged(bool b){changed=b;if (b) {init();};} //force an init after changes
int getcomponentindexbyname(std::string name)const;
int getcomponentindexbypointer(const Component* ptr)const;
Model& operator=(const Spectrum&);//copy only the y-data
Model& operator+=(const Spectrum&);//add only the y-data
Model& operator*=(const Spectrum&);//add only the y-data

void setdetectorname(std::string name);
void setdetectorgain(double gain);
void setdetectorcorrfactor(double cfactor);
double getdetectorgain()const;
double getdetectorcorrfactor()const;
std::string getdetectorname()const;
bool ismulti()const;
void updateHL();
void updateHLnodraw();
//void getgradient(size_t compindex, size_t parameterindex,Slice_iter<double> array);
//functions for storing and recalling old parameter settings
//there are 2 storage spaces, one called origparams to be able to come back after a diverged fit
//the other called userparams to store the current user parameters
//the storage has the size freeparametersx number of spectra in the multispectrum
void setallparamstocurrent(); //reset the parameter storage to the current parameters in the model
void retrieveparams();//retrieve a free paramter set
void storeparams();//store a free parameter set
//the user set
void setalluserparamstocurrent();
void storeuserparams(std::string result); //store the current set of parameters in a special user memory:DOn`t use these internally,use storeparams instead
void retrieveuserparams(); //retrieve that set
size_t getcurrspecnr(); //returns value of the currently selected spectrum 0=first or only spectrum
void init(); //initialise vectors with free params and the storage of the params
bool checkindex(size_t index)const; //check if index is inside valid range of free parameters
bool checklinindex(size_t index)const; //check if index is inside valid range of free linear parameters
bool checknonlinindex(size_t index)const; //check if index is inside valid range of free nonlinear parameters

size_t getcomponentfreeparamindex(size_t index);
size_t getfreeparamindex(size_t index);
Parameter* getfreeparam(size_t index);
Parameter* getfreelinparam(size_t index);
Parameter* getfreenonlinparam(size_t index);

Component* getcomponentforfreeparam(size_t index);
size_t getnroffreeparameters()const;
size_t getnroffreelinparameters()const;
size_t getnroffreenonlinparameters()const;

Spectrum* getgradientptr(size_t compindex, size_t parameterindex);
void setcurrentspectrum(size_t index,bool redraw=true);
size_t getnrofspectra();
void save_params_as_dat(std::string filename);//save user params as binary file
size_t getnrofallparams()const;
void resetstorage();
void save_params(std::string filename,bool dat,bool crlb,Fitter* fitter);
void save_model(std::string filename,Fitter* fitter);


void save_report(std::ofstream &projectfile,Fitter* myfitter,size_t specnr);
void save_fitter_details(std::ofstream &projectfile,Fitter* myfitter);
double getusersigma(size_t specnr, size_t paramnr)const;
double getuservalue(size_t specnr, size_t paramnr)const;
void setusersigma(size_t specnr, size_t paramnr,double value);
void setuservalue(size_t specnr, size_t paramnr,double value);
int getparamindexbypointer(const Parameter* ptr)const;
int getfreeparamindexbypointer(const Parameter* ptr)const;
size_t getfreeindexfromnonlinindex(size_t j)const;
size_t getfreeindexfromlinindex(size_t j)const;
void storemonitorstring(size_t specnr,std::string="");
std::string getmonitorstring(size_t specnr);
void storeresultstring(std::string result,size_t specnr);
std::string getresultstring(size_t specnr);
void printparameters()const;
bool hasstored(size_t specnr)const;
const Graph* getgraphptr()const{return graphptr;}
Graph* getgraphptr(){return graphptr;}
void updatemonitors();
double getfirstnonexludeenergy()const;
};

#endif
