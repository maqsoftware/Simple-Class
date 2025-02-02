/// <reference path="cocos2d-typescript-definitions-master/cocos2d/cocos2d-lib.d.ts" />
var xc = xc || {};
xc._MULTIPLE_CHOICE_HELP_SHOWN=false;
xc._FILL_IN_THE_BLANKS_HELP_SHOWN=false;
xc._MEANING_HELP_SHOWN=false;
xc._PICTURE_HELP_SHOWN=false;
xc._READING_STORY_FIRST_TIME=true;
xc.StoryQuestionHandlerLayer = cc.Layer.extend({
    _contentPanel: null,
    _pageConfigPanel: null,
    _storyBaseDir: "",
    _questions: [],
    _currentQuestionIndex: 0,
    _contentPanelWidth: null,
    _configPanelWidth: null,
    _Q_MULTIPLE_CHOICE: "multiple_choice",
    _Q_FILL_IN_THE_BLANKS:"fill_in_the_blanks",
    _Q_MEANINGS:"meanings",
    _Q_PICTURES:"picture",
    _Q_WORDS: "words",    
    _currentQName: null,    
    _totalPoints: 0,
    _menuContext: null,
    _storyId: null,
    _hasWordsQuestions: false,
    _handler: null,
    _resources: [],

    ctor: function (storyId, storyBaseDir, resources) {
        this._super();
        this._name = "StoryQuestionHandlerLayer";
        this._tabHeight = 64;
        this._storyId = storyId;
        this._storyBaseDir = storyBaseDir;
        this._controlPanel = null;
        this._contentPanelWidth = cc.director.getWinSize().width; //assuming landscape
        this._contentPanelHeight = cc.director.getWinSize().height; //assuming landscape
        this._configPanelWidth = (cc.director.getWinSize().width - this._contentPanelWidth) / 2;
        this._resources = resources;

        return true;
    },

    displayText:function(text, location) {
        var texts = text.split("_");
        if(texts && texts.length > 0) {
            var langText = texts[0];
            cc.log('text:' + langText.toLowerCase());
            this._text = new cc.LabelTTF(text, "Arial", 100)
            this._text.setName("wordMeaning");
            this._text.color = new cc.Color(255, 255, 255);
            this._text.setPosition(location.x, location.y + 1000);
            this.addChild(this._text);
            var textDropAction = new cc.MoveTo(0.5, cc.p(location.x, location.y));
            textDropAction.easing(cc.easeBackOut());
            this._text.runAction(textDropAction);            
        }
        
    },

    init: function (menuContext) {
        cc.log('this._storyBaseDir:' + this._storyBaseDir);
        this._menuContext = menuContext;        
        this.loadQuestions();
        this._referenceToContext = this;

        this._item = new ccui.Button('template/template_02/skip.png', 'template/template_02/skip_click.png', null, ccui.Widget.PLIST_TEXTURE);
        this._item.setPosition(cc.p(150,cc.director.getWinSize().height - 150));
        this._item.addTouchEventListener(this.skipQuestion, this);
        this.addChild(this._item, 5);
    },


    skipQuestion: function(sender, type) {
        switch (type) {
            case ccui.Widget.TOUCH_ENDED:  
                this.questionCallBack(sender, false, true);
                sender.setEnabled(false);
                break;
        }        
    },

    wordQuestionHandler: function(questions) {
        if(this._currentQName) {
            var oldQuestionChild = this.getChildByName(this._currentQName);
            if(oldQuestionChild) {
                oldQuestionChild.removeFromParent();
            }
            
        }
        this._handler = new xc.WordQuestionHandler(this._storyId, cc.director.getWinSize().width, cc.director.getWinSize().height, questions, this._storyBaseDir, this.getParent()._menuContext.getMaxPoints(), this.getParent()._menuContext.getPoints());
        this._handler.setName(this._Q_WORDS); 
        this._currentQName = this._Q_WORDS;
        this._hasWordsQuestions = true;
        this.addChild(this._handler);        
    },
    questionHandler: function(question) {
        if(this._currentQName) {
            var oldQuestionChild = this.getChildByName(this._currentQName);
            if(oldQuestionChild) {
                oldQuestionChild.removeFromParent();
            }            
        }
        var questionType = question["type"];
        if(questionType == this._Q_MULTIPLE_CHOICE) {
            cc.log('handle multiple choice question');
            this._handler = new xc.MultipleChoiceQuestionHandler(xc.StoryQuestionHandlerLayer.res.multi_question_choice_json, cc.director.getWinSize().width, cc.director.getWinSize().height, question, this.questionCallBack, this);
            this._handler.setName(this._Q_MULTIPLE_CHOICE); 
            this._currentQName = this._Q_MULTIPLE_CHOICE;          
        } else if(questionType == this._Q_FILL_IN_THE_BLANKS) {
            this._handler = new xc.FillInTheBlanksQuestionHandler(xc.StoryQuestionHandlerLayer.res.multi_question_choice_json, cc.director.getWinSize().width, cc.director.getWinSize().height, question, this.questionCallBack, this);
            this._handler.setName(this._Q_FILL_IN_THE_BLANKS); 
            this._currentQName = this._Q_FILL_IN_THE_BLANKS;          
        } else if(questionType == this._Q_MEANINGS) {
            this._handler = new xc.MeaningQuestionHandler(xc.StoryQuestionHandlerLayer.res.meaning_question_choice_json, cc.director.getWinSize().width, cc.director.getWinSize().height, question, this.questionCallBack, this);
            this._handler.setName(this._Q_MEANINGS); 
            this._currentQName = this._Q_MEANINGS;          
        } else if(questionType == this._Q_PICTURES) {
            this._handler = new xc.PictureQuestionHandler(this._storyBaseDir, xc.StoryQuestionHandlerLayer.res.picture_question_choice_json, cc.director.getWinSize().width, cc.director.getWinSize().height, question, this.questionCallBack, this);
            this._handler.setName(this._Q_PICTURES); 
            this._currentQName = this._Q_PICTURES;
        } 
        this.addChild(this._handler);        
    },

    questionCallBack: function(sender, isCorrect, isAllAnswered) {
        //play animation for correct/incorrect answer, update score and move to next question
        this.updateScore(isCorrect);
        this.postAnswerAnimation(isCorrect, isAllAnswered);
    },

    updateScore: function(isCorrect) {
        cc.log("update score for answer :" + isCorrect);
        if(cc.sys.isNative) {
            if(isCorrect) {
                this._menuContext.addPoints(1);
                cc.log("current point in Story Question Handler: %d",  this._menuContext.getPoints());
            } else {
                this._menuContext.addPoints(-1);
                cc.log("current point in Story Question Handler: %d",  this._menuContext.getPoints());
            }            
        }
    },

    finishedSuccessAnimation: function() {
        if(this._referenceToContext._isAllAnswered) {
            this._referenceToContext.nextQuestion();
        }
    },
    
    postAnswerAnimation: function(isCorrect, isAllAnswered) {
        cc.log("postAnswerAnimation for answer :" + isCorrect);
        this._isAllAnswered = isAllAnswered;
        if(this._isAllAnswered) {
            cc.log("play success animation");
            var delayAction = new cc.DelayTime(2.0);                        
            var sequenceAction = new cc.Sequence(delayAction, new cc.CallFunc(this.finishedSuccessAnimation, this));
            this.runAction(sequenceAction);          
        }
    },

    nextQuestion: function () {
        if(this._handler) {
            this._handler.removeFromParent();
        }

        if(xc._currentQuestionIndex == undefined) {
            xc._currentQuestionIndex = 0;
        }                
        this._item.setEnabled(true);

        cc.log('current meny point before transition: ' + this._menuContext.getPoints());
        cc.sys.localStorage.setItem("xc.story.totalPoints", ""+this._menuContext.getMaxPoints());
        cc.sys.localStorage.setItem("xc.story.currentPoints", ""+this._menuContext.getPoints());

        if(xc._currentQuestionIndex < this._questions.length) {
            var question = this._questions[xc._currentQuestionIndex];
            cc.log('question["type"]:' + question["type"]);
            if(question["type"] == "words") {
                xc._currentQuestionIndex++;
                var cIndex = xc._currentQuestionIndex;
                cc.log("cIndex 1111:" + cIndex);
                var questions = this._questions.filter(function(element, index) {
                    return index >= cIndex;
                });
                xc.wordQuestions = questions;
                xc.StoryQuestionTransitionScene.load(this._storyId, this._storyBaseDir, xc.StoryQuestionTransitionLayer);
                //xc.StoryQuestionHandlerScene.load(this._storyId, this._storyBaseDir, xc.StoryQuestionHandlerLayer, true);
            } else {
                xc._currentQuestionIndex++;
                xc.StoryQuestionTransitionScene.load(this._storyId, this._storyBaseDir, xc.StoryQuestionTransitionLayer);
                //xc.StoryQuestionHandlerScene.load(this._storyId, this._storyBaseDir, xc.StoryQuestionHandlerLayer, true);
            }                            
        } else {
            this.showCopyRight();                      
        }
    },

    copyrightShown: function() {
        cc.log("copy right shown");
        this._item.setVisible(false);
        if(cc.sys.isNative)
        {
            this._menuContext.showScore();
        } else {
            xc.CatalogueScene.load(xc.CatalogueLayer);
        }               
    },

    showCopyRight: function() {      
        //load text file based on Current Story Id and Page index
        this._isTextShown = true;
        var langDir = goa.TextGenerator.getInstance().getLang();
        cc.log("langDir:" + langDir);
        var storyText = "";
        var that = this;
        var textFileUrl =  "res/story" + "/" + langDir + "/" + this._storyBaseDir + ".json";
        cc.log('textFileUrl:' + textFileUrl);
        if(cc.sys.isNative) {
            var fileExists = jsb.fileUtils.isFileExist(textFileUrl);
            if(fileExists) {

                cc.loader.loadJson(textFileUrl, function(err, json) {            
                    if(!err && json != null && json != undefined) {
                        storyText = json["copyright"];
                        cc.log('story text received:' + storyText);
                        that.parent.addChild(new xc.CopyRightHandler(xc.StoryQuestionHandlerLayer.res.copyright_json, storyText, cc.director.getWinSize().width, cc.director.getWinSize().height, that.copyrightShown, that));
                    }                                
                });                
           
            } 
        } else {
            cc.loader.loadJson(textFileUrl, function(err, json) {            
                if(!err && json != null && json != undefined) {
                    storyText = json["copyright"];
                    cc.log('story text received:' + storyText);
                    that.parent.addChild(new xc.CopyRightHandler(xc.StoryQuestionHandlerLayer.res.copyright_json, storyText, cc.director.getWinSize().width, cc.director.getWinSize().height, that.copyrightShown, that));
                }                                
            });                            
        }        
    },    

    loadQuestions: function() {
        var langDir = goa.TextGenerator.getInstance().getLang();
        cc.log("langDir:" + langDir);
        var storyText = "";
        var that = this;
        var questionFileUrl =  "res/story" + "/" + langDir + "/" + this._storyBaseDir + ".questions.json";
        cc.log('questionFileUrl:' + questionFileUrl);

        if(cc.sys.isNative) {
            var fileExists = jsb.fileUtils.isFileExist(questionFileUrl);
            if(fileExists) {
                cc.loader.loadJson(questionFileUrl, function(err, json) {            
                    if(!err && json != null && json != undefined) {
                        cc.log('story questions received:' + json);
                        that.buildQuestions(json);
                    }                                
                });                           
            } 
        } else {
            cc.loader.loadJson(questionFileUrl, function(err, json) {            
                if(!err && json != null && json != undefined) {
                    cc.log('story questions received:' + json);
                    that.buildQuestions(json);                    
                }                                
            });                            
        }    
    },

    buildQuestions: function (json) {
        //build questions array for easy looping based on index
        this.processQuestions(json[this._Q_MULTIPLE_CHOICE], this._Q_MULTIPLE_CHOICE);
        this.processQuestions(json[this._Q_FILL_IN_THE_BLANKS], this._Q_FILL_IN_THE_BLANKS);
        this.processQuestions(json[this._Q_MEANINGS], this._Q_MEANINGS);
        this.processQuestions(json[this._Q_PICTURES], this._Q_PICTURES);
        // if(cc.sys.isNative) {
            this.processQuestions(json[this._Q_WORDS], this._Q_WORDS);
        // }
                
        cc.log("questions:" + this._questions.length);
        cc.log("total points:" + this._totalPoints);
        if(cc.sys.isNative) {
            this._menuContext.setMaxPoints(this._totalPoints);
        }
        //create UI for questions
        var question = this._questions[xc._currentQuestionIndex];
        if(this._handler) {
            this._handler.removeFromParent();
        }
        if(question) {
            if(question["type"] == "words") {
                if(xc.wordQuestions) {
                    this.wordQuestionHandler(xc.wordQuestions);
                } else {
                    this.wordQuestionHandler(this._questions);
                }
                
            } else {
                this.questionHandler(question);
            }
            
        } else if(cc.sys.isNative) {
            this._menuContext.showScore();
        }
                             
    },

    computePoints:function(array, type) {
        var context = this;
        if(type == this._Q_MULTIPLE_CHOICE) {
            this._totalPoints += 1 * array.length;
        } else if(type == this._Q_FILL_IN_THE_BLANKS) {
            this._totalPoints += 1 * array.length;
        } else if(type == this._Q_MEANINGS) {
            this._totalPoints += 4 * array.length;
        } else if(type == this._Q_PICTURES) {
            this._totalPoints += 4 * array.length;
        } else if(type == this._Q_WORDS) {
            this._totalPoints += 1 * array.length;
        }
    },

    processQuestions: function (array, type) {        
        if(array && array.length > 0) {
            this.computePoints(array, type);
            array = array.map(function(question, index){
                if(typeof question === 'object') {
                    question["type"]=type;
                    return question;                     
                } else {
                    questionO = {};
                    questionO["word"] = question;
                    questionO["type"] = type;
                    return questionO;
                }                
            });
            this._questions = this._questions.concat(array);
        }
    },


    onExit: function() {        
        this._super();   
        var that = this;       
    }    
});

xc.StoryQuestionHandlerScene = cc.Scene.extend({
    layerClass: null,
    _menuContext: null,
    ctor: function (storyId, storyBaseDir, resources, layer) {
        this._super();
        this.layerClass = layer;
        this._sceneLayer = new this.layerClass(storyId, storyBaseDir, resources);
        this.addChild(this._sceneLayer);
        if (cc.sys.isNative) {
            this._menuContext = goa.MenuContext.create(this._sceneLayer, storyId);
            this.addChild(this._menuContext, 10);
            this._menuContext.setVisible(true);        
            cc.log('current meny point at entry transition: ' + this._menuContext.getPoints());
            var currentPoints = cc.sys.localStorage.getItem("xc.story.currentPoints");
            this._menuContext.addPoints(currentPoints);
                
        }                                        
        
        this._sceneLayer.init(this._menuContext);
    }
});


xc.StoryQuestionHandlerScene.load = function(storyId, storyBaseDir, layer, enableTransition) {
    var that = this;
    var t_resources = [];
   
    if(storyBaseDir != null) {           
        for (var i in layer.res) {
            cc.log('preloading:' + layer.res[i]);
            t_resources.push(layer.res[i]);
        }        
        cc.LoaderScene.preload(t_resources, function () {
            if(cc.sys.isNative) {
                xc.storyQuestionConfigurationObject = cc.loader.getRes(xc.StoryQuestionHandlerLayer.res.storyQuestionsConfig_json);                         
            } else {
                xc.storyQuestionConfigurationObject = cc.loader.cache[xc.StoryQuestionHandlerLayer.res.storyQuestionsConfig_json];
            }

            // cc.spriteFrameCache.addSpriteFrames(xc.StoryQuestionHandlerLayer.res.particle_system_plist);
            // cc.spriteFrameCache.addSpriteFrames(xc.StoryQuestionHandlerLayer.res.template_plist);
            // cc.spriteFrameCache.addSpriteFrames(xc.StoryQuestionHandlerLayer.res.template_01_plist);                        
            // cc.spriteFrameCache.addSpriteFrames(xc.StoryQuestionHandlerLayer.res.template_02_plist);
            var scene = new xc.StoryQuestionHandlerScene(storyId, storyBaseDir, t_resources, layer);
            scene.layerClass = layer;            
            if(enableTransition) {
                cc.director.runScene(new cc.TransitionFade(2.0, scene, true));
            }  else {
                cc.director.runScene(scene);
            }              
        }, this);            
    }
}


xc.StoryQuestionHandlerLayer.res = {
        storyQuestionsConfig_json: xc.path + "misc/storyQuestionsConfig.json",
        multi_question_choice_json: xc.path + "template/template.json",
        picture_question_choice_json: xc.path + "template/template_1.json",
        meaning_question_choice_json: xc.path + "template/template_2.json",
        copyright_json: xc.path + "template/copyright.json"
};