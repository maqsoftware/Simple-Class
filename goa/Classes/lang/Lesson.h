//
//  Lesson.hpp
//  goa
//
//  Created by Srikanth Talapadi on 31/07/17.
//
//

#ifndef Lesson_h
#define Lesson_h

#include "cocos2d.h"
#include <stdio.h>
#include <iterator> // std::ostream_iterator
using namespace std;
USING_NS_CC;

static const int ANY_FORMAT = 0;
static const int UPPER_CASE_LETTER_FORMAT = 1;

class Lesson
{
public:
    enum class CONCEPT {
        LETTER = 1,
        LETTER_CASE_EQUATE = 2,
        LETTER_ORDER = 3,
        LETTER_SOUND = 4,
        LETTER_PHONIC = 5,
        LETTER_TYPE = 6,
        LETTER_WRITING = 7,
        SYLLABLE_SOUND = 8,
        WORD_BEGINNING = 9,
        WORD_SPELLING = 10,
        WORD_SOUND = 11,
        WORD_SPLIT_INTO_SYLLABLES = 12,
        WORD_WIRITING = 13,
        SENTENCE_SPLIT_INTO_WORDS = 14
    };
    
    typedef struct _MultiChoice
    {
        std::string help;
        std::string question;
        std::vector<std::string> answers;
        int correctAnswer;
    } MultiChoice;

    std::vector<MultiChoice> getMultiChoices(int lessons = 1, int choices = 4, int answerFormat = ANY_FORMAT, int choiceFormat = ANY_FORMAT);
    
    typedef struct _Bag
    {
        std::string help;
        std::string answerString;
        std::vector<std::string> answers;
        std::vector<std::string> otherChoices;
    } Bag;
    
    std::vector<Bag> getBag(int lessons = 1, int minAnswers = 1, int maxAnswers = 4,
                            int minChoices = 3, int maxChoices = 6,
                            bool order = true);
    
    void setSessionId(int sessionId);
    int getSessionId();
    
    void setComplexity(int complexity);
    int getComplexity();
    
    void setConcept(CONCEPT concept);
    void setComplexityAndConcept(int cc);
    CONCEPT getConcept();
    
    std::function<void(std::vector<MultiChoice>)> multiChoiceReadyCallback;
    virtual void onReceivedMultipleChoiceQuiz(cocos2d::EventCustom *eventCustom);
    static std::vector<MultiChoice> unmarshallMultiChoices(std::string* strArray);
    static std::vector<Bag> unmarshallBag(std::string* strArray);
    
CC_CONSTRUCTOR_ACCESS:
    Lesson();
    virtual ~Lesson();
    
protected:
    int _sessionId = 0; //Setting an initial value
    int _complexity = 0; //Setting an initial value
    CONCEPT _concept;
};

#endif /* Lesson_h */
