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
 * eelsmodel - components/dielecmodel.cpp
 **/

#include "src/components/dielecmodel.h"

#include <cmath>
#include <complex>
#include <iostream>
#include <sstream>

#ifdef DEBUG
    #include "debug_new.h" //memory leak checker
#endif
#include "src/core/parameter.h"

#include "src/gui/Drudeoptions.h"
#include "src/gui/eelsmodel.h"
#include "src/gui/integerinput.h"
#include "src/gui/imagedisplay.h"

class QWorkspace;

QWorkspace* getworkspaceptr();
EELSModel* geteelsmodelptr();

DielecModel::DielecModel()
:Component()
{
  name = "Dielectric Model";
  this->setdescription("Model for the dielectric function in low loss EELS");
  setshifter(false);
}
DielecModel::DielecModel(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{

nrofextraparams=6; //extra params outside of the lorentz functions



  if (parameterlistptr==0){
    //ask for nr of oscilators
    const int min=1;
    const int max=100;
    int d=10;
    Integerinput myinput(0,"","enter number of Lorentz oscillators",d,min,max);
    nosc=size_t(d);
    //add the required parameters

    //the extra parameters
    Parameter* p1=new Parameter("Eps0",1.0,1);
    p1->interactivevalue("enter Epsilon0");
    p1->setchangeable(false);
    Parameter* p2=new Parameter("strength",1.0,1);
    p2->interactivevalue("enter strength, propto beam current");
    this->addparameter(p1);
    this->addparameter(p2);
    //params needed for Kroeger formula
    Parameter* p3=new Parameter("E0",300e3,1);
    p3->interactivevalue("enter Primary energy");
    Parameter* p4=new Parameter("absolute thickness [m]",10.0e-9,1);
    p4->interactivevalue("enter absolute thickness in m");
    p3->setchangeable(false);
    p4->setchangeable(false);
    this->addparameter(p3);
    this->addparameter(p4);
    Parameter* p5=new Parameter("thetamax",5.0e-3,1);
    p5->interactivevalue("enter thetamax [rad]");
    p5->setchangeable(false);
    this->addparameter(p5);

    Parameter* p8=new Parameter("Optionstring",50,1); //default, no kroeger and set 50 thetapoints
    p8->setchangeable(false);
    this->addparameter(p8);

    //then enter the Lorentz oscillators
    for (size_t i=0;i<nosc;i++){
        addlorentzparams(i); //add a set of 3 params for each lorentz and give them appropriate names
    }


  }
  else{
    //get the parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
        Parameter* p=(*parameterlistptr)[i];
        this->addparameter(p);
    }
    nosc=(parameterlistptr->size()-nrofextraparams)/3; //get nr of oscillators from the size of the list 3/oscillator and 2 extra for Drude and nrofextraparams for strength and eps0 and ...
  }

  //give a name and description
  name = "Dielectric Model";
  this->setdescription("Model for the dielectric function in low loss EELS");

  this->setcanconvolute(true);
  setshifter(false); //seems to be really important here


  //create eps1 and eps2 spectrum
  eps1spectrum=new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  eps2spectrum=new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  eps1spectrum->name = "Epsilon1";
  eps2spectrum->name = "Epsilon2";
  eps1spectrum->display(getworkspaceptr());
  eps2spectrum->display(getworkspaceptr());

  //the last param is the option pointer
  optionparamptr=this->getparameter(6);
  getoptions(); //get the options

    //test for Kroeger
    kroegermatrix.resize(this->getnpoints(),thetapoints);
    kroegerim=0;
}


DielecModel::~DielecModel(){
    //delete the spectra
    if (eps1spectrum!=0){
        delete(eps1spectrum);
    }
     if (eps2spectrum!=0){
        delete(eps2spectrum);
    }
}



void DielecModel::calculate()
{
    //check for changes
    bool changed=false;
    for (size_t i=0;i<this->get_nr_of_parameters();i++){
        changed=changed|this->getparameter(i)->changed();
    }

    if (changed){
        //get the params and calculate eps1 and eps2
        const Parameter* eps0ptr= getparameter(0);
        const double eps0=eps0ptr->getvalue();
        const Parameter* strengthptr= getparameter(1);
        const double strength=strengthptr->getvalue();

        const Parameter* E0ptr= getparameter(2);
        const double E0=E0ptr->getvalue();
        const Parameter* thickptr= getparameter(3);
        const double t=thickptr->getvalue();

        const Parameter* thetamaxptr= getparameter(4);
        const double thetamax=thetamaxptr->getvalue();

        const double pi=acos(-1.0);


        //finaly calculate the loss function from this
        double alpha=1.0;
        bool firsttime=true;
        for (unsigned int i=0;i<(this->getnpoints());i++){
            const double en=this->getenergy(i);
            //const double ensq=pow(en,2.0);
            double Imfactor=0.0; //Im(-1/eps)
            double Refactor=1.0; //Re(1/eps)
            double sumgi=0.0;
            double eps1=eps0;
            double eps2=0.0;
            if (en>=0.0){
                //A set of Lorentz oscillator to model Im(-1/eps)
                for (size_t j=0;j<nosc;j++){
                    //the Lorentz part
                    const Parameter* Eiptr= getparameter(nrofextraparams+j*3);
                    const Parameter* fiptr= getparameter(nrofextraparams+1+j*3);
                    const Parameter* gammaiptr= getparameter(nrofextraparams+2+j*3);
                    const double Ei=Eiptr->getvalue();
                    //const double Eisq=pow(Ei,2.0);
                    const double fi=(fiptr->getvalue());
                    const double gammai=fabs(gammaiptr->getvalue());
                    //const double gammaisq=pow(gammai,2.0);

                    const double a=Ei;
                    const double b=gammai/2.0;
                    const double c=en;
                    const double asq=pow(a,2.0);
                    const double bsq=pow(b,2.0);
                    const double csq=pow(c,2.0);
                    const double prefac=fi*bsq*alpha; //alpha is a factor to normalise esp0
                    Imfactor+=prefac/(pow(en-Ei,2.0)+bsq); //a sum of lorentzian
                    //obtained with Mathematica, analytical solution of KKA for Lorentz function
                    //const double KKAIm=prefac*(a*(asq+bsq-csq)*(pi+2.0*atan2(a,b))+b*(asq+bsq+csq)*log((asq+bsq)/csq))/(2.0*b*(pow(asq,2.0)+2.0*asq*(bsq-csq)+pow(bsq+csq,2.0)));
                    const double KKAIm=prefac*a*(asq+bsq-csq)*pi/(b*(pow(asq,2.0)+2.0*asq*(bsq-csq)+pow(bsq+csq,2.0)));

                    Refactor-=(2.0/pi)*KKAIm; //1-2/pi KKAim
                    sumgi+=KKAIm;
                    //get eps1,eps2 from these
                    const double denom=pow(Imfactor,2.0)+pow(Refactor,2.0);
                    eps1=Refactor/denom;
                    eps2=Imfactor/denom;
                }

            }

            //store the eps1 and eps2 results in a spectrum
            eps1spectrum->setcounts(i,eps1);
            eps2spectrum->setcounts(i,eps2);
            double cts=0.0;
            if (dokroeger){
                cts=getlossKroeger(eps1,eps2,t,E0,thetamax,i,!dokroeger); //get loss function via Kroeger, if dokroeger is false, only the volume part is calculated
            }
            else{
                cts=getloss(eps1,eps2);
            }





            this->setcounts(i,cts);
            //if you come here for the first time, recalc alpha, energy should be close to zero
                if ((firsttime)&&(en>=0.0)){
                    //calc alpha to normalise eps0
                    //solve quadratic equation
                    const double a=eps0*(pow((2.0/pi)*sumgi,2.0)+pow(Imfactor,2.0));
                    const double b=sumgi*(2.0/pi)*(1.0-2.0*eps0);
                    const double c=eps0-1.0;
                    const double D=pow(b,2.0)-4.0*a*c;
                    if ((D>=0.0)&&(a!=0.0)){
                        const double alphap=(-b+sqrt(D))/(2.0*a); //we take only the positive solution...TEST...not sure what to do
                        const double alpham=(-b-sqrt(D))/(2.0*a);
                        if (alphap>0.0){
                            alpha=alphap;
                        }
                        else{
                            //maybe the other one is positive?
                            if (alpham>0.0){
                                alpha=alpham;
                            }
                            else{
                                //no realistic solution found
                                alpha=1.0;
                            }
                        }
                    }
                    else{
                        alpha=1.0;
                    }
                    i--; //redo this point with the corrected alpha
                    firsttime=false;
                }

        }


        //now normalise to one and scale with strength
        double area=0.0;
        for (unsigned int i=0;i<(this->getnpoints());i++){
            area+=this->getcounts(i);
        }
        for (unsigned int i=0;i<(this->getnpoints());i++){
            const double cts=this->getcounts(i);
            this->setcounts(i,strength*cts/area);
        }


        //set parameters as unchanged since last time we calculated
        this->setunchanged();
        //show the eps1 and eps2 spectra
        eps1spectrum->display(); //this triggers an overwrite of strengthptr///WHY? in the repaint procedure
        eps2spectrum->display();
    }

    if (showkroeger){
        displayKroeger();
    }

}


DielecModel* DielecModel::clone()const{
return new DielecModel(*this);}
DielecModel* DielecModel::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new DielecModel(n,estart,dispersion,parameterlistptr);
 }

double  DielecModel::getloss(double eps1,double eps2)const{
    //return a loss function from the dielectric function


    //add option to do Kroeger here
    const double loss=eps2/(pow(eps1,2.0)+pow(eps2,2.0)); //the loss function Im(-1/eps)
    return loss;

}






void DielecModel::changenosc(int d){
    size_t noscold=nosc;
    nosc=size_t(d);
    if (nosc==noscold){
         //do nothing
         return;
    }
    if (nosc>noscold){
         //add new points at end
         for (size_t i=0;i<(nosc-noscold);i++){
            addlorentzparams(noscold+i);
        }
    }
    else{
        //else remove them, this loses information
        for (size_t i=0;i<(noscold-nosc);i++){
            this->pullparameter();
            this->pullparameter();
            this->pullparameter();
        }
    }


    //update the storage
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    mymodel->resetstorage();

return;
}
void DielecModel::addlorentzparams(size_t i){
    //i is the index use for the name
     std::string name;
    std::ostringstream s;
    if ((s << "El"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
        // conversion worked
        name=s.str();
    }

    //create evenly distributed energy for the oscillators from 0 to maximum energy
    const double maxenergy=this->getenergy(this->getnpoints()-1);
    const double energy=(double(i)/double(nosc))*maxenergy;
    Parameter* Lp1=new Parameter(name,energy,1);
    Lp1->setboundaries(this->getenergy(0),this->getenergy(this->getnpoints()-1));
    this->addparameter(Lp1);
    std::ostringstream s2;
    if ((s2 << "f"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
        // conversion worked
        name=s2.str();
    }
    Parameter* Lp2=new Parameter(name,1.0,1);
    Lp2->setboundaries(0.1,1000.0);
    this->addparameter(Lp2);
    std::ostringstream s3;
    if ((s3 << "gamma"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
        // conversion worked
        name=s3.str();
    }
    const double gammabest=maxenergy/double(nosc); //estimate a good value for gamma
    Parameter* Lp3=new Parameter(name,gammabest,1);
    Lp3->setboundaries(0.1,100.0);
    this->addparameter(Lp3);
}

void DielecModel::displayKroeger(){
    //make a plot of the Kroeger result
    if (kroegerim==0){
        kroegerim=new Imagedisplay("kroeger test",&kroegermatrix);
    }
    kroegerim->updatereloadmatrix();
}



double DielecModel::getlossKroeger(double eps1,double eps2,double t,double E0,double thetamax,size_t i,bool onlyvolume){
    const double EN=this->getenergy(i);
    if (EN<=this->getdispersion()){
        return 0.0;
    }
//const double E00=E0/1000; // in kV
const double e=1.602177e-19;
const double h=1.05457e-34;
const double c = 2.998e8;
const double me = 9.1094e-31;

//some parameters that depend on the experiment

const std::complex<double>  eEl(eps1,-eps2); //carefull, eps2 has to be negative for Kroeger!

const double ev=1.0; //dielectric constant of vacuum

const double thetamin=1.0e-8;

const double steplog=exp(log(thetamax/thetamin)/(thetapoints-1.0));

//derived constants
const double mec2 = me*(pow(c,2.0))/e;
const double v=c*sqrt(1.0-1.0/(pow((1+E0/mec2),2.0)));
const double beta=v/c;

const double thetaE=EN*e/(me*pow(v,2.0));
const double dE=t*EN*e/(2.0*h*v);


double sumtheta=0.0;
double thetaold=0.0;
for (size_t j=0;j<thetapoints;j++){
    //LOG space is more efficient
    double theta=thetamin*pow(steplog,j); //log grid
    const double dt=theta-thetaold; //the step taken in theta
    double P=Pefl(theta,thetaE,v,beta,eEl,ev, dE, me,t,onlyvolume); //full Kroeger formula
    if (P<0.0){
        P=0.0;
    }
    sumtheta+=P*sin(theta)*dt;
    thetaold=theta;

    //store in kroegermatrix if needed
    if (showkroeger){
                kroegermatrix(i,j)=P;
    }
}

return sumtheta; //return the calculated loss
}





double DielecModel::Pefl(double theta,double thetaE,double v,double beta,std::complex<double> ec,double ev,double de,double me,double t,bool onlyvolume){
    //same notation as in
    //R.Erni UM doi:10.01016\j.ultramic.2007.03.005

    const double e=1.602177e-19;    
    const double h=1.05457e-34;

    const double thetasq=pow(theta,2.0);
    const double betasq=pow(beta,2.0);
    const double thetaEsq=pow(thetaE,2.0);
    const std::complex<double>lambdasq=thetasq-ec*betasq*thetaEsq;
    const std::complex<double> lambda=sqrt(lambdasq);
    const std::complex<double> phisq=lambdasq+thetaEsq;
    const std::complex<double> musq=1.0-ec*betasq;
    const std::complex<double> term1=musq*t/(ec*phisq);//bulk loss


    if (onlyvolume){
        //return fabs(pow(e/(Pi*h*v),2.0)*(e/(4.0*Pi*e0))*term1.imag()); //make sure to return no neg values with abs
        return term1.imag(); //make sure to return no neg values with abs
    }

    //calculate surface and retardation terms
    const std::complex<double> lambda0sq=thetasq-ev*betasq*thetaEsq;
    const std::complex<double> lambda0=sqrt(lambda0sq);
    std::complex<double>  tanhterm;
    std::complex<double>  cothterm;
    double sindesq;
    double cosdesq;
    if (integrate){
        //use integration from e1 to e2 to avoid interpolation issues on a discrete grid
        const double de2=de+(t*this->getdispersion()*e)/(4.0*h*v);//de+delta
        const double de1=de-(t*this->getdispersion()*e)/(4.0*h*v);//de-delta
        const double deltaE=de2-de1; //width of a pixel in Joule
        const std::complex<double> logsinhdiff=log(sinh(lambda*de2/thetaE)/sinh(lambda*de1/thetaE));
        const std::complex<double> logcoshdiff=log(cosh(lambda*de2/thetaE)/cosh(lambda*de1/thetaE));
        tanhterm=(1.0/deltaE)*(thetaE/lambda)*logcoshdiff;
        cothterm=(1.0/deltaE)*(thetaE/lambda)*logsinhdiff;

        const double sindiff=sin(de2)-sin(de1);
        const double cosdiff=cos(de2)-cos(de1);
        const double sinde=-cosdiff/(deltaE); //integrated between de1 and de2
        const double cosde=sindiff/(deltaE); //integrated between de1 and de2
        sindesq=pow(sinde,2.0);
        cosdesq=pow(cosde,2.0);
    }
    else{
        tanhterm=tanh(lambda*de/thetaE); //to speed up, use many times
        cothterm=1.0/tanhterm; //to speed up, use many times
        sindesq=pow(sin(de),2.0);
        cosdesq=pow(cos(de),2.0);
    }
    const std::complex<double> Lplus=lambda0*ec+ev*lambda*tanhterm;
    const std::complex<double> Lmin=lambda0*ec+ev*lambda*cothterm;
    const std::complex<double> phi0sq=lambda0sq+thetaEsq;
    const std::complex<double> phi01sq=thetasq+thetaEsq*(1.0-(ec-ev)*betasq);
    const std::complex<double> prefact2=-2.0*pow(ec-ev,2.0)*h*thetasq/(pow(phisq,2.0)*pow(phi0sq,2.0)*me*v);

    const std::complex<double> A=(sindesq/Lplus+cosdesq/Lmin)*pow(phi01sq,2.0)/(ec*ev);
    const std::complex<double> B=(1.0/Lplus-1.0/Lmin)*(lambda0*betasq*thetaE*phi01sq/ev)*sin(2.0*de);

    const std::complex<double> C=-(cosdesq*tanhterm/Lplus+sindesq*cothterm/Lmin)*lambda0*lambda*thetaEsq*pow(betasq,2.0);
    std::complex<double> cyefl=term1+prefact2*(A+B+C);

    if (std::isnan(cyefl.imag())){
        cyefl.imag(0.0);
    }

    //return fabs(pow(e/(Pi*h*v),2.0)*(e/(4.0*Pi*e0))*cyefl.imag()); //make sure to return no neg values with abs
    //TEST, prefactor is not important, rescaled anyway
    return cyefl.imag();

}

void DielecModel::setoptions(){
//call a function to set the options
    interactiveoptions();

    //force an update of the model
    Parameter* firstptr= getparameter(4);
    firstptr->forcechanged(); //otherwise the model doesn't want to calculate
    geteelsmodelptr()->componentmaintenance_doupdate(); //redraw the parameters in componentmaintenance
    geteelsmodelptr()->componentmaintenance_updatescreen();
    geteelsmodelptr()->slot_componentmaintenance_updatemonitors();//make sure the monitors are recalculated
}



void DielecModel::interactiveoptions(){
    Drudeoptions* myoptions=new Drudeoptions(getworkspaceptr(),"",nosc,thetapoints,integrate,showkroeger,dokroeger,false, 0);
    const int result=myoptions->exec();
    std::string optionstring="";
    if (result==1){
        //OK pressed
        thetapoints=myoptions->getthetapoints();
        kroegermatrix.resize(this->getnpoints(),thetapoints);
        if (kroegerim!=0){
            kroegerim->updatereloadmatrix(); //changes the image size to the new size?
            kroegerim->setname("Kroeger q,E plane");
        }

        dokroeger=myoptions->getdokroeger();
        if (dokroeger){
        integrate=myoptions->getintegrate();

        showkroeger=myoptions->getshow();
        }
        else{
            integrate=false;
            showkroeger=false;
        }

        if (!showkroeger){
            //kill the display
            if (kroegerim!=0) {
                delete(kroegerim);
                kroegerim=0;
            }
        }

        size_t newnosc=myoptions->getnosc();
        changenosc(newnosc);


    }

    //create a string to encode these options
    std::ostringstream s;
    s<<"Optionstring ";
    if (dokroeger){
        s << "K";
    }
    if (showkroeger){
        s << "S";
    }
    if (integrate){
        s << "I";
    }
    optionstring=s.str();

    //store it in the eps0 param
    optionparamptr->setchangeable(true);
    optionparamptr->setname(optionstring);
    optionparamptr->setvalue(thetapoints);
    optionparamptr->setchangeable(false);
return;
}


void DielecModel::getoptions(){
    //read the options from the special options parameter
    std::string optionstring=optionparamptr->getname();
    //find the options
    showkroeger=false;
    integrate=false;
    dokroeger=false;

    if (optionstring.find("K",0)!=std::string::npos ){
            dokroeger=true;
    }
    if ((optionstring.find("S",0)!=std::string::npos )&&(dokroeger)){
            showkroeger=true;
    }
    if ((optionstring.find("I",0)!=std::string::npos )&&(dokroeger)){
            integrate=true;
    }
    thetapoints=size_t(optionparamptr->getvalue());
return;
}






