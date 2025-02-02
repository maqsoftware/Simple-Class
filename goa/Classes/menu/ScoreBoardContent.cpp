//
//  ScoreBoardContent.cpp
//  safari
//
//  Created by Shyamal  Upadhyaya on 02/08/16.
//
//

#include "ScoreBoardContext.h"
#include "../hero/HelloWorldScene.h"
#include "../menu/ScrollableGameMapScene.hpp"
#include "LevelMenu.h"
#include "storage/local-storage/LocalStorage.h"
#include "MapScene.h"
#include "LevelHelpScene.h"
#include "../story/ScrollableCatalogue.hpp"

USING_NS_CC;

static const std::string REWARD_STICKER = "s";
static const std::string REWARD_PATCH = "p";
static const std::string REWARD_MEDAL = "m";
static const std::string REWARD_GEM = "g";
static const std::string REWARD_CANDY = "c";
static const std::string REWARD_BADGE = "b";

ScoreBoardContext* ScoreBoardContext::create(int stars, std::string gameName, std::string sceneName, int level)
{
    ScoreBoardContext* scoreBoard = new (std::nothrow) ScoreBoardContext();
    if(scoreBoard && scoreBoard->init(stars, gameName, sceneName, level)) {
        scoreBoard->autorelease();
        return scoreBoard;
    }
    CC_SAFE_DELETE(scoreBoard);
    return nullptr;
}


ScoreBoardContext::ScoreBoardContext():
_stars(0),
_gameToUnlock(""),
_autoLevel(false)
{
    
}

ScoreBoardContext::~ScoreBoardContext() {
}

bool ScoreBoardContext::init(int stars, std::string gameName, std::string sceneName, int level)
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
        
    FileUtils::getInstance()->addSearchPath("res/scoreboard");
    this->_stars = stars;
    this->_gameName = gameName;
    this->_sceneName = sceneName;
    this->createScoreBoard();
    std::string contents = FileUtils::getInstance()->getStringFromFile("config/game_map.json");
    
    rapidjson::Document d;
    std::map<std::string, std::map<std::string, std::string>> gameIcons;
    if (false == d.Parse<0>(contents.c_str()).HasParseError()) {
        for (rapidjson::SizeType i = 0; i < d.Size(); i++) {
            const rapidjson::Value& game = d[i];
            std::string jsonGameName = game["name"].GetString();
            std::map<std::string, std::string> gameIconMap;
            gameIconMap["icon"] = game["icon"].GetString();
            gameIconMap["cIcon"] = game["cIcon"].GetString();
            gameIconMap["title"] = LangUtil::getInstance()->translateString(game["title"].GetString());
            gameIcons[jsonGameName] = gameIconMap;
            if(gameName == jsonGameName) {
                if(game.HasMember("autoLevel") && game["autoLevel"].GetBool() == true) {
                    _autoLevel = true;
                }
                if(game.HasMember("numLevels")) {
                    _numberOfLevels = game["numLevels"].GetString();
                    
                    std::string currentLevel;
                    localStorageGetItem(_gameName + ".currentLevel", &currentLevel);
                    int curLevel = atoi(currentLevel.c_str());
                    int totalLevels = atoi(_numberOfLevels.c_str());
                    if(curLevel >= totalLevels) {
                        _nextButton->setEnabled(false);
                    } else {
                        _nextButton->setEnabled(true);
                    }
                } else {
                    _nextButton->setEnabled(false);
                }
                
                if(_autoLevel) {
                    _levelButton->setVisible(false);
                }
                
                if(game.HasMember("rewards")) {
                    const rapidjson::Value& rewards = game["rewards"];
                    auto badgesToAdd = getStarBadges(level);
                    if(rewards.HasMember(MenuContext::to_string(level).c_str())) {
                        const rapidjson::Value& levelRewards = rewards[MenuContext::to_string(level).c_str()];
                        if(levelRewards.HasMember("unlock")) {
                            std::string gameToUnlock = levelRewards["unlock"].GetString();
                            unlockGame(gameToUnlock);
                        }
                        if(levelRewards.HasMember("badge")) {
                            badgesToAdd.push_back(levelRewards["badge"].GetString());
                        }
                        if(levelRewards.HasMember("unlockMin")) {
                            std::string unlockMin = levelRewards["unlockMin"].GetString();
                            std::string unlockMinStr;
                            localStorageGetItem("unlockMin", &unlockMinStr);
                            rapidjson::Document unlockMinDoc;
                            rapidjson::Document::AllocatorType& allocator = unlockMinDoc.GetAllocator();
                            if(unlockMinStr.empty()) {
                                unlockMinDoc.SetObject();
                            } else {
                                unlockMinDoc.Parse(unlockMinStr.c_str());
                            }
                            bool unlockMinModified = true;
                            int unlockCount = 0;
                            if(unlockMinDoc.HasMember(unlockMin.c_str())) {
                                rapidjson::Value& unlockMinGames = unlockMinDoc[unlockMin.c_str()];
                                for (rapidjson::SizeType bgi = 0; bgi < unlockMinGames.Size(); bgi++) {
                                    if(unlockMinGames[bgi].GetString() == gameName) {
                                        unlockMinModified = false;
                                        break;
                                    }
                                }
                                if(unlockMinModified) {
                                    unlockMinGames.PushBack(rapidjson::Value(gameName.c_str(), allocator).Move(), allocator);
                                    unlockCount = unlockMinGames.Size();
                                }
                            } else {
                                rapidjson::Value unlockMinGames(rapidjson::kArrayType);
                                unlockMinGames.PushBack(rapidjson::Value(gameName.c_str(), allocator).Move(), allocator);
                                unlockMinDoc.AddMember(rapidjson::Value(unlockMin.c_str(), allocator).Move(), unlockMinGames, allocator);
                                unlockCount = 1;
                            }
                            if(unlockMinModified) {
                                if(levelRewards.HasMember("min")) {
                                    std::string minStr = levelRewards["min"].GetString();
                                    int min = atoi(minStr.c_str());
                                    if(unlockCount >= min) {
                                        unlockGame(unlockMin);
                                    }
                                }
                                rapidjson::StringBuffer buffer;
                                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                                unlockMinDoc.Accept(writer);
                                const char* output = buffer.GetString();
                                localStorageSetItem("unlockMin", output);
                            }
                        }
                    }
                    addBadges(badgesToAdd);
                }
            }
        }
    }
    if((!_gameToUnlock.empty() && gameIcons.count(_gameToUnlock) > 0) || _badges.size() > 0) {
        SpriteFrameCache* cache = SpriteFrameCache::getInstance();
        cache->addSpriteFramesWithFile("gift.plist"); // relative
        _gift = Sprite::createWithSpriteFrameName("Gift0001.png");
        for (int i = 1; i < 61; i++)
        {
            std::string num = StringUtils::format("%02d", i);
            _giftFrames.pushBack(cache->getSpriteFrameByName("Gift00" + num + ".png"));
        }
        
        // create the animation out of the frames and an action for the new animation
        
        _giftAnimation = Animation::createWithSpriteFrames(_giftFrames, 0.06f);
        _giftAnimation->retain();
        
        // use/run the animation
        
        auto openGift = Animate::create(_giftAnimation);
        Vector<FiniteTimeAction *> actions;
        
        if(!_gameToUnlock.empty() && gameIcons.count(_gameToUnlock) > 0) {
            auto unlockedGameButton = ui::Button::create(gameIcons[_gameToUnlock]["icon"], gameIcons[_gameToUnlock]["cIcon"], gameIcons[_gameToUnlock]["icon"], ui::Widget::TextureResType::LOCAL);
            auto titleStr = LangUtil::getInstance()->translateString("Game Unlocked");
            auto rewardCard = CSLoader::createNode("scoreboard/rewardcard.csb");
            auto rewardUnlocked = rewardCard->getChildByName("reward_unlocked");
            if(rewardUnlocked != nullptr) {
                auto label = Label::createWithTTF(titleStr, LangUtil::getInstance()->getFontFile(), 48);
                label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
                label->setColor(Color3B(210.0, 76.0, 74.0));
                rewardUnlocked->addChild(label);
            }
            auto rewardName = rewardCard->getChildByName("reward_name");
            if(rewardName != nullptr) {
                auto label = Label::createWithTTF(LangUtil::getInstance()->translateString(gameIcons[_gameToUnlock]["title"]), LangUtil::getInstance()->getFontFile(), 48);
                label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
                label->setColor(Color3B(210.0, 76.0, 74.0));
                rewardName->addChild(label);
            }
            unlockedGameButton->addTouchEventListener(CC_CALLBACK_2(ScoreBoardContext::buttonClicked, this));
            unlockedGameButton->setName("unlockedGame");
            unlockedGameButton->setScale(0.7);
            rewardCard->addChild(unlockedGameButton);
            rewardCard->setScale(0.1);
            addChild(rewardCard);
            auto jumpAction = JumpTo::create(1.0, Vec2(850, -800), 600, 2);
            auto scaleAction = ScaleTo::create(1.0, 1.0);
            auto spawn = Spawn::create(jumpAction, scaleAction, NULL);
            actions.pushBack(TargetedAction::create(rewardCard, spawn));
        }
        if(_badges.size() > 0) {
            int numRewards = 0;
            for (auto it = _badges.begin() ; it != _badges.end(); ++it) {
                auto badge = *it;
                auto badgeButton = ui::Button::create("rewards/" + badge + ".png", "rewards/" + badge + ".png", "rewards/" + badge + ".png", ui::Widget::TextureResType::LOCAL);
                std::replace(badge.begin(), badge.end(), '_', ' ');
                auto titleStr = LangUtil::getInstance()->translateString("Trophy earned");
                if(badgeButton != nullptr) {
                    auto rewardCard = CSLoader::createNode("scoreboard/rewardcard.csb");
                    auto rewardUnlocked = rewardCard->getChildByName("reward_unlocked");
                    if(rewardUnlocked != nullptr) {
                        auto label = Label::createWithTTF(titleStr, LangUtil::getInstance()->getFontFile(), 48);
                        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
                        label->setColor(Color3B(210.0, 76.0, 74.0));
                        rewardUnlocked->addChild(label);
                    }
                    auto rewardName = rewardCard->getChildByName("reward_name");
                    if(rewardName != nullptr) {
                        auto label = Label::createWithTTF(LangUtil::getInstance()->translateString(badge.substr(2)), LangUtil::getInstance()->getFontFile(), 48);
                        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
                        label->setColor(Color3B(210.0, 76.0, 74.0));
                        rewardName->addChild(label);
                    }
//                    badgeButton->setScale(0.5);
                    rewardCard->addChild(badgeButton);
                    rewardCard->setScale(0.1);
                    addChild(rewardCard);

                    auto finalPos = Vec2(-850, -800);
                    if(numRewards > 0) {
                        finalPos = Vec2(0, -800);
                    }
                    auto jumpAction = JumpTo::create(1.0, finalPos, 600, 2);
                    auto scaleAction = ScaleTo::create(1.0, 1.0);
                    auto spawn = Spawn::create(jumpAction, scaleAction, NULL);
                    actions.pushBack(TargetedAction::create(rewardCard, spawn));
                }
                numRewards++;
            }
        }
        if(actions.size() > 0) {
            addChild(_gift);
            Size visibleSize = Director::getInstance()->getVisibleSize();
            auto moveAction = MoveTo::create(1.0, Vec2(visibleSize.width / 2, visibleSize.height * 5 / 8));
            actions.pushBack(TargetedAction::create(this, moveAction));
            actions.pushBack(CallFunc::create([=]() {
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/sfx/boing.ogg");
            }));
            _gift->runAction(Sequence::create(openGift, Spawn::create(actions), NULL));
        }
    }
    
    std::size_t isStories = _gameName.find("storyId");
    if (isStories!=std::string::npos) {
        _nextButton->setEnabled(false);
        _replayButton->setVisible(false);
    }
    
    return true;
}

std::map<std::string, std::map<std::string, int>> ScoreBoardContext::getRewards() {
    std::map<std::string, std::map<std::string, int>> rewards;
    std::string badgesStr;
    localStorageGetItem("badges", &badgesStr);
    if(!badgesStr.empty()) {
        rapidjson::Document badgesDoc;
        if (false == badgesDoc.Parse<0>(badgesStr.c_str()).HasParseError()) {
            for (rapidjson::Value::ConstMemberIterator itr = badgesDoc.MemberBegin();
                 itr != badgesDoc.MemberEnd(); ++itr) {
                std::string badge = itr->name.GetString();
                const rapidjson::Value& badgeGames = itr->value;
                assert(badgeGames.IsArray());
                auto badgeType = badge.substr(0, 1);
                if(rewards.count(badgeType) > 0) {
                    rewards[badgeType][badge] = badgeGames.Size();
                } else {
                    std::map<std::string, int> badgesOfAType;
                    badgesOfAType[badge] = badgeGames.Size();
                    rewards[badgeType] = badgesOfAType;
                }
            }
        }
    }
    return rewards;
}


std::vector<std::string> ScoreBoardContext::getStarBadges(int level) {
    std::vector<std::string> starBadges;
    std::string progressStr;
    localStorageGetItem(_gameName + ".level", &progressStr);
    rapidjson::Document d;
    if(!progressStr.empty()) {
        d.Parse(progressStr.c_str());
        if(d.Size() >= level) {
            if(d[level].GetInt() == 3) {
                starBadges.push_back("b/3_star");
            }
            if(level >= 3) {
                if(d[level-2].GetInt() == 3 && d[level-1].GetInt() == 3 && d[level].GetInt() == 3) {
                    starBadges.push_back("m/3_3_star_in_a_row");
                }
            }
            if(level >= 5) {
                if(d[level-4].GetInt() == 3 && d[level-3].GetInt() == 3 && d[level-2].GetInt() == 3 && d[level-1].GetInt() == 3 && d[level].GetInt() == 3) {
                    starBadges.push_back("g/5_3_star_in_a_row");
                }
            }
        }
    }
    return starBadges;
}

bool ScoreBoardContext::addBadges(std::vector<std::string> badges) {
    std::string badgesStr;
    localStorageGetItem("badges", &badgesStr);
    rapidjson::Document badgesDoc;
    rapidjson::Document::AllocatorType& allocator = badgesDoc.GetAllocator();
    if(badgesStr.empty()) {
        badgesDoc.SetObject();
    } else {
        badgesDoc.Parse(badgesStr.c_str());
    }
    bool badgeModified = false;
    for (auto it = badges.begin() ; it != badges.end(); ++it) {
        auto badge = *it;
        if(badgesDoc.HasMember(badge.c_str())) {
            rapidjson::Value& badgeGames = badgesDoc[badge.c_str()];
            bool gameHasBadge = false;
            for (rapidjson::SizeType bgi = 0; bgi < badgeGames.Size(); bgi++) {
                if(badgeGames[bgi].GetString() == _gameName) {
                    gameHasBadge = true;
                    break;
                }
            }
            if(!gameHasBadge) {
                badgeGames.PushBack(rapidjson::Value(_gameName.c_str(), allocator).Move(), allocator);
                _badges.push_back(badge);
                badgeModified = true;
            }
        } else {
            rapidjson::Value badgeGames(rapidjson::kArrayType);
            badgeGames.PushBack(rapidjson::Value(_gameName.c_str(), allocator).Move(), allocator);
            badgesDoc.AddMember(rapidjson::Value(badge.c_str(), allocator).Move(), badgeGames, allocator);
            badgeModified = true;
            _badges.push_back(badge);
        }
    }
    if(badgeModified) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        badgesDoc.Accept(writer);
        const char* output = buffer.GetString();
        localStorageSetItem("badges", output);
    }
    auto test = getRewards();
    return badgeModified;
}

bool ScoreBoardContext::unlockGame(std::string gameToUnlock) {
    std::string unlockedGamesStr;
    localStorageGetItem("unlockedGames", &unlockedGamesStr);
    rapidjson::Document unlockDoc;
    rapidjson::Document::AllocatorType& allocator = unlockDoc.GetAllocator();
    if(unlockedGamesStr.empty()) {
        unlockDoc.SetObject();
    } else {
        unlockDoc.Parse(unlockedGamesStr.c_str());
    }
    if(!unlockDoc.HasMember(gameToUnlock.c_str())) {
        unlockDoc.AddMember(rapidjson::Value(gameToUnlock.c_str(), allocator).Move(), "unlock", allocator);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        unlockDoc.Accept(writer);
        const char* output = buffer.GetString();
        localStorageSetItem("unlockedGames", output);
        ScrollableGameMapScene::pushTopBarGame(gameToUnlock);
        _gameToUnlock = gameToUnlock;
        return true;
    }
    return false;
}

void ScoreBoardContext::createScoreBoard() {
    Node *rootNode = CSLoader::createNode(SCORE_BOARD_FILE);
    this->addChild(rootNode);
    this->processChildNodes(rootNode);
    this->showStars();
}



void ScoreBoardContext::processChildNodes(cocos2d::Node *rootNode) {
    //iterate thru all children
    auto children = rootNode->getChildren();
    Node* mainLayer = NULL;
    for (std::vector<Node*>::iterator it = children.begin() ; it != children.end(); ++it) {
        cocos2d::Node* node = *it;
        //based on custom data create layers
        cocostudio::ComExtensionData* data = (cocostudio::ComExtensionData*)node->getComponent("ComExtensionData");
        if(data != NULL) {
            CCLOG("%s", data->getCustomProperty().c_str());
            
            if(data->getCustomProperty() == MAIN_LAYER)
            {
                mainLayer = node;
            }
        }
    }
    
    assert(mainLayer != NULL);
    //iterate thru all children
    auto mainChildren = mainLayer->getChildren();
    
    for (std::vector<Node*>::iterator it = mainChildren.begin() ; it != mainChildren.end(); ++it) {
        cocos2d::Node* node = *it;
        
        cocostudio::ComExtensionData* data = (cocostudio::ComExtensionData*)node->getComponent("ComExtensionData");
        if(data != NULL && !data->getCustomProperty().empty()) {
            std::string action = data->getCustomProperty();
            CCLOG("action %s", action.c_str());
            if(dynamic_cast<cocos2d::ui::Button *>(node))
            {
                cocos2d::ui::Button * button = dynamic_cast<cocos2d::ui::Button *>(node);
                
                if(button) {
                    button->addTouchEventListener(CC_CALLBACK_2(ScoreBoardContext::buttonClicked, this));
                    button->setName(action);
                    
                    if(button->getName() == "next") {
                        _nextButton = button;
                    }
                    if(button->getName() == "replay") {
                        _replayButton = button;
                    }
                    if(button->getName() == "level") {
                        _levelButton = button;
                    }
                    
                }
                
            } else if(dynamic_cast<Sprite *>(node)) {
                Sprite* sprite = dynamic_cast<Sprite *>(node);
                if(action == "show_one") {
                    this->starOne = sprite;
                    this->starOne->setVisible(false);
                } else if(action == "show_two") {
                    this->starTwo = sprite;
                    this->starTwo->setVisible(false);
                } else if(action == "show_three") {
                    this->starThree = sprite;
                    this->starThree->setVisible(false);
                }
            }
        }
        
    }
    
}

void ScoreBoardContext::showStars() {
    if(_stars == 1) {
        this->starOne->setVisible(true);
    } else if(_stars == 2) {
        this->starOne->setVisible(true);
        this->starTwo->setVisible(true);
    } else if(_stars == 3){
        this->starOne->setVisible(true);
        this->starTwo->setVisible(true);
        this->starThree->setVisible(true);
    }

}

void ScoreBoardContext::transit() {
    std::size_t isStories = _gameName.find("storyId");
    if (isStories!=std::string::npos) {
        //ScriptingCore::getInstance()->runScript("src/start/storyPlay.js");
        Director::getInstance()->replaceScene(ScrollableCatalogue::createScene());
    } else if(!this->_sceneName.empty()) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5, HelloWorld::createScene(this->_gameName,this->_sceneName, true), Color3B::BLACK));
    } else {
        StartMenu::startScene(this->_gameName);
    }
}

void ScoreBoardContext::buttonClicked(Ref* pSender, ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button* clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
	CCLOG("clicked button in scoreBoard = %s", clickedButton->getName().c_str());
    switch (eEventType) {
        case ui::Widget::TouchEventType::BEGAN:
        {
            break;
        }
        case ui::Widget::TouchEventType::MOVED:
            break;
        case ui::Widget::TouchEventType::ENDED:
        {
            if(clickedButton->getName() == "replay") {
                std::size_t isStories = _gameName.find("storyId");
                if (isStories!=std::string::npos || _gameName == "Show Stories"){
                    Director::getInstance()->replaceScene(TransitionFade::create(2.0, ScrollableGameMapScene::createScene(), Color3B::BLACK));                    
                } else {
                   // this->transit();
					MenuContext::launchGameFromJS(_gameName);
                }
                
            }
            else if(clickedButton->getName() == "level") {
                std::size_t isStories = _gameName.find("storyId");
                if (isStories!=std::string::npos || _gameName == "Show Stories"){
                    Director::getInstance()->replaceScene(TransitionFade::create(2.0, ScrollableCatalogue::createScene(), Color3B::BLACK));
                    
                } else if(_gameName == "map") {
                    Director::getInstance()->replaceScene(MapScene::createScene());
                } else {
                    Director::getInstance()->replaceScene(LevelMenu::createScene(_gameName));
                }                
            }
            else if(clickedButton->getName() == "game") {
                Director::getInstance()->replaceScene(TransitionFade::create(2.0, MainMenuHome::createScene(), Color3B::BLACK));
            }
            else if(clickedButton->getName() == "next")  {

                std::size_t isStories = _gameName.find("storyId");
                
                if (isStories!=std::string::npos || _gameName == "Show Stories") {
                    Director::getInstance()->replaceScene(ScrollableCatalogue::createScene());
//                    ScriptingCore::getInstance()->runScript("src/start/storyPlay.js");
                } else if(_gameName == "map") {
                    std::string currentLevel;
                    localStorageGetItem(_gameName + ".currentLevel", &currentLevel);
                    if(!currentLevel.empty())
                    {
                        if(MapScene::levelToGameNameMap.count(currentLevel) == 1)
                        {
                            std::string gameName = MapScene::levelToGameNameMap.at(currentLevel);
                            Director::getInstance()->replaceScene(LevelHelpScene::createScene(gameName.c_str()));
                        } else {
                            Director::getInstance()->replaceScene(MapScene::createScene());
                        }
                    } else {
                        Director::getInstance()->replaceScene(MapScene::createScene());
                    }
                    
                } else {
                    std::string currentLevel;
                    localStorageGetItem(_gameName + ".currentLevel", &currentLevel);
                    int curLevel = atoi(currentLevel.c_str());
                    curLevel++;
                    int totalLevels = atoi(_numberOfLevels.c_str());
                    if(curLevel <= totalLevels) {
                        localStorageSetItem(_gameName + ".currentLevel", MenuContext::to_string(curLevel));
                        MenuContext::launchGameFromJS(_gameName);
                    }
                    
                }
            } else if(clickedButton->getName() == "unlockedGame") {
                if(!_gameToUnlock.empty()) {
                    ScrollableGameMapScene::nagivateToGame(_gameToUnlock);
                }
            }
            break;
        }
            
        case ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
    
    
    
}
