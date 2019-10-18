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
 * eelsmodel - gui/componentmaintenance.h
 *
 * A graphical way to maintain the components in a model.
 * The user can select amongst all possible components and remove and add to
 * the current model.
 **/

#ifndef COMPONENTMAINTENANCE_H
#define COMPONENTMAINTENANCE_H

#include <map>
#include <vector>

#include <QLabel>
#include <QPixmap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>

#include "src/core/parameter.h"
#include "src/gui/monitorchooser.h"

class QMdiArea;

class Component;
class Model;
class Component;

class Componentmaintenance : public QWidget  {
  //member data
  Q_OBJECT
  QList<QTreeWidgetItem *> allList;            //list with qstandarditems of all possible components
  QList<QTreeWidgetItem *> modelList;          //list with qstandarditems of all components of the model
  QTreeWidget* lv2;                                 //treeview of the modellist
  QTreeWidget* lv1;

  QLabel* complabel;
  const std::vector<Component*>* compvector;      //vector with components in the model
  const std::vector<Component*>* allvector;       //vector with all possible components


  std::vector<size_t> foundlistZ;                  //a vector with atoms found by the autoid wizard
  std::vector<double> foundlistweight;              //a vector with weights indicating the importance of the found atom types by the autoid wizard
  std::vector<bool> overlaplist;                     //a vector that keeps a warning for overlap issues
  std::map<const QTreeWidgetItem*,Parameter*> itemmap;        //map that links item* which represent the parameters to the parameter*
  std::map<const QTreeWidgetItem*,Component*> componentmap;   //map that links item* which represent the components to the Component*
  QPixmap okicon,lockedicon,unlockedicon,coupledicon,addicon,removeicon,infoicon,monitoricon,wizicon; //some icons
  bool couplesequence;                            //bool to determine wether we are in the sequence of creating a coupling
  Parameter* coupleslave;                         //temp storage while in coupling sequence
  QTreeWidgetItem* coupleslaveitem;
  bool monitorsequence;                           //bool to determine wether we are in the sequence of creating a monitor
  Parameter* monitor1;                              //temp storage while in monitor sequence
  QTreeWidgetItem* monitor1item;
  bool shiftButtonPressed;
  bool rightbutton;
public:
//constructor and destructor
Componentmaintenance(QMdiArea *parent, const char *name
                       ,const std::vector<Component*>& componentvector
                       ,const std::vector<Component*>& allcomponentsvector);
~Componentmaintenance();
//private inspectors
private:
bool modelvalid()const;                          //check if the model is still valid
bool itemisparameter(QTreeWidgetItem* item)const;  //check if an item refers to a parameter
bool itemiscomponent(QTreeWidgetItem* item)const;  //check if an item refers to a component
//modifiers
void updateitem(QTreeWidgetItem* item);
void keyPressEvent( QKeyEvent *event );
void keyReleaseEvent( QKeyEvent *event );
void add_finestruct(Component* mycomponent,Model* mymodel,double Eonset,double Ewidth, double resolution);
public slots:
void slot_remove_components();                                        //remove an existing component from the model
void slot_add_components();                                           //add a new component
void slot_update();                                                   //re-init the component list when changes are made
void slot_info();                                                     //display info on a component
void slot_couple();                                                   //initiate a coupling sequence
void slot_monitor();                                                  //initiate a monitor sequence
void quit(){delete this;};                                            //quit this widget
void slot_param_doubleclick(QTreeWidgetItem*,int col);                //double click on a paramter: edit the values
void slot_param_click(QTreeWidgetItem*,int col);                      //finish the coupling sequence
void slot_param_press(QTreeWidgetItem* item,int col);
void slot_param_rightpress(QTreeWidgetItem* item,int col);
void slot_param_rightclick(QTreeWidgetItem*);     //right click on a parameter: change the locked state
void slot_rename(QTreeWidgetItem* item,int col);                      //click long on a component: rename it
void slot_setposition(int x, int y,int w, int h); //changes position of window on screen
void slot_update_monitors();
void slot_atomwizard();
signals:
void remove_components(int);        //ask eelsmodel to remove the ith component in the model
void add_components(int);           //ask eelsmodel to add a component with index i to the model
void signal_update();               //tell eelsmodel to update the model, since something changed
void componentmaintenance_died();   //tell eelsmodel that this window died


public:
void getposition(int& xpos,int& ypos,int& w,int& h)const; //return the position of the window
};


#endif
