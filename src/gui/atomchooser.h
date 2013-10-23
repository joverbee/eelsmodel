/***************************************************************************
                          GDosoptions.h  -  description
                             -------------------
    begin                : Sat Nov 23 2002
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

#ifndef ATOMCHOOSER_H
#define ATOMCHOOSER_H

#include <qdialog.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <vector>

class QWorkspace;


class Atomchooser : public QDialog  {
   Q_OBJECT
    QDoubleValidator * double_validator;
    QLineEdit* alphaedit;
    QLineEdit* betaedit;
    QLineEdit* HTedit;
    QLineEdit* Resedit;
    QRadioButton* finestructbutton;
    QString valstring;
    std::vector<QPushButton*> buttonlist; //a list with button in the mendeleev table
public:
	Atomchooser(QWorkspace* parent, const char* name, double Estart,double Estop,double E0, double beta, double alpha, double resolution,bool dofinestructure );
	~Atomchooser();
    double getalpha();
    double getbeta();
    double getE0();
    double getresolution();
    bool getdofine();
    double getnumber(const QString& valstring)const;
    void getZ(std::vector<size_t>* Zlist); //returns a vector with the atomic numbers of the selected atoms
    void getedges(std::vector<double>* Elist,int type); //returns a vector with edge energies for the selected atoms and excitation of type 0=K, 1=L23
    void getnamed(std::vector<std::string>* Namelist,int type);
public slots:
	void slot_OK();
	void slot_Cancel();
};

#endif
