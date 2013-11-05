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
 * eelsmodel - gui/getfilename.h
 **/

#ifndef GETFILENAME_H
#define GETFILENAME_H

#include <string>

#include <QWidget>
#include <QString>

/* QWidget to select a file from a list. */

class Getfilename : public QWidget  {
Q_OBJECT
private:

public: 
QString filter;
QString name;
QString caption;
Getfilename(QWidget *parent=0, std::string nm=0);
~Getfilename();
std::string open();
std::string save();
void setdirname(std::string dname);//set the working directory
void setfilter(std::string fstring);
void setname(std::string nm);
void setcaption(std::string capt);
};

#endif
