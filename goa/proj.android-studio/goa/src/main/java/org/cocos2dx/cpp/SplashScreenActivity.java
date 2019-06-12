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

    public static String getDataFilePath() {
        return "/storage/emulated/0/Android/data/com.maq.xprize.chimple.hindi/files/";
    }

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
    public void unzipFile() {
        int totalSize = getTotalSize();
        sharedPref = getSharedPreferences("ExpansionFile", MODE_PRIVATE);
        String flagFilePath = null;
        File[] fileList = getObbDirs();
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + getPackageName()) && file.isDirectory() && file.canRead()) {
                flagFilePath = file.getAbsolutePath();

            }
        }
        if (flagFilePath == null) {
            flagFilePath = "/storage/emulated/0/Android/data/" + getPackageName() + "/files/.success.txt";
        }
        File directory = new File(flagFilePath + java.io.File.separator + "DirectoryLoL");
        String lol;
        if (!directory.exists()) {
            lol = directory.mkdirs() ? "Directory has been created" : "Directory not created";
        } else
            lol = "Already there!";
        mainFileVersion = sharedPref.getInt(getString(R.string.mainFileVersion), 0);
        patchFileVersion = sharedPref.getInt(getString(R.string.patchFileVersion), 0);
        try {
            for (DownloadExpansionFile.XAPKFile xf : xAPKs) {
                if (xf.mIsMain && xf.mFileVersion != mainFileVersion || !xf.mIsMain && xf.mFileVersion != patchFileVersion) {
                    obbFilePath = getObbFilePath(xf.mIsMain, xf.mFileVersion);
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
                    obbFilePath = getObbFilePath(xf.mIsMain, xf.mFileVersion);
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

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    public String getObbFilePath(boolean isMain, int fileVersion) {
        String flagFilePath = null;
        File[] fileList = getObbDirs();
        for (File file : fileList) {
            if (!file.getAbsolutePath().equalsIgnoreCase(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + getPackageName()) && file.isDirectory() && file.canRead()) {
                flagFilePath = file.getAbsolutePath();

            }
        }
        if (flagFilePath == null) {
            flagFilePath = "/storage/emulated/0/Android/data/" + getPackageName() + "/files/.success.txt";
        }
        return flagFilePath + File.separator +
                Helpers.getExpansionAPKFileName(this, isMain, fileVersion);
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

