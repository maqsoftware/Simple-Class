/*  
 * This is class is used for initiating the firebase instance
 * and some other helper functions needed to log the events.
 * 
 * Author: Vishal Maurya
 * Created: 27-06-2019
*/


#ifndef __FIREBASEHELPER_HPP__
#define __FIREBASEHELPER_HPP__

#include <vector>
#include <string>
#include <utility>
#include <ctime>
#include <stack>
#include "firebase/app.h"
#include "firebase/analytics.h"
#include "platform/android/jni/JniHelper.h"

using namespace std;
namespace analytics = firebase::analytics;

class FirebaseHelper {

    private:
        // VARIABLES NEEDED TO LOG THE EVENT
        stack< pair<string, time_t> > _currentEvent;
        time_t startTime;
        time_t endTime;
        long double elapsedTime = 0.0;
        firebase::App *app;
    
    public:
        // FUNCTIONS NEEDED TO LOG THE EVENTS
        static bool isStarted;
        void logEventBasedOnTime(pair <string, time_t>, long double);
        string replaceWhiteSpaceWithUnderscore(string);
        void __initFirebase();
        void logCustomEvent(string, string, string, string, double);
        string getTargetName(string);
        string getLowerCase(string);

        time_t getElapsedTime();
        pair<string, time_t> getTopEvent();
        void setElapsedTime();
        void setStartTime();
        void setEndTime();
        void pushToCurrentEvent(string);
        void removeFromTopEvent();

};

extern FirebaseHelper firebase_instance;

#endif