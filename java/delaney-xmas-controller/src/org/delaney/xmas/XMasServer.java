package org.delaney.xmas;

import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.webapp.WebAppContext;

public class XMasServer {

	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		Server server = new Server(Integer.parseInt(args[0]));

        server.setStopAtShutdown(true);

        //another way is to use an external jetty configuration file
        //XmlConfiguration configuration =
        //new XmlConfiguration(new File("/path/jetty-config.xml").toURL());
        //configuration.configure(server);
        WebAppContext context = new WebAppContext();
        String webapp = "./docroot";
        context.setDescriptor(webapp+"/WEB-INF/web.xml");
        context.setResourceBase(webapp);
        context.setContextPath("/");
        context.setParentLoaderPriority(true);
 
        server.setHandler(context);
 
        server.start();
        server.join();
	}

}
