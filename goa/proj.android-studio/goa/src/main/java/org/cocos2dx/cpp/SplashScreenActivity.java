package org.cocos2dx.cpp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
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
import com.maq.xprize.chimple.hindi.R;

import java.io.File;
import java.io.IOException;
import java.util.zip.ZipFile;

import chimple.DownloadExpansionFile;
import utils.Zip;

import static chimple.DownloadExpansionFile.xAPKs;
import static org.cocos2dx.cpp.AppActivity.sharedPref;

public class SplashScreenActivity extends Activity {

    Intent intent = null;
    String obbFilePath;
    File obbFile;
    ZipFile obbZipFile;
    Zip zipFileHandler;
    String dataFilePath;
    File packageDir;
    int mainFileVersion;
    int patchFileVersion;

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)


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
        setContentView(R.layout.activity_splash_screen);

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
    public String getDataFilePath() {
        String internalDataFilePath = null;
        String externalDataFilePath = null;
        String DataFilePath = null;
        File[] fileList = getObbDirs();
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + getPackageName()) && file.isDirectory() && file.canRead()) {
//              For external storage path
                externalDataFilePath = file.getAbsolutePath();
                externalDataFilePath = externalDataFilePath.substring(0, externalDataFilePath.indexOf("obb"));
                externalDataFilePath = externalDataFilePath + "data/" + getPackageName() + "/files/";
            } else {
//              For internal storage path
                internalDataFilePath = file.getAbsolutePath();
                internalDataFilePath = internalDataFilePath.substring(0, internalDataFilePath.indexOf("obb"));
                internalDataFilePath = internalDataFilePath + "data/" + getPackageName() + "/files/";
            }
        }
        if (externalDataFilePath == null) {
            DataFilePath = internalDataFilePath;
        } else {
            DataFilePath = externalDataFilePath;
        }
        return DataFilePath;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public String getOBBFilePath() {
        String internalOBBFilePath = null;
        String externalOBBFilePath = null;
        String OBBFilePath = null;
        File[] fileList = getObbDirs();
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + getPackageName()) && file.isDirectory() && file.canRead()) {
//              For external storage path
                externalOBBFilePath = file.getAbsolutePath();
            } else {
//              For internal storage path
                internalOBBFilePath = file.getAbsolutePath();
            }
        }
        if (externalOBBFilePath == null) {
            OBBFilePath = internalOBBFilePath;
        } else {
            OBBFilePath = externalOBBFilePath;
        }
        return OBBFilePath;
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public void unzipFile() {
        int totalSize = getTotalSize();
        sharedPref = getSharedPreferences("ExpansionFile", MODE_PRIVATE);
        mainFileVersion = sharedPref.getInt(getString(R.string.mainFileVersion), 0);
        patchFileVersion = sharedPref.getInt(getString(R.string.patchFileVersion), 0);
        try {
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if (xf.mIsMain && xf.mFileVersion != mainFileVersion || !xf.mIsMain && xf.mFileVersion != patchFileVersion) {
                    obbFilePath = getOBBFilePath() + File.separator +
                            Helpers.getExpansionAPKFileName(this, xf.mIsMain, xf.mFileVersion);
                    obbFile = new File(obbFilePath);
                    obbZipFile = new ZipFile(obbFile);
                    zipFileHandler = new Zip(obbZipFile, this);
                    dataFilePath = getDataFilePath();
                    packageDir = new File(dataFilePath);
                    if (xf.mIsMain && packageDir.exists()) {
                        DownloadExpansionFile.deleteDir(packageDir);
                        packageDir.mkdir();
                    }
                    zipFileHandler.unzip(dataFilePath, totalSize, xf.mIsMain, xf.mFileVersion);
                    zipFileHandler.close();
                }
            }
            toCallApplication();
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public int getTotalSize() {
        int totalSize = 0;
        try {
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if (!xf.mIsMain && (xf.mFileVersion != patchFileVersion) || xf.mIsMain && (xf.mFileVersion != mainFileVersion)) {
                    obbFilePath = getOBBFilePath() + File.separator +
                            Helpers.getExpansionAPKFileName(this, xf.mIsMain, xf.mFileVersion);
                    obbFile = new File(obbFilePath);
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

