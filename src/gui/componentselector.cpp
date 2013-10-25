/***************************************************************************
                          componentselector.cpp  -  description
Select a component in the model
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

#include "src/gui/componentselector.h"

//#define COMPONENTSELECTOR_DEBUG

#include <sstream>

#include <QInputDialog>
#include <QString>

#include "src/core/eelsmodel.h"
#include "src/core/component.h"
#include "src/core/model.h"

Eelsmodel* geteelsmodelptr();

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
