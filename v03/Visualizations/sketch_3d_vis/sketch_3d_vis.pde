import saito.objloader.*;

OBJModel model ;
float rotX, rotY;

int fps = 5; 

//Table data;
TrackingData[] shelf = new TrackingData[6];

int timec = -1;

StringList stnames;

Plot2D[] p = new Plot2D[6];

void setup()
{
    size(1000, 600, P3D);
    frameRate(fps);
    model = new OBJModel(this, "wildflower mock.obj", "absolute", POLYGON);
    model.enableDebug();

    model.scale(0.8);
    model.translateToCenter();
    //model.shapeMode(POLYGON);

    stroke(255);
    noStroke();
    
    //set up arrays of data tables
    for (int i = 0; i < 6; i++)
    {
      shelf[i] = new TrackingData();
      shelf[i].loadDataFile("data" + i + ".txt");
      shelf[i].count = 0;
      
      p[i] = new Plot2D(100 + i * 150, 30, 60, 25, "shelf" + str(i));
    }
    
    //text coordinates
    shelf[0].setTextCoord(-100, -100, -100);
    shelf[1].setTextCoord(-100, -100, 100);
    shelf[2].setTextCoord(-200, -100, -100);
    shelf[3].setTextCoord(-250, -100, 200);
    shelf[4].setTextCoord(-400, -100, 200);
    shelf[5].setTextCoord(300, -100, 100);
    
    //count0 = count1 = count2 = count3 = count4 = count5 = 0;
    
    textSize(24);
    
    stnames = new StringList();
    stnames.append("dash"); stnames.append("uriah"); stnames.append("dario");
    stnames.append("eliana"); stnames.append("beatrice"); stnames.append("eleanor");
    stnames.append("riella"); stnames.append("elliot");
    
    // graph vis    
    //p0 = new Plot2D(100, 30, 100, 50, "shelf0");
    
}

void draw()
{
    background(129);
    for(int j = 0; j < 6; j++)
    {
      shelf[j].count = 0;
      shelf[j].names = "";
    }
    // load each time set (8 rows) and visualize the rssi strength
    if (timec >= shelf[0].data.getRowCount()) {timec = -1;}
    //int[] rssi = new int[8]
    for (int i = 0; i < 8; i++)
    {
      for(int j = 0; j < 6; j++)
      {
        shelf[j].rssi = abs(shelf[j].data.getRow(timec + i + 1).getInt("rssi"));
      //fill(map(rssi,60, 128, 0, 255), 0, 0);
      //text(stnames.get(i), i*100 + 5, 40);
        if((shelf[j].rssi) > 60 && (shelf[j].rssi <= 90))
        {
          //shelf0.showname = true;
          shelf[j].names = shelf[j].names + stnames.get(shelf[j].data.getRow(timec + i + 1).getInt("id")) + ", ";
          shelf[j].count++;
        }
      }
    }
    timec += 8;
    textSize(18);
    for(int j = 0; j < 6; j++)
    {
      p[j].addPoint(shelf[j].count);
      p[j].draw(255); 
    }
    noStroke();
    lights();
    noStroke();
    
    pushMatrix();
    translate(width/2, height/2, 0);
    rotateX(rotY);
    rotateY(rotX);
    directionalLight(0, 0, -100, 0, -1, 0);
    pointLight(255, 255, 255, 0, -200, 0);
    model.draw();
    //sphere(100);
    //textSize(14);
    
    fill(0);
    for(int j = 0; j < 6; j++)
    {
      text(shelf[j].names, shelf[j].x, shelf[j].y, shelf[j].z);
    }
    noFill();
    
    //text(shelf.names, 0, 0, 0);
    textSize(24);
    popMatrix();
    
    //graph vis
    //g.draw();
    //p0.draw(255);
    
}

boolean bTexture = true;
boolean bStroke = false;

void keyPressed()
{
    if(key == 't') {
        if(!bTexture) {
            model.enableTexture();
            bTexture = true;
        } 
        else {
            model.disableTexture();
            bTexture = false;
        }
    }

    if(key == 's') {
        if(!bStroke) {
            stroke(255);
            bStroke = true;
        } 
        else {
            noStroke();
            bStroke = false;
        }
    }

    else if(key=='1')
        model.shapeMode(POINTS);
    else if(key=='2')
        model.shapeMode(POLYGON);
    else if(key=='3')
        model.shapeMode(TRIANGLES);
}

void mouseDragged()
{
    rotX += (mouseX - pmouseX) * 0.01;
    rotY -= (mouseY - pmouseY) * 0.01;
}



