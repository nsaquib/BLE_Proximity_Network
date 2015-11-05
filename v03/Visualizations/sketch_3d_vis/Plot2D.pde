class Plot2D {
  ArrayList<Points> poop;
  float x, y;
  int w, h; 
  String name;
  
  Plot2D(float _x, float _y, int _w, int _h, String _name)
  {
    poop  = new ArrayList();
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    name = _name;
  }
  
  void addPoint(float cy)
  {
    float yy = map(cy, 0, 5, y, y - h);
    Points P = new Points(x + w, yy);
    poop.add(P);
  }
  
  void draw(int strokecol)
  {
    noFill();
    stroke(strokecol);
    fill(strokecol);
    text(name, x - w, y);
    noFill();
    beginShape();
    for (int i=0;i<poop.size();i++) {
      Points P = (Points)poop.get(i);
      vertex(P.x, P.y);
      if (P.x < x)poop.remove(i);
      P.x--;
    }
    endShape();
    noStroke();
  }
}

class Points {
  float x, y;
  Points(float x, float y) {
    this.x = x;
    this.y = y;
  }
}

