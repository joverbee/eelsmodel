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
 * eelsmodel - gui/atomchooser.h
 **/

#ifndef ATOMCHOOSER_H
#define ATOMCHOOSER_H

#include <vector>

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QString>
#include <QValidator>

class QMdiArea;

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
    Atomchooser(QMdiArea* parent, const char* name, double Estart,double Estop,double E0, double beta, double alpha, double resolution,bool dofinestructure );
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
    void setweight(size_t Z, double weight,bool overlap);
};

#endif
