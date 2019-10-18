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
 * eelsmodel - gui/GDOSoptions.h
 **/

#ifndef GDOSOPTIONS_H
#define GDOSOPTIONS_H

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QString>
#include <QValidator>

class QMdiArea;

/* A Qwidget to choose the type of fitter to use. */

class GDosoptions : public QDialog  {
   Q_OBJECT
    QIntValidator * int_validator;
    QDoubleValidator * double_validator;
    QComboBox* interpolchooser;
    QComboBox* lifetimechooser;
    QRadioButton* sumrulebutton;
    QLineEdit* nmaxedit;
    QLineEdit* integrationedit;
    QLineEdit* thresholdedit;
QString valstring;
public:
    GDosoptions(QMdiArea *parent, const char *name,size_t degree,size_t interpolationtype,bool dosumrule,size_t broadeningtype,double integrationwidth,double threshold);
	~GDosoptions();
	size_t getdegree();
    bool getsumrule();
    size_t getinterpolation();
    size_t getbroadeningtype();
    double getnumber(const QString&)const;
    double getintegrationwidth()const;
    double getthreshold()const;

public slots:
	void slot_OK();
	void slot_Cancel();
	void slot_changeinterpol(int index);
};

#endif
