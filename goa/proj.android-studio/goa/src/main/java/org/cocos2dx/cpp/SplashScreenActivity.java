package org.cocos2dx.cpp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.google.android.vending.expansion.downloader.Helpers;

import java.io.File;
import java.io.IOException;
import java.util.zip.ZipFile;

import chimple.DownloadExpansionFile;
import utils.Zip;

import static android.support.v7.app.AlertDialog.Builder;
import static chimple.DownloadExpansionFile.xAPKs;
import static com.maq.xprize.chimple.hindi.R.layout;
import static com.maq.xprize.chimple.hindi.R.string;
import static com.maq.xprize.chimple.hindi.R.string.Attention;
import static com.maq.xprize.chimple.hindi.R.string.dataPath;
import static com.maq.xprize.chimple.hindi.R.string.dialogInfo;
import static com.maq.xprize.chimple.hindi.R.string.dialogNo;
import static com.maq.xprize.chimple.hindi.R.string.dialogYes;
import static org.cocos2dx.cpp.AppActivity.pathToAppDelegate;
import static org.cocos2dx.cpp.AppActivity.sharedPref;

public class SplashScreenActivity extends Activity {

    Intent intent = null;
    String obbFilePath;
    File obbFile;
    ZipFile obbZipFile;
    Zip zipFileHandler;
    String unzipDataFilePath;
    File packageDir;
    int mainFileVersion;
    int patchFileVersion;
    boolean flagSwitchToInternal = false;

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case 1: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    new DownloadFile().execute(null, null, null);
                } else {
                    Toast.makeText(this, "Permission required!", Toast.LENGTH_LONG).show();
                    finish();
                }
            }
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public void customDialog() {
        final Builder builderSingle = new Builder(this);
        builderSingle.setTitle(Attention);
        builderSingle.setMessage(dialogInfo);
        final SharedPreferences.Editor editor = sharedPref.edit();
        builderSingle.setNegativeButton(
                dialogNo,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        editor.putInt(getString(dataPath), 1);
                        editor.apply();
                        startExtraction();
                    }
                });

        builderSingle.setPositiveButton(
                dialogYes,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        editor.putInt(getString(dataPath), 2);
                        editor.apply();
                        startExtraction();
                    }
                });
//      to initialize pathToAppDelegate with the selected path
        String junkDataFilePath = getDataFilePath();
        builderSingle.show();
    }


    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (Build.VERSION.SDK_INT < 19) { // lower api
            View v = this.getWindow().getDecorView();
            v.setSystemUiVisibility(View.GONE);
        } else {
            //for new api versions.
            View decorView = this.getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            decorView.setSystemUiVisibility(uiOptions);
        }
        setContentView(layout.activity_splash_screen);
        if (isSDcard() && sharedPref.getInt(getString(dataPath), 0) == 0) {
            flagSwitchToInternal = true;
            customDialog();
        } else {
            final SharedPreferences.Editor editor = sharedPref.edit();
            editor.putInt(getString(dataPath), 1);
            editor.apply();
            new DownloadFile().execute(null, null, null);
        }
    }

    private void startExtraction() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            // Permission is not granted
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        } else {
            new DownloadFile().execute(null, null, null);
        }
    }

    /* function to call the main application after extraction */
    public void toCallApplication() {
        intent = new Intent(this, AppActivity.class);
        startActivity(intent);
        finish();
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public void unzipFile() {
        int totalSize = getTotalSize();
        sharedPref = getSharedPreferences("ExpansionFile", MODE_PRIVATE);
        mainFileVersion = sharedPref.getInt(getString(string.mainFileVersion), 0);
        patchFileVersion = sharedPref.getInt(getString(string.patchFileVersion), 0);
        try {
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if (xf.mIsMain && xf.mFileVersion != mainFileVersion || !xf.mIsMain && xf.mFileVersion != patchFileVersion) {
                    obbFile = getOBBFilePath(xf);
                    obbZipFile = new ZipFile(obbFile);
                    zipFileHandler = new Zip(obbZipFile, this);
                    unzipDataFilePath = getDataFilePath();
                    pathToAppDelegate = unzipDataFilePath;
                    packageDir = new File(unzipDataFilePath);
                    if (xf.mIsMain && packageDir.exists()) {
                        DownloadExpansionFile.deleteDir(packageDir);
                        packageDir.mkdirs();
                    }
                    zipFileHandler.unzip(unzipDataFilePath, totalSize, xf.mIsMain, xf.mFileVersion);
                    zipFileHandler.close();
                }
            }
            toCallApplication();
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public boolean isSDcard() {
        File[] fileList = getObbDirs();
        return fileList.length >= 2;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public String getDataFilePath() {
        String internalDataFilePath = null;
        String externalDataFilePath = null;
        String DataFilePath = null;
        File[] fileList = getExternalFilesDirs(null);
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/data/" + getPackageName() + "files/") &&
                    file.isDirectory() &&
                    file.canRead() &&
                    isSDcard() &&
                    sharedPref.getInt(getString(dataPath), 0) == 2) {
//              For external storage path
                externalDataFilePath = file.getAbsolutePath() + File.separator;
            } else if ((sharedPref.getInt(getString(dataPath), 0) == 1 || !flagSwitchToInternal) && internalDataFilePath == null) {
//              For internal storage path
                internalDataFilePath = file.getAbsolutePath() + File.separator;
            }
        }
        if (externalDataFilePath == null) {
            DataFilePath = internalDataFilePath;
        } else if (sharedPref.getInt(getString(dataPath), 0) == 2) {
            DataFilePath = externalDataFilePath;
        }
        return DataFilePath;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public File getOBBFilePath(DownloadExpansionFile.XAPKFile xf) {
        sharedPref = getSharedPreferences("ExpansionFile", MODE_PRIVATE);
        mainFileVersion = sharedPref.getInt(getString(string.mainFileVersion), 0);
        patchFileVersion = sharedPref.getInt(getString(string.patchFileVersion), 0);
        String internalOBBFilePath = null;
        String externalOBBFilePath = null;
        File externalOBBFile = null;
        File internalOBBFile = null;
        File[] fileList = getObbDirs();
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + getPackageName()) &&
                    file.isDirectory() &&
                    file.canRead() &&
                    isSDcard()) {
//              For external storage path
                externalOBBFilePath = file.getAbsolutePath() + File.separator +
                        Helpers.getExpansionAPKFileName(this, xf.mIsMain, xf.mFileVersion);
                externalOBBFile = new File(externalOBBFilePath);
            } else {
//              For internal storage path
                internalOBBFilePath = file.getAbsolutePath() + File.separator +
                        Helpers.getExpansionAPKFileName(this, xf.mIsMain, xf.mFileVersion);
                internalOBBFile = new File(internalOBBFilePath);
            }
        }
        if (externalOBBFile.exists()) {
            return externalOBBFile;
        } else {
            return internalOBBFile;
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public int getTotalSize() {
        int totalSize = 0;
        try {
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if (!xf.mIsMain && (xf.mFileVersion != patchFileVersion) || xf.mIsMain && (xf.mFileVersion != mainFileVersion)) {
                    obbFile = getOBBFilePath(xf);
                    obbZipFile = new ZipFile(obbFile);
                    totalSize += obbZipFile.size();
                }
            }
        } catch (IOException ie) {
            System.out.println(ie);
        }
        return totalSize;
    }

    @SuppressLint("StaticFieldLeak")
    private class DownloadFile extends AsyncTask<String, Integer, String> {
        @RequiresApi(api = Build.VERSION_CODES.KITKAT)
        @Override
        protected String doInBackground(String... sUrl) {
            unzipFile();
            return null;
        }
    }
}

