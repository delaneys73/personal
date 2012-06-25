package org.delaney.xmas.sequence;

import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Sequence {
	
	private Map<String,Change> changeMap = new HashMap<String,Change>();
	private Map<String,String[]> macros = new HashMap<String,String[]>();
	private List<Baseline> baseLines = new ArrayList<Baseline>();
	
	private int bpm;
	private long beatInterval;
	private int beatOut;
	private boolean hasBeat = false;
	
	public int getBpm() {
		return bpm;
	}

	public void setBpm(int bpm) {
		this.bpm = bpm;
	}

	public long getBeatInterval() {
		return beatInterval;
	}

	public void setBeatInterval(long beatInterval) {
		this.beatInterval = beatInterval;
	}

	public int getBeatOut() {
		return beatOut;
	}

	public void setBeatOut(int beatOut) {
		this.beatOut = beatOut;
	}

	public boolean isHasBeat() {
		return hasBeat;
	}

	public void setHasBeat(boolean hasBeat) {
		this.hasBeat = hasBeat;
	}

	public Map<String, String[]> getMacros() {
		return macros;
	}

	public void setMacros(Map<String, String[]> macros) {
		this.macros = macros;
	}

	public void setChangeMap(Map<String, Change> changeMap) {
		this.changeMap = changeMap;
	}

	
	
	public Map<String, Change> getChangeMap() {
		return changeMap;
	}

	
	
	public List<Baseline> getBaseLines() {
		return baseLines;
	}

	public void setBaseLines(List<Baseline> baseLines) {
		this.baseLines = baseLines;
	}

	@Override
	public String toString() {
		return "Sequence [beatInterval=" + beatInterval + ", beatOut="
				+ beatOut + ", bpm=" + bpm + ", changeMap=" + changeMap
				+ ", hasBeat=" + hasBeat + ", macros=" + macros + "]";
	}
	
	
}
