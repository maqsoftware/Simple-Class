Last updated: August 13, 2019

## Prerequisites
Install Cocos Studio for [Mac](http://cocos2d-x.org/filedown/CocosForMac-v3.10.dmg) or [Windows](http://cocos2d-x.org/filedown/CocosForWin-v3.10.exe)

This is used for creating scene assets representing the monster characters used in the game

## Steps to localize only instructions
 	
* To build the application with a new locale, add a new `productFlavor` in [build.gradle](https://github.com/maqsoftware/Simple-Class/blob/master/goa/proj.android-studio/goa/build.gradle) file.
* Localize all the tutorial videos that are present in [help](https://github.com/maqsoftware/Simple-Class/tree/master/goa/Resources/res/lang/eng/help) section.
* The [lang](https://github.com/maqsoftware/Simple-Class/tree/master/goa/Resources/res/lang/eng) folder contains a ***eng_en.po*** file, which contains strings in the `msgstr` tags, should also be localized and the new file should be saved as ***eng_[lang].po***.

  ```	
  msgid "text"	
  msgstr "text in local language"
  ```
  After editing the ***.po*** file, a ***.mo*** file should be generated using an external utility ([sample](https://po2mo.net/)).
* The [config](https://github.com/maqsoftware/Simple-Class/tree/master/goa/Resources/res/config) folder contains JSON files that can be localized.
 	* Update `title` in _alphabet_game_map_en.json_, _shapes_game_map_en.json_, _words_game_map_en.json_, and _writing_game_map_en.json_
	* The help instruction is contained in the ***game_levels_en.json*** file with `help` as the key for each section.	
      ```json	
      "miningbg": [	
          {	
            "levels": [	
              1	
            ],	
            "help": "help/instruction text in local language",	
            "video": "help.webm"	
          }	
        ]	
      ```
    * Save all the JSON files in [config](https://github.com/maqsoftware/Simple-Class/tree/master/goa/Resources/res/config) folder and append the ***lang*** code in their names. For example _grammar_game_map_sw.json_.
* Update the `localeCode` in [LevelHelpScene.h](https://github.com/maqsoftware/Simple-Class/blob/master/goa/Classes/menu/LevelHelpScene.h) file as per the desired build variant.
* Update the TTS locale settings according to the new locale in [VoiceMoldManager.java](https://github.com/maqsoftware/Simple-Class/blob/master/goa/proj.android-studio/goa/src/main/java/org/cocos2dx/cpp/VoiceMoldManager.java).
* Add splash screen image in [chimpleloading](https://github.com/maqsoftware/Simple-Class/tree/master/goa/Resources/res/chimpleloading) folder with translated text.
* Add locale specific icon and extraction images in [src](https://github.com/maqsoftware/Simple-Class/tree/master/goa/proj.android-studio/goa/src) folder.

## Steps to localize entire app content
- Create a new folder under Resources/res/lang/. Let's say the folder name is xyz

- Record letter pronunciation voice files for all letters in the language and save in **sounds** folder *ex a.ogg*

- Record phonetic pronunciation voice files for all letters in the language and save in **audio/phonetic** folder *ex a.ogg*

- Using Cocos Studio, create monster csb files for each letter in the alphabet and store *ex A.csb*

- Using Cocos Studio, create fruit csb file for each letter in the alphabet and store in **alphabet fruits** folder *ex A.csb*

- Generate a fnt (font) file from ttf font of the language using any convertor such as [BMGlyph](https://www.bmglyph.com) or [Glyph Designer](https://www.71squared.com/glyphdesigner) or [Hiero](https://github.com/libgdx/libgdx/wiki/Hiero)

- In `goa/Classes/hero/RPGConfig.h`, add the language like this:
  ```cpp
  enum SupportedLanguages
  {
      ENGLISH = 0,
      KANNADA = 1,
      GERMAN = 2,
      SWAHILI = 3,
      TELUGU = 4,
      XYZ = 5
  };
  ```

- Create a new subclass of **LangUtil** in `goa/Classes/lang/XyzUtil.cpp` like `goa/Classes/lang/SwahiliUtil.cpp`. Override each method with language specific details such as number of letters in an alphabet, etc.

- In `goa/Classes/lang/LangUtil.cpp`:
	Add a new switch case in line 58 like this:
  ```cpp
        case SupportedLanguages::XYZ:
        {
            LangUtil::_instance = new XyzUtil();
            Data moData = FileUtils::getInstance()->getDataFromFile("res/lang/xyz/xyz.mo");
            I18N::I18nUtils::getInstance()->removeAllMO();
            I18N::I18nUtils::getInstance()->addMO(moData.getBytes());
            break;
        }
  ```
  Also comment other languages and add this line after line 28:
  ```cpp
  _instance = new XyzUtil();
  ```
- Translate the `Resources/res/lang/eng/eng.po` file to the new language and store in `Resources/res/lang/xyz/xyz.mo`. For convenience, you can use [POEditor](https://poeditor.com/) or any other PO editing software.
- Copy the folder `Resources/res/story/eng` to `Resources/res/story/xyz`:
  - Translate each story by translating the 2 json files:
    - Story.json *ex Adhabu_Punishment.json*
    - Story.questions.json *ex Adhabu_Punishment.questions.json*
  - Record the voice-over for each story and save in **Story** folder with prefix of page number. *For ex, Adhabu_Punishment/Adhabu_Punishment_0.ogg, Adhabu_Punishment/Adhabu_Punishment_1.ogg*
