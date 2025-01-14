//
//  DotNum.hpp
//  goa
//
//  Created by Srikanth Talapadi on 01/04/17.
//
//

#ifndef DotNum_hpp
#define DotNum_hpp
#include "cocos2d.h"

class DotNum : public cocos2d::Node {
public:
    static DotNum* create(int num, bool isShowFinger = true);
    int getNum();
CC_CONSTRUCTOR_ACCESS:
    DotNum();
    virtual ~DotNum();
    bool init(int num, bool isShowFinger);
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
protected:
    int _num;
    bool _isShowFinger;
    cocos2d::Node* _dotNode;
    void enableTouch(bool enabled);
};

#endif /* DotNum_hpp */
