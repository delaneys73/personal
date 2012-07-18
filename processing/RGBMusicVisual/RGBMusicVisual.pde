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

Minim minim;
AudioPlayer ftf;
AudioInput input;

int numlines = 16;
color from = color(255, 0, 0);
color to = color(0,255,0);

int sampleRangeFrom = 256;
int sampleRangeTo = 768;
int timingResolution = 100;
int colourFade = 50;
double colour = 0;
double dir = 1;
boolean stereo = false;
boolean beatDetect = false;
long last = 0;
long lastcol = 0;
 
void setup()
{
   colorMode(RGB,255);
   size(1024, 600, OPENGL);
   minim = new Minim(this);
  String fileName = "Wizards in Winter.mp3";  
   ftf = minim.loadFile(fileName, 1024);
   ftf.loop();
   input = minim.getLineIn();

   beat = new BeatDetect(ftf.bufferSize(), ftf.sampleRate());
   beat.setSensitivity(timingResolution);

   // make a new beat listener, so that we won't miss any buffers for the analysis
   bl = new BeatListener(beat, ftf);

}

void calcLights(float f,float[] lightsOn,int lightsPerChannel)
{
    float normVal = 0;
    normVal = (f*50) + 50;
      
    float inc = 100 / lightsPerChannel;
    
    for (int j=0;j < lightsPerChannel;j++)
    {
       float level = (j*inc);
       if (normVal > level && normVal <= (j+1)*inc)
       {
         
//         println(j+" - "+level+" - "+normVal+" - "+inc);
         lightsOn[j] = (normVal - level) / inc;
       }
     
    }
  
}

void draw()
{
  background(6);
  stroke(255);

  text(""+sampleRangeFrom+","+sampleRangeTo,0,0);
  long now = millis();
  if ((now+colourFade) > lastcol)
  {
    lastcol = now;
    colour+=dir;
    if (colour>255)
    {
      colour = 255;
      dir = -1;
    }
    
    if (colour < 0)
    {
       colour = 0;
        dir = 1; 
    }
    
  }
  if ((now+timingResolution) > last)
  {
    last = now;
    // we draw the waveform by connecting neighbor values with a line
    // we multiply each of the values by 50
    // because the values in the buffers are normalized
    // this means that they have values between -1 and 1.
    // If we don't scale them up our waveform
    // will look more or less like a straight line.
    int lightsPerChannel = 5;
    float[] lightsOn = new float[5];
 
    int start = 0;
    int end = ftf.bufferSize()-1;
     
    for (int i = start;i < end;i++)
    {
        float avgLevel = (ftf.left.get(i) + ftf.right.get(i))/2;
        calcLights(avgLevel,lightsOn,5);
    }
      
    int barheight = 50;
    int w = 150;
    int y = 280;
    for (int k=0;k < lightsPerChannel;k++)
    {
        int x = (k*(w+55))+27;

        color colorFill = GetColour(colour,0, 255);
        fill(colorFill,lightsOn[k]*255);
        
        if (lightsOn[k]>0)
        {
          visualise(x,y,w,barheight);
        } 
    }
  }
}

color GetColour(double v,double vmin,double vmax)
{
   double dv;

  double r = 1.0;
  double g = 1.0;
  double b = 1.0;
    
   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      r = 0;
      g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      r = 0;
      b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      r = 4 * (v - vmin - 0.5 * dv) / dv;
      b = 0;
   } else {
      g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      b = 0;
   }

   return color((int)(r*255),(int)(g*255),(int)(b*255));
}

void visualise(int x,int y,int w,int h)
{
   rect(x,y,w,h);
   //quad(x,y,x+w,y,x+w-h,y+h,x+h,y+h);
}

