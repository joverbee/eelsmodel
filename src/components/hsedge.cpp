/***************************************************************************
                         hsedge.h  -  description
Hartree Slater cross section component based on tabulated GOS tables by P. Rez
The tables are part of Gatan Digital Micrograph and are NOT GPL
This program only allows you to read the files if you already have them via a license
                             -------------------
    begin                : Sat Oct 26 2002
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

/***************************************************************************

 some comments on implementation - there is no guarantee that these cross
 sections are accurate

The GOS tables are ascii files with a set of GOS values for q,E
Momentum transfer q is given in angstrom^-1
Energy in eV
The scales and size of the table are given by two info lines
The energy and momentum scale are logarithmic
This table is interpolated here for each point where the calculation needs a
GOS
Interpolation is done with a bi-linear interpolator

The calculation is simmilar to e.g. Kedge only the GOS is now obtained from the
tables instead of from an analytical function.

For energies larger than the tabulated energy a power law extrapolation is used.
The extrapolation points are chosen to be close to the two last tabulated values
for energy.

For q-values larger then the tabulated ones, a GOS=0.0 is taken. No warning is produced.

The interpolation aproach is not very efficient. Storage of a one-time interpolated
table might have been more efficient, but whenever Ek changes we have to recalculate
the table because the thetamax limit needs E to convert to q-values.



Tested against C K edge for 300kV and 1,10 and 100mrad collection angle
main difference with DM calculated edge is in interpolation, probably they
use bi-cubic interpolation

****************************************************************************
*/

#include "src/components/hsedge.h"
#include "src/core/parameter.h"
#include "src/gui/getfilename.h"
#include <cmath>

#include <iostream>
#include <string>
#include <fstream>
#include "src/core/curvematrix.h"
#include "src/gui/imagedisplay.h"
#include "src/gui/fileopener.h"




HSedge::HSedge() //create a dummy version
:Component(),tempspectrum(){
  GOSmatrix=0;
  this->setname("Hartree Slater cross section");
  this->setdescription("Hartree Slater cross section from tabulated GOS tables by P. Rez");

}

HSedge::HSedge(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion){
  GOSmatrix=0;
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Parameter* p5;
  Parameter* p6;
  Parameter* p7;

  if (parameterlistptr==0){
    p1=new Parameter("E0",300e3,1);
    p1->interactivevalue("enter primary energy [eV]");
    p1->setchangeable(false);  //the fitter should not change this in normal operation

    p2=new Parameter("Ek",estart,1);
    p2->interactivevalue("enter edge energy [eV]");

    //load the HSGOS table, store the filename in dummy parameter
    Getfilename myfile(0,"Select a HS-GOS file");
    //myfile.setdirname("~");
    myfile.setfilter("GOS tables (*.*)");
    myfile.setcaption("Select a HS-GOS file");
    filename=myfile.open();
    if (filename=="") throw Componenterr::unable_to_create();//cancelled by user
    #ifdef COMPONENT_DEBUG
      std::cout <<"the filename is: "<<filename <<"\n";
    #endif
    p3=new Parameter(filename,0.0,1);
    p3->setchangeable(false);  //the fitter should not change this


    p5=new Parameter("thetamax",2.0e-3,1);
    p5->interactivevalue("enter collection half angle theta [rad]");
    p5->setchangeable(false); //the fitter should not change this in normal operation

    p7=new Parameter("convergence angle",0.0e-3,1);
    p7->interactivevalue("enter convergence half angle [rad]");
    p7->setchangeable(false); //the fitter should not change this in normal operation

    p4=new Parameter("thetasteps",100.0,1);
    p4->interactivevalue("enter number of steps for theta integration");
    p4->setchangeable(false); //the fitter should not change this in normal operation

    p6=new Parameter("strength",1.0e2,1);
    p6->interactivevalue("enter strength of edge");
  }
  else{
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
    p3=(*parameterlistptr)[2];
    p4=(*parameterlistptr)[3];
    p5=(*parameterlistptr)[4];
    p6=(*parameterlistptr)[5];
    #ifdef COMPONENT_DEBUG
    std::cout <<"creating a HS with paramterpointer "<<parameterlistptr<<" \n";
    std::cout <<"p1 "<<p1->getname()<<" \n";
    std::cout <<"p2 "<<p2->getname()<<" \n";
    std::cout <<"p3 "<<p3->getname()<<" \n";
    std::cout <<"p4 "<<p4->getname()<<" \n";
    std::cout <<"p5 "<<p5->getname()<<" \n";
    std::cout <<"p6 "<<p6->getname()<<" \n";
    #endif


      //try to see if we also find an convergence angle
    //to be compatible with older version
    if (parameterlistptr->size()==7){
        p7=(*parameterlistptr)[6];
    }
    else{
        //create a new convergence angle parameters and put it to 0.0
        p7=new Parameter("convergence angle",0.0e-3,1);
        //p7.interactivevalue("enter convergence half angle [rad]");
        p7->setchangeable(false); //the fitter should not change this in normal operation
    }



    filename=p3->getname();
    //first check wether this file can be found
    Fileopener f(0,0,filename);
    filename=f.open();
    //filename is now an updated name
    if (filename=="") throw Componenterr::unable_to_create();//cancelled by user    p3.setname(filename); //store this new name in the parameter
  }

   p6->setlinear(true); //strength is a linear parameter

  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);
  this->addparameter(p5);
  this->addparameter(p6);
  this->addparameter(p7);


  //give a name and description
  this->setname("Hartree Slater cross section");
  this->setdescription("Hartree Slater cross section from tabulated GOS tables by P. Rez");
  this->setcanconvolute(true);
  setshifter(false);
  //load the GOS file
  this->readGOSfile(filename);
  //we have an analytical gradient wrt strength parameter
  this->sethasgradient(5,true);
}


HSedge::~HSedge(){
  //clean up the GOS table
  if (GOSmatrix!=0)    delete(GOSmatrix);
}

void HSedge::calculate(){
  //get the parameters
  const Parameter* E0ptr= getparameter(0);
  const double E0=E0ptr->getvalue();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  const Parameter* thetastepptr= getparameter(3);
  const double thetasteps=thetastepptr->getvalue();
  const Parameter* thetamaxptr= getparameter(4);
  const double thetamax=thetamaxptr->getvalue();
  const Parameter* strengthptr= getparameter(5);
  const double strength=strengthptr->getvalue();
  const Parameter* alphaptr= getparameter(6);
  const double alpha=alphaptr->getvalue();

  //if parameters have changed, calculate again
  if (E0ptr->changed()||Ekptr->changed()||thetastepptr->changed()||thetamaxptr->changed()||alphaptr->changed()){
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating HS edge \n E0: " << E0 << " Ek:" <<Ek
            << " thetasteps: "<<thetasteps<<" thetamax: "<<thetamax<<" strength: "<<strength<<"\n";
    #endif
    bool newAR=true;
    double A=0;
    double r=0;
    const double dispersion=this->getdispersion();
    //const double eps=1.0e-99;
    //definition of fundamental constants
    const double R=13.606; //Rydberg of energy in eV
    const double e=1.602e-19; //electron charge in C
    const double m0=9.110e-31; //electron rest mass in kg
    const double a0=5.292e-11; //Bohr radius in m
    const double c=2.998e8; //speed of light in m/s
    const double Pi=acos(-1.0);
    //calculate relativistic correction factors
    const double gamma=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
    const double T=E0*(1.0+gamma)/(2.0*pow(gamma,2.0)); //in eV

    double cts=0.0;
    const double Emax=getgosenergy(nrow-1)+Ek; //maximum tabulated energy
    for (unsigned int i=0;i<(this->getnpoints());i++){//energy loop
      const double E=this->getenergy(i);
      double integral=0.0;

      if ((E>=Ek)&&(E>0.0)&&(E<Emax)&&(Ek>1.0)){

        const double qa0sqmin=pow(E,2.0)/(4.0*R*T)+pow(E,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
        const double qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin((thetamax+alpha)/2.0),2.0);
        const double lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/floor(thetasteps);
        // do integration on a logarithmic grid lnqa0sq
        for (double lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq+=lnqa0sqstep){//collection angle loop
          const double qa0sq=exp(lnqa0sq);
          const double q=sqrt(qa0sq)/a0; //in m^-1
          //get GOS via interpolation from table
          double dfkdE=getinterpolatedgos(E-Ek,q);
          //integration over theta
          const double theta=2.0*sqrt(fabs(R*(qa0sq-qa0sqmin)/(4.0*pow(gamma,2.0)*T))); //fabs to avoid small neg number to cause probs with sqrt
          integral+=getcorrfactorKohl(alpha,thetamax,theta)*dfkdE*lnqa0sqstep;
          }//end of angle loop
        cts= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E*T))*integral*dispersion; /* in m**2/eV */
        }//end of if E>=Ek
      else{
        if (E<=Ek){
          //take 00. before edge onset
          cts=0.0;
        }
        else{
          //do an extrapolation with a power law function for E>Emax, but only do this once (when newAR=true)
          if (newAR){
            //first time, we need to calculate A and r
            #ifdef COMPONENT_DEBUG
            std::cout <<"Warning: HSGOS table is limitted at "<<Emax<<" power law extrapolation is used beyond this\n";
            #endif
            //estimate A,r for power law
            //take energies close at the two last tabulated energies

            const double E1=getgosenergy(nrow-3)+Ek;
            const double E2=getgosenergy(nrow-2)+Ek;
            try{
	    	//calculate integrated intensity in point E1
	    	//for E1
	    	double qa0sqmin=pow(E1,2.0)/(4.0*R*T)+pow(E1,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
            	double qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin((thetamax+alpha)/2.0),2.0);
            	double lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/floor(thetasteps);
	    	integral=0;
 	    	for (double lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq+=lnqa0sqstep){//collection angle loop
          		const double qa0sq=exp(lnqa0sq);
          		const double q=sqrt(qa0sq)/a0; //in m^-1
          		//get GOS via interpolation from table
          		double dfkdE=getinterpolatedgos(E1-Ek,q);
          		//integration over theta
                const double theta=2.0*sqrt(fabs(R*(qa0sq-qa0sqmin)/(4.0*pow(gamma,2.0)*T))); //fabs to avoid small neg number to cause probs with sqrt
          		integral+=getcorrfactorKohl(alpha,thetamax,theta)*dfkdE*lnqa0sqstep;
            	}//end of angle loop
            	const double y1= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E1*T))*integral*dispersion; /* in m**2/bin */

		//and for E2
		qa0sqmin=pow(E2,2.0)/(4.0*R*T)+pow(E2,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
       	    	qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin((thetamax+alpha)/2.0),2.0);
            	lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/floor(thetasteps);
	    	integral=0;
 	    	for (double lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq+=lnqa0sqstep){//collection angle loop
          		const double qa0sq=exp(lnqa0sq);
          		const double q=sqrt(qa0sq)/a0; //in m^-1
          		//get GOS via interpolation from table
          		double dfkdE=getinterpolatedgos(E2-Ek,q);
          		//integration over theta
                const double theta=2.0*sqrt(fabs(R*(qa0sq-qa0sqmin)/(4.0*pow(gamma,2.0)*T))); //fabs to avoid small neg number to cause probs with sqrt
                //see eq. 3.146 in Egerton
                integral+=getcorrfactorKohl(alpha,thetamax,theta)*dfkdE*lnqa0sqstep;
            	}//end of angle loop
            	const double y2= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E2*T))*integral*dispersion; /* in m**2/bin */

            	//calculate A and r
	  	if ((y1>0.0)&&(y2>0.0)){
              		r=log(y2/y1)/log(E1/E2);
              		A=y1/(pow(E1,-r)); //now calculate for the dispersion in our spectrum
		}
		else{//some trouble occurs if y1=0 for values of Ek unreasonably low
			A=0;
			r=0;
		}
	    }catch(...){
		//in case unrealistic values for Ek are given STRANGE this doesnt catch inf and overflow error
		r=0;
		A=0;
            }
	    //check sanity of r
	    if ((r>100.0)||(r<1.0)){
		r=0;
		A=0;
	    }

            #ifdef COMPONENT_DEBUG
              std::cout <<"Estimated power law: A="<<A<<" and r="<<r<<"\n";
            #endif
            newAR=false;
            cts=A*pow(E,-r);
          }
          else{
            //A and r already exist
            cts=A*pow(E,-r);
          }
        }
     }
     //multiply by 1e28 to give reasonable numbers
     cts=cts*1.0e28;
     tempspectrum.setdatapoint(i,E,cts,0.0);//store the shape in the temp spectrum
      this->setdatapoint(i,E,cts*strength,0.0);//then, multiply by the strength in the component spectrum
    }//end of energy loop

  }//end of if parameters changed

  else{
    if(strengthptr->changed()){
      //if only the strength has changed take the spectrum from the tempspectrum and multiply with strength
      //this is a lot faster and important for the fitting routines
      for (unsigned int i=0;i<(this->getnpoints());i++){
        const double E=this->getenergy(i);
        this->setdatapoint(i,E,tempspectrum.getcounts(i)*strength,0.0);
        }
      #ifdef COMPONENT_DEBUG
      std::cout <<"parameters have not changed, only strength has changed\n";
      #endif
      }
    #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, do nothing\n";
    #endif
    }
    //set parameters as unchanged since last time we calculated
    this->setunchanged();
}


void HSedge::readGOSfile(std::string filename){
  //read the GOS filename
  //if something goes wrong, stop creating this component

  //test existence of filename and get a corrected filename if not found
  Fileopener f(0,0,filename);
  filename=f.open();
  //filename is now an updated name
  if (filename=="") throw Componenterr::unable_to_create();//cancelled by user
  //store this new filename in the paramname
  (getparameter(2))->setname(filename);
  std::string myline;

  int format=filetype(filename); //indicates file format 0=unix, 1=win,2=mac ,3=error
  if (format==3) throw Component::Componenterr::unable_to_create();
  //carefull for read errors
  try{
    //open file for reading
	  std::ifstream GOSfile(filename.c_str(),std::ios_base::binary);
    if (!GOSfile) {
      throw Component::Componenterr::unable_to_create();
    }
    //read name line
    myline=getmultiplatformline(&GOSfile,format);
    #ifdef COMPONENT_DEBUG
      std::cout <<"name line:"<<myline<<"\n";
    #endif

    //read first info line
     myline=getmultiplatformline(&GOSfile,format);
    #ifdef COMPONENT_DEBUG
      std::cout <<"info line 1:"<<myline<<"\n";
    #endif
    //extract the info
    //first line consist of a string and 3 numbers separated by spaces

    //check which type of edge it is
    //and correct to get the TOTAL cross section, eg L32 and not only L3
    double correction=1.0;
    /*
     * disabled since it seems no longer nessecary, don't understand why
     * 
     * std::string::size_type loc = myline.find( "P3/2", 0 );
    if( loc != std::string::npos ) {
        //L3 edge
        correction=4.0/6.0;
    }
    loc = myline.find( "D5/2", 0 );
    if( loc != std::string::npos ) {
        //M5 edge
        correction=6.0/10.0;
    }
*/
    
    
    myline.erase(0,6); //erase the core state info
    const char* mylinec=myline.c_str();
    std::sscanf(mylinec,"%f %f %f %d",&info1_1,&info1_2,&info1_3,&ncol);

    //read second info line
     myline=getmultiplatformline(&GOSfile,format);
    #ifdef COMPONENT_DEBUG
      std::cout <<"info line 2:"<<myline<<"\n";
    #endif
    const char* mylinec2=myline.c_str();
    std::sscanf(mylinec2,"%e %e %d",&info2_1,&info2_2,&nrow);

    #ifdef COMPONENT_DEBUG
        std::cout <<"Info paramters of GOS file\n";
        std::cout <<"info 1_1 " <<info1_1<<"\n";
        std::cout <<"info 1_2 " <<info1_2<<"\n";
        std::cout <<"info 1_3 " <<info1_3<<"\n";
        std::cout <<"info 2_1 " <<info2_1<<"\n";
        std::cout <<"info 2_2 " <<info2_2<<"\n";
        std::cout <<"ncol " <<ncol<<"\n";
        std::cout <<"nrow " <<nrow<<"\n";
    #endif

    GOSmatrix=new CurveMatrix(nrow,ncol);

    //read the data
    for (int i=0;i<nrow;i++){
      for (int j=0;j<ncol;j++){
        float data;
        try{
          GOSfile>>data; //read a single float from the stream
        }
        catch(...){
          throw Component::Componenterr::unable_to_create();
        }
        #ifdef COMPONENT_DEBUG
          std::cout <<data<<"\n";
        #endif
        const double R=13.606; //Rydberg of energy in eV
        (*GOSmatrix)(i,j)=data/(correction*R); //careful data is stored per rydberg and we want per eV to be compatible with sigmak
      }
    }
  }  //end of try
  catch(...){
    throw Component::Componenterr::unable_to_create();
  }

  //test GOS and interpolation by showing it as an image
  #ifdef COMPONENT_DEBUG
  GOSmatrix->debugdisplay();
  //Imagedisplay*  myimage= new Imagedisplay(0,filename.c_str(),GOSmatrix);

  //now test the interpolation routine and plot result
  /*const int qpoints=100;
  const double qmax=30.0*1e10;
  const int epoints=this->getnpoints();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  CurveMatrix* interpmatrix=new CurveMatrix(epoints,qpoints);
  for (int i=0;i<epoints;i++){
    for (int j=0;j<qpoints;j++){
      const double energy=this->getenergy(i);
      const double q=(double(j)/double(qpoints))*qmax;
      (*interpmatrix)[i][j]=getinterpolatedgos(energy-Ek,q); //gos is tabulated with E=0->xx eV not from Ek
    }
  }
  //show result
  Imagedisplay*  myinterpimage= new Imagedisplay(0,filename.c_str(),interpmatrix);
  */
  #endif
}

double HSedge::getinterpolatedgos(double energy,double q)const{
  //interpolate the tabulated GOS with a bi-linear method
  //find index of the four neighbours
  //E in eV, q in m^-1
  int i,j;
  for (i=0;i<nrow;i++){
    const double gosenergy=getgosenergy(i);
    if (gosenergy>energy) break;
  }
  for (j=0;j<ncol;j++){
    const double gosq=getgosq(j);
    if (gosq>q) break;
  }
  //i and j are now indices of upper right corner
  //check to see if we are not on a boundary
  if (i==0){
    //energy too low
    return 0.0;
  }
  if (i==nrow){
    //energy too high
    return 0.0;
  }
  if (j==0){
    //q vector too low
    return (*GOSmatrix)(i,0);
  }
  if (j==ncol){
    //q vector too high
    return 0.0;
  }
  //determine distance between the four points
  const double upperenergy=getgosenergy(i);
  const double lowerenergy=getgosenergy(i-1);
  const double upperq=getgosq(j);
  const double lowerq=getgosq(j-1);

  const double deltaenergy=upperenergy-lowerenergy;
  const double deltaq=upperq-lowerq;
  //determine distance from lower left corner
  const double distE=energy-lowerenergy;
  const double distq=q-lowerq;

  //return bi-linear result
  const double result1=(1.0/(deltaenergy*deltaq))*((deltaenergy-distE)*(deltaq-distq))*(*GOSmatrix)(i-1,j-1);
  const double result2=(1.0/(deltaenergy*deltaq))*((deltaenergy-distE)*(distq))*(*GOSmatrix)(i-1,j);
  const double result3=(1.0/(deltaenergy*deltaq))*(distE*(deltaq-distq))*(*GOSmatrix)(i,j-1);
  const double result4=(1.0/(deltaenergy*deltaq))*(distE*distq)*(*GOSmatrix)(i,j);

  return result1+result2+result3+result4;
}

double HSedge::getgosenergy(int i)const{
  //give energy of tabulated gos point
  //exponential dependence
  return info2_1*(exp(double(i)*info2_2/info2_1)-1.0);
}

double HSedge::getgosq(int j)const{
  //give momentum transfer of tabulated gos point
  //exponential dependence
  //was in angstrom^-1 ->convert to m^-1
  return (info1_1*(exp(double(j)*info1_2)-1.0))*1e10;
}


std::string HSedge::getmultiplatformline(std::ifstream* file,int format){
  //read a line from an open filestream with format 0=unix,1=win,2=mac
  std::string line="";


  //carefull \cr means in bcc32 lf+cr instead of only cr
  //char lf='\n';
  //char cr='\r';
  const char lf=(char)10;
  const char cr=(char)13;

  if (format==0){
  //UNIX format
  std::getline(*file,line,lf);
  }
  else{
    if (format==1){
    //Win format
    std::getline(*file,line,lf);
    //remove cr from end of line
    line.erase(line.find(cr));
    }
    else{
    //Mac format
    std::getline(*file,line,cr);
    }
  }
  return line;
}

int HSedge::filetype(std::string filename)const{
  //determine file type by looking for type of end of line character
  //0=Unix
  //1=WIn
  //2=Mac
  //3=error

  //open file for reading
	std::ifstream dummyfile(filename.c_str(),std::ios_base::binary);
  if (!dummyfile) {
    return 3;
  }
  std::string myline;
  //std::string::size_type mypos;
  //MAC ends with CR
  //UNIX end with LF
  //WIN ends with CR,LF

  //const char eol='\n';
  //const char cr='\r';
  const char eol=(char)10;
  const char cr=(char)13;
	#ifdef COMPONENT_DEBUG
            std::cout <<"eol decimal:"<<(int)eol<<"\n";
			std::cout <<"eol decimal:"<<(int)cr<<"\n";
    #endif


  int type=3;
  std::getline(dummyfile,myline,eol);
  //carefull the eol is removed by getline and is no longer present in myline
  //is this the same behaviour as under UNIX????

  if (myline.find(cr,0)!=std::string::npos) {
        //found CR, either MAC or WIN
        if ((!dummyfile.eof())||(myline.size()>myline.max_size()-1)) {//if less then total file has been read it means there was a lf
          //WIN format
          #ifdef COMPONENT_DEBUG
            std::cout <<"file is in Windows format\n";
          #endif
          type=1;
        }
        else{ //if eof reached it means that no lf chars were present
          //MAC format
          #ifdef COMPONENT_DEBUG
            std::cout <<"file is in Macintosh format\n";
          #endif
          type=2;
        }
  }
  else{
      //UNIX format
      #ifdef COMPONENT_DEBUG
        std::cout <<"file is in Unix format\n";
      #endif
      type=0;
  }
  dummyfile.close();
  return type;
}

Spectrum* HSedge::getgradient(size_t j){
//get analytical partial derivative to strength parameter
//(the more we can avoid calculating these time consuming componets the better)

  switch(j){
  case 5:
  //analytical derivative wrt strength equals tempspectrum
    for (size_t i=0;i<this->getnpoints();i++)
  {
    gradient.setcounts(i,tempspectrum.getcounts(i));
  }
  break;

  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}

HSedge* HSedge::clone()const{
  return new HSedge(*this);
}
HSedge* HSedge::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new HSedge(n,estart,dispersion,parameterlistptr);
}

double HSedge::getcorrfactor(double E,double alpha,double beta,double E0){
    //correction factor due to Scheinfein and Isaacson
    const double alphasq=pow(alpha,2.0);
    const double betasq=pow(beta,2.0);
    const double thetaE=E/(E0*(1022.0e3+E0)/(511.0e3+E0));
    const double thetaEsq=pow(thetaE,2.0);
    double corrfactor=1.0;
    const double eps=1e-6; //1 microrad is smallest reasonable convergence angle
    if ((thetaEsq!=0.0)&&(alpha>eps)){
         corrfactor=( sqrt(pow(alphasq+betasq+thetaEsq,2.0)-4.0*alphasq*betasq)-alphasq-betasq-thetaEsq+2.0*betasq*log((0.5/thetaEsq)*(sqrt(pow(alphasq+thetaEsq-betasq,2.0)+4.0*betasq*thetaEsq)+alphasq+thetaEsq-betasq))+2.0*alphasq*log((0.5/thetaEsq)*(sqrt(pow(betasq+thetaEsq-alphasq,2.0)+4.0*alphasq*thetaEsq)+betasq+thetaEsq-alphasq))    ) * 1.0/(2.0*alphasq*log(1.0+betasq/thetaEsq));
    }
    #ifdef COMPONENT_DEBUG
        std::cout <<"E="<<E<<" thetaE="<<thetaE<< " alpha="<<alpha<<" beta="<<beta<<" corrfactor="<<corrfactor<<"\n";
      #endif

    return corrfactor;
}

double HSedge::getcorrfactorKohl(double alpha,double beta,double theta){
    //described in H. Kohl Ultramicroscopy 19 (1985) 265-268
    //this factor can be inserted in the integration over theta to take into account the effect of convergence angle
    const double alphasq=pow(alpha,2.0);
    const double betasq=pow(beta,2.0);
    const double thetasq=pow(theta,2.0);
    double corrfactor=1.0;
    const double eps=1e-6; //1 microrad is smallest reasonable convergence angle
    const double Pi=acos(-1.0);

    if (alphasq<pow(eps,2.0)){
            //for very small alpha we need no correction factor, this avoids numerical trouble
            corrfactor=1.0;
            return corrfactor;
    }
    if (fabs(alpha-beta)>=theta){
        double thetapsq=alphasq;
        if (alphasq>betasq) {
            thetapsq=betasq;
        }
        corrfactor=thetapsq/alphasq;
        }
    else{
        //for abs(alpha-beta)<theta<alpha+beta (Make sure in the integration loop to go up to maximum alpha+beta
        const double x=(alphasq+thetasq-betasq)/(2.0*alpha*theta);
        const double y=(betasq+thetasq-alphasq)/(2.0*beta*theta);
        corrfactor=(1.0/Pi)*(acos(x)+(betasq/alphasq)*acos(y)-(1.0/(2.0*alphasq))*sqrt(4.0*alphasq*betasq-pow(alphasq+betasq-thetasq,2.0)));
        }
    #ifdef COMPONENT_DEBUG
        std::cout << " alpha="<<alpha<<" beta="<<beta<<" theta="<<theta << " corrfactor="<<corrfactor<<"\n";
    #endif
    return corrfactor;
}
