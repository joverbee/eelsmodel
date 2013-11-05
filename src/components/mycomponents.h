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
 * eelsmodel - components/mycomponents.h
 *
 * A list of all possible components in eelsmodel.
 * If a user creates a new component, it should be added in the back of this
 * file.
 **/

//include all components here
#include "src/components/powerlaw.h"
#include "src/components/fast_BG.h"
#include "src/components/line.h"
#include "src/components/lorentz.h"
#include "src/components/gaussian.h"
#include "src/components/pseudovoigt.h"
#include "src/components/pearsonIV.h"
#include "src/components/expbackground.h"
#include "src/components/kedge.h"
#include "src/components/kedgerel.h"
#include "src/components/ledge.h"
#include "src/components/logpoly.h"
#include "src/components/mscatter.h"
#include "src/components/edgefile.h"
#include "src/components/fixedpattern.h"
#include "src/components/hsedge.h"
#include "src/components/spinsplitedge.h"
#include "src/components/spinsplitedgehyd.h"
#include "src/components/eshift.h"
#include "src/components/dos.h"
#include "src/components/gdos.h"
#include "src/components/fowler.h"
#include "src/components/mscatterfft.h"
#include "src/components/mscatterexpfft.h"

#include "src/components/doslifetime.h"
#include "src/components/doslifetimespline.h"
#include "src/components/drudelorentz.h"
#include "src/components/dielecmodel.h"

#include "src/components/dieleclinear.h"

//create a small version here and store the pointer in the list
#ifdef MY_COMP_EXEC
Component* p;

//Background functions
p=new Powerlaw();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new fast_BG();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Expbackground();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Logpoly();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

//General functions
p=new Line();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Lorentz();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Gaussian();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new PseudoVoigt();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new PearsonIV();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

//Low loss shapes
p=new Fowler();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list


//Multiple scattering components
p=new Mscatter();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new MscatterExpfft();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Mscatterfft();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

//cross sections
p=new EdgeFile();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Kedgerel();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Kedge();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Ledge();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new HSedge();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new SpinsplitEdge();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new SpinsplitEdgeHyd();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list


//special functions
p=new FixedPattern();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new Eshift();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list




//Fine structure components
p=new Dos();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new DosLifetime();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new DosLifetimeSpline();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new GDos();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

p=new DrudeLorentz();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list

//p=new DielecModel();    //create a dummy version of your component
//ptr_to_all_components.push_back(p); //put it in the list

p=new DielecLinear();    //create a dummy version of your component
ptr_to_all_components.push_back(p); //put it in the list


#endif
