#ifndef _SPRITECREATE_HPP_
#define _SPRITECREATE_HPP_

#include "cocos2d.h"
#include "editor-support/cocostudio/CocoStudio.h"
class SpriteCreate : public cocos2d::Sprite
{
public:

	SpriteCreate();
	~SpriteCreate();

	std::string currentRockName, NextRockName, LayerTypeName;
	
	static SpriteCreate* createSprite(std::string const& spriteName, int postionX, int postionY, float anchorX, float anchorY, std::string const& currentRockName, std::string const& NextRockName, std::string const& LayerTypeName);

private:
};

#endif // __EVENTLISTENERCLASS_SCENE_H__
