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
 * eelsmodel - gui/fileopener.h
 **/

#ifndef FILEOPENER_H
#define FILEOPENER_H

#include <string>

#include <QWidget>
#include <QString>

/* QWidget to open a file from a filename
 * if the filename is not found the path is stripped and the modelpath is taken
 * if file is still not found a fileopener window is show
 * if this is cancelled an empty string is returned
 * if file found the full filename with path is returned
 */

class Fileopener : public QWidget  {
   Q_OBJECT
QString dirname;
QString filter;
QString name;
QString caption;
QString filename;
QString modelpath;
QString projfile;
public:
	Fileopener(QWidget *parent, const char *name, std::string filename);
	~Fileopener();
        std::string open();
};

#endif
