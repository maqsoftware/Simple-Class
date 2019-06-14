//
//  BlastLetterNode.cpp 
//  goa
//
//  Created by Karim Mirazul  on 05/10/16
//
//
#ifndef __BLASTLETTERNODE_NODE_H__
#define __BLASTLETTERNODE_NODE_H__

#include "cocos2d.h"
#include "../menu/MenuContext.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "../ext/LipiTKNode.h"

class BlastLetterNode : public LipiTKNode
{
public:
	BlastLetterNode();
	~BlastLetterNode();
	cocos2d::Sprite* createDrawingBoard() override;
	static BlastLetterNode *create(int width, int height, cocos2d::Point position);
	virtual void draw(cocos2d::DrawNode *paintingNode, cocos2d::Point fromPoint, cocos2d::Point currentPoint) override;
	virtual void broadCastRecognizedChars(std::vector<std::string> results) override;
	virtual ui::Button* createButton(std::string const& normalImage, std::string const& selectedImage, std::string const& disableImage, Vec2 position) override;
	virtual void clearDrawing(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType) override;
	std::vector<std::string> getPosibileCharacter();
	void drawAllowance(bool permission);
	void setblast(bool blastOrNot);
	cocos2d::ui::Button* _clearButton;
	virtual void postTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event, cocos2d::Point touchPoint) override;

protected:
	MenuContext * menu;
	std::vector<std::string> _result;
	bool _drawAllowance = true,_blastHappend = false;
};

#endif  // __BLASTLETTERNODE_NODE_H__
