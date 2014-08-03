/*
    FramelessHelper, an easy way to support move/resize on
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

#include <QRubberBand>
#include <QMouseEvent>
#include <QMutex>
#include <QDebug>
#include "NcFramelessHelper.h"

class NcCursorPosCalculator
{

public:
  NcCursorPosCalculator();
  void reset();
  void recalculate( const QPoint& globalMousePos, const QRect& frameRect );

public:
  bool onEdges;
  bool onLeftEdge;
  bool onRightEdge;
  bool onTopEdge;
  bool onBottomEdge;
  bool onTopLeftEdge;
  bool onBottomLeftEdge;
  bool onTopRightEdge;
  bool onBottomRightEdge;

  static int mBorderWidth;
};
//TODO: Should not be static.
int NcCursorPosCalculator::mBorderWidth = 5;

class NcWidgetData
{
public:
  NcWidgetData( NcFramelessHelperImpl* _d, QWidget* topLevelWidget );
  ~NcWidgetData();

  //void setWidget( QWidget* topLevelWidget );
  QWidget* widget();
  void handleWidgetEvent( QEvent* event );
  void updateRubberBandStatus();

private:
  void updateCursorShape( const QPoint& globalMousePos );
  void resizeWidget( const QPoint& globalMousePos );
  void moveWidget( const QPoint& globalMousePos );

  void handleMousePressEvent( QMouseEvent* event );
  void handleMouseReleaseEvent( QMouseEvent* event );
  void handleMouseMoveEvent( QMouseEvent* event );
  void handleLeaveEvent( QEvent* event );
  void handleHoverMoveEvent( QHoverEvent* event );

private:
  NcFramelessHelperImpl* d;
  QRubberBand* mRubberBand;
  bool mLeftButtonPressed;
  QWidget* mWidget;
  QPoint mDragPos;
  NcCursorPosCalculator mPressedMousePos;
  NcCursorPosCalculator mMoveMousePos;
  bool mCursorShapeChanged;
  Qt::WindowFlags mWindowFlags;
};


class NcFramelessHelperImpl
{
public:
  QHash< QWidget*, NcWidgetData* > mHashWidgetData;
  bool mWidgetMovable;
  bool mWidgetResizable;
  bool mUseRubberBandOnResize;
  bool mUseRubberBandOnMove;
};

NcCursorPosCalculator::NcCursorPosCalculator()
{
  reset();
}

void NcCursorPosCalculator::reset()
{
  onEdges = false;
  onLeftEdge = false;
  onRightEdge = false;
  onTopEdge = false;
  onBottomEdge = false;
  onTopLeftEdge = false;
  onBottomLeftEdge = false;
  onTopRightEdge = false;
  onBottomRightEdge = false;
}

void NcCursorPosCalculator::recalculate( const QPoint& globalMousePos, const QRect& frameRect )
{
  int globalMouseX = globalMousePos.x();
  int globalMouseY = globalMousePos.y();

  int frameX = frameRect.x();
  int frameY = frameRect.y();

  int frameWidth = frameRect.width();
  int frameHeight = frameRect.height();

  onLeftEdge = globalMouseX >= frameX &&
      globalMouseX <= frameX + mBorderWidth;


  onRightEdge = globalMouseX >= frameX + frameWidth - mBorderWidth &&
      globalMouseX <= frameX + frameWidth;

  onTopEdge = globalMouseY >= frameY &&
      globalMouseY <= frameY + mBorderWidth;


  onBottomEdge = globalMouseY >= frameY + frameHeight - mBorderWidth &&
      globalMouseY <= frameY + frameHeight;

  onTopLeftEdge = onTopEdge && onLeftEdge;
  onBottomLeftEdge = onBottomEdge && onLeftEdge;
  onTopRightEdge = onTopEdge && onRightEdge;
  onBottomRightEdge = onBottomEdge && onRightEdge;

  //only these checks would be enough
  onEdges = onLeftEdge || onRightEdge ||
      onTopEdge || onBottomEdge;

}


NcWidgetData::NcWidgetData( NcFramelessHelperImpl* _d, QWidget* topLevelWidget )
{
  d = _d;
  mWidget = topLevelWidget;
  mLeftButtonPressed = false;
  mRubberBand = 0;
  mCursorShapeChanged = false;

  mWindowFlags = mWidget->windowFlags();

  //---from Qt docs of setWindowFlags()----
  //Note: This function calls setParent() when
  //changing the flags for a window, causing the
  //widget to be hidden. You must call show()
  //to make the widget visible again..

  bool visible = mWidget->isVisible();

  mWidget->setMouseTracking( true );
  mWidget->setWindowFlags( /*Qt::CustomizeWindowHint|*/Qt::FramelessWindowHint|Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
  //Bug fix, mouse move events does not propagate from child widgets.
  //so need the hover events.
  mWidget->setAttribute( Qt::WA_Hover );

  updateRubberBandStatus();

  mWidget->setVisible( visible );
}

NcWidgetData::~NcWidgetData()
{
  //---from Qt docs of setWindowFlags()----
  //Note: This function calls setParent() when
  //changing the flags for a window, causing the
  //widget to be hidden. You must call show()
  //to make the widget visible again..

  bool visible = mWidget->isVisible();

  mWidget->setMouseTracking( false );
  mWidget->setWindowFlags( mWindowFlags );//^  Qt::CustomizeWindowHint ^ Qt::FramelessWindowHint );
  mWidget->setAttribute( Qt::WA_Hover, false );

  mWidget->setVisible( visible );

  delete mRubberBand;
}

void NcWidgetData::updateRubberBandStatus()
{
  if ( d->mUseRubberBandOnMove || d->mUseRubberBandOnResize )
  {
    if ( !mRubberBand )
      mRubberBand = new QRubberBand( QRubberBand::Rectangle );
  }
  else
  {
    delete mRubberBand;
    mRubberBand = 0;
  }
}

QWidget* NcWidgetData::widget()
{
  return mWidget;
}

void NcWidgetData::handleWidgetEvent( QEvent* event )
{
  switch ( event->type() )
  {
  default: //qDebug() << "Event = " << event;
    break;
  case QEvent::MouseButtonPress:
    handleMousePressEvent( static_cast<QMouseEvent*>( event ) );
    break;

  case QEvent::MouseButtonRelease:
    handleMouseReleaseEvent( static_cast<QMouseEvent*>( event ) );
    break;

  case QEvent::MouseMove:
    handleMouseMoveEvent( static_cast<QMouseEvent*>( event ) );
    break;

  case QEvent::Leave:
    handleLeaveEvent( event );
    break;

    //Bug fix, hover event is necessary coz child widget does not
    //propagate mousemove events. so the cursor remains in edge shape
    //even in middle of widget.
  case QEvent::HoverMove:
    handleHoverMoveEvent( static_cast<QHoverEvent*>( event ) );
    break;
    //case QEvent::Enter:
    //qDebug() << "Enter event";//d->handleEnterEvent( event );
    //break;
  }
}

void NcWidgetData::updateCursorShape( const QPoint& globalMousePos )
{
  if ( mWidget->isFullScreen() || mWidget->isMaximized() )
  {
    if ( mCursorShapeChanged )
      mWidget->unsetCursor();

    return;
  }

  mMoveMousePos.recalculate( globalMousePos, mWidget->frameGeometry() );

  if( mMoveMousePos.onTopLeftEdge || mMoveMousePos.onBottomRightEdge )
  {
    mWidget->setCursor( Qt::SizeFDiagCursor );
    mCursorShapeChanged = true;
  }
  else if( mMoveMousePos.onTopRightEdge || mMoveMousePos.onBottomLeftEdge )
  {
    mWidget->setCursor( Qt::SizeBDiagCursor );
    mCursorShapeChanged = true;
  }
  else if( mMoveMousePos.onLeftEdge || mMoveMousePos.onRightEdge )
  {
    mWidget->setCursor( Qt::SizeHorCursor );
    mCursorShapeChanged = true;
  }
  else if( mMoveMousePos.onTopEdge || mMoveMousePos.onBottomEdge )
  {
    mWidget->setCursor( Qt::SizeVerCursor );
    mCursorShapeChanged = true;
  }
  else
  {
    if ( mCursorShapeChanged )
    {
      mWidget->unsetCursor();
      mCursorShapeChanged = false;
    }
  }
}

void NcWidgetData::resizeWidget( const QPoint& globalMousePos )
{
  QRect origRect;

  if ( d->mUseRubberBandOnResize )
    origRect = mRubberBand->frameGeometry();
  else
    origRect = mWidget->frameGeometry();


  int left = origRect.left();
  int top = origRect.top();
  int right = origRect.right();
  int bottom = origRect.bottom();
  origRect.getCoords( &left, &top, &right, &bottom );

  int minWidth = mWidget->minimumWidth();
  int minHeight = mWidget->minimumHeight();

  if ( mPressedMousePos.onTopLeftEdge )
  {
    left = globalMousePos.x();
    top = globalMousePos.y();
  }
  else if ( mPressedMousePos.onBottomLeftEdge )
  {
    left = globalMousePos.x();
    bottom = globalMousePos.y();
  }
  else if ( mPressedMousePos.onTopRightEdge )
  {
    right = globalMousePos.x();
    top = globalMousePos.y();
  }
  else if ( mPressedMousePos.onBottomRightEdge )
  {
    right = globalMousePos.x();
    bottom = globalMousePos.y();
  }
  else if ( mPressedMousePos.onLeftEdge )
  {
    left = globalMousePos.x();
  }
  else if ( mPressedMousePos.onRightEdge )
  {
    right = globalMousePos.x();
  }
  else if ( mPressedMousePos.onTopEdge )
  {
    top = globalMousePos.y();
  }
  else if ( mPressedMousePos.onBottomEdge )
  {
    bottom = globalMousePos.y();
  }

  QRect newRect( QPoint(left, top), QPoint(right, bottom) );

  if ( newRect.isValid() )
  {
    if ( minWidth > newRect.width() )
    {
      //determine what has caused the width change.
      if( left != origRect.left() )
        newRect.setLeft( origRect.left() );
      else
        newRect.setRight( origRect.right() );
    }
    if ( minHeight > newRect.height() )
    {
      //determine what has caused the height change.
      if ( top != origRect.top() )
        newRect.setTop( origRect.top() );
      else
        newRect.setBottom( origRect.bottom() );
    }

    if ( d->mUseRubberBandOnResize )
    {
      mRubberBand->setGeometry( newRect );
    }
    else
    {
      mWidget->setGeometry( newRect );
    }
  }
  else
  {
    //qDebug() << "Calculated Rect is not valid" << newRect;
  }

}

void NcWidgetData::moveWidget( const QPoint& globalMousePos )
{
  if ( d->mUseRubberBandOnMove )
  {
    mRubberBand->move( globalMousePos - mDragPos );
  }
  else
  {
    mWidget->move( globalMousePos - mDragPos );
  }
}

void NcWidgetData::handleMousePressEvent( QMouseEvent* event )
{
  if ( event->button() == Qt::LeftButton )
  {
    mLeftButtonPressed = true;

    QRect frameRect = mWidget->frameGeometry();
    mPressedMousePos.recalculate( event->globalPos(), frameRect );

    mDragPos = event->globalPos() - frameRect.topLeft();

    if ( mPressedMousePos.onEdges  )
    {
      if ( d->mUseRubberBandOnResize )
      {
        mRubberBand->setGeometry( frameRect );
        mRubberBand->show();
      }
    }
    else if ( d->mUseRubberBandOnMove )
    {
      mRubberBand->setGeometry( frameRect );
      mRubberBand->show();
    }

  }
}

void NcWidgetData::handleMouseReleaseEvent( QMouseEvent* event )
{
  if ( event->button() == Qt::LeftButton )
  {
    mLeftButtonPressed = false;
    mPressedMousePos.reset();
    if ( mRubberBand && mRubberBand->isVisible() )
    {
      mRubberBand->hide();
      mWidget->setGeometry( mRubberBand->geometry() );
    }
  }
}

void NcWidgetData::handleMouseMoveEvent( QMouseEvent* event )
{
  if ( mLeftButtonPressed )
  {
    if ( d->mWidgetResizable && mPressedMousePos.onEdges )
    {
      resizeWidget( event->globalPos() );
    }
    else if ( d->mWidgetMovable )
    {
      moveWidget( event->globalPos() );
    }
  }
  else if ( d->mWidgetResizable )
  {
    updateCursorShape( event->globalPos() );
  }

}

void NcWidgetData::handleLeaveEvent( QEvent* /*event*/ )
{
  if ( !mLeftButtonPressed )
    mWidget->unsetCursor();
}

void NcWidgetData::handleHoverMoveEvent( QHoverEvent* event )
{
  if ( d->mWidgetResizable )
    updateCursorShape( mWidget->mapToGlobal( event->pos() ) );
}

NcFramelessHelper::NcFramelessHelper( QObject* parent )
  : QObject( parent ),
    d( new NcFramelessHelperImpl )
{
  d->mWidgetMovable = true;
  d->mWidgetResizable = true;
  d->mUseRubberBandOnResize = false;
  d->mUseRubberBandOnMove = false;
}

NcFramelessHelper::~NcFramelessHelper()
{
  QList<QWidget*> keys = d->mHashWidgetData.keys();
  int size = keys.size();
  for ( int i = 0; i < size; ++i )
  {
    delete d->mHashWidgetData.take( keys[i] );
  }

  delete d;
}

bool NcFramelessHelper::eventFilter( QObject *obj, QEvent *event )
{
  QEvent::Type type = event->type();

  if ( type == QEvent::MouseMove ||
       type == QEvent::HoverMove ||
       type == QEvent::MouseButtonPress ||
       type == QEvent::MouseButtonRelease ||
       type == QEvent::Leave
     )
 {
   NcWidgetData* data = d->mHashWidgetData.value( static_cast<QWidget*>(obj) );
   if ( data )
   {
     data->handleWidgetEvent( event );
   }

 }

  return false;
}

void NcFramelessHelper::activateOn( QWidget* topLevelWidget )
{
  if ( d->mHashWidgetData.contains( topLevelWidget ) )
    return;

  NcWidgetData* data = new NcWidgetData( d, topLevelWidget );
  d->mHashWidgetData.insert( topLevelWidget, data );

  topLevelWidget->installEventFilter( this );

}

void NcFramelessHelper::removeFrom( QWidget* topLevelWidget )
{
  NcWidgetData* data = d->mHashWidgetData.take( topLevelWidget );
  if ( data )
  {
    topLevelWidget->removeEventFilter( this );
    delete data;
  }
}

void NcFramelessHelper::setWidgetMovable( bool movable )
{
  d->mWidgetMovable = movable;
}

bool NcFramelessHelper::isWidgetMovable()
{
  return d->mWidgetMovable;
}

void NcFramelessHelper::setWidgetResizable( bool resizable )
{
  d->mWidgetResizable = resizable;
}

bool NcFramelessHelper::isWidgetResizable()
{
  return d->mWidgetResizable;
}

void NcFramelessHelper::useRubberBandOnMove( bool use )
{
  d->mUseRubberBandOnMove = use;
  QList<NcWidgetData*> list = d->mHashWidgetData.values();
  int size = list.size();
  for ( int i = 0; i < size; ++i )
    list[i]->updateRubberBandStatus();
}

bool NcFramelessHelper::isUsingRubberBandOnMove()
{
  return d->mUseRubberBandOnMove;
}

void NcFramelessHelper::useRubberBandOnResize( bool use )
{
  d->mUseRubberBandOnResize = use;
  QList<NcWidgetData*> list = d->mHashWidgetData.values();
  int size = list.size();
  for ( int i = 0; i < size; ++i )
    list[i]->updateRubberBandStatus();
}

bool NcFramelessHelper::isUsingRubberBandOnResisze()
{
  return d->mUseRubberBandOnResize;
}

void NcFramelessHelper::setBorderWidth( int newBorderWidth )
{
  //TODO: Make it non-static.
  if ( newBorderWidth >= 0 )
    NcCursorPosCalculator::mBorderWidth = newBorderWidth;
}

int NcFramelessHelper::borderWidth()
{
  return NcCursorPosCalculator::mBorderWidth;
}
