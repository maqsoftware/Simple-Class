//
//  MultipleChoice.cpp
//  goa
//
//  Created by Srikanth Talapadi on 08/01/2017.
//
//

#include "MultipleChoice.hpp"
#include "../effects/FShake.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "platform/CCFileUtils.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

MultipleChoice *MultipleChoice::create(QuestionHandler* qHandler, std::vector<std::string> questions) {
    MultipleChoice* lhs = new (std::nothrow) MultipleChoice();
    if(lhs && lhs->initWithQuestions(qHandler, questions))
    {
        lhs->autorelease();
        return lhs;
    }
    CC_SAFE_DELETE(lhs);
    return nullptr;
}

void MultipleChoice::onEnterTransitionDidFinish() {
    Node::onEnterTransitionDidFinish();
    runAction(FadeIn::create(1.0f));
}

void MultipleChoice::onExitTransitionDidStart() {
    _questionTextField->onExitTransitionDidStart();
}


MultipleChoice::MultipleChoice() {
    
}

MultipleChoice::~MultipleChoice() {
    
}

bool MultipleChoice::init() {
    return true;
}

bool MultipleChoice::initWithQuestions(QuestionHandler* qHandler, std::vector<std::string> questions) {
    _questions = questions;
    _qHandler = qHandler;
    auto bg = CSLoader::createNode("template/template.csb");
    auto visibleSize = Director::getInstance()->getVisibleSize();
    bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bg->setName("bg");
    addChild(bg);
    if(_questions.size() == 6) {
        auto qOriginalNode = static_cast<TextField*> (bg->getChildByName("TextField_2"));
        _questionTextField = CommonTextField::create();
        _questionTextField->setPosition(qOriginalNode->getPosition());
        _questionTextField->setName(qOriginalNode->getName());
        _questionTextField->setTextColor(qOriginalNode->getTextColor());
        _questionTextField->setFontSize(qOriginalNode->getFontSize());
        _questionTextField->setFontName(qOriginalNode->getFontName());
        qOriginalNode->getParent()->addChild(_questionTextField);
        qOriginalNode->getParent()->removeChild(qOriginalNode);
        auto qNode = _questionTextField;
        
        if(qNode) {
            qNode->setOriginalText(_questions[1]);
            qNode->setString(QuestionHandler::wrapString(_questions[1], 40));
            qNode->setEnabled(false);
            qNode->setTouchEnabled(false);
            qNode->setFocusEnabled(false);
            qNode->setFontName(QuestionHandler::FONT_NAME);
            qNode->setTextColor(Color4B(QuestionHandler::FONT_COLOR));
            qNode->setFontSize(QuestionHandler::FONT_SIZE);
        }
        std::vector<std::string> buttons = {
            "Button_7",
            "Button_8",
            "Button_9",
            "Button_10"
        };
        std::random_shuffle ( buttons.begin(), buttons.end() );
        QuestionHandler::setButtonProperties(bg->getChildByName(buttons[0]), "1", _questions[2], CC_CALLBACK_2(MultipleChoice::buttonSelected, this), CC_CALLBACK_2(MultipleChoice::soundSelected, this));
        QuestionHandler::setButtonProperties(bg->getChildByName(buttons[1]), "0", _questions[3], CC_CALLBACK_2(MultipleChoice::buttonSelected, this), CC_CALLBACK_2(MultipleChoice::soundSelected, this));
        QuestionHandler::setButtonProperties(bg->getChildByName(buttons[2]), "0", _questions[4], CC_CALLBACK_2(MultipleChoice::buttonSelected, this), CC_CALLBACK_2(MultipleChoice::soundSelected, this));
        QuestionHandler::setButtonProperties(bg->getChildByName(buttons[3]), "0", _questions[5], CC_CALLBACK_2(MultipleChoice::buttonSelected, this), CC_CALLBACK_2(MultipleChoice::soundSelected, this));
        
#if defined(AUTO_CLICK) && (AUTO_CLICK > 0)
        runAction(Sequence::create(DelayTime::create(2.0), CallFunc::create([=]() {
            auto bg = this->getChildByName("bg");
            auto button = bg->getChildByName("1");
            this->buttonSelected(button, ui::Widget::TouchEventType::ENDED);
        }), NULL));
#endif
        
    }
    return true;
}

void MultipleChoice::soundSelected(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType) {
    ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    std::string word = clickedButton->getTitleText();    
    _qHandler->getMenuContext()->pronounceWord(word);
}

void MultipleChoice::buttonSelected(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType) {
    ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    cocostudio::ComExtensionData* data = (cocostudio::ComExtensionData*)clickedButton->getComponent("ComExtensionData");
    
    switch (eEventType) {
        case ui::Widget::TouchEventType::ENDED:
        {
            clickedButton->setEnabled(false);
            auto buttonName = clickedButton->getName();
            if(buttonName == "1") {
                _qHandler->getMenuContext()->addPoints(1);
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/sfx/success.ogg");
                auto soundButton = clickedButton->getParent()->getChildByName("sound_button");
                if(soundButton != nullptr) {
                    soundButton->removeFromParentAndCleanup(true);
                }
                clickedButton->runAction(Spawn::create(FadeOut::create(1.0f), NULL));
                runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([=]() {
                }), DelayTime::create(1.0f), CallFunc::create([=]() {
                    _qHandler->gotoNextQuestion(1);
                }), NULL));
            } else {
                clickedButton->runAction(FShake::actionWithDuration(1.0f, 10.0f));
                _qHandler->getMenuContext()->addPoints(-1);
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/sfx/error.ogg");
            }
            break;
        }
    }
    
}
