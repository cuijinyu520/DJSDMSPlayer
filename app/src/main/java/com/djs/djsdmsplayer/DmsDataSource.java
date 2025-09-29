package com.djs.djsdmsplayer;

import android.net.Uri;

import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DataSpec;
import com.google.android.exoplayer2.upstream.TransferListener;

import java.io.IOException;

public class DmsDataSource implements DataSource {
    private final DmsPlayer dmsPlayer;
    private final byte[] frameBuffer;
    private boolean isOpen = false;
    private long position = 0;

    public DmsDataSource(DmsPlayer player) {
        this.dmsPlayer = player;
        this.frameBuffer = new byte[4 * 1024 * 1024]; // 4MB buffer for frames
    }

    @Override
    public Uri getUri() {
        // 返回当前数据源的URI
        // 示例实现：
        return null; // 或返回实际的Uri对象
    }
    
    @Override
    public void addTransferListener(TransferListener transferListener) {
        // Not implemented
    }
    
    @Override
    public long open(DataSpec dataSpec) throws IOException {
        isOpen = true;
        position = dataSpec.position;
        return dmsPlayer.getDuration();
    }
    
    @Override
    public int read(byte[] buffer, int offset, int readLength) throws IOException {
        if (!isOpen) {
            throw new IOException("DataSource is not open");
        }
        
        // Get next frame from DMS player
        int frameSize = dmsPlayer.getNextFrame(frameBuffer);
        
        if (frameSize <= 0) {
            return -1; // End of stream
        }
        
        // Ensure target buffer is large enough
        if (readLength < frameSize) {
            throw new IOException("Buffer too small for frame data: " + frameSize + " > " + readLength);
        }
        
        // Copy frame data to ExoPlayer's buffer
        System.arraycopy(frameBuffer, 0, buffer, offset, frameSize);
        position += frameSize;
        
        return frameSize;
    }
    
    @Override
    public void close() throws IOException {
        isOpen = false;
    }
}