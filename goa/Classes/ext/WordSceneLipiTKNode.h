//
//  WordSceneLipiTKNode.h
//  goa
//
//  Created by Shyamal.Upadhyaya on 25/10/16.
//
//

#ifndef WordSceneLipiTKNode_h
#define WordSceneLipiTKNode_h

#include "LipiTKNode.h"

class WordSceneLipiTKNode : public LipiTKNode {
public:
    WordSceneLipiTKNode();
    ~WordSceneLipiTKNode();
    
    cocos2d::Sprite* createDrawingBoard() override;
    static WordSceneLipiTKNode *create(int width, int height, cocos2d::Point position);

    void draw(cocos2d::DrawNode *paintingNode, cocos2d::Point fromPoint, cocos2d::Point currentPoint) override;

    virtual void postTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event, cocos2d::Point touchPoint) override;
    virtual void postTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event, cocos2d::Point touchPoint) override;
    virtual void postTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event, cocos2d::Point touchPoint) override;
};

#endif /* WordSceneLipiTKNode_h */
