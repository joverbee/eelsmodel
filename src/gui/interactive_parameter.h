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
 * eelsmodel - gui/interactive_parameter.h
 *
 * A dialog to enter parameter value, name and boundaries interactively.
 * Requires a pointer to an existing parameter for the initial values.
 **/

#ifndef INTERACTIVE_PARAMETER_H
#define INTERACTIVE_PARAMETER_H

#include <string>

#include <QCheckBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QValidator>
#include <QVBoxLayout>
#include <QWidget>

#include "src/core/parameter.h"

class Interactive_Parameter : public QDialog  {
   Q_OBJECT
    QLineEdit* lowerboundedit;
    QLineEdit* upperboundedit;
    QLineEdit* parvaledit;
    double upperbound,lowerbound,value;
    QDoubleValidator * bound_validator;
    QDoubleValidator * double_validator;
    bool bound,canchange,finished,displayed;
    QString parameter_name;
    QLineEdit* parnameedit;
    QLabel* parnamelabel;
    QLabel* parvallabel;
    QCheckBox* check_change;
    QCheckBox* check_displayed;
    QCheckBox * check_bound;
    QLabel* lowboundlabel;
    QLabel* upboundlabel;
    QPushButton *OK ;

public:
	Interactive_Parameter(QWidget *parent, const char *name,const Parameter* initialparameter,const std::string& description);
	~Interactive_Parameter();
double getnumber(const QString&)const;
double getvalue()const{return value;};
double getubound()const{return upperbound;};
double getlbound()const{return lowerbound;};
bool getbound()const{return bound;};
bool isdisplayed()const{return displayed;};
bool getcanchange()const{return canchange;};
std::string getname()const{return parameter_name.toStdString();};
bool isfinished()const{return finished;};
bool  validate(); //check if value is valid (between bound and numeric etc)
public slots:
void slot_change_upper_bound(const QString&);
void slot_change_lower_bound(const QString&);
void slot_change_bound(bool boundstate);
void slot_change_val(const QString&);
void slot_change_name(const QString&);
void slot_change_displayed(bool state);
void slot_change_canchange(bool state);
void slot_ok();
};

#endif
