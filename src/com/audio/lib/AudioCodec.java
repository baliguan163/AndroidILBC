package com.audio.lib;


public class AudioCodec {
	private static AudioCodec sudioCodec;
	
	static {
		   System.loadLibrary("audioilbc");
		   sudioCodec = null;
	  }
	
	   AudioCodec() {
	        this.audio_codec_init(0);
	   }
	    
	    public static AudioCodec getInstance() {
	        if(sudioCodec == null) {
	            Class var0 = AudioCodec.class;
	            synchronized(AudioCodec.class) {
	                if(sudioCodec == null) {
	                	sudioCodec = new AudioCodec();
	                }
	            }
	        }
	        return sudioCodec;
	    }
	    
	public static native int audio_codec_init(int mode);
	public static native int audio_encode(byte[] sample, int sampleOffset,int sampleLength, byte[] data, int dataOffset);
	public static native int audio_decode(byte[] data, int dataOffset,int dataLength, byte[] sample, int sampleLength);
}
