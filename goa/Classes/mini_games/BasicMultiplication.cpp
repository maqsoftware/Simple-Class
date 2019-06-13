//
//  BasicMultiplication.cpp 
//  goa
//
//  Created by Karim Mirazul  on 20/05/17
//
//
#include "BasicMultiplication.h"
#include "../menu/HelpLayer.h"
#include "../util/CommonLabelTTF.h"

USING_NS_CC;

Scene* BasicMultiplication::createScene()
{
	auto scene = Scene::create();
	auto layer = BasicMultiplication::create();
	scene->addChild(layer);
	layer->_menuContext = MenuContext::create(layer, BasicMultiplication::gameName());
	scene->addChild(layer->_menuContext);
	return scene;
}

BasicMultiplication *BasicMultiplication::create() {
	BasicMultiplication *blast = new (std::nothrow) BasicMultiplication();
	if (blast && blast->init()) {
		blast->autorelease();
		return blast;
	}
	CC_SAFE_DELETE(blast);
	return nullptr;

}

bool BasicMultiplication::init()
{
	if (!Layer::init()) { return false; }

	return true;
}

void BasicMultiplication::onEnterTransitionDidFinish() {
	
	LearningPlay();
}

void BasicMultiplication::LearningPlay() {

	Node* bg = CSLoader::createNode("basicmultiplication/basicmultiplication.csb");
	addChild(bg);
	bg->setName("bg");
	topBoardSetting();

	if (Director::getInstance()->getVisibleSize().width > 2560) {
		auto myGameWidth = (Director::getInstance()->getVisibleSize().width - 2560) / 2;
		bg->setPositionX(myGameWidth);
	}

	_animationPath = { 
		{ 1,	"round_elep/round_elep.csb" },
		{ 2,	"Giraffe_animation/Giraffe_animation.csb" },
		{ 3,    "gorillacoin/gorillacoin.csb" },
		{ 4 ,	"lion_animation/lion.csb"},
		{ 5,	"owl/owlanimation.csb"},
		{ 6,	"zebra_animation/zebra_animation.csb"}
	};
	_animationName = _animationPath.at(RandomHelper::random_int(1,6));

}

void BasicMultiplication::topBoardSetting() {

	int row = _menuContext->getCurrentLevel();
	int column = RandomHelper::random_int(1, 10);

	if (row > 6)
		row = 6;
	
	Node* gridlearning = CSLoader::createNode("basicmultiplication/gridlearning.csb");
	addChild(gridlearning);
	gridlearning ->setName("gridlearning");
	
	_answer = row * column;
	_row = row;
	_column = column;

	std::ostringstream topBoardEquation;
	topBoardEquation << row << " x " << column << " = ";
	_topBoardEquation = topBoardEquation.str();

	auto board = getChildByName("gridlearning")->getChildByName("board");

	auto labelNumber = CommonLabelTTF::create(_topBoardEquation, "Helvetica", 150);
	labelNumber->setColor(Color3B::WHITE);
	labelNumber->setPosition(Vec2(board->getContentSize().width/2, board->getContentSize().height/2));
	board->addChild(labelNumber);
	labelNumber->setName("board");

	gridGrayAndListnerController("gridlearning",row , column);

	if (_menuContext->getCurrentLevel() == 1) {

		auto downGrid = getGridWithIndex("gridlearning",1, 1);
		auto help = HelpLayer::create(Rect(downGrid->getPositionX(), downGrid->getPositionY(), downGrid->getContentSize().width, downGrid->getContentSize().height), Rect(Director::getInstance()->getVisibleSize().width * 0.5,board->getPositionY(), board->getContentSize().width, board->getContentSize().height));
		help->click(Vec2(downGrid->getPositionX(), downGrid->getPositionY()));
		help->setName("helpLayer");
		this->addChild(help, 4);
	}

	_questionValue = getRandomValueRange(1, 10, 5);
}

void BasicMultiplication::gridGrayAndListnerController(const string gridName, int row , int column ) {
	int rowValue = 6;
	if (gridName != "gridlearning")
		rowValue = 8;

	// Here apply only on correct grid
	for (int rows = 1; rows <= rowValue; rows++) {
		for (int columns = 1; columns <= 10; columns++) {
			auto sprite = getGridWithIndex(gridName,rows, columns);

			if (rows <= row && columns <= column) {
				addEventsOnGrid(sprite);
			}
			else {
				if(sprite)
					sprite->setColor(Color3B(164, 150, 165));
			}
		}
	}
}

void BasicMultiplication::addEventsOnGrid(cocos2d::Sprite* object)
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(false);

	listener->onTouchBegan = [=](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		auto target = event->getCurrentTarget();
		Point locationInNode = touch->getLocation();
	
		if (target->getBoundingBox().containsPoint(locationInNode) && _optionTouchGrid ) {
			target->setColor(Color3B(255, 198, 44));

			auto audioBg = CocosDenshion::SimpleAudioEngine::getInstance();
			audioBg->playEffect("res/sounds/sfx/grid_pressed_sound.ogg", false);

			float popUpSize = 1.1, OriginalSize = 1;

			if (_gridName != "quiz") {
				topBoardEquationController((Sprite*)target);
			}
			else {
				popUpSize = 1.1, OriginalSize = 0.6899;
				topBoardEquationControllerQuiz((Sprite*)target);
			}
				
			auto action1 = ScaleTo::create(0.1, popUpSize);
			auto action2 = ScaleTo::create(0.1, OriginalSize);

			auto scaleAction = Sequence::create(action1, action2, NULL);
			target->runAction(scaleAction);

			if (getChildByName("helpLayer"))
				removeChildByName("helpLayer");

			return false;
		}
		return false;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, object);
}

void BasicMultiplication::topBoardEquationController(Sprite* target) {

	auto board = (CommonLabelTTF*)getChildByName("gridlearning")->getChildByName("board")->getChildByName("board");

	if (target->getTag() != 528) {
		_counter++;
		std::ostringstream topBoardEquation;
		topBoardEquation << _topBoardEquation << _counter;
		board->setString(topBoardEquation.str());
		target->setTag(528);
	}
	IndexValuePopup((Sprite*)target);

	if (_counter == _answer) {

		auto sequence = Sequence::create(
		
		CallFunc::create([=]() {	// Play Animation Method Calling
			playAnimationAnimal("gridlearning");
			_optionTouchGrid = false;
		}),

		DelayTime::create(2),		// DeayTime for 2 second
		
		CallFunc::create([=]() {	// CallFunc for grid movement ..

			auto gridPanel = getChildByName("gridlearning");
			
			auto action = MoveTo::create(1, Vec2(gridPanel->getPositionX() - 2560, gridPanel->getPositionY()));
			EaseBackIn *easeAction = EaseBackIn::create(action);
			gridPanel->runAction(easeAction);

		}),

		DelayTime::create(1.5),		// DelayTime for 1 second

		CallFunc::create([=]() {	// CallFunc for remove gridPanel
			removeChildByName("gridlearning");
			_optionTouchGrid = true;
			_gridName = "quiz";
		}),

		CallFunc::create([=]() {
			_counter = 0;
			QuizPlay();
		}),
		NULL);

		this->runAction(sequence);
	}
}

void BasicMultiplication::topBoardEquationControllerQuiz(Sprite* target) {

	auto board = (CommonLabelTTF*)getChildByName("quiz")->getChildByName("ans")->getChildByName("answer");

	if (target->getTag() != 528) {
		_counter++;
		std::ostringstream topBoardEquation;
		topBoardEquation << _counter;
		board->setString(topBoardEquation.str());
		target->setTag(528);
	}

	IndexValuePopup((Sprite*)target);

	if (_counter == _answer) {

		auto sequence = Sequence::create(

			CallFunc::create([=]() {	// Play Animation Method Calling
			playAnimationAnimal("grid");
			_optionTouchGrid = false;
		}),
			CallFunc::create([=]() {
			_counter = 0;
		}),
			NULL);

		this->runAction(sequence);
	}
}

void BasicMultiplication::playAnimationAnimal(const string gridName) {

	for (int rows = 1; rows <= 6; rows++) {
		for (int columns = 1; columns <= 10; columns++) {
			auto sprite = getGridWithIndex(gridName,rows, columns);

			if (rows <= _row && columns <= _column) {
			
				sprite->setColor(Color3B::WHITE);
				auto animationAnimalTimeline = CSLoader::createTimeline(_animationName);
				auto animationAnimal = CSLoader::createNode(_animationName);
				animationAnimal->runAction(animationAnimalTimeline);
				animationAnimal->setScale(0.5);

				auto size = sprite->getContentSize();

				sprite->addChild(animationAnimal);
				animationAnimal->setPosition(Vec2(sprite->getContentSize().width/2,sprite->getContentSize().height/2));
				animationAnimalTimeline->gotoFrameAndPlay(0, true);
			}
		}
	}
}

void BasicMultiplication::IndexValuePopup(Sprite* target) {

	string TargetName = target->getName();
	std::ostringstream gridIndexName;
	std::ostringstream gridIndexNamecolumn;

	gridIndexName << TargetName[0] << "r";
	auto rightIndexName = gridIndexName.str();

	if (TargetName.length() > 4) 
		gridIndexNamecolumn << 10 << "c";
	else
		gridIndexNamecolumn << TargetName[2] << "c";
	
	auto leftIndexName = gridIndexNamecolumn.str();

	TextFieldTTF* gridIndexRow, *gridIndexColumn;
	
	float popUpSize = 1.4, OriginalSize = 1;

	if (_gridName != "quiz") {
		gridIndexRow = (TextFieldTTF*)getChildByName("gridlearning")->getChildByName(rightIndexName);
		gridIndexColumn = (TextFieldTTF*)getChildByName("gridlearning")->getChildByName(leftIndexName);
	}
	else {
		popUpSize = 1.1, OriginalSize = 0.6899;
		gridIndexRow = (TextFieldTTF*)getChildByName("quiz")->getChildByName("grid")->getChildByName(rightIndexName);
		gridIndexColumn = (TextFieldTTF*)getChildByName("quiz")->getChildByName("grid")->getChildByName(leftIndexName);
	}

	auto action1 = ScaleTo::create(0.1, popUpSize);
	auto action2 = ScaleTo::create(0.1, OriginalSize);

	auto scaleAction1 = Sequence::create(action1, action2, NULL);
	gridIndexRow->runAction(scaleAction1);

	auto action3 = ScaleTo::create(0.1, popUpSize);
	auto action4 = ScaleTo::create(0.1, OriginalSize);

	auto scaleAction2 = Sequence::create(action3, action4, NULL);
	gridIndexColumn->runAction(scaleAction2);
}

string BasicMultiplication::getGridNameInString(int row, int column) {
	std::ostringstream gridName;	
	gridName << row << "r" << column << "c";
	return gridName.str();
}

Sprite* BasicMultiplication::getGridWithIndex(string gridSource,int row, int column) {

	auto gridName = getGridNameInString(row, column);
	Sprite* grid;
	if (gridSource == "gridlearning") {
		grid = (Sprite*)getChildByName(gridSource)->getChildByName(gridName);
	}
	else {
		grid = (Sprite*)getChildByName("quiz")->getChildByName(gridSource)->getChildByName(gridName);
	}
	if(grid)
		grid->setTag(1);
	return grid;
}

Sprite* BasicMultiplication::createSprite(string name,int width, int height,int posiX,int posiY,int scaleXY) {

	auto sprite = Sprite::create();
	sprite->setTextureRect(Rect(0, 0,width,height));
	sprite->setPosition(Vec2(posiX,posiY));
	sprite->setName(name);
	addChild(sprite);
	return sprite;
}

void BasicMultiplication::QuizPlay() {
	_animationName = _animationPath.at(RandomHelper::random_int(1, 6));

	Node* quiz = CSLoader::createNode("basicmultiplication/quiz.csb");
	addChild(quiz);
	quiz->setName("quiz");
	quizTopBoardSetting();

	if (Director::getInstance()->getVisibleSize().width > 2560) {
		auto myGameWidth = (Director::getInstance()->getVisibleSize().width - 2560) / 2;
		quiz->setPositionX(myGameWidth);
	}

	auto option1 = (Sprite*)quiz->getChildByName("1");
	auto option2 = (Sprite*)quiz->getChildByName("2");
	auto option3 = (Sprite*)quiz->getChildByName("3");
	auto option4 = (Sprite*)quiz->getChildByName("4");

	std::random_shuffle(_optionValue.begin(), _optionValue.end());
	TextOnBox(option1, _optionValue[0] * _menuContext->getCurrentLevel());
	TextOnBox(option2, _optionValue[1] * _menuContext->getCurrentLevel());
	TextOnBox(option3, _optionValue[2] * _menuContext->getCurrentLevel());
	TextOnBox(option4, _optionValue[3] * _menuContext->getCurrentLevel());

	float popUpSize = 0.3, OriginalSize = 1;
	auto grid = getChildByName("quiz")->getChildByName("grid");
	grid->setScale(popUpSize);
	auto scaleTo = ScaleTo::create(1, OriginalSize);
	EaseElasticOut *easeAction = EaseElasticOut::create(scaleTo);
	grid->runAction(easeAction);

	popUp((Sprite*)quiz->getChildByName("ans"));
	popUp((Sprite*)quiz->getChildByName("board"));
	popUp(option1);
	popUp(option2);
	popUp(option3);
	popUp(option4);


	addEventsOnQuizButton(option1);
	addEventsOnQuizButton(option2);
	addEventsOnQuizButton(option3);
	addEventsOnQuizButton(option4);
}

void BasicMultiplication::quizTopBoardSetting() {
	auto board = (Sprite*)getChildByName("quiz")->getChildByName("board");
	auto size = board->getContentSize();
	
	std::ostringstream boardValue;
	boardValue << _menuContext->getCurrentLevel() << " X " << _questionValue[_questionCounter] << " = ?";
	
	auto label = CommonLabelTTF::create(boardValue.str(), "Helvetica", 150);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(size.width * 0.5, size.height * 0.5));
	board->addChild(label);
	label->setName("label");

	_answer = _menuContext->getCurrentLevel() * _questionValue[_questionCounter];
	_row = _menuContext->getCurrentLevel();
	_column = _questionValue[_questionCounter];

	auto answer = getChildByName("quiz")->getChildByName("ans");

	auto labelNumber = CommonLabelTTF::create("0", "Helvetica", 150);
	labelNumber->setColor(Color3B::WHITE);
	labelNumber->setPosition(Vec2(answer->getContentSize().width / 2, answer->getContentSize().height / 2));
	answer->addChild(labelNumber);
	labelNumber->setName("answer");

	_optionValue.clear();
	optionRandomization();
	gridGrayAndListnerController("grid",_row, _column);

}

void BasicMultiplication::TextOnBox(Sprite* sprite , int number) {
	auto size = sprite->getContentSize();

	std::ostringstream boardValue;
	boardValue << number;

	auto label = CommonLabelTTF::create(boardValue.str(), "Helvetica", 150);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(size.width * 0.5, size.height * 0.5));
	sprite->addChild(label);
	label->setName("label");
	label->setTag(number);
}

void BasicMultiplication::popUp(Sprite* target) {

	target->setScale(0.3);
	auto scaleTo = ScaleTo::create(1, 1);
	EaseElasticOut *easeAction = EaseElasticOut::create(scaleTo);
	target->runAction(easeAction);

}

void BasicMultiplication::optionRandomization() {

	auto questionValue = _questionValue[_questionCounter];
	_quizAnswer = questionValue * _menuContext->getCurrentLevel();

	if (questionValue > 2 && questionValue < 9) {
		_optionValue.push_back(questionValue+1);
		_optionValue.push_back(questionValue-1);
		_optionValue.push_back(questionValue-2);
	}
	else if (questionValue < 3 && questionValue > 0) {
		_optionValue = getRandomOptionValue(questionValue, 3 , 9 , 3);
	}
	else if (questionValue > 8 && questionValue < 11) {
		_optionValue = getRandomOptionValue(questionValue, 1 , 8 , 3);
	}
	else {
		_optionValue = getRandomOptionValue(questionValue, 1, 10, 3);
	}
	_optionValue.push_back(questionValue);

	std::random_shuffle(_optionValue.begin(), _optionValue.end());

}

void BasicMultiplication::addEventsOnQuizButton(cocos2d::Sprite* object)
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(false);

	listener->onTouchBegan = [=](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		auto target = event->getCurrentTarget();
		Point locationInNode = touch->getLocation();

		if (target->getBoundingBox().containsPoint(locationInNode) && _optionTouch) {

			auto action1 = ScaleTo::create(0.1, 0.9);
			auto action2 = ScaleTo::create(0.1, 1);

			auto scaleAction = Sequence::create(action1, action2, NULL);
			target->runAction(scaleAction);

			if (target->getChildByName("label")->getTag() != _quizAnswer) {

				auto audioBg = CocosDenshion::SimpleAudioEngine::getInstance();
				audioBg->playEffect("res/sounds/sfx/error.ogg", false);

				_menuContext->pickNumber(_quizAnswer, target->getChildByName("label")->getTag(), _menuContext->MULTIPLY);

				FShake* shake = FShake::actionWithDuration(0.5f, 10.0f);
				target->runAction(shake);
				_totalHit++;
				_wrongHit++;
			}
			else {
				_questionCounter++;
				if (_questionValue.size() > _questionCounter) {
					_totalHit++;

					auto audioBg = CocosDenshion::SimpleAudioEngine::getInstance();
					audioBg->playEffect("res/sounds/sfx/success.ogg", false);
					
					_menuContext->pickNumber(_quizAnswer, target->getChildByName("label")->getTag(),_menuContext->MULTIPLY);

					auto sequence = Sequence::create(
						CallFunc::create([=]() {
							_optionTouch = false;
						}),
						DelayTime::create(0.5),

						CallFunc::create([=]() {
							removeChildByName("quiz");
							_optionTouchGrid = true;
							_gridName = "quiz";
							_counter = 0;
							QuizPlay();
							_optionTouch = true;
						}),NULL);
					runAction(sequence);
				}
				else {
					_menuContext->setMaxPoints(_totalHit);
					_menuContext->addPoints(_totalHit - _wrongHit);

					auto sequence = Sequence::create(
						CallFunc::create([=]() {
							_optionTouch = false;
						}),
						DelayTime::create(0.5), 
						
						CallFunc::create([=]() {
							_menuContext->showScore();
							_optionTouch = true;
						}), NULL);
					runAction(sequence);
				}
			}

			return false;
		}
		return false;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, object);
}

std::vector<int> BasicMultiplication::getRandomOptionValue(int exceptValue,int min , int max , int range) {
	
	std::vector<int> optionValue;

	while (true) {
		optionValue = getRandomValueRange(min, max, range);
		int flag = true;

		for (size_t i = 0; i < optionValue.size(); i++) {
			if (optionValue[i] == exceptValue) {
				flag = false;
			}
		}
		if (flag)
			break;
	}

	return optionValue;
}

vector<int> BasicMultiplication::getRandomValueRange(int min, int max, int getValue) {
	int count = 0;
	vector<int> objectVector;
	while (count < getValue) {
		int temp = RandomHelper::random_int(min, max);
		bool flag = true;

		for (size_t index = 0; index < objectVector.size(); index++) {
			if (objectVector[index] == temp) {
				flag = false;
				break;
			}
		}

		if (flag) {
			objectVector.push_back(temp);
			count++;
		}
	}

	//sort(objectVector.begin(), objectVector.end());
	return objectVector;
}

BasicMultiplication::~BasicMultiplication(void)
{
	this->removeAllChildrenWithCleanup(true);
}
