var xc = xc || {};

xc.ParseUtil = xc.ParseUtil || {};

xc.ParseUtil.copyUserAddedDataToScene = function (newScene) {
    var oldScene = xc.story.items[xc.pageIndex].scene;
    if (oldScene && oldScene.Content && oldScene.Content.Content
        && oldScene.Content.Content.ObjectData
        && oldScene.Content.Content.ObjectData.Children) {
        oldScene.Content.Content.ObjectData.Children.forEach(function (element) {
            if (element.UserData && element.UserData.userAdded && newScene && newScene.Content && newScene.Content.Content
                && newScene.Content.Content.ObjectData && newScene.Content.Content.ObjectData.Children) {
                newScene.Content.Content.ObjectData.Children.push(element);
            }
        }, this);
    }
}

xc.ParseUtil.saveScene = function (newScene) {
    if (xc.story && xc.story.items != null) {
        xc.story.items[xc.pageIndex].scene = newScene;
        cc.log('lenght saveScene:' + newScene.Content.Content.ObjectData.Children.length);
    }
}

xc.ParseUtil.saveObjectToStoredScene = function (jsonObject) {
    if (xc.story && xc.story.items != null) {
        var replace = false;
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {
            if (children[index].ActionTag == jsonObject.ActionTag) {
                children.splice(index, 1, jsonObject);
                replace = true;
                break;
            }
        }
        if (!replace) {
            //jsonObject.userAdded = true;
            children.push(jsonObject);
            cc.log('adding object with Action tag:' + jsonObject.ActionTag);
        }
        xc.ParseUtil.saveScene(xc.story.items[xc.pageIndex].scene);
    }
}

xc.ParseUtil.updateScaleRotationAndPositionObjectFromStoredScene = function (target) {
    if (xc.story && xc.story.items != null) {
        cc.log('target x:' + target.x);
        cc.log('target y:' + target.y);
        cc.log('target ActionTag:' + target.ActionTag);
        cc.log('target _actionTag:' + target._actionTag);   

        if(!target.ActionTag && target._actionTag) {
            target.ActionTag = target._actionTag;
        } else if(target.ActionTag && !target._actionTag) {
            target._actionTag = target.ActionTag;
        }

        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {            
            if (target.ActionTag) {
                if (children[index].ActionTag == target.ActionTag) {
                    children[index].Scale.ScaleX = target.scaleX;
                    children[index].Scale.ScaleY = target.scaleY;

                    children[index].Position.X = target.x;
                    children[index].Position.Y = target.y;

                    children[index].RotationSkewX = target.rotationX;
                    children[index].RotationSkewY = target.rotationY;
                    break;
                }
            }
        }

        xc.ParseUtil.saveScene(xc.story.items[xc.pageIndex].scene);
    }
}

xc.ParseUtil.updateFlipObjectFromStoredScene = function (tag, scaleX) {
    if (xc.story && xc.story.items != null) {
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {
            if (children[index].ActionTag == tag) {
                children[index].Scale.ScaleX = scaleX;
                break;
            }
        }
        xc.ParseUtil.saveScene(xc.story.items[xc.pageIndex].scene);
    }
}


xc.ParseUtil.removeObjectFromStoredScene = function (tag) {
    cc.log('removeObjectFromStoredScene with tag' + tag);
    if (xc.story && xc.story.items != null) {
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        cc.log('lenght before:' + xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children.length);
        for (var index = 0; index < children.length; index++) {
            cc.log('action tag in removeObjectFromStoredScene 111' + children[index].ActionTag);
            if (children[index].ActionTag == tag) {
                children.splice(index, 1);
                break;
            }
        }
        cc.log('lenght after:' + xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children.length);
        xc.ParseUtil.saveScene(xc.story.items[xc.pageIndex].scene);
    }
}

xc.ParseUtil.getUserData = function (tag, dataKey) {
    var result = null;
    if (xc.story && xc.story.items != null && xc.story.items[xc.pageIndex].scene.Content) {
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {
            if (children[index].ActionTag == tag) {
                var object = children[index];
                var obj = JSON.parse(object.UserData);
                result = obj[dataKey];
                break;
            }
        }
        return result;
    }
}


xc.ParseUtil.getUserDataByActionTag = function (tag) {
    var result = null;
    if (xc.story && xc.story.items != null && xc.story.items[xc.pageIndex].scene.Content) {
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {
            if (children[index].ActionTag == tag) {
                var object = children[index];
                var obj = JSON.parse(object.UserData);
                result = obj;
                break;
            }
        }
        return result;
    }
}


xc.ParseUtil.updateUserData = function (tag, dataKey, dataValue) {
    if (xc.story && xc.story.items != null && xc.story.items[xc.pageIndex].scene.Content) {
        var children = xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children;
        for (var index = 0; index < children.length; index++) {
            if (children[index].ActionTag == tag) {
                var object = children[index];                
                var obj = JSON.parse(object.UserData);
                obj[dataKey] = dataValue;
                object.UserData = JSON.stringify(obj);
                break;
            }
        }
        xc.ParseUtil.saveScene(xc.story.items[xc.pageIndex].scene);
    }
}

xc.ParseUtil.saveCharacterToJSON = function (fileToLoad, load, paramActionTag) {
    var resourcePath = fileToLoad.replace(xc.path + "wikitaki/", "");
    var skeletonObject = xc.ParseUtil.constructJSONFromCharacter(load.node, resourcePath, paramActionTag);
    xc.ParseUtil.saveObjectToStoredScene(skeletonObject);
}

xc.ParseUtil.constructJSONFromCharacter = function (skeleton, resourcePath, paramActionTag) {
    var object = Object.create(Object.prototype);
    object.FileData = {};
    object.FileData.Type = "Normal";
    object.FileData.Path = resourcePath;
    object.FileData.Plist = "";

    object.InnerActionSpeed = 1.0;
    object.AnchorPoint = {
        "ScaleX": skeleton.getAnchorPoint().x,
        "ScaleY": skeleton.getAnchorPoint().y
    };

    object.Position = {
        "X": skeleton.getPosition().x,
        "Y": skeleton.getPosition().y
    };

    object.RotationSkewX = skeleton.getRotationX();
    object.RotationSkewY = skeleton.getRotationY();
    object.Scale = {
        "ScaleX": skeleton.getScaleX(),
        "ScaleY": skeleton.getScaleY()
    };
    object.CColor = {
        "R": skeleton.color.r,
        "G": skeleton.color.g,
        "B": skeleton.color.b,
        "A": skeleton.color.a
    };
    object.Tag = skeleton.tag;
    object.Size = {
        "X": skeleton.width,
        "Y": skeleton.height
    };
    object.ActionTag = paramActionTag;
    object.Name = skeleton.getName();
    object.ctype = "ProjectNodeObjectData";

    var existingUserData = {};

    existingUserData.currentAnimationName = skeleton._currentAnimationName;
    existingUserData.resourcePath = resourcePath;
    existingUserData.userAdded = true;
    existingUserData._actionTag = object.ActionTag;
    object.UserData = JSON.stringify(existingUserData);
    skeleton._actionTag = object.ActionTag;
    skeleton.UserData = object.UserData;

    return object;
}

xc.ParseUtil.constructJSONFromCCSprite = function (sprite, filePath) {

    var object = Object.create(Object.prototype);
    object.FlipX = sprite._flippedX;
    object.FlipY = sprite._flippedY;
    object.FileData = {};
    object.FileData.Type = "Normal";
    object.FileData.Path = filePath.replace(xc.path + "wikitaki/", "");        
    object.FileData.Plist = "";
    object.BlendFunc = {
        "Src": sprite.getBlendFunc.src,
        "Dst": sprite.getBlendFunc.dst
    };

    object.AnchorPoint = {
        "ScaleX": sprite.getAnchorPoint().x,
        "ScaleY": sprite.getAnchorPoint().y
    };

    object.Position = {
        "X": sprite.getPosition().x,
        "Y": sprite.getPosition().y
    };

    object.RotationSkewX = sprite.getRotationX();
    object.RotationSkewY = sprite.getRotationY();
    object.Scale = {
        "ScaleX": sprite.getScaleX(),
        "ScaleY": sprite.getScaleY()
    };
    object.CColor = {
        "R": sprite.color.r,
        "G": sprite.color.g,
        "B": sprite.color.b,
        "A": sprite.color.a
    };
    object.IconVisible = false;
    object.Size = {
        "X": sprite.getBoundingBox().width,
        "Y": sprite.getBoundingBox().height
    };
    object.Tag = sprite.tag;
    if (sprite.getName().indexOf("%%") === -1) {
        sprite.setName(sprite.getName() + "%%" + xc.ParseUtil.generateUUID());
    }

    var i = new Date().getTime();
    i = i & 0xffffffff;

    object.ActionTag = i;
    cc.log('Action Tag for image:' + object.ActionTag);


    object.Name = sprite.getName();
    object.ctype = "SpriteObjectData";

    var existingUserData = {};

    existingUserData.userAdded = true;
    object.userData = JSON.stringify(existingUserData);
    sprite.userData = object.UserData;
    return object;
}

xc.ParseUtil.constructJSONFromText = function (panel, resourcePath) {
    //create panel data
    var panelObject = Object.create(Object.prototype);
    panelObject.ClipAble = panel.clippingEnabled;
    panelObject.BackColorAlpha = panel.getBackGroundColorOpacity();

    panelObject.FileData = {};
    panelObject.FileData.Type = "Normal";
    resourcePath = resourcePath.replace("/", "");
    panelObject.FileData.Path = resourcePath;

    panelObject.FileData.Plist = "";

    panelObject.ComboBoxIndex = panel.getBackGroundColorType();
    panelObject.SingleColor = {
        "R": panel.getBackGroundColor().r,
        "G": panel.getBackGroundColor().g
    };

    panelObject.FirstColor = {
        "R": panel.getBackGroundColor().r,
        "G": panel.getBackGroundColor().g
    };
    panelObject.EndColor = {};

    panelObject.ColorVector = {
        "ScaleX": panel.getBackGroundColorVector().x,
        "ScaleY": panel.getBackGroundColorVector().y
    };

    panelObject.Scale9Enable = panel.isBackGroundImageScale9Enabled();
    panelObject.Scale9OriginX = panel.getBackGroundImageCapInsets().x;
    panelObject.Scale9OriginY = panel.getBackGroundImageCapInsets().y;
    panelObject.Scale9Width = panel.getBackGroundImageCapInsets().width;
    panelObject.Scale9Height = panel.getBackGroundImageCapInsets().height;
    panelObject.TouchEnable = panel.touchEnabled;
    panelObject.AnchorPoint = {
        "ScaleX": panel.getAnchorPoint().x,
        "ScaleY": panel.getAnchorPoint().y
    };
    panelObject.Position = {
        "X": panel.getPosition().x,
        "Y": panel.getPosition().y
    };

    panelObject.Scale = {
        "ScaleX": panel.getScaleX(),
        "ScaleY": panel.getScaleY()
    };

    panelObject.CColor = {

    };

    var i = new Date().getTime();
    i = i & 0xffffffff;

    panelObject.Tag = i;
    panelObject.ActionTag = i;

    panelObject.Size = {
        "X": panel.getContentSize().width,
        "Y": panel.getContentSize().height
    };

    panelObject.Name = "xcTextPanel";
    panelObject.ctype = "PanelObjectData";

    var textNode = panel.children[0];
    var textObject = Object.create(Object.prototype);

    textObject.IsCustomSize = !textNode.isIgnoreContentAdaptWithSize();
    textObject.FontSize = textNode.fontSize;

    textObject.LabelText = textNode.getString();
    textObject.OutlineColor = {
        "G": 0,
        "B": 0
    };
    textObject.ShadowColor = {
        "R": 110,
        "G": 110,
        "B": 110
    };
    textObject.ShadowOffsetX = 2.0;
    textObject.ShadowOffsetY = -2.0;

    textObject.AnchorPoint = {
        "ScaleX": textNode.getAnchorPoint().x,
        "ScaleY": textNode.getAnchorPoint().y
    };
    textObject.Position = {
        "X": textNode.getPosition().x,
        "Y": textNode.getPosition().y
    };
    textObject.Scale = {
        "ScaleX": textNode.getScaleX(),
        "ScaleY": textNode.getScaleY()
    };
    textObject.CColor = {
        "R": textNode.getTextColor().r,
        "G": textNode.getTextColor().g,
        "B": textNode.getTextColor().b
    };

    var i = new Date().getTime();
    i = i & 0xffffffff;

    textObject.Tag = i;    
    textObject.ActionTag = i; 
    textObject.Size = {
        "X": textNode.getContentSize().width,
        "Y": textNode.getContentSize().height
    };
    textObject.Name = textNode.name;
    textObject.ctype = "TextObjectData";

    panelObject.Children = [textObject];

    return panelObject;
}

xc.ParseUtil.constructJSONFromTextNode = function (textNode, resourcePath) {
    var object = Object.create(Object.prototype);
    object.FileData = {};
    object.FileData.Type = "Normal";
    object.FileData.Path = resourcePath;
    object.FileData.Plist = "";

    object.AnchorPoint = {
        "ScaleX": textNode.getAnchorPoint().x,
        "ScaleY": textNode.getAnchorPoint().y
    };

    object.Position = {
        "X": textNode.getPosition().x,
        "Y": textNode.getPosition().y
    };

    object.RotationSkewX = textNode.getRotationX();
    object.RotationSkewY = textNode.getRotationY();
    object.Scale = {
        "ScaleX": textNode.getScaleX(),
        "ScaleY": textNode.getScaleY()
    };
    object.CColor = {
        "R": textNode.color.r,
        "G": textNode.color.g,
        "B": textNode.color.b,
        "A": textNode.color.a
    };
    object.tag = textNode.tag;
    object.Size = {
        "X": textNode.width,
        "Y": textNode.height
    };
    var i = new Date().getTime();
    i = i & 0xffffffff;
 
    object.ActionTag = i;
    object.Name = textNode.getName();
    object.ctype = "ProjectNodeObjectData";
    return object;
}

xc.ParseUtil.generateUUID = function () {
    var d = new Date().getTime();
    var uuid = 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
        var r = (d + Math.random() * 16) % 16 | 0;
        d = Math.floor(d / 16);
        return (c == 'x' ? r : (r & 0x3 | 0x8)).toString(16);
    });
    return uuid;
}


xc.ParseUtil.changeSize = function (obj, name, scaleFactor) {
    // if (obj['xcCompressed']) {
    //     return;
    // }
    // if (obj['ctype'] && obj['ctype'] == 'PointFrameData') {
    //     name = obj['ctype'];
    // }
    // for (var key in obj) {
    //     var element = obj[key];
    //     if (name == 'Size' || name == 'Position' || name == 'PointFrameData') {
    //         if (key == 'X' || key == 'Y') {
    //             obj[key] = obj[key] / scaleFactor;
    //         }
    //     }
    //     if (typeof (element) == 'object') {
    //         this.changeSize(element, key, scaleFactor);
    //     }
    // }
}


xc.ParseUtil.disableFavoriteChoiceIfCharacterAlreadyLoadedInPage = function (item, itemConfiguration) {
    if (itemConfiguration && itemConfiguration['uniqueCharacterID'] &&
        xc.story.items[xc.pageIndex].scene.Content && xc.story.items[xc.pageIndex].scene.Content.Content
        && xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData) {
        xc.story.items[xc.pageIndex].scene.Content.Content.ObjectData.Children.forEach(function (child) {
            var uniqueCharacterID = xc.ParseUtil.getUserData(child._actionTag,'uniqueCharacterID')
            if (uniqueCharacterID && uniqueCharacterID == itemConfiguration['uniqueCharacterID']) {
                item.setEnabled(false);
            }
        }, this);
    }
}


xc.ParseUtil.cacheThumbnailForFavorites = function (skeleton) {
    var renderer = new cc.RenderTexture(64 * 4, 64 * 4);
    renderer.begin();
    skeleton.visit();    
    renderer.end();
    renderer.scaleY = -1;
    var sprite = renderer.getSprite();
    var uniqueCharacterID = xc.ParseUtil.getUserData(skeleton._actionTag,'uniqueCharacterID')
    var cacheName = xc.path  + "wikitaki/"+ uniqueCharacterID + '.png';
    //cc.textureCache.cacheImage(cacheName, sprite.texture);
    renderer.cleanup();
}

xc.ParseUtil.removeExistingBoundingBox = function (target, tag) {
    if (!tag) {
        tag = xc.DEFAULT_BOUNDING_BOX_TAG;
    }
    
    if (target) {
        var boundingBoxNode = target.getChildByTag(tag);
        if (boundingBoxNode) {
            boundingBoxNode.removeFromParent(true);
        }
    }    
}

xc.ParseUtil.drawBoundingBox = function (target, tag, color) {
    var box = null;
    //set up defaults for tag and color
    if (!tag) {
        tag = xc.DEFAULT_BOUNDING_BOX_TAG;
    }

    if (!color) {
        color = xc.SECONDARY_COLOR;
    }

    if (target.getName().indexOf("Skeleton") != -1 || target.getName().indexOf("skeleton") != -1) {
        box = target.getBoundingBoxToWorld();
        var dn = new cc.DrawNode();
        dn.clear();
        dn.tag = tag;
        target.addChild(dn);
        if (target.getName().indexOf("birdskeleton") != -1) {
            dn.drawRect(cc.p(-box.width / (Math.abs(target.getScaleX())), -box.height / (2 * Math.abs(target.getScaleX()))), cc.p(box.width / (2 * Math.abs(target.getScaleX())), box.height / Math.abs(target.getScaleX())), null, 3, color);
        } else {
            dn.drawRect(cc.p(-box.width / (2 * Math.abs(target.getScaleX())), 0), cc.p(box.width / (2 * Math.abs(target.getScaleX())), box.height / Math.abs(target.getScaleX())), null, 3, color);
        }
    } else {
        box = target.getBoundingBox();
        var dn = new cc.DrawNode();
        dn.clear();
        dn.tag = tag;
        target.addChild(dn);
        dn.drawRect(cc.p(0, 0), cc.p(box.width / Math.abs(target.getScaleX()), box.height / Math.abs(target.getScaleX())), null, 3, color);
    }

    if (color == xc.SECONDARY_COLOR) {
        xc.currentTouchedNode = target;
    }

}


xc.ParseUtil.deflate = function (xcStory) {
    // var Base64 = { _keyStr: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=", encode: function (e) { var t = ""; var n, r, i, s, o, u, a; var f = 0; e = Base64._utf8_encode(e); while (f < e.length) { n = e.charCodeAt(f++); r = e.charCodeAt(f++); i = e.charCodeAt(f++); s = n >> 2; o = (n & 3) << 4 | r >> 4; u = (r & 15) << 2 | i >> 6; a = i & 63; if (isNaN(r)) { u = a = 64 } else if (isNaN(i)) { a = 64 } t = t + this._keyStr.charAt(s) + this._keyStr.charAt(o) + this._keyStr.charAt(u) + this._keyStr.charAt(a) } return t }, decode: function (e) { var t = ""; var n, r, i; var s, o, u, a; var f = 0; e = e.replace(/[^A-Za-z0-9\+\/\=]/g, ""); while (f < e.length) { s = this._keyStr.indexOf(e.charAt(f++)); o = this._keyStr.indexOf(e.charAt(f++)); u = this._keyStr.indexOf(e.charAt(f++)); a = this._keyStr.indexOf(e.charAt(f++)); n = s << 2 | o >> 4; r = (o & 15) << 4 | u >> 2; i = (u & 3) << 6 | a; t = t + String.fromCharCode(n); if (u != 64) { t = t + String.fromCharCode(r) } if (a != 64) { t = t + String.fromCharCode(i) } } t = Base64._utf8_decode(t); return t }, _utf8_encode: function (e) { e = e.replace(/\r\n/g, "\n"); var t = ""; for (var n = 0; n < e.length; n++) { var r = e.charCodeAt(n); if (r < 128) { t += String.fromCharCode(r) } else if (r > 127 && r < 2048) { t += String.fromCharCode(r >> 6 | 192); t += String.fromCharCode(r & 63 | 128) } else { t += String.fromCharCode(r >> 12 | 224); t += String.fromCharCode(r >> 6 & 63 | 128); t += String.fromCharCode(r & 63 | 128) } } return t }, _utf8_decode: function (e) { var t = ""; var n = 0; var r = c1 = c2 = 0; while (n < e.length) { r = e.charCodeAt(n); if (r < 128) { t += String.fromCharCode(r); n++ } else if (r > 191 && r < 224) { c2 = e.charCodeAt(n + 1); t += String.fromCharCode((r & 31) << 6 | c2 & 63); n += 2 } else { c2 = e.charCodeAt(n + 1); c3 = e.charCodeAt(n + 2); t += String.fromCharCode((r & 15) << 12 | (c2 & 63) << 6 | c3 & 63); n += 3 } } return t } }

    // var compressed = JSONC.pack( xcStory );
    // return compressed;

    // var binaryxcStoryJSON = pako.deflate(chipleStoryJSON, { to: 'string' });
    // var base64endcoedStoryJSONStr = Base64.encode(binaryxcStoryJSON);
    // return base64endcoedStoryJSONStr;

    var chipleStoryJSON = JSON.stringify(xcStory);
    var compressed = LZString.compressToEncodedURIComponent(chipleStoryJSON);
    return compressed;
}

xc.ParseUtil.inflate = function (base64CompressedJSON) {
    // var Base64 = { _keyStr: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=", encode: function (e) { var t = ""; var n, r, i, s, o, u, a; var f = 0; e = Base64._utf8_encode(e); while (f < e.length) { n = e.charCodeAt(f++); r = e.charCodeAt(f++); i = e.charCodeAt(f++); s = n >> 2; o = (n & 3) << 4 | r >> 4; u = (r & 15) << 2 | i >> 6; a = i & 63; if (isNaN(r)) { u = a = 64 } else if (isNaN(i)) { a = 64 } t = t + this._keyStr.charAt(s) + this._keyStr.charAt(o) + this._keyStr.charAt(u) + this._keyStr.charAt(a) } return t }, decode: function (e) { var t = ""; var n, r, i; var s, o, u, a; var f = 0; e = e.replace(/[^A-Za-z0-9\+\/\=]/g, ""); while (f < e.length) { s = this._keyStr.indexOf(e.charAt(f++)); o = this._keyStr.indexOf(e.charAt(f++)); u = this._keyStr.indexOf(e.charAt(f++)); a = this._keyStr.indexOf(e.charAt(f++)); n = s << 2 | o >> 4; r = (o & 15) << 4 | u >> 2; i = (u & 3) << 6 | a; t = t + String.fromCharCode(n); if (u != 64) { t = t + String.fromCharCode(r) } if (a != 64) { t = t + String.fromCharCode(i) } } t = Base64._utf8_decode(t); return t }, _utf8_encode: function (e) { e = e.replace(/\r\n/g, "\n"); var t = ""; for (var n = 0; n < e.length; n++) { var r = e.charCodeAt(n); if (r < 128) { t += String.fromCharCode(r) } else if (r > 127 && r < 2048) { t += String.fromCharCode(r >> 6 | 192); t += String.fromCharCode(r & 63 | 128) } else { t += String.fromCharCode(r >> 12 | 224); t += String.fromCharCode(r >> 6 & 63 | 128); t += String.fromCharCode(r & 63 | 128) } } return t }, _utf8_decode: function (e) { var t = ""; var n = 0; var r = c1 = c2 = 0; while (n < e.length) { r = e.charCodeAt(n); if (r < 128) { t += String.fromCharCode(r); n++ } else if (r > 191 && r < 224) { c2 = e.charCodeAt(n + 1); t += String.fromCharCode((r & 31) << 6 | c2 & 63); n += 2 } else { c2 = e.charCodeAt(n + 1); c3 = e.charCodeAt(n + 2); t += String.fromCharCode((r & 15) << 12 | (c2 & 63) << 6 | c3 & 63); n += 3 } } return t } };

    var base64DecodedxcStoryJSON = LZString.decompressFromEncodedURIComponent(base64CompressedJSON);
    return JSON.parse(base64DecodedxcStoryJSON);

    // var base64DecodedxcStoryJSON = Base64.decode(base64CompressedJSON);
    // var restoredBase64EncodedxcStoryJSON = pako.inflate(base64DecodedxcStoryJSON, { to: 'string' });
    // return JSON.parse(restoredBase64EncodedxcStoryJSON);

    // var json = JSONC.unpack( base64CompressedJSON );
    // return json; 
}