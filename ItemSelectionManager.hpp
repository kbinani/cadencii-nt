/**
 * ItemSelectionManager.hpp
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
#ifndef __cadencii_ItemSelectionManager_hpp__
#define __cadencii_ItemSelectionManager_hpp__

#include "vsq/Event.hpp"
#include "ItemSelectionStatusListener.hpp"
#include <set>

namespace cadencii{

    /**
     * @brief 編集対象アイテムの選択状態を管理するクラス
     */
    class ItemSelectionManager{
    private:
        /**
         * @brief 選択されたアイテムへのポインタ
         */
        std::set<void *> itemList;

        /**
         * @brief 選択されたアイテムのうち、音符・歌手変更イベントへのポインタ
         */
        std::vector<const VSQ_NS::Event *> eventItemList;

        /**
         * @brief アイテムの選択状態を監視するリスナーのリスト
         */
        std::vector<ItemSelectionStatusListener *> listenerList;

    public:
        /**
         * @brief アイテムの選択状態を解除する
         */
        void clear(){
            itemList.clear();
            eventItemList.clear();
            notifyStatusChange();
        }

        /**
         * @brief アイテムを選択状態にする
         */
        void add( const VSQ_NS::Event *event ){
            if( !isContains( event ) ){
                itemList.insert( (void *)event );
                eventItemList.push_back( event );
                notifyStatusChange();
            }
        }

        /**
         * @brief アイテムが選択状態担っているかどうかを取得する
         */
        bool isContains( const VSQ_NS::Event *event )const{
            return itemList.find( (void *)event ) != itemList.end();
        }

        /**
         * @brief 選択状態の音符・歌手変更イベントのリストを取得する
         */
        const std::vector<const VSQ_NS::Event *> *getEventItemList()const{
            return &eventItemList;
        }

        /**
         * @brief 選択状態を監視するリスナーを登録する
         */
        void addStatusListener( ItemSelectionStatusListener *listener ){
            listenerList.push_back( listener );
        }

    private:
        /**
         * @brief すべてのリスナーに、選択状態が変化したことを通知する
         */
        void notifyStatusChange(){
            std::vector<ItemSelectionStatusListener *>::iterator i = listenerList.begin();
            for( ; i != listenerList.end(); ++i ){
                (*i)->statusChanged();
            }
        }
    };

}

#endif