//
//  LanguageManager.h
//  safari
//
//  Created by Shyamal  Upadhyaya on 24/07/16.
//
//

#include <stdio.h>
#include "cocos2d.h"
#include "../hero/RPGConfig.h"
#include "LangUtil.h"
#include "../i18n/I18nUtils.h"

USING_NS_CC;

#ifndef LanguageManager_h
#define LanguageManager_h

class LanguageManager {
private:
    LanguageManager(); // constructor is private
public:
    ~LanguageManager();
    static LanguageManager* _instance;
    static LanguageManager* getInstance();
    std::string translateString(std::string input);
    void changeLanguage(SupportedLanguages lang);
    std::string translateParameterizedString(std::string const& input, std::string const& param);
    std::string translateTwoParameterizedString(std::string const& parameterizedString, std::string const& substituteString1, std::string const& substituteString2);
};


#endif /* LanguageManager_h */
