/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import android.app.Activity;
import android.arch.lifecycle.LiveData;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.widget.Toast;

import com.maq.xprize.bali.db.entity.User;
import com.maq.xprize.bali.repo.UserRepo;
import com.maq.xprize.chimple.hindi.R;

import org.cocos2dx.lib.Cocos2dxActivity;

import java.io.File;
import java.util.Locale;

import chimple.DownloadExpansionFile;

import static chimple.DownloadExpansionFile.xAPKs;

public class AppActivity extends Cocos2dxActivity {
    public static final String TAG = "GOA";
    public static final String AUTHORITY = "com.maq.xprize.bali.provider";
    public static final String MULTIPLE_CHOICE_QUIZ = "MULTIPLE_CHOICE_QUIZ";
    public static final String COINS = "COINS";
    public static final String GAME_NAME = "GAME_NAME";
    public static final String GAME_LEVEL = "GAME_LEVEL";
    public static final String GAME_EVENT = "GAME_EVENT";
    public static final String BAG_OF_CHOICE_QUIZ = "BAG_OF_CHOICE_QUIZ";
    /**
     * The URI for the Multiple Choice Quiz
     */
    public static final Uri URI_MULTIPLE_CHOICE_QUIZ = Uri.parse(
            "content://" + AUTHORITY + "/" + MULTIPLE_CHOICE_QUIZ);
    public static final Uri URI_BAG_OF_CHOICE_QUIZ = Uri.parse(
            "content://" + AUTHORITY + "/" + BAG_OF_CHOICE_QUIZ);
    public static final Uri URI_COIN = Uri.parse(
            "content://" + AUTHORITY + "/" + COINS);
    private static final boolean D = true;
    // Intent request codes
    public static int width;
    // Return Intent extra
    public static SharedPreferences sharedPref;
    public static Activity _activity;
    protected static String pathToAppDelegate = null;
    private static AppActivity _appActivity;
    private static Context _context;
    private static String currentGameName;
    private static boolean supportForTTSEnabled = false;

    static {
        System.out.println("Loaded library");
    }

    private Handler handler = null;
    private TextToSpeech textToSpeechInstance;

    public static AppActivity instance() {
        return _appActivity;
    }

    //  LauncherScreen variables and functions from Bali
    public static native void setMultipleChoiceQuiz(String[] jsonInfo);

    public static native void setBagOfChoiceQuiz(String[] jsonInfo);

    //  Method called by AppDelegate.cpp
    public static String getPathToAppDelegate() {
        return pathToAppDelegate;
    }

    public static void queryMultipleChoiceQuiz(int numQuizes, int numChoices, int answerFormat, int choiceFormat) {
        System.out.println("entry queryMultipleChoiceQuiz");
        new AsyncTask<int[], Void, Void>() {
            @Override
            protected Void doInBackground(int[]... params) {
                System.out.println("doInBackground");
                int numQuizes = params[0][0];
                int numChoices = params[0][1];
                int answerFormat = params[0][2];
                int choiceFormat = params[0][3];
                Cursor cursor = _context.getContentResolver().query(
                        URI_MULTIPLE_CHOICE_QUIZ,
                        null,
                        null,
                        new String[]{Integer.toString(numQuizes), Integer.toString(numChoices + 1),
                                Integer.toString(answerFormat), Integer.toString(choiceFormat)},
                        null
                );
                System.out.println("called getContentResolver");

                if (cursor == null) {

                } else if (cursor.getCount() < 1) {
                    cursor.close();
                } else {
                    System.out.println("got data getContentResolver");
                    String[] columnNames = cursor.getColumnNames();
                    for (String s : columnNames) {
                        System.out.println(s);
                    }
                    String[] sendArray = new String[cursor.getCount() * columnNames.length + 2];
                    int i = 0;
                    sendArray[i++] = Integer.toString(cursor.getCount());
                    sendArray[i++] = Integer.toString(columnNames.length - 4);
                    while (cursor.moveToNext()) {
                        int j = 0;
                        sendArray[i++] = cursor.getString(j++);
                        sendArray[i++] = cursor.getString(j++);
                        sendArray[i++] = Integer.toString(cursor.getInt(j++));
                        for (; j < columnNames.length; j++) {
                            sendArray[i++] = cursor.getString(j);
                        }
                    }
                    final String[] finalSendArray = sendArray;
                    ((Cocos2dxActivity) _activity).runOnGLThread(new Runnable() {
                        @Override
                        public void run() {
                            System.out.println("calling setMultipleChoiceQuiz");
                            for (String s : finalSendArray) {
                                System.out.println(s);
                            }
                            setMultipleChoiceQuiz(finalSendArray);
                        }
                    });
                    cursor.close();
                }
                return null;
            }
        }.execute(new int[]{numQuizes, numChoices, answerFormat, choiceFormat});

    }

    public static void queryBagOfChoiceQuiz(int numQuizes, int minAnswers, int maxAnswers,
                                            int minChoices, int maxChoices, int order) {
        Log.d("queryBagOfChoiceQuiz", "entry queryBagOfChoiceQuiz");
        new AsyncTask<int[], Void, Void>() {
            @Override
            protected Void doInBackground(int[]... params) {
                Log.d("queryBagOfChoiceQuiz", "doInBackground");
                int numQuizes = params[0][0];
                int minAnswers = params[0][1];
                int maxAnswers = params[0][2];
                int minChoices = params[0][3];
                int maxChoices = params[0][4];
                int order = params[0][5];
                Cursor cursor = _context.getContentResolver().query(
                        URI_BAG_OF_CHOICE_QUIZ,
                        null,
                        null,
                        new String[]{Integer.toString(numQuizes), Integer.toString(minAnswers),
                                Integer.toString(maxAnswers), Integer.toString(minChoices),
                                Integer.toString(maxChoices), Integer.toString(order)},
                        null
                );
                Log.d("queryBagOfChoiceQuiz", "called getContentResolver");

                if (cursor == null) {

                } else if (cursor.getCount() < 1) {
                    cursor.close();
                } else {
                    System.out.println("got data getContentResolver");
                    String[] columnNames = cursor.getColumnNames();
                    for (String s : columnNames) {
                        Log.d("queryBagOfChoiceQuiz", s);
                    }
                    String[] sendArray = new String[cursor.getCount() * columnNames.length + 1];
                    int i = 0;
                    sendArray[i++] = Integer.toString(cursor.getCount());
                    while (cursor.moveToNext()) {
                        for (int j = 0; j < columnNames.length; j++) {
                            sendArray[i++] = cursor.getString(j);
                        }
                    }
                    final String[] finalSendArray = sendArray;
                    ((Cocos2dxActivity) _activity).runOnGLThread(new Runnable() {
                        @Override
                        public void run() {
                            Log.d("queryBagOfChoiceQuiz", "calling setMultipleChoiceQuiz");
                            for (String s : finalSendArray) {
                                if (s != null) {
                                    Log.d("queryBagOfChoiceQuiz", s);
                                }
                            }
                            setBagOfChoiceQuiz(finalSendArray);
                        }
                    });
                    cursor.close();
                }
                return null;
            }
        }.execute(new int[]{numQuizes, minAnswers, maxAnswers, minChoices, maxChoices, order});

    }

    public static void updateCoins(String gameName, int gameLevel, int gameEvent, int coins) {
        final String fGameName = gameName;
        final int fGameLevel = gameLevel;
        final int fGameEvent = gameEvent;
        final int fCoins = coins;
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                System.out.println("doInBackground");

                ContentValues contentValues = new ContentValues(4);
                contentValues.put(GAME_NAME, fGameName);
                contentValues.put(GAME_LEVEL, fGameLevel);
                contentValues.put(GAME_EVENT, fGameEvent);
                contentValues.put(COINS, fCoins);
                int coins = _context.getContentResolver().update(
                        URI_COIN,
                        contentValues,
                        null,
                        null
                );
                Log.d(COINS, String.valueOf(fCoins));

                return null;
            }
        }.execute();
    }

    public static native boolean discoveredBluetoothDevices(String devices);

    public static native boolean photoDestinationURL(String path);

    public static void pronounceWord(String word) {
        System.out.println("word for pronounciation:" + word);
        if (supportForTTSEnabled && _appActivity.textToSpeechInstance != null) {
            System.out.println("TTS supported for pronounciation:" + word);
            _appActivity.textToSpeechInstance.speak(word, TextToSpeech.QUEUE_FLUSH, null);
        }
    }

    public static void takePhoto() {
        String tag = "Take Photo";
        String message = "I've been called from C++";
        Log.d(tag, "Showing alert dialog: " + message);

        Intent startCameraActivity = new Intent(_activity, CameraActivity.class);
        _activity.startActivityForResult(startCameraActivity, 0);
    }

    public static native boolean sendRecognizedStringToGame(String path);

    public static boolean launchGameNotification() {
        boolean result = false;
        if (currentGameName != null) {
            Log.d(TAG, "launchGameNotification WIFi-Direct:" + currentGameName);
            String gameName = currentGameName;
            result = launchGame(gameName);
            currentGameName = null;
        }
        return result;
    }

    public static native boolean launchGame(String gameName);

    public static native void updateInformation(String jsonInfo);

    //  Method to check if SD card is mounted
    public boolean isSDcard() {
        File[] fileList = getObbDirs();
        return fileList.length >= 2;
    }

    public String getDataFilePath() {
        String internalDataFilePath = null;
        String externalDataFilePath = null;
        String dataFilePath = null;
        File[] fileList = getExternalFilesDirs(null);
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/data/" + getPackageName() + "/files") &&
                    file.isDirectory() &&
                    file.canRead() &&
                    isSDcard() &&
                    sharedPref.getInt(getString(R.string.dataPath), 0) == 2) {
//              For external storage path
                externalDataFilePath = file.getAbsolutePath() + File.separator;
            } else if (sharedPref.getInt(getString(R.string.dataPath), 0) == 1 && internalDataFilePath == null) {
//              For internal storage path
                internalDataFilePath = file.getAbsolutePath() + File.separator;
            }
        }
        if (externalDataFilePath == null) {
            dataFilePath = internalDataFilePath;
        } else if (sharedPref.getInt(getString(R.string.dataPath), 0) == 2) {
            dataFilePath = externalDataFilePath;
        }
        pathToAppDelegate = dataFilePath;

        return dataFilePath;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        sharedPref = getSharedPreferences("ExpansionFile", MODE_PRIVATE);
        int defaultFileVersion = 0;
        boolean extractionRequired = false;
        needExtraction();
//      Used to initialize the pathToAppDelegate with the selected path
        getDataFilePath();
        if ((sharedPref.getInt(getString(R.string.dataPath), 0) == 0)) {
            SharedPreferences.Editor editor = sharedPref.edit();
            editor.putInt(getString(R.string.mainFileVersion), defaultFileVersion);
            editor.putInt(getString(R.string.patchFileVersion), defaultFileVersion);
            editor.apply();
            extractionRequired = true;
        } else {
            int mainFileVersion = sharedPref.getInt(getString(R.string.mainFileVersion), defaultFileVersion);
            int patchFileVersion = sharedPref.getInt(getString(R.string.patchFileVersion), defaultFileVersion);
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if ((xf.mIsMain && xf.mFileVersion != mainFileVersion) || (!xf.mIsMain && xf.mFileVersion != patchFileVersion)) {
                    extractionRequired = true;
                    break;
                }
            }
        }
        if (extractionRequired) {
            Intent intent = new Intent(AppActivity.this, SplashScreenActivity.class);
            startActivity(intent);
            finish();
        }

        super.onCreate(savedInstanceState);

//        BaliApplication Code which was run by the launcher of Bali
        LiveData<User> userLiveData = UserRepo.getCurrentLiveUser(this);

        _appActivity = this;
        _activity = this;
        _context = this;
        handler = new Handler(getMainLooper());

        getGLSurfaceView().setMultipleTouchEnabled(false);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        textToSpeechInstance = new TextToSpeech(getApplicationContext(), new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                if (status == TextToSpeech.SUCCESS) {
                    int result = textToSpeechInstance.setLanguage(new Locale("hi", "IN"));
                    if (result == TextToSpeech.LANG_MISSING_DATA || result == TextToSpeech.LANG_NOT_SUPPORTED) {
                        Log.e("TTS", "This Language is not supported");
                    } else {
                        supportForTTSEnabled = true;
                    }
                } else {
                    Log.e("TTS", "Initialization Failed!");
                }
            }
        });
    }

    private void needExtraction() {
        File[] fileList = getExternalFilesDirs(null);
        SharedPreferences.Editor editor = sharedPref.edit();
        for (File file : fileList) {
            File flagFile = new File(".success.txt");
            file = new File(file + File.separator + flagFile);
            /*
             * Checks if any older version has been installed and extracted successfully
             * if extracted successfully, then set the existing path location as the preference.
             */
            if (file.exists()) {
                if (file.toString().contains("emulated")) {
                    editor.putInt(getString(R.string.dataPath), 1);
                } else {
                    editor.putInt(getString(R.string.dataPath), 2);
                }
                editor.apply();
            }
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
    }

    private void execute(final String photoUrl) {
        ((Cocos2dxActivity) _activity).runOnGLThread(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                System.out.println("comes and calling desimation with :" + photoUrl);
                photoDestinationURL(photoUrl);
            }
        });
    }

    private void executeRecognizedCharcter(final String characterStr) {
        ((Cocos2dxActivity) _activity).runOnGLThread(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                System.out.println("comes and calling desimation with :" + characterStr);
                sendRecognizedStringToGame(characterStr);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (textToSpeechInstance != null) {
            textToSpeechInstance.stop();
            textToSpeechInstance.shutdown();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    protected void onResume() {
        super.onResume();
        Intent intent = new Intent();
        intent.setClassName("com.maq.xprize.bali", "com.maq.xprize.bali.service.TollBroadcastReceiver");
        intent.putExtra("onResume", "com.maq.xprize.chimple.hindi");
        sendBroadcast(intent);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Intent intent = new Intent();
        intent.setClassName("com.maq.xprize.bali", "com.maq.xprize.bali.service.TollBroadcastReceiver");
        intent.putExtra("onPause", "com.maq.xprize.chimple.hindi");
        sendBroadcast(intent);
    }

    public void showToast(final String msg) {
        AppActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(AppActivity.this, msg, Toast.LENGTH_SHORT).show();

            }
        });
    }

    public void appendStatus(String status) {
        Log.d(TAG, "Status from WIFi-Direct: " + status);
    }

    private void updateOtherDeviceInformation(final String information) {
        ((Cocos2dxActivity) _activity).runOnGLThread(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                System.out.println("Updating information :" + information);
                updateInformation(information);
            }
        });
    }
}
