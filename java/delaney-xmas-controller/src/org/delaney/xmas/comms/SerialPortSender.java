package org.delaney.xmas.comms;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.TooManyListenersException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SerialPortSender implements CommSender {

	private InputStream in;
	private OutputStream out;
	private static Logger log = LoggerFactory.getLogger(SerialPortSender.class);
	private CommPort commPort;
	private Thread writerThread;
	private String commPortName;
	private int baudRate;
	private static int bufferSize;
	private int outputLines = 8;
	private long autoDelay = 1000;
	
	public SerialPortSender() {
		// TODO Auto-generated constructor stub
	}
	
	public void reset(){
		setBeat(0, 0);
		for (int x=0;x<outputLines;x++)
		{
			String light = zeroPad(x, 3);
			send(0,"LI"+light+"0");
		}
		send(0,"EN");
	}
	
	@Override
	public void connect() throws Exception {
		connect(commPortName);
	}
	
	@Override
	public void close() {
		shutdown();
	}
	
	public void init(){
		try {
			log.info("Connecting to comm port {}",commPortName);
			connect(commPortName);
			log.info("Sending reset");
			reset();
			setModeAuto();
		} catch (Exception e) {
			log.error("Error",e);
		}
	}
	
	@Override
	public void send(long position, String message) {
		try {
			if (message.startsWith("L")&&message.length()==3)
			{
				Integer light = Integer.parseInt(message.substring(1,2),10);
				message = "LI"+zeroPad(light,3)+message.substring(2);
			}
			
			out.write((message+"\n").getBytes());
			//log.debug("{} : {}",new Object[]{position,message});
		} catch (IOException e) {
			log.error("Could not send",e);
		}
	}
	
	@Override
	public void setModeAuto() {
		send(0,"OL"+zeroPad(outputLines,4));
		send(0,"AU"+zeroPad(autoDelay,4));
		log.info("Setting auto mode");
	}
	
	@Override
	public void setModeManual() {
		send(0,"OL"+zeroPad(outputLines,4));
		send(0,"MN");
		log.info("Entering manual mode");
	}

	public static String zeroPad(long num,int len)
	{
		String rtn = ""+num;
		if (rtn.length()<len)
		{
			int pad = len-rtn.length();
			for (int x=0;x < pad;x++){
				rtn= "0"+rtn;
			}
		}
		
		return rtn;
	}
	
	@Override
	public void setBeat(long beatInterval, int outline) {
		String beat = zeroPad(beatInterval, 4);
		
		
		String message = "BE"+beat+outline+"\n";
		
		try {
			out.write(message.getBytes());
		} catch (IOException e) {
			log.error("Could not send",e);
		}
	}

	public void connect(String portName) throws UnsupportedCommOperationException, IOException, TooManyListenersException, PortInUseException, NoSuchPortException
	{
        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        if ( portIdentifier.isCurrentlyOwned() )
        {
            log.error("Error: Port is currently in use");
        }
        else
        {
            commPort = portIdentifier.open(this.getClass().getName(),2000);
            
            if ( commPort instanceof SerialPort )
            {
                SerialPort serialPort = (SerialPort) commPort;
                serialPort.setSerialPortParams(baudRate,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
                
                in = serialPort.getInputStream();
                out = serialPort.getOutputStream();
                
                writerThread = new Thread(new SerialWriter(out));                
                writerThread.start();
                
                serialPort.addEventListener(new SerialReader(in));
                serialPort.notifyOnDataAvailable(true);

            }
            else
            {
                log.error("Error: Only serial ports are handled by this example.");
            }
        }     
    }

	
	@SuppressWarnings("deprecation")
	public void shutdown()
	{
		writerThread.stop();
		commPort.close();
	}
	  /**
     * Handles the input coming from the serial port. A new line character
     * is treated as the end of a block in this example. 
     */
    public static class SerialReader implements SerialPortEventListener 
    {
        private InputStream in;
        private byte[] buffer = new byte[bufferSize];
        
        public SerialReader ( InputStream in )
        {
            this.in = in;
        }
        
        public void serialEvent(SerialPortEvent arg0) {
            int data;
          
            try
            {
                int len = 0;
                while ( ( data = in.read()) > -1 )
                {
                    if ( data == '\n' ) {
                        break;
                    }
                    buffer[len++] = (byte) data;
                }
                String msg = new String(buffer,0,len); 
                log.debug(msg);
            }
            catch ( IOException e )
            {
                log.error("",e);
            }             
        }

    }

    /** */
    public static class SerialWriter implements Runnable 
    {
        OutputStream out;
        
        public SerialWriter ( OutputStream out )
        {
            this.out = out;
        }
        
        public void run ()
        {
            try
            {                
                int c = 0;
                while ( ( c = System.in.read()) > -1 )
                {
                    this.out.write(c);
                }                
            }
            catch ( IOException e )
            {
              	log.error("",e);
            }            
        }
    }
	
    static String getPortTypeName ( int portType )
    {
        switch ( portType )
        {
            case CommPortIdentifier.PORT_I2C:
                return "I2C";
            case CommPortIdentifier.PORT_PARALLEL:
                return "Parallel";
            case CommPortIdentifier.PORT_RAW:
                return "Raw";
            case CommPortIdentifier.PORT_RS485:
                return "RS485";
            case CommPortIdentifier.PORT_SERIAL:
                return "Serial";
            default:
                return "unknown type";
        }
    }
    
	public static void main(String[] args) throws UnsupportedCommOperationException, IOException, TooManyListenersException, PortInUseException, NoSuchPortException
	{
		   
        
		SerialPortSender sender = new SerialPortSender();
		sender.connect("/dev/ttyUSB0");
		sender.setBeat(284, 7);
		sender.send(10, "LI0021");
		sender.send(10, "LI0031");
		sender.send(10, "LI0041");
		//sender.send(10, "EN");
		//sender.shutdown();
	}

	public String getCommPortName() {
		return commPortName;
	}

	public void setCommPortName(String commPortName) {
		this.commPortName = commPortName;
	}
	
	public long getAutoDelay() {
		return autoDelay;
	}
	
	public void setAutoDelay(long autoDelay) {
		this.autoDelay = autoDelay;
	}

	public int getOutputLines() {
		return outputLines;
	}

	public void setOutputLines(int outputLines) {
		this.outputLines = outputLines;
	}

	public int getBaudRate() {
		return baudRate;
	}

	public void setBaudRate(int baudRate) {
		this.baudRate = baudRate;
	}

	public int getBufferSize() {
		return bufferSize;
	}

	public void setBufferSize(int bs) {
		bufferSize = bs;
	}	
}
