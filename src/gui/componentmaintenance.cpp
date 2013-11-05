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
 * eelsmodel - gui/componentmaintenance.cpp
 **/

#include "src/gui/componentmaintenance.h"

//#define DEBUG_COMPONENTMAINTENANCE

#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <string>
#include <sstream>

#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QMessageBox>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QWorkspace>

#include "src/core/component.h"
#include "src/core/eelsmodel.h"
#include "src/core/model.h"
#include "src/core/monitor.h"

#include "src/gui/atomchooser.h"
#include "src/gui/saysomething.h"

//embedded icons
#include "./icons/locked.xpm"
#include "./icons/unlock.xpm"
#include "./icons/linked.xpm"
#include "./icons/add.xpm"
#include "./icons/remove.xpm"
#include "./icons/info.xpm"
#include "./icons/ok.xpm"
#include "./icons/monitor.xpm"
#include "./icons/atomwiz.xpm"

QWorkspace* getworkspaceptr();
Eelsmodel* geteelsmodelptr();

Componentmaintenance::Componentmaintenance(QWorkspace *parent, const char *name
                                           ,const std::vector<Component*>& componentvector
                                           ,const std::vector<Component*>& allcomponentsvector)
: QWidget(parent),allList(),modelList(),itemmap(){
    this->setWindowTitle(name);
   shiftButtonPressed = false;
   rightbutton=false;
  parent->addWindow(this); //add it explicitly to the workspace
  couplesequence=false;
  monitorsequence=false;
  monitor1item=0;
  coupleslaveitem=0;
  //define the pixmaps
  lockedicon=QPixmap(locked_xpm);
  unlockedicon=QPixmap(unlock_xpm);
  coupledicon=QPixmap(linked_xpm);
  addicon=QPixmap(add_xpm);
  removeicon=QPixmap(remove_xpm);
  infoicon=QPixmap(info_xpm);
  okicon=QPixmap(ok_xpm);
  monitoricon=QPixmap(monitor_xpm);
  wizicon=QPixmap(wiz_xpm);
  //get pointers to the vectors containing all components and the model components
  compvector=&componentvector;
  allvector=&allcomponentsvector;

  //set the layout of this widget
  QGridLayout *lay = new QGridLayout( this );
  //lay->setMargin( 5 );
  // create a widget which layouts its childs in a column

  // First child: a Label
  //lay->addWidget( new QLabel( "Select some components to build a model!", this ) ,0,0,1,2);

  // Second child: the ListView
  lv1 = new QTreeWidget(this);
  lv1->setColumnCount(1);

   for (unsigned int i=0;i<allcomponentsvector.size();i++){
       std::string name=((*allvector)[i])->getname();
       allList.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str()).arg(i))));
  }
  lv1->insertTopLevelItems(0, allList);
  lv1->setHeaderLabel ( "Available Components" );



  lay->addWidget( lv1 ,0,0,8,2);
  lv1->setSortingEnabled(false); //disable sorting, important for getting the indices right
  lv1->setRootIsDecorated( TRUE );


  // create a pushbutton for adding components
  QPushButton* add= new QPushButton( addicon,"Add", this );
  lay->addWidget( add ,8,0);
  connect( add, SIGNAL( clicked() ), this, SLOT( slot_add_components() ) );

  // create a pushbutton for removing components
  QPushButton *remove = new QPushButton(removeicon, "Remove", this  );
  lay->addWidget( remove ,8,1);
  connect( remove, SIGNAL( clicked() ), this, SLOT( slot_remove_components() ) );

  // create a pushbutton for getting info
  QPushButton *info = new QPushButton( infoicon,"Info", this  );
  lay->addWidget( info ,8,2);
  connect( info, SIGNAL( clicked()), this, SLOT( slot_info() ) );

  // create a pushbutton for coupling
  QPushButton *couple = new QPushButton(coupledicon,"Couple parameters", this  );
  lay->addWidget( couple ,8,3);
  connect( couple, SIGNAL( clicked()), this, SLOT( slot_couple() ) );

  // create a pushbutton for adding a monitor
  QPushButton *monitor = new QPushButton(monitoricon,"add Monitor", this  );
  lay->addWidget( monitor ,8,4);
  connect( monitor, SIGNAL( clicked()), this, SLOT( slot_monitor() ) );

  // create a pushbutton for exitting this widget
  //QPushButton *ok = new QPushButton(okicon, "OK", this  );
  //lay->addWidget( ok ,8,5);
  //connect( ok, SIGNAL( clicked() ), this, SLOT( quit() ) );



//a wizard to make cross sections
  QPushButton *atombutton = new QPushButton(wizicon, "Add Xsections", this  );
  lay->addWidget( atombutton ,8,5);
  connect( atombutton, SIGNAL( clicked() ), this, SLOT( slot_atomwizard() ) );


  // and another label
  //complabel=new QLabel( "Model components", this );
  //lay->addWidget( complabel ,9,0,1,4);

  // create the list of model components
  lv2 = new QTreeWidget( this );
  lay->addWidget( lv2 ,0,2,8,4);
  lv2->setColumnCount(3);
  lv2->setSortingEnabled(false); //disable sorting, important for getting the indices right
  lv2->setRootIsDecorated( TRUE );



    QStringList labels;
    labels << "Model components" << "Monitors" << "Index";
    lv2->setHeaderLabels (labels);


//lv2->header()->setStretchLastSection(false);
//lv2->header()->resizeSection(2,200);
//lv2->header()->setResizeMode(0,QHeaderView::Stretch);
//lv2->header()->setResizeMode(1,QHeaderView::Stretch);
//lv2->header()->setResizeMode(2,QHeaderView::Custom);
//lv2->resizeColumnToContents(0);
//lv2->resizeColumnToContents(0);




  //make sure all lists and maps are cleared at startup
  itemmap.clear();
  componentmap.clear();
  lv2->clear();
  modelList.clear();

  this->slot_update(); //creates the list
  this->setLayout(lay);
  this->show();
  this->setMinimumSize(this->minimumSizeHint()); //needed since QT4 to make sure the widget is not shown minimized

  //connect the things you can do by clicking on the list
  connect(lv2, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int)  ), this, SLOT( slot_param_doubleclick(QTreeWidgetItem*,int) ) );
 connect(lv2, SIGNAL( itemClicked ( QTreeWidgetItem *, int)   ), this, SLOT( slot_param_click(QTreeWidgetItem*,int) ) );
 connect(lv2, SIGNAL( itemPressed ( QTreeWidgetItem *, int)   ), this, SLOT( slot_param_press(QTreeWidgetItem*,int) ) );
 connect(lv2, SIGNAL( itemChanged ( QTreeWidgetItem *, int)), this, SLOT( slot_rename(QTreeWidgetItem*,int) ) );

  this->repaint(); //force a repaint
}

Componentmaintenance::~Componentmaintenance(){
  //tell eelsmodel that we leave
  emit componentmaintenance_died();
  //kill all structures referred to by pointers which we created
  if (lv2!=0) delete(lv2);
}

void Componentmaintenance::slot_update_monitors(){
    //update all items without recreating the list
    //but make sure the monitors are up to date
    Model * mymodel=(geteelsmodelptr())->getmodel_nonconst();
    mymodel->updatemonitors();
for (int i=0;i<modelList.size();i++){
        QTreeWidgetItem* myitem=modelList[i];
        updateitem(myitem); //update the component name
        for(int j=0;j<myitem->childCount();j++){
                updateitem(myitem->child(j));
        }
    }
}
void  Componentmaintenance::slot_update(){
  //add items to the list when new components apeared
  //remove them from the list when they are no longer in the componentlist
  //check if the model is still valid
  if (!modelvalid()) {
    quit();
  }
  //create a new list, this way we are sure that the names are OK and that all components
  //are listed that are present in the model
  //clear up the maps
  modelList.clear();
  itemmap.clear();
  componentmap.clear();
  lv2->clear();

  for (size_t i=0;i<compvector->size();i++){
      Component* mycomponent=(*compvector)[i];
      QTreeWidgetItem* myitem=new QTreeWidgetItem(lv2); //create a new item
      myitem->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);  //make the item editable
	  componentmap[myitem]=mycomponent;   //link the item with the component via componentmap
      modelList.append(myitem);           //add it to the itemlist
	  updateitem(myitem); //write names and icons etc

	  size_t compnr=mycomponent->get_nr_of_parameters();
	  for ( size_t i = 0; i < compnr; i++ ){
	    //add for each parameter a label with its name and make it double-clickable
	    Parameter* myparameter=mycomponent->getparameter(i);
	    QTreeWidgetItem*  mychild =new QTreeWidgetItem(myitem);
        itemmap[mychild]=myparameter; //add a pair to the itemmap
        updateitem(mychild); //draw the proper icons, and add name and index
	  }
  }
//try to make collumn 2 small
lv2->header()->resizeSection ( 2, 5 );

  //lv2->triggerUpdate();

//update the number of components etc in complabel
const Model * mymodel=(geteelsmodelptr())->getmodel();
int total=mymodel->getnrofallparams();
int free=mymodel->getnroffreeparameters();
std::string labelstring;
    std::ostringstream s;
      if ((s << "Model components "<<total<<" parameters of which "<<free<<" are free")){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     labelstring=s.str();
      }
      this->setWindowTitle(labelstring.c_str());

}

void Componentmaintenance::slot_remove_components(){
  //look for an item in the model list that is selected
  //if more are selected, remove only the first
  //to avoid removing components in a list that is changing
  //after a remove_components, the REMOVER should call Componentmaintenance::update()
  //to update the new contents of the model

  QTreeWidgetItem * item=lv2->currentItem();



    //find the index in the model

    //check whether it is a component and not the parameter of a component
    //in that case return the parent
    if (item->parent()!=0){
       item=item->parent();
    }
    const Component* mycomponent=componentmap[item];
    int index=0;
    for (size_t i=0;i<compvector->size();i++){
      const Component* currentcomponent=compvector->at(i);
      if (currentcomponent==mycomponent){
        index=i;
        break;
      }
    }

    if (index>=0){
      emit remove_components(index);  //ask eelsmodel to do it for us, we don't know when it will be ready so we can`t update ourselves
    }
}

void Componentmaintenance::slot_add_components(){
  //the remover should call slot_update to draw the result
  QTreeWidgetItem * item=lv1->currentItem();
  emit add_components(lv1->indexOfTopLevelItem(item)); //ask eelsmodel to do it for us
}


void  Componentmaintenance::slot_info(){
  //show info on a selected component

  QTreeWidgetItem * item=lv1->currentItem();
  const int index=lv1->indexOfTopLevelItem(item);
  const std::string name=((*allvector)[index])->getname();
  const std::string description=((*allvector)[index])->getdescription();
  const std::string mesg= "name: "+name+"\n"+"description: "+description+"\n";
  QMessageBox* infobox;
  infobox->information (this,"Component info",mesg.c_str());
}

void  Componentmaintenance::slot_couple(){
  //couple two parameters
  //get the parameter that is selected
  QTreeWidgetItem* item=lv2->currentItem();
  if (item==0){
    Saysomething mysay(0,"Info","You have to select a parameter to couple first");
    couplesequence=false;
    coupleslave=0;
    coupleslaveitem=0;
    return;
  }
  else if (item->isSelected()&&itemisparameter(item)){
    coupleslave=(itemmap[item]); //store the pointer to this parameter in temp storage
    coupleslaveitem=item;
    couplesequence=true;
    //change the cursor to show that we are in couple sequence mode
    QCursor crosshair=QCursor(coupledicon,-1,-1);
    this->setCursor (crosshair);
    return;
    //now wait for the user to click on the parameter to couple to
  }
  Saysomething mysay(0,"Info","You have to select a parameter to couple first");
    couplesequence=false;
    coupleslave=0;
    coupleslaveitem=0;
}

void  Componentmaintenance::slot_monitor(){
  //monitor two parameters
  //get the parameter that is selected
  QTreeWidgetItem* item=lv2->currentItem();
  if (item==0){
      Saysomething mysay(0,"Info","You have to select a parameter to monitor first");
        monitorsequence=false;
        monitor1=0;
        monitor1item=0;
        return;
    }
  if (item->isSelected()&&itemisparameter(item)){
    monitor1=(itemmap[item]); //store the pointer to this parameter in temp storage
    monitor1item=item;
    monitorsequence=true;
    //change the cursor to show that we are in couple sequence mode
    QCursor crosshair=QCursor(monitoricon,-1,-1);
    this->setCursor (crosshair);
    return;
    //now wait for the user to click on the parameter to couple to
  }
  //no parameter selected
  Saysomething mysay(0,"Info","You have to select a parameter to monitor first");
  monitorsequence=false;
  monitor1=0;
  monitor1item=0;
}


void Componentmaintenance::slot_rename(QTreeWidgetItem* item,int col){
  //if a component was renamed, change its name
  if (itemiscomponent(item)){
    Component* mycomponent=(componentmap[item]);
    std::string newname;
    try{
      newname=(item->text(col).toStdString());
      mycomponent->setdisplayname(newname);
    }
    catch(...){
      //something went wrong, do nothing
    }

    //slot_update();//redraw with the new names
    updateitem(item); //show the updated name
    emit signal_update(); //warn the model
  }
}


void Componentmaintenance::slot_param_doubleclick(QTreeWidgetItem* item,int col){
  if (itemisparameter(item)){
    //call interactive parameter edit
    (itemmap[item])->interactivevalue("edit value");
    //calculate the changes and update the screen
    //slot_update();//draw the right locks after changing the changeable state
    updateitem(item);//draw the right locks after changing the changeable state
    emit signal_update(); //warn the model
  }
}

void Componentmaintenance::slot_param_rightclick(QTreeWidgetItem* item){
  //if right clicked on a parameter: change locked state or in case it was coupled, remove the coupling

  //if you right press in couple sequence, the proces is interupted
  if (couplesequence){
    //get the normal cursor again
    QCursor arrow(Qt::ArrowCursor);
    this->setCursor (arrow);
    couplesequence=false;
    coupleslave=0;
    coupleslaveitem=0;
    return;
  }

  //check if item is a parameter by looking it up in the itemmap
  if (itemisparameter(item)){
    if((itemmap[item])->iscoupledslave()){
      //remove the coupling in case of a coupled slave
      (itemmap[item])->deletecoupling();
    }
    else{
      //toggle the locked state
      const bool oldstate= (itemmap[item])->ischangeable();
      (itemmap[item])->setchangeable(!oldstate);
#ifdef DEBUG_COMPONENTMAINTENANCE
    std::cout << "togling without shift \n";
#endif
      //if shift pressed then change all paramters in the component to this state
      if (shiftButtonPressed){
#ifdef DEBUG_COMPONENTMAINTENANCE
    std::cout << "shift pressed\n";
#endif
          //get the component this parameter belongs to
          //browse through all components
          Component* mycomponent=0;
          for (size_t i=0;i<compvector->size();i++){
            mycomponent=(*compvector)[i];
            if (mycomponent->hasparameter(itemmap[item])){
#ifdef DEBUG_COMPONENTMAINTENANCE
    std::cout << "parameter found in component: "<<i<<"\n";
#endif
                for (size_t j=0;j<mycomponent->get_nr_of_parameters();j++){
                    Parameter* myparameter=mycomponent->getparameter(j);
                    myparameter->setchangeable(!oldstate);
                }
            }

          }

     }
    }
    //slot_update();        //update the names and symbols etc
    updateitem(item); //update name and symbols etc
    emit signal_update(); //the model needs to know when coupling changes
  }
  if (itemiscomponent(item)){
    Component* mycomponent=(componentmap[item]);
    try{
        mycomponent->setoptions();
    }
    catch(...){
        //do nothing
        }
    slot_update();//redraw with the new names
    emit signal_update(); //warn the model
  }
}

void Componentmaintenance::updateitem(QTreeWidgetItem* item){
    //set the icons,name and monitor and index for the parameter
   //update the lock or coupled icons
   if (itemisparameter(item)){

  const Parameter* myparameter=itemmap[item];
  item->setText(0,myparameter->getname().c_str());

  //add an index
      std::string index;
      std::ostringstream s;
      const int i=(geteelsmodelptr()->getmodel())->getparamindexbypointer(myparameter);
      if ((s << i)){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     index=s.str();
      }
      item->setText(2,index.c_str() );


   //get the monitors and icons right
  if (myparameter->ismonitored()){
    item->setIcon (1,monitoricon);
    item->setText(1,(myparameter->getmonitor())->getresultstring().c_str());

  }
  if (myparameter->iscoupledslave()){
    item->setIcon(0,coupledicon);
  }
  else {
    if (myparameter->ischangeable()){
      item->setIcon (0,unlockedicon);
    }
    else{
      item->setIcon (0,lockedicon);
    }
  }
   }
   if (itemiscomponent(item)){
       //update name
         const Component* mycomponent=componentmap[item];
         std::string name=mycomponent->getdisplayname();
         if (name=="") {
           name=mycomponent->getname();      //if the displayname is empty, take the name of the component type
         }
        item->setText(0,name.c_str());
   }

}


void Componentmaintenance::slot_param_click(QTreeWidgetItem* item,int col){
    //was it clicked with right?
    if (rightbutton){
        slot_param_rightclick(item);
        return;
    }

    if (itemisparameter(item)&&couplesequence){
    //finish the coupling sequence
    const double fraction=1.0; //currently only fixed fraction coupling implemented
    try{
        if (coupleslave==itemmap[item]){
           Saysomething mysay(0,"Info","Can not couple parameter with itself");
           couplesequence=false;
           coupleslave=0;
           coupleslaveitem=0;

         }
         else{
              coupleslave->couple((itemmap[item]),fraction);
              //update display for both slave and master
              updateitem(item);
              updateitem(coupleslaveitem);
         }
    }
    catch(...){
      //something went wrong, stop coupling
      coupleslave=0;
      coupleslaveitem=0;
    }
    //slot_update();


    emit signal_update(); //the model needs to know when changeable changes
    couplesequence=false;
    //get the normal cursor again
    QCursor arrow(Qt::ArrowCursor);
    this->setCursor (arrow);
  }


  if (itemisparameter(item)&&monitorsequence){
    //finish the monitor sequence
    int type=-1;
    MonitorChooser * mychooser=new MonitorChooser(0,"Monitor Chooser",type);
    (void) mychooser; //get rid of unused variable warning
    if ((type<0)||(type>2)){
      //not a valid type, exit this cycle
      monitor1=0;
      monitor1item=0;
      monitorsequence=false;
      slot_update();
      //get the normal cursor again
      QCursor arrow(Qt::ArrowCursor);
      this->setCursor (arrow);
      return;
    }
    try{
      //create a new monitor
      Monitor* mymonitor=new Monitor(monitor1,itemmap[item],type);
      (void) mymonitor; //get rid of unused variable warning
      //update display for both parameters
      updateitem(monitor1item);
      updateitem(item);
    }
    catch(...){
      //something went wrong, stop coupling
      monitor1=0;
      monitor1item=0;
    }
    //slot_update();
    emit signal_update(); //the model needs to know when changeable changes
    monitorsequence=false;
    //get the normal cursor again
    QCursor arrow(Qt::ArrowCursor);
    this->setCursor (arrow);
  }


}

bool  Componentmaintenance::modelvalid()const{
  //check if the componentvector is still valid
  //if not, close down the component maintenance
  //this would mean that the model died
  try{
     if (compvector->size()>0){
       const Component* mycomponent=(*compvector)[0];
       std::string name=mycomponent->getname();
     }
  }
  catch(...){
     return false;
  }
  return true;
}

bool Componentmaintenance::itemisparameter(QTreeWidgetItem* item)const{
  //check if an item is a parameter by looking it up in the itemmap
  return (itemmap.find(item)!=itemmap.end());
}

bool Componentmaintenance::itemiscomponent(QTreeWidgetItem* item)const{
  //check if an item is a component by looking it up in the itemmap
  return (componentmap.find(item)!=componentmap.end());
}
void Componentmaintenance::getposition(int& xpos,int& ypos,int& w,int& h)const{
//return the position of the window
  QPoint p=this->pos();
  QSize s=this->size();

  xpos=p.x();
  ypos=p.y();
  w=s.width();
  h=s.height();
#ifdef DEBUG_COMPONENTMAINTENANCE
  std::cout<<"getposition: "<<xpos<<" : "<<ypos<<"\n";
  std::cout<<"getposition width x height: "<<w<<" : "<<h<<"\n";
#endif

}
void Componentmaintenance::slot_setposition(int x,int y,int w,int h){
//changes position of window on screen
//only when it fits on the screen
  // if (((qApp->mainWidget())->width()>x)&&((qApp->mainWidget())->height()>y)){
    const QPoint p=QPoint(x,y);
    this->resize(w,h);
    this->move(p);
    // }
#ifdef DEBUG_COMPONENTMAINTENANCE
  std::cout<<"setposition: "<<x<<" : "<<y<<"\n";
#endif
}

void Componentmaintenance::keyPressEvent( QKeyEvent *event )
{
   if( event->key()==Qt::Key_Shift) {
#ifdef DEBUG_COMPONENTMAINTENANCE
      std::cout << "shift key has been pressed\n";
#endif
      this->shiftButtonPressed = true;
   }
}


void Componentmaintenance::keyReleaseEvent( QKeyEvent *event )
{
   if( event->key()==Qt::Key_Shift) {
#ifdef DEBUG_COMPONENTMAINTENANCE
      std::cout << "shift key has been released\n";
#endif
      this->shiftButtonPressed = false;
   }
}



void Componentmaintenance::slot_param_press(QTreeWidgetItem* item,int col){
  //find out which button was pressed, this is needed because there is no signal to intercept right button clicks and presses
  if(QApplication::mouseButtons()==Qt::RightButton){
      slot_param_rightpress(item,col); //interupts coupling if in coupling sequence
      rightbutton=true;
  }
  else{
      rightbutton=false;
  }
  //left button is handled by other signals
}

void Componentmaintenance::slot_param_rightpress(QTreeWidgetItem* item,int col){
    //interupt coupling sequence
    couplesequence=false;
    //get the normal cursor again
    QCursor arrow(Qt::ArrowCursor);
    this->setCursor (arrow);
}

void Componentmaintenance::slot_atomwizard(){
    //open the atomwizard and create crosssections for selected atoms
    Model * mymodel=(geteelsmodelptr())->getmodel_nonconst();
    if (mymodel==0){
        return;
    }
    double E0=300e3;
    double beta=5.0e-3;
    double alpha=1.0e-3;
    const double chemicalshift=2.0; //allow for a small shift in energy due to bonding effects
    double resolution=3.0*mymodel->getdispersion(); //assume that experimenter did good sampling, anyway the MTF will give a psf of about 3 pixels
    bool dofinestructure=true;
    const double Estart=mymodel->getenergy(0);
    const double Estop=mymodel->getenergy(mymodel->getnpoints()-1);

    Atomchooser* myatom=new Atomchooser(getworkspaceptr(), "",Estart,Estop,E0, beta, alpha, resolution,dofinestructure );
    const int res=myatom->exec();
    if (res==1){
            //Add cross sections presses
            alpha=myatom->getalpha();
            beta=myatom->getbeta();
            E0=myatom->getE0();
            resolution=myatom->getresolution();
            dofinestructure=myatom->getdofine();
            //collect the names of the selected atoms and their energies
            std::vector<size_t> Zlist;
            std::vector<double> ElistK;
            std::vector<double> ElistL23;
            std::vector<std::string> namelistK;
            std::vector<std::string> namelistL23;
            myatom->getZ(&Zlist); //a list of atoms to create a cross section for
            myatom->getedges(&ElistK,0); //the K energies
            myatom->getedges(&ElistL23,1); //the L23 energies
            myatom->getnamed(&namelistK,0);
            myatom->getnamed(&namelistL23,1);

            for (size_t i=0;i<Zlist.size();i++){
                //loop over the selected atoms
                 //create the parameter list
                std::vector<Parameter*> parameterlist;

                Parameter* p1=new Parameter("E0",E0,1);
                p1->setchangeable(false);  //the fitter should not change this in normal operation
                parameterlist.push_back(p1);

                Parameter* p2=new Parameter("Ek",ElistK[i]-chemicalshift,1);
                p2->setchangeable(false);  //keep the edge onsets constant for better accuracy
                parameterlist.push_back(p2);

                Parameter* p3=new Parameter("Z",Zlist[i],1);
                p3->setchangeable(false);  //the fitter should not change this in normal operation
                parameterlist.push_back(p3);

                //100 should be a good value for most cases
                Parameter* p4=new Parameter("thetasteps",100.0,1);
                p4->setchangeable(false); //the fitter should not change this in normal operation
                parameterlist.push_back(p4);

                Parameter* p5=new Parameter("collection angle",beta,1);
                p5->setchangeable(false); //the fitter should not change this in normal operation
                parameterlist.push_back(p5);

                Parameter* p6=new Parameter("strength",1.0e2,1);
                parameterlist.push_back(p6);
                //auto-adjust this parameter after installing the edge

                Parameter* p7=new Parameter("convergence angle",alpha,1);
                p7->setchangeable(false); //the fitter should not change this in normal operation
                parameterlist.push_back(p7);


                if ((ElistK[i]!=0.0)&&(ElistK[i]>Estart)&&(ElistK[i]<Estop)){
                    //add a K-edge
                    mymodel->addcomponent(mymodel->getcomponentindexbyname("Hydrogenic K-edge"),&parameterlist); //get a K edge and supply the inf
                    Component* mycomponent=mymodel->getcomponent(mymodel->getcomponentsnr()-1);
                    mycomponent->setdisplayname(namelistK[i]);
                    mycomponent->estimateparams(); //auto-adjust strength
                     if (dofinestructure){
                        //add a fine structure component
                        const double Eonset=ElistK[i]-chemicalshift;
                        const double Ewidth=70.0; //make it dependent on the edge onset
                        add_finestruct(mycomponent,mymodel,Eonset,Ewidth, resolution);
                        //check for overlaps and emit a warning if neede
                        for (size_t j=0;j<Zlist.size();j++){
                            if ((((Eonset+Ewidth)>ElistK[j]-chemicalshift)&&(Eonset<ElistK[j]-chemicalshift))||(((Eonset+Ewidth)>ElistL23[j]-chemicalshift)&&(Eonset<ElistL23[j]-chemicalshift))){
                                //overlap detected
                                Saysomething mysay(0,"Info","Overlapping edges, adjust Estop manually!");
                            }
                        }
                    }
                }
                if ((ElistL23[i]!=0.0)&&(ElistL23[i]>Estart)&&(ElistL23[i]<Estop)){
                    //add a L23-edge
                    p2->setchangeable(true);
                    p2->setvalue(ElistL23[i]);
                    p2->setchangeable(false);
                    parameterlist[1]=p2;

                    mymodel->addcomponent(mymodel->getcomponentindexbyname("Hydrogenic L-edge"),&parameterlist); //get a L edge and supply the inf
                    Component* mycomponent=mymodel->getcomponent(mymodel->getcomponentsnr()-1);
                    mycomponent->setdisplayname(namelistL23[i]);
                    mycomponent->estimateparams(); //auto-adjust strength
                    if (dofinestructure){
                        //add a fine structure component
                        const double Eonset=ElistL23[i]-chemicalshift;
                        const double Ewidth=50.0; //make it dependent on the edge onset
                        add_finestruct(mycomponent,mymodel,Eonset,Ewidth, resolution);
                        //check for overlaps and emit a warning if neede
                        for (size_t j=0;j<Zlist.size();j++){
                            if ((((Eonset+Ewidth)>ElistK[j]-chemicalshift)&&(Eonset<ElistK[j]-chemicalshift))||(((Eonset+Ewidth)>ElistL23[j]-chemicalshift)&&(Eonset<ElistL23[j]-chemicalshift))){
                                //overlap detected
                                Saysomething mysay(0,"Info","Overlapping edges, adjust Estop manually!");
                            }
                        }
                    }
                }
               
                //and update everything
                this->slot_update();
                mymodel->calculate();
                mymodel->display(getworkspaceptr());

            }
    }


 delete(myatom); //kill the window
}
void Componentmaintenance::add_finestruct(Component* mycomponent,Model* mymodel,double Eonset,double Ewidth, double resolution){
    //add a Fine structure component to a given hydrogenic cross section
    std::vector<Parameter*> parameterlistfine;
    Parameter* pf1=new Parameter("Estart",Eonset,1); //same onset as cross section
    pf1->setchangeable(false);
    parameterlistfine.push_back(pf1);

    const double Estopfine=Eonset+Ewidth;
    Parameter* pf2=new Parameter("Estop",Estopfine,1);
    pf2->setchangeable(false);
    parameterlistfine.push_back(pf2);


    //store the options in the 3rd param
    Parameter* pf3=new Parameter("Constant broadening",-1.25,0);
    parameterlistfine.push_back(pf3);

    Parameter* pf4=new Parameter("Alpha",0.0,1);
    pf4->setchangeable(false);
    parameterlistfine.push_back(pf4);

    //add the datapoints
    //estimate required degree
    const size_t degree=size_t(Ewidth/fabs(resolution));

    for (size_t i=0;i<degree;i++){
        std::string name;
        std::ostringstream s;
        if ((s << "a"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
            // conversion worked
            name=s.str();
        }
        Parameter* p=new Parameter(name,1.0,1);
        p->setboundaries(-1.0,20.0);
        p->setlinear(true); //these parameters are linear
        parameterlistfine.push_back(p);
    }
    //now link this to a cross section which needs to be multiplied
    int cnr=mymodel->getcomponentindexbypointer(mycomponent);

    //save this number in a parameter
    Parameter* pf5=new Parameter("compnr",cnr,1);
    pf5->setchangeable(false);
    parameterlistfine.push_back(pf5);

    mymodel->addcomponent(mymodel->getcomponentindexbyname("Generic Fine Structure (DOS)"),&parameterlistfine); //get a L edge and supply the inf
    Component* mycomponentfine=mymodel->getcomponent(mymodel->getcomponentsnr()-1);
    mycomponent->setmultiplierptr(mycomponentfine); //tell to the xsection that we are his multiplier
    //and change the name
    std::string finename;
    std::ostringstream s;
    if ((s << mycomponent->getdisplayname()<< " Fine structure")){ //converting an int to a string in c++ style rather than unsafe c-style
        // conversion worked
        finename=s.str();
    }
    mycomponentfine->setdisplayname(finename);

    //add monitor to show the mass center
    Parameter* aptr=mycomponentfine->getparameter(3);
    Parameter* bptr=mycomponentfine->getparameter(2);
    Monitor* mptr=new Monitor(aptr,bptr,3); //make a monitor on a param to show result of mass center
    (void*) mptr; //supress unused variable warning
    //couple the edge onsets
    (mycomponentfine->getparameter(0))->couple(mycomponent->getparameter(1),1.0);
}
