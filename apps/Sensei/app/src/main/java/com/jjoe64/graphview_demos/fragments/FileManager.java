package com.jjoe64.graphview_demos.fragments;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.widget.ArrayAdapter;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.StringTokenizer;

/**
 * Created by Saquib on 3/10/2016.
 */
public class FileManager {

    private static final String TAG = "FileManager";

    public void writeToFile(String fileName, String toWrite) {

        File root = android.os.Environment.getExternalStorageDirectory();
        // See
        // http://stackoverflow.com/questions/3551821/android-write-to-sd-card-folder
        File dir = new File(root.getAbsolutePath() + "/Wildflower");
        dir.mkdirs();
        File file = new File(dir, fileName);
//        if (!file.exists()) {
//            file.createNewFile();
//        }
        try {
            java.io.FileOutputStream f = new java.io.FileOutputStream(file);
            java.io.PrintWriter pw = new java.io.PrintWriter(f);
            pw.println(toWrite);
            pw.flush();
            pw.close();
            f.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            Log.e(TAG, "******* File not found. Did you"
                    + " add a WRITE_EXTERNAL_STORAGE permission to the   manifest?");
        } catch (java.io.IOException e) {
            e.printStackTrace();
        }
    }

}
