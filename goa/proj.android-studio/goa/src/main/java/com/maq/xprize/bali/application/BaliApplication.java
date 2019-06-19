package com.maq.xprize.bali.application;

import android.annotation.SuppressLint;
import android.app.Application;
import android.content.Context;
import android.util.Log;

import com.maq.xprize.bali.db.AppDatabase;
import com.maq.xprize.bali.service.ThreadManager;

import org.acra.ACRA;
import org.acra.ReportField;
import org.acra.annotation.ReportsCrashes;

//import com.maq.xprize.bali.crash.FtpCrashSenderFactory;
//import com.maq.xprize.bali.ftp.FtpManager;
//import com.maq.xprize.bali.launcher.LauncherScreen;


/**
 * Created by shyamalupadhyaya on 08/09/17.
 */

@SuppressLint("Registered")
@ReportsCrashes(
        customReportContent = {
                ReportField.APP_VERSION_CODE,
                ReportField.APP_VERSION_NAME,
                ReportField.ANDROID_VERSION,
                ReportField.PACKAGE_NAME,
                ReportField.REPORT_ID,
                ReportField.BUILD,
                ReportField.STACK_TRACE
        }
)
public class BaliApplication extends Application {
    private static final String TAG = BaliApplication.class.getName();
    @SuppressLint("StaticFieldLeak")
    private static Context context;
    private static final int COIN_NOTIFICATION = 1;
    public static int INITIAL_COIN = 0;

    private static final int COIN_NOTIFICATION_FIVE_RANGE = 5;
    private static final int COIN_NOTIFICATION_TWENTY_RANGE = 20;
    private static final int COIN_NOTIFICATION_FOURTY_RANGE = 40;
    private static final int COIN_NOTIFICATION_SIXTY_RANGE = 60;
    private static final int COIN_NOTIFICATION_EIGHTY_RANGE = 80;


    private static final int TOTAL_COINS = 10;

    public static final String ftpHost = "192.168.0.1";
    public static final int ftpPort = 21;
    public static final String ftpUser = "anonymous";
    public static final String ftpPassword = "nobody@chimple.in";


    @Override
    public void onCreate() {
        super.onCreate();
        if (!ACRA.isACRASenderServiceProcess()) {
            Log.d(TAG, "Created...");
//            initialize();
        }
        context = this;
    }

    private void initialize() {
        Log.d(TAG, "Initializing...");

        Thread initializationThread = new Thread() {
            @Override
            public void run() {
                // Initialize all of the important frameworks and objects
                BaliContext.getInstance().initialize(BaliApplication.this);
                //TODO: for now force the creation here
                AppDatabase.getInstance(BaliApplication.this);

                initializationComplete();
            }
        };

        initializationThread.start();
    }

    private void initializationComplete() {
        Log.d(TAG, "Initialization complete...");
//        ftpManager = BaliContext.getInstance().getFtpManager();
        //    private FtpManager ftpManager;
        ThreadManager threadManager = BaliContext.getInstance().getThreadManager();
    }

    public static Context getContext() {
        return context;
    }

}

