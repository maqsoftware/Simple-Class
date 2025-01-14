/// <reference path="cocos2d-typescript-definitions-master/cocos2d/cocos2d-lib.d.ts" />
var xc = xc || {};
xc.PLAY_KEY = xc.path + "wikitaki/xcPlayStory.json";
xc.PLAY_RECORD_LAYER_INIT = false;

xc.PlayRecordingLayer = cc.LayerColor.extend({
    _defaultPageIndex: 0,
    _contentPanel: null,
    _pageConfigPanel: null,
    _objectConfigPanel: null,
    _contentPanelWidth: null,
    _configPanelWidth: null,
    _configPanelHeight: null,
    ctor: function () {
        this._super(xc.PRIMARY_COLOR);
        this._name = "PlayLayer";
        this._controlPanel = null;
        this._contentPanelWidth = cc.director.getWinSize().width; //assuming landscape
        this._contentPanelHeight = cc.director.getWinSize().height; //assuming landscape
        this._configPanelWidth = (cc.director.getWinSize().width - this._contentPanelHeight) / 2;
        this._configPanelHeight = cc.director.getWinSize().height;
        return true;
    },
    init: function () {
        //construct UI
        //create scene with first page
        this._contentPanel = new xc.PlayContentPanel(this._contentPanelWidth, this._contentPanelHeight, cc.p(this._configPanelWidth, 0));
        this.addChild(this._contentPanel);
        this._pageConfigPanel = new xc.BaseConfigPanel(this._configPanelWidth, this._configPanelHeight, cc.p(0, 0), xc.storyPlayConfigurationObject.editDefault, this._contentPanel);
        this.addChild(this._pageConfigPanel);

        this._blankPageConfigPanel = new xc.BaseConfigPanel(this._configPanelWidth, this._configPanelHeight, cc.p(this._contentPanelWidth - this._configPanelWidth, 0), [], this._contentPanel);
        this.addChild(this._blankPageConfigPanel);
        
        this._pageConfigPanel.setVisible(true);
        this.playRecordedScene();

    },

    buttonPressed: function(selectedItem) {
        var selectedConfig = this._configuration.addObjects[selectedItem._selectedIndex];
        if (selectedConfig != null && selectedConfig.name === "back") {
            this._contentPanel.backPressed();  
        }          

    },

    playEnded: function () {
        //create delay action
        var delayAction = new cc.DelayTime(2);
        var createWebViewAction = new cc.CallFunc(this.referenceToContext.createWebView, this.referenceToContext);
        var playEndSequence = cc.Sequence.create(delayAction, createWebViewAction);
        this.referenceToContext.runAction(playEndSequence);
        
    },
    
    createWebView: function() {
        var that = this;
        if (xc.story.items[xc.pageIndex].sceneText != null && xc.story.items[xc.pageIndex].sceneText !== "undefined" 
        && xc.story.items[xc.pageIndex].sceneText.length > 0) {
            this.addChild(new xc.TextCreatePanel(xc.PlayRecordingLayer.res.textBubble_json, cc.director.getWinSize().width, cc.director.getWinSize().height, cc.p(385, 250), xc.story.items[xc.pageIndex].sceneText, that.processText, null, that));
        }     
        this._pageConfigPanel.setVisible(true);           
    },
    
    playRecordedScene: function () {      
        if (this._contentPanel._constructedScene.node && this._contentPanel._constructedScene.action.getDuration() > 0) {            
            this._contentPanel._constructedScene.action.referenceToContext = this;
            this._contentPanel._constructedScene.action.setLastFrameCallFunc(this.playEnded);
            this._contentPanel._constructedScene.action.gotoFrameAndPause(0);
            
            this._contentPanel._constructedScene.node.runAction(this._contentPanel._constructedScene.action);
            this._contentPanel._constructedScene.action.gotoFrameAndPlay(0, this._contentPanel._constructedScene.action.getDuration(), 0, false);

        } else {
            this.referenceToContext = this;
            this.playEnded();
        }
    }
});

xc.PlayRecordingScene = cc.Scene.extend({
    layerClass: null,
    ctor: function (layer) {
        this._super();
        this.layerClass = layer;
        var rlayer = new this.layerClass();
        this.addChild(rlayer);
        rlayer.init();        
    },
});

xc.PlayRecordingScene.load = function(layer) {
    var t_resources = [];
    for (var i in layer.res) {
        cc.log('preloading:' + layer.res[i]);
        t_resources.push(layer.res[i]);
    }

    cc.spriteFrameCache.addSpriteFrames(xc.PlayRecordingLayer.res.thumbnails_plist);
    cc.spriteFrameCache.addSpriteFrames(xc.PlayRecordingLayer.res.record_animation_plist);
    cc.LoaderScene.preload(t_resources, function () {                    
    var scene = new xc.PlayRecordingScene(layer);
    scene.layerClass = layer;
    cc.director.runScene(scene);                                      

    }, this);
}

xc.PlayRecordingLayer.res = {
        thumbnails_png: xc.path + "wikitaki/thumbnails.png",
        thumbnails_plist: xc.path + "wikitaki/thumbnails.plist",
        // human_skeleton_png: xc.path + "wikitaki/human_skeleton.png",
        // human_skeleton_plist: xc.path + "wikitaki/human_skeleton.plist",
        animalskeleton_png: xc.path + "wikitaki/animalskeleton.png",
        animalskeleton_plist: xc.path + "wikitaki/animalskeleton.plist",
        animalskeleton_json: xc.path + "wikitaki/animalskeleton.json",
        birdskeleton_png: xc.path + "wikitaki/birdskeleton.png",
        birdskeleton_plist: xc.path + "wikitaki/birdskeleton.plist",
        birdskeleton_json: xc.path + "wikitaki/birdskeleton.json",
        HelloWorld_png: xc.path + "wikitaki/HelloWorld.png",
        // human_skeleton_json: xc.path + "wikitaki/human_skeleton.json",
        play_png: xc.path + "wikitaki/play.png",
        record_animation_png: xc.path + "wikitaki/recording.png",
        record_animation_plist: xc.path + "wikitaki/recording.plist",
        Config_json: xc.path + "wikitaki/misc/storyConfig.json",
        EditPlayConfig_json: xc.path + "wikitaki/misc/playConfig.json",
        OnlyStoryPlayConfig_json: xc.path + "wikitaki/misc/onlyPlayConfig.json",
        human_skeleton_json: xc.path + "animation/human_skeleton.json",
        animation_skeleton_png: xc.path + "animation/animation.png",
        animation_skeleton_plist: xc.path + "animation/animation.plist",
        animationa_skeleton_png: xc.path + "animation/animationa/animationa.png",
        animationa_skeleton_plist: xc.path + "animation/animationa/animationa.plist",
        animationb_skeleton_png: xc.path + "animation/animationb/animationb.png",
        animationb_skeleton_plist: xc.path + "animation/animationb/animationb.plist",
        animationc_skeleton_png: xc.path + "animation/animationc/animationc.png",
        animationc_skeleton_plist: xc.path + "animation/animationc/animationc.plist",
        textBubble_json: xc.path + "template/bubble_tem.json"
};