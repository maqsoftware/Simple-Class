/**
 *   This class is used for scaling the circular icons based on the 
 *   resolution of the device.
 *   It can accept any kind of GUI Element (Sprite, Buttons etc. ) provided by cocos.
 *
 *   Author: Vishal Maurya
 *   Created: 07.06.2019
**/

#include <iostream>
using namespace std;

USING_NS_CC;

#ifndef _SCALE_GUI_ELEMENTS_
#define _SCALE_GUI_ELEMENTS_

template <class T>
class ScaleUIElement
{
    public:
        static void scaleGuiElements(T menuButton)
        {
            Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
            float aspectRatio = (1.0 * frameSize.width) / frameSize.height;

            if (aspectRatio >= 2.0) 
            {
                menuButton->setScaleX(0.70);
            }
            else if (aspectRatio >= 1.69 && aspectRatio < 2.0)
            {
                menuButton->setScaleX(0.82);
            }
            else if (aspectRatio >= 1.5 && aspectRatio < 1.69)
            {
                menuButton->setScaleX(0.88);
            }
        }
};
#endif // _SCALE_GUI_ELEMENTS_