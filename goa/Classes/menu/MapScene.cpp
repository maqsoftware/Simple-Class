//
//  MapScene.cpp
//  safari
//
//  Created by Shyamal  Upadhyaya on 26/07/16.
//
//

#include "MapScene.h"
#include "../menu/LevelHelpScene.h"
#include "../menu/LevelHelpOverlay.h"
#include "storage/local-storage/LocalStorage.h"
#include "StartMenuScene.h"
#include "MainMenuHome.hpp"
#include "../misc/ScaleGuiElements.cpp"

static const std::string CURRENT_LEVEL = ".currentLevel";
const std::map<std::string, std::string> MapScene::levelToGameNameMap = MapScene::createLevelToGameName();

USING_NS_CC;

Scene *MapScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MapScene::create();

    layer->menuContext = MenuContext::create(layer, MapScene::gameName());
    scene->addChild(layer->menuContext, 2);

    std::string currentLevelStr;
    localStorageGetItem(MapScene::gameName() + CURRENT_LEVEL, &currentLevelStr);

    if (currentLevelStr.empty())
    {
        std::string defaultLevel = "1";
        localStorageSetItem(MapScene::gameName() + CURRENT_LEVEL, defaultLevel);
        layer->unlockLevel = 1;
    }
    localStorageGetItem(MapScene::gameName() + CURRENT_LEVEL, &currentLevelStr);

    if (!currentLevelStr.empty())
    {
        layer->menuContext->setCurrentLevel(std::atoi(currentLevelStr.c_str()));
        layer->unlockLevel = std::atoi(currentLevelStr.c_str());
    }

    layer->loadMap();
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene

    return scene;
}

MapScene *MapScene::create()
{
    MapScene *mapLayer = new (std::nothrow) MapScene();
    if (mapLayer && mapLayer->init())
    {
        mapLayer->autorelease();
        return mapLayer;
    }
    CC_SAFE_DELETE(mapLayer);
    return nullptr;
}

MapScene::MapScene() : menuContext(nullptr),
                       unlockLevel(0)
{
}

MapScene::~MapScene()
{
}

bool MapScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    //    FileUtils::getInstance()->addSearchPath("res/map");

    this->currentLangUtil = LangUtil::getInstance();

    gameNameToLevelMap.insert({CAMP, "1"});
    gameNameToLevelMap.insert({FARMHOUSE, "2"});
    gameNameToLevelMap.insert({MININGBG, "3"});
    gameNameToLevelMap.insert({CITY1, "4"});
    gameNameToLevelMap.insert({CITY2, "5"});
    gameNameToLevelMap.insert({CITY3, "6"});
    gameNameToLevelMap.insert({CITY4, "7"});
    gameNameToLevelMap.insert({CITY5, "8"});

    return true;
}

void MapScene::loadMap()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Node *rootNode = CSLoader::createNode(MAP_FILE);
    this->addChild(rootNode);
    this->processChildNodes(rootNode);
    cocos2d::ui::Button *backButton = createBackButton();
    ScaleUIElement<cocos2d::ui::Button *>::scaleGuiElements(backButton);

    backButton->setPosition(Vec2(origin.x + 150, origin.y + visibleSize.height - 150));
    this->addChild(backButton);
}

void MapScene::processChildNodes(cocos2d::Node *rootNode)
{
    //iterate thru all children
    auto children = rootNode->getChildren();
    Node *mainLayer = NULL;
    for (std::vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
    {
        cocos2d::Node *node = *it;
        //based on custom data create layers
        cocostudio::ComExtensionData *data = (cocostudio::ComExtensionData *)node->getComponent("ComExtensionData");
        if (data != NULL)
        {
            CCLOG("%s", data->getCustomProperty().c_str());

            if (data->getCustomProperty() == MAIN_LAYER)
            {
                mainLayer = node;
            }
        }
    }

    assert(mainLayer != NULL);
    //iterate thru all children
    auto mapChildren = mainLayer->getChildren();

    for (std::vector<Node *>::iterator it = mapChildren.begin(); it != mapChildren.end(); ++it)
    {
        cocos2d::Node *node = *it;

        cocostudio::ComExtensionData *data = (cocostudio::ComExtensionData *)node->getComponent("ComExtensionData");
        if (data != NULL && !data->getCustomProperty().empty())
        {
            std::unordered_map<std::string, std::string> attributes = RPGConfig::parseUserData(data->getCustomProperty());

            std::unordered_map<std::string, std::string>::const_iterator it = attributes.find("text");

            std::unordered_map<std::string, std::string>::const_iterator itNextScene = attributes.find("nextScene");

            if (it != attributes.end())
            {
                // process text
                if (it->first.compare("text") == 0)
                {
                    std::string lName = node->getName();
                    std::string englishName = "";
                    bool isBilingual = localeCode != "en";
                    std::string fileName = "map_places";
                    if (lName.compare("City1") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 0, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 0, isBilingual);
                    }
                    else if (lName.compare("City2") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 1, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 1, isBilingual);
                    }
                    else if (lName.compare("City3") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 2, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 2, isBilingual);
                    }
                    else if (lName.compare("city4") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 3, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 3, isBilingual);
                    }
                    else if (lName.compare("City5") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 4, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 4, isBilingual);
                    }
                    else if (lName.compare("Camp") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 5, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 5, isBilingual);
                    }
                    else if (lName.compare("Farm House") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 6, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 6, isBilingual);
                    }
                    else if (lName.compare("Mining Town") == 0)
                    {
                        englishName = MainMenuHome::readTitleFromJson(fileName, 7, false);
                        lName = MainMenuHome::readTitleFromJson(fileName, 7, isBilingual);
                    }

                    /* 
                     * Make the text of the map section
                     * bilingual by adding an English label 
                     * above the Hindi label.
                    */
                    std::string mapText = this->currentLangUtil->translateString(lName);
                    node->setPosition(Vec2(node->getPosition().x, node->getPosition().y + 40));

                    if (isBilingual)
                    {
                        cocos2d::Label *label = Label::createWithSystemFont(mapText, "arial", 70);
                        label->setPosition(Vec2(node->getPosition().x, node->getPosition().y - 60));
                        label->setColor(Color3B(128, 64, 0));
                        mainLayer->addChild(label);
                    }

                    // Add English Label above the hindi text
                    cocos2d::Label *englishLabel = Label::createWithTTF(englishName, "fonts/arial.ttf", 70);
                    englishLabel->setPosition(Vec2(node->getPosition().x, node->getPosition().y + 15));
                    englishLabel->setColor(Color3B(128, 64, 0));
                    mainLayer->addChild(englishLabel);
                }
            }

            if (itNextScene != attributes.end())
            {
                //bind events
                cocos2d::ui::Button *button = dynamic_cast<cocos2d::ui::Button *>(node);
                if (button)
                {
                    button->setPosition(Vec2(button->getPosition().x, button->getPosition().y + 30));
                    button->setScale(0.95);
                    button->setName(itNextScene->second);
                    button->setTitleText("");
                    button->addTouchEventListener(CC_CALLBACK_2(MapScene::islandSelected, this));
                    std::string buttonLevel = gameNameToLevelMap.at(button->getName());
                    if (!buttonLevel.empty())
                    {
                        std::string unlockStr;
                        localStorageGetItem(".unlock", &unlockStr);
                        bool lockAll = false;
                        if (unlockStr.empty() || unlockStr == "1")
                        {
                            lockAll = true;
                        }
                        // unlocked all games in Map Scene
                        button->setEnabled(true);
                    }
                    else
                    {
                        button->setEnabled(false);
                    }
                }
            }
        }
    }
}

void MapScene::islandSelected(Ref *pSender, ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button *clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType)
    {
    case ui::Widget::TouchEventType::BEGAN:
    {
        clickedButton->setHighlighted(true);
        break;
    }
    case ui::Widget::TouchEventType::MOVED:
        break;
    case ui::Widget::TouchEventType::ENDED:
    {
        clickedButton->setEnabled(false);
        if (gameNameToLevelMap.count(clickedButton->getName()) == 1)
        {
            std::string level = gameNameToLevelMap.at(clickedButton->getName());
            localStorageSetItem("map.currentLevel", level);
        }
        Director::getInstance()->replaceScene(LevelHelpScene::createScene(clickedButton->getName().c_str()));
        break;
    }

    case ui::Widget::TouchEventType::CANCELED:
        break;
    default:
        break;
    }
}

const std::map<std::string, std::string> MapScene::createLevelToGameName()
{

    std::map<std::string, std::string> levelToGameNameMaps;

    levelToGameNameMaps.insert(std::make_pair("1", CAMP));
    levelToGameNameMaps.insert(std::make_pair("2", FARMHOUSE));
    levelToGameNameMaps.insert(std::make_pair("3", MININGBG));
    levelToGameNameMaps.insert(std::make_pair("4", CITY1));
    levelToGameNameMaps.insert(std::make_pair("5", CITY2));
    levelToGameNameMaps.insert(std::make_pair("6", CITY3));
    levelToGameNameMaps.insert(std::make_pair("7", CITY4));
    levelToGameNameMaps.insert(std::make_pair("8", CITY5));

    return levelToGameNameMaps;
}

cocos2d::ui::Button *MapScene::createBackButton()
{

    std::string buttonNormalIcon = "menu/back.png";
    std::string buttonPressedIcon = buttonNormalIcon;
    cocos2d::ui::Button *button = ui::Button::create();
    std::string buttonDisabledIcon = buttonNormalIcon;
    if (buttonDisabledIcon.find(".png") != std::string::npos)
    {
        buttonDisabledIcon = buttonDisabledIcon.insert(buttonDisabledIcon.find(".png"), "_disabled");
    }

    button->loadTextureNormal(buttonNormalIcon);
    button->loadTexturePressed(buttonPressedIcon);
    button->loadTextureDisabled(buttonDisabledIcon);
    button->addTouchEventListener(CC_CALLBACK_2(MapScene::backButtonPressed, this));

    return button;
}

void MapScene::backButtonPressed(Ref *pSender, ui::Widget::TouchEventType eEventType)
{
    Director::getInstance()->replaceScene(MainMenuHome::createScene());
}
