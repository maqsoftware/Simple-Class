/// <reference path="cocos2d-typescript-definitions-master/cocos2d/cocos2d-lib.d.ts" />
var xc = xc || {};
xc.storyCoverPageFontSize = 120;
xc.storyFontName = "Arial";
xc.storyFontSize = 85;
xc.storyCoverPageFontSize = 120;
xc.storyFontColor = cc.color.BLACK;

xc.StoryCoverPageLayer = cc.Layer.extend({
    _contentPanel: null,
    _pageConfigPanel: null,
    _storyIndex: 0,
    _objectConfigPanel: null,
    _contentPanelWidth: null,
    _configPanelWidth: null,
    _configPanelHeight: null,
    _isTitleDisplayed: false,
    _isTextShown: false,
    _resources: [],
    _soundId: null,

    ctor: function (pageIndex, storyInformation, resources) {
        this._super();
        this._name = "StoryCoverPageLayer";
        this._tabHeight = 64;
        this._pageIndex = pageIndex;
        this._storyInformation = storyInformation;
        this._controlPanel = null;
        this._contentPanelWidth = cc.director.getWinSize().width; //assuming landscape
        this._contentPanelHeight = cc.director.getWinSize().height; //assuming landscape
        this._configPanelWidth = (cc.director.getWinSize().width - this._contentPanelWidth) / 2;
        this._resources = resources;

        return true;
    },

    bindTouchListener: function (target, funcName, loop) {
        var context = this;
        var listener = cc.EventListener.create({
            event: cc.EventListener.TOUCH_ONE_BY_ONE,
            swallowTouches: true,
            onTouchBegan: function (touch, event) {
                var target = event.getCurrentTarget();
                var location = target.convertToNodeSpace(touch.getLocation());
                var targetSize = target.getContentSize();
                var targetRectangle = cc.rect(0, 0, targetSize.width, targetSize.
                    height);

                if (cc.rectContainsPoint(targetRectangle, location)) {
                    context[funcName](target, loop);
                    return true;
                }

                return false;
            }          
        });
        cc.eventManager.addListener(listener, target);
    },

    init: function () {
        var coverPageUrl = this._storyInformation["coverPage"];
        this._baseDir = "";
        if(coverPageUrl.indexOf("/") != -1) {
            var parts = coverPageUrl.split("/");
            if(parts != undefined && parts.length > 0) {
                this._baseDir = parts[0];
            }
        }
        
        this._constructedScene = ccs.load(xc.path + coverPageUrl, xc.path);
        // this._constructedScene.node.retain();
        // this._constructedScene.action.retain();
        
        if (this._constructedScene.node) {
            this._constructedScene.node.setPosition(cc.director.getWinSize().width/2, cc.director.getWinSize().height/2);
            this._constructedScene.node.setAnchorPoint(cc.p(0.5,0.5));            
            this.addChild(this._constructedScene.node,0);
        }        
        
        this.setUpScene();        
    },

    setUpScene: function () {
        if (this._constructedScene.node) {
            this._constructedScene.action._referenceToContext = this;
            this._constructedScene.action.setFrameEventCallFunc(this.enterFrameEvent);
            this._constructedScene.action.gotoFrameAndPause(0);

            var isAutoConfigued = cc.sys.localStorage.getItem("autoSoundEnabled");            
            if(isAutoConfigued == null) {
                cc.sys.localStorage.setItem("autoSoundEnabled", "true");
            }            
            this.showText();                     
        }
    },


    onExit: function() {        
        this._super();
        var that = this;
        cc.audioEngine.stopAllEffects();
        cc.audioEngine.stopMusic(true);

        that._resources.forEach(function(url) {                
            if(url.endsWith(".json")) {
                cc.log('cleaning url:' + url);
                cc.loader.release(url);
                delete cc.loader[url];
                
            };   

            if(url.endsWith(".ogg")) {
                cc.log('cleaning url:' + url);
                cc.audioEngine.unloadEffect(url);
            }       

            if(url.endsWith(".plist")) {
                cc.log('cleaning url:' + url);
                cc.spriteFrameCache.removeSpriteFramesFromFile(url);
                cc.loader.release(url);
                delete cc.loader[url];                        
            };   

            if(url.endsWith(".png")) {
                cc.log("removing image: " + url);
                cc.textureCache.removeTextureForKey(url);
                cc.loader.release(url);
                delete cc.loader[url]
            }                                                
        });       
        that._resources = [];
    },

    enterFrameEvent: function(event) {
        cc.log('enterFrameEvent' + event.getEvent());    
        var langDir = goa.TextGenerator.getInstance().getLang();
        var eventData = event.getEvent();
        var page = this._referenceToContext._storyInformation["pages"][this._referenceToContext._pageIndex];
        if(page) {
            var soundFile = eventData;
            if(soundFile != undefined) {
                var soundFile = xc.path + this._referenceToContext._baseDir + "/sounds/" + soundFile + ".ogg";
                if(cc.sys.isNative) {
                    var fileExists = jsb.fileUtils.isFileExist(soundFile);
                    if(fileExists) {
                        cc.loader.load(soundFile, function(err, data) {
                            if(!err) {
                                that._resources.push(soundFile);
                                cc.audioEngine.playEffect(soundFile, false);
                            }
                        }); 
                    }
                }
            }            
        }        
    },
    
    sceneTouched: function (target) {
        //load content
        if(this._storyInformation != undefined && this._storyInformation.hasOwnProperty("pages") && this._storyInformation["pages"] != undefined && this._storyInformation["pages"].length > 0) {
            cc.log('loading story:' + this._storyInformation["pages"][0]);
            xc.SceenTransitionScene.load(0, this._storyInformation, xc.SceenTransitionLayer);
        }
    },

    processText: function(sender, type) {
        this.sceneTouched();
    },

    soundText: function(dt) {
        var that = this;
        var langDir = goa.TextGenerator.getInstance().getLang();
        var soundFile = "res/story/" + langDir + "/" + this._baseDir + "/" + this._baseDir + "_0.ogg";
        if(cc.sys.isNative) {
            var fileExists = jsb.fileUtils.isFileExist(soundFile);
            if(fileExists) {
                if(that.soundEnabled) {
                        cc.log('playing sound file' + soundFile);
                        cc.audioEngine.playMusic(soundFile, false);
                } else {
                    cc.audioEngine.pauseMusic();
                }                        
                
                // cc.loader.load(soundFile, function(err, data) {
                //     if(!err) {
                //         that._resources.push(soundFile);
                        
                //     }
                // }); 
            }
        } else {
            cc.loader.load(soundFile, function(err, data) {
                if(!err) {
                    that._resources.push(soundFile);
                    var soundId;
                    if(that.soundEnabled) {
                        cc.audioEngine.playMusic(soundFile, false);
                    } else {
                        cc.audioEngine.pauseMusic(that._soundId);
                    }
                }
            }); 
        }   
    },

    processAudio: function(soundEnabled) {
        var that = this;
        that.soundEnabled = soundEnabled;
        that.scheduleOnce(this.soundText,1.0);
      
    },    

    showText: function() {      
        //load text file based on Current Story Id and Page index
        this._isTextShown = true;
        var langDir = goa.TextGenerator.getInstance().getLang();
        cc.log("langDir:" + langDir);
        var storyText = "";
        var that = this;
        var textFileUrl =  "res/story" + "/" + langDir + "/" + this._baseDir + ".json";
        cc.log('textFileUrl:' + textFileUrl);
        if(cc.sys.isNative) {
            var fileExists = jsb.fileUtils.isFileExist(textFileUrl);
            if(fileExists) {                
                cc.loader.loadJson(textFileUrl, function(err, json) {            
                    if(!err && json != null && json != undefined) {
                        storyText = json[0];
                        cc.log('story text received:' + storyText);
                        that.parent.addChild(new xc.BubbleSpeech(xc.StoryCoverPageLayer.res.textBubble_json, cc.director.getWinSize().width, cc.director.getWinSize().height, cc.p(385, 250), storyText, that.processText, that.processAudio, that, xc.storyCoverPageFontSize, true));
                    } else {
                        that.sceneTouched();
                    }                             
                });           
            } else {
                that.sceneTouched();
            }
        } else {

            cc.loader.loadJson(textFileUrl, function(err, json) {            
                if(!err && json != null && json != undefined) {
                    storyText = json[0];
                    cc.log('story text received:' + storyText);
                    that.parent.addChild(new xc.BubbleSpeech(xc.StoryCoverPageLayer.res.textBubble_json, cc.director.getWinSize().width, cc.director.getWinSize().height, cc.p(385, 250), storyText, that.processText, that.processAudio, that, xc.storyCoverPageFontSize, true));
                }                                
            });                            
        }        
    }    
});

xc.StoryCoverPageScene = cc.Scene.extend({
    layerClass: null,
    _menuContext: null,
    ctor: function (pageIndex, storyInformation, t_resources, layer) {
        this._super();
        this.layerClass = layer;
        this._sceneLayer = new this.layerClass(pageIndex, storyInformation, t_resources);
        this.addChild(this._sceneLayer);
        this._sceneLayer.init();
                
        if (cc.sys.isNative) {
            this._menuContext = goa.MenuContext.create(this._sceneLayer, "Show Stories");
            this.addChild(this._menuContext);
            this._menuContext.setVisible(true);
        }                        
    }
});


xc.StoryCoverPageScene.load = function(pageIndex, storyInformation, layer, enableTransition) {
    var that = this;
    var t_resources = [];
    //also push json
    var currentStoryJSON = null;
    if(storyInformation != null) {
        xc.currentStoryId = storyInformation["storyId"];
        
        var coverPageJSON = storyInformation["coverPage"];
        var storyResources = storyInformation["resources"];

        if(coverPageJSON != null) {

            if(coverPageJSON) {
                t_resources.push(xc.path + coverPageJSON);
            }

            if(storyResources != undefined) {
                storyResources.forEach(function(e) {
                    t_resources.push(xc.path + e);
                });
            }

            for (var i in layer.res) {
                cc.log('preloading:' + layer.res[i]);
                t_resources.push(layer.res[i]);
            }
            
            cc.LoaderScene.preload(t_resources, function () {
                cc.spriteFrameCache.addSpriteFrames(xc.StoryCoverPageLayer.res.template_plist);
                cc.spriteFrameCache.addSpriteFrames(xc.StoryCoverPageLayer.res.template_01_plist);
                cc.spriteFrameCache.addSpriteFrames(xc.StoryCoverPageLayer.res.template_02_plist);
                
                var scene = new xc.StoryCoverPageScene(pageIndex, storyInformation, t_resources, layer);
                scene.layerClass = layer;            
                if(enableTransition) {
                    cc.director.runScene(new cc.TransitionFade(2.0, scene, true));
                }  else {
                    cc.director.runScene(scene);
                }              
            }, this);
            
        }
    }

}

xc.StoryCoverPageLayer.res = {
        play_png: xc.path + "wikitaki/play.png",
        textBubble_json: xc.path + "template/bubble_tem_01.json",
        template_plist: xc.path + "template/template.plist",
        template_png: xc.path + "template/template.png",
        template_01_png: xc.path + "template/template_01/template_01.png",
        template_01_plist: xc.path + "template/template_01/template_01.plist",
        template_02_png: xc.path + "template/template_02/template_02.png",
        template_02_plist: xc.path + "template/template_02/template_02.plist"    
};