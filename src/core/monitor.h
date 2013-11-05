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
 * Copyright: 2003-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel core/monitor.h
 **/

#ifndef MONITOR_H
#define MONITOR_H

//#define MONITOR_DEBUG

#include <string>

#include "src/core/parameter.h"

/* A monitor class to monitor the ratio,difference or sum of two parameters and the statistical standard deviation of two parameters */

class Monitor {
private:
double cov; //covariance between parameters
Parameter* pp1;
Parameter* pp2;
int type;
double value;
std::string headerstring;
public:
Monitor(Parameter* parp1,Parameter* parp2,int typeindex);
~Monitor();
//inspectors
double ratio()const;
double sum()const;
double difference()const;
double ratiosigma()const;
double sumsigma()const;
double differencesigma()const;
double getresult()const;
double getsigma()const;
int gettype()const;
Parameter* getpar1(){return pp1;}
Parameter* getpar2(){return pp2;}
std::string getresultstring()const;
std::string getresultstringonlyvalues()const;
std::string getheaderstring()const;
void setheaderstring(std::string s);
void setvalue(double d);
//
void setcovariance(double p12){cov=p12;}
};

#endif
