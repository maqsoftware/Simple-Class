//
//  DoorNode.h
//  goa



#ifndef __DOOR_SCENE_H__
#define __DOOR_SCENE_H__

#include "cocos2d.h"
#include "../menu/MenuContext.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "../ext/WordSceneLipiTKNode.h"

class DoorNode;
class Door : public cocos2d::Layer
{
public:
	Door();
	~Door();
	virtual bool init();
	static cocos2d::Scene* createScene();
	static Door * create();
	void onEnterTransitionDidFinish();
	void clearScreen(float dt);
	void showScore(float dt);
	void gameHelpLayer();
	float extraX;
	bool _type;
	std::string _randomWord;
	std::string _alphabet;
	int _score = 0;
	int _wordLength;
	std::string _myWord;
	cocos2d::Vector <cocos2d::Node *> _BoxRef;
	std::vector<std::string> _animalRef;
	void postTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint);
	virtual void postTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint);
	virtual void postTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint);
	void characterRecognisation(std::vector<string> str);
	DoorNode *_doorNode;
	cocos2d::Vector <DoorNode *> _doorNodeRef;
	MenuContext * menu;
	void clearScreen();
	void nextDoor();
	void createCanvas(int index);
	int _count = 0;
protected:
	
	cocos2d::Node* _car;
	cocos2d::DrawNode * _road;
	float _prevDegree;
	DoorNode * _DoorNodeLiPi;
};

#endif 



