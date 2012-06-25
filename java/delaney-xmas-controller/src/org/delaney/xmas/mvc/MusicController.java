package org.delaney.xmas.mvc;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import javazoom.spi.mpeg.sampled.file.tag.IcyInputStream;
import javazoom.spi.mpeg.sampled.file.tag.MP3MetadataParser;
import javazoom.spi.mpeg.sampled.file.tag.MP3Tag;

import org.apache.commons.io.FileUtils;
import org.delaney.xmas.music.MusicPlayer;
import org.eclipse.jetty.util.log.Log;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;


@Controller
public class MusicController {
	private MusicPlayer defaultPlayer;
	private MusicPlayer sequencePlayer;
	private String musicDirectory;
	private boolean allowOverride;
	private String[] extensions;
	private List<String> names = new ArrayList<String>();
	private Long lastScan=0l;
	private Long scanInterval= 86400000l;
	
	private List<String> getSongs() 
	{
		Long now = System.currentTimeMillis();
		
		if (now > lastScan+scanInterval)
		{
			lastScan = now;
			names.clear();
			File dir = new File(musicDirectory);
			
			if (dir.exists())
			{
				Collection<File> music = FileUtils.listFiles(dir, extensions, false);
				
				for (File f:music)
				{
					try {
					//IcyInputStream is = new IcyInputStream(new FileInputStream(f));
					//MP3Tag[] tags = is.getTags();
					
					names.add(f.getName().substring(0, f.getName().lastIndexOf(".")));
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			}
		}
		return names;
	}
	
	@RequestMapping(value="")
	public String defaultPage(@RequestParam(required=false) String message,Model model)
	{
		
		model.addAttribute("music", getSongs());
		if (message!=null)
		{
			model.addAttribute("message", message);
		}
		else
		{
			model.addAttribute("message", "");
		}
		return "home";
	}
	
	@RequestMapping(value = "/play.action/{songId}",method=RequestMethod.GET)
	public String play(@PathVariable String songId,Model model) {
	
		String message = "";
		
		String songFile = musicDirectory+File.separator+songId;
		File sequenceFile = new File(songFile.substring(0,songFile.lastIndexOf("."))+".seq");
		
		MusicPlayer player = defaultPlayer;
		
		if (sequenceFile.exists())
		{
			player = sequencePlayer;
			Log.info("Using custom sequence for lights {}",sequenceFile.getName());
		}
		
		if (!player.getPlaying() || !sequencePlayer.getPlaying() || allowOverride)
		{
			if (defaultPlayer.getPlaying())
			{
				defaultPlayer.stop();	
			}
			
			if (sequencePlayer.getPlaying())
			{
				sequencePlayer.stop();	
			}
			
			player.play(songFile);
			message="Now playing "+songId;
		}
		else
		{
			message="Music sequence already active, please wait to complete";
		}
			
		return "redirect:/app/?message="+message;
	}

	public String getMusicDirectory() {
		return musicDirectory;
	}

	public void setMusicDirectory(String musicDirectory) {
		this.musicDirectory = musicDirectory;
	}

	public boolean isAllowOverride() {
		return allowOverride;
	}

	public void setAllowOverride(boolean allowOverride) {
		this.allowOverride = allowOverride;
	}

	public MusicPlayer getDefaultPlayer() {
		return defaultPlayer;
	}

	public void setDefaultPlayer(MusicPlayer defaultPlayer) {
		this.defaultPlayer = defaultPlayer;
	}

	public MusicPlayer getSequencePlayer() {
		return sequencePlayer;
	}

	public void setSequencePlayer(MusicPlayer sequencePlayer) {
		this.sequencePlayer = sequencePlayer;
	}

	public String[] getExtensions() {
		return extensions;
	}

	public void setExtensions(String[] extensions) {
		this.extensions = extensions;
	}
	
	
}
