/**
 * This file is licensed under MIT
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Jonas Gehring
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package com.jjoe64.graphview_demos.fragments;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Typeface;
import android.hardware.usb.UsbManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.LegendRenderer;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;
import com.jjoe64.graphview_demos.MainActivity;
import com.jjoe64.graphview_demos.R;

import com.physicaloid.lib.Physicaloid;
import com.physicaloid.lib.usb.driver.uart.UartConfig;

import java.io.IOException;
import java.util.Arrays;
import java.util.Random;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by jonas on 28.08.14.
 */
public class CollectData extends Fragment {
    static final int PREFERENCE_REV_NUM = 1;

    // debug settings
    private static final boolean SHOW_DEBUG                 = true;
    private static final boolean USE_WRITE_BUTTON_FOR_DEBUG = false;

    public static final boolean isICSorHigher = ( Build.VERSION.SDK_INT > Build.VERSION_CODES.HONEYCOMB_MR2 );

    // occurs USB packet loss if TEXT_MAX_SIZE is over 6000
    private static final int TEXT_MAX_SIZE = 15000;

    private static final int MENU_ID_SETTING        = 0;
    private static final int MENU_ID_CLEARTEXT      = 1;
    private static final int MENU_ID_SENDTOEMAIL    = 2;
    private static final int MENU_ID_OPENDEVICE     = 3;
    private static final int MENU_ID_CLOSEDEVICE    = 4;
    private static final int MENU_ID_WORDLIST       = 5;

    private static final int REQUEST_PREFERENCE         = 0;
    private static final int REQUEST_WORD_LIST_ACTIVITY = 1;

    // Defines of Display Settings
    private static final int DISP_CHAR  = 0;
    private static final int DISP_DEC   = 1;
    private static final int DISP_HEX   = 2;

    // Linefeed Code Settings
    private static final int LINEFEED_CODE_CR   = 0;
    private static final int LINEFEED_CODE_CRLF = 1;
    private static final int LINEFEED_CODE_LF   = 2;

    // Load Bundle Key (for view switching)
    private static final String BUNDLEKEY_LOADTEXTVIEW = "bundlekey.LoadTextView";

    Physicaloid mSerial = null;

    public static boolean collectData = false;

    private ScrollView mSvText;
    private TextView mTvSerial;
    private StringBuilder mText = new StringBuilder();
    private boolean mStop = false;

    String TAG = "AndroidSerialTerminal";

    Handler mHandler = new Handler();

    private Button btWriteFile;
    private Button startCollect;
    private EditText etWrite;
    private ProgressBar mProgressBar;

    //File parameters
    private String[] names = {"Alice", "Bob", "Chris", "Dave", "Eliza", "Fred", "George", "Helen", "Jack", "Kelly", "Linda", "Meg", "Noah", "Oscar", "Peter", "Quinn"};
    private int networkSize = 16;
    private int currDeviceID = 0;
    private int[] dataReceived = new int[networkSize];

    // Default settings
    private int mTextFontSize       = 12;
    private Typeface mTextTypeface  = Typeface.MONOSPACE;
    private int mDisplayType        = DISP_CHAR;
    private int mReadLinefeedCode   = LINEFEED_CODE_LF;
    private int mWriteLinefeedCode  = LINEFEED_CODE_LF;
    private int mBaudrate           = 57600;
    private int mDataBits           = UartConfig.DATA_BITS8;
    private int mParity             = UartConfig.PARITY_NONE;
    private int mStopBits           = UartConfig.STOP_BITS1;
    private int mFlowControl        = UartConfig.FLOW_CONTROL_OFF;
    private String mEmailAddress    = "@gmail.com";

    private boolean mRunningMainLoop = false;

    private static final String ACTION_USB_PERMISSION =
            "jp.ksksue.app.terminal.USB_PERMISSION";

    // Linefeed
    private final static String BR = System.getProperty("line.separator");

    public CheckBox[] devicesOnline = new CheckBox[networkSize];

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_collect_data_3, container, false);


        //mSvText = (ScrollView) rootView.findViewById(R.id.svText);
        //mTvSerial = (TextView) rootView.findViewById(R.id.tvSerial);
        startCollect = (Button) rootView.findViewById(R.id.startCollect);
        startCollect.setEnabled(true);
        mProgressBar=(ProgressBar) rootView.findViewById(R.id.progressBar);
        mProgressBar.setMax(15);
        //btWriteFile = (Button) rootView.findViewById(R.id.btWriteFile);
        //btWriteFile.setEnabled(false);
        //etWrite = (EditText) rootView.findViewById(R.id.etWrite);
        //etWrite.setEnabled(false);
        //etWrite.setHint("CR : \\r, LF : \\n, bin : \\u0000");
        LinearLayout checkboxList = (LinearLayout) rootView.findViewById(R.id.checkboxList);
        LinearLayout checkboxList2 = (LinearLayout) rootView.findViewById(R.id.checkboxList2);
        for (int i = 0; i < networkSize; i++) {
            devicesOnline[i] = new CheckBox(getActivity());
            devicesOnline[i].setText(names[i]);
            devicesOnline[i].setId(i);
            devicesOnline[i].setHeight(75);
            devicesOnline[i].setTextSize(25);
            devicesOnline[i].setTextAppearance(getActivity(), android.R.style.TextAppearance_Medium);
            if (i <= 7)
                checkboxList.addView(devicesOnline[i]);
            else
                checkboxList2.addView(devicesOnline[i]);
        }

        if (SHOW_DEBUG) {
            Log.d(TAG, "New FTDriver");
        }

        // get service
        mSerial = new Physicaloid(getActivity());

        if (SHOW_DEBUG) {
            Log.d(TAG, "New instance : " + mSerial);
        }
        // listen for new devices
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        getActivity().registerReceiver(mUsbReceiver, filter);

        if (SHOW_DEBUG) {
            Log.d(TAG, "FTDriver beginning");
        }
        closeUsbSerial();
        openUsbSerial();
        Arrays.fill(dataReceived,1);

        // ---------------------------------------------------------------------------------------
        // Collect Button
        // ---------------------------------------------------------------------------------------
        startCollect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //if (mSerial != null) {
                    String toSend = "D";
                    for (int i = 0; i < networkSize; i++) {
                        toSend = toSend.concat(Integer.toString(dataReceived[i]));
                    }
                    byte[] start_code = toSend.getBytes();
                    //mSerial.write(start_code, start_code.length);
                    //startCollect.setEnabled(false);
                    //collectData = true;
                    Log.d(TAG, "Should be collecting data now");

                    //TEMP
                    new CountDownTimer(300000, 10000) {
                        int device_counter = 0;
                        public void onTick(long millisUntilFinished) {
                            if (device_counter < networkSize) {
                                Random rand = new Random();
                                int n = rand.nextInt(20) + 0;
                                n = n * 1000;
                                SystemClock.sleep(n);
                                devicesOnline[device_counter].setChecked(true);
                                device_counter = device_counter + 1;
                                mProgressBar.setProgress(device_counter);
                            }
                            else{
                                mProgressBar.setProgress(mProgressBar.getMax());
                            }
                        }

                        public void onFinish() {
                            device_counter = device_counter + 1;
                            mProgressBar.setProgress(mProgressBar.getMax());
                        }
                    }.start();
                //}
            }
        });

        // ---------------------------------------------------------------------------------------
        // Write Button
        // ---------------------------------------------------------------------------------------
        /*btWriteFile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mTvSerial.getText().toString().length() != 0) {
                    FileManager fm = new FileManager();
                    String filename = mTvSerial.getText().toString().substring(0, 1) + ".txt";
                    fm.writeToFile(filename, mTvSerial.getText().toString());
                    btWriteFile.setEnabled(false);
                }
            }
        });*/

        return rootView;
    }

    private String changeEscapeSequence(String in) {
        String out = new String();
        try {
            out = unescapeJava(in);
        } catch (IOException e) {
            return "";
        }
        switch (mWriteLinefeedCode) {
            case LINEFEED_CODE_CR:
                out = out + "\r";
                break;
            case LINEFEED_CODE_CRLF:
                out = out + "\r\n";
                break;
            case LINEFEED_CODE_LF:
                out = out + "\n";
                break;
            default:
        }
        return out;
    }

//    public void setWriteTextString(String str) {
//        etWrite.setText(str);
//    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_WORD_LIST_ACTIVITY) {
            if(resultCode == getActivity().RESULT_OK) {
                try {
                    String strWord = data.getStringExtra("word");
                    //etWrite.setText(strWord);
                    // Set a cursor position last
                    //etWrite.setSelection(etWrite.getText().length());
                } catch(Exception e) {
                    Toast.makeText(getActivity(), e.getMessage(), Toast.LENGTH_LONG).show();
                }
            }
        } else if (requestCode == REQUEST_PREFERENCE) {

            SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(getActivity());

            String res = pref.getString("display_list", Integer.toString(DISP_CHAR));
            mDisplayType = Integer.valueOf(res);

            res = pref.getString("fontsize_list", Integer.toString(12));
            mTextFontSize = Integer.valueOf(res);
            mTvSerial.setTextSize(mTextFontSize);

            res = pref.getString("typeface_list", Integer.toString(3));
            switch(Integer.valueOf(res)){
                case 0:
                    mTextTypeface = Typeface.DEFAULT;
                    break;
                case 1:
                    mTextTypeface = Typeface.SANS_SERIF;
                    break;
                case 2:
                    mTextTypeface = Typeface.SERIF;
                    break;
                case 3:
                    mTextTypeface = Typeface.MONOSPACE;
                    break;
            }
//            mTvSerial.setTypeface(mTextTypeface);
//            etWrite.setTypeface(mTextTypeface);

            res = pref.getString("readlinefeedcode_list", Integer.toString(LINEFEED_CODE_CRLF));
            mReadLinefeedCode = Integer.valueOf(res);

            res = pref.getString("writelinefeedcode_list", Integer.toString(LINEFEED_CODE_CRLF));
            mWriteLinefeedCode = Integer.valueOf(res);

            res = pref.getString("email_edittext", "@gmail.com");
            mEmailAddress = res;

            int intRes;

            res = pref.getString("baudrate_list", Integer.toString(57600));
            intRes = Integer.valueOf(res);
            if (mBaudrate != intRes) {

                mBaudrate = intRes;
                mSerial.setBaudrate(mBaudrate);
            }

            res = pref.getString("databits_list", Integer.toString(UartConfig.DATA_BITS8));
            intRes = Integer.valueOf(res);
            if (mDataBits != intRes) {
                mDataBits = Integer.valueOf(res);
                mSerial.setDataBits(mDataBits);
            }

            res = pref.getString("parity_list",
                    Integer.toString(UartConfig.PARITY_NONE));
            intRes = Integer.valueOf(res);
            if (mParity != intRes) {
                mParity = intRes;
                mSerial.setParity(mParity);
            }

            res = pref.getString("stopbits_list",
                    Integer.toString(UartConfig.STOP_BITS1));
            intRes = Integer.valueOf(res);
            if (mStopBits != intRes) {
                mStopBits = intRes;
                mSerial.setStopBits(mStopBits);
            }

            res = pref.getString("flowcontrol_list",
                    Integer.toString(UartConfig.FLOW_CONTROL_OFF));
            intRes = Integer.valueOf(res);
            if (mFlowControl != intRes) {
                mFlowControl = intRes;
                if(mFlowControl == UartConfig.FLOW_CONTROL_ON) {
                    mSerial.setDtrRts(true, true);
                } else {
                    mSerial.setDtrRts(false, false);
                }
            }

            /*
            res = pref.getString("break_list", Integer.toString(FTDriver.FTDI_SET_NOBREAK));
            intRes = Integer.valueOf(res) << 14;
            if (mBreak != intRes) {
                mBreak = intRes;
                mSerial.setSerialPropertyBreak(mBreak, FTDriver.CH_A);
                mSerial.setSerialPropertyToChip(FTDriver.CH_A);
            }
            */
        }
    }

    // ---------------------------------------------------------------------------------------
    // End of Menu button
    // ---------------------------------------------------------------------------------------

    /**
     * Saves values for view switching
     */
    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        //outState.putString(BUNDLEKEY_LOADTEXTVIEW, mTvSerial.getText().toString());
    }

    /**
     * Loads values for view switching
     */

//    @Override
//    protected void onRestoreInstanceState(Bundle savedInstanceState) {
//        super.onRestoreInstanceState(savedInstanceState);
//        mTvSerial.setText(savedInstanceState.getString(BUNDLEKEY_LOADTEXTVIEW));
//    }

    @Override
    public void onDestroy() {
        mSerial.close();
        mStop = true;
        getActivity().unregisterReceiver(mUsbReceiver);
        super.onDestroy();
    }

    private void mainloop() {
        mStop = false;
        mRunningMainLoop = true;
        startCollect.setEnabled(true);
        //btWriteFile.setEnabled(true);
        //etWrite.setEnabled(true);
        Toast.makeText(getActivity(), "connected", Toast.LENGTH_SHORT).show();
        if (SHOW_DEBUG) {
            Log.d(TAG, "start mainloop");
        }
        new Thread(mLoop).start();
    }

    private Runnable mLoop = new Runnable() {
        @Override
        public void run() {
            int len;
            byte[] rbuf = new byte[4096];

            for(;;) {// this is the main loop for transferring

                // ////////////////////////////////////////////////////////
                // Read and Display to Terminal
                // ////////////////////////////////////////////////////////
                len = mSerial.read(rbuf);
                rbuf[len] = 0;

                if (len > 0) {
                    if (SHOW_DEBUG) {
                        Log.d(TAG, "Read  Length : " + len);
                    }

                    switch (mDisplayType) {
                        case DISP_CHAR:
                            setSerialDataToTextView(mDisplayType, rbuf, len, "", "");
                            break;
                        case DISP_DEC:
                            setSerialDataToTextView(mDisplayType, rbuf, len, "013", "010");
                            break;
                        case DISP_HEX:
                            setSerialDataToTextView(mDisplayType, rbuf, len, "0d", "0a");
                            break;
                    }

                    mHandler.post(new Runnable() {
                        public void run() {
                            /*if (mTvSerial.length() > TEXT_MAX_SIZE) {
                                StringBuilder sb = new StringBuilder();
                                sb.append(mTvSerial.getText());
                                sb.delete(0, TEXT_MAX_SIZE / 2);
                                mTvSerial.setText(sb);
                            }
                            mTvSerial.append(mText);
                            if (mText.toString().length() > 1) {
                                Log.d(TAG, "Passed this");
                                Log.d(TAG, mText.toString());
                                Pattern pattern = Pattern.compile(".*,.*,.*");
                                Matcher matcher = pattern.matcher(mText.toString());
                                if (matcher.find()) {
                                    Log.d(TAG, "Should have found a device");
                                    String[] deviceInfo = matcher.group(0).toString().split(",");
                                    Log.d(TAG, deviceInfo[1]);
                                    currDeviceID = Integer.parseInt(deviceInfo[1]);
                                    int success = Integer.parseInt(deviceInfo[2].substring(0, 1));
                                    dataReceived[currDeviceID] = success;
                                    Log.d(TAG, Integer.toString(success));
                                    if (success == 0) {
                                        Log.d(TAG, "Should be writing a file");
                                        FileManager fm = new FileManager();
                                        String filename = deviceInfo[1] + ".txt";
                                        fm.writeToFile(filename, mTvSerial.getText().toString());
                                        mTvSerial.setText("");
                                    }
                                }
                            }
                            mText.setLength(0);
                            mSvText.fullScroll(ScrollView.FOCUS_DOWN);*/
                        }
                    });
                }

                try {
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                if (mStop) {
                    mRunningMainLoop = false;
                    return;
                }
            }
        }
    };

    private String IntToHex2(int Value) {
        char HEX2[] = {
                Character.forDigit((Value >> 4) & 0x0F, 16),
                Character.forDigit(Value & 0x0F, 16)
        };
        String Hex2Str = new String(HEX2);
        return Hex2Str;
    }

    boolean lastDataIs0x0D = false;

    void setSerialDataToTextView(int disp, byte[] rbuf, int len, String sCr, String sLf) {
        int tmpbuf;
        for (int i = 0; i < len; ++i) {
            if (SHOW_DEBUG) {
                //Log.d(TAG, "Read  Data[" + i + "] : " + rbuf[i]);
            }

            // "\r":CR(0x0D) "\n":LF(0x0A)
            if ((mReadLinefeedCode == LINEFEED_CODE_CR) && (rbuf[i] == 0x0D)) {
                mText.append(sCr);
                mText.append(BR);
            } else if ((mReadLinefeedCode == LINEFEED_CODE_LF) && (rbuf[i] == 0x0A)) {
                mText.append(sLf);
                mText.append(BR);
            } else if ((mReadLinefeedCode == LINEFEED_CODE_CRLF) && (rbuf[i] == 0x0D)
                    && (rbuf[i + 1] == 0x0A)) {
                mText.append(sCr);
                if (disp != DISP_CHAR) {
                    mText.append(" ");
                }
                mText.append(sLf);
                mText.append(BR);
                ++i;
            } else if ((mReadLinefeedCode == LINEFEED_CODE_CRLF) && (rbuf[i] == 0x0D)) {
                // case of rbuf[last] == 0x0D and rbuf[0] == 0x0A
                mText.append(sCr);
                lastDataIs0x0D = true;
            } else if (lastDataIs0x0D && (rbuf[0] == 0x0A)) {
                if (disp != DISP_CHAR) {
                    mText.append(" ");
                }
                mText.append(sLf);
                mText.append(BR);
                lastDataIs0x0D = false;





            } else if (lastDataIs0x0D && (i != 0)) {
                // only disable flag
                lastDataIs0x0D = false;
                --i;
            } else {
                switch (disp) {
                    case DISP_CHAR:
                        mText.append((char) rbuf[i]);
                        break;
                    case DISP_DEC:
                        tmpbuf = rbuf[i];
                        if (tmpbuf < 0) {
                            tmpbuf += 256;
                        }
                        mText.append(String.format("%1$03d", tmpbuf));
                        mText.append(" ");
                        break;
                    case DISP_HEX:
                        mText.append(IntToHex2((int) rbuf[i]));
                        mText.append(" ");
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void loadDefaultSettingValues() {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(getActivity());
        String res = pref.getString("display_list", Integer.toString(DISP_CHAR));
        mDisplayType = Integer.valueOf(res);

        res = pref.getString("fontsize_list", Integer.toString(12));
        mTextFontSize = Integer.valueOf(res);

        res = pref.getString("typeface_list", Integer.toString(3));
        switch(Integer.valueOf(res)){
            case 0:
                mTextTypeface = Typeface.DEFAULT;
                break;
            case 1:
                mTextTypeface = Typeface.SANS_SERIF;
                break;
            case 2:
                mTextTypeface = Typeface.SERIF;
                break;
            case 3:
                mTextTypeface = Typeface.MONOSPACE;
                break;
        }
        //mTvSerial.setTypeface(mTextTypeface);
        //etWrite.setTypeface(mTextTypeface);

        res = pref.getString("readlinefeedcode_list", Integer.toString(LINEFEED_CODE_CRLF));
        mReadLinefeedCode = Integer.valueOf(res);

        res = pref.getString("writelinefeedcode_list", Integer.toString(LINEFEED_CODE_CRLF));
        mWriteLinefeedCode = Integer.valueOf(res);

        res = pref.getString("email_edittext", "@gmail.com");
        mEmailAddress = res;

        res = pref.getString("baudrate_list", Integer.toString(57600));
        mBaudrate = Integer.valueOf(res);

        res = pref.getString("databits_list", Integer.toString(UartConfig.DATA_BITS8));
        mDataBits = Integer.valueOf(res);

        res = pref.getString("parity_list", Integer.toString(UartConfig.PARITY_NONE));
        mParity = Integer.valueOf(res);

        res = pref.getString("stopbits_list", Integer.toString(UartConfig.STOP_BITS1));
        mStopBits = Integer.valueOf(res);

        res = pref.getString("flowcontrol_list", Integer.toString(UartConfig.FLOW_CONTROL_OFF));
        mFlowControl = Integer.valueOf(res);
    }

    private void sendTextToEmail() {
        Intent intent =
                new Intent(Intent.ACTION_SENDTO, Uri.parse("mailto:"
                        + mEmailAddress));

        intent.putExtra("subject", "Result of " + getString(R.string.app_name));
        intent.putExtra("body", mTvSerial.getText().toString().trim());
        startActivity(intent);
    }

    // Load default baud rate
    int loadDefaultBaudrate() {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(getActivity());
        String res = pref.getString("baudrate_list", Integer.toString(9600));
        return Integer.valueOf(res);
    }

    private void openUsbSerial() {
        if(mSerial == null) {
            //Toast.makeText(getActivity(), "cannot open", Toast.LENGTH_SHORT).show();
            return;
        }

        if (!mSerial.isOpened()) {
            if (SHOW_DEBUG) {
                Log.d(TAG, "onNewIntent begin");
            }
            if (!mSerial.open()) {
                //Toast.makeText(getActivity(), "cannot open", Toast.LENGTH_SHORT).show();
                return;
            } else {
                loadDefaultSettingValues();

                boolean dtrOn=false;
                boolean rtsOn=false;
                if(mFlowControl == UartConfig.FLOW_CONTROL_ON) {
                    dtrOn = true;
                    rtsOn = true;
                }
                mSerial.setConfig(new UartConfig(mBaudrate, mDataBits, mStopBits, mParity, dtrOn, rtsOn));

                if(SHOW_DEBUG) {
                    Log.d(TAG, "setConfig : baud : "+mBaudrate+", DataBits : "+mDataBits+", StopBits : "+mStopBits+", Parity : "+mParity+", dtr : "+dtrOn+", rts : "+rtsOn);
                }

                //mTvSerial.setTextSize(mTextFontSize);

                Toast.makeText(getActivity(), "connected", Toast.LENGTH_SHORT).show();
            }
        }

        if (!mRunningMainLoop) {
            mainloop();
        }

    }

    private void closeUsbSerial() {
        detachedUi();
        mStop = true;
        mSerial.close();
    }

    protected void onNewIntent(Intent intent) {
        if (SHOW_DEBUG) {
            Log.d(TAG, "onNewIntent");
        }

        openUsbSerial();
    };

    private void detachedUi() {
        //etWrite.setEnabled(false);
        //Toast.makeText(getActivity(), "disconnect", Toast.LENGTH_SHORT).show();
    }

    // BroadcastReceiver when insert/remove the device USB plug into/from a USB
    // port
    BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
                if (SHOW_DEBUG) {
                    Log.d(TAG, "Device attached");
                }
                if (!mSerial.isOpened()) {
                    if (SHOW_DEBUG) {
                        Log.d(TAG, "Device attached begin");
                    }
                    openUsbSerial();
                }
                if (!mRunningMainLoop) {
                    if (SHOW_DEBUG) {
                        Log.d(TAG, "Device attached mainloop");
                    }
                    mainloop();
                }
            } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                if (SHOW_DEBUG) {
                    Log.d(TAG, "Device detached");
                }
                mStop = true;
                detachedUi();
//                mSerial.usbDetached(intent);
                mSerial.close();
            } else if (ACTION_USB_PERMISSION.equals(action)) {
                if (SHOW_DEBUG) {
                    Log.d(TAG, "Request permission");
                }
                synchronized (this) {
                    if (!mSerial.isOpened()) {
                        if (SHOW_DEBUG) {
                            Log.d(TAG, "Request permission begin");
                        }
                        openUsbSerial();
                    }
                }
                if (!mRunningMainLoop) {
                    if (SHOW_DEBUG) {
                        Log.d(TAG, "Request permission mainloop");
                    }
                    mainloop();
                }
            }
        }
    };


    /**
     * <p>Unescapes any Java literals found in the <code>String</code> to a
     * <code>Writer</code>.</p>
     *
     * <p>For example, it will turn a sequence of <code>'\'</code> and
     * <code>'n'</code> into a newline character, unless the <code>'\'</code>
     * is preceded by another <code>'\'</code>.</p>
     *
     * <p>A <code>null</code> string input has no effect.</p>
     *
     * @param str  the <code>String</code> to unescape, may be null
     * @throws IllegalArgumentException if the Writer is <code>null</code>
     * @throws IOException if error occurs on underlying Writer
     */
    private String unescapeJava(String str) throws IOException {
        if (str == null) {
            return "";
        }
        int sz = str.length();
        StringBuffer unicode = new StringBuffer(4);

        StringBuilder strout = new StringBuilder();
        boolean hadSlash = false;
        boolean inUnicode = false;
        for (int i = 0; i < sz; i++) {
            char ch = str.charAt(i);
            if (inUnicode) {
                // if in unicode, then we're reading unicode
                // values in somehow
                unicode.append(ch);
                if (unicode.length() == 4) {
                    // unicode now contains the four hex digits
                    // which represents our unicode character
                    try {
                        int value = Integer.parseInt(unicode.toString(), 16);
                        strout.append((char) value);
                        unicode.setLength(0);
                        inUnicode = false;
                        hadSlash = false;
                    } catch (NumberFormatException nfe) {
                        // throw new NestableRuntimeException("Unable to parse unicode value: " + unicode, nfe);
                        throw new IOException("Unable to parse unicode value: " + unicode, nfe);
                    }
                }
                continue;
            }
            if (hadSlash) {
                // handle an escaped value
                hadSlash = false;
                switch (ch) {
                    case '\\':
                        strout.append('\\');
                        break;
                    case '\'':
                        strout.append('\'');
                        break;
                    case '\"':
                        strout.append('"');
                        break;
                    case 'r':
                        strout.append('\r');
                        break;
                    case 'f':
                        strout.append('\f');
                        break;
                    case 't':
                        strout.append('\t');
                        break;
                    case 'n':
                        strout.append('\n');
                        break;
                    case 'b':
                        strout.append('\b');
                        break;
                    case 'u':
                    {
                        // uh-oh, we're in unicode country....
                        inUnicode = true;
                        break;
                    }
                    default :
                        strout.append(ch);
                        break;
                }
                continue;
            } else if (ch == '\\') {
                hadSlash = true;
                continue;
            }
            strout.append(ch);
        }
        if (hadSlash) {
            // then we're in the weird case of a \ at the end of the
            // string, let's output it anyway.
            strout.append('\\');
        }
        return new String(strout.toString());
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        ((MainActivity) activity).onSectionAttached(
                getArguments().getInt(MainActivity.ARG_SECTION_NUMBER));
    }

}
