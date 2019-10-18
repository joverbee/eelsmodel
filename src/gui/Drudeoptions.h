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
 * eelsmodel - gui/Drudeoptions.h
 **/

#ifndef DRUDEOPTIONS_H
#define DRUDEOPTIONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QValidator>

class QMdiArea;

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
    Drudeoptions(QMdiArea *parent, const char *name,size_t nosc,size_t thetapoints,bool integrate,bool show ,bool dokroeger,bool dorel=false,int outputformat=0);
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
