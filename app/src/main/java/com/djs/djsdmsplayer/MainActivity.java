package com.djs.djsdmsplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.ViewModelProvider;

import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.ui.PlayerView;

public class MainActivity extends AppCompatActivity implements FilePicker.FilePickListener {
    private static final int PERMISSION_REQUEST_CODE = 1001;

    private PlayerViewModel viewModel;
    private PlayerView playerView;
    private TextView txtStatus;
    private Button btnSelectMxf, btnSelectKdm, btnPlay, btnStop;

    private String currentPickType; // "mxf" or "kdm"

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initializeUI();
        initializeViewModel();
        requestPermissions();
        initializeExoPlayer();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 清理缓存文件
        FilePicker.clearCacheFiles(this);
        if (viewModel != null) {
            viewModel.release();
        }
    }

    private void initializeUI() {
        playerView = findViewById(R.id.player_view);
        txtStatus = findViewById(R.id.txt_status);
        btnSelectMxf = findViewById(R.id.btn_select_mxf);
        btnSelectKdm = findViewById(R.id.btn_select_kdm);
        btnPlay = findViewById(R.id.btn_play);
        btnStop = findViewById(R.id.btn_stop);

        btnSelectMxf.setOnClickListener(v -> selectMxfFile());
        btnSelectKdm.setOnClickListener(v -> selectKdmFile());
        btnPlay.setOnClickListener(v -> startPlayback());
        btnStop.setOnClickListener(v -> stopPlayback());

        btnStop.setEnabled(false);
    }

    private void initializeViewModel() {
        viewModel = new ViewModelProvider(this).get(PlayerViewModel.class);

        viewModel.getStatus().observe(this, status -> {
            txtStatus.setText(status);
        });

        viewModel.getIsPlaying().observe(this, isPlaying -> {
            btnPlay.setEnabled(!isPlaying);
            btnStop.setEnabled(isPlaying);
        });
    }

    private void requestPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                    PERMISSION_REQUEST_CODE);
        }
    }

    private void initializeExoPlayer() {
        ExoPlayer player = new ExoPlayer.Builder(this).build();
        playerView.setPlayer(player);

        viewModel.setExoPlayer(player);

        player.addListener(new Player.Listener() {
            @Override
            public void onPlaybackStateChanged(int state) {
                if (state == Player.STATE_ENDED) {
                    new Handler(Looper.getMainLooper()).post(() -> {
                        stopPlayback();
                        txtStatus.setText("播放完成");
                    });
                }
            }
        });
    }

    private void selectMxfFile() {
        currentPickType = "mxf";
        FilePicker.pickMxfFile(this, this);
    }

    private void selectKdmFile() {
        currentPickType = "kdm";
        FilePicker.pickKdmFile(this, this);
    }

    private void startPlayback() {
        if (viewModel.getMxfPath() == null) {
            Toast.makeText(this, "请先选择MXF文件", Toast.LENGTH_SHORT).show();
            return;
        }

        viewModel.startPlayback();
    }

    private void stopPlayback() {
        viewModel.stopPlayback();
    }

    // FilePicker.FilePickListener implementation
    @Override
    public void onFileSelected(String filePath, String fileName) {
        if (!FilePicker.isFileAccessible(filePath)) {
            txtStatus.setText("文件不可访问: " + fileName);
            return;
        }

        double fileSize = FilePicker.getFileSizeMB(filePath);

        if ("mxf".equals(currentPickType)) {
            viewModel.setMxfPath(filePath);
            txtStatus.setText(String.format("MXF文件已选择: %s (%.2f MB)", fileName, fileSize));
        } else if ("kdm".equals(currentPickType)) {
            viewModel.setKdmPath(filePath);
            txtStatus.setText(String.format("KDM文件已选择: %s (%.2f MB)", fileName, fileSize));

            // 自动验证KDM
            viewModel.validateKdm();
        }
    }

    @Override
    public void onError(String errorMessage) {
        txtStatus.setText("错误: " + errorMessage);
        Toast.makeText(this, errorMessage, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(this, "存储权限已授予", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, "存储权限被拒绝，可能无法选择文件", Toast.LENGTH_LONG).show();
            }
        }
    }
}
