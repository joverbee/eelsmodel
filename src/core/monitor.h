/***************************************************************************
                          monitor.h  -  description
                             -------------------
    begin                : Fri Aug 8 2003
    copyright            : (C) 2003 by Jo Verbeeck
    email                : joverbee@ruca.ua.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MONITOR_H
#define MONITOR_H
//#define MONITOR_DEBUG
#include <src/core/parameter.h>
#include <string>
/**A monitor class to monitor the ratio,difference or sum of two parameters and the statistical standard deviation of two parameters
  *@author Jo Verbeeck
  */

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
