package org.delaney.xmas.music.jlayer;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.TimerTask;

import org.delaney.xmas.comms.CommSender;
import org.delaney.xmas.sequence.Baseline;
import org.delaney.xmas.sequence.Change;
import org.delaney.xmas.sequence.Sequence;
import org.delaney.xmas.sequence.SequenceParser;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PlaybackTimer extends TimerTask{
	
	private long position = -1l;
	private long startTime = 0;
	@SuppressWarnings("unused")
	private Logger log = LoggerFactory.getLogger(PlaybackTimer.class);
	private CommSender sender;
	private Sequence sequence;
	private Map<Baseline,BaselineStatus> baseLineExecution = new HashMap<Baseline,BaselineStatus>();
	
	private class BaselineStatus {
		
		public long lastExecution;
		public Boolean state = false;
		public Boolean hasReset = false;
	}
	
	public PlaybackTimer() {
		
	}
	
	public void init(File sequenceFile) throws FileNotFoundException, IOException {
		
		sequence = SequenceParser.parse(sequenceFile,true);
		sender.setBeat(sequence.getBeatInterval(), sequence.getBeatOut());
	}
	
	@Override
	public boolean cancel() {
		sender.reset();
		return super.cancel();
	}
	
	@Override
	public void run() {
		if (startTime==0l)
		{
			startTime = scheduledExecutionTime();
		}
		
		position = scheduledExecutionTime() - startTime;

		
		for (Baseline baseline: sequence.getBaseLines())
		{
			boolean beforeEnd= (baseline.getEndTime()>=position || baseline.getEndTime()==-1L);
			BaselineStatus status = baseLineExecution.get(baseline);
			
			if (baseline.getStartTime()<=position && beforeEnd)
			{
				
				if (status==null)
				{
					status = new BaselineStatus();
					status.lastExecution = scheduledExecutionTime();
					status.state = false;
					status.hasReset = false;
					baseLineExecution.put(baseline, status);
				}
				
				if (status.lastExecution+baseline.getInterval()<=scheduledExecutionTime())
				{
					status.state=!status.state;
					status.lastExecution=scheduledExecutionTime();
					String chg = "L"+baseline.getOutLine()+((status.state)?"1":"0");
					sender.send(position,chg);
				}
				
				
			}
			else
			{
				if (!beforeEnd && status!=null && status.hasReset==false)
				{
					//Reset the light status
					String chg = "L"+baseline.getOutLine()+"0";
					status.hasReset = true;
					sender.send(position,chg);
				}
			}
		}
		
		
		Change change = sequence.getChangeMap().get(Long.toString(position));
		
		if (change!=null)
		{
			String[] changes = null;
			
			if (change.getName().equals("LIGHT"))
			{
				changes = change.getParts();
			}
			
			if (change.getName().equals("MACRO"))
			{
				changes = sequence.getMacros().get(change.getData());
			}
			
			if (changes!=null)
			{
				for (String chg:changes)
				{
					sender.send(position,chg);
				}
			}
		}
	}
	
	public CommSender getSender() {
		return sender;
	}

	public void setSender(CommSender sender) {
		this.sender = sender;
	}
	
	
}
