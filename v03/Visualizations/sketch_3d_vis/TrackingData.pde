class TrackingData {
  Table data;
  int rssi;
  int count;
  boolean showname = false;
  String names = "";
  float x, y, z;
  
  TrackingData()
  {
    // prepare the table
    data = new Table();
    data.addColumn("time", Table.INT);
    data.addColumn("id", Table.INT);
    data.addColumn("rssi", Table.INT);
  }
  
  void loadDataFile(String filename)
  {
    String[] lines = loadStrings(filename);
    for(int i = 0; i < lines.length; i++)
    {
      String[] line = split(lines[i],',');
      if(line.length == 3)
      {
        //if(int(line[2]) != -128)
        //{
          TableRow newRow = data.addRow();
          newRow.setInt("time", int(line[0]));
          newRow.setInt("id", int(line[1]));
          newRow.setInt("rssi", int(line[2]));
        //}
      }
    }
    println(filename + ", " + str(data.getRowCount()) + " rows");
  }
  
  void setTextCoord(float _x, float _y, float _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }
}
