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
 * eelsmodel  - components/dieleclinear.cpp
 **/

//#define COMPONENT_DEBUG
#define LINEAR

#include "src/components/dieleclinear.h"

#include <cmath>
#include <complex>
#include <iostream>
#include <string>
#include <sstream>

#ifdef DEBUG
    #include "src/core/debug_new.h" //memory leak checker
#endif

#include "src/core/eelsmodel.h"
#include "src/core/parameter.h"
#include "src/core/curvematrix.h"

#include "src/gui/Drudeoptions.h"
#include "src/gui/imagedisplay.h"
#include "src/gui/integerinput.h"

class QWorkspace;

QWorkspace* getworkspaceptr();
Eelsmodel* geteelsmodelptr();

DielecLinear::DielecLinear()
:Component()
{
  nrofextraparams=7;
  this->setname("Dielectric Model Linear");
  this->setdescription("Piecewise linear model for the loss function in low loss EELS");
  setshifter(false);
  optionparamptr=0;
  kroegerim=0;
  outputformat=0;
}
DielecLinear::DielecLinear(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
outputformat=0;
nrofextraparams=7; //extra params outside of the points for modelling the loss function
  if (parameterlistptr==0){
    //ask for nr of oscilators
    const int min=1;
    const int max=this->getnpoints();
    int d=10;
    Integerinput myinput(0,"","enter number of points",d,min,max);
    degree=size_t(d);

    //add the required parameters

    //the extra parameters
    Parameter* p1=new Parameter("Eps0",1.0,1);
    p1->interactivevalue("enter Epsilon0");
    p1->setchangeable(false);
    this->addparameter(p1);

    Parameter* p2=new Parameter("strength",1.0,1);
    p2->setboundaries(0,1e30);
    p2->interactivevalue("enter strength, propto beam current");
    this->addparameter(p2);

    //params needed for Kroeger formula
    Parameter* p3=new Parameter("E0",300e3,1);
    p3->interactivevalue("enter Primary energy");
    p3->setchangeable(false);
    this->addparameter(p3);

    Parameter* p4=new Parameter("absolute thickness [m]",10.0e-9,1);
    p4->interactivevalue("enter absolute thickness in m");
    p4->setchangeable(false);
    p4->setboundaries(0,10e-6);
    this->addparameter(p4);

    Parameter* p5=new Parameter("thetamax",5.0e-3,1);
    p5->interactivevalue("enter thetamax [rad]");
    p5->setchangeable(false);
    this->addparameter(p5);

    Parameter* p6=new Parameter("Estop [eV]",this->getenergy(this->getnpoints()-1),1);
    p6->interactivevalue("enter Estop [eV]");
    p6->setchangeable(false);
    this->addparameter(p6);

    Parameter* p7=new Parameter("Optionstring",50,1); //default, no kroeger and set 50 thetapoints
    p7->setchangeable(false);
    this->addparameter(p7);



    //then enter the piecewise linear points
    for (size_t i=0;i<degree;i++){
        addlinparams(i);
    }
  }
  else{
    //get the parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
        Parameter* p=(*parameterlistptr)[i];
        this->addparameter(p);
        p->setchanged(); //make sure everything is calculated correctly
    }
    degree=(parameterlistptr->size()-nrofextraparams); //get nr of oscillators from the size of the list
  }

  //give a name and description
  this->setname("Dielectric Model Linear");
  this->setdescription("Piecewise linear model for the loss function in low loss EELS");

  this->setcanconvolute(true);
  setshifter(false); //seems to be really important here

  //create eps1 and eps2 spectrum
  eps1spectrum=new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  eps2spectrum=new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
  eps1spectrum->setname("Epsilon1");
  eps2spectrum->setname("Epsilon2");
  eps1spectrum->display(getworkspaceptr());
  eps2spectrum->display(getworkspaceptr());

  //the last param is the option pointer
  optionparamptr=this->getparameter(6);
  getoptions(); //get the options

    //test for Kroeger
    /*
 kroegermatrix.resize(this->getnpoints(),thetapoints);
 kroegerim=0;
 */
 this->showequalizer();
}


DielecLinear::~DielecLinear(){
    //delete the spectra
    if (eps1spectrum!=0){
        delete(eps1spectrum);
    }
     if (eps2spectrum!=0){
        delete(eps2spectrum);
    }
}



void DielecLinear::calculate()
{
    const Parameter* strengthptr= this->getparameter(1);
    const double strength=strengthptr->getvalue();

    //check for changes
    bool changed=false;
    for (size_t i=0;i<this->get_nr_of_parameters();i++){
        const Parameter* parptr=this->getparameter(i);
        if (parptr!=strengthptr){
            //skip strengthptr, if only that has changed we just need to renormalise
        changed=changed|parptr->changed();
        }
    }

    if (changed){
	
#ifdef COMPONENT_DEBUG
    	std::cout <<"parameters have changed calculating again\n";
    	
#endif
    	
        //get the params and calculate eps1 and eps2
        const Parameter* eps0ptr= this->getparameter(0);
        const double eps0=eps0ptr->getvalue();

        const Parameter* E0ptr= this->getparameter(2);
        const double E0=E0ptr->getvalue();

        const Parameter* thickptr= this->getparameter(3);
        const double t=thickptr->getvalue();

        const Parameter* thetamaxptr= this->getparameter(4);
        const double thetamax=thetamaxptr->getvalue();

        const Parameter* Estopptr= this->getparameter(5);
        const double Estop=Estopptr->getvalue();

        const double pi=acos(-1.0);
       
        const double edistance=Estop/double(degree+1); //energy step size

        //physical constants
        const double e=1.602177e-19;
        const double h=1.05457e-34;
        const double c = 2.998e8;
        const double me = 9.1094e-31;

        //some constants independent on energy loss
        const double mec2 = me*(pow(c,2.0))/e;
        const double v=c*sqrt(1.0-1.0/(pow((1+E0/mec2),2.0)));
        const double beta=v/c;
        const std::complex<double> betasq(pow(beta,2.0),0.0);//needed in lossrel, complex because lossrel is going to do complex calculations with this
        const double lambda=2*pi*h/(me*v)*sqrt(1.0-pow(beta,2.0)); //de broglie wavelength of fast electron in m
        const double k0=2*pi/lambda; //wave vector in 1/m
        const double qmax=k0*thetamax;
        const double thetamin=1e-6; //smaller than 1urad makes no sense
        const double qmin=k0*thetamin;

        double alpha=1.0; //correction factor to get eps0 right           

        //normalise the ai parameters so that they stay within a reasonable range
        //avg=1
        //find average
        double avg=0.0;
        for (size_t  j=0;j<degree;j++){
            const double ai=(this->getparameter(j+nrofextraparams))->getvalue();
            avg+=ai;
        }
        avg=avg/degree;
        if (avg<0){avg=1.0;}
        //rescale
        for (size_t  j=0;j<degree;j++){
            const double ai=(this->getparameter(j+nrofextraparams))->getvalue();
            (this->getparameter(j+nrofextraparams))->setvalue(ai/avg);
        }
#ifdef COMPONENT_DEBUG
    	std::cout <<"renormalised params avg was="<<avg<<"\n";
    	avg=0.0;
    	for (size_t  j=0;j<degree;j++){
    	            const double ai=(this->getparameter(j+nrofextraparams))->getvalue();
    	            std::cout <<ai<<"  ";
    	            avg+=ai;
    	}
    	std::cout <<"\n";
    	avg=avg/degree;
    	std::cout <<"new avg="<<avg<<" should be 1.0 \n";
#endif
        
        //prepare some stuff that is needed for every energy to speed up
        avec.clear();
        bvec.clear();
        en1vec.clear();
        en2vec.clear();
        en1sqvec.clear();
        en2sqvec.clear();
        for (size_t  j=0;j<(degree+1);j++){ //there are degree+1 linear parts in the loss function
                            const double en1=double(j)*edistance;
                            const double en2=double(j+1)*edistance;
                            const double en1sq=pow(en1,2.0);
                            const double en2sq=pow(en2,2.0);
                            double a0=0.0;
                            if (j!=0){
                                a0=(this->getparameter(j+nrofextraparams-1))->getvalue();
                            }
                            double a1=0.0;
                            if (j<degree){
                               a1=(this->getparameter(j+nrofextraparams))->getvalue();
                            }
                            const double a=(a1-a0)/edistance; //slope
                            const double b=a0-a*en1;
                            //store them for later use
                            avec.push_back(a);
                            bvec.push_back(b);
                            en1vec.push_back(en1);
                            en2vec.push_back(en2);
                            en1sqvec.push_back(en1sq);
                            en2sqvec.push_back(en2sq);
        }                  
        
        //start to calculate the Im(1/eps) parts as function of energy
            for (int i=-1;i<int(this->getnpoints());i++){//i=-1 special cycle to calculate at E=0 to normalise with eps0              
                double G=0.0;
                double Refactor=0.0;
                double Imfactor=0.0;
                double en=0.0;
                if (i>-1){
                    en=this->getenergy(i);
                }
                const double enpsq=pow(en,2.0);
                //analytical solution of the linear integral
                 for (size_t  j=0;j<(degree+1);j++){ //there are degree+1 linear parts in the loss function                                          
                     const double c=fabs(en2sqvec[j]-enpsq)/fabs(en1sqvec[j]-enpsq);
                     if ((en>en1vec[j])&&(en<=en2vec[j])){
                        Imfactor=alpha*(avec[j]*en+bvec[j]); //piecewise linear for the Im(1/eps)
                     }
                     if ((en>=0)&&(en1sqvec[j]!=enpsq)&&(en2sqvec[j]!=enpsq)&&(en2vec[j]>=0)&&(en1vec[j]>=0)){
                        //solve KKA for each linear part to get Real(1/eps)
                        const double gi=(pi/2.0)*((alpha*bvec[j]/2.0)*log(c)+alpha*avec[j]*(edistance+(en/2.0)*(log(fabs((en2vec[j]-en)/(en1vec[j]-en)))-log(fabs((en2vec[j]+en)/(en1vec[j]+en))))));
                        if (!std::isnan(gi)){
                            G+=gi;
                        }
                    }
                 }
            Refactor=1.0-G;
            //and get eps1, eps2 from this
            const double denom=pow(Imfactor,2.0)+pow(Refactor,2.0);
            const double eps1=Refactor/denom;
            const double eps2=Imfactor/denom;

            if ((i==-1)&&(eps0>1.01)){
                //special step at en=0 to normalise
                //eps1(0) has to be scaled to eps0
                alpha=(1.0-1.0/eps0)/G;
                const double minalpha=0.00001;
                const double maxalpha=100000.0;
                if ((std::isnan(alpha))||(alpha<minalpha)||(alpha>maxalpha)){
                    alpha=1.0;
                }                   
            }
            if (i>-1){ //for all other energies do the calculation
                //store the eps1,eps2 and calc the loss function

                switch(outputformat){
                    case 1:
                        eps1spectrum->setcounts(i,-Refactor);
                        eps2spectrum->setcounts(i,Imfactor);
                        break;
                    case 2:
                        {const double epstot=sqrt(pow(eps1,2.0)+pow(eps2,2.0));
                        const double refractive=sqrt((epstot+eps1)/2.0);
                        const double K=sqrt((epstot-eps1)/2.0);
                        eps1spectrum->setcounts(i,refractive);
                        eps2spectrum->setcounts(i,K);
                        break;}
                    default :
                        eps1spectrum->setcounts(i,eps1);
                        eps2spectrum->setcounts(i,eps2);
                }

                const std::complex<double> eps(eps1,eps2);
                double cts=0.0;
                if (dokroeger){
                    //get loss from Kroeger formula
                    cts=getlossKroeger(eps1,eps2,t,E0,thetamax,i,false);
                }
                else{
                    //or only bulk nonrelativistic (fast)
                    double qE=e*en/(h*v);
                    if (qE<qmin){qE=qmin;}
                    if (en>=this->getdispersion()){//skip the first pixel around en=0
                        if (!dorel){
                            //simple Im(1/eps)
                            //cts=getloss(eps1,eps2,qmax,qE);
                            cts=LossRel(eps,en,qmax,0.0,qE);
                        }
                        else
                        {
                            //the relativistic version, analytically integrated up to qmax
                            cts=LossRel(eps,en,qmax,betasq,qE);
                        }
                    }
                }
                this->setcounts(i,cts);
             }
        }       
        this->normalize(strength);

    }
else if (strengthptr->changed()){
    //if only the strengthptr changed, this is faster
    this->normalize(strength);
#ifdef COMPONENT_DEBUG
    	std::cout <<"only strengthptr has changed calculating again\n";    	
#endif
}
this->setunchanged();


/*
    if (showkroeger){
        displayKroeger();
    }
*/
}

//inherit show but also redefine what should happen with the dos window
void DielecLinear::show(){
        //show the eps1 and eps2 spectra
    if (eps1spectrum!=0) eps1spectrum->display(getworkspaceptr());
    if (eps2spectrum!=0) eps2spectrum->display(getworkspaceptr());
}

DielecLinear* DielecLinear::clone()const{
return new DielecLinear(*this);}
DielecLinear* DielecLinear::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new DielecLinear(n,estart,dispersion,parameterlistptr);
 }

/*double  DielecLinear::getloss(double eps1,double eps2,double qmax,double qE)const{
    //return a loss function from the dielectric function
    //bulk non rel
    double loss=0.0;
         loss=log(1.0+pow(qmax/qE,2.0))*eps2/(pow(eps1,2.0)+pow(eps2,2.0)); //the loss function Im(-1/eps)
    return loss;
}*/


void DielecLinear::changedegree(int d){
    size_t degreeold=degree;
    degree=size_t(d);
    if (degree==degreeold){
         //do nothing
         return;
    }
    if (degree>degreeold){
         //add new points at end
         for (size_t i=0;i<(degree-degreeold);i++){
            addlinparams(degreeold+i);
        }
    }
    else{
        //else remove them, this loses information
        for (size_t i=0;i<(degreeold-degree);i++){
            this->pullparameter();
        }
    }


    //update the storage
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    mymodel->resetstorage();
    this->showequalizer(); //redraw the equaliser with new number of sliders
return;
}
void DielecLinear::addlinparams(size_t i){
    //i is the index use for the name
    std::string name;
    std::ostringstream s;
    if ((s << "a"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
        // conversion worked
        name=s.str();
    }
    Parameter* Lp1=new Parameter(name,0.1,1);
    Lp1->setboundaries(-0.01,20.0); //allow slightly negative to make going to 0 possible
    this->addparameter(Lp1);
}

void DielecLinear::displayKroeger(){
    //make a plot of the Kroeger result
    /*
    if (kroegerim==0){
        kroegerim=new Imagedisplay(getworkspaceptr(),"kroeger test",&kroegermatrix);
    }
    kroegerim->updatereloadmatrix();
    */
}



double DielecLinear::getlossKroeger(double eps1,double eps2,double t,double E0,double thetamax,size_t i,bool onlyvolume){
    const double EN=this->getenergy(i);
    if (EN<=this->getdispersion()){
        return 0.0;
    }

//constants
const double e=1.602177e-19;
const double h=1.05457e-34;
const double c = 2.998e8;
const double me = 9.1094e-31;

//derived constants
const double mec2 = me*(pow(c,2.0))/e;
const double v=c*sqrt(1.0-1.0/(pow((1+E0/mec2),2.0)));
const double beta=v/c;
//some parameters that depend on the experiment
const double ev=1.0; //dielectric constant of vacuum
const double thetamin=1.0e-8;
const double steplog=exp(log(thetamax/thetamin)/(thetapoints-1.0));



const double thetaE=EN*e/(me*pow(v,2.0));
const double dE=t*EN*e/(2.0*h*v);


double sumtheta=0.0;
double thetaold=0.0;

for (size_t j=0;j<thetapoints;j++){
    //numerical integration of real Kroeger formula
    //LOG space is more efficient
    double theta=thetamin*pow(steplog,j); //log grid
    const double dt=theta-thetaold; //the step taken in theta
    double P=0.0;

    const std::complex<double>  eEl(eps1,-eps2); //carefull, eps2 has to be negative for Kroeger!
    P=Pefl(theta,thetaE,v,beta,eEl,ev, dE, me,t,false); //full Kroeger formula

    if (P<0.0){ //only keep positive losses, no gains
        P=0.0;
    }
    sumtheta+=P*sin(theta)*dt;
    thetaold=theta;

    //store in kroegermatrix if needed
    /*
    if (showkroeger){
                kroegermatrix(i,j)=P;
    }
    */
}

return sumtheta; //return the calculated loss
}



double DielecLinear::LossRel(std::complex<double> eps,double E,double qmax,const std::complex<double> betasq,double qE){

    //the q dependent part can be solved analytically IF we assume eps is not depending on q!
    //much faster, no numerical q intergration needed    
    const double Pi=acos(-1.0);
      
    //be safe with complex numbers to avoid casting to doubles
    const std::complex<double> qe(qE,0.0);    
    const std::complex<double> qmaxsq(pow(qmax,2.0),0.0);
    const std::complex<double> qesq=pow(qe,2.0);
    const std::complex<double> one(1.0,0.0);
    const std::complex<double> i(0.0,1.0);
    const std::complex<double> alpha=one-eps*betasq;
  
            
    //take care of complex log
    //make sure we are always in the positive branch of the ln(z)
    //for the imaginary part
    const std::complex<double> z=(qmaxsq+qesq*alpha)/(qesq*alpha);
    const std::complex<double> qintegral=log(norm(z))+i*fabs(arg(z));
        
    double loss=-(Pi)*imag( (alpha/eps)*qintegral);       
    if (loss<0.0) loss=0.0; //don't return negative losses
 
    return loss;  
}


double DielecLinear::Pefl(double theta,double thetaE,double v,double beta,std::complex<double> ec,double ev,double de,double me,double t,bool onlyvolume){
    //same notation as in
    //R.Erni UM 108 (2008) 84-99

    const double e=1.602177e-19;
    const double h=1.05457e-34;

    const double thetasq=pow(theta,2.0);
    const double betasq=pow(beta,2.0);
    const double thetaEsq=pow(thetaE,2.0);
    const std::complex<double>lambdasq=thetasq-ec*betasq*thetaEsq;
    const std::complex<double> lambda=sqrt(lambdasq);
    const std::complex<double> one(1.0,0.0);    
    const std::complex<double> T(t,0.0);   
    const std::complex<double> phisq=lambdasq+thetaEsq;
    const std::complex<double> musq=one-ec*betasq;
    const std::complex<double> term1=musq*T/(ec*phisq);//bulk loss


    if (onlyvolume){
        return term1.imag();
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
        cothterm=one/tanhterm; //to speed up, use many times
        sindesq=pow(sin(de),2.0);
        cosdesq=pow(cos(de),2.0);
    }
    const std::complex<double> Lplus=lambda0*ec+ev*lambda*tanhterm;
    const std::complex<double> Lmin=lambda0*ec+ev*lambda*cothterm;
    const std::complex<double> phi0sq=lambda0sq+thetaEsq;
    const std::complex<double> phi01sq=thetasq+thetaEsq*(one-(ec-ev)*betasq);
    const std::complex<double> prefact2=-2.0*pow(ec-ev,2.0)*h*thetasq/(pow(phisq,2.0)*pow(phi0sq,2.0)*me*v);

    const std::complex<double> A=(sindesq/Lplus+cosdesq/Lmin)*pow(phi01sq,2.0)/(ec*ev);
    const std::complex<double> B=(one/Lplus-one/Lmin)*(lambda0*betasq*thetaE*phi01sq/ev)*sin(2.0*de);

    const std::complex<double> C=-(cosdesq*tanhterm/Lplus+sindesq*cothterm/Lmin)*lambda0*lambda*thetaEsq*pow(betasq,2.0);
    std::complex<double> cyefl=term1+prefact2*(A+B+C);

    if (std::isnan(cyefl.imag())){
        cyefl.imag()=0.0;
    }

    //return fabs(pow(e/(Pi*h*v),2.0)*(e/(4.0*Pi*e0))*cyefl.imag()); //make sure to return no neg values with abs
    //TEST, prefactor is not important, rescaled anyway
    return cyefl.imag();

}


void DielecLinear::setoptions(){
//call a function to set the options
    interactiveoptions();
    //force an update of the model
    Parameter* firstptr= this->getparameter(nrofextraparams);
    firstptr->forcechanged(); //otherwise the model doesn't want to calculate
    geteelsmodelptr()->componentmaintenance_doupdate(); //redraw the parameters in componentmaintenance
    geteelsmodelptr()->componentmaintenance_updatescreen();
    geteelsmodelptr()->slot_componentmaintenance_updatemonitors();//make sure the monitors are recalculated
    
    //auto update params for a good first guess, sometimes this is unwanted
    //TODO make an options in the interactiveoptions to choose this
    estimateparams();
}



void DielecLinear::interactiveoptions(){
    Drudeoptions* myoptions=new Drudeoptions(getworkspaceptr(),"",degree,thetapoints,integrate,showkroeger,dokroeger,dorel,outputformat);
    const int result=myoptions->exec();
    std::string optionstring="";
    if (result==1){
        //OK pressed
        thetapoints=myoptions->getthetapoints();
        /*kroegermatrix.resize(this->getnpoints(),thetapoints);
        if (kroegerim!=0){
            kroegerim->updatereloadmatrix(); //changes the image size to the new size?
            kroegerim->setname("Kroeger q,E plane");
        }
        */
        outputformat=myoptions->getoutputformat();
        switch (outputformat){
            case 1:
            eps1spectrum->setname("Re(-1/eps)");
            eps2spectrum->setname("Im(-1/eps)");
            break;
            case 2:
            eps1spectrum->setname("n");
            eps2spectrum->setname("K");
            break;
            default :
            eps1spectrum->setname("Epsilon1");
            eps2spectrum->setname("Epsilon2");
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
        dorel=myoptions->getdorel();
        if (dorel){
            integrate=myoptions->getintegrate();
            showkroeger=myoptions->getshow();
        }

/*
        if (!showkroeger){
            //kill the display
            if (kroegerim!=0) {
                delete(kroegerim);
                kroegerim=0;
            }
        }
        */

        size_t newdegree=myoptions->getnosc();
        changedegree(newdegree);
}

    //create a string to encode these options
    std::ostringstream s;
    s<<"Optionstring ";
    if (dokroeger){
        s << "K";
    }
    if (dorel){
        s << "R";
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


void DielecLinear::getoptions(){
    //read the options from the special options parameter
    std::string optionstring=optionparamptr->getname();
    //find the options
    showkroeger=false;
    dorel=false;
    integrate=false;
    dokroeger=false;
    dorel=false;

    if (optionstring.find("K",0)!=std::string::npos ){
            dokroeger=true;
    }
    if (optionstring.find("R",0)!=std::string::npos ){
            dorel=true;
            dokroeger=false;
    }
    if ((optionstring.find("S",0)!=std::string::npos )&&((dokroeger)||(dorel))){
            showkroeger=true;
    }
    if ((optionstring.find("I",0)!=std::string::npos )&&((dokroeger)||(dorel))){
            integrate=true;
    }

    thetapoints=size_t(optionparamptr->getvalue());
return;
}
void DielecLinear::estimateparams(){
	//make a first estimate of the params starting from the experimental data
	//assume all other components are mimicking the zero loss
	
	//what to do with a fourier log component?
		
	    Parameter* strengthptr= getparameter(1);	       
	    Parameter* Estopptr= getparameter(5);
	    const double Estop=Estopptr->getvalue();
	    const double Estep=Estop/double(degree+1);	  
	    const Model* mymodel=geteelsmodelptr()->getmodel();	    	   	  
	    
	    //first find out total integral minus all other components
	    double areaspec=0.0;
	    for (unsigned int i=0;i<(this->getnpoints());i++){	        	          	                      
	            const double energy=this->getenergy(i);	            
	            if ((energy>=0)&&(energy<Estop)){
	            	areaspec+=mymodel->getHLptr()->getcounts(i);	
	            	//subtract all other components except ourselves
	            	//TODO do something sensible if a fourier log component is included
	            	for (size_t j=0;j<mymodel->getcomponentsnr();j++){
	            		const Component* mycomponent=mymodel->getcomponent(j);
	            		if ((mycomponent!=this)&&(!(mycomponent->getconvolutor())) ){
	                        areaspec-=mycomponent->getcounts(i);	                       
	            		}	            
	            	}	            		            	            	            	            					            	            			            		            	            		            		            	            
	            }	          
	    }	    
	    //recalibrate strength accordingly	 
	    strengthptr->setvalue(areaspec);
	    
	    
	    //then find out strength in intervals to adjust the a parameters
	    double areainterval=0.0; //the are in an interval of the experiment
	  	size_t jindex=0;
	    for (unsigned int i=0;i<(this->getnpoints());i++){	        	   	           	   	            
	   	            const double energy=this->getenergy(i);	   	            
	   	            if ((energy>=0)&&(energy<Estop)){
	   	            	areainterval+=mymodel->getHLptr()->getcounts(i);
	   	            	//subtract all other components except ourselves
	   	            	//TODO do something sensible if a fourier log component is included
	   	            	for (size_t j=0;j<mymodel->getcomponentsnr();j++){
	   	            		const Component* mycomponent=mymodel->getcomponent(j);
	   	            		if ((mycomponent!=this)&&(!(mycomponent->getconvolutor())) ){  	                       
	   	                        areainterval-=mycomponent->getcounts(i);
	   	            		}	            
	   	            	}	            		            	            	            	            					            	            			            		            	   	            	
	   	            	//find out in what energy interval we are
	   	            	if (energy>((double(jindex)+1.5)*Estep)){
	   	            			//adjust parameter accordingly
	   	            			Parameter* aptr= getparameter(nrofextraparams+jindex);
	   	            			const double newval=areainterval/areaspec;
	   	            			aptr->setvalue(newval);
	   	            			//std::cout <<"newval="<<newval<<" jindex="<<jindex<<"areainterval="<<areainterval<<" getvalue="<<aptr->getvalue()<<"\n";
	   	            			jindex++;	            				            			
	   	            			areainterval=0; //reset interval area to zero for next interval
	   	            	}	   	            		   	            	
	   	            }
	   	          
	   	    }	 		
}






