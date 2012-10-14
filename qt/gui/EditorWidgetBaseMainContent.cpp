/**
 * EditorWidgetBaseMainContent.cpp
 * Copyright © 2012 kbinani
 *
 * This file is part of cadencii.
 *
 * cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2.0 as published by the Free Software Foundation.
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
#include "../../vsq/Timesig.hpp"
#include "EditorWidgetBase.hpp"
#include "EditorWidgetBaseMainContent.hpp"
#include "../../Settings.hpp"

namespace cadencii{
    using namespace std;
    using namespace VSQ_NS;

    EditorWidgetBaseMainContent::EditorWidgetBaseMainContent(QWidget *parent) :
        QGraphicsView(parent)
    {
        deconstructStarted = false;
        scene = new QGraphicsScene();
        setScene( scene );
        parentWidget = 0;

        this->setMouseTracking( true );
        this->setAlignment( Qt::AlignLeft | Qt::AlignTop );
        this->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );

        scene->setSceneRect( 0, 0, 5000, 10 );
    }

    EditorWidgetBaseMainContent::~EditorWidgetBaseMainContent(){
        deconstructStarted = true;
        delete scene;
    }

    QRect EditorWidgetBaseMainContent::getVisibleArea(){
        QRect result;
        QScrollBar *horizontalScroll = horizontalScrollBar();
        QScrollBar *verticalScroll = verticalScrollBar();
        if( parentWidget ){
            QSize preferedSize = parentWidget->getPreferedSceneSize();
            float preferedHeight = preferedSize.height();
            float preferedWidth = preferedSize.width();
            int x = (int)((horizontalScroll->value() - horizontalScroll->minimum()) * preferedWidth / (horizontalScroll->maximum() + horizontalScroll->pageStep() - horizontalScroll->minimum()));
            int y = (int)((verticalScroll->value() - verticalScroll->minimum()) * preferedHeight / (verticalScroll->maximum() + verticalScroll->pageStep() - verticalScroll->minimum()));

            int width = this->width();
            int height = this->height();
            result = QRect( x, y, width, height );
        }else{
            result = QRect( 0, 0, horizontalScroll->width(), verticalScroll->height() );
        }
        return result;
    }

    void EditorWidgetBaseMainContent::mouseMoveEvent( QMouseEvent *e ){
        this->parentWidget->repaint();
        QWidget::mouseMoveEvent( e );
        emit onMouseMove( e );
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

    int EditorWidgetBaseMainContent::getSceneHeight(){
        return (int)scene->sceneRect().height();
    }

    void EditorWidgetBaseMainContent::drawForeground( QPainter *painter, const QRectF &rect ){
        QSize preferedSize = parentWidget->getPreferedSceneSize();
        scene->setSceneRect( 0, 0, preferedSize.width(), preferedSize.height() );

        QRect visibleArea( (int)rect.x(), (int)rect.y(), (int)rect.width(), (int)rect.height() );
        parentWidget->paintMainContent( painter, visibleArea );
    }

    void EditorWidgetBaseMainContent::setEditorWidgetBase( EditorWidgetBase *editorWidgetBase ){
        parentWidget = editorWidgetBase;
    }

    void EditorWidgetBaseMainContent::paintMeasureLines( QPainter *g, QRect visibleArea ){
        int left = visibleArea.left();
        int right = visibleArea.right();
        tick_t tickAtScreenRight = (tick_t)parentWidget->controllerAdapter->getTickFromX( right );

        const VSQ_NS::TimesigList *list = 0;
        static VSQ_NS::TimesigList defaultList;
        const VSQ_NS::Sequence *sequence = parentWidget->controllerAdapter->getSequence();
        if( sequence ){
            list = &sequence->timesigList;
        }else{
            list = &defaultList;
        }
        VSQ_NS::MeasureLineIterator i( list );
        QuantizeMode::QuantizeModeEnum mode = Settings::instance()->getQuantizeMode();
        if( Settings::instance()->isGridVisible() && mode != QuantizeMode::NONE ){
            VSQ_NS::tick_t assistLineStep = QuantizeMode::getQuantizeUnitTick( mode );
            i = VSQ_NS::MeasureLineIterator( list, assistLineStep );
        }
        i.reset( tickAtScreenRight );

        while( i.hasNext() ){
            MeasureLine line = i.next();
            int x = parentWidget->controllerAdapter->getXFromTick( line.tick );
            if( x < left ){
                continue;
            }else if( right < x ){
                break;
            }
            parentWidget->drawMeasureLine( g, visibleArea, x, line );
        }
    }

    void EditorWidgetBaseMainContent::paintSongPosition( QPainter *g, QRect visibleArea ){
        tick_t songPosition = parentWidget->controllerAdapter->getSongPosition();
        int x = parentWidget->controllerAdapter->getXFromTick( songPosition );
        g->setPen( QColor( 0, 0, 0 ) );
        g->drawLine( x, visibleArea.top(), x, visibleArea.bottom() );
        g->setPen( QColor( 0, 0, 0, 40 ) );
        g->drawLine( x - 1, visibleArea.top(), x - 1, visibleArea.bottom() );
        g->drawLine( x + 1, visibleArea.top(), x + 1, visibleArea.bottom() );
    }

    void EditorWidgetBaseMainContent::mousePressEvent( QMouseEvent *event ){
        emit onMousePress( event );
    }

    void EditorWidgetBaseMainContent::mouseReleaseEvent( QMouseEvent *event ){
        emit onMouseRelease( event );
    }

}
