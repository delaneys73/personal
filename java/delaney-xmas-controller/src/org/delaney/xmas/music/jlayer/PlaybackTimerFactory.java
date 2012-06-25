package org.delaney.xmas.music.jlayer;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import org.delaney.xmas.comms.CommSender;

public class PlaybackTimerFactory {

	private CommSender sender;
	
	public PlaybackTimer getTimer(String path) throws FileNotFoundException, IOException{
		PlaybackTimer rtn = new PlaybackTimer();
		rtn.setSender(sender);
		rtn.init(new File(path));
		return rtn;
	}

	public CommSender getSender() {
		return sender;
	}

	public void setSender(CommSender sender) {
		this.sender = sender;
	}
	
	
	
}
