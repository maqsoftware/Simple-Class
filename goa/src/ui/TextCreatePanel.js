var xc = xc || {};
xc.TextCreatePanel = cc.Layer.extend({
    ctor: function (jsonNode, width, height, position, existingText, callback, audiocallback, callbackContext, fontSize, enableOutline) {
        this._super(width, height);
        this.callback = callback;
        this.audiocallback = audiocallback;
        this._callbackContext = callbackContext;
        this._text = existingText;
        this._nodeJSON = jsonNode;
        this._fontSize = fontSize;
        this._constructedScene = ccs.load(this._nodeJSON,xc.path);
        this._constructedScene.node.retain();
        this._enableOutline = enableOutline;
        if (this._constructedScene.node) {
            this.addChild(this._constructedScene.node,0);
        }                        

        this._constructedScene.node.setPosition(cc.director.getWinSize().width/2, cc.director.getWinSize().height/2);
        this._constructedScene.node.setAnchorPoint(cc.p(0.5,0.5));


        var closeButton = this._constructedScene.node.getChildByName("Button_1");
        closeButton.setTitleText("");
        closeButton.addTouchEventListener(this.close, this);

        var soundButton = this._constructedScene.node.getChildByName("sound");
        soundButton.setVisible(false);
        
        this._textField = this._constructedScene.node.getChildByName("TextField_2");
        this._textField.setFontName(xc.storyFontName);
        this._textField.setTextColor(xc.storyFontColor);
        if(this._fontSize > 0) {
            this._textField.setFontSize(this._fontSize);    
        } else {
            this._textField.setFontSize(xc.storyFontSize);
        }
        
        this._textField.setAnchorPoint(0.5, 0.5);
        this._textField.setPlaceHolder("");
        this._textField.ignoreContentAdaptWithSize(false);
        this._textField.setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT);
        this._textField.setTextVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER);
        if (!this._enableOutline && this._text) {
            this._textField.setString(this._text);
        }
        this._textField.setTouchEnabled(true);

        if(this._enableOutline) {
            this._label = new ccui.Text(this._text, "Arial", 100);
            if(this._fontSize > 0) {
                this._label.setFontSize(this._fontSize);    
            } else {
                this._label.setFontSize(xc.storyFontSize);
            }        
            this._label.enableOutline(cc.color.RED, 10);
            this._label.color = cc.color.WHITE;
            this._label.setPosition(this._textField.getPosition());
            this.addChild(this._label);
        }
    },

    close: function (sender, type) {
        switch (type) {
            case ccui.Widget.TOUCH_ENDED:
                var contents = this._textField.getString();
                this._textField.cleanup();
                this.parent.removeChild(this, true);
                if (this.callback != null && this._callbackContext != null) {
                    this.callback.call(this._callbackContext, contents);
                }
                break;
        }
    }
});
