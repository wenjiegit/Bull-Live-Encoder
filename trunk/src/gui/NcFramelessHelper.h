/*
    NcFramelessHelper, an easy way to support move/resize on
    frameless toplevel windows.
    Copyright (C) 2011  Nishant Parashar
    Email:- nishcode (at) gmail (dot) com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//Version 3.0.0

#ifndef NC_FRAMELESS_HELPER_H
#define NC_FRAMELESS_HELPER_H

#include <QObject>

class NcFramelessHelperImpl;

class NcFramelessHelper : public QObject
{

public:
  explicit NcFramelessHelper( QObject* parent = 0 );
  ~NcFramelessHelper();

  void activateOn( QWidget* topLevelWidget );
  void removeFrom( QWidget* topLevelWidget );

  void setWidgetMovable( bool movable );
  bool isWidgetMovable();

  void setWidgetResizable( bool resizable );
  bool isWidgetResizable();

  void useRubberBandOnMove( bool use );
  bool isUsingRubberBandOnMove();

  void useRubberBandOnResize( bool use );
  bool isUsingRubberBandOnResisze();

  //Make sure to leave the same content margins
  //around the widget as the newBorderWidth
  //this can be done by
  //yourWidget->layout()->setMargin( newBorderWidth );
  //otherwise your widget will not expose the
  //area where this class works
  void setBorderWidth( int newBorderWidth );
  int borderWidth();

protected:
  virtual bool eventFilter( QObject* obj, QEvent* event );

private:
  NcFramelessHelperImpl* d;
};

#endif // NC_FRAMELESS_HELPER_H
