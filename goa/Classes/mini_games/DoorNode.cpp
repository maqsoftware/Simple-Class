//
//  DoorNode.cpp 
//  goa
//



#include "DoorNode.h"
#include "Door.h"
#include "../ext/WordSceneLipiTKNode.h"

USING_NS_CC;


DoorNode::DoorNode()
{
}

DoorNode::~DoorNode()
{

}

cocos2d::Sprite * DoorNode::createDrawingBoard()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto drawingBoardSprite = Sprite::create();
	drawingBoardSprite->setTextureRect(Rect(0, 0,_canvasWidth,_canvasHeight));
	drawingBoardSprite->setColor(Color3B::BLACK);
	drawingBoardSprite->setOpacity(50);
	return drawingBoardSprite;
}

DoorNode * DoorNode::create(int width, int height, cocos2d::Point position)
{
	DoorNode* wordSceneLipiTKNode = new (std::nothrow) DoorNode();
	if (wordSceneLipiTKNode && wordSceneLipiTKNode->initialize(width, height, position))
	{
		wordSceneLipiTKNode->autorelease();
		wordSceneLipiTKNode->setPosition(position);
		return wordSceneLipiTKNode;
	}
	CC_SAFE_DELETE(wordSceneLipiTKNode);
	return nullptr;
}

void DoorNode::draw(cocos2d::DrawNode * paintingNode, cocos2d::Point fromPoint, cocos2d::Point currentPoint)
{
	paintingNode->drawSegment(fromPoint, currentPoint, 30, Color4F(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
	//paintingNode->drawSegment(fromPoint, currentPoint, 5, Color4F(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
}



void DoorNode::broadCastRecognizedChars(std::vector<std::string> results)
{

//	CCLOG("car draw = %s", results.at(0).c_str());
//	_Door->characterRecogination(results.at(0).c_str());
	_Door->characterRecognisation(results);
}


void DoorNode::setParent(Door * parent)
{
	this->_Door = parent;
}

void DoorNode::clearDrawing(cocos2d::Ref * pSender, cocos2d::ui::Widget::TouchEventType eEventType)
{
	if (eEventType == cocos2d::ui::Widget::TouchEventType::ENDED)
	{

		_paintingNode->clear();
		_strokes.clear();
		_Door->clearScreen();
	}
}
void DoorNode::postTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint)
{
	CCLOG("touchPoint.x %f", touchPoint.x);
	CCLOG("touchPoint.y %f", touchPoint.y);
	_Door->postTouchBegan(touch, event, touchPoint);
}

void DoorNode::postTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint)
{
	CCLOG("touchPoint.x  in move %f", touchPoint.x);
	CCLOG("touchPoint.y  in move %f", touchPoint.y);
	_Door->postTouchMoved(touch, event, touchPoint);
	//	paintingNode->drawSegment(fromPoint, currentPoint, 5, Color4F(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
}

void DoorNode::postTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint)
{
	CCLOG("touchPoint.x  in end %f", touchPoint.x);
	CCLOG("touchPoint.y  in end %f", touchPoint.y);
	_Door->postTouchEnded(touch, event, touchPoint);
}
cocos2d::ui::Button *DoorNode::createButton(std::string const &normalImage, std::string const &selectedImage, std::string const& disableImage, cocos2d::Vec2 position)
{

	auto spritecache1 = SpriteFrameCache::getInstance();
	spritecache1->addSpriteFramesWithFile("cardraw/cardraw.plist");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	_button = cocos2d::ui::Button::create("cardraw/ref.png", "cardraw/ref_clicked.png", "cardraw/ref.png", cocos2d::ui::Widget::TextureResType::PLIST);
	_button->setPosition(Vec2(position.x - _button->getContentSize().width *2.7,position.y));
	_button->setName("carDrawRefresh");
	_button->setScale(0.5);
	_button->addTouchEventListener(CC_CALLBACK_2(DoorNode::clearDrawing, this));

	return _button;
}


