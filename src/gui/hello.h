/***************************************************************************
                          hello.h  -  description
                             -------------------
    begin                : Sat Apr 19 2003
    copyright            : (C) 2003 by Jo Verbeeck
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

#ifndef HELLO_H
#define HELLO_H

#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

/* popup screen when EELSMODEL starts, shows author copyright and licence info. */

class Hello : public QDialog  {
    Q_OBJECT
    QWidget* parentptr;
    QVBoxLayout *vbox;
    QHBoxLayout *box1;
    QVBoxLayout *bgrp2;
    QLabel* dummylabel;
    QLabel* authorlabel;
    QLabel* emaillabel;
    QTextEdit* gnu;
    QPushButton *OK;
    QPushButton *licenceb;

    QTextEdit* gnul;
public:

	Hello(QWidget *parent=0, const char *name=0);
	~Hello();
	void cleanup();
public slots:
void slot_ok();
void slot_licence();
};

#endif
