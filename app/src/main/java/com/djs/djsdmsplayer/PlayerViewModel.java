package com.djs.djsdmsplayer;

import android.app.Application;
import android.net.Uri;

import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;

public class PlayerViewModel extends AndroidViewModel {
    private final MutableLiveData<String> status = new MutableLiveData<>("Ready");
    private final MutableLiveData<Boolean> isPlaying = new MutableLiveData<>(false);
    
    private DmsPlayer dmsPlayer;
    private ExoPlayer exoPlayer;
    private String mxfPath;
    private String kdmPath;
    
    public PlayerViewModel(Application application) {
        super(application);
        dmsPlayer = new DmsPlayer();
        dmsPlayer.initialize();
    }
    
    public LiveData<String> getStatus() {
        return status;
    }
    
    public LiveData<Boolean> getIsPlaying() {
        return isPlaying;
    }
    
    public String getMxfPath() {
        return mxfPath;
    }
    
    public void setMxfPath(String path) {
        this.mxfPath = path;
    }
    
    public String getKdmPath() {
        return kdmPath;
    }
    
    public void setKdmPath(String path) {
        this.kdmPath = path;
    }
    
    public void setExoPlayer(ExoPlayer player) {
        this.exoPlayer = player;
    }
    
    public void validateKdm() {
        if (kdmPath == null) {
            status.setValue("No KDM file selected");
            return;
        }
        
        DmsPlayer.KdmInfo kdmInfo = dmsPlayer.validateKdm(kdmPath);
        if (kdmInfo != null) {
            status.setValue("KDM Valid: " + 
                (kdmInfo.validateRecipientResult == 0 ? "Yes" : "No"));
        } else {
            status.setValue("KDM validation failed");
        }
    }
    
    public void startPlayback() {
        if (mxfPath == null) {
            status.setValue("No MXF file selected");
            return;
        }
        
        try {
            // Bind KDM if available
            if (kdmPath != null) {
                boolean bound = dmsPlayer.bindKdm(kdmPath);
                if (!bound) {
                    status.setValue("Failed to bind KDM");
                    return;
                }
            }
            
            // Open MXF file
            DmsPlayer.MxfInfo mxfInfo = dmsPlayer.openMxf(mxfPath);
            if (mxfInfo == null) {
                status.setValue("Failed to open MXF file");
                return;
            }
            
            status.setValue("Playing MXF: " + mxfPath);
            
            // Create DMS data source factory
            DmsDataSource.Factory dataSourceFactory = () -> new DmsDataSource(dmsPlayer);
            
            // Create media source
            MediaSource mediaSource = new ProgressiveMediaSource.Factory(dataSourceFactory)
                    .createMediaSource(MediaItem.fromUri(Uri.parse("dms://mxf")));
            
            // Setup ExoPlayer
            exoPlayer.setMediaSource(mediaSource);
            exoPlayer.prepare();
            exoPlayer.setPlayWhenReady(true);
            
            isPlaying.setValue(true);
            
        } catch (Exception e) {
            status.setValue("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
    
    public void stopPlayback() {
        if (exoPlayer != null) {
            exoPlayer.stop();
        }
        
        dmsPlayer.stopPlayback();
        dmsPlayer.closeMxf();
        
        isPlaying.setValue(false);
        status.setValue("Playback stopped");
    }
    
    public void release() {
        if (exoPlayer != null) {
            exoPlayer.release();
        }
        
        dmsPlayer.uninitialize();
    }
    
    @Override
    protected void onCleared() {
        super.onCleared();
        release();
    }
}