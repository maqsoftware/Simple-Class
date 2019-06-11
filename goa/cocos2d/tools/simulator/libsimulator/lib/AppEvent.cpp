//
//  AppEvent.cpp
//  Simulator
//
//

#include "AppEvent.h"

AppEvent::AppEvent(const std::string& eventName, int type)
: EventCustom(eventName)
, _eventName(eventName)
{
    setEventType(type);
}

void AppEvent::setEventType(int type)
{
    _eventType = type;
}
int  AppEvent::getEventType()
{
    return _eventType;
}

void AppEvent::setDataString(const std::string data)
{
    _dataString = data;
}

std::string AppEvent::getDataString()
{
    return _dataString;
}