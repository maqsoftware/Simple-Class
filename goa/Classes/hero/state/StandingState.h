//
//  StandingState.h
//  Hello3
//
//  Created by Shyamal  Upadhyaya on 25/06/16.
//
//

#include <iostream>
#include "State.h"
#include "../character/SkeletonCharacter.h"
#include "StateMachine.h"

#ifndef StandingState_h
#define StandingState_h
class StandingState :  public State {
    
public:
    StandingState();
    ~StandingState();
    
    void enter(cocos2d::Vec2 forceVector, SkeletonCharacterState previousStateCommand) override;

    void exit() override;
    SkeletonCharacterState handleInput(SkeletonCharacterState command) override;
    SkeletonCharacterState getState() override;
};




#endif /* StandingState_h */
