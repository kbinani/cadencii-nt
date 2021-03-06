/**
 * XVSQFileReader.hpp
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
#ifndef CADENCII_SEQUENCE_IO_XVSQFILEREADER_HPP_
#define CADENCII_SEQUENCE_IO_XVSQFILEREADER_HPP_

#include <string>
#include <stack>
#include <map>
#include "../../vsq/Sequence.hpp"
#include "SAXAdapter.hpp"

namespace cadencii {

    /**
     * @brief A parser class for XVSQ file.
     * @todo parse <UstEvent>
     * @todo parse <AttachedCurves>
     * @todo parse <BgmFiles>
     * @todo parse <cacheDir>
     * @todo parse config
     */
    class XVSQFileReader {
        friend class SAXAdapter;

    public:
        class ParseException : public std::exception {
        };

    private:
        VSQ_NS::Sequence *sequence;
        VSQ_NS::Track currentTrack;
        VSQ_NS::Event currentEvent;
        VSQ_NS::Lyric currentLyric;
        VSQ_NS::Handle currentHandle;
        VSQ_NS::BPList currentBPList;
        VSQ_NS::Tempo currentTempo;
        VSQ_NS::Timesig currentTimesig;
        VSQ_NS::MixerItem currentMixerItem;

        int trackCount;
        std::stack<std::string> tagNameStack;

        std::map<std::string, VSQ_NS::DynamicsMode::DynamicsModeEnum> dynamicsModeValueMap;
        std::map<std::string, VSQ_NS::PlayMode::PlayModeEnum> playModeValueMap;
        std::map<std::string, VSQ_NS::EventType::EventTypeEnum> eventTypeValueMap;
        std::map<std::string, bool> boolValueMap;
        std::map<std::string, std::string> tagNameMap;

        /**
         * @brief A default instance of Track, to get default property of BPList.
         */
        VSQ_NS::Track defaultTrack;

    protected:
        SAXAdapter *adapter;

    public:
        XVSQFileReader();

        /**
         * @brief Read XVSQ from specified SAX(Simple API for XML) adapter.
         */
        void read(VSQ_NS::Sequence *sequence, SAXAdapter *adapter);

    protected:
        void startElement(const std::string &name);

        void endElement(const std::string &name);

        void characters(const std::string &ch);

    private:
        void charactersCommon(const std::string &ch, const std::string &tagName);

        void charactersVsqEvent(const std::string &ch, const std::string &tagName);

        void charactersID(const std::string &ch, const std::string &tagName);

        void charactersHandle(const std::string &ch, const std::string &tagName);

        void charactersLyric(const std::string &ch, const std::string &tagName);

        void charactersBPList(const std::string &ch, const std::string &tagName);

        void charactersTempo(const std::string &ch, const std::string &tagName);

        void charactersTimesig(const std::string &ch, const std::string &tagName);

        void charactersMaster(const std::string &ch, const std::string &tagName);

        void charactersMixer(const std::string &ch, const std::string &tagName);

        void charactersMixerItem(const std::string &ch, const std::string &tagName);

        void validate();

        template<class T>
        inline void insertIntegerEnumValueMap(std::map<std::string, T> &result, const T &enumValue);

        template<class classT, class enumT>
        inline void insertStringEnumValueMap(
            std::map<std::string, enumT> &result, const enumT &enumValue);

        inline const std::string getParentTag(int goBackCount = 0)const {
            std::stack<std::string> copy = tagNameStack;
            for (int i = 0; i <= goBackCount; i++) {
                if (copy.empty()) break;
                copy.pop();
            }
            return copy.empty() ? "" : copy.top();
        }

        inline bool isControlCurveTagName(const std::string &name) {
            return tagNameMap.find(name) != tagNameMap.end();
        }

        inline std::string getCurveNameFrom(const std::string &name) {
            return tagNameMap.at(name);
        }
    };
}

#endif
