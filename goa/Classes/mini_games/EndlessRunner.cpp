//
//  EndlessRunner.cpp 
//  goa
//
//  Created by Karim Mirazul  on 13/09/16
//
//

#include "EndlessRunner.h"
#include <vector>
#include "SpriteCreate.h"
#include "../menu/StartMenuScene.h"
#include "../menu/HelpLayer.h"
#include "../util/MatrixUtil.h"

#define COCOS2D_DEBUG 1
using namespace std;
using namespace cocos2d;

Scene* EndlessRunner::createScene()
{
	Scene* scene = Scene::create();
	auto layer = EndlessRunner::create();
	scene->addChild(layer);
    layer->_menuContext = MenuContext::create(layer, EndlessRunner::gameName());
	scene->addChild(layer->_menuContext);
	return scene;
}
EndlessRunner::EndlessRunner(){
}
//  jump_sound
EndlessRunner::~EndlessRunner(void)
{
	_eventDispatcher->removeCustomEventListeners("multipleChoiceQuiz");
	this->removeAllChildrenWithCleanup(true);
}

void EndlessRunner::onEnterTransitionDidFinish()
{

	CCSpriteFrameCache* framecache1 = CCSpriteFrameCache::sharedSpriteFrameCache();
	framecache1->addSpriteFramesWithFile("endlessrunner/endlessrunner_01.plist");
	CCSpriteFrameCache* framecache2 = CCSpriteFrameCache::sharedSpriteFrameCache();
	framecache2->addSpriteFramesWithFile("endlessrunner/endlessrunner_02.plist");

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	_eventDispatcher->addCustomEventListener("multipleChoiceQuiz", CC_CALLBACK_1(EndlessRunner::GameBegin, this));

	// lesson API use for set choices and questions ... begin
	_vmc = _lesson.getMultiChoices(3, 5);

	
}

void EndlessRunner::GameBegin(cocos2d::EventCustom *eventCustom) {

	CCLOG("onLessonReady begin");
	std::string* buf = static_cast<std::string*>(eventCustom->getUserData());
	CCLOG("onLessonReady to unmarshallMultiChoices");
	vector<Lesson::MultiChoice> vmc = Lesson::unmarshallMultiChoices(buf);

	_vmc = vmc;

	for (int i = 0; i < vmc.size(); i++) {
		CCLOG("vmc[%d].question : %s and correctAnswer : %s ",i,vmc[i].question.c_str(),vmc[i].answers[vmc[i].correctAnswer].c_str());
		for (int j = 0; j < vmc[i].answers.size(); j++) {
			CCLOG("		answer[%d] : %s",j,vmc[i].answers[j].c_str());
		}
	}


	_correctAnswerFromVmc = _vmc[letterBoardAlphaLength].answers[_vmc[letterBoardAlphaLength].correctAnswer];
	tempChar = _vmc[letterBoardAlphaLength].question;

	auto multipleChoices = MatrixUtil::generateMatrixForChoosing(_correctAnswerFromVmc,
		_vmc[letterBoardAlphaLength].answers, 1, 15, 65);
	_letterStream = multipleChoices[0];
	std::random_shuffle(_letterStream.begin(), _letterStream.end());

	CCLOG("Generate Matrix For Choosing ... line 82");

	for (int i = 0; i < _letterStream.size(); i++) {
		CCLOG("choices on egg %d : %s", i, _letterStream[i].c_str());
	}

	//end
	auto bgLayerGradient = LayerGradient::create(Color4B(255, 255, 255, 255), Color4B(255, 255, 255, 255));
	this->addChild(bgLayerGradient, 0);
	EndlessRunner::addEvents(bgLayerGradient);
	leftBarrier = EndlessRunner::CreateSprites("endlessrunner/barrier.png", (visibleSize.width * -15 / 100) + origin.x, (visibleSize.height * 0) + origin.y, 1, 1, 0, "IMG");

	leftBarrierForBigObject = EndlessRunner::CreateSprites("endlessrunner/barrier.png", (visibleSize.width * -70 / 100) + origin.x, (visibleSize.height * 0) + origin.y, 1, 1, 0, "IMG");
	rightBarrier = EndlessRunner::CreateSprites("endlessrunner/barrier.png", (visibleSize.width * 120 / 100) + origin.x, (visibleSize.height * 0) + origin.y, 1, 1, 0, "IMG");
	upBarrier = EndlessRunner::CreateSprites("endlessrunner/bgTouchImage.png", origin.x, origin.y + (visibleSize.height * 110 / 100), visibleSize.width, 1, 0, "IMG");
	upBarrier->setAnchorPoint(Vec2(0, 1));

	Character.action = CSLoader::createTimeline("endlessrunner/main_char.csb");
	Character.character = (Sprite *)CSLoader::createNode("endlessrunner/main_char.csb");
	Character.character->setPosition(Vec2((visibleSize.width * 25 / 100) + origin.x, (int)(visibleSize.height * 11 / 100) + origin.y));
	this->addChild(Character.character, 11);
	Character.character->runAction(Character.action);
	Character.character->setScale(1.2);
	Character.action->play("run", true);
	Character.character->getChildByName("net")->setVisible(false);
	happyManAction = CSLoader::createTimeline("endlessrunner/happy_mad.csb");
	hpUiCatchAction = CSLoader::createTimeline("endlessrunner/catch_score.csb");

	auto rotate1 = CSLoader::createTimeline("endlessrunner/life.csb");
	auto rotate2 = CSLoader::createTimeline("endlessrunner/life.csb");
	auto rotate3 = CSLoader::createTimeline("endlessrunner/life.csb");
	auto rotate4 = CSLoader::createTimeline("endlessrunner/life.csb");
	auto rotate5 = CSLoader::createTimeline("endlessrunner/life.csb");

	hpUi = (Sprite *)CSLoader::createNode("endlessrunner/hp_ui.csb");
	hpUi->setPosition(Vec2((visibleSize.width * 0.005) + origin.x, (visibleSize.height + origin.y) - (visibleSize.height * 0.38)));
	hpUi->setScale(0.7);
	this->addChild(hpUi, 7);
	hpUi->runAction(hpUiCatchAction);
	hpUi->getChildByName("happy_mad")->setScale(1.2);
	hpUi->getChildByName("life_1")->runAction(rotate1);	rotate1->play("rotate", true);
	hpUi->getChildByName("life_2")->runAction(rotate2); rotate2->play("rotate", true);
	hpUi->getChildByName("life_3")->runAction(rotate3); rotate3->play("rotate", true);
	hpUi->getChildByName("life_4")->runAction(rotate4); rotate4->play("rotate", true);
	hpUi->getChildByName("life_5")->runAction(rotate5); rotate5->play("rotate", true);

	hpUi->runAction(happyManAction);
	happyManAction->play("happy_idle", true);

	auto boardDisplay = (Sprite *)CSLoader::createNode("endlessrunner/letter_board.csb");
	boardDisplay->setPosition(Vec2((visibleSize.width / 2) + origin.x, (visibleSize.height + origin.y) - (visibleSize.height * 0.07)));
	this->addChild(boardDisplay, 10);

	letterOnBoard = CommonLabelTTF::create(tempChar,"Helvetica" , 150);
	float fontSize = std::max(float(50.0), float(150 - (tempChar.length() - 1) * 15));
	if (fontSize < 50 || fontSize > 150) {
		fontSize = 50.0f;
	}
	letterOnBoard->setFontSize(fontSize);
	letterOnBoard->setName("mainBoard");
	letterOnBoard->setPosition(Vec2((visibleSize.width / 2) + origin.x, (visibleSize.height + origin.y) - (visibleSize.height * 0.07)));
	//letterOnBoard->enableShadow(Color4B::BLACK, Size(8, -6), 5);
	this->addChild(letterOnBoard, 10);

	EndlessRunner::beforeInitBackgroundScene();
	EndlessRunner::sceneBackgroundFlow();

	SpriteCreate* mountain;
	int MountainRandomvalue = 0;
	int startPosition = origin.x;

	for (int i = 0; i <= 15; i++) {
		MountainRandomvalue = EndlessRunner::randmValueIncludeBoundery(0, (sizeof(mountainMidImages) / sizeof(mountainMidImages[0])) - 1);
		mountain = SpriteCreate::createSprite(mountainMidImages[MountainRandomvalue], startPosition, origin.y, 0, 0, "midLand", "midLand", "firstLayer");
		this->addChild(mountain, 9);
		mountain->setName("MidLand");

		if (i == 15) { mountain->setName("LastInit"); mountain->NextRockName = "endLand"; }
		startPosition = startPosition + mountain->getContentSize().width - LayerMode.tolerence;
		allPathBlocks.push_back(mountain);
		mountain->runAction(MoveTo::create(EndlessRunner::movingTime(mountain), Vec2((leftBarrier->getPosition().x), origin.y)));
	}
	EndlessRunner::CreateMonsterWithLetter(1.0 / 60.0);
	EndlessRunner::startGame();

}


void EndlessRunner::scheduleMethod() {
	Character.onAir = true;
	
	this->schedule(schedule_selector(EndlessRunner::sceneTree1Flow), 2.5f);
	this->schedule(schedule_selector(EndlessRunner::sceneTree2Flow), 1.4f);
	this->schedule(schedule_selector(EndlessRunner::CreateMonsterWithLetter), _speedForLetterComing);
	this->schedule(schedule_selector(EndlessRunner::addFirstBlockSecondLayer), RandomHelper::random_int(16, 24));

	this->scheduleUpdate();
}

std::string EndlessRunner::getStringDataLevelInfo(const wchar_t* alpha, int currentLevel,int deductionValue,int groupLetter) {
	std::ostringstream blockName;
	int startPoint = ((currentLevel - deductionValue) - 1) * groupLetter; // three letter sequence like : A,B,C or P,Q,R (caps letter)
	auto charLength = LangUtil::getInstance()->getNumberOfCharacters() - 1;

	for (int i = 0; i < groupLetter; i++) {
		if ((startPoint + i) > charLength) {
			int index = RandomHelper::random_int(0, charLength - groupLetter);
			blockName << (char)alpha[index];
			if ((char)alpha[index] == ' ') {
				blockName << (char)alpha[RandomHelper::random_int(0, charLength - groupLetter)];
			}
		}
		else {
			blockName << (char)alpha[startPoint + i];
			if ((char)alpha[startPoint + i] == ' ') {
				blockName<< (char)alpha[RandomHelper::random_int(0, charLength - groupLetter)];
			}
		}
	}
	return blockName.str();
}

void EndlessRunner::startGame() {
	_menuContext->showStartupHelp(CC_CALLBACK_0(EndlessRunner::scheduleMethod, this));
//	runAction(Sequence::create(CallFunc::create(CC_CALLBACK_0(MenuContext::showStartupHelp, _menuContext)), CallFunc::create(CC_CALLBACK_0(EndlessRunner::scheduleMethod, this)), NULL));
}

void EndlessRunner::update(float delta) {

	if (_flagHelp && (allLabels.size() > 0) && (_menuContext->getCurrentLevel() == 1)) {
		if (((allLabels[0]->getPositionX() - 700) <= Character.character->getPositionX())) {
			_flagHelp = false;
			auto help = HelpLayer::create(Rect((visibleSize.width * 25 / 100) + origin.x + 150, (int)(visibleSize.height * 11 / 100) + origin.y +250, 300,600),Rect(allLabels[0]->getPositionX()+10, allLabels[0]->getPositionY(), 400,250));
			help->click(Vec2((visibleSize.width * 25 / 100) + origin.x + 150, (int)(visibleSize.height * 11 / 100) + origin.y + 250));
			help->setName("helpLayer");
			this->addChild(help, 15);
			_resumeHelp = false;
			allMonster[0]->pause();
			allLabels[0]->pause();
		}
	}

	if (!_resumeHelp && (_menuContext->getCurrentLevel() == 1) && this->getChildByName("helpLayer")) {
		allMonster[0]->pause();
		allLabels[0]->pause();
	}



	if (initBool) {
		for (std::size_t i = 0; i < allPathBlocks.size(); i++) {
			if (rightBarrier->getBoundingBox().intersectsRect(allPathBlocks[i]->getBoundingBox())) {
				if (allPathBlocks[i]->getName() == "LastInit") {
					currentFirstLayerRock = allPathBlocks[i];
					FirstLayerModes = 1;
					initBool = false;
				}
			}
		}
	}

	if (Character.onAir) {
		EndlessRunner::FallDownCharacter();
	}

	if (Character.stillCheckFalg) {
		EndlessRunner::stillCharacterOnPath(delta);
	}
	EndlessRunner::mountainLayer1();

	EndlessRunner::startingIntersectMode();

	EndlessRunner::removePathBlockTouchByLeftBarrier();
	
	if (counterLife == 1 && false) {
		//auto totalMax = _menuContext->getMaxPoints();
		float points = _menuContext->getPoints();
		_menuContext->addPoints(-points/3.0f);
		_menuContext->showScore();
	}
}

void EndlessRunner::FallDownCharacter() {
	Character.onAir = false;
	Character.stillCheckFalg = true;
	Character.fallingTime = Character.character->getPositionY() / 700;
	Character.fallDownAction = MoveTo::create(Character.fallingTime, Vec2(Character.character->getPositionX(), origin.y));
	Character.character->runAction(Character.fallDownAction);
	Character.groundTouchFlag = true;
}

void EndlessRunner::stillCharacterOnPath(float delta) {
	
	for (std::size_t i = 0; i < allPathBlocks.size(); i++) {
		
		auto box = Character.character->getChildByName("floor_2")->getBoundingBox();
		Rect parent = Character.character->getBoundingBox();
		Rect boxs = Rect(parent.origin.x + box.origin.x, parent.origin.y + box.origin.y, box.size.width*1.2, box.size.height*1.2);

		if (boxs.intersectsRect(allPathBlocks[i]->getBoundingBox())) {
			
			if (allPathBlocks[i]->LayerTypeName == "firstLayer") {
				Character.character->setPosition(Vec2((visibleSize.width * 25 / 100) + origin.x, (int)(visibleSize.height * 11 / 100) + origin.y + 15));
				if (Character.groundTouchFlag) {
					Character.groundTouchFlag = false;
					Character.Clicked = false;
					Character.character->stopAction(Character.fallDownAction);

					auto A = CallFunc::create([=]() {Character.action->play("jump_end", false); });

					auto B = CallFunc::create([=]() {Character.action->play("run", true);});

					auto main_Sequence = Sequence::create(A, B, NULL);
					Character.character->runAction(main_Sequence);
				}
				
			}
			else if (allPathBlocks[i]->LayerTypeName == "secondLayer") {
				Character.character->setPosition(Vec2((visibleSize.width * 25 / 100) + origin.x, (visibleSize.height * 23 / 100) + origin.y));

				if (Character.groundTouchFlag) {
					Character.groundTouchFlag = false;
					Character.Clicked = false;
					Character.character->stopAction(Character.fallDownAction);

					auto A = CallFunc::create([=]() {
						Character.action->play("jump_end", false);
					});

					auto B = CallFunc::create([=]() {
						Character.action->play("run", true);
					});

					auto main_Sequence = Sequence::create(A, B, NULL);
					Character.character->runAction(main_Sequence);
				}
			}
		}
		else {
			Character.character->setPositionY(Character.character->getPositionY() - (0.8));
		}
	}
}

void EndlessRunner::startingIntersectMode() {
	auto classRefer = this;
	if (FirstLayerModes == LayerMode.FirstLayerRightIntersectMode) {
		if (!rightBarrier->getBoundingBox().intersectsRect(currentFirstLayerRock->getBoundingBox())) {
			int MountainRandomvalue = EndlessRunner::randmValueIncludeBoundery(0, (sizeof(mountainMidImages) / sizeof(mountainMidImages[0])) - 1);
			SpriteCreate* currentImage = SpriteCreate::createSprite(mountainMidImages[MountainRandomvalue], EndlessRunner::setPositionX(currentFirstLayerRock), 0 + origin.y, 0, 0, "midLand", "midLand", "firstLayer");
			this->addChild(currentImage, 9);
			allPathBlocks.push_back(currentImage);
			currentFirstLayerRock = currentImage;
			position = EndlessRunner::movingUpto(0);
			currentFirstLayerRock->runAction(MoveTo::create(EndlessRunner::movingTime(currentFirstLayerRock), Vec2(position.first, position.second)));
			FirstLayerModes = 1;
		}
	}
	
	if (SecondLayerModes == LayerMode.SecondLayerRightIntersectMode) {
		if (!rightBarrier->getBoundingBox().intersectsRect(currentSecondLayerRock->getBoundingBox())) {
			SecondLayerModes = 4;

			EndlessRunner::AddRocksInSecondLayerPath();
		}
	}

	for (std::size_t i = 0; i < allLabels.size(); i++) {

		auto box = Character.character->getChildByName("net")->getBoundingBox();
		Rect parent = Character.character->getBoundingBox();
		Rect netBoxs = Rect(parent.origin.x + (box.origin.x), parent.origin.y + (box.origin.y), box.size.width*1.2, box.size.height*1.2);

		Rect letteBox = allLabels[i]->getBoundingBox();
		if (LangUtil::getInstance()->getLang() == "kan"){
			Rect newLetterBox = Rect(letteBox.origin.x-30, letteBox.origin.y+(letteBox.size.height/2), letteBox.size.width,30);
			letteBox = newLetterBox;
		}

		auto letterValueString = allLabels[i]->getString();

		if (netBoxs.intersectsRect(letteBox))
		{
			if (allLabels[i]->getName() == _correctAnswerFromVmc) {
				
				if (popUp) {
					auto highScale = CallFunc::create([=]() { happyManAction->play("happy_pop", false); });
					auto smallScale = CallFunc::create([=]() {happyManAction->play("happy_idle", true); });
					auto scaleVary = Sequence::create(highScale, DelayTime::create(1.3), smallScale, NULL);
					hpUi->getChildByName("happy_mad")->runAction(scaleVary);
				}
				else {
					popUp = false;
				}

				// If correct alphabet picked ......
				_menuContext->pickWord(_correctAnswerFromVmc,allLabels[i]->getString(), true);
				_menuContext->addPoints(1);

				auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
				//auto path = LangUtil::getInstance()->getAlphabetSoundFileNameForString(allLabels[i]->getString());
				//audio->playEffect(path.c_str(), false);

				counterAlphabets = counterAlphabets + 2;
				_totalCounterAlphabets++;
				_speedForLetterComing = getSpeedForMonsterRunning();
				auto nextAlpha = CallFunc::create([=]() {
					if (_vmc.size() - 1 == letterBoardAlphaLength) {
					//	classRefer->unschedule(classRefer->CreateMonsterWithLetter);
						_menuContext->showScore();
						
					}
					else {
						letterBoardAlphaLength++;
						hpUiCatchAction->play("1", false);
						tempChar = _vmc[letterBoardAlphaLength].question;
						letterOnBoard->setString(tempChar);
						letterOnBoard->setString(tempChar);
						counterAlphabets = 0;
						//letters = CharGenerator::getInstance()->generateMatrixForChoosingAChar(tempChar, 1, 21, 70, _caseSensitivity);

						_letterStream.clear();
						_correctAnswerFromVmc = _vmc[letterBoardAlphaLength].answers[_vmc[letterBoardAlphaLength].correctAnswer];
						auto multipleChoices = MatrixUtil::generateMatrixForChoosing(_correctAnswerFromVmc,
							_vmc[letterBoardAlphaLength].answers, 1, 15, 65);

						_letterStream = multipleChoices[0];
						std::random_shuffle(_letterStream.begin(), _letterStream.end());
						
						CCLOG("Generate Matrix For Choosing ... line 404");

						for (int i = 0; i < _letterStream.size(); i++) {
							CCLOG("choices on egg %d : %s", i, _letterStream[i].c_str());
						}

						counterLetter = 0;

					}
				});

				auto moveToNextAlphabets = Sequence::create(DelayTime::create(1.5), nextAlpha, NULL);

				std::ostringstream counterForLetter;	counterForLetter << counterAlphabets; std::string counterValue = counterForLetter.str();
				hpUiCatchAction->play(counterValue,false);
				hpUi->getChildByName("happy_mad")->setScale(1.2);

				if (counterAlphabets >= 10) {
					this->runAction(moveToNextAlphabets);
				}

				if (!popUp) {
					auto highScale = CallFunc::create([=]() { happyManAction->play("change_mad_happy", false);});
					auto smallScale = CallFunc::create([=]() {happyManAction->play("happy_idle", true); });
					auto scaleVary = Sequence::create(highScale,DelayTime::create(1.3),smallScale,NULL);
					hpUi->getChildByName("happy_mad")->runAction(scaleVary);
					popUp = true;
				}
				for (std::size_t k = 0; k <allMonster.size(); k++) {
					if (allMonster[k]->getTag() == allLabels[i]->getTag()) {
						this->removeChild(allLabels[i]);
						allLabels.erase(allLabels.begin() + i);
						Character.action->play("correct_catch", false);
						allMonster[k]->getChildByName("monster_egg")->setVisible(false);
						hpUi->getChildByName("happy_mad")->getChildByName("happy")->setVisible(true);	
						hpUi->getChildByName("happy_mad")->getChildByName("mad")->setVisible(false);
						allMonster[k]->runAction(MoveBy::create(3, Vec2((visibleSize.width * 90 / 100) + origin.x, (visibleSize.height * 70 / 100) + origin.y)));
						break;
					}
				}
			}
			else {
				// If wrong alphabet picked ...
				_menuContext->pickWord(_correctAnswerFromVmc, allLabels[i]->getString(), true);
				_menuContext->addPoints(-1);

				hpUi->getChildByName("happy_mad")->setScale(1);
				
				if (!popUp) {
					auto highScale = CallFunc::create([=]() { happyManAction->play("mad_pop", false); });
					auto smallScale = CallFunc::create([=]() {happyManAction->play("mad_idle", true); });
					auto scaleVary = Sequence::create(highScale, DelayTime::create(1.3), smallScale, NULL);
					hpUi->getChildByName("happy_mad")->runAction(scaleVary);
				}
				else {
					popUp = true;
				}

				auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
				//auto path = LangUtil::getInstance()->getAlphabetSoundFileNameForString(allLabels[i]->getString());
				//audio->playEffect(path.c_str(), false);
				if (popUp) {
					auto highScale = CallFunc::create([=]() { happyManAction->play("change_happy_mad", false); });
					auto smallScale = CallFunc::create([=]() {happyManAction->play("mad_idle", true); });
					auto scaleVary = Sequence::create(highScale, DelayTime::create(1.3), smallScale, NULL);
					hpUi->getChildByName("happy_mad")->runAction(scaleVary);
					popUp = false;
				}
				for (std::size_t k = 0; k <allMonster.size(); k++) {
					if (allMonster[k]->getTag() == allLabels[i]->getTag()) {

						Character.action->play("worng_catch", false);
						this->removeChild(allLabels[i]);
						allLabels.erase(allLabels.begin() + i);
						hpUi->getChildByName("happy_mad")->getChildByName("mad")->setVisible(true);
						hpUi->getChildByName("happy_mad")->getChildByName("happy")->setVisible(false);
						allMonster[k]->getChildByName("monster_egg")->setVisible(false);
						allMonster[k]->getChildByName("monster_egg_crack")->setVisible(true);
						allMonster[k]->runAction(MoveBy::create(3, Vec2((visibleSize.width * 90 / 100) + origin.x, (visibleSize.height * 70 / 100) + origin.y)));
						break;
					}
				}
			}
		}
	}
	if (flagLifeDemo) {
		for (std::size_t i = 0; i < allBeforeStartBlocks.size(); i++) {
			
			auto box = Character.character->getChildByName("floor_3")->getBoundingBox();
			Rect parent = Character.character->getBoundingBox();
			Rect boxs = Rect(parent.origin.x + (box.origin.x), parent.origin.y + (box.origin.y), box.size.width, box.size.height);

			if (boxs.intersectsRect(allBeforeStartBlocks[i]->getBoundingBox())) {
				flagLifeDemo = false;

				if (counterLife > 1) {
					counterLife = counterLife - 1;
					std::ostringstream sstreamc; sstreamc << "life_" << counterLife; std::string counterLifeString = sstreamc.str();
					hpUi->getChildByName(counterLifeString)->stopAllActions();
					hpUi->getChildByName(counterLifeString)->getChildByName("life_on")->setVisible(false);
					hpUi->getChildByName(counterLifeString)->getChildByName("life_off")->setVisible(true);
				}
				auto upVisible = CallFunc::create([=]() {
					Character.character->setVisible(false);
				});
				auto blink = Blink::create(1.2, 10);
				auto visible = CallFunc::create([=]() {
					Character.character->setVisible(true);
					flagLifeDemo = true;
				});
				auto blinking = Sequence::create(upVisible,DelayTime::create(0.3),blink, visible, NULL);
				Character.character->runAction(blinking);
				CCLOG("INTERSET MAN");
			}
		}
	}
}

void EndlessRunner::sceneBackgroundFlow() {
	//Sprite Background for start the infinite flow .....
	currentlayer1Sprite = EndlessRunner::CreateSprites("endlessrunner/layer_1_mountain.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 18 / 100) + origin.y), 1, 1, 1, "bgElement");
	currentlayer1Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer1Sprite, 100), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 18 / 100) + origin.y))));

	currentlayer2Sprite = EndlessRunner::CreateSprites("endlessrunner/layer_2_mountain.png", 0, (int)((visibleSize.height * 15 / 100) + origin.y), 1, 1, 2, "bgElement");
	currentlayer2Sprite->setPositionX(rightBarrier->getPosition().x - LayerMode.tolerence - currentlayer2Sprite->getContentSize().width / 3);
	currentlayer2Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer2Sprite, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 15 / 100) + origin.y))));

	currentlayer3Sprite = EndlessRunner::CreateSprites("endlessrunner/layer_3_bush.png", 0, (int)((visibleSize.height * 13 / 100) + origin.y), 1, 1, 3, "bgElement");
	currentlayer3Sprite->setPositionX(rightBarrier->getPosition().x - LayerMode.tolerence - currentlayer3Sprite->getContentSize().width / 3);
	currentlayer3Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer3Sprite, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 13 / 100) + origin.y))));

	currentlayer5Sprite = EndlessRunner::CreateSprites("endlessrunner/layer_5_bush.png", 0, (int)((visibleSize.height * 7 / 100) + origin.y), 1, 1, 5, "bgElement");
	currentlayer5Sprite->setPositionX(rightBarrier->getPosition().x - LayerMode.tolerence - currentlayer5Sprite->getContentSize().width / 3);
	currentlayer5Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer5Sprite, 260), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 7 / 100) + origin.y))));

	currentlayer7Sprite = EndlessRunner::CreateSprites("endlessrunner/layer_7_bush.png", 0, (int)((visibleSize.height * 0 / 100) + origin.y), 1, 1, 7, "bgElement");
	currentlayer7Sprite->setPositionX(rightBarrier->getPosition().x - LayerMode.tolerence - currentlayer5Sprite->getContentSize().width / 3);
	currentlayer7Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer7Sprite, 400), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 0 / 100) + origin.y))));
}

void EndlessRunner::mountainLayer1() {
	//Endless Monutain Running Code 
	if (!rightBarrier->getBoundingBox().intersectsRect(currentlayer1Sprite->getBoundingBox())) {
		int xSizeIndexRange = EndlessRunner::randmValueIncludeBoundery(0, 6);
		auto layer1Mountain = EndlessRunner::CreateSprites("endlessrunner/layer_1_mountain.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 18 / 100) + origin.y), xSizeArray[xSizeIndexRange], 1, 1, "bgElement");
		currentlayer1Sprite = layer1Mountain;
		currentlayer1Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer1Sprite, 100), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 18 / 100) + origin.y))));
	}

	if (!rightBarrier->getBoundingBox().intersectsRect(currentlayer2Sprite->getBoundingBox())) {
		int xSizeIndexRange = EndlessRunner::randmValueIncludeBoundery(0, 6);
		auto layer2Mountain = EndlessRunner::CreateSprites("endlessrunner/layer_2_mountain.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 15 / 100) + origin.y), xSizeArray[xSizeIndexRange], 1, 2, "bgElement");
		currentlayer2Sprite = layer2Mountain;
		currentlayer2Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer2Sprite, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 15 / 100) + origin.y))));
	}

	if (!rightBarrier->getBoundingBox().intersectsRect(currentlayer3Sprite->getBoundingBox())) {
		auto layer3bush = EndlessRunner::CreateSprites("endlessrunner/layer_3_bush.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 13 / 100) + origin.y), 1, 1, 3, "bgElement");
		currentlayer3Sprite = layer3bush;
		currentlayer3Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer3Sprite, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 13 / 100) + origin.y))));
	}

	if (!rightBarrier->getBoundingBox().intersectsRect(currentlayer5Sprite->getBoundingBox())) {
		auto layer5bush = EndlessRunner::CreateSprites("endlessrunner/layer_5_bush.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 7 / 100) + origin.y), 1, 1, 5, "bgElement");
		currentlayer5Sprite = layer5bush;
		currentlayer5Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer5Sprite, 260), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 7 / 100) + origin.y))));
	}

	if (!rightBarrier->getBoundingBox().intersectsRect(currentlayer7Sprite->getBoundingBox())) {
		auto layer7bush = EndlessRunner::CreateSprites("endlessrunner/layer_7_bush.png", rightBarrier->getPosition().x - LayerMode.tolerence, (int)((visibleSize.height * 0 / 100) + origin.y), 1, 1, 7, "bgElement");
		currentlayer7Sprite = layer7bush;
		currentlayer7Sprite->runAction(MoveTo::create(EndlessRunner::movingTimes(currentlayer7Sprite, 400), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 0 / 100) + origin.y))));
	}
}

void EndlessRunner::sceneTree1Flow(float dt) {
	int MountainRandomvalue = EndlessRunner::randmValueIncludeBoundery(0, (sizeof(treeLayer4) / sizeof(treeLayer4[0])) - 1);
	auto layer4tree = EndlessRunner::CreateSprites(treeLayer4[MountainRandomvalue], rightBarrier->getPosition().x, (int)((visibleSize.height * 10 / 100) + origin.y), 1, 1, 4, "bgElement");
	layer4tree->runAction(MoveTo::create(EndlessRunner::movingTimes(layer4tree, 260), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 10 / 100) + origin.y))));
}

void EndlessRunner::sceneTree2Flow(float dt) {
	int MountainRandomvalue = EndlessRunner::randmValueIncludeBoundery(0, (sizeof(treeLayer6) / sizeof(treeLayer6[0])) - 1);
	auto layer6tree = EndlessRunner::CreateSprites(treeLayer6[MountainRandomvalue], rightBarrier->getPosition().x, (int)((visibleSize.height * 4 / 100) + origin.y), 1, 1, 6, "bgElement");
	layer6tree->runAction(MoveTo::create(EndlessRunner::movingTimes(layer6tree, 400), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 4 / 100) + origin.y))));
}

void EndlessRunner::beforeInitBackgroundScene() {

	int newtolerence = 0;

	auto layer0ground = EndlessRunner::CreateSprites("endlessrunner/ground.png", origin.x, origin.y, visibleSize.width, 1, 0, "none");

	for (int i = 0; i < 3; i++) {
		newtolerence = newtolerence + (LayerMode.tolerence);
		auto layer1mountain = EndlessRunner::CreateSprites("endlessrunner/layer_1_mountain.png", origin.x, origin.y, 1, 1, 1, "bgElement");
		layer1mountain->setPosition(Vec2(origin.x + (layer1mountain->getContentSize().width * i) - newtolerence - layer1mountain->getContentSize().width / 2, (int)((visibleSize.height * 18 / 100) + origin.y)));
		layer1mountain->runAction(MoveTo::create(EndlessRunner::movingTimes(layer1mountain, 100), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 18 / 100) + origin.y))));
		if (i == 2) { layer1mountain->setScaleX(1.5); }
	}
	newtolerence = 0;
	for (int i = 0; i < 2; i++) {
		newtolerence = newtolerence + (LayerMode.tolerence);
		auto layer2mountain = EndlessRunner::CreateSprites("endlessrunner/layer_2_mountain.png", origin.x, origin.y, 1, 1, 2, "bgElement");
		layer2mountain->setPosition(Vec2(origin.x + (layer2mountain->getContentSize().width * i) - newtolerence, (int)((visibleSize.height * 15 / 100) + origin.y)));
		layer2mountain->runAction(MoveTo::create(EndlessRunner::movingTimes(layer2mountain, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 15 / 100) + origin.y))));
		if (i == 1) { layer2mountain->setScaleX(1.5); }
	}

	newtolerence = 0;
	for (int i = 0; i < 2; i++) {
		newtolerence = newtolerence + (LayerMode.tolerence * 2);
		auto layer3 = EndlessRunner::CreateSprites("endlessrunner/layer_3_bush.png", origin.x, origin.y, 1, 1, 3, "bgElement");
		layer3->setPosition(Vec2(origin.x + (layer3->getContentSize().width * i) - newtolerence, (int)((visibleSize.height * 13 / 100) + origin.y)));
		layer3->runAction(MoveTo::create(EndlessRunner::movingTimes(layer3, 230), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 13 / 100) + origin.y))));
		if (i == 1) { layer3->setScaleX(1.5); }
	}
	newtolerence = 0;
	for (int i = 0; i < 4; i++) {
		auto layer4 = EndlessRunner::CreateSprites(treeLayer4[2], origin.x, origin.y, 1, 1, 4, "bgElement");
		newtolerence = newtolerence + (LayerMode.tolerence);
		layer4->setPosition(Vec2(origin.x + (layer4->getContentSize().width * i) - newtolerence, (int)((visibleSize.height * 10 / 100) + origin.y)));
		layer4->runAction(MoveTo::create(EndlessRunner::movingTimes(layer4, 260), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 10 / 100) + origin.y))));
	}
	newtolerence = 0;
	for (int i = 0; i < 2; i++) {
		auto layer5 = EndlessRunner::CreateSprites("endlessrunner/layer_5_bush.png", origin.x, origin.y, 1, 1, 5, "bgElement");
		newtolerence = newtolerence + (LayerMode.tolerence * 2);
		layer5->setPosition(Vec2(origin.x + (layer5->getContentSize().width * i) - newtolerence, (int)((visibleSize.height * 7 / 100) + origin.y)));
		layer5->runAction(MoveTo::create(EndlessRunner::movingTimes(layer5, 260), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 7 / 100) + origin.y))));
		if (i == 1) { layer5->setScaleX(1.5); }
	}
	newtolerence = 0;
	for (int i = 0; i < 4; i++) {
		auto layer6 = EndlessRunner::CreateSprites(treeLayer6[0], origin.x, origin.y, 1, 1, 6, "bgElement");
		layer6->setPosition(Vec2(origin.x + (layer6->getContentSize().width * i), (int)((visibleSize.height * 4 / 100) + origin.y)));
		layer6->runAction(MoveTo::create(EndlessRunner::movingTimes(layer6, 400), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 4 / 100) + origin.y))));
	}
	newtolerence = 0;
	for (int i = 0; i < 2; i++) {
		auto layer7 = EndlessRunner::CreateSprites("endlessrunner/layer_7_bush.png", origin.x, origin.y, 1, 1, 7, "bgElement");
		newtolerence = newtolerence + (LayerMode.tolerence * 2);
		layer7->setPosition(Vec2(origin.x + (layer7->getContentSize().width * i) - newtolerence, (int)((visibleSize.height * 0 / 100) + origin.y)));
		layer7->runAction(MoveTo::create(EndlessRunner::movingTimes(layer7, 400), Vec2(leftBarrierForBigObject->getPosition().x, (int)((visibleSize.height * 0 / 100) + origin.y))));
		if (i == 1) { layer7->setScaleX(1.5); }
	}
}

void EndlessRunner::AddRocksInFirstLayerPath() {

	if (currentFirstLayerRock->NextRockName == "midLand") {
		int MountainRandomvalue = EndlessRunner::randmValueIncludeBoundery(0, (sizeof(mountainMidImages) / sizeof(mountainMidImages[0])) - 1);
		SpriteCreate* currentImage = SpriteCreate::createSprite(mountainMidImages[MountainRandomvalue], EndlessRunner::setPositionX(currentFirstLayerRock), 0 + origin.y, 0, 0, "midLand", "", "firstLayer");
		this->addChild(currentImage, 9);
		allPathBlocks.push_back(currentImage);
		currentFirstLayerRock = currentImage;
		position = EndlessRunner::movingUpto(0);
		currentFirstLayerRock->runAction(MoveTo::create(EndlessRunner::movingTime(currentFirstLayerRock), Vec2(position.first, position.second)));
		FirstLayerModes = 1;

	}
}

void EndlessRunner::addFirstBlockSecondLayer(float dt) {
	if (startSecondFlag && !_flagHelp && _resumeHelp) {
		SpriteCreate* newSprite = EndlessRunner::addUpperLayerStartSpriteRock(currentFirstLayerRock, "secondLayer", (int)(visibleSize.height * 11 / 100) + origin.y, 10, 0);
		currentSecondLayerRock = newSprite;
		currentSecondLayerRock->runAction(MoveTo::create(EndlessRunner::movingTime(currentSecondLayerRock), Vec2(leftBarrier->getPosition().x + origin.x, (int)(visibleSize.height * 11 / 100) + origin.y)));
		SecondLayerModes = 2;
		int midValue[9] = {1,3,5,1,6,4,2,9,4};
		SecondLayerCounterMidObjectValue = midValue[RandomHelper::random_int(0,8)];
		startSecondFlag = false;
	}
}

int EndlessRunner::getSpeedForMonsterRunning()
{
	bool highLevelSpeed = false;
	if(_menuContext->getCurrentLevel() >= 19) {
		highLevelSpeed = true;
	}

	int totalAlphabets = _vmc.size() * 5;
	if (_totalCounterAlphabets > floor(totalAlphabets * 0.75)) {
		if (highLevelSpeed)
			return 1;
		return 1;
	}else if (_totalCounterAlphabets > floor(totalAlphabets * 0.50)) {
		if (highLevelSpeed)
			return 1;
		return 2;
	}else if (_totalCounterAlphabets > floor(totalAlphabets * 0.25)) {
		if (highLevelSpeed)
			return 2;
		return 3;
	}else if (_totalCounterAlphabets > 0) {
		if (highLevelSpeed)
			return 3;
		return 4;
	}
	if (highLevelSpeed)
		return 3;
	return 4;
}
void EndlessRunner::AddRocksInSecondLayerPath() {
	
	if (currentSecondLayerRock->NextRockName == "endLand") {
		SpriteCreate* currentImage = SpriteCreate::createSprite("endlessrunner/path_right_lands.png", EndlessRunner::setPositionX(currentSecondLayerRock), (int)(visibleSize.height * 11 / 100) + origin.y, 0, 0, "endLand","", "secondLayer");
		this->addChild(currentImage, 10);
		allPathBlocks.push_back(currentImage);
		currentSecondLayerRock = currentImage;
		currentSecondLayerRock->runAction(MoveTo::create(EndlessRunner::movingTime(currentSecondLayerRock), Vec2(leftBarrier->getPosition().x + origin.x, (int)(visibleSize.height * 11 / 100) + origin.y)));
		//startSecondFlag = true;
	}

	if (currentSecondLayerRock->NextRockName == "midLand") {
		int MountainRandomvalue = randmValueIncludeBoundery(0, (sizeof(mountainMidImages) / sizeof(mountainMidImages[0])) - 1);

		SpriteCreate* currentImage = SpriteCreate::createSprite(mountainMidImages[MountainRandomvalue], EndlessRunner::setPositionX(currentSecondLayerRock), (int)(visibleSize.height * 11 / 100) + origin.y, 0, 0, "midLand", "", "secondLayer");
		this->addChild(currentImage, 10);
		allPathBlocks.push_back(currentImage);

		if (SecondLayerCounterMidObjectValue > SecondLayerCounterMid) {
			currentImage->NextRockName = "midLand";
		}
		else {
			currentImage->NextRockName = "endLand";
			SecondLayerCounterMid = 0;
		}
		SecondLayerModes = 2;
		SecondLayerCounterMid++;
		currentSecondLayerRock = currentImage;
		currentSecondLayerRock->runAction(MoveTo::create(EndlessRunner::movingTime(currentSecondLayerRock), Vec2(leftBarrier->getPosition().x + origin.x, (int)(visibleSize.height * 11 / 100) + origin.y)));
	}
}

Sprite* EndlessRunner::CreateSprites(const std::string name, int PositionX, int positionY, float scaleX, float scaleY, int zOrder, const std::string vectorType) {
	Sprite* sprite = Sprite::createWithSpriteFrameName(name);
	sprite->setPosition(Vec2(PositionX, positionY));
	sprite->setAnchorPoint(Vec2(0, 0));
	sprite->setScaleX(scaleX);
	sprite->setScaleY(scaleY);
	this->addChild(sprite, zOrder);

	if (vectorType == "bgElement") {
		allSceneObject.push_back(sprite);
	}
	if (vectorType == "blinkBlock") {
		allBeforeStartBlocks.push_back(sprite);
	}
	return sprite;
}

SpriteCreate* EndlessRunner::addUpperLayerStartSpriteRock(SpriteCreate* SpriteObject, std::string MountainType, int positionY, int zOrder, int xCordinateforBlankSpace) {
	SpriteCreate* currentImage = SpriteCreate::createSprite("endlessrunner/path_left_lands.png", EndlessRunner::setPositionX(SpriteObject), positionY, 0, 0, "startLand", "midLand", MountainType);
	this->addChild(currentImage, zOrder);
	allPathBlocks.push_back(currentImage);
	auto extra = EndlessRunner::CreateSprites("endlessrunner/bgTouchImage.png", EndlessRunner::setPositionX(SpriteObject), positionY, 10, 20, 11, "blinkBlock");
	extra->runAction(MoveTo::create(EndlessRunner::movingTime(currentImage), Vec2(leftBarrier->getPosition().x + origin.x, positionY)));
	extra->setVisible(false);
	return currentImage;
}

float EndlessRunner::movingTime(SpriteCreate* SpriteObject) {
	if (leftBarrier->getPosition().x < 0) { return ((SpriteObject->getPosition().x + std::abs(leftBarrier->getPosition().x)) / LayerMode.PathMovingSpeed); }
	return ((SpriteObject->getPosition().x - std::abs(leftBarrier->getPosition().x)) / LayerMode.PathMovingSpeed);
}

float EndlessRunner::movingTimes(Sprite* SpriteObject, int Speed) {
	if (leftBarrierForBigObject->getPosition().x < 0) { return ((SpriteObject->getPosition().x + std::abs(leftBarrierForBigObject->getPosition().x)) / Speed); }
	return ((SpriteObject->getPosition().x - std::abs(leftBarrierForBigObject->getPosition().x)) / Speed);
}

std::pair<float, float> EndlessRunner::movingUpto(float positionY) {
	return std::make_pair((leftBarrier->getPosition().x + origin.x), (visibleSize.height * positionY) + origin.y);
}

float EndlessRunner::setPositionX(SpriteCreate* SpriteObject) {
	return (SpriteObject->getPosition().x + SpriteObject->getContentSize().width - LayerMode.tolerence);
}

int EndlessRunner::randmValueIncludeBoundery(int min, int max) {
	int maxValue = max, minValue = min;
	if (min > max) { maxValue = min;  minValue = max; }
	else if (min == max) { return min; }
	return (rand() % (maxValue - minValue + 1) + minValue);
}

void EndlessRunner::removePathBlockTouchByLeftBarrier() {
	for (std::size_t i = 0; i < allPathBlocks.size(); i++) {
		if (allPathBlocks[i]->getBoundingBox().intersectsRect(leftBarrier->getBoundingBox()))
		{
			this->removeChild(allPathBlocks[i]);
			allPathBlocks.erase(allPathBlocks.begin() + i);
		}
	}
	for (std::size_t i = 0; i < allSceneObject.size(); i++) {
		if (allSceneObject[i]->getBoundingBox().intersectsRect(leftBarrierForBigObject->getBoundingBox()))
		{
			this->removeChild(allSceneObject[i]);
			allSceneObject.erase(allSceneObject.begin() + i);
		}
	}
	for (std::size_t i = 0; i < allLabels.size(); i++) {
		if (allLabels[i]->getBoundingBox().intersectsRect(leftBarrier->getBoundingBox()))
		{
			this->removeChild(allLabels[i]);
			allLabels.erase(allLabels.begin() + i);
		}
	}
	for (std::size_t i = 0; i <allMonster.size(); i++) {
		if (allMonster[i]->getBoundingBox().intersectsRect(upBarrier->getBoundingBox()) || allMonster[i]->getBoundingBox().intersectsRect(leftBarrier->getBoundingBox()))
		{
			this->removeChild(allMonster[i]);
			allMonster.erase(allMonster.begin() + i);
		}
	}

	for (std::size_t i = 0; i < allBeforeStartBlocks.size(); i++) {
		if (allBeforeStartBlocks[i]->getBoundingBox().intersectsRect(leftBarrier->getBoundingBox()))
		{
			this->removeChild(allBeforeStartBlocks[i]);
			allBeforeStartBlocks.erase(allBeforeStartBlocks.begin() + i);
		}
	}
}

void EndlessRunner::CreateMonsterWithLetter(float dt) {
	if (_resumeHelp) {
		cocostudio::timeline::ActionTimeline* timeline;
		Sprite* monsterImage;
		int switchMonster = EndlessRunner::randmValueIncludeBoundery(0, 1);
		if (switchMonster == 0) {
			timeline = CSLoader::createTimeline("endlessrunner/monster_yellow.csb");
			monsterImage = (Sprite *)CSLoader::createNode("endlessrunner/monster_yellow.csb");
		}
		else {
			timeline = CSLoader::createTimeline("endlessrunner/monster_red.csb");
			monsterImage = (Sprite *)CSLoader::createNode("endlessrunner/monster_red.csb");
		}
		if(initBool)
		currentFirstLayerRock = allPathBlocks[4];

		monsterImage->setScale(1.175);
		Rect box = monsterImage->getChildByName("monster_egg")->getBoundingBox();

		monsterImage->runAction(timeline);  timeline->gotoFrameAndPlay(0);

		auto str = _letterStream[counterLetter];
		counterLetter++;
		if (counterLetter == _letterStream.size()) {
			std::random_shuffle(_letterStream.begin(), _letterStream.end());
			//letters = CharGenerator::getInstance()->generateMatrixForChoosingAChar(tempChar, 1, 21, 70, _caseSensitivity);
			counterLetter = 0;
		}
		
		if (_flagLetter && (_menuContext->getCurrentLevel() == 1)) {
			str = _vmc[letterBoardAlphaLength].answers[_vmc[letterBoardAlphaLength].correctAnswer];
			_flagLetter = false;
		}

		auto label = CommonLabelTTF::create(str, "Helvetica",150);

		auto fontSize = std::max(float(30.0), float(150 - (str.length() - 1) * 13));
		if (fontSize <= 50.0f || fontSize > 150.0f) {
			fontSize = 60.0f;
		}

		label->setFontSize(fontSize);
		label->setName(str);
		//label->enableShadow(Color4B::BLACK, Size(8, -6), 5);
		label->setTag(Character.uniqueId);
		monsterImage->setTag(Character.uniqueId);

		CCLOG("NEW MONSTER WORD GENERATED : %s and FontSize : %f ", label->getString().c_str(), fontSize);

		if (!startSecondFlag) {
			startSecondFlag = true;
			monsterImage->setPosition(Vec2(rightBarrier->getPosition().x, (visibleSize.height * 0.52)));
			auto nameLand = currentSecondLayerRock->currentRockName;
			if (nameLand == "startLand") {
				monsterImage->setPositionX(rightBarrier->getPosition().x - 500);
			}
			else if(nameLand == "midLand"){
				monsterImage->setPositionX(rightBarrier->getPosition().x);
			}
			else
			{
				if (SecondLayerCounterMidObjectValue >= 5) {
					monsterImage->setPositionX(rightBarrier->getPosition().x - 500);
				}
				else {
					monsterImage->setPosition(Vec2(rightBarrier->getPosition().x - 300, (visibleSize.height * 0.473)));

				}
			}
		}
		else if (FirstLayerModes == LayerMode.FirstLayerRightIntersectMode) {
			monsterImage->setPosition(Vec2(rightBarrier->getPosition().x - 300, (visibleSize.height * 0.473)));
		}
		else {
			monsterImage->setPosition(Vec2(rightBarrier->getPosition().x - 300, (visibleSize.height * 0.473)));
		}
		auto parent = monsterImage->getBoundingBox();
		auto boxs = Rect(parent.origin.x + (box.origin.x), parent.origin.y + (box.origin.y), box.size.width, box.size.height);
		label->setPosition(Vec2(boxs.origin.x + (box.size.width / 2), boxs.origin.y + (box.size.height / 2) + 20));

		this->addChild(monsterImage, 9);
		this->addChild(label, 9);
		monsterImage->runAction(MoveTo::create((monsterImage->getPosition().x + std::abs(leftBarrier->getPosition().x)) / LayerMode.PathMovingSpeed, Vec2(leftBarrier->getPosition().x + origin.x, monsterImage->getPosition().y)));
		label->runAction(MoveTo::create((label->getPosition().x + std::abs(leftBarrier->getPosition().x)) / LayerMode.PathMovingSpeed, Vec2(leftBarrier->getPosition().x + origin.x, label->getPosition().y)));

		CCLOG("MONSTER GENERATED SUCCESSFULLY and TAG %d", monsterImage->getTag());

		Character.uniqueId = Character.uniqueId + 1;
		allMonster.push_back(monsterImage);
		allLabels.push_back(label);
	}
}

void EndlessRunner::addEvents(LayerGradient* sprite)
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [=](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		if (rect.containsPoint(locationInNode))
		{
			if (!Character.Clicked) {
				Character.stillCheckFalg = false;
				Character.Clicked = true;

				auto A = CallFunc::create([=]() {
					Character.action->play("jump_start", false);
					auto audioBg = CocosDenshion::SimpleAudioEngine::getInstance();
					audioBg->playEffect("endlessrunner/sound/jump_sound.wav", false);
				});
				auto B = CallFunc::create([=]() {
					auto x = Character.action;
					Character.action->play("jump_up", false);
				});
				auto C = MoveTo::create(Character.upTime, Vec2(Character.character->getPositionX(), Character.character->getPositionY() + LayerMode.heightJump));
				auto D = CallFunc::create([=]() {
					Character.action->play("jump_mid", false);
				});
				auto E = CallFunc::create([=]() {
					Character.action->play("jump_down", false);
				});
				auto G = CallFunc::create([=]() {
					Character.onAir = true;
				});
				auto F = CallFunc::create([=]() {
					EndlessRunner::FallDownCharacter();
				});
				auto main_Sequence = Sequence::create(A, B, C, DelayTime::create(0.4), D, E, F, NULL);
				Character.character->runAction(main_Sequence);
			}
		}

		if (allLabels.size() > 0 && (!_resumeHelp)) {
			this->removeChildByName("helpLayer");
			_resumeHelp = true;
			allMonster[0]->resume();
			allLabels[0]->resume();
		}

		return false;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sprite);
}
