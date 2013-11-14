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
 * eelsmodel - gui/componentselector.cpp
 **/

#include "src/gui/componentselector.h"

//#define COMPONENTSELECTOR_DEBUG

#include <sstream>

#include <QInputDialog>
#include <QString>

#include "src/core/component.h"
#include "src/core/model.h"

#include "src/gui/eelsmodel.h"

EELSModel* geteelsmodelptr();

Componentselector::Componentselector(QWidget *parent, const char *name,const char* message,int& value)
: QWidget(parent) {
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    bool ok = FALSE;
    //create a selector list with available components in the model
    QStringList items;
    //go through the components of the model and add them to the list
    for (size_t i=0;i<(size_t)mymodel->getcomponentsnr();i++){
        //mae a unique name by addig a number
        std::string itemname="";
        if ((mymodel->getcomponent(i))->getdisplayname()!=""){
            itemname=((mymodel->getcomponent(i))->getdisplayname()).c_str();
        }
        else{
            itemname=((mymodel->getcomponent(i))->getname()).c_str();
        }
        std::ostringstream suffix;
        if ((suffix <<itemname<< " nr:"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
       // conversion worked
       std::string suf=suffix.str();
        items.append(suf.c_str()); //add the suffix
      }
    }

    QString item = QInputDialog::getItem(this,name,message, items, 0, false, &ok);
    if (ok && !item.isEmpty()){
         value=items.indexOf(item);
    }
     else    {
        value=0;
    }

#ifdef COMPONENTSELECTOR_DEBUG
std::cout << "selected value ="<<value<<" item="<<item.toStdString();
#endif

}

Componentselector::~Componentselector(){
}
