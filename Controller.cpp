/**
 * Controller.cpp
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
#include "Controller.hpp"
#include "vsq/VSQFileReader.hpp"
#include "vsq/FileInputStream.hpp"

namespace cadencii{

    Controller::Controller()
        : trackView( 0 ), mainView( 0 ), controlChangeView( 0 ), barCountView( 0 ), tempoView( 0 ),
          sequence( "Miku", 1, 4, 4, 500000 ), songPosition( 0 ), pixelPerTick( 0.2 )
    {
    }

    void Controller::setTrackView( TrackView *trackView )throw(){
        this->trackView = trackView;
        if( this->trackView ){
            this->trackView->setControllerAdapter( this );
            this->trackView->setSequence( &sequence );
        }

        if( mainView ){
            mainView->setTrackView( this->trackView );
        }
    }

    void Controller::setControlChangeView( ControlChangeView *controlChangeView )throw(){
        this->controlChangeView = controlChangeView;
        if( this->controlChangeView ){
            this->controlChangeView->setControllerAdapter( this );
            this->controlChangeView->setSequence( &sequence );
        }
        if( mainView ){
            mainView->setControlChangeView( this->controlChangeView );
        }
    }

    void Controller::setMainView( MainView *mainView )throw(){
        this->mainView = mainView;
        if( this->mainView ){
            this->mainView->setControllerAdapter( this );

            if( this->trackView ){
                this->mainView->setTrackView( this->trackView );
            }
            if( this->controlChangeView ){
                this->mainView->setControlChangeView( this->controlChangeView );
            }
            if( this->barCountView ){
                this->mainView->setBarCountView( this->barCountView );
            }
            if( this->tempoView ){
                this->mainView->setTempoView( this->tempoView );
            }
        }
    }

    void Controller::setBarCountView( BarCountView *barCountView )throw(){
        this->barCountView = barCountView;
        if( this->barCountView ){
            this->barCountView->setControllerAdapter( this );
            this->barCountView->setSequence( &sequence );
        }
        if( mainView ){
            mainView->setBarCountView( this->barCountView );
        }
    }

    void Controller::setTempoView( TempoView *tempoView )throw(){
        this->tempoView = tempoView;
        if( this->tempoView ){
            this->tempoView->setControllerAdapter( this );
            this->tempoView->setSequence( &sequence );
        }
        if( mainView ){
            mainView->setTempoView( this->tempoView );
        }
    }

    void Controller::openVSQFile( const ::std::string &filePath )throw(){
        VSQ_NS::VSQFileReader reader;
        VSQ_NS::FileInputStream stream( filePath );
        reader.read( sequence, &stream, "Shift_JIS" );
        stream.close();
        setupSequence();
    }

    void Controller::drawOffsetChanged( void *sender, VSQ_NS::tick_t offset )throw(){
        if( sender == (void *)trackView ){
            if( controlChangeView ) controlChangeView->setDrawOffset( offset );
            if( barCountView ) barCountView->setDrawOffset( offset );
            if( tempoView ) tempoView->setDrawOffset( offset );
        }else if( sender == (void *)controlChangeView ){
            if( trackView ) trackView->setDrawOffset( offset );
            if( barCountView ) barCountView->setDrawOffset( offset );
            if( tempoView ) tempoView->setDrawOffset( offset );
        }else if( sender == (void *)barCountView ){
            if( controlChangeView ) controlChangeView->setDrawOffset( offset );
            if( trackView ) trackView->setDrawOffset( offset );
            if( tempoView ) tempoView->setDrawOffset( offset );
        }else if( sender == (void *)tempoView ){
            if( controlChangeView ) controlChangeView->setDrawOffset( offset );
            if( trackView ) trackView->setDrawOffset( offset );
            if( barCountView ) barCountView->setDrawOffset( offset );
        }
    }

    VSQ_NS::tick_t Controller::getSongPosition()throw(){
        return songPosition;
    }

    int Controller::getXFromTick( VSQ_NS::tick_t tick )throw(){
        return (int)(tick * pixelPerTick) + 5;
    }

    double Controller::getTickFromX( int x )throw(){
        return (x - 5) / pixelPerTick;
    }

    void Controller::setTrackIndex( void *sender, int index )throw(){
        //TODO:trackIndexをフィールドで持っておくべき
        //TODO:senderの値によって、どのコンポーネントにsetTrackIndexを呼ぶか振り分ける処理が必要
        trackView->setTrackIndex( index );
        controlChangeView->setTrackIndex( index );
    }

    void Controller::setupSequence(){
        trackView->setSequence( &sequence );
        controlChangeView->setSequence( &sequence );
        barCountView->setSequence( &sequence );
        tempoView->setSequence( &sequence );
        setTrackIndex( this, 0 );
        controlChangeView->setControlChangeName( "pit" );
    }

}
