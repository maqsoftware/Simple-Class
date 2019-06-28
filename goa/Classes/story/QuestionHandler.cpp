//
//  QuestionHandler.cpp
//  goa
//
//  Created by Srikanth Talapadi on 07/01/2017.
//
//

#include "QuestionHandler.h"
#include "storage/local-storage/LocalStorage.h"
#include "MultipleChoice.hpp"
#include "FillInTheBlanks.hpp"
#include "Meaning.hpp"
#include "Picture.hpp"
#include "../puzzle/StoryWordBoard.hpp"
#include "../puzzle/WordBoard.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;


const std::string QuestionHandler::FONT_NAME = "fonts/Roboto-Regular.ttf";
const int QuestionHandler::FONT_SIZE = 85;
const Color3B QuestionHandler::FONT_COLOR = Color3B::BLACK;
const Color3B QuestionHandler::FONT_HIGHLIGHT_COLOR = Color3B::BLUE;

cocos2d::Scene *QuestionHandler::createScene(std::string storyId, std::string baseDir) {
    auto layer = QuestionHandler::create(storyId, baseDir);
    auto scene = Scene::create();
    scene->addChild(layer);
    layer->_menuContext = MenuContext::create(layer, storyId);
    scene->addChild(layer->_menuContext);
    return scene;
}

QuestionHandler *QuestionHandler::create(std::string storyId, std::string baseDir) {
    QuestionHandler* lhs = new (std::nothrow) QuestionHandler();
    if(lhs && lhs->initWithStoryId(storyId, baseDir))
    {
        lhs->autorelease();
        return lhs;
    }
    CC_SAFE_DELETE(lhs);
    return nullptr;
    
}

void QuestionHandler::onEnterTransitionDidFinish() {
    Node::onEnterTransitionDidFinish();
    gotoNextQuestion(0);
}

void QuestionHandler::gotoNextQuestion(int score) {
    int delay = 0.0;
    if(_currentQuestion >= 0) {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        _ps = CCParticleSystemQuad::create("scoreboard/particle_success.plist");
        _ps->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        this->addChild(_ps, 10);
        delay = 3.0;
    }
    runAction(Sequence::create(DelayTime::create(delay), CallFunc::create([=] {
        if(_ps) {
            this->removeChild(_ps);
            _ps = nullptr;
        }
        if(++_currentQuestion < _questions.size()) {
            if(_currentQuestionNode != nullptr) {
                removeChild(_currentQuestionNode);
            }
            if(_questions[_currentQuestion][0] == "multiple_choice") {
                _currentQuestionNode = MultipleChoice::create(this, _questions[_currentQuestion]);
                addChild(_currentQuestionNode);
            } else if(_questions[_currentQuestion][0] == "fill_in_the_blanks") {
                _currentQuestionNode = FillInTheBlanks::create(this, _questions[_currentQuestion]);
                addChild(_currentQuestionNode);
            } else if(_questions[_currentQuestion][0] == "meanings") {
                    _currentQuestionNode = Meaning::create(this, _questions[_currentQuestion]);
                    addChild(_currentQuestionNode);
            } else if(_questions[_currentQuestion][0] == "picture") {
                _currentQuestionNode = Picture::create(this, _questions[_currentQuestion]);
                addChild(_currentQuestionNode);
            } else if(_questions[_currentQuestion][0] == "words") {
                Director::getInstance()->replaceScene(TransitionFade::create(1.0, StoryWordBoard::createSceneWithWords(_storyId, _questions[_currentQuestion], 1, _baseDir, _menuContext->getMaxPoints(), _menuContext->getPoints()), Color3B::BLACK));
            } else {
                _menuContext->showScore();
            }
        } else {
            _menuContext->showScore();
        }
    }), NULL));
}

QuestionHandler::QuestionHandler() :
_currentQuestion(-1),
_currentQuestionNode(nullptr),
_totalPoints(0)
{
    
}

QuestionHandler::~QuestionHandler() {
    
}

bool QuestionHandler::init() {
    return true;
}

std::string QuestionHandler::getBaseDir() {
    return _baseDir;
}

bool QuestionHandler::initWithStoryId( std::string const& storyId, std::string const& baseDir) {
    _storyId = storyId;
    _baseDir = baseDir;
    std::string questionsJson = "story/" + LangUtil::getInstance()->getLang() + "/" + _baseDir + ".questions.json";
    if(FileUtils::getInstance()->isFileExist(questionsJson)) {
        std::string jsonData = FileUtils::getInstance()->getStringFromFile(questionsJson);
        rapidjson::Document qDoc;
        if (false == qDoc.Parse<0>(jsonData.c_str()).HasParseError()) {
            for (rapidjson::Value::ConstMemberIterator itr = qDoc.MemberBegin();
                 itr != qDoc.MemberEnd(); ++itr) {
                if(itr->name == "fill_in_the_blanks") {
                    const rapidjson::Value& f = itr->value;
                    assert(f.IsArray());
                    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
                        const rapidjson::Value& q = f[i];
                        if(q.HasMember("answer") && q.HasMember("choices") && q.HasMember("question")) {
                            std::vector<std::string> eachQ;
                            eachQ.push_back("fill_in_the_blanks");
                            eachQ.push_back(q["question"].GetString());
                            eachQ.push_back(q["answer"].GetString());
                            const rapidjson::Value& choicesVal = q["choices"];
                            assert(choicesVal.IsArray());
                            for (rapidjson::SizeType choicesValIndex = 0; choicesValIndex < choicesVal.Size(); choicesValIndex++) {
                                eachQ.push_back(choicesVal[choicesValIndex].GetString());
                            }
                            _questions.push_back(eachQ);
                            _totalPoints += 2;
                        }
                    }
                } else if(itr->name == "meanings") {
                    const rapidjson::Value& f = itr->value;
                    assert(f.IsArray());
                    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
                        const rapidjson::Value& q = f[i];
                        std::vector<std::string> eachQ;
                        eachQ.push_back("meanings");
                        for (rapidjson::Value::ConstMemberIterator itr = q.MemberBegin(); itr != q.MemberEnd(); ++itr) {
                            eachQ.push_back(itr->name.GetString());
                            eachQ.push_back(itr->value.GetString());
                        }
                        _questions.push_back(eachQ);
                        _totalPoints += 4;
                    }
                } else if(itr->name == "multiple_choice") {
                    const rapidjson::Value& f = itr->value;
                    assert(f.IsArray());
                    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
                        const rapidjson::Value& q = f[i];
                        if(q.HasMember("answer") && q.HasMember("choices") && q.HasMember("question")) {
                            std::vector<std::string> eachQ;
                            eachQ.push_back("multiple_choice");
                            eachQ.push_back(q["question"].GetString());
                            eachQ.push_back(q["answer"].GetString());
                            const rapidjson::Value& choicesVal = q["choices"];
                            assert(choicesVal.IsArray());
                            for (rapidjson::SizeType choicesValIndex = 0; choicesValIndex < choicesVal.Size(); choicesValIndex++) {
                                eachQ.push_back(choicesVal[choicesValIndex].GetString());
                            }
                            _questions.push_back(eachQ);
                            _totalPoints += 2;
                        }
                    }
                } else if(itr->name == "picture") {
                    const rapidjson::Value& f = itr->value;
                    assert(f.IsArray());
                    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
                        const rapidjson::Value& q = f[i];
                        std::vector<std::string> eachQ;
                        eachQ.push_back("picture");
                        for (rapidjson::Value::ConstMemberIterator itr = q.MemberBegin(); itr != q.MemberEnd(); ++itr) {
                            eachQ.push_back(itr->name.GetString());
                            eachQ.push_back(itr->value.GetString());
                        }
                        _questions.push_back(eachQ);
                        _totalPoints += 4;
                    }
                } else if(itr->name == "words") {
                    const rapidjson::Value& f = itr->value;
                    assert(f.IsArray());
                    std::vector<std::string> eachQ;
                    eachQ.push_back("words");
                    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
                        std::string word = f[i].GetString();
                        std::transform(word.begin(), word.end(),word.begin(), ::toupper);
                        eachQ.push_back(word);
                        _totalPoints++;
                    }
                    _questions.push_back(eachQ);
                }
            }
        }
    }
    return true;
    
}

MenuContext* QuestionHandler::getMenuContext() {
    return _menuContext;
}

void QuestionHandler::setButtonProperties(Node *button, std::string name, std::string text, const ui::Widget::ccWidgetTouchCallback &callback, const ui::Widget::ccWidgetTouchCallback &soundCallback, size_t length, const int fontSize){
    auto button1 = static_cast<Button*> (button);
    if(button1) {
        button1->setName(name);
        if(button1->getParent()) {
            if(button1->getParent()->getName() == "bg") {
                button1->setTitleText(wrapString(text, length));
                button1->setTitleFontName(QuestionHandler::FONT_NAME);
                button1->setTitleColor(QuestionHandler::FONT_COLOR);
                button1->setTitleFontSize(72);
            } else {
                auto label = Label::createWithTTF(wrapString(text, length), QuestionHandler::FONT_NAME, fontSize);
                label->setTextColor(Color4B(QuestionHandler::FONT_COLOR));
                button1->getParent()->addChild(label);
            }
        }
        
        Button* _soundButton = Button::create("template/template_02/sound_button.png", "template/template_02/click_sound_button.png", "template/template_02/click_sound_button.png", ui::Widget::TextureResType::PLIST);
        _soundButton->setName("sound_button");
        _soundButton->setAnchorPoint(Vec2(1,1));
        _soundButton->setScale(0.5);
        _soundButton->setPosition(Vec2(button1->getPosition().x + button1->getBoundingBox().size
                                       .width/2,button1->getPosition().y + button1->getBoundingBox().size.height/2));
        if(!button1->getTitleText().empty())
        {
            _soundButton->setTitleText(button1->getTitleText());
        } else {
            _soundButton->setTitleText(text);
        }
        _soundButton->addTouchEventListener(soundCallback);
        button1->getParent()->addChild(_soundButton);
        button1->addTouchEventListener(callback);
    }

}

std::string QuestionHandler::wrapString(std::string str, size_t width) {
    size_t curWidth = width;
    
    
    while( curWidth < str.length() ) {
        std::string::size_type spacePos = str.rfind( ' ', curWidth );
        if( spacePos == std::string::npos )
            spacePos = str.find( ' ', curWidth );
        if( spacePos != std::string::npos ) {
            str[ spacePos ] = '\n';
            curWidth = spacePos + width + 1;
        } else {
            return str;
        }
    }
    
    return str;
}


