import processing.video.*;
Movie myMovie;

int ff = 0;

void setup() {
  size(720, 525,P2D);
  frameRate(59.95);

  textFont(createFont("Monospace",12));

  myMovie = new Movie(this, "/tmp/test.mpg");
  myMovie.loop();
}

void draw() {
  //tint(255, 20);
  background(0);
  image(myMovie, 0, 0,width,height);
  fill(255,255,0);
  text(myMovie.time(),50,50);
  
  if(mousePressed)
  myMovie.jump(map(mouseX,0,width,0,myMovie.duration()));

}

// Called every time a new frame is available to read
void movieEvent(Movie m) {
  m.read();

  //if(ff%10==0 && ff!=0)
  //myMovie.jump(100);

  ff++;
}


