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

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.LegendRenderer;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.BarGraphSeries;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview_demos.MainActivity;
import com.jjoe64.graphview_demos.R;

/**
 * Created by jonas on 28.08.14.
 */
public class BatteryLife extends Fragment {
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_battery_graph, container, false);

        GraphView graph = (GraphView) rootView.findViewById(R.id.graph);
        BarGraphSeries<DataPoint> series = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(1, 5),
        });
        series.setColor(Color.rgb(158, 1, 66));
        series.setSpacing(0);
        graph.addSeries(series);

        BarGraphSeries<DataPoint> series2 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(2,4)
        });
        series2.setColor(Color.rgb(213, 62, 79));
        series2.setSpacing(0);
        graph.addSeries(series2);

        BarGraphSeries<DataPoint> series3 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(3,5)
        });
        series3.setColor(Color.rgb(244, 109, 67));
        series3.setSpacing(0);
        graph.addSeries(series3);


        BarGraphSeries<DataPoint> series4 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(4,4)
        });
        series4.setColor(Color.rgb(253, 174, 97));
        series4.setSpacing(0);
        graph.addSeries(series4);


        BarGraphSeries<DataPoint> series5 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(5,4)
        });
        series5.setColor(Color.rgb(254, 224, 139));
        series5.setSpacing(0);
        graph.addSeries(series5);

        BarGraphSeries<DataPoint> series6 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(6,3)
        });
        series6.setColor(Color.rgb(230, 245, 152));
        series6.setSpacing(0);
        graph.addSeries(series6);

        BarGraphSeries<DataPoint> series7 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(7,4)
        });
        series7.setColor(Color.rgb(171, 221, 164));
        series7.setSpacing(0);
        graph.addSeries(series7);

        BarGraphSeries<DataPoint> series8 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(8,5)
        });
        series8.setColor(Color.rgb(102, 194, 165));
        series8.setSpacing(0);
        graph.addSeries(series8);

        BarGraphSeries<DataPoint> series9 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(9,5)
        });
        series9.setColor(Color.rgb(0, 136, 189));
        series9.setSpacing(0);
        graph.addSeries(series9);

        BarGraphSeries<DataPoint> series10 = new BarGraphSeries<DataPoint>(new DataPoint[] {
                new DataPoint(10,3)
        });
        series10.setColor(Color.rgb(94, 79, 162));
        series10.setSpacing(20);
        graph.addSeries(series10);

        graph.getViewport().setXAxisBoundsManual(true);
        graph.getViewport().setMinX(0);
        graph.getViewport().setMaxX(16);
        graph.getViewport().setYAxisBoundsManual(true);
        graph.getViewport().setMinY(0);
        graph.getViewport().setMaxY(5);
        StaticLabelsFormatter staticLabelsFormatter = new StaticLabelsFormatter(graph);
        staticLabelsFormatter.setHorizontalLabels(new String[]{"", ""});
        staticLabelsFormatter.setVerticalLabels(new String[]{"low", "med", "high"});
        graph.getGridLabelRenderer().setLabelFormatter(staticLabelsFormatter);


        // legend
        series.setTitle("math");
        series2.setTitle("language");
        series3.setTitle("life");
        series4.setTitle("sensorial");
        series5.setTitle("letters");
        series6.setTitle("blocks");
        series7.setTitle("washing");
        series8.setTitle("buttons");
        series9.setTitle("tea");
        series10.setTitle("snack");
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
