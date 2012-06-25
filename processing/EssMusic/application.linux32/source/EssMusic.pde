import processing.opengl.*;
import ddf.minim.signals.*;
import ddf.minim.*;
import ddf.minim.analysis.*;
import ddf.minim.effects.*;

class BeatListener implements AudioListener
{
  private BeatDetect beat;
  private AudioPlayer source;
  
  BeatListener(BeatDetect beat, AudioPlayer source)
  {
  this.source = source;
  this.source.addListener(this);
  this.beat = beat;
  }
  
  void samples(float[] samps)
  {
  beat.detect(source.mix);
  }
  
  void samples(float[] sampsL, float[] sampsR)
  {
  beat.detect(source.mix);
  }
}

BeatDetect beat;
BeatListener bl;

float kickSize, snareSize, hatSize;

Minim minim;
AudioPlayer ftf;
AudioInput input;

int numlines = 32;
color from = color(255, 0, 0);
color to = color(255, 102, 153);
int timingResolution = 50;
boolean stereo = false;
long last = 0;
 
void setup()
{
  colorMode(RGB,255);
  size(500, 500, OPENGL);
   minim = new Minim(this);
   minim.debugOn();
//   String fileName = "e:/music/Santa-Clause-is-coming-to-town.mp3";
// String fileName = "//mss500gb/music/xmas/Dance of the sugar plum faries.mp3";
   String fileName = "Dance of the sugar plum faries.mp3";
//  String fileName = "//10.0.0.105/music/xmas/Black Eyed Peas - I Gotta Feeling.mp3";
 // String fileName = "//mss500gb/music/xmas/Queen - We Will Rock You 2011 Remaster.mp3";
 // String fileName = "//mss500gb/music/xmas/Jingle-bells.mp3";
   ftf = minim.loadFile(fileName, 1024);
   ftf.loop();
   input = minim.getLineIn();
  beat = new BeatDetect(ftf.bufferSize(), ftf.sampleRate());
beat.setSensitivity(300);
kickSize = snareSize = hatSize = 16;
// make a new beat listener, so that we won't miss any buffers for the analysis
bl = new BeatListener(beat, ftf);

}

void calcLights(float f,boolean[] lightsOn)
{
  float normVal = 0;
    normVal = (f*50) + 50;
      
    float inc = 100 / numlines;
    
    for (int j=0;j < numlines;j++)
    {
       if (normVal > j*inc && normVal <= (j+1)*inc)
       {
         lightsOn[j] = true;
       }
     
    }
  
}

void draw()
{
  long now = millis();
  if ((now+timingResolution) > last)
  {
    last = now;
  background(6);
  stroke(255);
  // we draw the waveform by connecting neighbor values with a line
  // we multiply each of the values by 50
  // because the values in the buffers are normalized
  // this means that they have values between -1 and 1.
  // If we don't scale them up our waveform
  // will look more or less like a straight line.
  
 
  boolean[] lightsOnLeft=new boolean[numlines];
    boolean[] lightsOnRight=new boolean[numlines];
  
  for (int a=0;a<numlines;a++)
  {
     lightsOnLeft[a] = false; 
     lightsOnRight[a] = false; 
  }
 
  for (int i = 0;i < ftf.bufferSize()-1;i++)
  {
    if (stereo)
    {
      calcLights(ftf.left.get(i),lightsOnLeft);
    }
    else
    {
      float avgLevel = (ftf.left.get(i) + ftf.right.get(i))/2;
      calcLights(avgLevel,lightsOnLeft);
    }
    calcLights(ftf.right.get(i),lightsOnRight);
   // line(i, 50 + ftf.left.get(i)*50, i+1, 50 + ftf.left.get(i+1)*50);
    }
    
    int barspace = (height / numlines)/2;
    int w = (width/2)-(2*barspace);
      
    for (int k=0;k < numlines;k++)
    {
      color colorFill = lerpColor(from, to, k+1/numlines);
      fill(colorFill);
      int y = (k*(barspace*2));
    
      if (!stereo)
      {
         w = width - (2*barspace);
      }
      else
      {
        if (lightsOnRight[k]==true)
        {
          visualise((width/2)+barspace,y,w,barspace);
        }  
      }
      
      if (lightsOnLeft[k]==true)
      {
        visualise(barspace,y,w,barspace);
      } 

       
      
    }
    if (beat.isKick()||beat.isSnare()||beat.isHat())
     { 
       fill(0,0,255);
       rect(barspace,height-20,width-(barspace*2),20);
     } 
  }
}

void visualise(int x,int y,int w,int h)
{
   //rect(x,y,w,h);
   quad(x,y,x+w,y,x+w-h,y+h,x+h,y+h);
}

