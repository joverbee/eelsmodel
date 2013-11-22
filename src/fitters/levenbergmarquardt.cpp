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
 * eelsmodel - fitters/levenbergmarquadt.cpp
 **/

 //A Levenberg Marquardt fitter for Poisson distributed noise
 //It also implements a trick to speed up by calculating the linear parameters directly from the nonlinear
 //This implementation is partially based on:
 //A. Van den Bos, Parameter Estimation for scientists and engineers, wiley, ISBN 978-0-470-14781-8, chapter 6
 //W.H.Lawton,E.A.Sylvestre, eliminiation of linear paramters in nonlinear regression, technometrics vol 13-3 461-467
 //J.J. Mor\'e , the levenberg marquardt algorithm: implementation and theory, US Energy Research and Development Administration

 //the method was first implemented in Matlab for testing, it was shown there is a small bias when choosing normal distribution on poisson experimental data

#include "src/fitters/levenbergmarquardt.h"

//#define FITTER_DEBUG
//#define FITTER_DEBUG_DETAIL
//#define FITTER_DEBUG_LIN

#include <cmath>
#include <cstdio>

#include "src/core/component.h"
#include "src/core/model.h"
#include "src/core/monitor.h"

LevenbergMarquardt::LevenbergMarquardt(Model* m)
:Fitter(m)
{
settype("Levenberg Marquardt Poisson");
setcandolin(true); //we can do a special trick with linear parameters to speed up things
#ifdef FITTER_DEBUG
 std::cout <<"constructed LM Poisson\n";
 #endif
}
LevenbergMarquardt::~LevenbergMarquardt(){
}
double LevenbergMarquardt::goodness_of_fit()const{
  return -likelyhoodfunction();
}
double LevenbergMarquardt::likelyhoodfunction()const{
  //calculate the real log likelyhood
  //the probability that this experiment was created by this model with these parameters
  double likelyhood=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!(modelptr->isexcluded(i))){//only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      //made use of stirling formula
      //log(n!)~nlog(n)-n for large n
      if ((fit>1.0)&&(exper>1.0)) {
        likelyhood+=2.0*(exper*log(fit/exper)-fit+exper); //numerically better to do ln(fit/exper) because fit~=exper
        //factor of 2 is to have a good comparison with chisquare values
        //and this is same as the likelihood ratio this number should be roughly equal to degrees of freedom
        //for more accurate statement see LR comments
       }
       if (fit<0.0){
           likelyhood=-(std::numeric_limits<double>::max)(); //a model that goes negative is not possible for Poisson, return -infty
        }
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"real likelihood before correction corrfactor: "<<likelyhood<<" the corrfactor="<<(modelptr->getdetectorcorrfactor())<<"\n";
  #endif
  return likelyhood;
}


std::string LevenbergMarquardt::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  //divide by number of points...TO DO
  double likelyhood=-likelyhoodfunction();
  const double n=this->degreesoffreedom();
  likelyhood=likelyhood/n;
  sprintf(s,"Likelihood merrit function (similar to chisq/dof): %e",likelyhood);
  std::string f=s;
  return f;
}


void LevenbergMarquardt::CRLB(){
//get the covariance matrix by inverting the Fischer Matrix
//the diagonal elements now contain the limit of the variance on a parameter
//if the fit would be repeated an infinite amount of times
//on data captured in exactly the same circumstances (only different in noise)
// the distribution around the "real" parameter is gaussian

//reset all sigmas in the model
  modelptr->resetsigmas();
//fill out the sigmas for the free paramaters
  //setup the covariance matrix
  preparecovariance();
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    double variance=fabs(getcovariance(j,j)); //avoid sqrt of neg with abs
    //store this in the sigma value of the parameters
    modelptr->getfreeparam(j)->setsigma(sqrt(variance));
    }
}
double LevenbergMarquardt::getcovariance(int i,int j){
  //before using this, set up the covariance matrix with preparecovariance()
  double result=0.0;
  try{
    //be carefull...
    result=(*information_matrix)(i,j);
  }
  catch(...){
    return 0.0;
  }
  return result;
}

void LevenbergMarquardt::preparecovariance(){
  //prepare covariance matrix by inverting the Fischer Information Matrix

  //this only works with dolintrick off!!!
  bool b=getdolintrick();
  if (b){
    this->dolintrick(false);
    this->calculate_ModifiedJacobian();
  }
 //calculate the Fischer information matrix
 for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    for (size_t k=0;k<modelptr->getnroffreeparameters();k++){
       (*information_matrix)(j,k)=0.0;
        for (unsigned int i=0;i<modelptr->getnpoints();i++){
          //only take non-excluded points
          if (!modelptr->isexcluded(i)){
            (*information_matrix)(j,k)+=Xprime(i,j)*Xprime(i,k);
          }
        }
    }
  }
  #ifdef FITTER_DEBUG
  std::cout <<"The Fischer information matrix:\n";
  information_matrix->debugdisplay();
  #endif
  //invert the Fischer matrix to get the lower bound for the variance
  information_matrix->inv_inplace();

  #ifdef FITTER_DEBUG
  std::cout <<"The inverted Fischer information matrix:\n";
  information_matrix->debugdisplay();
  #endif

  if (b){
    this->dolintrick(true);
     this->calculate_ModifiedJacobian();
  }
}

double LevenbergMarquardt::likelihoodratio(){
  //calculate the likelyhood ratio (LR)
  //this number should be compared to the chi square distribution with
  //n-k degrees of freedom (n=number of points to be fitted, k number of parameters)
  double LR=-likelyhoodfunction(); //formula's are the same
  #ifdef FITTER_DEBUG
  std::cout <<"likelyhood ratio is: "<<LR<<"\n";
  std::cout <<"the degrees of freedom: "<<this->degreesoffreedom()<<"\n";
  #endif
  return LR;
}
void LevenbergMarquardt::updatemonitors(){
  //fill in the CRLB values for each free parameter in the sigma porperty of the param
  this->CRLB();
  //CRLB already uses the covariance matrix so we don't need to recalculate it
  //update the covariances for each parameter that has a monitor
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    if (modelptr->getfreeparam(j)->ismonitored()){
      Monitor* mymonitor=(modelptr->getfreeparam(j))->getmonitor();
      //find the index of the 2n paramters of the monitor
      Parameter* mypar2=mymonitor->getpar2();
      bool found=false;
      size_t i=0;
      for (i=0;i<modelptr->getnroffreeparameters();i++){
          if (mypar2==modelptr->getfreeparam(i)){
             found =true;
             break;
             }
        }
      if (found){
        mymonitor->setcovariance(this->getcovariance(j,i));
      }
      else{
        mymonitor->setcovariance(0.0); //parameter 2 is not free so covariance is 0.0
      }
    }
  }

}



void LevenbergMarquardt::prepareforiteration(){
    //prepare fitting parameters for itteration
    lambdaiter=20; //max 10 itterations to optimise alpha
    lambdac=0.01; //alpha starting step
    nu=10.0; //factor with which to reduce lambda during itterations
    //copy the experiment in Y
    Y.resize(modelptr->getnpoints(),1);
    for (size_t i=0;i<modelptr->getnpoints();i++){
      Y(i,0)=(modelptr->getHLptr())->getcounts(i);
    }
    //make sure starting conditions have valid linear parameters calculated from the nonlinear
    //this is important because the partial derivative expects this to be the case
    //for the nezt iterations we can be sure because iteraction always leave with lin paramters calculated from nonlin
    if (getdolintrick()){
        //if no nonlinear parameters, just skip this calcstep
        //make sure starting parameters obey relation of linear to nonlinear if this option is set
        lin_from_nonlin();
        modelptr->calculate();
    }

    //if all parameters are linear, we can take bigger steps to start
    //and we should get a solution in 1 itteration only
    if (modelptr->islinear()){
        nmax=1; //only 1 iteration needed
    }

}

double LevenbergMarquardt::iteration(){
  //overloaded function of Fitter::itteration
  //do one itteration
  //lock the model so user can not add or remove components during itteration
   #ifdef FITTER_DEBUG
        std::cout << "at start of iteration lambdac="<<lambdac<<" \n";
    #endif
  modelptr->setlocked(true);
  const double lold=this->likelyhoodfunction();

  method_enum method=inversion;
  if (modelptr->islinear())
  {
      method=linear; //choose method to calculate the LM step
      this->dolintrick(false);
  }
  //if the model has changed (a new or removed component eg.) do an update
  if (modelptr->has_changed()){
    createmodelinfo();
  }

  //compute the general LM, maximise the likelihood, eq. 6.154
  double lambda=lambdac/nu;




  preparestep(method); //call this whenever X changed
  double l=calcstep(lambda,method); //and this whenever only lambda changed
  //check if this step was any good

  if ((l<lold)&&(!(modelptr->islinear()))){
        //step3: is worse than what it was, keep lambdac and recalc step
        lambda=lambdac;
        restorecurrentparams(); //get back to original params
        #ifdef FITTER_DEBUG
        std::cout << "levenbergmarquardt: step3\n";
        #endif
        l=calcstep(lambda,method); //and this whenever only lambda changed
        if (l<lold){
            //if still not good, step 4
            for (size_t i=0;i<lambdaiter;i++){

                #ifdef FITTER_DEBUG
                std::cout << "levenbergmarquardt: step4\n";
                #endif
                lambda=lambda*nu;
                restorecurrentparams(); //get back to original params
                l=calcstep(lambda,method); //and this whenever only lambda changed
                if (l>lold){
                    //success
                    #ifdef FITTER_DEBUG
                    std::cout <<"levenbergmarquardt: succesfully leaving step4\n";
                    #endif
                    break;
                }
            }
        }
    }
    if (l>lold){
        lambdac=lambda; //keep lambda, step was good
        }
    else{
        //no valid step found
        #ifdef FITTER_DEBUG
        std::cout << "levenbergmarquardt: no valid step found that reduces the chisq. function\n";
        #endif
        restorecurrentparams();
        modelptr->calculate();
    }
 //unlock the model so user can add or remove components
  modelptr->setlocked(false);

  //put the goodness in the goodnessqueue to check for convergence
  const double newgoodness=this->goodness_of_fit();
  addgoodness(newgoodness);

 #ifdef FITTER_DEBUG
std::cout << "results for itteration:\n";
std::cout << "lamdac=" << lambdac <<"\n";
std::cout << "parameters=";
modelptr->printparameters();
std::cout << "goodness of fit="<<newgoodness<<"\n";
#endif
  return newgoodness;
}
void LevenbergMarquardt::calculate_dtprime(){
//create dtprime
#ifdef FITTER_DEBUG_DETAIL
std::cout << "in calculate dtprime gn="<< "\t";
#endif
    for (size_t i=0;i<modelptr->getnpoints();i++){
        const double gn=fabs(modelptr->getcounts(i)); //model
        const double wn=fabs((modelptr->getHLptr())->getcounts(i)); //experiment
        dtprime(i,0)=(wn-gn)/sqrt(gn+eps);
        #ifdef FITTER_DEBUG_DETAIL
std::cout << gn<< "\t";
#endif
    }

      #ifdef FITTER_DEBUG_DETAIL
std::cout << "\n";
#endif

}

void LevenbergMarquardt::calculate_Y(){
//create Y=y/sqrt(gn), but store in Y because the nonlinear fitter needs to keep the dtprime
//    modelptr->calculate();
//    for (size_t i=0;i<modelptr->getnpoints();i++){
//        const double gn=fabs(modelptr->getcounts(i)); //model
//        const double wn=fabs((modelptr->getHLptr())->getcounts(i)); //experiment
//        Y(i,0)=wn/sqrt(gn+eps);
//    }
}
void LevenbergMarquardt::preparestep(method_enum method){
//prepare all that is needed to do a Levenberg Marquardt step
//without doing the step

if (getdolintrick()){
    //if no nonlinear parameters, just skip this calcstep
    //make sure starting parameters obey relation of linear to nonlinear if this option is set
    //lin_from_nonlin(); //this is done in prepare for iteration
    //modelptr->calculate();
    if (modelptr->islinear()) return ;
}else{
    //if no parameters, don't do anything
    if (modelptr->getnroffreeparameters()==0) return;
}
storecurrentparams();
//create Xprime
calculate_ModifiedJacobian();
calculate_dtprime();

#ifdef FITTER_DEBUG_DETAIL
std::cout << "dtprime=";
dtprime.debugdisplay();
std::cout <<"\n";
#endif
switch (method){
    case QR:
    //prepare QR decomp, then this speeds up the calculation of steps with
    //different lambda
    //take Cinv (this is diagonal) inside X to make the equation  inv(XT
    //Cinv X+lambda I)XT Cinv dt into inv(Xprime' Xprime +lambda I) Xprime dtprime
    //with Xprime=X*sqrt(Cinv) and dtprime=dt.*diag(Cinv);


    //Get corrected Jacobian Xprime=sqrt(Cinv)*X;

    //do a QR decomposition on it and store the results to use in calcstep
    Xprime.QRdecomp(Q,R);
    break;

    case linear:
    //transpose Xprime
    XprimeT.transpose(Xprime);
    XTX.multiply(XprimeT,Xprime);
    calcscaling();
    break;

    case inversion:
    default:
    //the straightforward way with inversion,

        //transpose Xprime
    XprimeT.transpose(Xprime);
    #ifdef FITTER_DEBUG_DETAIL
    std::cout << "Xprime=";
    Xprime.debugdisplay();
    std::cout<<"\n";

    std::cout << "XprimeT=";
    XprimeT.debugdisplay();
    std::cout<<"\n";
    #endif
    //multiply and store in Work
    XTX.multiply(XprimeT,Xprime);
    #ifdef FITTER_DEBUG_DETAIL
    std::cout << "XTX=";
    XTX.debugdisplay();
    std::cout<<"\n";
    #endif
    //calculate scaling
    calcscaling();

    break;
    }
}
double LevenbergMarquardt::calcstep(double lambda,method_enum method){
//take the step, assumes that preparestep was called before for this given set of starting parameters
//calculate the model at this step
//return the likelihood of the current step

	
// standard LM step=(inv(XT*X+lambda*eye(n))*XT*Cinv*dt)'; %eq 6.154
	

//solve (XT*X+lambda I)step=XT*Cinv dt
//there should be more stable and faster ways of doing this
//	with e.g QRsolve or LUsolve or SVDsolve

//QR decomposition, more numerically stable but slower
if (getdolintrick()){
    //if no nonlinear parameters, just skip this calcstep
    if (modelptr->getnroffreenonlinparameters()==0) return -(std::numeric_limits<double>::max)();
}else{
    //if no paramters, don't do anything
    if (modelptr->getnroffreeparameters()==0) return -(std::numeric_limits<double>::max)();
}


switch (method){
    case QR:
    /*
    //D=eye(n); //no scaling at the moment
    //Dlambda=sqrt(lambda)*Pi'*D*Pi; //3.4 in Mor\'e
    //no permutation at the moment e
    m1=[R; Dlambda]; //RHS of 3.4
    [W,Rlambda]=qr(m1,0); // 3.5
    Rinv=inv(Rlambda);
    dtprime=dt.*sqrt(diag(Cinv)); //take part of Cinv into dt!
    u=W(1:n,1:n)*Q'*dtprime;
    q=-Pi*Rinv*u; // a faster way to do it?
    qlambda=D*q;
    */
    break;

    case linear:
    //can we speed this up further?
    XTXcopy=XTX;
    XTXcopy.inv_inplace();
    Work.multiply(XTXcopy,XprimeT);
    Step.multiply(Work,dtprime);
    //add Step to freeparameters vector
    for (size_t i=0;i<XTX.dim1();i++){
        Parameter* p=0;
        double currval=0.0;
        p=modelptr->getfreeparam(i);
        currval=x0[i];
        updateparam(p,currval+Step(i,0)); //update only the nonlinear parameters
    }
    break;

    case inversion:
    default:
    //the straightforward way with inversion,
    //step=(inv(XT*X+lambda*eye(n))*XT*Cinv*dt)'; %eq 6.154



    XTXcopy=XTX;
    //add lambda to diagonal

    for (size_t i=0;i<XTXcopy.dim1();i++){
        XTXcopy(i,i)+=lambda*d0[i]; //apply scaling
    }
#ifdef FITTER_DEBUG_DETAIL
std::cout << "lambda="<<lambda;
std::cout << "XTX+lambda I=";
XTXcopy.debugdisplay();
std::cout<<"\n";
#endif

    //invert
    XTXcopy.inv_inplace(); 
#ifdef FITTER_DEBUG_DETAIL
std::cout << "inv(XTX+lambda I)=";
XTXcopy.debugdisplay();
std::cout<<"\n";
#endif
    //multiply with XT
    Work.multiply(XTXcopy,XprimeT);
 #ifdef FITTER_DEBUG_DETAIL
std::cout << "inv(XTX)XT=";
Work.debugdisplay();
std::cout<<"\n";
#endif


    //multiply with dtprime=dt/sqrt(gn)
    Step.multiply(Work,dtprime);

    //add Step to freeparameters vector
    for (size_t i=0;i<XTX.dim1();i++){
        Parameter* p=0;
        double currval=0.0;
        if (getdolintrick()){
            p=modelptr->getfreenonlinparam(i);
            currval=x0[modelptr->getfreeindexfromnonlinindex(i)];
        }
        else{
            p=modelptr->getfreeparam(i);
            currval=x0[i];
        }


        updateparam(p,currval+Step(i,0)); //update only the nonlinear parameters


    }

}
#ifdef FITTER_DEBUG
std::cout << "Calculated step before nonlin\n";
modelptr->printparameters();
#endif

//and now apply lin_from_nonlin
if (getdolintrick()){
    lin_from_nonlin();
}
modelptr->calculate();

#ifdef FITTER_DEBUG
std::cout << "Calculated step AFTER nonlin\n";
modelptr->printparameters();
std::cout << "the likelihood is:"<<this->likelyhoodfunction()<<"\n";
#endif

return this->likelyhoodfunction();
}

void LevenbergMarquardt::lin_from_nonlin(){
    //calculate linear parameters from nonlinear parameters

    //calculate_Y(); //in the new nonlinear, but with the old linear

    *tempspectrumptr=*modelptr; //copy the current model in tem

    //calculate Ga=matrix with derivatives to the linear parameters
    for (size_t i=0;i<modelptr->getnroffreelinparameters();i++){
        //set all other linear parameters to 0
        for (size_t j=0;j<modelptr->getnroffreelinparameters();j++){
             modelptr->getfreelinparam(j)->setvalue(0.0);
         }
        //and this one to 1
        modelptr->getfreelinparam(i)->setvalue(1.0);
        modelptr->calculate();
        //copy it in Ga
         for (size_t j=0;j<modelptr->getnpoints();j++){
             Ga(j,i)=modelptr->getcounts(j);

             //for Poisson we might use this, but problem is that then which gn to use?
             //the idea was that the linear parameters only depend on the nonlinear
             //but if we introduce gn, then the lin also depend on the whole model
             //const double gn=tempspectrumptr->getcounts(j);
             //Ga(j,i)=modelptr->getcounts(j)/sqrt(gn+eps);
         }
    }

#ifdef FITTER_DEBUG_DETAIL
std::cout << "Ga=";
Ga.debugdisplay();
std::cout<<"\n";
#endif

    //create GaT a transposed version
    GaT.transpose(Ga);
 #ifdef FITTER_DEBUG_DETAIL
std::cout << "GaT=";
GaT.debugdisplay();
std::cout<<"\n";
#endif
    //create Y contains the experimental values
    GaTGa.multiply(GaT,Ga);
 #ifdef FITTER_DEBUG_DETAIL
std::cout << "GaTGa=";
GaTGa.debugdisplay();
std::cout<<"\n";
#endif

    GaTGa.inv_inplace();

#ifdef FITTER_DEBUG_DETAIL
std::cout << "inv(GaTGa)=";
GaTGa.debugdisplay();
std::cout<<"\n";
#endif

    Work2.multiply(GaTGa,GaT);
#ifdef FITTER_DEBUG_DETAIL
std::cout << "inv(GaTGa)GaT=";
Work2.debugdisplay();
std::cout<<"\n";
#endif

    //B.multiply(Work2,Y);

    B.multiply(Work2,Y); //Y is now similar to dtprime, but keep dtprime only for the nonlin fitter
#ifdef FITTER_DEBUG_LIN
std::cout << "inv(GaTGa)GaT Y=";
B.debugdisplay();
std::cout<<"\n";
#endif

    //and copy these back into the linear parameters of the model
    //for (size_t i=0;i<modelptr->getnroffreelinparameters();i++){
     //   modelptr->getfreelinparam(i)->setvalue(B(i,0));
    //}

    //apply the step
    for (size_t i=0;i<GaTGa.dim1();i++){
        Parameter* p=modelptr->getfreelinparam(i);
        updateparam(p,B(i,0)); //update only the nonlinear parameters
        #ifdef FITTER_DEBUG_LIN
        std::cout << "Bi="<<B(i,0)<<"\n";
        #endif
    }
}

void LevenbergMarquardt::calculate_ModifiedJacobian(){
  //calculate derivative matrix
  //each row is a partial derivative of the model spectrum points to a free parameter

  //modelptr->calculate();//assume model is already calculated
  *tempspectrumptr=*modelptr; //copy the spectrum
#ifdef FITTER_DEBUG_DETAIL
std::cout << "Xprime.dim1="<<Xprime.dim1()<<" dim2="<<Xprime.dim2()<<"\n";
#endif
  for (size_t i=0;i<Xprime.dim2();i++){
      modified_partial_derivative(i,tempspectrumptr);
      }
  //after calling this function you can be sure that the model is calculated in the current
  //parameters: this saves time in the optimisation
}

void LevenbergMarquardt::modified_partial_derivative(size_t j,const Spectrum* currentspectrum){
// create partial derivative for the model spectrum wrt a parameter Pj
// and divide this by the sqrt of the model value at each point (therefore we called it modified)

// use either the analytical expression in the component or
// a numerical derivative
// depends on usegradients and on the fact that the component has a getgradient

//convert j to index in freeparameter if doing the dolintrick
size_t jid;
if (getdolintrick()){
    jid=modelptr->getfreeindexfromnonlinindex(j);
    usegradients=false; //in case of the lintric, the gradient are not only dg/dalfa but dg/dbeta*dbeta/dalfa
}
else{
    jid=j;
    }

  if ((usegradients)&&((modelptr->getcomponentforfreeparam(jid))->get_has_gradient(modelptr->getfreeparamindex(jid)))) {
    Spectrum* gradient=modelptr->getgradientptr(modelptr->getcomponentfreeparamindex(jid),modelptr->getfreeparamindex(jid));
    if (gradient==0){
        usegradients=false;
        return;
    }
    //copy it in the Xprime matrix
    for (unsigned int i=0;i<modelptr->getnpoints();i++){
        double gn=currentspectrum->getcounts(i); //make sure model data is really positive
        if (gn<0.0){
            gn=0.0; //don't use points where the model would go negative, is not physical for Poisson model
            }
        if (modelptr->isexcluded(i)){
        	Xprime(i,j)=0.0; //exluded points have no gradient	
        }
        else{
        	Xprime(i,j)=(gradient->getcounts(i))/sqrt(gn+eps);
        }
    }
    return;
  }


      //numerical partial derivative to a parameter p store in row j of derivative matrix
      // deriv=(f[i](p+delta)-currentspectrum[i])/delta
      Parameter* p=0;
      p=modelptr->getfreeparam(jid);
      //a real numerical derivative
      const double originalvalue=p->getvalue(); //make sure originalvalue can not be changed
      double delt=fabs(originalvalue*fraction);
      if (delt<minstep) delt=minstep;
      if (delt>maxstep) delt=maxstep;
      double delta=delt;//make sure delta can not be changed

#ifdef FITTER_DEBUG_DETAIL
      std::cout << "model in X0=\n";
      modelptr->printparameters();
#endif
      //add delta
      p->setvalue(originalvalue+delta);
      if (getdolintrick()){
          lin_from_nonlin();
          //delta should now also contain the distance including the change in linear parameters
          //we can assume that current parameters are present in the storage because we called that before calling modified_derivative
          delta=0.0;
          for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
              delta+=pow(x0[j]-modelptr->getfreeparam(j)->getvalue(),2.0); //sum of distances squared
          }
          delta=sqrt(delta);
      }
#ifdef FITTER_DEBUG_DETAIL
      std::cout << "model in X0+deltaX=\n";
      modelptr->printparameters();
      std::cout << "delta=" << delta <<"\n";
#endif

      modelptr->calculate();
      for (size_t i=0;i<modelptr->getnpoints();i++){
          Xprime(i,j)=modelptr->getcounts(i);
      }


      for (unsigned int i=0;i<modelptr->getnpoints();i++){
          double gn=currentspectrum->getcounts(i); //make sure model data is really positive
          if (gn<0.0){
              gn=0.0; //don't use points where the model would go negative, is not physical for Poisson model
          }

          Xprime(i,j)-=gn;
          Xprime(i,j)/=(delta*(sqrt(gn)+eps)); //store the MODIFIED derivative (divide by sqrt(gn): the model)
          if (modelptr->isexcluded(i)){
              Xprime(i,j)=0.0; //exluded points have no gradient
          }
      }

      //reset to original, BIG advantage, the model is already calculated now, this saves time
      if (getdolintrick()){
          //restore all params to currentparams
          restorecurrentparams();
      }
      else{
          p->setvalue(originalvalue);
      }
      *modelptr=*currentspectrum; //copy old model, no need to calculate again
      //modelptr->calculate();
}

void LevenbergMarquardt::storecurrentparams(){
    //store current free params
    x0.resize(modelptr->getnroffreeparameters()); //make sure it fits
    x0nonlin.resize(modelptr->getnroffreenonlinparameters()); //make sure it fits
    for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
        x0[j]=modelptr->getfreeparam(j)->getvalue();
    }
    for (size_t j=0;j<modelptr->getnroffreenonlinparameters();j++){
        x0nonlin[j]=modelptr->getfreenonlinparam(j)->getvalue();
    }

}


void LevenbergMarquardt::restorecurrentparams(){
    //get back old params
    for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
        modelptr->getfreeparam(j)->setvalue(x0[j]);
    }
}

void LevenbergMarquardt::calcscaling(){
    d0.resize(Xprime.dim2()); //make sure it fits
    for (size_t i=0;i<Xprime.dim2();i++){
        d0[i]=0.0;
        for (size_t j=0;j<modelptr->getnpoints();j++){
        d0[i]+=pow(Xprime(j,i),2.0); //sum of squares
        }
        if (d0[i]<eps){
            d0[i]=1.0; //avoid too small scaling
        }

        //trick set lambda=0 for linear params, is this similar to dolintrick?
//         if (!getdolintrick()){
//        if (modelptr->getfreeparam(i)->islinear()){
//            d0[i]=0.0; //make lambda 0 for linear params
//            }
//         }
    }



    #ifdef FITTER_DEBUG_DETAIL
std::cout << "Scaling d0=";
for (size_t i=0;i<d0.size();i++){
    std::cout << d0[i] <<" , ";
}
std::cout<<"\n";
#endif

}

void LevenbergMarquardt::createmodelinfo()
//create all links to parameters of a model, do this whenever the model has changed
{
  Fitter::createmodelinfo();
  //init storage for matrices
  if (getdolintrick()){
    Xprime.resize(modelptr->getnpoints(),modelptr->getnroffreenonlinparameters());
  }
  else{
    Xprime.resize(modelptr->getnpoints(),modelptr->getnroffreeparameters());
  }

  dtprime.resize(modelptr->getnpoints(),1);
  Ga.resize(modelptr->getnpoints(),modelptr->getnroffreelinparameters());

  prepareforiteration();
}
//The Matlab code
/*
%Levenberg Marquardt method for linear exponential noise distribution
%x=[x]=levenbergmarquardt(fun,x0,xdata,ydata,type,sigma,linear,options)
%fun is pointer to a function requiring l=fun(x0,xdata,ydata)
%the function should return a model with parameters x0
%and extra non-variable parameters xdata for the experimental data ydata
%x are the new optimized parameters
%type can be 'Poisson' or 'Normal'
%linear is vector noting with 1 that a parameter is linear, this helps to
%speed up the fitting, if unsure put zeros(size(x0))
function [x,result]=levenbergmarquardt(fun,x0,xdata,ydata,type,sigma,options)
%follow description on p199
%to do: optimise to take into account the remarks in Mor'e s paper, see
%levenberg.m for that

method='QR'; %choose method for calculation of the LM step

[f,linear]=fun(x0,xdata,ydata); %get to know which params are linear from the function
id=find(~linear); %a vector with id's in x0 that contain nonlin params
dolin=sum(linear)>0;
lold=sum(likelihood(fun,x0,xdata,ydata,type));
maxiter=options.MaxIter;
tol=options.TolCon;
lambdaiter=20; %max 10 itterations to optimise alpha
lambdac=0.01; %alpha starting step
nu=10; %factor with which to reduce lambda during itterations
result='';
%assume here that C in eq. 6.154 is also valid for Poisson...check
%this!!!!!!!
if strcmp(type,'Normal')
    C=diag(sigma);
    Cinv=inv(C);
else
    if ~(strcmp(type,'Poisson'))
        result='levenbergmarquardt: unknown distribution type';
        return
    end
end

nsize=3;
converged(0,nsize,tol); %reset convergence tester
x=x0;
for j=1:maxiter,
    %converged?
    if (converged(lold,nsize,tol))
        result='levenbergmarquardt: converged' ;
        break
    end

    %determine correlation matrix depending on noise distribution
    gn=fun(x0,xdata,ydata);
    wn=ydata;
    dt=(wn-gn)';
    if strcmp(type,'Poisson')
        C=diag(gn); %eq 6.37 p170, depends on the parameters, so we have to recalculate it every time
        Cinv=inv(C);
    end

    %only fit the nonlinear: a subset of these
    X=Jacobian(fun,x0,xdata,ydata,type,linear);
    XT=X';



    %compute the general LM, maximise the likelihood, eq. 6.154
    lambda=lambdac/nu;
    [m,n]=size(x0(id));
    [Q,R,Pi]=preparestep(X,XT,Cinv,lambda,n,dt,method); %call this whenever X changed
    step=calcstep(X,XT,Cinv,lambda,n,dt,Q,R,Pi,method); %and this whenever only lambda changed
    x(id)=x0(id)+step; %only update the nonlinear params
    if dolin
        x=lin_from_nonlin(fun,x,xdata,ydata,linear); %and get also the nonlin filled in
    end

    %test, see if step is not too large
    l=sum(likelihood(fun,x,xdata,ydata,type));
    if (l<lold)
        %step3: is worse than what it was, keep lambdac and recalc step
        lambda=lambdac;
        result='levenbergmarquardt: step3';
        step=calcstep(X,XT,Cinv,lambda,n,dt,Q,R,Pi,method); %and this whenever only lambda changed
        x(id)=x0(id)+step;
        if dolin
            x=lin_from_nonlin(fun,x,xdata,ydata,linear); %and get also the nonlin filled in
        end
        l=sum(likelihood(fun,x,xdata,ydata,type));
        if (l<lold)
            %if still not good, step 4
            for i=1:lambdaiter,
                result='levenbergmarquardt: step4';
                lambda=lambdac*nu;
                lambdac=lambda;
                step=calcstep(X,XT,Cinv,lambda,n,dt,Q,R,Pi,method); %and this whenever only lambda changed
                x(id)=x0(id)+step;
                if dolin
                    x=lin_from_nonlin(fun,x,xdata,ydata,linear); %and get also the nonlin filled in
                end
                l=sum(likelihood(fun,x,xdata,ydata,type));
                if (l>lold)
                    %success
                    result='levenbergmarquardt: succesfully leaving step4';
                    break
                end
            end
        end
    end
    if (l>lold)
        lambdac=lambda; %keep lambda, step was good
        x0=x;
        lold=l;
    else
        %no valid step found
        result='levenbergmarquardt: no valid step found that reduces the chisq. function';

        x=x0; %keep the old parameters
        break;
    end
end

function [Q,R,Pi]=preparestep(X,XT,Cinv,lambda,n,dt,method);
id=strmatch(method,strvcat('QR','SVD','Cholesky'));
switch (id)
    case 1
    %prepare QR decomp, then this speeds up the calculation of steps with
    %different lambda
    %take Cinv (this is diagonal) inside X to make the equation  inv(XT
    %Cinv X+lambda I)XT Cinv dt into inv(Xprime' Xprime +lambda I) Xprime dtprime
    %with Xprime=X*sqrt(Cinv) and dtprime=dt.*diag(Cinv);
    Xprime=sqrt(Cinv)*X;
    [Q,R,pi]=qr(Xprime,0);
    Pi=eye(n);
    Pi=Pi(:,pi); %mix up the unit matrix according to permutation %R2=(Q'*J)*Pi %this indeed R
    %Dmin=inv(D);
    %Jtilde=J*Dmin;
    %[U,S,V]=svd(Jtilde,'econ');
    %sigi=diag(S);
    %zi=U'*dt;
    case 2
    %SVD, most numerically stable, round off errors can be truncated by only keeping singular values up to a certain value

    case 3
    %cholesky, fastest but potentially unstable

    otherwise
    %do nothing for straightforward implementation
    Q=0;
    R=0;
    Pi=0;
end
return

function step=calcstep(X,XT,Cinv,lambda,n,dt,Q,R,Pi,method);
id=strmatch(method,strvcat('QR','SVD','Cholesky'));
switch (id)
    case 1
    %QR decomposition, more numerically stable but slower
    D=eye(n); %no scaling at the moment
    %apply the smart step
    Dlambda=sqrt(lambda)*Pi'*D*Pi; %3.4
    m1=[R; Dlambda]; %RHS of 3.4
    [W,Rlambda]=qr(m1,0); % 3.5
    Rinv=inv(Rlambda);
    dtprime=dt.*sqrt(diag(Cinv)); %take part of Cinv into dt!
    u=W(1:n,1:n)*Q'*dtprime;
    q=-Pi*Rinv*u; % a faster way to do it?
    qlambda=D*q;
    step=-qlambda';
    %phi=sqrt(sum((sigi.^2.*zi.^2)./(sigi.^2+alpha).^2))-delta;
    %Rmin=pinv(Rlambda);
    %phiprime=-norm(qlambda)*norm(Rmin'*(Pi'*D'*qlambda/norm(qlambda)))^2;
    case 2
    %SVD, most numerically stable, round off errors can be truncated by only keeping singular values up to a certain value
    case 3
    %cholesky, fastest but potentially unstable

    otherwise
    %straightforward matrix inversion, potentially unstalbe
    step=(inv(XT*Cinv*X+lambda*eye(n))*XT*Cinv*dt)'; %eq 6.154
end
return
*/
