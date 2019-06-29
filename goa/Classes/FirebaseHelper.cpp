#include "misc/FirebaseHelper.hpp"

USING_NS_CC;

bool FirebaseHelper::isStarted = false;

void FirebaseHelper::__initFirebase()
{
    isStarted = true;
    app = firebase::App::Create(
    firebase::AppOptions(), JniHelper::getEnv(), JniHelper::getActivity());
    analytics::Initialize(*app);
    analytics::SetAnalyticsCollectionEnabled(true);
}

string FirebaseHelper::replaceWhiteSpaceWithUnderscore(string eventName)
{
    string trimmed;
    for (auto &character : eventName)
    {
        if (character == ' ' || character == '_')
            continue;
        else
            trimmed += character;
    }
    return trimmed;
}

string FirebaseHelper::getLowerCase(string eventName)
{
    string allLowerCase;
    for (auto &character : eventName)
    {
        allLowerCase += putchar(tolower(character));
    }
    return allLowerCase;
}

void FirebaseHelper::logCustomEvent(string eventName, string parameter_name, double parameter_value)
{
    firebase::analytics::LogEvent(eventName.c_str(), parameter_name.c_str(), parameter_value);
}

void FirebaseHelper::pushToCurrentEvent(string eventName)
{
    setStartTime();
    (this->_currentEvent).push({eventName, this->startTime});
}

void FirebaseHelper::setStartTime()
{
    startTime = time(NULL);
}

void FirebaseHelper::setElapsedTime()
{
    elapsedTime = time(NULL);
}

time_t FirebaseHelper::getElapsedTime()
{
    return elapsedTime;
}

pair<string, time_t> FirebaseHelper::getTopEvent()
{
    pair<string, time_t> p;
    p.first = "Random";
    p.second = time(NULL);
    if (!_currentEvent.empty())
        return _currentEvent.top();
    else
        return p;
}

string FirebaseHelper::getTargetName(string reference)
{
    reference = reference.substr(reference.find('>') + 1, reference.size());
    reference = replaceWhiteSpaceWithUnderscore(reference);
    reference = getLowerCase(reference);
    return reference;
}

void FirebaseHelper::removeFromTopEvent()
{
    if (!_currentEvent.empty())
        _currentEvent.pop();
}

FirebaseHelper firebase_instance;