package org.delaney.xmas.music.jlayer;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Timer;

import javazoom.jl.decoder.JavaLayerException;
import javazoom.jl.player.advanced.AdvancedPlayer;
import javazoom.jl.player.advanced.PlaybackEvent;
import javazoom.jl.player.advanced.PlaybackListener;

import org.delaney.xmas.comms.CommSender;
import org.delaney.xmas.comms.DummySender;
import org.delaney.xmas.music.MusicPlayer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JLayerMusicPlayer implements MusicPlayer
{
	private AdvancedPlayer playMP3;
    private Timer timer;
    private Boolean playing = false;
    private String songFile;
    private Logger log = LoggerFactory.getLogger(JLayerMusicPlayer.class);
    private PlaybackTimerFactory timerFactory;
	private long timerFrequency;
	private Thread playThread;
	private PlaybackTimer playbackTimer;
	private CommSender sender;
	
    public JLayerMusicPlayer() {
    	timer = new Timer();
	}
    
    public void stop()
    {
    	playbackTimer.cancel();
    	playMP3.close();
    }
    
	@SuppressWarnings("deprecation")
	public void play(String mp3) {
		sender.setModeManual();
		this.songFile = mp3;
		if (playThread!=null)
		{
			playThread.stop();
		}
		
		playThread = new Thread(){

			public void run() {
				
				try {
					String seqFile = songFile.substring(0,songFile.lastIndexOf("."))+".seq";
					playbackTimer = timerFactory.getTimer(seqFile);
				
					playMP3 = new AdvancedPlayer(new FileInputStream(songFile));
					
					playing = true;
					playMP3.setPlayBackListener(new PlaybackListener() {
						@Override
						public void playbackFinished(PlaybackEvent evt) {
							super.playbackFinished(evt);
							playMP3.close();
							playbackTimer.cancel();
							playing = false;
							//Set auto mode again
							sender.setModeAuto();
						}
						
						@Override
						public void playbackStarted(PlaybackEvent evt) {
							super.playbackStarted(evt);
							timer.scheduleAtFixedRate(playbackTimer, 0, timerFrequency);
						}
					});
					
					playMP3.play();
					
				} catch (FileNotFoundException e1) {
					log.error("Sequence file missing",e1);
				} catch (IOException e1) {
					log.error("Could not read sequence file",e1);
				} catch (JavaLayerException e) {
					log.error("Could not play MP3",e);
				}				
			}		
		};
		playThread.start();
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		JLayerMusicPlayer player = new JLayerMusicPlayer();
		PlaybackTimerFactory factory = new PlaybackTimerFactory();
		factory.setSender(new DummySender());
		player.setTimerFactory(factory);
		player.setTimerFrequency(50l);
		player.play("/mnt/music/xmas/Black Eyed Peas - I Gotta Feeling.mp3");
	}

	public boolean getPlaying() {
		return playing;
	}

	public void setPlaying(Boolean playing) {
		this.playing = playing;
	}	

	public long getTimerFrequency() {
		return timerFrequency;
	}

	public void setTimerFrequency(long timerFrequency) {
		this.timerFrequency = timerFrequency;
	}

	public PlaybackTimerFactory getTimerFactory() {
		return timerFactory;
	}

	public void setTimerFactory(PlaybackTimerFactory timerFactory) {
		this.timerFactory = timerFactory;
	}

	public CommSender getSender() {
		return sender;
	}

	public void setSender(CommSender sender) {
		this.sender = sender;
	}
}