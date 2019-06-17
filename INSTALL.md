# Compilation and installation instructions for the Chimple application
Last updated: June 10, 2019

The steps for building the Chimple application on a Windows 7 or newer are listed below. To compile and install on macOS X (Mavericks or newer) or Linux, similar steps can be followed. *Please do not create any directories with spaces in them.[Creating directories names with spaces may cause problems on Linux installations.*]
## Pre-requisites
1. Install the latest version of [Android Studio](https://developer.android.com/studio).
    1. Open Android Studio->Tools->SDK Manager
    2. In the **SDK Platforms** tab, choose Android 4.4 (KitKat)
    3. In the **SDK Tools** tab, choose the latest Android SDK Platform-Tools and Android SDK Tools. Also, choose CMake version as 3.6.4111459
    4. Note the Android SDK Location at the top of the window
2. Install the [Java Development Kit](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html). These steps were created usign Java SE Development Kit 3.4 8u211 for Windows 64-bit.
    1. Open Control Panel->System and Security->System->Advanced System Settings->Environment Variables and set a new variable JAVA_HOME to the directory where JDK is installed. Provide the JDK directory, and not the JRE directory.
3. Install [Python 2](https://www.python.org/downloads/release/python-2716/). *Do not install Python 3.* These steps were created usign Python 2.7.16 for Windows 64-bit.
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
## Build and install goa
Goa is an application which has many games for a child to play while improving literacy.
1. Make sure an Android device is connected with USB debug permissions set.
2. Overwrite the directory *tools* in ANDROID_SDK_ROOT with C:\tools
3. `cd goa`
4. `cocos compile -p android`
5. Copy the content of _proj.android\libs_ folder into _proj.android-studio\goa\src\main\jniLibs_ folder. __Note__: Copy only _.so_ files.
6. Start Android Studio and import _proj.android-studio_ folder into it.
7. Build and run the application.

## Activate/Deactivate Kiosk Mode
By default, the Kiosk mode is enabled. So all games are open. To disable Kiosk mode, set the KIOSK variable in 
`goa/Classes/menu/ScrollableGameMapScene.cpp` to false

``static const bool KIOSK = false;``

## FAQs
1. How can I resolve an error encountered after running the "setup.py" script while installing cocos?

    > To resolve an error encountered after running the “setup.py” script while installing cocos, check to see whether you have Python 3 installed. If you do have Python 3 installed, uninstall it and try running setup.py again.