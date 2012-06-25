package org.delaney.xmas;

import java.io.File;
import java.io.FileOutputStream;
import java.util.Collection;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;

public class Packager {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception{
		Collection<File> jars = FileUtils.listFiles(new File("lib"), new String[]{"jar"}, false);
		String cp=""; 
		for (File jar:jars)
		{
			cp += "lib/"+jar.getName()+":";
		}
		
		jars = FileUtils.listFiles(new File("ext-lib"), new String[]{"jar"}, false);

		for (File jar:jars)
		{
			cp += "ext-lib/"+jar.getName()+":";
		}
		
		String script = "#!/bin/sh\njava -cp "+cp.substring(0, cp.length()-1)+" org.delaney.xmas.XMasServer $1";
		File scriptfile = new File("run.sh");
		if (scriptfile.exists())
		{
			FileUtils.forceDelete(scriptfile);
		}
		IOUtils.write(script, new FileOutputStream(scriptfile), "utf-8");

	}

}
