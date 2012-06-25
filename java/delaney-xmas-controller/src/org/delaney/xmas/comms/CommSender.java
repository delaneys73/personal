package org.delaney.xmas.comms;

public interface CommSender {
	void connect() throws Exception;
	void send(long position,String message);
	void setBeat(long beatInterval,int outline);
	void reset();
	void setModeManual();
	void setModeAuto();
	void close();
}
