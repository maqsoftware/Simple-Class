//
//  MenuContext.h
//  safari
//
//  Created by Srikanth Talapadi on 19/07/16.
//
//

#ifndef MenuContext_h
#define MenuContext_h

#include "cocos2d.h"
#include <sstream>
#include "ui/CocosGUI.h"
#include "../alphamon/HPMeter.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "ScoreBoardContext.h"
#include "../lang/SafariAnalyticsManager.h"
#include "../lang/Lesson.h"
#include "../mini_games/Dash.h"
#include "../mini_games/EndlessRunner.h"
#include "../util/Calculator.h"
#include "../hero/RPGConfig.h"

#define GAME_MAP_MENU "GameMapScene"
#define HELP_MENU "HelpScene"
#define EXIT_MENU "StartMenuScene"
#define MAP_MENU "MapScene"
#define BOOK_MENU "BookScene"
#define BAG_PACK_MENU "BagpackScene"

class ScrollableGameMapScene;
class MenuContext : public cocos2d::Node {
    static bool _gameIsStatic;

public:
    enum MATH_OPERATION
    {
        IDENTIFY = 0,
        ADD = 1,
        SUBTRACT = 2,
        MULTIPLY = 3
    };
    
    static MenuContext* create(Node *main, std::string const& gameName = "", bool lauchCustomEventOnExit = false, std::string const& sceneName = "");
    
    void pickAlphabet(char targetAlphabet, char chosenAlphabet, bool choose = true, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);
    void pickWord(std::string const& targetWord, std::string const& chosenString, bool choose = true);
    void writeAlphabet(char targetAlphabet, bool correct);
    void pickNumber(int targetNumber, int chosenNumber, MenuContext::MATH_OPERATION op);
    int getPoints();
    void addPoints(int points);
    void finalizePoints();
    static const std::string LANG;
    Node* jumpOut(std::string nodeCsbName, float duration, cocos2d::Vec2 position, std::string animationName = "");
    void showStartupHelp(std::function<void()> callback = nullptr);
    void showScore();
    bool isGamePaused();
    void exitMultiPlayerGame();
    void sendMessageToPeer(std::string message);
    std::vector<std::string> split(std::string s, char delim);
    std::vector<std::string> splitRegEx(std::string s, std::string delim);
    int getCurrentLevel();
    void setCurrentLevel(int level);
    int getMaxPoints();
    void setMaxPoints(int maxPoints);
    
    
    std::vector<cocos2d::Point> getPolygonPointsForSprite(cocos2d::Sprite* node, std::string fileName, float threshHold);
    
    std::vector<std::vector<cocos2d::Point>> getTrianglePointsForSprite(cocos2d::Sprite* node, std::string fileName, float threshHold);
    
    static void pronounceWord(std::string word, bool shouldReplaceWithSpace = true);
    static void pronounceSplitFileFromStory(std::string fileName);
    static void pronounceHashedText(std::string word, bool shouldReplaceWithSpace = true);
    static std::string _lastAudioId;
    static bool isGameStatic();
	void wordPairList(std::string question, std::string answer = "it is a word",bool isInitialSyllable = false);
	void showAnswer(std::string, std::string header);
    void onExitTransitionDidStart() override;
    void closeAnswerLayer(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    cocos2d::Rect getBoundingBox(cocos2d::Sprite* node) const;
    std::vector<cocos2d::Point> getPolygonPointsForSprite1(cocos2d::Sprite* node);
    
    int playStoryAudio(std::string soundFile);
    void stopStoryAudio();
    
    template <typename T>
    static inline std::string to_string(T value)
    {
        std::ostringstream os ;
        os << value ;
        return os.str() ;
    }
    static bool _isInStoryDialogSpeechCurrentlyActive;
    
CC_CONSTRUCTOR_ACCESS:
    MenuContext();
    virtual ~MenuContext();
    bool init(Node* main);
    bool onChimpTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onChimpTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void transitToScrollableGameMap();
    void launchGame(std::string const& gameName);
    void removeMenu();
    
    void createNewUnlockStoryDocument(std::string const& storyToUnlock);
    void createNewlockStoryDocument(std::string const& storyTolock);
    
    static void launchGameFromJS(std::string gameName);
    static void launchGameFinally(std::string gameName);
    
protected:
    int _points;
    bool _menuSelected;
    bool _gameIsPaused;
    bool _launchCustomEventOnExit;
	bool _calcFlag;
    int _currentLevel;
    int _maxPoints;
    cocos2d::Node* _main;
    cocos2d::Label* _label;
    cocos2d::ui::Slider * _pointMeter;
    cocos2d::ui::Button* _menuButton;
    cocos2d::ui::Button* _exitMenu;
    cocos2d::ui::Button* _helpMenu;
    cocos2d::ui::Button* _mapMenu;
    cocos2d::ui::Button* _bookMenu;
    cocos2d::ui::Button* _gamesMenu;
	cocos2d::ui::Button* _settingMenu;
	cocos2d::ui::Button* _closeButton;
    cocos2d::Node* _photoMenu;
    cocos2d::LayerColor* _greyLayer;
    cocos2d::Node* _chimp;
    cocos2d::ParticleSystem* _ps;
    int _chimpAudioId;
    void expandMenu(Ref* pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void pauseNodeAndDescendants(Node *pNode);
    void resumeNodeAndDescendants(Node *pNode);
    void showMap(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void showBook(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void showGamesMenu(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void showMainHomeMenu(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void changePhoto(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void showHelp(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void showMapMenu(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void waitForAudioLoad(std::string audioFileName, std::function<void(bool isSuccess)>callback);
    void chimpHelp();
    void tellHelp();
    void stopTellHelp();
    void addGreyLayer();
    void increasePoints(int points);
    void happyFace();
    void sadFace();
    void normalFace();
	void addCalculator(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void removeMenuOnly();

    void unlockNextStory();
    void createUnlockStoryDocument(std::string const& storyToUnlock);
    
    
	void showRewards();
    
    cocostudio::timeline::SkeletonNode* _character;
    
	cocos2d::Layer * _showAnswerLayer;

    std::string gameName;
    std::string sceneName;
    std::function<void()> _startupCallback;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)  
	void videoEventCallback(Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType);
#endif 

	void videoPlayStart(std::string gameName);
	void videoPlayOverCallback();

    bool onTouchBeganOnCharacter(cocos2d::Touch *touch, cocos2d::Event *event);
    
    cocos2d::ui::Button* createMenuItem(std::string const& normalImage,
                        std::string const& selectedImage ,
                        std::string const& disableImage,
                        float xPosOffSet);

    cocos2d::ClippingNode *createMaskedMenuItem(std::string const& normalImage,
                                                std::string const& selectedImage,
                                                std::string const& disableImage,
                                                float xPosOffSet);

    cocos2d::Node *createAvatarMenuItem(std::string const& normalImage,
                                        std::string const& selectedImage,
                                        std::string const& disableImage,
                                        float xPosOffSet);

    std::map<std::string, std::string> _wordsList;
	std::vector<std::string> _listOfWords;
	std::map<std::string, std::vector<std::string>> _listOfInitialSyllableWords;
    
    
};

#endif /* MenuContext_h */
