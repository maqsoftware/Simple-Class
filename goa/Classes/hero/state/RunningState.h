//
//  RunningState.h
//  Hello3
//
//  Created by Shyamal  Upadhyaya on 26/06/16.
//
//

#include <stdio.h>
#include "State.h"
#include "../character/SkeletonCharacter.h"
#include "StateMachine.h"

#ifndef RunningState_h
#define RunningState_h

class RunningState :  public State {
    
public:
    RunningState();
    ~RunningState();
    
    
    
    void enter(cocos2d::Vec2 forceVector, SkeletonCharacterState previousStateCommand) override;
    void exit() override;

    SkeletonCharacterState handleInput(SkeletonCharacterState command) override;

    SkeletonCharacterState getState() override;
};

#endif /* RunningState_h */
