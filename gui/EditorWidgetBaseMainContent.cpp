/**
 * EditorWidgetBaseMainContent.cpp
 * Copyright © 2012 kbinani
 *
 * This file is part of cadencii.
 *
 * cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <sstream>
#include <math.h>
#include <QScrollArea>
#include <QPainter>
#include <QScrollBar>
#include "vsq/Timesig.hpp"
#include "gui/EditorWidgetBase.h"
#include "gui/EditorWidgetBaseMainContent.h"

namespace cadencii{
    using namespace std;
    using namespace VSQ_NS;

    EditorWidgetBaseMainContent::EditorWidgetBaseMainContent(QWidget *parent) :
        QGraphicsView(parent)
    {
        deconstructStarted = false;
        scene = new QGraphicsScene();
        setScene( scene );
        controllerAdapter = 0;
        parentWidget = 0;

        this->defaultTimesigList.push( Timesig( 4, 4, 0 ) );
        this->measureLineIterator = new MeasureLineIterator( &defaultTimesigList );
        this->setMouseTracking( true );
        this->setAlignment( Qt::AlignLeft | Qt::AlignTop );

        scene->setSceneRect( 0, 0, 5000, 10 );
    }

    EditorWidgetBaseMainContent::~EditorWidgetBaseMainContent(){
        deconstructStarted = true;
        delete measureLineIterator;
        delete scene;
    }

    QRect EditorWidgetBaseMainContent::getVisibleArea(){
        QRect result;
        QScrollBar *horizontalScroll = horizontalScrollBar();
        QScrollBar *verticalScroll = verticalScrollBar();
        if( parentWidget ){
            QSizeF preferedSceneSize = parentWidget->getPreferedSceneSize();
            int x = (int)((horizontalScroll->value() - horizontalScroll->minimum()) * preferedSceneSize.width() / (horizontalScroll->maximum() + horizontalScroll->pageStep() - horizontalScroll->minimum()));
            int y = (int)((verticalScroll->value() - verticalScroll->minimum()) * preferedSceneSize.height() / (verticalScroll->maximum() + verticalScroll->pageStep() - verticalScroll->minimum()));

            int width = horizontalScroll->width();
            int height = verticalScroll->height();
            result = QRect( x, y, width, height );
        }else{
            result = QRect( 0, 0, horizontalScroll->width(), verticalScroll->height() );
        }
        return result;
    }

    void EditorWidgetBaseMainContent::mouseMoveEvent( QMouseEvent *e ){
        this->parentWidget->repaint();
        QWidget::mouseMoveEvent( e );
    }

    void EditorWidgetBaseMainContent::setTimesigList( TimesigList *timesigList ){
        MeasureLineIterator *previous = measureLineIterator;
        this->timesigList = timesigList;
        measureLineIterator = new MeasureLineIterator( this->timesigList );
        if( previous ){
            delete previous;
        }
    }

    void EditorWidgetBaseMainContent::scrollContentsBy( int dx, int dy ){
        QGraphicsView::scrollContentsBy( dx, dy );
        if( deconstructStarted ) return;
        if( dy && parentWidget ){
            parentWidget->notifyVerticalScroll();
        }
        if( dx && parentWidget ){
            parentWidget->notifyHorizontalScroll();
        }
    }

    int EditorWidgetBaseMainContent::getSceneWidth(){
        return (int)scene->sceneRect().width();
    }

    void EditorWidgetBaseMainContent::drawForeground( QPainter *painter, const QRectF &rect ){
        QSizeF preferedSceneSize = parentWidget->getPreferedSceneSize();
        scene->setSceneRect( 0, 0, preferedSceneSize.width(), preferedSceneSize.height() );

        QRect visibleArea( (int)rect.x(), (int)rect.y(), (int)rect.width(), (int)rect.height() );
        parentWidget->paintMainContent( painter, visibleArea );
    }

    void EditorWidgetBaseMainContent::setEditorWidgetBase( EditorWidgetBase *editorWidgetBase ){
        parentWidget = editorWidgetBase;
    }

    void EditorWidgetBaseMainContent::setControllerAdapter( ControllerAdapter *controllerAdapter ){
        this->controllerAdapter = controllerAdapter;
    }

    void EditorWidgetBaseMainContent::paintMeasureLines( QPainter *g, QRect visibleArea ){
        int top = visibleArea.top();
        int bottom = visibleArea.bottom();
        int left = visibleArea.left();
        int right = visibleArea.right();
        tick_t tickAtScreenRight = (tick_t)controllerAdapter->getTickFromX( right );
        measureLineIterator->reset( tickAtScreenRight );

        QColor barColor( 161, 157, 136 );
        QColor beatColor( 209, 204, 172 );
        while( measureLineIterator->hasNext() ){
            MeasureLine line = measureLineIterator->next();
            int x = controllerAdapter->getXFromTick( line.tick );
            if( x < left ){
                continue;
            }else if( right < x ){
                break;
            }
            if( line.isBorder ){
                g->setPen( barColor );
            }else{
                g->setPen( beatColor );
            }
            g->drawLine( x, top, x, bottom );
        }
    }

    void EditorWidgetBaseMainContent::paintSongPosition( QPainter *g, QRect visibleArea ){
        tick_t songPosition = controllerAdapter->getSongPosition();
        int x = controllerAdapter->getXFromTick( songPosition );
        g->setPen( QColor( 0, 0, 0 ) );
        g->drawLine( x, visibleArea.top(), x, visibleArea.bottom() );
        g->setPen( QColor( 0, 0, 0, 40 ) );
        g->drawLine( x - 1, visibleArea.top(), x - 1, visibleArea.bottom() );
        g->drawLine( x + 1, visibleArea.top(), x + 1, visibleArea.bottom() );
    }

}