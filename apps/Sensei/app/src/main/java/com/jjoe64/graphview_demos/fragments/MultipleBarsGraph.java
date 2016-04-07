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
import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GridLabelRenderer;
import com.jjoe64.graphview.LegendRenderer;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.BarGraphSeries;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.DataPointInterface;
import com.jjoe64.graphview.series.OnDataPointTapListener;
import com.jjoe64.graphview.series.Series;
import com.jjoe64.graphview_demos.MainActivity;
import com.jjoe64.graphview_demos.R;

public class MultipleBarsGraph extends Fragment {
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_main, container, false);

        GraphView graph = (GraphView) rootView.findViewById(R.id.graph);
        BarGraphSeries<DataPoint> series = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(0, 0.2),
                new DataPoint(1, 2),
                new DataPoint(2, 1.1),
                new DataPoint(3, 1.3),
                new DataPoint(4, 1.5),
                new DataPoint(5, 1.7),
                new DataPoint(6, 0.7),
                new DataPoint(7, 0.8),
                new DataPoint(8, 0.5),
                new DataPoint(9, 1)
        });
        series.setColor(Color.rgb(212, 161, 106)); // brown
        series.setSpacing(30);
        graph.addSeries(series);

        BarGraphSeries<DataPoint> series2 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(0, 1),
                new DataPoint(1, 1.9),
                new DataPoint(2, 0.9),
                new DataPoint(3, 1.7),
                new DataPoint(4, 0.6),
                new DataPoint(5, 0.9),
                new DataPoint(6, 0.3),
                new DataPoint(7, 0.9),
                new DataPoint(8, 1.3),
                new DataPoint(9, 1.4)
        });
        series2.setColor(Color.rgb(85, 44, 0)); // dark brown
        series2.setSpacing(30);
        graph.addSeries(series2);

        graph.getViewport().setXAxisBoundsManual(true);
        graph.getViewport().setYAxisBoundsManual(true);
        graph.getViewport().setMinX(-0.5);
        graph.getViewport().setMaxX(10);
        graph.getViewport().setMinY(0);
        graph.getViewport().setMaxY(3);
        graph.setTitle("Time Spent with Children");
        graph.setTitleTextSize(48);

        StaticLabelsFormatter staticLabelsFormatter = new StaticLabelsFormatter(graph);
        final String[] xLabels = new String[]{"Alice", "Bob", "Chris", "Dave", "Eliza", "Fred", "George", "Helen", "Isaac", "Jack"};
        staticLabelsFormatter.setHorizontalLabels(xLabels);
        staticLabelsFormatter.setVerticalLabels(new String[]{"0", "1", "2", "3"});
        graph.getGridLabelRenderer().setLabelFormatter(staticLabelsFormatter);

        GridLabelRenderer gridLabel = graph.getGridLabelRenderer();
        gridLabel.setHorizontalAxisTitle("Students");
        gridLabel.setVerticalAxisTitle("Hours");

        // Tap Listeners
        series.setOnDataPointTapListener(new OnDataPointTapListener() {
            @Override
            public void onTap(Series series, DataPointInterface dataPoint) {
                Toast.makeText(getActivity(), series.getTitle() + " - " + xLabels[(int) dataPoint.getX()] + " : " + dataPoint.getY() + ((dataPoint.getY() <= 1 && dataPoint.getY() != 0) ? " hour" : " hours"), Toast.LENGTH_SHORT).show();
            }
        });

        series2.setOnDataPointTapListener(new OnDataPointTapListener() {
            @Override
            public void onTap(Series series, DataPointInterface dataPoint) {
                Toast.makeText(getActivity(), series.getTitle() + " - " + xLabels[(int) dataPoint.getX()] + " : " + dataPoint.getY() + ((dataPoint.getY() <= 1 && dataPoint.getY() != 0) ? " hour" : " hours"), Toast.LENGTH_SHORT).show();
            }
        });

        // Legend
        series.setTitle("Alyssa");
        series2.setTitle("Ben");
        graph.getLegendRenderer().setVisible(true);
        graph.getLegendRenderer().setAlign(LegendRenderer.LegendAlign.TOP);

        return rootView;
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        ((MainActivity) activity).onSectionAttached(
                getArguments().getInt(MainActivity.ARG_SECTION_NUMBER));
    }

}


///**
// * This file is licensed under MIT
// *
// * The MIT License (MIT)
// *
// * Copyright (c) 2014 Jonas Gehring
// *
// * Permission is hereby granted, free of charge, to any person obtaining a copy
// * of this software and associated documentation files (the "Software"), to deal
// * in the Software without restriction, including without limitation the rights
// * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// * copies of the Software, and to permit persons to whom the Software is
// * furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice shall be included in
// * all copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// * THE SOFTWARE.
// */
//package com.jjoe64.graphview_demos.fragments;
//
//import android.app.Activity;
//import android.graphics.Color;
//import android.os.Bundle;
//import android.os.Environment;
//import android.support.v4.app.Fragment;
//import android.util.Log;
//import android.view.LayoutInflater;
//import android.view.View;
//import android.view.ViewGroup;
//
//import com.jjoe64.graphview.GraphView;
//import com.jjoe64.graphview.LegendRenderer;
//import com.jjoe64.graphview.series.BarGraphSeries;
//import com.jjoe64.graphview.series.DataPoint;
//import com.jjoe64.graphview_demos.MainActivity;
//import com.jjoe64.graphview_demos.R;
//
//import java.io.BufferedReader;
//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileNotFoundException;
//import java.io.IOException;
//import java.io.InputStreamReader;
//
//public class MultipleBarsGraph extends Fragment {
//    @Override
//    public View onCreateView(LayoutInflater inflater, ViewGroup container,
//                             Bundle savedInstanceState) {
//        View rootView = inflater.inflate(R.layout.fragment_main, container, false);
//
//        GraphView graph = (GraphView) rootView.findViewById(R.id.graph);
//
//        // Teacher 1
//        DataPoint[] dataTeacher1 = getTeacherTimeSpentData(4); // use correct id
//        BarGraphSeries<DataPoint> seriesTeacher1 = new BarGraphSeries<DataPoint>(dataTeacher1);
//        seriesTeacher1.setColor(Color.RED);
//        seriesTeacher1.setSpacing(30);
//        graph.addSeries(seriesTeacher1);
//
//        // Teacher 2
//        DataPoint[] dataTeacher2 = getTeacherTimeSpentData(4);
//        BarGraphSeries<DataPoint> seriesTeacher2 = new BarGraphSeries<DataPoint>(dataTeacher2);
//        seriesTeacher2.setColor(Color.BLUE);
//        seriesTeacher2.setSpacing(30);
//        graph.addSeries(seriesTeacher2);
//
//        graph.getViewport().setXAxisBoundsManual(true);
//        graph.getViewport().setMinX(-1);
//        graph.getViewport().setMaxX(15);
//
//        // legend
//        seriesTeacher1.setTitle("Erin");
//        seriesTeacher2.setTitle("Mary");
//        graph.getLegendRenderer().setVisible(true);
//        graph.getLegendRenderer().setAlign(LegendRenderer.LegendAlign.TOP);
//
//        return rootView;
//    }
//
//    @Override
//    public void onAttach(Activity activity) {
//        super.onAttach(activity);
//        ((MainActivity) activity).onSectionAttached(getArguments().getInt(MainActivity.ARG_SECTION_NUMBER));
//    }
//
//    DataPoint[] getTeacherTimeSpentData(int id) {
//        DataPoint[] dataPoints = new DataPoint[16];
//        FileInputStream is;
//        BufferedReader reader;
//        int deviceID;
//        long value;
//        int[] timeSpent = new int[16];
//
//        File sdcard = Environment.getExternalStorageDirectory();
//        File file = new File(sdcard, "/Wildflower/" + id + ".txt");
//        Log.d("DEBUG", "/Wildflower/" + id + ".txt");
//        if (file.exists()) {
//            Log.d("DEBUG", "File exists");
//            try {
//                is = new FileInputStream(file);
//                reader = new BufferedReader(new InputStreamReader(is));
//                String line;
//                try {
//                    line = reader.readLine();
//                    Log.d("DEBUG", line);
//                    while (line != null) {
//                        if (line.split("\t").length == 4) {
//                            String data = line.split("\t")[3];  // Data is the third index in each string
//                            value = Long.parseLong(data);
//                            if (value > -1) {
//                                deviceID = (int) value / 100000000;
//                                timeSpent[deviceID] += 10;      // Increment time spent by 10 seconds
//                                Log.d("TAG", String.valueOf(deviceID));
//                            }
//                        }
//                        line = reader.readLine();
//                    }
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//            } catch (FileNotFoundException e) {
//                e.printStackTrace();
//            }
//            return dataPoints;
//        }
//        Log.d("DEBUG", "Parsed file");
//        for (int i = 0; i < dataPoints.length; i++) {
////            dataPoints[i] = new DataPoint(i, timeSpent[i] / 60);
//            dataPoints[i] = new DataPoint(i, timeSpent[i] / 60);
//            Log.d("DEBUG", dataPoints[i].toString());
//        }
//        return dataPoints;
//    }
//}
