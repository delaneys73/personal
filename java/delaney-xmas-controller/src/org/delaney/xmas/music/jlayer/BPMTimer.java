package org.delaney.xmas.music.jlayer;

import java.util.TimerTask;


public class BPMTimer extends TimerTask {
	
	private int outLine;
	private boolean state = false;
	
	public BPMTimer(int out) {
		this.outLine = out;
	}
	
	@Override
	public void run() {
		System.out.println(outLine+":"+state);
		state = !state;
	}
}
