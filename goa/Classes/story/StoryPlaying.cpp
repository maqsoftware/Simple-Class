//
//  StoryPlaying.cpp
//  goa
//
//  Created by Shyamal.Upadhyaya on 07/01/17.
//
//

#include "StoryPlaying.hpp"
#include "../ext/util/lib/LTKStringUtil.h"
#include "../lang/TextGenerator.h"
#include "QuestionHandler.h"
#include "Managers/VoiceMoldManager.h" 
static const std::string STORY_JSON = ".storyJSON";
static const std::string SOUND_ENABLED_FOR_STORIES = ".soundEnabledForStories";

USING_NS_CC;
using namespace cocos2d::experimental;


Scene* StoryPlaying::createScene(int pageIndex, std::string storyId)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = StoryPlaying::create(pageIndex, storyId);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    //read StoryId from local Storage
    
    layer->_menuContext = MenuContext::create(layer, storyId);
    scene->addChild(layer->_menuContext);
    
    // return the scene
    
    return scene;
}

StoryPlaying* StoryPlaying::create(int pageIndex, std::string storyId)
{
    StoryPlaying* storyPlayingLayer = new (std::nothrow) StoryPlaying();
    if(storyPlayingLayer && storyPlayingLayer->init(pageIndex, storyId)) {
        storyPlayingLayer->autorelease();
        return storyPlayingLayer;
    }
    CC_SAFE_DELETE(storyPlayingLayer);
    return nullptr;
}


StoryPlaying::StoryPlaying():
_menuContext(nullptr),
_baseDir(""),
_isPlayEnded(false),
_isPlayStarted(false),
_nextButton(nullptr),
_prevButton(nullptr),
_totalStoryPages(0),
_offsetInY(0.0f),
_offsetInX(0.0f),
_isNodeDraggable(false),
_animationToPlayWhenTouched(""),
_originalSpriteColor(Color3B::GRAY),
_wordBubbleNode(nullptr),
_textDisplayAnimationRunning(false),
_pronouceWord(""),
_isTextShown(false),
_contentPageText(""),
_isAudionEngineInitialized(false),
_currentSplitWordIndex(0),
_totalSplitWords(0),
_splitSoundFilesDirectoryUrl(""),
_talkBubbleNode(nullptr),
_prevArmatureScale(1.0f),
_armatureScale(1.0f),
_armatureIndex(0),
_animationIndex(0),
_armature(nullptr),
_armatureDisplay(nullptr),
_isTextNodeDisplayTextAvailable(false),
_displayTextNode(nullptr)
{
}

StoryPlaying::~StoryPlaying() {
}

bool StoryPlaying::init(int pageIndex, std::string storyId)
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    this->_storyId = storyId;
    this->_pageIndex = pageIndex;
    this->load();
    
    return true;
}

void StoryPlaying::bindListenerToSkeletonNode(cocostudio::timeline::SkeletonNode* skeletonNode) {
    EventListenerTouchOneByOne* _listener = EventListenerTouchOneByOne::create();
    _listener->setSwallowTouches(true);
    _listener->onTouchBegan = CC_CALLBACK_2(StoryPlaying::onTouchBeganOnSkeleton, this);
    _listener->onTouchEnded = CC_CALLBACK_2(StoryPlaying::onTouchEndedOnSkeleton, this);
    _listener->onTouchMoved = CC_CALLBACK_2(StoryPlaying::onTouchMovedOnSkeleton, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, skeletonNode);
}


bool StoryPlaying::onTouchBeganOnSkeleton(Touch* touch, Event* event){
    if(this->_isPlayStarted || this->_isTextShown) {
        return false;
    }
    
    auto n = convertTouchToNodeSpace(touch);
    Node* target = event->getCurrentTarget();
    
    cocostudio::timeline::SkeletonNode* skeletonNode = dynamic_cast<cocostudio::timeline::SkeletonNode *>(target);
    if(skeletonNode != NULL && skeletonNode->getBoundingBox().containsPoint(n)) {
        auto tLocation = target->getParent()->convertToNodeSpace(touch->getLocation());
        _offsetInX = tLocation.x - skeletonNode->getPosition().x;
        _offsetInY = tLocation.y - skeletonNode->getPosition().y;
        _zOrder = target->getLocalZOrder();
        
        bindEventsToTarget(skeletonNode);
        
        return true;
    }
    return false;
}


void StoryPlaying::playAnimationOnNode(std::string animationName, Node* node) {
    Action* action = node->getActionManager()->getActionByTag(node->getTag(), node);
    
    if(action != NULL) {
        cocostudio::timeline::ActionTimeline* animationAction = animationAction = dynamic_cast<cocostudio::timeline::ActionTimeline *>(action);
        
        if(animationAction != NULL) {
            if(_isNodeDraggable) {
                animationAction->play(animationName, true);
            } else {
                animationAction->play(animationName, false);
            }
        }
    }
    
    std::string dbNodeStartsWith = "db_";
    if (node->getName().find(dbNodeStartsWith) != std::string::npos)
    {
        _changeAnimationTo(_animationToPlayWhenTouched);
    }
}

void StoryPlaying::onTouchMovedOnSkeleton(cocos2d::Touch *touch, cocos2d::Event *event) {
    if(_isNodeDraggable) {
        cocos2d::Node* target = event->getCurrentTarget();
        cocostudio::timeline::SkeletonNode* skeletonNode = dynamic_cast<cocostudio::timeline::SkeletonNode *>(target);
        if(skeletonNode != NULL) {
            skeletonNode->setLocalZOrder(1);
            auto mLocation = skeletonNode->getParent()->convertToNodeSpace(touch->getLocation());
            auto diffLocation = Vec2(mLocation.x - _offsetInX, mLocation.y - _offsetInY);
            skeletonNode->setPosition(diffLocation);
        }
    }
}


void StoryPlaying::onTouchEndedOnSkeleton(cocos2d::Touch *touch, cocos2d::Event *event) {
    cleanUpWhenTouchEnded(touch, event);
}

void StoryPlaying::cleanUpWhenTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
    _offsetInX = 0.0f;
    _offsetInY = 0.0f;
    _isNodeDraggable = false;
    
    cocos2d::Node* target = event->getCurrentTarget();
    target->setLocalZOrder(_zOrder);
    
    showText(target->getName());
    
    Action* action = target->getActionManager()->getActionByTag(target->getTag(), target);
    
    if(action != NULL) {
        cocostudio::timeline::ActionTimeline* animationAction = dynamic_cast<cocostudio::timeline::ActionTimeline *>(action);
        
        if(animationAction != NULL) {
            animationAction->pause();
            animationAction = NULL;
        }
    }
    
    if(_mainTimeLine != NULL) {
        _mainTimeLine->pause();
    }
    _animationToPlayWhenTouched = "";
    
    std::string dbNodeStartsWith = "db_";
    if (target->getName().find(dbNodeStartsWith) != std::string::npos)
    {
        _stopAnimationTo(_animationToPlayWhenTouched);
    }
    
    
    bool isSkeletonCharacter = dynamic_cast<cocostudio::timeline::SkeletonNode *>(target);
    if(isSkeletonCharacter) {
        cocostudio::timeline::SkeletonNode* sNode = dynamic_cast<cocostudio::timeline::SkeletonNode *>(target);
        changeBoneColor(sNode, true);
        skinColors.clear();
    }
    else if(target->getChildrenCount() > 0) {
        cocos2d::Vector<Node*> nodes = target->getChildren();
        for (std::vector<Node*>::iterator it = nodes.begin() ; it != nodes.end(); ++it) {
            Node* n = *it;
            if(skinColors.find(n->getName()) != skinColors.end()) {
                Color3B originalColor = skinColors.at(n->getName());
                n->setColor(originalColor);
            }
        }
    }
    else {
        target->setColor(_originalSpriteColor);
    }
}

void StoryPlaying::bindListenerToCompositeNode(Node* node) {
    EventListenerTouchOneByOne* _listener = EventListenerTouchOneByOne::create();
    _listener->setSwallowTouches(true);
    _listener->onTouchBegan = CC_CALLBACK_2(StoryPlaying::onTouchBeganOnComposite, this);
    _listener->onTouchEnded = CC_CALLBACK_2(StoryPlaying::onTouchEndedOnComposite, this);
    _listener->onTouchMoved = CC_CALLBACK_2(StoryPlaying::onTouchMovedOnComposite, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
}


bool StoryPlaying::onTouchBeganOnComposite(Touch* touch, Event* event){
    if(this->_isPlayStarted || this->_isTextShown) {
        return false;
    }
    
    Node* target = event->getCurrentTarget();
    auto n = convertTouchToNodeSpace(touch);
    
    auto boundingBox = utils::getCascadeBoundingBox(target);
    if(target != NULL && boundingBox.containsPoint(n)) {
        auto tLocation = target->getParent()->convertToNodeSpace(touch->getLocation());
        _offsetInX = tLocation.x - target->getPosition().x;
        _offsetInY = tLocation.y - target->getPosition().y;
        _zOrder = target->getLocalZOrder();
        
        bindEventsToTarget(target);
        
        return true;
    }
    
    return false;
}


void StoryPlaying::onTouchEndedOnComposite(cocos2d::Touch *touch, cocos2d::Event *event) {
    cleanUpWhenTouchEnded(touch, event);
}


void StoryPlaying::onTouchMovedOnComposite(cocos2d::Touch *touch, cocos2d::Event *event) {
    if(_isNodeDraggable) {
        cocos2d::Node* target = event->getCurrentTarget();
        if(target != NULL) {
            target->setLocalZOrder(1);
            auto mLocation = target->getParent()->convertToNodeSpace(touch->getLocation());
            auto diffLocation = Vec2(mLocation.x - _offsetInX, mLocation.y - _offsetInY);
            target->setPosition(diffLocation);
        }
    }
}


void StoryPlaying::bindListenerToNode(Node* node) {
    EventListenerTouchOneByOne* _listener = EventListenerTouchOneByOne::create();
    _listener->setSwallowTouches(true);
    _listener->onTouchBegan = CC_CALLBACK_2(StoryPlaying::onTouchBeganOnNode, this);
    _listener->onTouchEnded = CC_CALLBACK_2(StoryPlaying::onTouchEndedOnNode, this);
    _listener->onTouchMoved = CC_CALLBACK_2(StoryPlaying::onTouchMovedOnNode, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
}


bool StoryPlaying::onTouchBeganOnNode(Touch* touch, Event* event){
    if(this->_isPlayStarted || this->_isTextShown) {
        return false;
    }
    
    auto n = convertTouchToNodeSpace(touch);
    Node* target = event->getCurrentTarget();
    
    //check for pixel perfect touch if possible
    
    if(target->getBoundingBox().containsPoint(n)) {
        if(_nodesToTringlePointsMapping.find(target->getName()) != _nodesToTringlePointsMapping.end()) {
            auto tLocation = target->convertToNodeSpace(touch->getLocation());
            std::vector<std::vector<cocos2d::Point>> points = _nodesToTringlePointsMapping.at(target->getName());
            bool result = false;
            for (std::vector<std::vector<cocos2d::Point>>::iterator it = points.begin() ; it != points.end(); ++it) {
                std::vector<cocos2d::Point> v2 = *it;
                if(v2.size() == 3) {
                    Point p1 = v2.at(0);
                    Point p2 = v2.at(1);
                    Point p3 = v2.at(2);
                    if(pointInTriangle(tLocation, p1, p2, p3)) {
                        result = true;
                        break;
                    }
                }
            }
            return result;
        } else {
            _zOrder = target->getLocalZOrder();
            auto tLocation = target->getParent()->convertToNodeSpace(touch->getLocation());
            _offsetInX = tLocation.x - target->getPosition().x;
            _offsetInY = tLocation.y - target->getPosition().y;
            
            bindEventsToTarget(target);
            
            return true;
        }
    }
    return false;
}


void StoryPlaying::onTouchEndedOnNode(cocos2d::Touch *touch, cocos2d::Event *event) {
    cleanUpWhenTouchEnded(touch, event);
}


void StoryPlaying::onTouchMovedOnNode(cocos2d::Touch *touch, cocos2d::Event *event) {
    if(_isNodeDraggable) {
        cocos2d::Node* target = event->getCurrentTarget();
        if(target != NULL) {
            target->setLocalZOrder(1);
            auto mLocation = target->getParent()->convertToNodeSpace(touch->getLocation());
            auto diffLocation = Vec2(mLocation.x - _offsetInX, mLocation.y - _offsetInY);
            target->setPosition(diffLocation);
        }
    }
}

void StoryPlaying::bindEventsToTarget(Node* node) {
    cocostudio::ComExtensionData* data = (cocostudio::ComExtensionData*)node->getComponent("ComExtensionData");
    
    if(data != NULL && !data->getCustomProperty().empty()) {
        
        
        std::vector<std::string> contentPageInfo = _menuContext->split(data->getCustomProperty(), ';');
        for (std::vector<std::string>::iterator it = contentPageInfo.begin() ; it != contentPageInfo.end(); ++it) {
            std::string token = *it;
            LTKStringUtil::trimString(token);
            if(token.compare("drag") == 0) {
                _isNodeDraggable = true;
            } else {
                _animationToPlayWhenTouched = token;
            }
        }
        
        if(!_animationToPlayWhenTouched.empty()) {
            std::size_t shouldAnimationRunOnMainNode = _animationToPlayWhenTouched.find("main.");
            if (shouldAnimationRunOnMainNode != std::string::npos) {
                std::vector<std::string> mainAnimations = _menuContext->split(_animationToPlayWhenTouched, '.');
                if(mainAnimations.size() == 2) {
                    std::string animationName = mainAnimations.at(1);
                    _mainTimeLine->play(animationName, false);
                }
            } else {
                playAnimationOnNode(_animationToPlayWhenTouched, node);
            }
        }
    }
    
    bool isSkeletonCharacter = dynamic_cast<cocostudio::timeline::SkeletonNode *>(node);
    if(isSkeletonCharacter) {
        cocostudio::timeline::SkeletonNode* sNode = dynamic_cast<cocostudio::timeline::SkeletonNode *>(node);
        changeBoneColor(sNode, false);
    } else if(node->getChildrenCount() > 0) {
        cocos2d::Vector<Node*> nodes = node->getChildren();
        for (std::vector<Node*>::iterator it = nodes.begin() ; it != nodes.end(); ++it) {
            Node* n = *it;
            skinColors.insert(std::make_pair(n->getName(), n->getColor()));
            n->setColor(Color3B::GRAY);
        }
    }
    else {
        _originalSpriteColor = node->getColor();
        node->setColor(Color3B::GRAY);
    }
};

void StoryPlaying::changeBoneColor(cocostudio::timeline::BoneNode* skeleton, bool revert) {
    cocos2d::Vector<cocostudio::timeline::BoneNode*> bones = skeleton->getChildBones();
    for (std::vector<cocostudio::timeline::BoneNode*>::iterator it = bones.begin() ; it != bones.end(); ++it) {
        cocostudio::timeline::BoneNode* bone = *it;
        changeSkinColor(bone, revert);
        if(bone->getChildBones().size() > 0) {
            changeBoneColor(bone, revert);
        }
    }
}


void StoryPlaying::changeSkinColor(cocostudio::timeline::BoneNode* bone, bool revert) {
    cocos2d::Vector<cocostudio::timeline::SkinNode*> skins = bone->getSkins();
    for (std::vector<cocostudio::timeline::SkinNode*>::iterator it = skins.begin() ; it != skins.end(); ++it) {
        cocostudio::timeline::SkinNode* skin = *it;
        if(revert) {
            if(skinColors.find(skin->getName()) != skinColors.end()) {
                Color3B originalColor = skinColors.at(skin->getName());
                skin->setColor(originalColor);
            }
        } else {
            skinColors.insert(std::make_pair(skin->getName(), skin->getColor()));
            skin->setColor(Color3B::GRAY);
        }
    }
}

void StoryPlaying::processScene(Node* parent) {
    if(parent != NULL) {
        auto children = parent->getChildren();
        for (std::vector<Node*>::iterator it = children.begin() ; it != children.end(); ++it) {
            cocos2d::Node* node = *it;
            
            std::size_t panelFound = node->getName().find("Panel");
            if (panelFound != std::string::npos) {
                //ignore
            } else {
                //process skeleton and non-skeleton nodes
                bool isSkeletonCharacter = dynamic_cast<cocostudio::timeline::SkeletonNode *>(node);
                if(isSkeletonCharacter) {
                    cocostudio::timeline::SkeletonNode* skeletonNode = dynamic_cast<cocostudio::timeline::SkeletonNode *>(node);
                    if(skeletonNode != NULL) {
                        
                        bindListenerToSkeletonNode(skeletonNode);
                        
                        Action* action = skeletonNode->getActionManager()->getActionByTag(skeletonNode->getTag(), skeletonNode);
                        
                        if(action != NULL) {
                            cocostudio::timeline::ActionTimeline* skeletonAction = dynamic_cast<cocostudio::timeline::ActionTimeline *>(action);
                            if(skeletonAction != NULL) {
                                skeletonAction->setFrameEventCallFunc(CC_CALLBACK_1(StoryPlaying::onFrameEvent, this));
                            }
                        }
                    }
                    
                } else {
                    if(node->getChildrenCount() > 0) {
                        bindListenerToCompositeNode(node);
                    } else {
                        bindListenerToNode(node);
                    }
                }
                
            }
        }
        
    }
}

void StoryPlaying::load() {
    
    if(AudioEngine::lazyInit())
    {
        _isAudionEngineInitialized = true;
    }else
    {
        _isAudionEngineInitialized = false;
    }
    
    std::string data;
    localStorageGetItem(STORY_JSON, &data);
    
    int storyIndex = atoi(data.c_str());
    
    std::string contents = FileUtils::getInstance()->getStringFromFile("misc/shelfConfig.json");
    
    rapidjson::Document d;
    
    if (!d.Parse<0>(contents.c_str()).HasParseError()) {
        const rapidjson::Value& storyConfigs = d["stories"];
        assert(storyConfigs.IsArray());
        const rapidjson::Value& story = storyConfigs[storyIndex];
        const rapidjson::Value& dsStoryPages = story["pages"];
        
        _totalStoryPages = dsStoryPages.Size();
        const std::string contentPageName = dsStoryPages[this->_pageIndex]["contentJson"].GetString();
        std::vector<std::string> contentPageInfo = _menuContext->split(contentPageName, '/');
        if(contentPageInfo.size() > 0) {
            _baseDir = contentPageInfo.at(0);
        }
        
        loadContentPage(contentPageName);
        
        loadTimings();
        
        processScene(_contentPageNode);
        
        processPixelPerfectNodes(_contentPageNode);
        
        createNextAndPreviousButtons();
        
        createWordBubble();
        
        loadContentPageText();
        
        preloadAllAudio();
        
        playMasterAnimation();
        
    }
}

void StoryPlaying::createDragonBoneNode(Node* parentNode, std::string dragonBoneName) {
    
    std::string skeletonFile = dragonBoneName + "/" +  dragonBoneName + "_ske.json";
    std::string texFile = dragonBoneName + "/" + dragonBoneName + "_tex.json";
    _dragonBonesData = _factory.loadDragonBonesData(skeletonFile);
    _factory.loadTextureAtlasData(texFile);
    
    if (_dragonBonesData)
    {
        _changeArmature(parentNode);
        _changeAnimation();
        
        EventListenerTouchOneByOne* _listener = cocos2d::EventListenerTouchOneByOne::create();
        _listener->setSwallowTouches(true);
        _listener->onTouchBegan = CC_CALLBACK_2(StoryPlaying::onTouchBeganOnComposite, this);
        _listener->onTouchEnded = CC_CALLBACK_2(StoryPlaying::onTouchEndedOnComposite, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, _armatureDisplay->getParent());
    }
}

std::vector<float> StoryPlaying::splitFloat(std::string s, char delim)
{
    std::vector<float> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (getline(ss, item, delim)) {
        float i = std::atof(item.c_str());
        elems.push_back(i);
    }
    return elems;
}


void StoryPlaying::loadTimings() {
    std::string timeFileUrl = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + "_timing" + ".json";
    if(!timeFileUrl.empty() && FileUtils::getInstance()->isFileExist(timeFileUrl))
    {
        std::string jsonData = FileUtils::getInstance()->getStringFromFile(timeFileUrl);
        rapidjson::Document coverPageTextDocument;
        if (!coverPageTextDocument.Parse<0>(jsonData.c_str()).HasParseError()) {
            // document is ok
            rapidjson::Value::MemberIterator jsonIterator;
            const char *key,*value;
            int i = 0;
            for (jsonIterator=coverPageTextDocument.MemberBegin(); jsonIterator!=coverPageTextDocument.MemberEnd(); jsonIterator++)
            {
                key   = jsonIterator->name.GetString();
                value = jsonIterator->value.GetString();
                std::string sValue = value;
                
                if(i == _pageIndex + 1) {
                    _contentPageText = value;
                    
                    if (!_contentPageText.empty() && _contentPageText[_contentPageText.length()-1] == '\n') {
                        _contentPageText.erase(_contentPageText.length()-1);
                    }
                    
                    _loadedSplitWordsTimings = this->splitFloat(_contentPageText, ',');
                    break;
                }
                i++;
            }
        }
    }
}


void StoryPlaying::preloadAllAudio() {
    _loadedSplitWordsEffects.clear();
    std::string pageI = MenuContext::to_string(_pageIndex + 1);
    _splitSoundFilesDirectoryUrl = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + "/" + pageI;
    
    if(!_splitSoundFilesDirectoryUrl.empty())
    {
        int prefix = 0;
        std::string _splitFile = "";
        for (int i=0; i<_individualTextsTokens.size(); i++)
        {
            if(i < 10) {
                _splitFile = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + "/" + pageI + "/"  +  _baseDir  + "_" + pageI + "-" + MenuContext::to_string(prefix) + MenuContext::to_string(i) +".ogg";
            } else {
                _splitFile = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + "/" + pageI + "/"  +  _baseDir  + "_" + pageI + "-" +  MenuContext::to_string(i) +".ogg";
                
            }
            
            
            if(!_splitFile.empty() && FileUtils::getInstance()->isFileExist(_splitFile)) {
                _loadedSplitWordsEffects.push_back(_splitFile);
            }
        }
        
        for (std::vector<std::string>::iterator it = _loadedSplitWordsEffects.begin() ; it != _loadedSplitWordsEffects.end(); ++it)
        {
            std::string effect = (std::string) *it;
            if(_isAudionEngineInitialized) {
                if(!effect.empty()) {
                    AudioEngine::preload(effect.c_str());
                }
            }
        }
    }
}



void StoryPlaying::createWordBubble() {
    
    std::string mappingFileUrl = "story/eng/" + _baseDir + ".mapping.json";
    if(!mappingFileUrl.empty() && FileUtils::getInstance()->isFileExist(mappingFileUrl))
    {
        std::string jsonData = FileUtils::getInstance()->getStringFromFile(mappingFileUrl);
        
        rapidjson::Document mappingDocument;
        if (!mappingDocument.Parse<0>(jsonData.c_str()).HasParseError()) {
            // document is ok
            rapidjson::Value::MemberIterator jsonIterator;
            const char *key,*value;
            for (jsonIterator=mappingDocument.MemberBegin(); jsonIterator!=mappingDocument.MemberEnd(); jsonIterator++)
            {
                key   = jsonIterator->name.GetString();
                value = jsonIterator->value.GetString();
                std::string sValue = value;
                
                _wordMappings.insert({key,sValue});
            }
        }
        
        Size visibleSize = Director::getInstance()->getVisibleSize();
        _wordBubbleNode = CSLoader::createNode("template/hang_bubble.csb");
        _wordBubbleNode->setPosition(Vec2(visibleSize.width/2, visibleSize.height + 650));
        _wordBubbleNode->setVisible(false);
        
        Node* soundWordNode = _wordBubbleNode->getChildByName(SOUND_BUTTON_WORD);
        
        if(soundWordNode != NULL) {
            cocos2d::ui::Button* soundWordButton = dynamic_cast<cocos2d::ui::Button *>(soundWordNode);
            if(soundWordButton != NULL) {
                soundWordButton->setTitleText("");
                soundWordButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::pronounceWord, this));
            }
        }
        
        this->addChild(_wordBubbleNode, 2);
    }
}

void StoryPlaying::loadContentPageText() {
    _jsonFile = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + ".json";
    if (!_jsonFile.empty() && FileUtils::getInstance()->isFileExist(_jsonFile))
    {
        std::string jsonData = FileUtils::getInstance()->getStringFromFile(_jsonFile);
        rapidjson::Document coverPageTextDocument;
        if (!coverPageTextDocument.Parse<0>(jsonData.c_str()).HasParseError()) {
            // document is ok
            rapidjson::Value::MemberIterator jsonIterator;
            const char *key,*value;
            int i = 0;
            for (jsonIterator=coverPageTextDocument.MemberBegin(); jsonIterator!=coverPageTextDocument.MemberEnd(); jsonIterator++)
            {
                key   = jsonIterator->name.GetString();
                value = jsonIterator->value.GetString();
                std::string sValue = value;
                
                if(i == _pageIndex + 1) {
                    _contentPageText = value;
                    
                    if (!_contentPageText.empty() && _contentPageText[_contentPageText.length()-1] == '\n') {
                        _contentPageText.erase(_contentPageText.length()-1);
                    }
                    
                    _individualTextsTokens = _menuContext->splitRegEx(_contentPageText, "\\s+");
                    break;
                }
                i++;
            }
        }
    }
}

void StoryPlaying::renderTextAndPlayDialog(Node* parentNode, Node* storyTextNode) {
    renderStoryText(parentNode, storyTextNode);
    std::string pageI = MenuContext::to_string(_pageIndex + 1);
    _jsonFile = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + ".json";
    std::string jsonData = FileUtils::getInstance()->getStringFromFile(_jsonFile);
    rapidjson::Document coverPageTextDocument;
    if (!coverPageTextDocument.Parse<0>(jsonData.c_str()).HasParseError()){
        // document is ok
        rapidjson::Value::MemberIterator jsonIterator;
        const char *key, *value;
        int i = 0;
        for (jsonIterator = coverPageTextDocument.MemberBegin(); jsonIterator != coverPageTextDocument.MemberEnd(); jsonIterator++)
        {
            key = jsonIterator->name.GetString();
            value = jsonIterator->value.GetString();
            std::string sValue = value;
            if (i == _pageIndex + 1)
            {
                std::string _contentPageText = value;

                bool isSplitWordsEffectedLoaded = _loadedSplitWordsEffects.size() > 0;

                if (_soundEnabled.compare("true") == 0 && (isSplitWordsEffectedLoaded || !_contentPageText.empty()))
                {
                    VoiceMoldManager::shared()->speak(_contentPageText);
                }
                break;
            }
            i++;
        }
    }
}


void StoryPlaying::highlightedNarrateWord(float dt) {
    if(_loadedSplitWordsEffects.size() >0 && _contentCommonTextTokens.size() > 0)
    {
        MenuContext::_isInStoryDialogSpeechCurrentlyActive = true;
    }
    
    if(_loadedSplitWordsTimings.size() > 0)
    {
        _currentSplitWordIndex = 0;
        _totalSplitTimings = _loadedSplitWordsTimings.size();
        
        
        if(_currentSplitWordIndex < _loadedSplitWordsTimings.size() &&
           _currentSplitWordIndex < _totalSplitTimings &&
           _currentSplitWordIndex < _contentCommonTextTokens.size()
           )
        {
            float time = _loadedSplitWordsTimings.at(_currentSplitWordIndex);
            highlightedNWord = _contentCommonTextTokens.at(_currentSplitWordIndex);
            this->highlightWord(time, highlightedNWord);
        }
    }
}

void StoryPlaying::unhighlightText(float dt) {
    this->unschedule(schedule_selector(StoryPlaying::unhighlightText));
    
    if(_currentSplitWordIndex <= _contentCommonTextTokens.size())
    {
        preHighlightedNWord = _contentCommonTextTokens.at(_currentSplitWordIndex - 1);
        preHighlightedNWord->setTextColor(Color4B::BLACK);
    }
    
    if(_currentSplitWordIndex < _loadedSplitWordsTimings.size() &&
       _currentSplitWordIndex < _totalSplitTimings &&
       _currentSplitWordIndex < _contentCommonTextTokens.size() &&
       MenuContext::_isInStoryDialogSpeechCurrentlyActive)
    {
        float time = _loadedSplitWordsTimings.at(_currentSplitWordIndex);
        highlightedNWord = _contentCommonTextTokens.at(_currentSplitWordIndex);
        this->highlightWord(time, highlightedNWord);
    }
}

void StoryPlaying::highlightWord(float time, cocos2d::ui::Text* text) {
    
    text->setTextColor(Color4B::BLUE);
    _currentSplitWordIndex++;
    this->schedule(schedule_selector(StoryPlaying::unhighlightText), time);
}




void StoryPlaying::createDialogBubble() {
    if(!_contentPageText.empty())
    {
        _talkBubbleNode = CSLoader::createNode("template/bubble_tem.csb");
        this->addChild(_talkBubbleNode, 1);
        
        Node* closeNode = _talkBubbleNode->getChildByName(CLOSE_BUTTON);
        if(closeNode != NULL) {
            cocos2d::ui::Button* closeButton = dynamic_cast<cocos2d::ui::Button *>(closeNode);
            if(closeButton != NULL) {
                closeButton->setTitleText("");
                closeButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::closeDialog, this));
#if defined(AUTO_CLICK) && (AUTO_CLICK > 0)
                runAction(Sequence::create(DelayTime::create(10.0), CallFunc::create([=]() {
                    this->closeDialog(closeButton, ui::Widget::TouchEventType::ENDED);
                }), NULL));
                
#endif
                
            }
        }
        
        
        Node* soundNode = _talkBubbleNode->getChildByName(SOUND_BUTTON);
        if(soundNode != NULL) {
            cocos2d::ui::Button* soundButton = dynamic_cast<cocos2d::ui::Button *>(soundNode);
            if(soundButton != NULL) {
                soundButton->setTitleText("");
                soundButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::playSound, this));
                
                if(_soundEnabled.compare("true") == 0) {
                    soundButton->setHighlighted(true);
                } else {
                    soundButton->setHighlighted(false);
                }
            }
        }
        
        Node* chooseText = _talkBubbleNode->getChildByName(STORY_TEXT);        
        
        renderTextAndPlayDialog(_talkBubbleNode, chooseText);
        
        if(chooseText != NULL) {
            cocos2d::ui::TextField* chooseLabel = dynamic_cast<cocos2d::ui::TextField *>(chooseText);
            if(chooseLabel != NULL) {
                chooseLabel->setTouchEnabled(false);
                chooseLabel->setString("");
                chooseLabel->setPlaceHolder("");
            }
        }
    } else {
        _nextButton->setEnabled(true);
        _nextButton->setVisible(true);
        if(_pageIndex != 0) {
            _prevButton->setEnabled(true);
            _prevButton->setVisible(true);
        }
        
    }
}


void StoryPlaying::positionTextNode(CommonText* textNode, Node* storyTextNode, float currentNodeX, float currentNodeY) {
    float xPos = 0.0f;
    float yPos = 0.0f;
    if(currentNodeX == 0.0f) {
        xPos = storyTextNode->getPosition().x - storyTextNode->getBoundingBox().size.width/2 + 50.0f;
    } else {
        xPos = currentNodeX;
    }
    
    if(currentNodeY == 0.0f) {
        yPos = storyTextNode->getPosition().y + storyTextNode->getBoundingBox().size.height/2 - 50.0f;
    } else {
        yPos = currentNodeY;
    }
    
    textNode->setPosition(Vec2(xPos , yPos));
}

void StoryPlaying::renderStoryText(Node* parentNode, Node* storyTextNode) {
    _isTextShown = true;
    std::vector<std::string> individualTexts = _individualTextsTokens;
    Vec2 lastRenderedLabelPosition = Vec2(0.0f,0.0f);
    Node* lastRenderedLabel;
    bool firstPassFinished = false;
    bool newLineCharRecognized = false;
    float yOffset = 0;
    int howManyTimes = 0;
    int whichToken = 0;
    float initialX = 0.0f;
    for (std::vector<std::string>::iterator it = individualTexts.begin() ; it != individualTexts.end(); ++it) {
        std::string token = *it;
        newLineCharRecognized = false;
        std::string newLineChar = "\n";
        if (token.find(newLineChar) != std::string::npos)
        {
            newLineCharRecognized = true;
            std::string::size_type i = token.find(newLineChar);
            if (i != std::string::npos) {
                token.erase(i, newLineChar.length());
            }
        }
        //create CommonText node
        auto label = CommonText::create();
        label->setCommonTextInStory(true);
        if(whichToken < _loadedSplitWordsEffects.size())
        {
            std::string audioFile = _loadedSplitWordsEffects.at(whichToken);
            label->setSplitFileNameInStory(audioFile);
        }
        
        whichToken++;
        label->setString(token);
        label->setFontSize(75);
        label->setFontName(LangUtil::getInstance()->getFontFile());
        label->setTextColor(Color4B::BLACK);
        if(firstPassFinished)
        {
            if(newLineCharRecognized || lastRenderedLabel->getPosition().x + lastRenderedLabel->getBoundingBox().size.width/2 + label->getBoundingBox().size.width > parentNode->getBoundingBox().size.width - 250.0f)
            {
                //wrap
                howManyTimes++;
                float adjustedY = yOffset - (howManyTimes * lastRenderedLabel->getBoundingBox().size.height);
                //float initialX = storyTextNode->getPosition().x - storyTextNode->getBoundingBox().size.width/2 + 100.0f;
                lastRenderedLabelPosition = Vec2(initialX + label->getBoundingBox().size.width/2, adjustedY);
            } else {
                token = ' ' + token;
                label->setString(token);
                lastRenderedLabelPosition = Vec2(lastRenderedLabel->getPosition().x + lastRenderedLabel->getBoundingBox().size.width/2 + label->getBoundingBox().size.width/2, lastRenderedLabel->getPosition().y);
            }
            
            positionTextNode(label, storyTextNode, lastRenderedLabelPosition.x, lastRenderedLabelPosition.y);
        } else {
            positionTextNode(label, storyTextNode, 0.0f, 0.0f);
            yOffset = label->getPosition().y;
        }
        
        lastRenderedLabel = label;
        firstPassFinished = true;
        if(initialX == 0.0f) {
            initialX = label->getPosition().x;
            label->setPositionX(label->getPositionX() + initialX/2);
        }
        
        _contentCommonTextTokens.push_back(label);
        parentNode->addChild(label);
    }
}


void StoryPlaying::playNextSplitWordCallBack(int id, const std::string& file) {
    
    AudioEngine::stop(id);
    CommonText* preHighlighteWord = _contentCommonTextTokens.at(_currentSplitWordIndex - 1);
    //unhighlited word
    preHighlighteWord->setTextColor(Color4B::BLACK);
    
    if(_currentSplitWordIndex < _totalSplitWords &&
       _currentSplitWordIndex < _loadedSplitWordsEffects.size() &&
       _currentSplitWordIndex < _contentCommonTextTokens.size())
    {
        std::string audioFile = _loadedSplitWordsEffects.at(_currentSplitWordIndex);
        CommonText* highlighteWord = _contentCommonTextTokens.at(_currentSplitWordIndex);
        StoryPlaying::playSplitAudio(audioFile, highlighteWord);
    } else {
        if(_isTextNodeDisplayTextAvailable) {
            enableTouchAndDisableTextShown();
        }
    }
}


void StoryPlaying::playSplitAudio(std::string audioFile, CommonText* text) {
    
    if(!audioFile.empty() && FileUtils::getInstance()->isFileExist(audioFile))
    {
        text->setTextColor(Color4B::BLUE);
        auto musicId = cocos2d::experimental::AudioEngine::play2d(audioFile);
        cocos2d::experimental::AudioEngine::setVolume(musicId, 1.0f);
        _currentSplitWordIndex++;
        cocos2d::experimental::AudioEngine::setFinishCallback(musicId,CC_CALLBACK_2(StoryPlaying::playNextSplitWordCallBack, this));
    } else {
        if(_isTextNodeDisplayTextAvailable) {
            enableTouchAndDisableTextShown();
        }
    }
}

void StoryPlaying::highlightedNarrateDialog(float dt) {
    if(_loadedSplitWordsEffects.size() >0 && _contentCommonTextTokens.size() > 0)
    {
        MenuContext::_isInStoryDialogSpeechCurrentlyActive = true;
        _currentSplitWordIndex = 0;
        _totalSplitWords = _loadedSplitWordsEffects.size();
        
        
        if(_currentSplitWordIndex < _loadedSplitWordsEffects.size() &&
           _currentSplitWordIndex < _contentCommonTextTokens.size() &&
           _currentSplitWordIndex < _totalSplitWords)
        {
            std::string audioFile = _loadedSplitWordsEffects.at(_currentSplitWordIndex);
            CommonText* highlightedWord = _contentCommonTextTokens.at(_currentSplitWordIndex);
            StoryPlaying::playSplitAudio(audioFile, highlightedWord);
        }
    }
    
}

void StoryPlaying::narrateDialog(float dt) {
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(1.0f);
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_soundFile.c_str(), false);
}

void StoryPlaying::playSound(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            
            if(_soundEnabled.compare("true") == 0) {
                _soundEnabled = "false";
                clickedButton->setHighlighted(false);
                localStorageSetItem(SOUND_ENABLED_FOR_STORIES, _soundEnabled);
                
                if(CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
                {
                    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
                }
                
            } else {
                _soundEnabled = "true";
                clickedButton->setHighlighted(true);
                localStorageSetItem(SOUND_ENABLED_FOR_STORIES, _soundEnabled);
                
                std::string pageI = MenuContext::to_string(_pageIndex + 1);
                _jsonFile = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + ".json";
                std::string jsonData = FileUtils::getInstance()->getStringFromFile(_jsonFile);
                rapidjson::Document coverPageTextDocument;
                if (!coverPageTextDocument.Parse<0>(jsonData.c_str()).HasParseError())
                {
                    // document is ok
                    rapidjson::Value::MemberIterator jsonIterator;
                    const char *key, *value;
                    int i = 0;
                    for (jsonIterator = coverPageTextDocument.MemberBegin(); jsonIterator != coverPageTextDocument.MemberEnd(); jsonIterator++)
                    {
                        key = jsonIterator->name.GetString();
                        value = jsonIterator->value.GetString();
                        std::string sValue = value;
                        if (i == _pageIndex + 1)
                        {
                            std::string _contentPageText = value;

                            bool isSplitWordsEffectedLoaded = _loadedSplitWordsEffects.size() > 0;

                            if (_soundEnabled.compare("true") == 0 && (isSplitWordsEffectedLoaded || !_contentPageText.empty()))
                            {
                                VoiceMoldManager::shared()->speak(_contentPageText);
                            }
                            break;
                        }
                        i++;
                    }
                }
            }
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}

void StoryPlaying::enableTouchAndDisableTextShown() {
    _isTextShown = false;
    _nextButton->setEnabled(true);
    _nextButton->setVisible(true);
    if(_pageIndex != 0) {
        _prevButton->setEnabled(true);
        _prevButton->setVisible(true);
    }
    MenuContext::_isInStoryDialogSpeechCurrentlyActive = false;
}

void StoryPlaying::closeDialog(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            clickedButton->setHighlighted(true);
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            if(CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
            {
                CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            }
            
            clickedButton->setEnabled(false);
            _talkBubbleNode->setVisible(false);
            _showAgainTextButton->setEnabled(true);
            _showAgainTextButton->setVisible(true);
            enableTouchAndDisableTextShown();
            break;
        }
            
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}

void StoryPlaying::playEnded() {
    _isPlayEnded = true;
    _isPlayStarted = false;
    if(!_isTextNodeDisplayTextAvailable) {
        createDialogBubble();
    } else {
        if(_displayTextNode != NULL)
        {
            renderTextAndPlayDialog(this, _displayTextNode);
        }
        if(_loadedSplitWordsEffects.size() == 0 || _soundEnabled.compare("true") != 0) {
            enableTouchAndDisableTextShown();
        }
    }
    
}

void StoryPlaying::createNextAndPreviousButtons() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    _nextButton = cocos2d::ui::Button::create("template/template_02/side_arrow.png", "template/template_02/click_side_arrow.png", "template/template_02/side_arrow.png", cocos2d::ui::Widget::TextureResType::PLIST);
    _nextButton->setPosition(Vec2(visibleSize.width - 200.0, visibleSize.height/2));
    _nextButton->setName(NEXT_BUTTON);
    _nextButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::nextStory, this));
    this->addChild(_nextButton, 2);
    _nextButton->setEnabled(false);
    _nextButton->setVisible(false);
    
    
    if(_pageIndex != 0) {
        _prevButton = cocos2d::ui::Button::create("template/template_02/side_arrow.png", "template/template_02/click_side_arrow.png", "template/template_02/side_arrow.png", cocos2d::ui::Widget::TextureResType::PLIST);
        _prevButton->setPosition(Vec2(200.0, visibleSize.height/2));
        _prevButton->setName(PREVIOUS_BUTTON);
        _prevButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::previousStory, this));
        this->addChild(_prevButton, 2);
        _prevButton->setFlippedX(true);
        _prevButton->setEnabled(false);
        _prevButton->setVisible(false);
    }
    
    
    _showAgainTextButton = cocos2d::ui::Button::create("template/template_02/text_button.png", "template/template_02/click_side_arrow.png", "template/template_02/text_button_clicked.png", cocos2d::ui::Widget::TextureResType::PLIST);
    _showAgainTextButton->setPosition(Vec2(320.0, visibleSize.height - 150));
    _showAgainTextButton->setName(SHOW_TEXT_AGAIN_BUTTON);
    _showAgainTextButton->addTouchEventListener(CC_CALLBACK_2(StoryPlaying::showTextAgain, this));
    this->addChild(_showAgainTextButton, 2);
    _showAgainTextButton->setEnabled(false);
    _showAgainTextButton->setVisible(false);
    
    
}


void StoryPlaying::onFrameEvent(cocostudio::timeline::Frame* frame) {
    if(!_isPlayEnded) {
        cocostudio::timeline::EventFrame* eventFrame = dynamic_cast<cocostudio::timeline::EventFrame *>(frame);
        if(eventFrame != NULL) {
            Node* node = frame->getNode();
            std::string event = eventFrame->getEvent();
            this->playFrameEventEffect(event);
        }
    }
}

void StoryPlaying::playFrameEventEffect(std::string eventData) {
    if(!eventData.empty()) {
        std::string eventSoundFile = "sounds/sfx/" +  eventData + ".ogg";
        if(!eventSoundFile.empty() && FileUtils::getInstance()->isFileExist(eventSoundFile)) {
            _loadedEffects.push_back(eventSoundFile);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(eventSoundFile.c_str(), false);
        }
    }
}


void StoryPlaying::processPixelPerfectNodes(Node* parent) {
    std::string pixelPerfectMappingUrl = "misc/pixelPerfectConfig.json";
    if(!pixelPerfectMappingUrl.empty() && FileUtils::getInstance()->isFileExist(pixelPerfectMappingUrl))
    {
        std::string jsonData = FileUtils::getInstance()->getStringFromFile(pixelPerfectMappingUrl);
        rapidjson::Document mappingDocument;
        if (!mappingDocument.Parse<0>(jsonData.c_str()).HasParseError()) {
            rapidjson::Value::MemberIterator jsonIterator;
            const char *key;
            for (jsonIterator=mappingDocument.MemberBegin(); jsonIterator!=mappingDocument.MemberEnd(); jsonIterator++)
            {
                key   = jsonIterator->name.GetString();
                const rapidjson::Value& mappings = mappingDocument[key];
                
                for (rapidjson::Value::ConstMemberIterator itr = mappings.MemberBegin();
                     itr != mappings.MemberEnd(); ++itr)
                {
                    _pixelPerfectMapping.insert({itr->name.GetString(), itr->value.GetString()});
                }
                
            }
        }
    }
    
    
    cocos2d::Vector<Node*> nodes = parent->getChildren();
    for (std::vector<Node*>::iterator it = nodes.begin() ; it != nodes.end(); ++it) {
        Node* n = *it;
        //if name starts with db_* then it is Dragon Bone Parent Node
        std::string dbNodeStartsWith = "db_";
        if (n->getName().find(dbNodeStartsWith) != std::string::npos)
        {
            std::string dbNodeName = n->getName();
            std::string::size_type i = dbNodeName.find(dbNodeStartsWith);
            this->createDragonBoneNode(n, dbNodeName);
            
        }
        
        std::string dbDisplayText = "display_text";
        if (n->getName().find(dbDisplayText) != std::string::npos)
        {
            std::string displayTextNodeName = n->getName();
            _displayTextNode = dynamic_cast<cocos2d::ui::TextField *>(n);
            if(_displayTextNode != NULL) {
                _displayTextNode->setTouchEnabled(false);
                _displayTextNode->setString("");
                _displayTextNode->setPlaceHolder("");
                _displayTextNode->setFontSize(100);
                _displayTextNode->setTextHorizontalAlignment(TextHAlignment::CENTER);
                _displayTextNode->setTextVerticalAlignment(TextVAlignment::CENTER);
                _displayTextNode->setFontName(LangUtil::getInstance()->getFontFile());
                _displayTextNode->setTextColor(Color4B::BLACK);
            }
            
            _isTextNodeDisplayTextAvailable = true;
        }
        
        if(_pixelPerfectMapping.find(n->getName()) != _pixelPerfectMapping.end()) {
            std::string spriteUrl = _pixelPerfectMapping.at(n->getName());
            Sprite* sprite = dynamic_cast<Sprite *>(n);
            if(sprite != NULL)
            {
                std::vector<std::vector<cocos2d::Point>> points = _menuContext->getTrianglePointsForSprite(sprite, spriteUrl, 0.0f);
                
                _nodesToTringlePointsMapping.insert({n->getName(), points});
            }
        }
    }
}

bool StoryPlaying::pointInTriangle(Point p, Point p0, Point p1, Point p2) {
    float A = 1/2 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    int sign = A < 0 ? -1 : 1;
    float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
    float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;
    
    return s > 0 && t > 0 && (s + t) < 2 * A * sign;
    
}

void StoryPlaying::playMasterAnimation() {
    if(_mainTimeLine && _mainTimeLine->getDuration() == 0) {
        playEnded();
    }
    else if(_mainTimeLine && _mainTimeLine->IsAnimationInfoExists("master")) {
        //play master animation
        _mainTimeLine->setAnimationEndCallFunc("master", CC_CALLBACK_0(StoryPlaying::playEnded, this));
        _mainTimeLine->setFrameEventCallFunc(CC_CALLBACK_1(StoryPlaying::onFrameEvent, this));
        _mainTimeLine->play("master", false);
        _isPlayStarted = true;
        _isPlayEnded = false;
    } else {
        playEnded();
    }
    
}

void StoryPlaying::loadContentPage(std::string contentPageName) {
    _contentPageNode = CSLoader::createNode(contentPageName);
    
    _mainTimeLine = CSLoader::createTimeline(contentPageName);
    _mainTimeLine->retain();
    _contentPageNode->runAction(_mainTimeLine);
    
    this->addChild(_contentPageNode);
    //find sound configuration
    
    _soundEnabled = "";
    localStorageGetItem(SOUND_ENABLED_FOR_STORIES, &_soundEnabled);
}


void StoryPlaying::onExitTransitionDidStart() {
    _mainTimeLine->release();
    _nodesToTringlePointsMapping.clear();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    for (std::vector<std::string>::iterator it = _loadedEffects.begin() ; it != _loadedEffects.end(); ++it)
    {
        std::string effect = (std::string) *it;
        if(!effect.empty()) {
            CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(effect.c_str());
        }
    }
    
    
    if(_isAudionEngineInitialized)
    {
        AudioEngine::uncacheAll();
    }
    
    
    if(!_contentCommonTextTokens.empty())
    {
        for (std::vector<CommonText*>::iterator it = _contentCommonTextTokens.begin() ; it != _contentCommonTextTokens.end(); ++it) {
            CommonText* nCommonText = *it;
            nCommonText->removeFromParentAndCleanup(true);
        }
        _contentCommonTextTokens.clear();
    }
    
    if(_talkBubbleNode != NULL)
    {
        _talkBubbleNode->removeFromParentAndCleanup(true);
    }
}

void StoryPlaying::onEnterTransitionDidFinish() {
    Node::onEnterTransitionDidFinish();
}

void StoryPlaying::nextStory(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            clickedButton->setEnabled(false);
            clickedButton->setVisible(false);
            if(_pageIndex + 1 != _totalStoryPages) {
                Director::getInstance()->replaceScene(TransitionFade::create(1.0, StoryPlaying::createScene(_pageIndex + 1, _storyId), Color3B::BLACK));
            } else {
                Director::getInstance()->replaceScene(TransitionFade::create(1.0, QuestionHandler::createScene(_storyId, _baseDir), Color3B::BLACK));
            }
            break;
        }
            
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
    
}

void StoryPlaying::previousStory(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            clickedButton->setHighlighted(true);
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            clickedButton->setEnabled(false);
            clickedButton->setVisible(false);
            if(_pageIndex > 0) {
                Director::getInstance()->replaceScene(TransitionFade::create(1.0, StoryPlaying::createScene(_pageIndex - 1, _storyId), Color3B::BLACK));
            }
            
            break;
        }
            
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}


void StoryPlaying::showText( std::string const& nodeName) {
    if(_wordBubbleNode != nullptr)
    {
        _wordBubbleNode->setVisible(true);
        Size visibleSize = Director::getInstance()->getVisibleSize();
        bool mappingFound = translatedText(nodeName);
        if(!mappingFound)
            return;
        
        if(_textDisplayAnimationRunning) {
            this->unschedule(schedule_selector(StoryPlaying::removeWordBubble));
            displayTextAnimationFinished();
        } else {
            _textDisplayAnimationRunning = true;
            _wordBubbleNode->setPosition(Vec2(visibleSize.width/2, visibleSize.height + 650));
            auto moveTo = MoveTo::create(1.0, Vec2(visibleSize.width/2, visibleSize.height + 50));
            auto elastic = EaseBackOut::create(moveTo);
            auto showTextFinished = CallFunc::create(CC_CALLBACK_0(StoryPlaying::displayTextAnimationFinished, this));
            _wordBubbleNode->runAction(Sequence::create(elastic, showTextFinished, NULL));
            
        }
    }
}

void StoryPlaying::displayTextAnimationFinished() {
    this->scheduleOnce(schedule_selector(StoryPlaying::removeWordBubble), 3.0f);
}

void StoryPlaying::removeWordBubble(float dt) {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto moveTo = MoveTo::create(1.0, Vec2(visibleSize.width/2, visibleSize.height + 650));
    auto elastic = EaseBackOut::create(moveTo);
    auto callbackAction = CallFunc::create(CC_CALLBACK_0(StoryPlaying::wordBubbleDisappeared, this));
    _wordBubbleNode->runAction(Sequence::create(elastic, callbackAction, NULL));
    
}

void StoryPlaying::wordBubbleDisappeared() {
    _textDisplayAnimationRunning = false;
    _pronouceWord = "";
}


bool StoryPlaying::translatedText(std::string text) {
    if(_wordMappings.find(text) != _wordMappings.end()) {
        std::string mappedStr = _wordMappings.at(text);
        std::replace(mappedStr.begin(), mappedStr.end(), '_', ' ');
        
        _pronouceWord =  TextGenerator::getInstance()->translateString(mappedStr);
        
        if(_wordBubbleNode != NULL)
        {
            Node* chooseText = _wordBubbleNode->getChildByName(TEXT_FIELD_WORD);
            
            if(chooseText != NULL) {
                cocos2d::ui::TextField* chooseLabel = dynamic_cast<cocos2d::ui::TextField *>(chooseText);
                if(chooseLabel != NULL) {
                    chooseLabel->setTouchEnabled(false);
                    chooseLabel->setString(_pronouceWord);
                    chooseLabel->setFontSize(100);
                    chooseLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
                    chooseLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
                    chooseLabel->setFontName(LangUtil::getInstance()->getFontFile());
                    chooseLabel->setTextColor(Color4B::BLACK);
                }
            }
        }
        
        return true;
    }
    
    return false;
}


void StoryPlaying::pronounceWord(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            clickedButton->setHighlighted(true);
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            _menuContext->pronounceWord(_pronouceWord, false);
            break;
        }
            
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}


void StoryPlaying::showTextAgain(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType) {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
        {
            clickedButton->setHighlighted(true);
            break;
        }
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
        {
            clickedButton->setVisible(false);
            clickedButton->setEnabled(false);
            MenuContext::_isInStoryDialogSpeechCurrentlyActive = true;
            if(_talkBubbleNode) {
                _talkBubbleNode ->setVisible(true);
                Node* closeNode = _talkBubbleNode->getChildByName(CLOSE_BUTTON);
                if(closeNode != NULL) {
                    cocos2d::ui::Button* closeButton = dynamic_cast<cocos2d::ui::Button *>(closeNode);
                    if(closeButton != NULL) {
                        closeButton->setEnabled(true);
                        _isTextShown = true;
                        _showAgainTextButton->setEnabled(false);
                        _showAgainTextButton->setVisible(false);
                        //show next/prev buttons
                        _nextButton->setEnabled(false);
                        _nextButton->setVisible(false);
                        if(_pageIndex != 0) {
                            _prevButton->setEnabled(false);
                            _prevButton->setVisible(false);
                        }
                    }
                }
            } else {
                createDialogBubble();
            }
            break;
        }
            
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}


void StoryPlaying::_changeArmature(Node* parentNode)
{
    const auto& armatureNames = _dragonBonesData->getArmatureNames();
    if (armatureNames.empty())
    {
        return;
    }
    
    // Remove prev Armature.
    if (_armature)
    {
        _armature->dispose();
        parentNode->removeChild(_armatureDisplay);
    }
    
    // Get Next Armature name.
    _armatureIndex++;
    if (_armatureIndex >= armatureNames.size())
    {
        _armatureIndex = 0;
    }
    
    const auto& armatureName = armatureNames[_armatureIndex];
    
    // a. Build Armature Display. (buildArmatureDisplay will advanceTime animation by Armature Display)
    _armatureDisplay = _factory.buildArmatureDisplay(armatureName);
    _armature = _armatureDisplay->getArmature();
    
    // Add Armature Display.
    _armatureDisplay->setScale(_armatureScale * 0.5f);
    parentNode->addChild(_armatureDisplay);
    
    _animationIndex = 0;
}

void StoryPlaying::_changeAnimation()
{
    const auto& animationNames = _armatureDisplay->getAnimation().getAnimationNames();
    if (animationNames.empty())
    {
        return;
    }
    
    // Get next Animation name.
    _animationIndex++;
    if (_animationIndex >= animationNames.size())
    {
        _animationIndex = 0;
    }
    
    const auto& animationName = animationNames[_animationIndex];
    
    // Play Animation.
    _armatureDisplay->getAnimation().play(animationName, 100);
}

void StoryPlaying::_stopAnimationTo(std::string animName)
{
    const auto& animationNames = _armatureDisplay->getAnimation().getAnimationNames();
    if (animationNames.empty())
    {
        return;
    }
    
    
    // Get next Animation name.
    _animationIndex++;
    if (_animationIndex >= animationNames.size())
    {
        _animationIndex = 0;
    }
    
    
    for (std::vector<std::string>::const_iterator p = animationNames.begin();
         p != animationNames.end(); ++p) {
        std::string s = *p;
        
        if (s.find(animName) != std::string::npos)
        {
            _armatureDisplay->getAnimation().stop(animName);
        }
    }
}

void StoryPlaying::_changeAnimationTo(std::string animName)
{
    const auto& animationNames = _armatureDisplay->getAnimation().getAnimationNames();
    if (animationNames.empty())
    {
        return;
    }
    
    
    // Get next Animation name.
    _animationIndex++;
    if (_animationIndex >= animationNames.size())
    {
        _animationIndex = 0;
    }
    
    
    for (std::vector<std::string>::const_iterator p = animationNames.begin();
         p != animationNames.end(); ++p) {
        std::string s = *p;
        
        if (s.find(animName) != std::string::npos)
        {
            _armatureDisplay->getAnimation().play(animName);
        }
    }
}
