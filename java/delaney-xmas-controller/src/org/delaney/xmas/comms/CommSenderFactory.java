package org.delaney.xmas.comms;

import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CommSenderFactory {

	private static Class<CommSender> implClass;
	private static String options;
	private static final Logger log = LoggerFactory.getLogger(CommSenderFactory.class);
	public static CommSender getDefaultSender()
	{
		CommSender rtn = null;
		try {
			rtn = implClass.newInstance();
		} catch (Exception e) {
			log.error("Could not create new CommSender {}",implClass,e);
		}
		
		return rtn;
	}
	
	public static CommSender getNamedSender(String portId)
	{
		return getDefaultSender();
	}

	public static Class<CommSender> getImplClass() {
		return implClass;
	}

	public static void setImplClass(Class<CommSender> implClass) {
		CommSenderFactory.implClass = implClass;
	}

	public static String getOptions() {
		return options;
	}

	public static void setOptions(String options) {
		CommSenderFactory.options = options;
	}
	
	
}
