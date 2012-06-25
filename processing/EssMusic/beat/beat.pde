import ddf.minim.*;
import ddf.minim.analysis.*;

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

AudioPlayer song;
BeatDetect beat;
BeatListener bl;

float kickSize, snareSize, hatSize;

void setup()
{
  size(512, 200);
  smooth();
  Minim.start(this);
  song = Minim.loadFile("song.mp3", 1024);
  song.play();
  // a beat detection object that is FREQ_ENERGY mode that
  // expects buffers the length of song's buffer size
  // and samples captured at songs's sample rate
  beat = new BeatDetect(song.bufferSize(), song.sampleRate());
  beat.setSensitivity(300);
  kickSize = snareSize = hatSize = 16;
  // make a new beat listener, so that we won't miss any buffers for the analysis
  bl = new BeatListener(beat, song);
}

void draw()
{
background(0);
fill(255);
if ( beat.isKick() ) kickSize = 32;
if ( beat.isSnare() ) snareSize = 32;
if ( beat.isHat() ) hatSize = 32;
stroke(255);
strokeWeight(kickSize);
rect(width/4, height/2, 20, 20);
stroke(255);
strokeWeight(snareSize);
rect(width/2, height/2, 20, 20);
stroke(255);
strokeWeight(hatSize);
rect(3*width/4, height/2, 20, 20);
kickSize = constrain(kickSize * 0.95, 16, 32);
snareSize = constrain(snareSize * 0.95, 16, 32);
hatSize = constrain(hatSize * 0.95, 16, 32);
}

void stop()
{
// always close Minim audio classes when you are finished with them
song.close();
// always stop Minim before exiting
Minim.stop();

// this closes the sketch
super.stop();
}

