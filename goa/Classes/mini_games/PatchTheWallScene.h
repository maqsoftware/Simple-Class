#ifndef __PATCHTHEWALL_SCENE_H__
#define __PATCHTHEWALL_SCENE_H__

#include "cocos2d.h"
#include <vector>
#include "../menu/MenuContext.h"
#include "../puzzle/Alphabet.h"
#include "../lang/TextGenerator.h"
#include "../menu/StartMenuScene.h"
#include "../menu/HelpLayer.h"
#include "../lang/Lesson.h"
#include "../util/CommonLabelTTF.h"

class PatchTheWall : public cocos2d::Layer
{
private:
	Lesson _lesson;
public:
	PatchTheWall();
	~PatchTheWall();
	MenuContext *_menuContext;

	vector<Lesson::MultiChoice> _vmc;
	cocos2d::Node *_patchBg;
	cocos2d::ui::Slider * _slideBar;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	int _moveFlag = 0, _totalLetter = 0, _totalCount = 0, _helpFlag = 0, _level;
	HelpLayer *_help;
	
	std::vector<std::vector<string>> _matrix;

	struct SpriteDetails
	{
		CommonLabelTTF *_label;
		cocos2d::Sprite *_sprite;
		string _id;
		float xP, yP;
		int _sequence;
	}SpriteDetails;
	std::vector<struct SpriteDetails> _spriteDetails;
	std::vector<struct SpriteDetails> _patchDetails;

	struct Position
	{
		float x, y;
		int _flag, _sequence;
	}Position;
	std::vector<struct Position> _position;

	virtual bool init();
	static cocos2d::Scene* createScene();
	void addEvents(struct SpriteDetails);
	void blastCome(float);
	void letterCome(Node *blastNode, int);
	bool checkAnswer(string creamText, string coneText);
	void onEnterTransitionDidFinish() override;
	void gameBegin(cocos2d::EventCustom * eventCustom);
    CREATE_FUNC(PatchTheWall);
    
    static const char* gameName() { return PATCH_THE_WALL.c_str();}
};

#endif // __PATCHTHEWALL_SCENE_H__
