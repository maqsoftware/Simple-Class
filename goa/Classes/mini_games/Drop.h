#ifndef __DROP_SCENE_H__
#define __DROP_SCENE_H__

#include "cocos2d.h"
#include "../menu/MenuContext.h"
#include "../menu/StartMenuScene.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "../puzzle/CharGenerator.h"
#include "../lang/TextGenerator.h"
#include "../menu/HelpLayer.h"
#include "../lang/Lesson.h"
#include "../util/MatrixUtil.h"

using namespace cocos2d;
using namespace std;


class Drop : public cocos2d::Layer
{
private:
	Lesson _lesson;
protected:
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	MenuContext *_menuContext;
	std::string _dropCurrentTheme = "";
	int letterHolderId = 0;
	Sprite* _removalPole;

	std::map<std::string, std::string> _scenePath;
	std::map<std::string, float> _sceneBasedNumericalVal;

	std::vector<Sprite*> _FallingLetter;
	std::vector<Sprite*> _letterHolderSpriteBin;
	std::vector<LabelTTF*> _basketBin;
	std::vector<Rect> _basketRect;
	std::vector<std::string> _wordOptionBin;
	std::vector<cocostudio::timeline::ActionTimeline *> _basketAnimBin;
	std::vector<Sprite *>_dropHeroTrailerImageBin;
	cocos2d::LabelTTF* _label = NULL;
	int _dropHelpSelector = 0, _speakerTouchCount = 0;
	Sprite* _basketImg;
	HelpLayer* _help;
	int _middleBasketIndex = NULL, _index =0, _pointCounter = 0;
	bool _helpFlag = false, _initObj = true, _flagForIndex= true, _speakerTouchFlag =true;
	float _gapBetweenTwoBasket;
	string _levelOneString, _wordToDisplay;
	std::string _labelPrefix = "";
	bool _isGameDone = false, _stopMovingHelpObject = false;
	std::vector<std::string> _choices;
	int _maxLengthOfChoice, _maxLengthOfLayoutWord;

	

public:
	~Drop();
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Drop);

	void update(float dt);
	void leftFloat(Sprite* spriteAlphabet, int time, float positionX, float positionY);
	void addEvents(Sprite* touchSprite);
	void letterAndHolderMaker(float dt);
	void removeLetterHolder();
	void removeHolderAnimation(Sprite* obj);
	void creatHelp( float gap);
	void gameStart(cocos2d::EventCustom *eventCustom);
	//void removeHolderAnimationForHero(std::tuple<Sprite*,Sprite*,int> tp);
	void basketLetterCollisionChecker();
	void removeHeroTrailer();
	void removeFallingLetter();
	void layingOutBasket(bool flag, float gap, std::string str, int i);
	void onEnterTransitionDidFinish();
	std::pair<int, int> levelAllInfo(int levelNum, int sceneRepetitionNo, int totalScene, int catagoryRepetitionNo, int totalcatagory);
	void addSpeaker(string word);
	bool isSpeakerAddLevel();
	void addSpeakerTouchEvents(Sprite *obj, string word);
	void wordPopUp();
	void setAllSpriteProperties(Sprite* object, int zOrder, float posX, float posY, bool visibility, float anchorPointX, float anchorPointY, float rotation, float scaleX, float scaleY);
	LabelTTF* setAllLabelProperties(std::string letter, int zOrder, float posX, float posY, bool visibility, float anchorPointX, float anchorPointY, float rotation, float scaleX, float scaleY, int labelSizeInPixel);
	std::pair<Sprite*, cocostudio::timeline::ActionTimeline*> setAnimationAndProperties(std::string csbString, float posX, float posY, int zOrder);
	std::string getConvertInUpperCase(std::string data);
	string getConvertVectorStringIntoString(vector<string> value);
	static const char* gameName() { return DROP.c_str(); }
	int getMaxWordLength(std::vector<string>);
};

#endif // __DROP_SCENE_H__
