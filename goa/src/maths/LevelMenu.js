/// <reference path="../cocos2d-typescript-definitions-master/cocos2d/cocos2d-lib.d.ts" />

var xc = xc || {}

xc.LevelMenuLayer = cc.Node.extend({
  _listener: null,
  _touchPoint: null,
  _clazz: null,
  _span: null,
  _numLevels: null,
  _gameName: null,
  _isJSGame: false,
  _parallax: null,
  _scrollView: null,
  ctor: function(args) {
    this._super()
    var gameConfig = args[0]
    this._span = parseInt(gameConfig.span)
    this._numLevels = parseInt(gameConfig.numLevels)
    this._clazz = xc[gameConfig.pureJS]
    this._gameName = gameConfig.name
    this._isJSGame = gameConfig.isJSGame
    cc.spriteFrameCache.addSpriteFrames(xc.LevelMenuLayer.res.level_plist)
    if(gameConfig.menuPlist) {
      cc.spriteFrameCache.addSpriteFrames(xc.path + gameConfig.menuPlist)
    }
    this._parallax = new cc.ParallaxNode()
    this._parallax.setContentSize(cc.size(cc.director.getVisibleSize().width * this._span, cc.director.getVisibleSize().height))
    this.addChild(this._parallax)
    if(gameConfig.backgroundJson) {
        var pLayer = ccs.load(xc.path + gameConfig.backgroundJson, xc.path)
        this._parallax.addChild(pLayer.node, -4, cc.p(0.2, 0.2), cc.p(0, 0))
    }
    if(gameConfig.maingroundJson) {
        var pLayer = ccs.load(xc.path + gameConfig.maingroundJson, xc.path)
        this._parallax.addChild(pLayer.node, -3, cc.p(0.4, 0.4), cc.p(0, 0))        
    }
    if(gameConfig.foregroundJson) {
        var pLayer = ccs.load(xc.path + gameConfig.foregroundJson, xc.path)
        this._parallax.addChild(pLayer.node, -2, cc.p(0.6, 0.6), cc.p(0, 0))
    }
    if(gameConfig.frontgroundJson) {
        var pLayer = ccs.load(xc.path + gameConfig.frontgroundJson, xc.path)
        this._parallax.addChild(pLayer.node, -1, cc.p(0.8, 0.8), cc.p(0, 0))
    }
    this._scrollView = new xc.ScrollView(gameConfig)
    this.addChild(this._scrollView)
    this._scrollView.addEventListener(this.scrolled)
  },
  scrolled: function(target, event) {
    if(target.getParent() && target.getParent()._parallax) {
      target.getParent()._parallax.setPosition(target.getInnerContainerPosition())
    }
    
  }
})


xc.ScrollView = ccui.ScrollView.extend({
  _clazz: null,
  _span: null,
  _numLevels: null,
  _gameName: null,
  _isJSGame: false,
  _initPos: null,
  ctor: function(gameConfig) {
    // this._super(cc.size(cc.director.getVisibleSize().width * this._span, cc.director.getVisibleSize().height), new cc.ParallaxNode())
    this._super();
    this._span = parseInt(gameConfig.span)
    this._numLevels = parseInt(gameConfig.numLevels)
    this._clazz = xc[gameConfig.pureJS]
    this._gameName = gameConfig.name
    this._isJSGame = gameConfig.isJSGame    
    this.setContentSize(cc.director.getVisibleSize())
    this.setDirection(ccui.ScrollView.DIR_HORIZONTAL)
    this.setInnerContainerSize(cc.size(cc.director.getVisibleSize().width * this._span, cc.director.getVisibleSize().height))
    // this.setViewSize(cc.director.getVisibleSize())
    // this.setBounceable(false)
    // var this._parallax = this.getContainer()
    var gameProgress = JSON.parse(cc.sys.localStorage.getItem(this._gameName + '.level')) || [0]
    var gap = cc.director.getVisibleSize().width * this._span / (this._numLevels + 1)
    var goalTolerance = cc.director.getVisibleSize().height / 10
    var goal = getRandomArbitrary(goalTolerance, cc.director.getVisibleSize().height - goalTolerance)
    var start = goalTolerance
    var prevPos = null
    var numDots = 10
    for(var i = 1; i <= this._numLevels; i++) {
      var levelStatus = parseInt(gameProgress[i])
      var but
      start = getRandomArbitrary(start, goal)
      if(Math.abs(goal - start) < goalTolerance) {
        goal = getRandomArbitrary(goalTolerance, cc.director.getVisibleSize().height - goalTolerance)
      }
      var newPos = cc.p(gap * i, start)
      if(i == gameProgress.length) {
        but = new ccui.Button("levelstep/present.png", "levelstep/present_pressed.png", "levelstep/present.png", ccui.Widget.PLIST_TEXTURE)
        // but.setColor(cc.color(256, 128, 0))
        var label = new cc.LabelTTF(i.toString(), "Arial", 128, null, cc.TEXT_ALIGNMENT_CENTER, cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        label.setPosition(but.getContentSize().width / 2, but.getContentSize().height / 2)
        but.addChild(label)        
        var mark = new cc.Sprite("#levelstep/mark.png")
        mark.setPosition(but.getContentSize().width / 2, but.getContentSize().height * 1.5)
        but.addChild(mark)
        but.addTouchEventListener(this.itemSelected, this)
        this._initPos = cc.p(-Math.max(0, Math.min(cc.director.getVisibleSize().width * ( this._span - 1), newPos.x - cc.director.getVisibleSize().width / 2)), 0)
      } else if(i > gameProgress.length && !xc.LevelMenuLayer.debug) {
        // but.setColor(cc.color(128, 128, 128))
        but = new ccui.Button("levelstep/disabled.png", "levelstep/disabled.png", "levelstep/disabled.png", ccui.Widget.PLIST_TEXTURE)        
        var label = new cc.LabelTTF(i.toString(), "Arial", 128, null, cc.TEXT_ALIGNMENT_CENTER, cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        label.setPosition(but.getContentSize().width / 2, but.getContentSize().height / 2)
        but.addChild(label)        
      } else {
        but = new ccui.Button("levelstep/done.png", "levelstep/done_pressed.png", "levelstep/done.png", ccui.Widget.PLIST_TEXTURE)                
        var label = new cc.LabelTTF(i.toString(), "Arial", 128, null, cc.TEXT_ALIGNMENT_CENTER, cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        label.setPosition(but.getContentSize().width / 2, but.getContentSize().height / 2)
        but.addChild(label)        
        // but.setColor(cc.color(128, 0, 0))
        var star = new cc.Sprite(levelStatus >= 1 ? "#levelstep/star.png" : "#levelstep/star_empty.png")
        star.setScale(0.8)
        star.setPosition(but.getContentSize().width / 4, but.getContentSize().height * 3 / 4 - 10)
        but.addChild(star)
        star = new cc.Sprite(levelStatus >= 2 ? "#levelstep/star.png" : "#levelstep/star_empty.png")
        star.setScale(0.8)
        star.setPosition(but.getContentSize().width / 2, but.getContentSize().height * 7 / 8 - 10)
        but.addChild(star)
        star = new cc.Sprite(levelStatus >= 3 ? "#levelstep/star.png" : "#levelstep/star_empty.png")
        star.setScale(0.8)
        star.setPosition(but.getContentSize().width * 3 / 4 - 10, but.getContentSize().height * 3 / 4 - 10)
        but.addChild(star)
        but.addTouchEventListener(this.itemSelected, this)
      }
      // but.setScale(0.25)
      but.setPosition(newPos)
      if(prevPos) {
        var line = new cc.DrawNode()
        line.drawSegment(prevPos, newPos, 20, cc.color("#FFA364"))
        this.addChild(line, 1)
      }
      this.addChild(but, 2)
      prevPos = newPos
    }
  },
  onEnter: function() {
    ccui.ScrollView.prototype.onEnter.call(this);
    this.setInnerContainerPosition(this._initPos)
  },
  itemSelected: function (sender, type) {
    switch (type) {
      case ccui.Widget.TOUCH_ENDED:
        var level = parseInt(sender.getChildren()[0].getString())
        cc.log(this._gameName + '.currentLevel')
        cc.sys.localStorage.setItem(this._gameName + '.currentLevel', level.toString())
        cc.log(cc.sys.localStorage.getItem(this._gameName + '.currentLevel'))
        // if(this._isJSGame) {
        //   xc.GameScene.load(this._clazz, this._gameName, level)
        // } else {
          goa.MenuContext.launchGameFromJS(this._gameName)
        // }
        break;
    }
  },
  levelCompleted: function(level, numStars) {
    var gameProgress = JSON.parse(cc.sys.localStorage.getItem(this._gameName + '.level')) || [0]
    var currentStars = gameProgress[level] || 0
    gameProgress[level] = Math.max(numStars, currentStars)
    cc.sys.localStorage.setItem(this._gameName + '.level', JSON.stringify(gameProgress))
  }
})

xc.LevelMenuLayer.debug = true

xc.LevelMenuLayer.res = {
  level_plist: xc.path + "levelstep/levelstep.plist",
  level_png: xc.path + "levelstep/levelstep.png",
  gray_star_png: xc.path + "help/gray_star.png",
  white_star_png: xc.path + "help/white_star.png"  
}

// Returns a random number between min (inclusive) and max (exclusive)
function getRandomArbitrary(min, max) {
  return Math.random() * (max - min) + min;
}