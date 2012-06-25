package org.delaney.xmas.music.minim;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

import org.delaney.xmas.comms.CommSender;
import org.delaney.xmas.comms.SerialPortSender;
import org.delaney.xmas.music.MusicPlayer;
import org.springframework.beans.factory.InitializingBean;

import ddf.minim.AudioInput;
import ddf.minim.AudioPlayer;
import ddf.minim.Minim;
import ddf.minim.analysis.BeatDetect;

public class MinimMusicPlayer implements MusicPlayer,InitializingBean {

	private boolean playing = false;
	private boolean beatDetect = false;
	private BeatDetect beat;
	private BeatListener bl;

	private CommSender sender;
	
	private Minim minim;
	private AudioPlayer ftf;
	private AudioInput input;
	private Timer timer;

	private int lightFrom;
	private int lightTo;
	private int timingResolution = 100;
	private boolean stereo = false;

	private int beatLight;
	private long ellapsed = 0l;
	private TimerTask timerTask;
	private FileInputStream currentStream;
	private int numlines;

	public MinimMusicPlayer() {
		setup();
	}

	@Override
	public void afterPropertiesSet() throws Exception {
		numlines = lightTo - lightFrom +1;		
	}
	
	@Override
	public boolean getPlaying() {
		return playing;
	}

	@Override
	public void play(String filename) {
		sender.setModeManual();
		
		playing = true;
		
		ftf = minim.loadFile(filename, 1024);
		ftf.play();
		
		
		input = minim.getLineIn();
		if (beatDetect)
		{
			beat = new BeatDetect(ftf.bufferSize(), ftf.sampleRate());
			//TODO See if this should match up with timing resolution
			beat.setSensitivity(timingResolution);
			// make a new beat listener, so that we won't miss any buffers for the
			// analysis
			bl = new BeatListener(beat, ftf);
		}
		timerTask = new TimerTask(){
			@Override
			public void run() {
				ellapsed += timingResolution;
				
				if (ellapsed<=ftf.length())
				{
					loop();
				}
				else
				{
					stop();
				}
			}
		};
		timer.scheduleAtFixedRate(timerTask,0, timingResolution);
		ellapsed = 0l;
	}

	@Override
	public void stop() {
		sender.setModeAuto();
		ftf.close();
		//minim.stop();
		playing = false;
		timerTask.cancel();
		try {
			currentStream.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void setup() {
		
		minim = new Minim(this);
		timer = new Timer();
	}

	
	public String sketchPath(String fileName) {
		return fileName.substring(0,fileName.lastIndexOf(File.separator));
	}
	
	public InputStream createInput(String fileName){
		try {
			currentStream=new FileInputStream(fileName);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		return currentStream;
	}
	
	private void calcLights(float f, boolean[] lightsOn) {
		float normVal = 0;
		normVal = (f * 50) + 50;

		float inc = 100 / numlines;

		for (int j = 0; j < numlines; j++) {
			if (normVal > j * inc && normVal <= (j + 1) * inc) {
				lightsOn[j] = true;
			}

		}

	}

	private void loop() {
		// we draw the waveform by connecting neighbor values with a line
		// we multiply each of the values by 50
		// because the values in the buffers are normalized
		// this means that they have values between -1 and 1.
		// If we don't scale them up our waveform
		// will look more or less like a straight line.

		boolean[] lightsOnLeft = new boolean[numlines];
		boolean[] lightsOnRight = new boolean[numlines];

		for (int a = 0; a < numlines; a++) {
			lightsOnLeft[a] = false;
			lightsOnRight[a] = false;
		}

		for (int i = 0; i < ftf.bufferSize() - 1; i++) {
			if (stereo) {
				calcLights(ftf.left.get(i), lightsOnLeft);
			} else {
				float avgLevel = (ftf.left.get(i) + ftf.right.get(i)) / 2;
				calcLights(avgLevel, lightsOnLeft);
			}
			calcLights(ftf.right.get(i), lightsOnRight);
		}

		for (int k = 0; k < numlines; k++) {
			if ((lightsOnLeft[k] == true || lightsOnRight[k] == true)
					&& k != beatLight) {
				setLight(k+lightFrom, true);
			} else {
				setLight(k+lightFrom, false);
			}
		}

		if (beatDetect)
		{
			if (beat.isKick() || beat.isSnare() || beat.isHat()) {
				setLight(beatLight, true);
			} else {
				setLight(beatLight, false);
			}
		}
		sender.send(0, "EN");
	}

	private void setLight(int k, boolean b) {
		
		sender.send(0, "LI"+SerialPortSender.zeroPad(k,3)+ (b ? "1":"0"));
	}

	public int getTimingResolution() {
		return timingResolution;
	}

	public void setTimingResolution(int timingResolution) {
		this.timingResolution = timingResolution;
	}

	public boolean isStereo() {
		return stereo;
	}

	public void setStereo(boolean stereo) {
		this.stereo = stereo;
	}

	public int getBeatLight() {
		return beatLight;
	}

	public void setBeatLight(int beatLight) {
		this.beatLight = beatLight;
	}

	public CommSender getSender() {
		return sender;
	}

	public void setSender(CommSender sender) {
		this.sender = sender;
	}

	public boolean isBeatDetect() {
		return beatDetect;
	}

	public void setBeatDetect(boolean beatDetect) {
		this.beatDetect = beatDetect;
	}

	public int getLightFrom() {
		return lightFrom;
	}

	public void setLightFrom(int lightFrom) {
		this.lightFrom = lightFrom;
	}

	public int getLightTo() {
		return lightTo;
	}

	public void setLightTo(int lightTo) {
		this.lightTo = lightTo;
	}
	
}
