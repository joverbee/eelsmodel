/***************************************************************************
                          drudeoptions.h  -  description
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

#ifndef DRUDEOPTIONS_H
#define DRUDEOPTIONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QValidator>

class QWorkspace;

/* A Qwidget to choose the options for Drude Lorentz. */

class Drudeoptions : public QDialog  {
   Q_OBJECT
    QIntValidator * int_validator;
    QCheckBox* intbutton;
    QCheckBox* showbutton;
    QCheckBox* relbutton;
    QCheckBox* kroegerbutton;
    QLineEdit* nmaxedit;
    QLineEdit* oscedit;
    QComboBox * mylistw;
    QStringList* mylist;
QString valstring;
public:
	Drudeoptions(QWorkspace *parent, const char *name,size_t nosc,size_t thetapoints,bool integrate,bool show ,bool dokroeger,bool dorel=false,int outputformat=0);
	~Drudeoptions();
	size_t getthetapoints();
	size_t getnosc();
    bool getintegrate();
    bool getshow();
    bool getdokroeger();
    bool getdorel();
    int getoutputformat();
    double getnumber(const QString&)const;

public slots:
	void slot_OK();
	void slot_Cancel();
	void slot_dokroeger(bool b);
	void slot_dorel(bool b);
};

#endif
