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
color fromL = color(255, 0, 0);
color toL = color(255, 128, 128);

color fromR = color(0, 0, 255);
color toR = color(80, 102, 153);

int sampleRangeFrom = 256;
int sampleRangeTo = 768;
int timingResolution = 100;
boolean stereo = false;
boolean beatDetect = false;
long last = 0;
 
void setup()
{
   colorMode(RGB,255);
   size(1024, 600, OPENGL);
   minim = new Minim(this);
//   String fileName = "e:/music/Santa-Clause-is-coming-to-town.mp3";
// String fileName = "//mss500gb/music/xmas/Dance of the sugar plum faries.mp3";
  // String fileName = "e:/music/Katy Perry - Firework.mp3";
 // String fileName = "/home/stephen/Music/Katy Perry - Firework.mp3";
  String fileName = "Wizards in Winter.mp3";  
 // String fileName = "//mss500gb/music/xmas/Queen - We Will Rock You 2011 Remaster.mp3";
 // String fileName = "//mss500gb/music/xmas/Jingle-bells.mp3";
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

void keyTyped()
{
   if (key==' ')
   {
       stereo = !stereo; 
   } 
   
   if (key=='\t')
   {
      beatDetect = ! beatDetect; 
   }
   
   if (key=='+' || key=='=')
   {
      numlines++; 
   }
   
   if (key=='-' || key=='_')
   {
      numlines--; 
   }
   
   if (key=='q')
   {
     exit();    
   }
   
   if (key==',')
   {
      if (sampleRangeFrom - 64 < 0)
      {
        sampleRangeFrom = 0; 
      }
      else
      {
        sampleRangeFrom -= 64;
      }
   }
   
   if (key=='.')
   {
      if (sampleRangeFrom + 64 > 1023)
      {
        sampleRangeFrom = 1023; 
      }
      else
      {
        sampleRangeFrom += 64;
      }
   }
   
   if (key=='[')
   {
      if (sampleRangeTo - 64 < 0)
      {
        sampleRangeTo = 0; 
      }
      else
      {
        sampleRangeTo -= 64;
      }
   }
   
   if (key=='.')
   {
      if (sampleRangeTo + 64 > 1023)
      {
        sampleRangeTo = 1023; 
      }
      else
      {
        sampleRangeTo += 64;
      }
   }
}

void draw()
{
  background(6);
  stroke(255);

  text(""+sampleRangeFrom+","+sampleRangeTo,0,0);
  long now = millis();
  if ((now+timingResolution) > last)
  {
    last = now;
    // we draw the waveform by connecting neighbor values with a line
    // we multiply each of the values by 50
    // because the values in the buffers are normalized
    // this means that they have values between -1 and 1.
    // If we don't scale them up our waveform
    // will look more or less like a straight line.
    int lightsPerChannel = numlines;
    if (stereo)
    {
      lightsPerChannel = numlines / 2;
    }  
   
    float[] lightsOnLeft=new float[lightsPerChannel];
    float[] lightsOnRight=new float[lightsPerChannel];
    
    for (int a=0;a<lightsPerChannel;a++)
    {
       lightsOnLeft[a] = 0; 
       lightsOnRight[a] = 0; 
    }
 
    int start = 0;
    int end = ftf.bufferSize()-1;
    
    if (sampleRangeFrom < end)
    {
       start = sampleRangeFrom; 
    }
 
 
    if (sampleRangeTo <= end)
    {
      end = sampleRangeTo; 
    }
 
    for (int i = start;i < end;i++)
    {
      if (stereo)
      {
        calcLights(ftf.left.get(i),lightsOnLeft,lightsPerChannel);
      }
      else
      {
        float avgLevel = (ftf.left.get(i) + ftf.right.get(i))/2;
        calcLights(avgLevel,lightsOnLeft,lightsPerChannel);
      }
      calcLights(ftf.right.get(i),lightsOnRight,lightsPerChannel);
    }
      
    int barspace = (height / lightsPerChannel)/2;
    int w = (width/2)-(2*barspace);
        
    for (int k=0;k < lightsPerChannel;k++)
    {
        int y = (k*(barspace*2));
      
        if (!stereo)
        {
           w = width - (2*barspace);
        }
        else
        {
          color colorFill = lerpColor(fromR, toR, k+1/lightsPerChannel);
          fill(colorFill,lightsOnRight[k]*255);
          if (lightsOnRight[k]>0)
          {
            visualise((width/2)+barspace,y,w,barspace);
          }  
        }
  
        color colorFill = lerpColor(fromL, toL, k+1/lightsPerChannel);
        fill(colorFill,lightsOnLeft[k]*255);
        
        if (lightsOnLeft[k]>0)
        {
          visualise(barspace,y,w,barspace);
        } 
  
         
        
      }
      if (beatDetect)
      {
      
       if (beat.isKick()||beat.isSnare()||beat.isHat())
       { 
         fill(0,0,255);
         rect(barspace,height-20,width-(barspace*2),20);
       } 
       
      }
    }
}

void visualise(int x,int y,int w,int h)
{
   //rect(x,y,w,h);
   quad(x,y,x+w,y,x+w-h,y+h,x+h,y+h);
}

