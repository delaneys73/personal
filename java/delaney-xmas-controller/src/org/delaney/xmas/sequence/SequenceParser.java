package org.delaney.xmas.sequence;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.io.IOUtils;

public class SequenceParser {

	private static long getTimeindex(String raw)
	{
		long rtn = 0l;
		if (raw.indexOf(":")!=-1)
		{
			String[] parts = raw.split(":");
			long minutes = Long.parseLong(parts[0],10);
			rtn = minutes *60000l;
			
			if (parts[1].length()>2)
			{
				if (parts[1].indexOf(".")==-1)
				{
					rtn += Long.parseLong(parts[1],10);
				}
				else
				{
					String[] sparts = parts[1].split("\\.");
					rtn += Long.parseLong(sparts[0], 10)*1000;
					rtn += Long.parseLong(sparts[1], 10)*100;
				}
			}
			else
			{
				rtn += Long.parseLong(parts[1], 10)*1000;
			}
			
		}
		else
		{
			rtn = Long.parseLong(raw,10);
		}
		return rtn;
	}
	
	public static Sequence parse(File sequence,boolean parseBeat) throws FileNotFoundException, IOException{
		FileInputStream is = new FileInputStream(sequence);
		Sequence rtn = new Sequence();
		Map<String,Change> changeMap = new HashMap<String,Change>();
		Map<String,String[]> macrosMap = new HashMap<String,String[]>();
		List<String> lines = IOUtils.readLines(is);

		try {
		for (String line:lines)
		{
			if (!"".equals(line.trim()) && !line.startsWith("#"))
			{
				String[] parts = line.split("\\|");
				
				if ("BPM".equals(parts[0]))
				{
					String[] beatparts = parts[1].split(",");
					int BPM = Integer.parseInt(beatparts[0], 10);
					int bpmOutput = Integer.parseInt(beatparts[1], 10);
					rtn.setBpm(BPM);
					rtn.setBeatOut(bpmOutput);
					long beatInterval = (60000 / BPM)/2;
					rtn.setBeatInterval(beatInterval);		
				}
				else if ("DEFINE".equals(parts[0]))
				{
					String macroName = parts[1];
					String[] changes = parts[2].split(",") ;
					macrosMap.put(macroName, changes);
				} 
				else if ("BASELINE".equals(parts[0]))
				{
					String[] blparts = parts[1].split(",");
					Baseline baseline = new Baseline();
					baseline.setStartTime(getTimeindex(blparts[1]));
					if ("".equals(blparts[2]))
					{
						baseline.setEndTime(-1l);
					}
					else
					{
						baseline.setEndTime(getTimeindex(blparts[2]));
					}
					baseline.setInterval(Long.parseLong(blparts[3],10));
					baseline.setOutLine(Integer.parseInt(blparts[0],10));
					rtn.getBaseLines().add(baseline);
				}
				else {
					String timeIndex = ""+getTimeindex(parts[1]);
					String[] changes = parts[2].split(",");
					Change change;
					if ("RAW".equals(parts[0]))
					{
						change = new RawChange();
					}
					else
					{
						change = new Change();
					}
					change.setName(parts[0]);
					change.setData(parts[2]);
					change.setParts(changes);
					if (parts.length==4)
					{
						change.setIntensity(Integer.parseInt(parts[3], 10));
					}
					else
					{
						change.setIntensity(255);
					}
					changeMap.put(timeIndex, change);
				}
			}
		}
		} 
		finally
		{
			is.close();
		}
		rtn.setHasBeat(parseBeat);
		rtn.setChangeMap(changeMap);
		rtn.setMacros(macrosMap);
		return rtn;
	}
	
}
