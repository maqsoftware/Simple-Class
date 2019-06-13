//
//  CommonText.cpp
//  goa
//
//  Created by Srikanth Talapadi on 17/12/2016.
//
//

#include "CommonText.h"
#include "../menu/MenuContext.h"

USING_NS_CC;

bool CommonText::touchSpeak(cocos2d::Touch* touch, cocos2d::Event* event) {
    if(!MenuContext::isGameStatic() && isVisible() && getOpacity() > 0) {
        auto n = getParent()->convertTouchToNodeSpace(touch);
        auto rect = this->getBoundingBox();
        if(rect.containsPoint(n))
        {
            auto scaleUp = ScaleTo::create(0.5, 1.5);
            auto elasticUp = EaseOut::create(scaleUp, 2.0);
            auto scaleDown = ScaleTo::create(0.5, 1.0);
            auto elasticDown = EaseIn::create(scaleDown, 2.0);
            runAction(Sequence::create(elasticUp, elasticDown, NULL));
            if(isInStory && !inStoryWhichFilename.empty())
            {
                if(MenuContext::_isInStoryDialogSpeechCurrentlyActive)
                {
                    MenuContext::pronounceSplitFileFromStory(inStoryWhichFilename);
                }
            } else {
                MenuContext::pronounceWord(this->getString());
            }
            
        }
    }
        return false;
}

void CommonText::onEnterTransitionDidFinish() {
    ui::Text::onEnterTransitionDidFinish();
    if(!isInStory)
    {
        auto scaleUp = ScaleTo::create(0.5, 1.1);
        auto elasticUp = EaseOut::create(scaleUp, 2.0);
        auto scaleDown = ScaleTo::create(0.5, 1.0);
        auto elasticDown = EaseIn::create(scaleDown, 2.0);
        runAction(Sequence::create(elasticUp, elasticDown, NULL));
        
    }
}

void CommonText::onExitTransitionDidStart() {
    ui::Text::onExitTransitionDidStart();
    _eventDispatcher->removeEventListener(_listener);
    _listener = nullptr;
}

bool CommonText::init() {
    if(ui::Text::init()) {
        _listener = EventListenerTouchOneByOne::create();
        _listener->onTouchBegan = CC_CALLBACK_2(CommonText::touchSpeak, this);
        _eventDispatcher->addEventListenerWithFixedPriority(_listener, -1);
        return true;
    }
    return false;
}

void CommonText::setCommonTextInStory(bool fromStory) {
    this->isInStory = fromStory;
}


void CommonText::setSplitFileNameInStory(const std::string fileName) {
    this->inStoryWhichFilename = fileName;
}


CommonText *CommonText::create() {
    CommonText *commonText = new (std::nothrow) CommonText();
    if(commonText && commonText->init()) {
        commonText->autorelease();
        return commonText;
    }
    CC_SAFE_DELETE(commonText);
    return nullptr;
}

CommonText::CommonText():
isInStory(false)
{
    
}

CommonText::~CommonText() {
    
}
