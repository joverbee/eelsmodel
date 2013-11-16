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
 * eelsmodel - core/model.cpp
 **/

#include "src/core/model.h"

//#define MODEL_DEBUG
#define BUFSIZE 1024 //buffer size for faster writing files

#include <iostream>
#include <vector>

#include <QApplication>
#include <QDateTime>

#include "src/components/mycomponents.h"

#include "src/core/eelsmodel.h"
#include "src/core/monitor.h"
#include "src/core/multispectrum.h"
#include "src/core/slice_iter.h"

#include "src/fitters/fitter.h"

#include "src/gui/componentmaintenance.h"
#include "src/gui/graph.h"
#include "src/gui/imagedisplay.h"
#include "src/gui/saysomething.h"




class Parameter;
class QWorkspace;
extern QWorkspace* getworkspaceptr();
extern Eelsmodel* geteelsmodelptr();//main.cpp contains this global function with a pointer to eelsmodel

Model::Model(Spectrum* HL)
:Spectrum(HL->getnpoints(),
HL->getrawenergy(0),
HL->getdispersion()),
componentptrvector(),
ptr_to_all_components(),
paramvector(),
plotspecvector(),
imdisplayvector(),
freeparamvector(),
freeparamcomponentvector(),
freeparamindex(),
freecomponentindex(),
originalvalues(),
originalvalues_sigma(),
uservalues(),
uservalues_sigma(),
stored(),
userstored()
{
 detectorgain=1.0;
 detectorname="ideal detector";
 detectorcorrfactor=1.0;
  graphptr=0;
  this->seteshift(HL->geteshift());
  //creating a new model
  //get a pointer to the HL spectrum, this we will use for getting the same energy scale for our model
  HLptr=HL;
  multi=false;
  Multiptr=0;
  dummyspec =new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  setlocked(false);
  setchanged(true);
  //make a list of all possible components that are defined in mycomponents.h
  //in mycomponents.h all components are added to the back of the vector
  #define MY_COMP_EXEC  //also includes the creating of a small version of each component and puts it in the vector
  #include "src/components/mycomponents.h"
  this->init();
  this->resetstorage(); //init storage space for all parameters, only needs to be reset when nr of total params changes
}

Model::Model(Multispectrum* HL)
:Spectrum((HL->getcurrentspectrum())->getnpoints(),
(HL->getcurrentspectrum())->getrawenergy(0),
(HL->getcurrentspectrum())->getdispersion()),
componentptrvector(),
ptr_to_all_components(),
paramvector(),
freeparamvector(),
freeparamcomponentvector(),
freeparamindex(),
freecomponentindex(),
originalvalues(),
originalvalues_sigma(),
uservalues(),
uservalues_sigma(),
stored(),
userstored()
{
    detectorgain=1.0;
    detectorname="ideal detector";
    detectorcorrfactor=1.0;
  graphptr=0;
  this->seteshift(HL->geteshift());
  //creating a new model
  //get a pointer to the HL spectrum, this we will use for getting the same energy scale for our model
  HLptr=HL->getcurrentspectrum();
  Multiptr=HL;
  dummyspec =new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  multi=true;
  setlocked(false);
  setchanged(true);
  //make a list of all possible components that are defined in mycomponents.h
  //in mycomponents.h all components are added to the back of the vector
  #define MY_COMP_EXEC  //also includes the creating of a small version of each component and puts it in the vector
  #include "src/components/mycomponents.h"
  this->init(); //init storage for free parameters
  this->resetstorage(); //init storage space for all parameters, only needs to be reset when nr of total params changes
  }




Model::~Model(){
  //clean up all components
  for (unsigned int i=0;i<componentptrvector.size();i++){
    delete(componentptrvector[i]);
    }

if (graphptr!=0)  delete graphptr; //delete the graph if the model dies
}

void Model::maintaincomponents(){
  //show a window with all possible and all current components
  //the user can add and remove components
  Componentmaintenance*  myview=new  Componentmaintenance(0,"component maintenance",componentptrvector,ptr_to_all_components);
  myview->show();
}


Component* Model::getcomponent(int index){
  if ((index>=0)&&(index<(int)componentptrvector.size())){
    return componentptrvector[index];
  }
  else{
    return NULL;
  }
}

const Component* Model::getcomponent(int index)const{
  if ((index>=0)&&(index<(int)componentptrvector.size())){
    return componentptrvector[index];
  }
  else{
    return NULL;
  }
}

Component* Model::getlastcomponent(){
  return componentptrvector.back();
}

int Model::getcomponentindexbyname(std::string name)const{
  for (unsigned int i=0;i<ptr_to_all_components.size();i++) {
    if ((ptr_to_all_components[i])->getname()==name){
      std::cout << "Component found with name: "<<name<<" and index "<<i<<"\n";
      return (int)i;
    }
  }
  std::cout << "Component not found with name: "<<name<<"\n";
  return -1;
}

int Model::getcomponentindexbypointer(const Component* ptr)const{
  //return index in the componentptrvector from a component with pointer ptr
  //-1 is returned is pointer is not found in the model
  for (unsigned int i=0;i<componentptrvector.size();i++) {
    if ((componentptrvector[i])==ptr){
      //found

       return (int)i;
    }
  }
  //not found
  return -1;
}

void Model::calculate(){
  //erase current content
  this->clear();
   //apply shift components if present they come first since they change the energy scale for all the rest
   for (size_t i=0;i<componentptrvector.size();i++){
     if (componentptrvector[i]->getshifter()){
       componentptrvector[i]->calculate();
       #ifdef MODEL_DEBUG
	 std::cout << "Calculating a shifter component: "<<componentptrvector[i]->getname()<<"\n";
       #endif
       }
     }

 //apply multiplier components if present
 for (size_t i=0;i<componentptrvector.size();i++){
   if (componentptrvector[i]->get_ismultiplier()){
     componentptrvector[i]->calculate();
     #ifdef MODEL_DEBUG
       std::cout << "Calculating a multiplier component: "<<componentptrvector[i]->getname()<<"\n";
     #endif
     }
   }


   //calculate each component and add up

   //first all components which need to be convoluted
   for (size_t i=0;i<componentptrvector.size();i++){
     if ((componentptrvector[i]->getcanconvolute())&&(!(componentptrvector[i]->get_ismultiplier()))&&(!(componentptrvector[i]->getshifter()))){
       //only those components that can get convoluted and are not shifters or multipliers
       componentptrvector[i]->calculate();
       if (componentptrvector[i]->get_hasmultiplier()){//if it has a multiplier
	 const Component* multiptr=componentptrvector[i]->getmultiplierptr();
         #ifdef MODEL_DEBUG
	     std::cout << "multiptr:"<<multiptr<<"\n";
	     std::cout << "getmutliplierptr:"<<componentptrvector[i]->getmultiplierptr()<<"\n";
         #endif

	 if (multiptr!=0){
	   #ifdef MODEL_DEBUG
	     std::cout << "Calculating a multiplied component: "<<componentptrvector[i]->getname()<<"\n";
	   #endif
	   *dummyspec=(*componentptrvector[i]);
	   (*dummyspec)*=(*multiptr);//multiply the result with the multiplier component
	   (*this)+=(*dummyspec);
           #ifdef MODEL_DEBUG
	     //dummyspec->display(0);
	     std::cout << "&(*multiptr):"<<&(*multiptr)<<"\n";
	     std::cout << "multiptr:"<<multiptr<<"\n";
           #endif
	   }
         }
       else{//if it doesnt'have a multiplier
         #ifdef MODEL_DEBUG
	   std::cout << "Calculating a non-multiplied component: "<<componentptrvector[i]->getname()<<"\n";
	 #endif
	 *this+=*componentptrvector[i]; //this might be slow
         }
       }
     }


   //then the convolution if needed
   for (size_t i=0;i<componentptrvector.size();i++){
     if (componentptrvector[i]->getconvolutor()){
       componentptrvector[i]->calculate();
  #ifdef MODEL_DEBUG
	   std::cout << "Calculating a convolutor component: "<<componentptrvector[i]->getname()<<"\n";
	 #endif
       *this=*componentptrvector[i];
       }
     }
   //then add components which can not be convoluted and are not convolutors and are not shifters and are not multipliers
   for (size_t i=0;i<componentptrvector.size();i++){
     if ((!(componentptrvector[i]->getcanconvolute()))&&(!(componentptrvector[i]->get_ismultiplier()))&&(!(componentptrvector[i]->getconvolutor()))&&(!(componentptrvector[i]->getshifter()))){
  #ifdef MODEL_DEBUG
	   std::cout << "Calculating a background component: "<<componentptrvector[i]->getname()<<"\n";
	 #endif
       componentptrvector[i]->calculate();
       *this+=*componentptrvector[i];
       }
     }
   }

void Model::display(QWorkspace* parent){
    if (graphptr==0) {
      graphptr =new Graph(parent,"model",this); //draw the model
      graphptr->setcaption("model");
      graphptr->addgraph(HLptr); //show the HL in the background
      graphptr->show();
    }
    else{
      this->updateHL(); //update layer 0 of the graph (model part)
    }
    //tell components to show their content if they have something to show
    componentshow();
}

void Model::componentshow(){
  //show contents of all component in a separate graph if they are visible
  for (unsigned int i=0;i<componentptrvector.size();i++) {
    Component* mycomponent=componentptrvector[i];
    mycomponent->show();
    }

}

bool Model::ismulti()const{
//return wether the spectrum is a multispectrum
return multi;
}

bool Model::islinear()const{
    //return if the whole model is linear in its parameters
    return getnroffreenonlinparameters()==0;
}

void Model::updateHL(){
if (ismulti()){
  HLptr=Multiptr->getcurrentspectrum();
  
}

//and display the model
if (graphptr!=0) {
    graphptr->updategraph(1,HLptr); //update layer 1 of the graph (spectrum part)
    graphptr->updategraph(0,this); //update layer 0 of the graph (model part)
    
  
}
//and also update the graph for all the parameters that need displaying (only makes sense for 1D or 2D
if (ismulti()){
  for (unsigned int i=0;i<paramvector.size();i++) {
     Parameter* myparameter=paramvector[i];                                                                                                                        
       if (myparameter->isdisplayed()){
    	   //find out if 1D,2D SI    	       	   
    	   const double val=myparameter->getvalue();
    	   if (Multiptr->is2D()){
    		   //2D, make image
    		   Imagedisplay* imdisplayptr=imdisplayvector[i];
    		   if (imdisplayptr==0) {
    			   size_t imheight=Multiptr->getstride();
    			   size_t imwidth=Multiptr->getsize()/Multiptr->getstride();
    			   imdisplayptr=new Imagedisplay(getworkspaceptr(),myparameter->getname(),imheight,imwidth);       			   
    			   imdisplayvector[i]=imdisplayptr; //store for later use
    		    }
    		   	CurveMatrix* matrix=imdisplayptr->getmatrix();
    		   	const size_t id1=this->getcurrspecnr()%Multiptr->getstride();
    		    const size_t id2=this->getcurrspecnr()/Multiptr->getstride(); //modulus
    		   	(*matrix)(id1,id2)=val;    		   	
    		    imdisplayptr->updatereloadmatrix();
    	   }
    	   else{
    		   //1D, make line plot
    		   Spectrum* plotspec=plotspecvector[i];
    		   if (plotspec==0) {
    			   plotspec=new Spectrum(this->getnrofspectra());
    			   plotspec->clear();//set all to zero
    			   plotspec->setname(myparameter->getname());
    			   plotspec->setxunits("nr.");
    			   plotspec->setyunits("value");
    			   plotspecvector[i]=plotspec; //store for later use
    		   }
    		   plotspec->setdatapoint (this->getcurrspecnr(),double(this->getcurrspecnr()),val,0.0);
    		   plotspec->display(getworkspaceptr());    		  
    	   }    	       	       	      	      			      		    	          
     }
     else{
    	 //if not displayed just delete the display?
    	 if (Multiptr->is2D()){
    		 if (imdisplayvector[i]!=0) delete(imdisplayvector[i]);
    		 imdisplayvector[i]=0;
    	 }else{
    		 if (plotspecvector[i]!=0) delete(plotspecvector[i]);
    		 plotspecvector[i]=0;
    	 }
     }
  }
}
}

void Model::updateHLnodraw(){
if (ismulti()){
  HLptr=Multiptr->getcurrentspectrum();
  //retrieveuserparams();
}
else{
    //calculate();
}
}

void Model::printcomponents(){
  for (unsigned int i=0;i<ptr_to_all_components.size();i++) {
    std::cout << "name: " << (ptr_to_all_components[i])->getname() << " description:" <<  (ptr_to_all_components[i])->getdescription() <<"\n";
    }
}

void Model::resetsigmas(){
  for (unsigned int i=0;i<componentptrvector.size();i++) {
    Component* mycomponent=componentptrvector[i];
    for (size_t j=0;j<mycomponent->get_nr_of_parameters();j++) {
      Parameter* myparameter=mycomponent->getparameter(j);
      myparameter->setsigma(0.0);
      }
    }
}

std::vector<Component*>& Model::getcomponentvector(){return componentptrvector;}

std::vector<Component*>& Model::getallcomponentvector(){return ptr_to_all_components;}

void Model::addcomponent(int index,std::vector<Parameter*>* parameterlistptr){
  if (is_locked())  return; //don't do it when the model is locked
  Component *mycomponent;
  try{
    //make a new real version of the dummy component
    mycomponent=(ptr_to_all_components[index])->new_component(this->getnpoints(),this->getrawenergy(0),this->getdispersion(),parameterlistptr);
  }
  catch(...){
  //something went wrong during creation
  return;
  }
  componentptrvector.push_back(mycomponent); //add to back of component vector
  mycomponent->seteshift(this->geteshift()); //give every new component the same eshift as we have
  this->calculate();
  this->display();
  //init(); //update set of free parameters
  setchanged(true); //notify that something has changed, this also forces an init
  this->resetstorage(); //init storage space for all parameters, only needs to be reset when nr of total params changes
};

void Model::removecomponent(int index){
  if (is_locked())  return; //don't do it when the model is locked
  //delete(componentptrvector[index]); //delete the component
  //awkward way to use erase....
  std::vector<Component*>::iterator where = std::find( componentptrvector.begin(), componentptrvector.end(), componentptrvector[index] );
  if (where!=componentptrvector.end()){
    delete(*where);//delete the component
    componentptrvector.erase(where); //remove the space in the vector
    //carefull, also update everything that relies on the nr in the component list
    //like the DOS
    for (unsigned int i=0;i<componentptrvector.size();i++) {
        Component* mycomponent=componentptrvector[i];
        mycomponent->orderchanged();
    }
  }
  //init(); //update set of free parameters
  setchanged(true);
  this->resetstorage(); //init storage space for all parameters, only needs to be reset when nr of total params changes
}
Model& Model::operator=(const Spectrum& a1){
//copy only the y-data
 if (a1.getnpoints()!=this->getnpoints()) {throw Spectrumerr::not_same_size();}
 for (unsigned int bin=0;bin<this->getnpoints();bin++)
    {
    this->setcounts(bin,a1.getcounts(bin));
  }
return *this;
}
Model& Model::operator+=(const Spectrum& a1){
//add only the y-data
 if (a1.getnpoints()!=this->getnpoints()) {throw Spectrumerr::not_same_size();}
 for (unsigned int bin=0;bin<this->getnpoints();bin++)
    {
    double cts=this->getcounts(bin)+a1.getcounts(bin);
    this->setcounts(bin,cts);
  }
return *this;
}
Model& Model::operator*=(const Spectrum& a1){
//multiply only the y-data
 if (a1.getnpoints()!=this->getnpoints()) {throw Spectrumerr::not_same_size();}
 for (unsigned int bin=0;bin<this->getnpoints();bin++)
    {
    double cts=this->getcounts(bin)*a1.getcounts(bin);
    this->setcounts(bin,cts);
  }
return *this;
}

void Model::seteshift(double e){
	//override of Spectrum::seteshift
	//set eshift for the model
	this->Spectrum::seteshift(e);
	//do the same for all components, they also get a setchanged command so they will recalculate completely when calculate is called
	for (unsigned int i=0;i<componentptrvector.size();i++) {
		Component* mycomponent=componentptrvector[i];
		mycomponent->seteshift(e);
    }
    //every new component that gets added needs also to get the same eshift
    //this is done in addcomponent
}
Spectrum* Model::getgradientptr(size_t compindex, size_t parameterindex){
  Component* mycomponent=componentptrvector[compindex];
  //ask for an analytical gradient if available
  if (mycomponent->get_has_gradient(parameterindex)){
    return mycomponent->getgradient(parameterindex);
  }
  else{
    //no gradient
    return 0;
  }
}

void Model::getgradient(size_t compindex, size_t parameterindex,Slice_iter<double> array){
  //get the analytical gradient of a component
  //but apply convolution and eshift if needed
  //store it in the array that fitter supplies
  //this is important but unfortunately takes up a lot of time to convolute for each component
  //get the component
  Component* mycomponent=componentptrvector[compindex];
  //ask for an analytical gradient if available
  Spectrum* gradient=getgradientptr(compindex,parameterindex);
  *this=(*gradient); //copy in y data in this



  //apply convolution if needed
  if (mycomponent->getcanconvolute()){
    //look for a convoluter component and calculate it
    for (size_t i=0;i<componentptrvector.size();i++){
      if (componentptrvector[i]->getconvolutor()){
	componentptrvector[i]->calculate();
#ifdef MODEL_DEBUG
	std::cout << "Calculating a convolutor component: "<<componentptrvector[i]->getname()<<"\n";
#endif
	*this=*(componentptrvector[i]); //copy output
       }
     }
  }
  //*gradient=*this;
  //gradient->display(getworkspaceptr());

  //Saysomething mysay(0,"Error","the component didn't appear to be valid");
  //copy the gradient to the array needed by the fitter
  //check wether size of array fits the spectrum
  if (array.size()!=this->getnpoints()) throw;
  for (size_t i=0;i<array.size();i++){
    array[i]=this->getcounts(i);
  }

  return;
}
void Model::setdetectorname(std::string name){
     detectorname=name;
     }
void Model::setdetectorgain(double gain){
     //set the gain factor for the CCD camera
     //apply this to the spectra since in principle they were recorded with this ccd
     //and they should convert their counts to electrons
      if (this->ismulti()){
         Multispectrum* mspec=this->getmultispectrumptr();
         mspec->setpppc(gain);
      }
      else{
           Spectrum* spec=this->getHLptr();
           spec->setpppc(gain);
      }
//force a redraw of all spectra
  this->updateHL();
}

void Model::setdetectorcorrfactor(double cfactor){
     detectorcorrfactor=cfactor;
}
double Model::getdetectorgain()const{
       return (this->getHLptr())->getpppc();
}

double Model::getdetectorcorrfactor()const{
       return detectorcorrfactor;
}
std::string Model::getdetectorname()const{
            return detectorname;
}


size_t Model::getcurrspecnr(){
    size_t currspecnr=0; //the default for no multispectrum
    if (ismulti()){
        currspecnr=getmultispectrumptr()->getcurrentslice();
    }
return currspecnr;
}

void Model::setallparamstocurrent(){
     if (this->ismulti()){
        const int oldslice=getcurrspecnr();
        for (unsigned int k=0;k<nrofspectra;k++){
            (this->getmultispectrumptr())->setcurrentslicenoupdate(k); //careful, change only the slice here,
            //if you do setcurrenspectrum also the according params will be loaded and that is not what we want now
            this->storeparams();

        }
    //and restore the to the slice that was selected
    this->setcurrentspectrum(oldslice);
     }
}

void Model::setalluserparamstocurrent(){
    if (this->ismulti()){
        const int oldslice=getcurrspecnr();
        for (unsigned int k=0;k<nrofspectra;k++){
            (this->getmultispectrumptr())->setcurrentslicenoupdate(k); //careful, change only the slice here,
            //if you do setcurrenspectrum also the according params will be loaded and that is not what we want now

            this->storeuserparams(getresultstring(oldslice)); //store the current params to this spectrum
        }
    //and restore the to the slice that was selected
    this->setcurrentspectrum(oldslice);
    //retrieve is not needed since we alreay where at the store parameters
    }
}


void Model::storeuserparams(std::string result){
//store user parameter values in a vector, leave this for the user, don`t use this function internally
    for (size_t j=0; j<this->getnrofallparams(); j++){

      uservalues[j+(this->getnrofallparams())*getcurrspecnr()]=(paramvector[j])->getvalue();
      uservalues_sigma[j+(this->getnrofallparams())*getcurrspecnr()]=(paramvector[j])->getsigma();

    }
     storemonitorstring(getcurrspecnr()); //store all monitor results
     storeresultstring(result,getcurrspecnr()); //store all monitor results
 userstored[getcurrspecnr()]=true;
}

void Model::storeparams(){
//store original parameter values in a vector, use this function internally to store parameters
    for (size_t j=0; j<this->getnrofallparams(); j++){

          originalvalues[j+(this->getnrofallparams())*getcurrspecnr()]=(paramvector[j])->getvalue();
          originalvalues_sigma[j+(this->getnrofallparams())*getcurrspecnr()]=(paramvector[j])->getsigma();
    }
stored[getcurrspecnr()]=true;
}



void Model::retrieveuserparams(){
  if (!userstored[getcurrspecnr()]) return;
    //retrieve user parameter from a vector
    for (size_t j=0; j<this->getnrofallparams(); j++){
        //force write this to the parameter, after all these where parameters that this model once put there
        Parameter* myparam=paramvector[j];
        //const bool oldstate=myparam->ischangeable();

        //don't write to locked parameters!, they are locked and we want to keep them like this
        //myparam->setchangeable(true);
        myparam->setvalue(uservalues[j+(this->getnrofallparams())*getcurrspecnr()]);
        myparam->setsigma(uservalues_sigma[j+(this->getnrofallparams())*getcurrspecnr()]);
        //myparam->setchangeable(oldstate);
      }
   //now force a calculate for these parameters
   this->calculate();
   geteelsmodelptr()->slot_componentmaintenance_updatemonitors(); //and update the monitors
}

void Model::retrieveparams(){
  if (!stored[getcurrspecnr()]) return;
    //retrieve user parameter from a vector
    for (size_t j=0; j<this->getnrofallparams(); j++){
        //force write this to the parameter, after all these where parameters that this model once put there
        Parameter* myparam=paramvector[j];
        //const bool oldstate=myparam->ischangeable();
        //myparam->setchangeable(true);
        myparam->setvalue(originalvalues[j+(this->getnrofallparams())*getcurrspecnr()]);
        myparam->setsigma(originalvalues_sigma[j+(this->getnrofallparams())*getcurrspecnr()]);
        //myparam->setchangeable(oldstate);
      }
   //now force a calculate for these parameters
   this->calculate();
   geteelsmodelptr()->slot_componentmaintenance_updatemonitors(); //and update the monitors
}

size_t Model::getnroffreeparameters()const{
    return freeparameters;
    }
Component* Model::getcomponentforfreeparam(size_t index){
    //return the component for free parameter with index
    if (checkindex(index))
        return freeparamcomponentvector[index];
    else {
        return 0;
    }
}

Parameter* Model::getfreeparam(size_t index){
    //return the free parameter with index
    if (checkindex(index))
        return freeparamvector[index];
    else {
        return 0;
    }
}

Parameter* Model::getfreelinparam(size_t index){
    //return the free parameter with index
    if (checklinindex(index))
        return freelinparamvector[index];
    else {
        return 0;
    }
}

Parameter* Model::getfreenonlinparam(size_t index){
    //return the free parameter with index
    if (checknonlinindex(index))
        return freenonlinparamvector[index];
    else {
        return 0;
    }
}

size_t Model::getfreeparamindex(size_t index){
    //return the index of the free parameter with index in the total parameter vector
    if (checkindex(index))
        return freeparamindex[index];
    else {
        return 0;
    }
}

size_t Model::getcomponentfreeparamindex(size_t index){
    //return the index of the free parameter with index in the total parameter vector
    if (checkindex(index))
        return freecomponentindex[index];
    else {
        return 0;
    }
}

int Model::getparamindexbypointer(const Parameter* ptr)const{
    //return the index in param of a parameter pointed to by pointer
    //-1 is returned is pointer is not found in the model
    for (size_t i=0;i<paramvector.size();i++) {
    if ((paramvector[i])==ptr){
      //found
       return (int)i;
    }
  }
  //not found
  return -1;
}

bool Model::checkindex(size_t index)const{
    return (index<freeparameters);
    }
bool Model::checklinindex(size_t index)const{
    return (index<freelinparameters);
    }

bool Model::checknonlinindex(size_t index)const{
    return (index<freenonlinparameters);
}

void Model::init(){
    //update the number of free parameters
    //and create new vectors pointing to the free parameters, the components these free parameters belong to
    //and their original indices in the model
    //carefull, this also erases all stored parameters
    freeparamvector.clear();
    freelinparamvector.clear();
    freenonlinparamvector.clear();
    freenonlinid.clear();
    freelinid.clear();
    freeparamcomponentvector.clear();
    freeparamindex.clear();
    freecomponentindex.clear();
    for (unsigned int i=0;i<componentptrvector.size();i++){
    int j=0;
    for (std::vector<Parameter*>::iterator it=(componentptrvector[i])->beginparameter();it<(componentptrvector[i])->endparameter();it++){
      //go through all parameters of the component
      //add to the freeparamvector if the parameter is changeable
      if ((*it)->ischangeable()){
        freeparamvector.push_back(*it);
        if ((*it)->islinear()){
             freelinparamvector.push_back(*it);
             freelinid.push_back(j);
        }else
        {
            freenonlinparamvector.push_back(*it);
            freenonlinid.push_back(j);
        }
        freeparamcomponentvector.push_back(componentptrvector[i]);
        freeparamindex.push_back(j);
        freecomponentindex.push_back(i);
        }
      j++;  //make sure j starts at 0
      }
    }
    freeparameters=freeparamvector.size();
    freelinparameters=freelinparamvector.size();
    freenonlinparameters=freenonlinparamvector.size();

    //update the number of spectra
    nrofspectra=1;
    if (ismulti()){
        nrofspectra=getmultispectrumptr()->getsize();
    }
}
size_t Model::getnrofspectra(){
    return nrofspectra;
    }
void Model::setcurrentspectrum(size_t index,bool redraw){
  //change the HL to a new current spectrum if we have a multispectrum
  if (!ismulti()) return;
  if (redraw){
    (this->getmultispectrumptr())->setcurrentslice(index); //change slice
    retrieveuserparams(); //get the parameters for this slice and calculate to update the model
    this->updateHL();

  }
  else{
    (this->getmultispectrumptr())->setcurrentslicenoupdate(index); //change slice but no redrawing
    retrieveuserparams(); //get the parameters for this slice and calculate to update the model
    this->updateHLnodraw(); //update the model, also does the retrieveuserparams
  }
}

void Model::save_params(std::string filename,bool dat,bool crlb,Fitter* myfitter){
  //save the whole user parameter storage to a file
  //either binary (dat=true) or in tab separated txt format
  //write crlbs instead of param values in binary format if clrb=true
    if (dat){
        //BINARY format
     //read it a real*8 in DM
     //the image will be axbxc with c the number of free params
     try{
     std::ofstream fp;

    //init a buffer for faster writing
     char buffer[BUFSIZE];
     fp.rdbuf()->pubsetbuf(buffer, BUFSIZE);

     //open AFTER setting the buffer!
     fp.open(filename.c_str(), std::ios_base::binary); //open for writing, overwrite if already exists


     //do the writing    
     for (size_t j=0; j<this->getnrofallparams(); j++){
        for (size_t i=0; i<nrofspectra; i++){
            double val=getuservalue(i,j);  //much faster to take the stored values
            if (crlb){
                //get the calculated crlb on the parameters instead of the param value
                val=getusersigma(i,j);   //much faster to take the stored values
                }
            if (!userstored[i]){
                val=0.0; //force to zero if not stored
            }
            fp.write((char *)&val, sizeof(val));
      }
     }
     //closing file not needed because handled in destructor of fp
     }
     catch(...){
     //not written
     }
    }
     else{
         //TXT format

         //open this file for writing ascii
        std::ofstream projectfile;


        //init a buffer for faster writing
        char buffer[BUFSIZE];
        projectfile.rdbuf()->pubsetbuf(buffer, BUFSIZE);

        //open the file, do this after initialising the buffer otherwise it might not work!!!!!
        projectfile.open(filename.c_str());

        if (!projectfile) {
            Saysomething mysay(0,"can't open file for writing");
        }

        //store the results in one report file
        save_fitter_details(projectfile,myfitter); //first save the fitter details and header info




        for (size_t i=0; i<nrofspectra; i++){
            save_report(projectfile,myfitter,i); //write it
        }
        //closing file not needed because handled in destructor of fp
    }
}
void Model::save_model(std::string filename,Fitter* myfitter){
  //save the model evaluated for each parameter in a binary file   
  //BINARY format
  //read it a real*8 in DM
  //the image will be same size as the SI 
  try{
     std::ofstream fp;
    //init a buffer for faster writing
     char buffer[BUFSIZE];
     fp.rdbuf()->pubsetbuf(buffer, BUFSIZE);
     //open AFTER setting the buffer!
     fp.open(filename.c_str(), std::ios_base::binary); //open for writing, overwrite if already exists

     //do the writing        
     for (size_t i=0; i<nrofspectra; i++){
    	 this->setcurrentspectrum(i,false); //make current model up to date and recalc without redrawing
         //and save the model
    	 for (size_t j=0; j<this->getnpoints(); j++){
    		 const double val=this->getcounts(j);
    	 		 fp.write((char *)&val, sizeof(val));
    	 }
     }
     
     //closing file not needed because handled in destructor of fp
  }
  catch(...){
     //not written
  }   
}

void Model::save_report(std::ofstream &projectfile,Fitter* myfitter,size_t specnr){
	//write the values in a row
	size_t parnr=0;
	//add the results of the monitors that were stored
    projectfile<<getmonitorstring(specnr);
    //and the results of the quality of the fit
    projectfile<<getresultstring(specnr)<<"\t";



    //list the parameters in a header
  	for (size_t i=0;i<(this->getcomponentsnr());i++){
		Component* mycomponent=this->getcomponent(i);
        //list the parameters in a header
        for (size_t j=0;j<mycomponent->get_nr_of_parameters();j++){
			Parameter* myparameter=mycomponent->getparameter(j);
            projectfile << getuservalue(specnr,parnr)<<"\t"; //much faster to take just the stored values
            if (myparameter->ischangeable()){
				projectfile << "free\t";
			}
            else{
                projectfile << "locked\t";
			}
            projectfile << getusersigma(specnr,parnr)<<"\t"; //much faster to take just the stored values
            parnr++;
		}
	}
	//end the line
	projectfile <<"\n";
}



void Model::save_fitter_details(std::ofstream &projectfile,Fitter* myfitter){
        //write a header with info
		projectfile <<"EELSMODEL report file\n";
		QDateTime now=QDateTime::currentDateTime();
		projectfile <<now.toString().toStdString()<<"\n";
		projectfile <<"FILENAME of HL spectrum:"<< (this->getHLptr())->getfilename() <<"\n";
		projectfile <<"NAME of HL spectrum:"<< (this->getHLptr())->getname() <<"\n";
		//set the precision of numbers
		//8 digits in engineering format
		projectfile.setf(std::ofstream::scientific);
		projectfile.precision(8);
		projectfile <<"Detector used:"<<this->getdetectorname()<<"\n";
		projectfile <<"Detector gain:"<<this->getdetectorgain()<<"\n";
		projectfile <<"Detector correlation factor:"<<this->getdetectorcorrfactor()<<"\n";
		projectfile <<"\n";
		projectfile <<"List of components and their parameters\n";


		//print the fitter settings
		if (myfitter!=0){
			projectfile <<"FITTER settings were\n";
			projectfile <<"\n";
			projectfile <<"FITTERTYPE:"<<myfitter->gettype()<<"\n";
			projectfile <<"FRACTION:"<<myfitter->getfraction()<<"\n";
			projectfile <<"MAXSTEP:"<<myfitter->getmaxstep()<<"\n";
			projectfile <<"MINSTEP:"<<myfitter->getminstep()<<"\n";
			projectfile <<"TOLERANCE:"<<myfitter->gettolerance()<<"\n";
			projectfile <<"USEGRADIENTS:"<<myfitter->getusegradients()<<"\n";
		}

        //make header line

        //list the monitors in a header
        for (size_t i=0;i<(this->getcomponentsnr());i++){
            Component* mycomponent=this->getcomponent(i);
            //list the parameters in a header
            for (size_t j=0;j<mycomponent->get_nr_of_parameters();j++){
                Parameter* myparameter=mycomponent->getparameter(j);
                Monitor* mymonitor=myparameter->getmonitor();
                if (mymonitor!=0){
                  projectfile << mymonitor->getheaderstring()<<"\t sigma \t"; //much faster to take just the stored values
                }
            }
        }


        if (myfitter!=0){
			projectfile <<"goodness_of_fit\t";
			projectfile <<"LRtestconfidence\t";
		}
		//print parameter names on the top row

		for (size_t i=0;i<(this->getcomponentsnr());i++){
			Component* mycomponent=this->getcomponent(i);
			//list the parameters
			const int compnr=mycomponent->get_nr_of_parameters();
			for (int j=0;j<compnr;j++){
				Parameter* myparameter=mycomponent->getparameter(j);
				projectfile << mycomponent->getname()<<": ";
				projectfile << myparameter->getname()<<"\t";
				projectfile << "free/locked\t";
				projectfile << "sigma \t";
			}
		}

		/*//add monitors if available
		for (size_t i=0;i<(this->getcomponentsnr());i++){
			Component* mycomponent=this->getcomponent(i);
			//list the parameters
			size_t compnr=mycomponent->get_nr_of_parameters();
			for (size_t j=0;j<compnr;j++){
				Parameter* myparameter=mycomponent->getparameter(j);
				if (myparameter->ismonitored()){
					Monitor* mymonitor=myparameter->getmonitor();
					if (mymonitor!=0){
						projectfile << "monitor on p"<<j<<"\t";
					}
				}
			}
		}*/

		//add fitter values
		//projectfile <<myfitter->goodness_of_fit_string()<<"\t";
		//projectfile <<myfitter->LRtestconfidence_string()<<"\t";

		projectfile <<"\n";//end the line
}


void Model::resetstorage(){
    //reset all user storage space, needed only when total nr of params changes
    //so only called by addcomponent and removecomponent and the constructors
    originalvalues.clear();
    originalvalues_sigma.clear();
    uservalues.clear();
    uservalues_sigma.clear();
    uservalues_monitorstring.clear();
    uservalues_resultstring.clear();
    stored.clear();
    userstored.clear();
    paramvector.clear();
    plotspecvector.clear();
    imdisplayvector.clear();
    //store a pointer to all parameters in this vector
    for (size_t i=0;i<componentptrvector.size();i++) {
        Component* mycomponent=componentptrvector[i];
        for (size_t j=0;j<mycomponent->get_nr_of_parameters();j++) {
            Parameter* myparam=mycomponent->getparameter(j);
            paramvector.push_back(myparam);
        }
    }
    plotspecvector.resize(paramvector.size());//make same size available for storing plots of parameters
    imdisplayvector.resize(paramvector.size());
    //create storage for parameters for each spectrum in the model
    const size_t allpars=getnrofallparams(); //do this after creating paramvector because it depends on its size!
    originalvalues.resize(allpars*nrofspectra);
    originalvalues_sigma.resize(allpars*nrofspectra);
    uservalues.resize(allpars*nrofspectra);
    uservalues_sigma.resize(allpars*nrofspectra);
    uservalues_monitorstring.resize(nrofspectra);
    uservalues_resultstring.resize(nrofspectra);
    stored.resize(nrofspectra,false); //set all to unstored status
    userstored.resize(nrofspectra,false); //set all to unstored status

}

size_t Model::getnrofallparams()const{
    //return number of ALL parameters in the model
    return paramvector.size();
}

double Model::getusersigma(size_t specnr, size_t paramnr)const{
    return uservalues_sigma[paramnr+(this->getnrofallparams())*specnr];
}
double Model::getuservalue(size_t specnr, size_t paramnr)const{
        return uservalues[paramnr+(this->getnrofallparams())*specnr];
}

void Model::setusersigma(size_t specnr, size_t paramnr,double value){
    uservalues_sigma[paramnr+(this->getnrofallparams())*specnr]=value;
    userstored[specnr]=true;
    return;
}
void Model::setuservalue(size_t specnr, size_t paramnr,double value){
        uservalues[paramnr+(this->getnrofallparams())*specnr]=value;
        userstored[specnr]=true;
        return;
}


size_t Model::getnroffreelinparameters()const{
    return freelinparameters;
}

size_t Model::getnroffreenonlinparameters()const{
    return freenonlinparameters;
}

size_t Model::getfreeindexfromnonlinindex(size_t j)const{
    //input index in freenonlinparamvector and get the index of that param in freeparamvector
    if (checknonlinindex(j))        {
        return freenonlinid[j];
    }else{
        return 0;
    }
}
size_t Model::getfreeindexfromlinindex(size_t j)const{
    //input index in freelinparamvector and get the index of that param in freeparamvector
    if (checklinindex(j))        {
        return freelinid[j];
    }else{
        return 0;
    }
}

void Model::storemonitorstring(size_t specnr,std::string s){
    uservalues_monitorstring[specnr]="";
    //add monitors if available
	for (size_t i=0;i<(this->getcomponentsnr());i++){
		Component* mycomponent=this->getcomponent(i);
        //list the parameters
        size_t compnr=mycomponent->get_nr_of_parameters();
        for (size_t j=0;j<compnr;j++){
			Parameter* myparameter=mycomponent->getparameter(j);
            if (myparameter->ismonitored()){
                Monitor* mymonitor=myparameter->getmonitor();
				if (mymonitor!=0){
				    if (s==""){
                        (uservalues_monitorstring[specnr]).append(mymonitor->getresultstringonlyvalues()); //only store the values and sigma
				    }
				    else{
				        (uservalues_monitorstring[specnr]).append(s); //only store the values and sigma
                    }
                    (uservalues_monitorstring[specnr]).append("\t");

                }
            }
        }
    }
}
std::string Model::getmonitorstring(size_t specnr){
    return uservalues_monitorstring[specnr];
}

void Model::storeresultstring(std::string result,size_t specnr){
    //a place to store the quality of the fit and the LR test result for each specnr, this helps to speed up the saving
    uservalues_resultstring[specnr]=result;
}
std::string Model::getresultstring(size_t specnr){
    return uservalues_resultstring[specnr];
}
void Model::printparameters()const{
    //print a list of current parameters
    std::cout << "Parameters: ";
	for (size_t i=0;i<(this->getcomponentsnr());i++){
		const Component* mycomponent=this->getcomponent(i);
        //list the parameters
        size_t compnr=mycomponent->get_nr_of_parameters();
        for (size_t j=0;j<compnr;j++){
			const Parameter* myparameter=mycomponent->getparameter(j);
            std::cout << myparameter->getname() << ":"<<myparameter->getvalue()<<"\t";
        }
    }
    std::cout <<"\n";
}
bool Model::hasstored(size_t specnr)const{
    if (specnr<userstored.size()){
        return userstored[specnr];
    }
    else
    {
        return false;
    }
}

void Model::updatemonitors(){
    //loop over all components and update their monitors if needed
    for (size_t i=0;i<(this->getcomponentsnr());i++){
		Component* mycomponent=this->getcomponent(i);
		mycomponent->updatemonitors();
    }
    //and store the results in the monitor storage
    storemonitorstring(this->getcurrspecnr());
}
double Model::getfirstnonexludeenergy()const{
	size_t bin=0;
	for (bin=0;bin<this->getnpoints();bin++){
	    if (!this->isexcluded(bin)) break;
	}
	return this->getenergy(bin);
}
