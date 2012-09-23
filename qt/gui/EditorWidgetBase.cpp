/**
 * EditorWidgetBase.cpp
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
#include <QScrollBar>
#include "qt/gui/EditorWidgetBase.hpp"
#include "qt/gui/EditorWidgetBaseMainContent.hpp"
#include "ui_EditorWidgetBase.h"

namespace cadencii{
    using namespace std;
    using namespace VSQ_NS;

    EditorWidgetBase::EditorWidgetBase( QWidget *parent ) :
        QWidget( parent ), controllerAdapter( 0 ), sequence( 0 )
    {
        ui = new Ui::EditorWidgetBase;
        ui->setupUi( this );
        ui->scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui->scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui->scrollArea->setBackgroundRole( QPalette::Dark );
        ui->scrollArea->setEditorWidgetBase( this );
        ui->keyboard->setEditorWidgetBase( this );
    }

    EditorWidgetBase::~EditorWidgetBase(){
        delete ui;
    }

    void EditorWidgetBase::notifyVerticalScroll(){
        QRect rect = ui->scrollArea->getVisibleArea();
        ui->keyboard->update();
    }

    void EditorWidgetBase::notifyHorizontalScroll(){
        QRect visibleRect = ui->scrollArea->getVisibleArea();
        tick_t drawOffset = (tick_t)controllerAdapter->getTickFromX( visibleRect.x() );
        controllerAdapter->drawOffsetChanged( getScrollEventSender(), drawOffset );
    }

    void EditorWidgetBase::repaint(){
        ui->scrollArea->repaint();
        ui->keyboard->repaint();
        QWidget::repaint();
    }

    void EditorWidgetBase::setDrawOffsetInternal( tick_t drawOffset ){
        static QMutex mutex;
        if( mutex.tryLock() ){
            int xScrollTo = -controllerAdapter->getXFromTick( drawOffset );
            QScrollBar *scrollBar = ui->scrollArea->horizontalScrollBar();
            int maxValue = scrollBar->maximum() + scrollBar->pageStep();
            int minValue = scrollBar->minimum();
            int contentWidth = (int)ui->scrollArea->getSceneWidth();
            int value = (int)(minValue + (minValue - maxValue) * (double)xScrollTo / contentWidth);
            if( scrollBar->value() != value ) scrollBar->setValue( value );
            mutex.unlock();
        }
    }

    void EditorWidgetBase::drawMeasureLine( QPainter *painter, const QRect &rect, int x, const MeasureLine &measureLine ){
        static QBrush barBrush( QColor::fromRgb( 161, 157, 136 ), Qt::SolidPattern );
        static QBrush beatBrush( QColor::fromRgb( 209, 204, 172 ), Qt::SolidPattern );
        QBrush brush = measureLine.isBorder ? barBrush : beatBrush;
        Qt::PenStyle style = measureLine.isAssistLine ? Qt::DashLine : Qt::SolidLine;
        QPen pen( brush, 1.0, style );
        painter->setPen( pen );
        painter->drawLine( x, rect.top(), x, rect.bottom() );
    }

    QSize EditorWidgetBase::getPreferedSceneSize(){
        int width = controllerAdapter->getPreferedComponentWidth();
        int height = this->height();
        return QSize( width, height );
    }

}