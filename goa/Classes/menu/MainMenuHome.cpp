//
//  MainMenuHome.cpp
//  goa
//
//  Created by Shyamal.Upadhyaya on 25/04/17.
//
//
#include "MainMenuHome.hpp"
#include "../hero/RPGConfig.h"
#include "MapScene.h"
#include "../misc/ScaleGuiElements.cpp"
#include "misc/FirebaseHelper.hpp"
#include "LevelHelpScene.h"

USING_NS_CC;

MainMenuHome::MainMenuHome() : _greyLayer(NULL), _gameNameToNavigate(""), _leftMostX(1250)
{
}

MainMenuHome::~MainMenuHome()
{
}

Scene *MainMenuHome::createScene()
{
    auto scene = Scene::create();
    auto layer = MainMenuHome::create();
    scene->addChild(layer);
    layer->menuContext = MenuContext::create(layer, "MainMenuHome");
    scene->addChild(layer->menuContext);
    return scene;
}

bool MainMenuHome::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Node::init())
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    this->currentLangUtil = LangUtil::getInstance();

    //    const int numRows = NUMBER_OF_BUTTONS_ROWS;
    //    const int numCols = NUMBER_OF_BUTTONS_COLS;
    //    int index = 0;
    //    int yOffset = 50;

    Node *rootNode = CSLoader::createNode(BACKGROUND_MENU);
    if (visibleSize.width > 2560)
    {
        auto myGameWidth = (visibleSize.width - 2560) / 2;
        rootNode->setPositionX(myGameWidth);
    }

    this->addChild(rootNode);
    this->bindEvents(rootNode);

    //    auto storyTopBarButton = createButton();
    //
    //    storyTopBarButton->setPosition(Vec2(_leftMostX + storyTopBarButton->getBoundingBox().size.width/2, visibleSize.height + yOffset - (0 + 0.5) * ((visibleSize.height + yOffset) / (numRows + 1))));
    //
    //    addChild(storyTopBarButton);

    //    const char *args[] = {
    //        "Jimbi alielezea \"Asubuhi tuamkapo, sisi huvirejesha ______________ vyetu mwilini.\""
    //
    //    };
    //    std::vector<std::string> v(args, std::end(args));
    //
    //    for (std::vector<std::string>::iterator it = v.begin() ; it != v.end(); ++it)
    //    {
    //        std::string item = *(it);
    //        unsigned long generatedHash = RPGConfig::hash(item);
    //        CCLOG("item: %s, hash: %ld", item.c_str(), generatedHash);
    //    }

    cocos2d::ui::Button *backButton = createBackButton();
    ScaleUIElement<cocos2d::ui::Button *>::scaleGuiElements(backButton);

    backButton->setPosition(Vec2(origin.x + 150, origin.y + visibleSize.height - 150));
    this->addChild(backButton);

    return true;
}

cocos2d::ui::Button *MainMenuHome::createBackButton()
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
    button->addTouchEventListener(CC_CALLBACK_2(MainMenuHome::backButtonPressed, this));

    return button;
}

void MainMenuHome::backButtonPressed(Ref *pSender, ui::Widget::TouchEventType eEventType)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//cocos2d::ui::Button* MainMenuHome::createButton() {
//    std::string ICONS = ICON_FOLDER;
//
//    std::string gameName = "story-catalogue";
//    std::string gameTitle = "Stories";
//    std::string buttonNormalIcon = "menu/library.png";
//    std::string buttonPressedIcon = "menu/library.png";
//    cocos2d::ui::Button* button = ui::Button::create();
//    std::string buttonDisabledIcon = buttonNormalIcon;
//    if(buttonDisabledIcon.find(".png") != std::string::npos) {
//        buttonDisabledIcon = buttonDisabledIcon.insert(buttonDisabledIcon.find(".png"), "_disabled");
//    }
//
//    button->loadTextureNormal(buttonNormalIcon);
//    button->loadTexturePressed(buttonPressedIcon);
//    button->loadTextureDisabled(buttonDisabledIcon);
//    button->addTouchEventListener(CC_CALLBACK_2(MainMenuHome::storySelected, this));
//
//    button->setName(gameName);
////    button->setTitleText(LangUtil::getInstance()->translateString(gameTitle));
////    button->setTitleAlignment(TextHAlignment::CENTER, TextVAlignment::BOTTOM);
////    button->setTitleFontName(LangUtil::getInstance()->getFontFile());
////    button->setTitleColor(Color3B(0xFF, 0xF2, 0x00));
////    button->setTitleFontSize(48);
////    auto label = button->getTitleRenderer();
////    label->setPosition(Vec2(label->getPositionX(), label->getPositionY()- 200));
////    button->setScale(0.5);
//    return button;
//}

void MainMenuHome::bindEvents(cocos2d::Node *rootNode)
{
    //iterate thru all children
    auto children = rootNode->getChildren();
    for (std::vector<Node *>::iterator it = children.begin(); it != children.end(); ++it)
    {
        cocos2d::Node *node = *it;

        if (node)
        {
            //bind events
            cocos2d::ui::Button *button = dynamic_cast<cocos2d::ui::Button *>(node);
            if (button)
            {
                button->setTitleText("");
                button->setScale(0.90);
                Vec2 buttonPosition = button->getPosition();

                if (_leftMostX >= button->getBoundingBox().origin.x)
                {
                    _leftMostX = button->getBoundingBox().origin.x;
                }

                if (button->getPosition().x + button->getBoundingBox().size.width <= _rightMostX)
                {
                    _rightMostX = button->getPosition().x + button->getBoundingBox().size.width;
                }

                if (button->getName() == "alphabet" || button->getName() == "writing" || button->getName() == "grammar")
                {
                    button->setPosition(Vec2(buttonPosition.x, buttonPosition.y - 40));
                }
                else if (button->getName() == "map" || button->getName() == "library" || button->getName() == "shapes")
                {
                    button->setPosition(Vec2(buttonPosition.x, buttonPosition.y - 70));
                }

                if (button->getName() == "library")
                {
                    button->setName("story-catalogue");
                    button->addTouchEventListener(CC_CALLBACK_2(MainMenuHome::storySelected, this));
                }
                else if (button->getName() == "map")
                {
                    button->addTouchEventListener(CC_CALLBACK_2(MainMenuHome::mainMenuMapSelected, this));
                }
                else
                {
                    button->addTouchEventListener(CC_CALLBACK_2(MainMenuHome::mainMenuSelected, this));
                }
            }
            else
            {
                //bind events
                cocos2d::ui::TextField *textTitle = dynamic_cast<cocos2d::ui::TextField *>(node);

                if (textTitle)
                {
                    std::string translatedString;
                    std::string fileName = "main_menu_title";
                    bool isBilingual = localeCode != "en";
                    string gameName = textTitle->getName().c_str();
                    string mainMenuEnglishText = "";
                    int engLabelOffset = isBilingual ? 0 : 50; // to remove the extra space between image and text
                    if (gameName == "alphabet")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 0, false);
                        translatedString = readTitleFromJson(fileName, 0, isBilingual);
                    }
                    else if (gameName == "shapes")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 1, false);
                        translatedString = readTitleFromJson(fileName, 1, isBilingual);
                    }
                    else if (gameName == "writing")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 2, false);
                        translatedString = readTitleFromJson(fileName, 2, isBilingual);
                    }
                    else if (gameName == "library")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 3, false);
                        translatedString = readTitleFromJson(fileName, 3, isBilingual);
                    }
                    else if (gameName == "grammar")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 4, false);
                        translatedString = readTitleFromJson(fileName, 4, isBilingual);
                    }
                    else if (gameName == "map")
                    {
                        mainMenuEnglishText = readTitleFromJson(fileName, 5, false);
                        translatedString = readTitleFromJson(fileName, 5, isBilingual);
                    }

                    textTitle->setText(mainMenuEnglishText);
                    textTitle->setTextHorizontalAlignment(TextHAlignment::CENTER);
                    textTitle->setTextVerticalAlignment(TextVAlignment::TOP);
                    textTitle->setEnabled(false);
                    textTitle->setTouchEnabled(false);
                    textTitle->setFocusEnabled(false);
                    textTitle->setFontName("arial");
                    textTitle->setTextColor(Color4B(Color3B::WHITE));
                    textTitle->setFontSize(70);
                    Vec2 englishTextPosition = textTitle->getPosition();

                    if (gameName == "alphabet" || gameName == "writing" || gameName == "grammar")
                    {
                        textTitle->setPosition(Vec2(englishTextPosition.x, englishTextPosition.y + 130 - engLabelOffset));
                    }
                    else if (gameName == "map" || gameName == "library" || gameName == "shapes")
                    {
                        textTitle->setPosition(Vec2(englishTextPosition.x, englishTextPosition.y + 100 - engLabelOffset));
                    }

                    if (isBilingual)
                    {
                        Label *mainMenuLocaleText = Label::createWithSystemFont(translatedString, "arial", 70);
                        mainMenuLocaleText->setPosition(Vec2(textTitle->getPositionX(), textTitle->getPositionY() - 90));
                        mainMenuLocaleText->setColor(Color3B::WHITE);
                        this->addChild(mainMenuLocaleText);
                    }
                }
            }
        }
    }
}

std::string MainMenuHome::readTitleFromJson(std::string fileName, int pos, bool isBilingual)
{
    std::string contents = FileUtils::getInstance()->getStringFromFile("config/misc_strings.json");
    rapidjson::Document mapPlaces;
    if (!mapPlaces.Parse<0>(contents.c_str()).HasParseError()) // to validate the structure of JSON file
    {
        if (isBilingual)
        {
            const rapidjson::Value &placeNameLoc = mapPlaces[fileName.c_str()][localeCode.c_str()]; // get the list of locale specific strings
            assert(placeNameLoc.IsArray());
            return placeNameLoc[pos].GetString();
        }
        const rapidjson::Value &placeNameEng = mapPlaces[fileName.c_str()]["en"];
        assert(placeNameEng.IsArray());
        return placeNameEng[pos].GetString();
    }
}

void MainMenuHome::mainMenuSelected(Ref *pSender, ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button *clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    CCLOG("main menu buttons :: %s", (clickedButton->getName()).c_str());
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

        firebase_instance.pushToCurrentEvent(clickedButton->getName());
        addGreyLayer();
        clickedButton->setEnabled(false);
        _gameNameToNavigate = clickedButton->getName();
        localStorageSetItem("mainMenuHomeSelectedItem", _gameNameToNavigate);

        this->scheduleOnce(schedule_selector(MainMenuHome::transition), 1.5);
        break;
    }

    case ui::Widget::TouchEventType::CANCELED:
        break;
    default:
        break;
    }
}

void MainMenuHome::mainMenuMapSelected(Ref *pSender, ui::Widget::TouchEventType eEventType)
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

        firebase_instance.pushToCurrentEvent(clickedButton->getName());
        addGreyLayer();
        clickedButton->setEnabled(false);
        _gameNameToNavigate = clickedButton->getName();
        localStorageSetItem("mainMenuHomeSelectedItem", _gameNameToNavigate);
        this->scheduleOnce(schedule_selector(MainMenuHome::transitionToMap), 1.5);
        break;
    }

    case ui::Widget::TouchEventType::CANCELED:
        break;
    default:
        break;
    }
}

void MainMenuHome::addGreyLayer()
{
    if (!_greyLayer)
    {
        //later customize and add image
        Size visibleSize = Director::getInstance()->getVisibleSize();
        _greyLayer = LayerGradient::create(Color4B(0, 0, 0, 100), Color4B(15, 15, 15, 100));
        _greyLayer->setContentSize(visibleSize);
        addChild(_greyLayer, 3);

        Node *animationNode = CSLoader::createNode("loading/animation_4.csb");
        animationNode->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        animationNode->setAnchorPoint(Vec2(0.5, 0.5));
        _greyLayer->addChild(animationNode, 1);

        cocostudio::timeline::ActionTimeline *_animationTimeLine = CSLoader::createTimeline("loading/animation_4.csb");
        animationNode->runAction(_animationTimeLine);
        _animationTimeLine->gotoFrameAndPlay(0);

        auto _listener = EventListenerTouchOneByOne::create();
        _listener->setSwallowTouches(true);
        _listener->onTouchBegan = CC_CALLBACK_2(MainMenuHome::greyLayerTouched, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, _greyLayer);
    }
}

bool MainMenuHome::greyLayerTouched(Touch *touch, Event *event)
{
    return true;
}

void MainMenuHome::transition(float dt)
{
    Director::getInstance()->replaceScene(ScrollableGameMapScene::createSceneWithSubGames(_gameNameToNavigate));
}

void MainMenuHome::storyTransition(float dt)
{
    Director::getInstance()->replaceScene(LevelHelpScene::createScene(_gameNameToNavigate));
}

void MainMenuHome::storySelected(Ref *pSender, ui::Widget::TouchEventType eEventType)
{
    cocos2d::ui::Button *clickedButton = dynamic_cast<cocos2d::ui::Button *>(pSender);
    switch (eEventType)
    {
    case ui::Widget::TouchEventType::BEGAN:
        break;
    case ui::Widget::TouchEventType::MOVED:
        break;
    case ui::Widget::TouchEventType::ENDED:
    {
        firebase_instance.pushToCurrentEvent(clickedButton->getName());
        addGreyLayer();
        clickedButton->setEnabled(false);
        _gameNameToNavigate = clickedButton->getName();
        localStorageSetItem("mainMenuHomeSelectedItem", _gameNameToNavigate);

        this->scheduleOnce(schedule_selector(MainMenuHome::storyTransition), 1.5);

        break;
    }

    case ui::Widget::TouchEventType::CANCELED:
        break;
    default:
        break;
    }
}

void MainMenuHome::transitionToMap(float dt)
{
    Director::getInstance()->replaceScene(MapScene::createScene());
}
