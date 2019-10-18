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
 * eelsmodel components/eshift.cpp
 **/

#include "src/components/eshift.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "src/core/eelsmodel.h"
#include "src/core/multispectrum.h"
#include "src/core/model.h"
#include "src/core/parameter.h"

#include "src/gui/getgraphptr.h"
#include "src/gui/gettopspectrum.h"
#include "src/gui/saysomething.h"

Eelsmodel* geteelsmodelptr();
QMdiArea* getworkspaceptr();

Eshift::Eshift()
:Component()
{
this->setname("Eshift");
this->setdescription("energy shift by linear interpolation of model");
}
Eshift::Eshift(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
//add the required parameters
Parameter* p1;
if (parameterlistptr==0){
  p1=new Parameter("delta E",0.0,1);
  p1->interactivevalue("enter energy shhift");
}
else{
  p1=(*parameterlistptr)[0];
}
this->addparameter(p1);

//give a name and description
this->setname("Eshift");
this->setdescription("energy shift by linear interpolation of model");
this->setcanconvolute(false); //convoluting this has no meaning
this->setconvolutor(false);
this->setshifter(true); //this is a special component which needs to be treated first in the model calculation sequence
//because if an eshift occurs, all components are affected by this and need to be recalculated
//get a const pointer to the HL spectrum (we don't want to change the HL)
mymodel=(geteelsmodelptr())->getmodel_nonconst();
HLptr=0;
HLptr=mymodel->getHLptr(); //pointer to the HL spectrum
if (HLptr==0) throw Componenterr::unable_to_create();

}
Eshift::~Eshift(){
    //reset the Eshift before taking off
    HLptr->seteshift(0);
    mymodel->seteshift(0);
}
void Eshift::calculate(){
	//check if any of parameters is changed, if not : don't calculate
	const Parameter* p1= getparameter(0);
	const double dE=p1->getvalue();


	if (p1->changed()){
		//do the energy shift on the HL spectrum
		HLptr->seteshift(dE);
		//and on the model
		mymodel->seteshift(dE);
		//set parameters as unchanged since last time we calculated
		this->setunchanged();
	}
	else
	{
		#ifdef COMPONENT_DEBUG
		std::cout <<"dE=0, no need to calculate the energy shift\n";
		#endif
	}

}

Eshift* Eshift::clone()const{
return new Eshift(*this);}
Eshift* Eshift::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Eshift(n,estart,dispersion,parameterlistptr);
 }
