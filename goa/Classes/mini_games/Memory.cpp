//
//  Memory.cpp
//
//  Created by Jyoti Prakash on 19/09/16.
//
//

#include "Memory.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "SimpleAudioEngine.h"
#include "../lang/LangUtil.h"
#include "../menu/StartMenuScene.h"
#include "../lang/TextGenerator.h"
#include "../util/CommonText.h"
#include "../util/MatrixUtil.h"


USING_NS_CC;

//TextGenerator::getInstance()->getSynonyms(9)
//TextGenerator::getInstance()->getAntonyms(9)
//TextGenerator::getInstance()->getHomonyms(9)



Memory::Memory() :
	_currentNest(0),
	_currentClickedPair(3),
	_currentSelectedNestNames(2),
	_level(0),
	_finalGridIds(0),
	_activeNestIds(25),
	_chickenTimeline(25),
	_gridTwoByTwoIds(4),
	_gridTwoByThreeIds(6),
    _gridThreeByFourIds(12),
    _gridThreeBySixIds(18),
    _gridFourByFiveIds(20),
    _gridFourBySixIds(24),
	_counter(0),
	_helpflag(0),
	_right(0),
	_wrong(0)
{

}

Memory::~Memory() {
	_eventDispatcher->removeCustomEventListeners("multipleChoiceQuiz");
}


Scene *Memory::createScene() {

	auto scene = Scene::create();
	auto layer = Memory::create();

	scene->addChild(layer);
	layer->_menuContext = MenuContext::create(layer, Memory::classname());
	scene->addChild(layer->_menuContext);
	
	return scene;

}

Memory *Memory::create() {
	
	Memory *memory = new (std::nothrow) Memory();
	if (memory && memory->init()) {
		memory->autorelease();
		return memory;
	}
	CC_SAFE_DELETE(memory);
	return nullptr;

}

void Memory::onEnterTransitionDidFinish() {
	_eventDispatcher->addCustomEventListener("multipleChoiceQuiz", CC_CALLBACK_1(Memory::gameBegin, this));
	_finalGridIds.resize(0);
	auto complexity = (float)_menuContext->getCurrentLevel() / 50.0f;
	if (complexity >= 0.0f && complexity <= 0.15f) {
		_gridTwoByTwoIds.resize(_gridTwoByTwoIds_Size);
		_gridTwoByTwoIds = { 9, 10, 15, 16 };
		_pairCount = 2;
		_finalGridIds = _gridTwoByTwoIds;
		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);

		_lesson.getMultiChoices(_pairCount, 0);
	}

	if (complexity >= 0.16f && complexity <= 0.30f) {
		_gridTwoByThreeIds.resize(_gridTwoByThreeIds_Size);
		_gridTwoByThreeIds = { 8, 9, 10, 14, 15, 16 };
		_pairCount = 3;
		_finalGridIds = _gridTwoByThreeIds;
		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);
		_lesson.getMultiChoices(_pairCount, 0);
	}

	if (complexity >= 0.31f && complexity <= 0.45f) {
		_gridThreeByFourIds.resize(_gridThreeByFourIds_Size);
		_gridThreeByFourIds = { 8, 9, 10, 11, 14, 15, 16, 17, 20, 21, 22, 23 };
		_pairCount = 6;
		_finalGridIds = _gridThreeByFourIds;

		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);
		_lesson.getMultiChoices(_pairCount, 0);
	}

	if (complexity >= 0.46f && complexity <= 0.60f) {
		_gridThreeBySixIds.resize(_gridThreeBySixIds_Size);
		_gridThreeBySixIds = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
		_pairCount = 9;
		_finalGridIds = _gridThreeBySixIds;

		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);
		_lesson.getMultiChoices(_pairCount, 0);
	}

	if (complexity >= 0.61f && complexity <= 0.80f) {
		_gridFourByFiveIds.resize(_gridFourByFiveIds_Size);
		_gridFourByFiveIds = { 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23 };
		_pairCount = 10;
		_finalGridIds = _gridFourByFiveIds;
		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);
		_lesson.getMultiChoices(_pairCount, 0);
	}

	if (complexity >= 0.81f && complexity <= 1.0f) {
		_gridFourBySixIds.resize(_gridFourBySixIds_Size);
		_gridFourBySixIds = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,  13, 14, 15, 16,  17, 18, 19, 20, 21, 22, 23, 24 };
		_pairCount = 12;
		_finalGridIds = _gridFourBySixIds;

		_right = _pairCount * 2;
		_menuContext->setMaxPoints(_right);
		_lesson.getMultiChoices(_pairCount, 0);
	}


}

void Memory::gameBegin(cocos2d::EventCustom *eventCustom) {

	CCLOG("onLessonReady begin");
	std::string* buf = static_cast<std::string*>(eventCustom->getUserData());
	CCLOG("onLessonReady to unmarshallMultiChoices");
	vector<Lesson::MultiChoice> vmc = Lesson::unmarshallMultiChoices(buf);
	_vmc = vmc;
	for (int i = 0; i < vmc.size(); i++) {
		CCLOG("vmc : %d question -> %s , correctAnswer index : %d  , correctAnswer value : %s", i, vmc[i].question.c_str(), vmc[i].correctAnswer, vmc[i].answers[vmc[i].correctAnswer].c_str());
		for (int j = 0; j < vmc[i].answers.size(); j++) {
			CCLOG("%d answerchoice -> %s\n", j, vmc[i].answers[j].c_str());
		}
	}

	_wrong = 0;
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("balloonhero/balloonhero.plist");
	//_sceneNumber = RandomHelper::random_int(1, 2);
	//_sceneNumber = 3;

	//auto x = _menuContext->getCurrentLevel();

	_data.clear();
	
	auto mapping = MatrixUtil::questionToAnswerMapping(vmc);
	_data = mapping;
	_hint = LangUtil::getInstance()->translateString(vmc[0].help);

	//_menuContext->setCurrentLevel(_menuContext->getCurrentLevel() + 1);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	_memoryfarm = CSLoader::createNode("memoryfarm/memoryfarm.csb");

	_memoryfarm->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	_memoryfarm->setAnchorPoint(Vec2(0.5, 0.5));
	addChild(_memoryfarm);

	generateGrid(_finalGridIds);

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("memoryfarm/memoryfarm.plist");

	for (int i = 0; i < vmc.size();i++) {
		_data_key.push_back(vmc[i].question);
	}

	for (int i = 0; i < vmc.size(); i++) {
		_data_value.push_back(vmc[i].answers[vmc[i].correctAnswer]);
	}


	generateRandomNumbers();

	_chickenTimeline.resize(25);
	//_chickenTimelineTemp = CSLoader::createTimeline("memoryfarm/chicken.csb");
	//_memoryfarm->getChildByName("background")->getChildByName("nest1")->getChildByName("chicken")->runAction(_chickenTimelineTemp);
	//_chickenTimelineTemp->play("fly", false);

	int nestsCount = _pairCount * 2;
	int j = 0;
	for (int i = 0; i < nestsCount; i++) {

		std::ostringstream sstreamc;
		sstreamc << "nest" << _finalGridIds[_currentNest];
		std::string queryc = sstreamc.str();

		_activeNestIds[_finalGridIds[_currentNest]] = _finalGridIds[_currentNest];
		_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->setAnchorPoint(Vec2(0.5, 0.5));


		_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->setTag(_finalGridIds[_currentNest]);

		std::string labelName;

		_chickenTimeline[_finalGridIds[_currentNest]] = CSLoader::createTimeline("memoryfarm/chicken.csb");
		_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("chicken")->runAction(_chickenTimeline[_finalGridIds[_currentNest]]);


		if (i == _pairCount) {
			generateRandomNumbers();
		}

		if (i < _pairCount) {

			labelName = _data_key[_randomIndex[i]];
		}
		else {

			labelName = _data_value[_randomIndex[j]];
			j++;
		}

		auto nestWidth = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->getContentSize().width;

		auto nestHeight = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->getContentSize().height;


		auto label = CommonText::create();
		label->setString(labelName);
		label->setFontSize(100);
		label->setFontName(LangUtil::getInstance()->getFontFile());
		label->setPosition(Vec2(nestWidth * 0.47, nestHeight * 0.48));
		label->setAnchorPoint(Vec2(0.5, 0.5));
		label->setTextColor(Color4B::BLUE);
		label->setName(labelName);


		_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->addChild(label, 0);

		//_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("egg")->setVisible(true);

		_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("egg")->setPositionY(visibleSize.height * 0.05);


		label->setVisible(false);

		setupTouch();
	}

	//CCLOG("asjdasd : ", _randomIndex);

	////////////////////help


	if (_menuContext->getCurrentLevel() == 1) {

		auto box1 = _memoryfarm->getChildByName("background")->getChildByName("nest9");
		
		auto matchNode = box1->getChildByName("nestfront")->getChildren();
		auto stringOne = matchNode.at(0)->getName();

		auto stringTwo = _data[stringOne];
		int ids[4] = { 10, 15, 16, NULL };
		int  i;
		for (i = 0; i < 3; i++) {

			std::ostringstream sstreamc;
			sstreamc << "nest" << ids[i];
			std::string queryc = sstreamc.str();


			auto tempBox = _memoryfarm->getChildByName("background")->getChildByName(queryc);

			auto matchNode = tempBox->getChildByName("nestfront")->getChildren();
			auto tempString = matchNode.at(0)->getName();

			if (tempString == stringTwo) {
				break;
			}

		}

		std::ostringstream sstreamd;
		sstreamd << "nest" << ids[i];
		std::string queryd = sstreamd.str();

		auto box2 = _memoryfarm->getChildByName("background")->getChildByName(queryd);


		box1pos = box1->getPosition() + Vec2(visibleSize.width * 0.03, visibleSize.height * 0.05);

		box2pos = box2->getPosition() + Vec2(visibleSize.width * 0.03, visibleSize.height * 0.05);


		if (Director::getInstance()->getVisibleSize().width == 2560) {
			box1pos = box1->getPosition() + Vec2(visibleSize.width * 0, visibleSize.height * 0.05);

		}

		help1 = HelpLayer::create(Rect(box1pos.x + origin.x, box1pos.y + origin.y, box1->getChildByName("nestfront")->getContentSize().width, box1->getChildByName("nestfront")->getContentSize().height), Rect(0, 0, 0, 0));

		//help1->setAnchorPoint(Vec2(0.5,0.5));
		help1->click(Vec2(box1pos));
		
		//help1->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));

		this->addChild(help1);
	}

	///////////////////////help end
	_counter = 0;
	_helpflag = 0;

	addGrid(visibleSize.width/2, visibleSize.height * 0.10, visibleSize.width / 2, visibleSize.height * 0.95);

	cocos2d::ui::Text * _label;

	//_hint = "Catch the antonyms";
	_label = CommonText::create();
	_label->setFontName(LangUtil::getInstance()->getFontFile());
	_label->setString(_hint);
	_label->setFontSize(70);

	_label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.95));
	_label->setAnchorPoint(Vec2(0.5, 0.5));
	_label->setName("hintLabel");
	_label->setTextColor(Color4B::BLUE);
	_label->setColor(Color3B::BLACK);

	this->addChild(_label, 20);


}
bool Memory::init() {

	if (!Layer::init())
	{
		return false;
	}

	return true;

}



void Memory::setupTouch() {
	
	   // CCLOG("NEST %d setuptouch done", nestIndex);
		std::ostringstream sstreamc;
		sstreamc << "nest" << _finalGridIds[_currentNest];
		std::string queryc = sstreamc.str();

		auto _nest = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");
		
		//nestj->setPosition(Vec2(1000,1000));
		//CC_SPRITE_DEBUG_DRAW;
		_nest->setAnchorPoint(Vec2(0.5, 0.5));
		//_nest->setVisible(true);
		CCLOG("nest xpos : %f", _nest->getPosition().x);
		CCLOG("nest ypos : %f", _nest->getPosition().y);
		CCLOG("nest anchor : %f  :   %f ", _nest->getAnchorPoint().x, _nest->getAnchorPoint().y);
		//nestj->getChildByName("chicken")->setVisible(false);
		//nestj->setPosition(Vec2(500, 500));
		//auto nest = _nest->getChildByName("nestfront");
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(Memory::onTouchBegan, this);
		listener->onTouchEnded = CC_CALLBACK_2(Memory::onTouchEnded, this);
		listener->onTouchMoved = CC_CALLBACK_2(Memory::onTouchMoved, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _nest);
		_currentNest++;
	
}


bool Memory::onTouchBegan(Touch* touch, Event* event) {


	auto target = event->getCurrentTarget();
	Point locationInNode = target->getParent()->convertToNodeSpace(touch->getLocation());
	
	//Size s = target->getContentSize();
	//Rect rect = Rect(0, 0, s.width, s.height);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	auto bb = target->getBoundingBox();

	if (target->getBoundingBox().containsPoint(locationInNode))
	{
		

		if (_menuContext->getCurrentLevel() == 1 && _helpflag == 0 && _counter == 0) {
			this->removeChild(help1);
			
			auto box1 = _memoryfarm->getChildByName("background")->getChildByName("nest9");

			auto matchNode = box1->getChildByName("nestfront")->getChildren();
			auto stringOne = matchNode.at(0)->getName();

			auto stringTwo = _data[stringOne];
			int ids[4] = {10, 15, 16, NULL};
			int  i;
			for (i = 0; i < 3; i++) {

				std::ostringstream sstreamc;
				sstreamc << "nest" << ids[i];
				std::string queryc = sstreamc.str();


				auto tempBox = _memoryfarm->getChildByName("background")->getChildByName(queryc);

				auto matchNode = tempBox->getChildByName("nestfront")->getChildren();
				auto tempString = matchNode.at(0)->getName();

				if (tempString == stringTwo) {
					break;
				}

			}



			std::ostringstream sstreamd;
			sstreamd << "nest" << ids[i];
			std::string queryd = sstreamd.str();

			auto box2 = _memoryfarm->getChildByName("background")->getChildByName(queryd);


			box1pos = box1->getPosition() + Vec2(visibleSize.width * 0.03, visibleSize.height * 0.05);
			box2pos = box2->getPosition() + Vec2(visibleSize.width * 0.03, visibleSize.height * 0.05);

			if (Director::getInstance()->getVisibleSize().width == 2560) {
				box2pos = box2->getPosition() + Vec2(visibleSize.width * 0, visibleSize.height * 0.05);

			}

			help2 = HelpLayer::create(Rect(box2pos.x + origin.x, box2pos.y + origin.y, box2->getChildByName("nestfront")->getContentSize().width, box2->getChildByName("nestfront")->getContentSize().height), Rect(0, 0, 0, 0));


			help2->click(Vec2(box2pos));


			this->addChild(help2);
			_helpflag = 1;

		}

		if (_menuContext->getCurrentLevel() == 1 && _helpflag == 1 && _counter == 1) {
			this->removeChild(help2);
			_helpflag++;
		}


		//CCLOG("NEST CLICKED : %d ", counter++);
		if (_counter < 2) {

			auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
			audio->playEffect("sounds/memory/tap.ogg", false);

			std::ostringstream sstreamc;
			sstreamc << "nest" << target->getTag();
			std::string queryc = sstreamc.str();

			auto child = target->getChildren();
			std::string childName = child.at(0)->getName();

			_currentSelectedNestNames[_counter] = childName;
			//target->getChildByName("Chimple")->setVisible(true);
			auto pauseCurrentTarget = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");
	_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->getChildByName(childName)->setVisible(true);
	
	Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(pauseCurrentTarget);
	
	

	_currentClickedPair[_counter] = target->getTag();

	_chickenTimeline[_currentClickedPair[_counter]]->play("stand", false);
	

	bool flag;
	if (_counter == 1) {

		//pause listener on all nests which have non zero values
		
		pauseAllActiveListeners();
		flag = checkMatch();

		if (flag == true) {

			//make the hen fly

			auto flycallfunc = CallFunc::create([=] {
				chickenFly(); 
				
			});


			auto finishcallfunc = CallFunc::create([=] {
				
				if (_level == _pairCount) {

					auto createMatchLayer = CallFunc::create([=] {
					
						addGrid(visibleSize.width/2, visibleSize.height, visibleSize.width / 2, visibleSize.height / 2);
						auto heightpercent = 0.90;
						for (int i = 0; i < _data_key.size(); i++) {
							cocos2d::ui::Text * _label;

							
							_label = CommonText::create();
							_label->setFontName(LangUtil::getInstance()->getFontFile());

							auto first = _data_key[i];
							auto second = _data_value[i];

							if (first.length() != 0 && second.length() !=0 ) {
								_label->setString(first + " ------- " + second);
								_label->setFontSize(60);

								_label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * heightpercent));
								_label->setAnchorPoint(Vec2(0.5, 0.5));
								_label->setName("label");
								_label->setTextColor(Color4B::BLACK);
								_label->setColor(Color3B::BLACK);

								this->addChild(_label, 20);
								heightpercent -= 0.05;
							}
					}

					});

					auto showScore = CallFunc::create([=] {

						
						if (_wrong > _right/2) {
							_menuContext->addPoints(0.33 * _right);
						}

						if (_wrong <=  _right/2 && _wrong!=0) {
							_menuContext->addPoints(0.66 * _right);
						}

						if (_wrong == 0) {
							_menuContext->addPoints(_right);
						}

						//_menuContext->showScore();
                        auto hintLabel = getChildByName("hintLabel");
                        hintLabel->removeFromParent();
						_menuContext->showAnswer("wordPairs", _hint);
					});

					this->runAction(showScore);
				}
			});

			auto removelistenercallfunc = CallFunc::create([=] {

				removecurrentlabelsandlisteners();
				_activeNestIds[_currentClickedPair[0]] = 0;
				_activeNestIds[_currentClickedPair[1]] = 0;
			});

			auto resumeListenercallfunc = CallFunc::create([=] {

				resumeAllActiveListeners();
			});


			auto completeSequence = Sequence::create(flycallfunc, DelayTime::create(2.0), finishcallfunc, removelistenercallfunc, resumeListenercallfunc, NULL);

			this->runAction(completeSequence);



		}else{

		//if they doesnot matches resume the listener of current two selected nest
			
			

			auto resumelistenercallfunc = CallFunc::create([=] {

				std::ostringstream sstreamc;
				sstreamc << "nest" << _currentClickedPair[0];
				std::string queryc = sstreamc.str();

				auto pauseNode1 = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");

				Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(pauseNode1);

				
				std::ostringstream sstreamd;
				sstreamd << "nest" << _currentClickedPair[1];
				std::string queryd = sstreamd.str();

				auto pauseNode2 = _memoryfarm->getChildByName("background")->getChildByName(queryd)->getChildByName("nestfront");

				Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(pauseNode2);

				resumeAllActiveListeners();

			});

			auto makeChickenSit = CallFunc::create([=] {

				_chickenTimeline[_currentClickedPair[0]]->play("sit", false);
				_chickenTimeline[_currentClickedPair[1]]->play("sit", false);
				
				_wrong++;

			});

			auto setspritesinvisiblefunc = CallFunc::create([=] {
				
			std::ostringstream sstreamc;
			sstreamc << "nest" << _currentClickedPair[0];
			std::string queryc = sstreamc.str();

			_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->getChildByName(_currentSelectedNestNames[0])->setVisible(false);


			std::ostringstream sstreamd;
			sstreamd << "nest" << _currentClickedPair[1];
			std::string queryd = sstreamd.str();

		
			_memoryfarm->getChildByName("background")->getChildByName(queryd)->getChildByName("nestfront")->getChildByName(_currentSelectedNestNames[1])->setVisible(false); 
			
			});

			auto wrongSequence = Sequence::create(DelayTime::create(1.0) , makeChickenSit, setspritesinvisiblefunc, resumelistenercallfunc, NULL);

			this->runAction(wrongSequence);

		
	}
		
		_counter = -1;
		

		}

	
			
	}
	_counter++;
	
	
		return true; // to indicate that we have consumed it.
	}

	return false; // we did not consume this event, pass thru.
}

void Memory::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
	//    CCLOG("onTouchEnded");
	
	auto target = event->getCurrentTarget();
	Point locationInNode = target->getParent()->convertToNodeSpace(touch->getLocation());


}

void Memory::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
	//    CCLOG("onTouchMoved");
	
}

void Memory::pauseAllActiveListeners() {


	for (int i = 0; i < _pairCount * 2; i++) {

		if (_activeNestIds[_finalGridIds[i]] == 0 || _currentClickedPair[0] == _finalGridIds[i] || _currentClickedPair[1] == _finalGridIds[i]) {
			continue;
		}


		std::ostringstream sstreamc;
		sstreamc << "nest" << _finalGridIds[i];
		std::string queryc = sstreamc.str();
		
		auto pauseNode = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");

		Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(pauseNode);


	}
	//_currentClickedPair[0] = _currentClickedPair[1] = 0;

}

void Memory::resumeAllActiveListeners() {



	for (int i = 0; i < _pairCount * 2; i++) {

		if (_activeNestIds[_finalGridIds[i]] == 0) {
			continue;
		}


		std::ostringstream sstreamc;
		sstreamc << "nest" << _finalGridIds[i];
		std::string queryc = sstreamc.str();

		auto pauseNode = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");

		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(pauseNode);


	}
	//_currentClickedPair[0] = _currentClickedPair[1] = 0;

}

bool Memory::checkMatch() {

	std::string str1 = _currentSelectedNestNames[0];
	std::string str2 = _currentSelectedNestNames[1];

	int strStatus = 1;

	for (int i = 0; i < _vmc.size(); i++) {
		if (str1 == _vmc[i].question) {
			strStatus = 0;
		}
	}

	if (strStatus == 0) {
		return checkAnswer(str1, str2);
	}
	else {
		return checkAnswer(str2, str1);
	}

	return false;
}


bool Memory::checkAnswer(string questionText, string answerText) {

	for (int i = 0; i < _vmc.size(); i++) {
		if (_vmc[i].question.compare(questionText) == 0) {
			if (_vmc[i].answers[_vmc[i].correctAnswer].compare(answerText) == 0) {
				_level++;
				return true;
			}
		}
	}
	return false;
}


void Memory::chickenFly() {

	std::ostringstream sstreamnest1;
	sstreamnest1 << "nest" << _currentClickedPair[0];
	std::string querynest1 = sstreamnest1.str();

	std::ostringstream sstreamnest2;
	sstreamnest2 << "nest" << _currentClickedPair[1];
	std::string querynest2 = sstreamnest2.str();


	_chickenTimeline[_currentClickedPair[0]]->play("fly", true);
	_chickenTimeline[_currentClickedPair[1]]->play("fly", true);

	auto moveTonest1 = MoveTo::create(4, Vec2(-3100, 1800));
	Sprite *chicken1 = (Sprite *)_memoryfarm->getChildByName("background")->getChildByName(querynest1)->getChildByName("chicken");
	chicken1->runAction(moveTonest1);

	auto moveTonest2 = MoveTo::create(4, Vec2(-3100, 1800));
	Sprite *chicken2 = (Sprite *)_memoryfarm->getChildByName("background")->getChildByName(querynest2)->getChildByName("chicken");
	chicken2->runAction(moveTonest2);

	_menuContext->wordPairList(_memoryfarm->getChildByName("background")->getChildByName(querynest1)->getChildByName("nestfront")->getChildren().at(0)->getName(), _memoryfarm->getChildByName("background")->getChildByName(querynest2)->getChildByName("nestfront")->getChildren().at(0)->getName());

}

void Memory::removecurrentlabelsandlisteners() {
	
	std::ostringstream sstreamc;
	sstreamc << "nest" << _currentClickedPair[0];
	std::string queryc = sstreamc.str();

	_memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront")->getChildByName(_currentSelectedNestNames[0])->setVisible(false);


	std::ostringstream sstreamd;
	sstreamd << "nest" << _currentClickedPair[1];
	std::string queryd = sstreamd.str();

	_memoryfarm->getChildByName("background")->getChildByName(queryd)->getChildByName("nestfront")->getChildByName(_currentSelectedNestNames[1])->setVisible(false);

	auto pauseNode1 = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getChildByName("nestfront");

	Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(pauseNode1);

	auto pauseNode2 = _memoryfarm->getChildByName("background")->getChildByName(queryd)->getChildByName("nestfront");

	Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(pauseNode2);

}

void Memory::generateRandomNumbers() {

	int a = _data_key.size();
	//std::vector<int> randomIndex;
	_randomIndex.clear();
	while (_randomIndex.size() != _data_key.size()) {
		bool duplicateCheck = true;
		int numberPicker = RandomHelper::random_int(0, a - 1);
		for (int i = 0; i < _randomIndex.size(); i++) {
			if (numberPicker == _randomIndex[i]) {
				duplicateCheck = false;
			}
		}
		if (duplicateCheck) {
			_randomIndex.push_back(numberPicker);
		}
	}
}



void Memory::generateGrid(std::vector<int> grid) {
	int found = 0;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	for (int i = 1; i <= 24; i++) {
		found = 0;
		for (int j = 0; j < grid.size(); j++) {
			if (i == grid[j]) {
				found = 1;
			}
		}
		

		std::ostringstream sstreamc;
		sstreamc << "nest" << i;
		std::string queryc = sstreamc.str();

		if (found == 0) {

			auto pauseNode = _memoryfarm->getChildByName("background")->getChildByName(queryc);
			pauseNode->setVisible(false);
			Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(pauseNode);

		}else{
		
			float currentX = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getPositionX();
			float currentY = _memoryfarm->getChildByName("background")->getChildByName(queryc)->getPositionY();

	 // _memoryfarm->getChildByName("background")->getChildByName(queryc)->setPositionX(currentX);	//_memoryfarm->getChildByName("background")->getChildByName(queryc)->setPositionY(currentX);

			switch (grid.size()) {

			
			case 6:
				_memoryfarm->getChildByName("background")->getChildByName(queryc)->setPositionX(currentX + (0.07 * visibleSize.width));
				break;
			case 12:
				//_memoryfarm->getChildByName("background")->getChildByName(queryc)->setPositionY(currentY + (0.05 * visibleSize.height));
				
				//_memoryfarm->getChildByName("mainground")->getChildByName("Panel_1")->setPositionY(currentY + (0.09 * visibleSize.height));
				//_memoryfarm->getChildByName("mainground")->getChildByName("Panel_2")->setPositionY(currentY + (0.09 * visibleSize.height));
				//_memoryfarm->getChildByName("mainground")->getChildByName("Panel_3")->setPositionY(currentY + (0.09 * visibleSize.height));
				
				break;
			case 18:break;
			case 20:
				_memoryfarm->getChildByName("background")->getChildByName(queryc)->setPositionX(currentX + (0.07 * visibleSize.width));
				break;
			

			}

		}
		
	}

}




void Memory::addGrid(float sizex, float sizey, float posx, float posy) {

	auto gridPanel = Sprite::create();
	gridPanel->setTextureRect(Rect(0, 0, sizex, sizey));
	gridPanel->setColor(Color3B::WHITE);
	gridPanel->setPosition(Vec2(posx, posy));
	gridPanel->setOpacity(60);
	gridPanel->setName("gridpanel");
	addChild(gridPanel, 1);

}
