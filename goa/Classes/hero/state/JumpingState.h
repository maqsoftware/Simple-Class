//
//  JumpingState.h
//  Hello3
//
//  Created by Shyamal  Upadhyaya on 26/06/16.
//
//

#include <stdio.h>
#include "State.h"
#include "StateMachine.h"
#include "../character/SkeletonCharacter.h"

#ifndef JumpingState_h
#define JumpingState_h

USING_NS_CC;

class JumpingState :  public State {
    
public:
    JumpingState();
    ~JumpingState();
        
    void enter(cocos2d::Vec2 forceVector, SkeletonCharacterState previousStateCommand) override;

    void exit() override;

    SkeletonCharacterState handleInput(SkeletonCharacterState command) override;

    SkeletonCharacterState getState() override;
};




#endif /* JumpingState_h */
