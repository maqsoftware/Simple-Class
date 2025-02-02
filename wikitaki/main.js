/**
 * A brief explanation for "project.json":
 * Here is the content of project.json file, this is the global configuration for your game, you can modify it to customize some behavior.
 * The detail of each field is under it.
 {
    "project_type": "javascript",
    // "project_type" indicate the program language of your project, you can ignore this field

    "debugMode"     : 1,
    // "debugMode" possible values :
    //      0 - No message will be printed.
    //      1 - cc.error, cc.assert, cc.warn, cc.log will print in console.
    //      2 - cc.error, cc.assert, cc.warn will print in console.
    //      3 - cc.error, cc.assert will print in console.
    //      4 - cc.error, cc.assert, cc.warn, cc.log will print on canvas, available only on web.
    //      5 - cc.error, cc.assert, cc.warn will print on canvas, available only on web.
    //      6 - cc.error, cc.assert will print on canvas, available only on web.

    "showFPS"       : true,
    // Left bottom corner fps information will show when "showFPS" equals true, otherwise it will be hide.

    "frameRate"     : 60,
    // "frameRate" set the wanted frame rate for your game, but the real fps depends on your game implementation and the running environment.

    "noCache"       : false,
    // "noCache" set whether your resources will be loaded with a timestamp suffix in the url.
    // In this way, your resources will be force updated even if the browser holds a cache of it.
    // It's very useful for mobile browser debuging.

    "id"            : "gameCanvas",
    // "gameCanvas" sets the id of your canvas element on the web page, it's useful only on web.

    "renderMode"    : 0,
    // "renderMode" sets the renderer type, only useful on web :
    //      0 - Automatically chosen by engine
    //      1 - Forced to use canvas renderer
    //      2 - Forced to use WebGL renderer, but this will be ignored on mobile browsers

    "engineDir"     : "frameworks/cocos2d-html5/",
    // In debug mode, if you use the whole engine to develop your game, you should specify its relative path with "engineDir",
    // but if you are using a single engine file, you can ignore it.

    "modules"       : ["cocos2d"],
    // "modules" defines which modules you will need in your game, it's useful only on web,
    // using this can greatly reduce your game's resource size, and the cocos console tool can package your game with only the modules you set.
    // For details about modules definitions, you can refer to "../../frameworks/cocos2d-html5/modulesConfig.json".

    "jsList"        : [
    ]
    // "jsList" sets the list of js files in your game.
 }
 *
 */


var chimple = chimple || {};
chimple.RESOURCE_DESIGN_HEIGHT = 1800;
chimple.DEVICE_WIDTH = 640;
chimple.DEVICE_HEIGHT = 450;
chimple.HAND_GEAR_LEFT = "hand_gear_left";
chimple.image = {};
chimple.isHTML5 = function () {
    return typeof document !== "undefined";
};

(function () {
    chimple.DEFAULT_MODE = "PLAY";
    chimple.EDIT_MODE = "edit";
    chimple.PLAY_MODE = "play";
    chimple.customSprites = [];

    if (!cc.sys.isNative) {
        var d = document;

        var c = {
            "project_type": "javascript",

            "debugMode": 1,
            "showFPS": false,
            "frameRate": 60,
            "noCache": false,
            "id": "gameCanvas",
            "renderMode": 0,
            "engineDir": "frameworks/cocos2d-html5",

            "modules": ["cocos2d", "cocostudio"],

            "jsList": [
                "src/resource.js",
                "src/app.js",
                "src/ui/PageConfigPanel.js",
                "src/ui/ObjectConfigPanel.js",
                "src/ui/AbstractContentPanel.js",
                "src/ui/ContentPanel.js",
                "src/ui/PlayContentPanel.js",
                "src/ui/BaseConfigPanel.js",
                "src/ui/ButtonPanel.js",
                "src/ui/ScrollableButtonPanel.js",
                "src/ui/TabBarPanel.js",
                "src/ui/TabBar.js",
                "src/ui/TabPanel.js",
                "src/ui/PreviewPanel.js",
                "src/ui/TextCreatePanel.js",
                "src/util/CharacterUtil.js",
                "src/util/ParseUtil.js",
                "src/TextEditScene.js",
                "src/PlayRecordingScene.js",
                "src/SpriteTouchHandler.js",
                "src/SkeletonTouchHandler.js",
                "src/TextTouchHandler.js",
                "src/EditStoryScene.js",
                "src/play.js",
                "src/Preload.js",
                "src/pako.js",
                "src/ui/TextReadPanel.js",
                "src/lz-string.js",
                "src/jsonc.js"
            ]

        };
        retrieveMode = function () {

            var query_string = {};
            var query = window.location.search.substring(1);
            var vars = query.split("&");
            for (var i = 0; i < vars.length; i++) {
                var pair = vars[i].split("=");
                // If first entry with this name
                if (typeof query_string[pair[0]] === "undefined") {
                    query_string[pair[0]] = decodeURIComponent(pair[1]);
                    // If second entry with this name
                } else if (typeof query_string[pair[0]] === "string") {
                    var arr = [query_string[pair[0]], decodeURIComponent(pair[1])];
                    query_string[pair[0]] = arr;
                    // If third or later entry with this name
                } else {
                    query_string[pair[0]].push(decodeURIComponent(pair[1]));
                }
            }
            if (query_string != null && query_string != undefined) {
                var mode = query_string['mode'];
                var fesid = query_string['fesid'] || window.recipeId;
                console.log('mode:' + mode);
                if (!fesid) {
                    mode = chimple.EDIT_MODE;
                } else {
                    if (mode && mode.indexOf(chimple.EDIT_MODE) != -1) {
                        mode = chimple.EDIT_MODE;
                    } else {
                        mode = chimple.PLAY_MODE;
                    }
                }
                chimple.mode = mode;
            }
        };

        document.ccConfig = c;
        this.retrieveMode();
    } else {
        chimple.mode = chimple.EDIT_MODE; 
    }




    cc.game.onStart = function () {
        if (!cc.sys.isNative && document.getElementById("cocosLoading")) //If referenced loading.js, please remove it
            document.body.removeChild(document.getElementById("cocosLoading"));

        // Pass true to enable retina display, on Android disabled by default to improve performance
        cc.view.enableRetina(cc.sys.os === cc.sys.OS_IOS ? true : false);
        // Adjust viewport meta
        cc.view.adjustViewPort(true);
        // Setup the resolution policy and design resolution size
        cc.view.setDesignResolutionSize(chimple.DEVICE_WIDTH, chimple.DEVICE_HEIGHT, cc.ResolutionPolicy.SHOW_ALL);
        // Instead of set design resolution, you can also set the real pixel resolution size
        // Uncomment the following line and delete the previous line.
        // cc.view.setRealPixelResolution(960, 640, cc.ResolutionPolicy.SHOW_ALL);
        // The game will be resized when browser size change
        // cc.view.resizeWithBrowserSize(true);
        chimple.designScaleFactor = chimple.RESOURCE_DESIGN_HEIGHT / chimple.DEVICE_HEIGHT;

        if (!cc.sys.isNative) {
            //load resources
            // Preloader.preload(g_resources, function () {
            //     cc.spriteFrameCache.addSpriteFrames(res.thumbnails_plist);
            //     cc.spriteFrameCache.addSpriteFrames(res.human_skeleton_plist);
            //     cc.spriteFrameCache.addSpriteFrames(res.record_animation_plist);
            //     cc.log("mode:" + chimple.mode);
            //     if (chimple.mode.indexOf(chimple.EDIT_MODE) != -1) {
            //         cc.director.runScene(new HelloWorldScene());
            //     } else {
            //         chimple.pageIndex = 0;
            //         window.PLAYING_STORY_FIRST_TIME = true;
            //         cc.director.runScene(new PlayFullStoryScene());
            //     }

            // });

            cc.LoaderScene.preload(g_resources, function () {
                cc.spriteFrameCache.addSpriteFrames(res.thumbnails_plist);
                cc.spriteFrameCache.addSpriteFrames(res.human_skeleton_plist);
                cc.spriteFrameCache.addSpriteFrames(res.record_animation_plist);
                cc.log("mode:" + chimple.mode);
                if (chimple.mode.indexOf(chimple.EDIT_MODE) != -1) {
                    cc.director.runScene(new HelloWorldScene());
                } else {
                    chimple.pageIndex = 0;
                    window.PLAYING_STORY_FIRST_TIME = true;
                    cc.director.runScene(new PlayFullStoryScene());
                }

            }, this);            
        } else {
            cc.LoaderScene.preload(g_resources, function () {
                cc.spriteFrameCache.addSpriteFrames(res.thumbnails_plist);
                cc.spriteFrameCache.addSpriteFrames(res.human_skeleton_plist);
                cc.spriteFrameCache.addSpriteFrames(res.record_animation_plist);
                cc.log("mode:" + chimple.mode);
                if (chimple.mode.indexOf(chimple.EDIT_MODE) != -1) {
                    cc.director.runScene(new HelloWorldScene());
                } else {
                    chimple.pageIndex = 0;
                    window.PLAYING_STORY_FIRST_TIME = true;
                    cc.director.runScene(new PlayFullStoryScene());
                }

            }, this);
        }
    };
    cc.game.run();

})();


