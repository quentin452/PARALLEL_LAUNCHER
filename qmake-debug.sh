#!/bin/sh
QMAKE_OPTS="app.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug"
if [ -n "$(which qmake-qt5)" ]; then
	qmake-qt5 $QMAKE_OPTS
elif [ -n "$(which qtchooser)" ]; then
	qmake -qt=qt5 $QMAKE_OPTS
else
	qmake $QMAKE_OPTS
fi
lupdate app.pro -no-obsolete
