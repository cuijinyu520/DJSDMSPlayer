package com.djs.djsdmsplayer;

import android.view.Surface;
import androidx.annotation.Nullable;

public class DmsPlayer {
    static {
        System.loadLibrary("dms");
        System.loadLibrary("dms_jni");
    }
    
    private long nativePtr;
    
    public static class KdmInfo {
        public String id;
        public String recipientSubjectName;
        public String cplId;
        public String contentTitle;
        public String notValidBefore;
        public String notValidAfter;
        public int sessionCount;
        public int remainSessionCount;
        public int validateTimeWindowResult;
        public int validateRecipientResult;
    }
    
    public static class MxfInfo {
        public int width;
        public int height;
        public float frameRate;
        public long duration; // microseconds
        public String codec;
        public boolean isEncrypted;
    }
    
    // Native methods
    public native boolean initialize();
    public native void uninitialize();
    
    @Nullable
    public native KdmInfo validateKdm(String kdmFilePath);
    
    public native boolean bindKdm(String kdmFilePath);
    
    @Nullable
    public native MxfInfo openMxf(String mxfFilePath);
    
    public native void closeMxf();
    
    public native boolean startPlayback();
    
    public native void stopPlayback();
    
    public native int getNextFrame(byte[] buffer);
    
    public native void seekTo(long positionUs);
    
    public native long getDuration();
    
    public native float getFrameRate();
    
    public native boolean isEncrypted();
}