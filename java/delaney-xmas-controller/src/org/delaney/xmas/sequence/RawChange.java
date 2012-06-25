package org.delaney.xmas.sequence;

public class RawChange extends Change{

	private int value;
	
	private String bitString; 
	
	@Override
	public void setData(String bs)
	{
		value = Integer.parseInt(bs,2);
		super.setData(bs);
	}
	
	public int getValue() {
		return value;
	}

	public void setValue(int value) {
		this.value = value;
	}

	public String getBitString() {
		return bitString;
	}
	
	
	
}
