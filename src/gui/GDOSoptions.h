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

#ifndef GDOSOPTIONS_H
#define GDOSOPTIONS_H

#include <qdialog.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qstring.h>
class QWorkspace;
/**A Qwidget to choose the type of fitter to use
  *@author Jo Verbeeck
  */

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
	GDosoptions(QWorkspace *parent, const char *name,size_t degree,size_t interpolationtype,bool dosumrule,size_t broadeningtype,double integrationwidth,double threshold);
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
