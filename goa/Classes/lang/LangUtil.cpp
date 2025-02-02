
//
//  LangUtil.cpp
//  safari
//
//  Created by Srikanth Talapadi on 23/07/16.
//
//

#include "LangUtil.h"
#include "EnglishUtil.h"
#include "KannadaUtil.h"
#include "SwahiliUtil.h"
#include "TeluguUtil.h"
#include "cocos2d.h"

USING_NS_CC;
LangUtil* LangUtil::_instance = 0;

LangUtil::LangUtil() {
    I18N::I18nUtils::getInstance();
}

LangUtil::~LangUtil() {

}

LangUtil* LangUtil::getInstance() {
    if(!_instance) {
         _instance = new EnglishUtil();
        //_instance = new SwahiliUtil();
       // _instance = new TeluguUtil();
    }
    return _instance;
}

std::string LangUtil::convertUTF16CharToString(wchar_t alpha) {
    std::u16string u16s = std::u16string(1, alpha);
    std::string u8;
    cocos2d::StringUtils::UTF16ToUTF8(u16s, u8);
    return u8;
}

wchar_t LangUtil::convertStringToUTF16Char(std::string alphaString) {
    return alphaString.c_str()[0];
}

std::string LangUtil::translateString(std::string input) {
    return I18N::__(input);
}

void LangUtil::changeLanguage(SupportedLanguages lang) {

    if(this->wordManager != NULL) {
        delete this->wordManager;
        WordManager::destroyInstance();
    }

    switch (lang) {
        case SupportedLanguages::ENGLISH:
        {
            LangUtil::_instance = new EnglishUtil();
            Data moData = FileUtils::getInstance()->getDataFromFile("res/lang/eng/eng.mo");
            I18N::I18nUtils::getInstance()->removeAllMO();
            I18N::I18nUtils::getInstance()->addMO(moData.getBytes());
            break;
        }
        case SupportedLanguages::SWAHILI:
        {
            LangUtil::_instance = new SwahiliUtil();
            Data moData = FileUtils::getInstance()->getDataFromFile("res/lang/swa/swa.mo");
            I18N::I18nUtils::getInstance()->removeAllMO();
            I18N::I18nUtils::getInstance()->addMO(moData.getBytes());
            break;
        }

        case SupportedLanguages::KANNADA:
        {
            LangUtil::_instance = new KannadaUtil();
            Data kannadaMoData = FileUtils::getInstance()->getDataFromFile("res/lang/kan/kan.mo");
            I18N::I18nUtils::getInstance()->removeAllMO();
            I18N::I18nUtils::getInstance()->addMO(kannadaMoData.getBytes());
            break;
        }
        case SupportedLanguages::TELUGU:
        {
            LangUtil::_instance = new EnglishUtil();
            Data moData = FileUtils::getInstance()->getDataFromFile("res/lang/swa/swa.mo");
            I18N::I18nUtils::getInstance()->removeAllMO();
            I18N::I18nUtils::getInstance()->addMO(moData.getBytes());
            break;
        }


        default:
            break;
    }
}


//"You have chosen the %s1 hat.
std::string LangUtil::translateParameterizedString( std::string const& parameterizedString, std::string const& substituteString) {
    return I18N::i18nFormatStr(translateString(parameterizedString),translateString(substituteString).c_str());
}

//"There are birds %s1 in the %s2."
std::string LangUtil::translateTwoParameterizedString( std::string const& parameterizedString, 
                                                       std::string const& substituteString1, 
                                                       std::string const& substituteString2) {

    return I18N::i18nFormatStr(translateString(parameterizedString), translateString(substituteString1).c_str(), translateString(substituteString2).c_str());
}

WordInfo* LangUtil::loadLanguageSpecificWordMappingForAWord(const char* word) {
    return this->wordManager->loadLanguageSpecificWordMappingForAWord(word);
}

std::string LangUtil::getFontFile() {
    return "fonts/Roboto-Regular.ttf";
}
