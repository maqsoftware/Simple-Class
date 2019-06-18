#include "SpriteCreate.h"
using namespace cocos2d;

SpriteCreate::SpriteCreate()
{
}

SpriteCreate::~SpriteCreate()
{
}

SpriteCreate* SpriteCreate::createSprite( std::string const& spriteName, int positionX, int positionY, float anchorX, float anchorY, std::string const& currentRockName, std::string const& nextRockName, std::string const& layerTypeName)
{
	SpriteCreate *pSprite = new SpriteCreate();

	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("thirdgamePlist.plist");
	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Game_screen.plist");


	if (pSprite->initWithSpriteFrameName(spriteName))
	{
		pSprite->setPosition(Vec2(positionX, positionY));
		pSprite->setAnchorPoint(Vec2(anchorX, anchorY));
		pSprite->currentRockName = currentRockName;
		pSprite->NextRockName = nextRockName;
		pSprite->LayerTypeName = layerTypeName;

		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

