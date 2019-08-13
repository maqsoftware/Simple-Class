Last updated: August 12, 2019

## Pre-requisites
1. Install the latest version of [Android Studio](https://developer.android.com/studio).
2. Install and set up Java using [Java Development Kit](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html).
3. Install [Python 2](https://www.python.org/downloads/release/python-2716/). *Do not install Python 3.*
4. Download [Ant 1.9.14](https://ant.apache.org/bindownload.cgi) and unzip it to C:\ant.
5. Download [NDK R12b](https://dl.google.com/android/repository/android-ndk-r12b-windows-x86_64.zip) and unzip it to C:\ndk.
6. Download [Cocos2d-x 3.14.1](https://digitalocean.cocos2d-x.org/Cocos2D-X/cocos2d-x-3.14.1.zip) and unzip it to C:\cocos.
7. Download [Android SDK tools 25.2.5](https://dl.google.com/android/repository/tools_r25.2.5-windows.zip) and unzip it to C:\tools.
8. Install cocos command line tool.

    `cd C:\cocos`
    
    `setup.py`
    
    It will prompt the following variables:
    1. ANDROID_SDK_ROOT: Enter the directory from Step 1.4 above
    2. NDK_ROOT: C:\ndk\android-ndk-r12b
    3. ANT_ROOT: C:\ant\apache-ant-x.xx.x\bin
## Installation steps for all apps
1. Make sure an Android device is connected with USB debug permissions set.
2. Overwrite the directory *tools* in ANDROID_SDK_ROOT with C:\tools
3. `cd goa`
4. `cocos compile -p android`
5. Copy the content of _proj.android\libs_ folder into _proj.android-studio\goa\src\locale\jniLibs_ folder. For example for English build copy to _proj.android-studio\goa\src\english\jniLibs_ __Note__: Copy only _.so_ files.
6. Start Android Studio and import _proj.android-studio_ folder into it.
7. Download the OBB file from the latest GitHub [release](https://github.com/maqsoftware/Simple-Class/releases) and follow the instructions provided there.
8. To change the app locale, just select the desired language from the build variant.
9. Set the `localeCode` in [LevelHelpScene.h](https://github.com/maqsoftware/Simple-Class/blob/master/goa/Classes/menu/LevelHelpScene.h) file as per the build variant.
10. Build the project.

## Activate/Deactivate Kiosk Mode
By default, the Kiosk mode is enabled. So all the games are open. To disable Kiosk mode, set the KIOSK variable in 
`goa/Classes/menu/ScrollableGameMapScene.cpp` to false

```cpp
static const bool KIOSK = false;
```

## FAQs
1. How can I resolve an error encountered after running the "setup.py" script while installing cocos?

    > To resolve an error encountered after running the “setup.py” script while installing cocos, check to see whether you have Python 3 installed. Check whether you have Python 3 installed, uninstall it and try running setup.py again.

2. The application throws _keystore_ file not found error. How do I fix this?
	> _keystore_ file is used to digitally sign an Android application and hence, it is not provided with the project. One must create their own _keystore_ file using the [Android KeyTool](https://developer.android.com/studio/publish/app-signing) which comes with the Android Studio in order to sign the APK.

3. How to fix missing google-services.json file error?
	> Put google-services.json file in [app](https://github.com/maqsoftware/Simple-Class/tree/master/goa/proj.android-studio/goa/src) folder. It is there in Firebase console after the app project has been created.

4. How to enable the _Build variant_ option in the _Build_ menu?
	> The _Build variant_ option is enabled only when the module's gradle file is opened.
