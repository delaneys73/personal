package org.delaney.xmas.comms;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DummySender implements CommSender {

	private Logger log = LoggerFactory.getLogger(DummySender.class);
	
	@Override
	public void connect() throws Exception {
		log.info("Connect");
	}
	
	@Override
	public void close() {
		log.info("Close");
	}
	
	@Override
	public void send(long position,String message) {
		log.debug("{} - {}",position,message);
	}
	
	@Override
	public void setModeAuto() {
		log.info("AUTO");
	}
	
	@Override
	public void setModeManual() {
		log.info("MANUAL");
	}
	
	@Override
	public void reset() {
		log.info("RESET");
	}

	@Override
	public void setBeat(long beatInterval, int outline) {
		log.info("BEAT: {} - {}",beatInterval,outline);
	}

}
