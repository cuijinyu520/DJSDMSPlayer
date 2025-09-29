package com.djs.djsdmsplayer;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.Format;
import com.google.android.exoplayer2.extractor.Extractor;
import com.google.android.exoplayer2.extractor.ExtractorInput;
import com.google.android.exoplayer2.extractor.ExtractorOutput;
import com.google.android.exoplayer2.extractor.PositionHolder;
import com.google.android.exoplayer2.extractor.TrackOutput;
import com.google.android.exoplayer2.util.MimeTypes;
import com.google.android.exoplayer2.util.ParsableByteArray;

import java.io.IOException;
import java.util.Collections;

public class DmsExtractor implements Extractor {
    private static final int MAX_FRAME_SIZE = 4 * 1024 * 1024; // 4MB

    private final DmsPlayer dmsPlayer;
    private ExtractorOutput output;
    private TrackOutput trackOutput;
    private long durationUs;
    private float frameRate;
    private long currentTimeUs = 0;
    private boolean hasOutputFormat = false;

    private final ParsableByteArray sampleData = new ParsableByteArray(MAX_FRAME_SIZE);

    public DmsExtractor(DmsPlayer player) {
        this.dmsPlayer = player;
        this.durationUs = player.getDuration();
        this.frameRate = player.getFrameRate();
    }

    @Override
    public boolean sniff(ExtractorInput input) throws IOException {
        return true; // We always handle DMS content
    }

    @Override
    public void init(ExtractorOutput output) {
        this.output = output;
        this.trackOutput = output.track(0, C.TRACK_TYPE_VIDEO);
        output.endTracks();
    }

    @Override
    public int read(ExtractorInput input, PositionHolder seekPosition) throws IOException {
        if (!hasOutputFormat) {
            setupTrackFormat();
            hasOutputFormat = true;
        }

        // Read a frame from DmsDataSource
        int bytesRead = input.read(sampleData.getData(), 0, MAX_FRAME_SIZE);
        if (bytesRead == C.RESULT_END_OF_INPUT) {
            return RESULT_END_OF_INPUT;
        }

        if (bytesRead > 0) {
            sampleData.setPosition(0);
            sampleData.setLimit(bytesRead);

            // Output the sample
            trackOutput.sampleData(sampleData, bytesRead);
            trackOutput.sampleMetadata(
                    currentTimeUs,
                    C.BUFFER_FLAG_KEY_FRAME,
                    bytesRead,
                    0,
                    null
            );

            // Calculate next frame time
            currentTimeUs += (long)(1000000 / frameRate);
        }

        return bytesRead > 0 ? RESULT_CONTINUE : RESULT_END_OF_INPUT;
    }

    @Override
    public void seek(long position, long timeUs) {
        currentTimeUs = timeUs;
        dmsPlayer.seekTo(timeUs);
    }

    @Override
    public void release() {
        // Cleanup resources
    }

    private void setupTrackFormat() {

/*        // 获取实际的视频信息（如果可用）
        int width = dmsPlayer.getFrameWidth();
        int height = dmsPlayer.getFrameHeight();
        String codec = dmsPlayer.getCodecInfo();

        Format format = new Format.Builder()
                .setSampleMimeType(MimeTypes.VIDEO_RAW)
                .setWidth(width > 0 ? width : 1920)
                .setHeight(height > 0 ? height : 1080)
                .setFrameRate(frameRate)
                .setAverageBitrate(Format.NO_VALUE)
                .setPeakBitrate(Format.NO_VALUE)
                .setCodecs(codec != null ? codec : "jpeg2000")
                .setInitializationData(Collections.emptyList())
                .build();*/

        // 使用 Format.Builder 替代已弃用的 createVideoSampleFormat 方法
        Format format = new Format.Builder()
                .setSampleMimeType(MimeTypes.VIDEO_RAW)
                .setWidth(1920) // 宽度 (根据实际内容调整)
                .setHeight(1080) // 高度 (根据实际内容调整)
                .setFrameRate(frameRate)
                .setAverageBitrate(Format.NO_VALUE)
                .setPeakBitrate(Format.NO_VALUE)
                .setCodecs("jpeg2000") // 假设是 JPEG 2000 编码
                .setInitializationData(Collections.emptyList())
                .setPcmEncoding(Format.NO_VALUE)
                .setEncoderDelay(Format.NO_VALUE)
                .setEncoderPadding(Format.NO_VALUE)
                .setAccessibilityChannel(Format.NO_VALUE)
                .setTileCountHorizontal(Format.NO_VALUE)
                .setTileCountVertical(Format.NO_VALUE)
                .setCryptoType(Format.NO_VALUE)
                .build();

        trackOutput.format(format);
    }
}