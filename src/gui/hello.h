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
 * eelsmodel - gui/hello.h
 **/

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
    QLabel* githublabel;
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
