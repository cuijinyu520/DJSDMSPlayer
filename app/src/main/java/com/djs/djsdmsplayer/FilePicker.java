package com.djs.djsdmsplayer;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.provider.OpenableColumns;
import android.util.Log;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class FilePicker {
    private static final String TAG = "FilePicker";
    
    public interface FilePickListener {
        void onFileSelected(String filePath, String fileName);
        void onError(String errorMessage);
    }
    
    /**
     * 启动文件选择器选择MXF文件
     */
    public static void pickMxfFile(AppCompatActivity activity, FilePickListener listener) {
        pickFile(activity, listener, "application/octet-stream", ".mxf");
    }
    
    /**
     * 启动文件选择器选择KDM文件
     */
    public static void pickKdmFile(AppCompatActivity activity, FilePickListener listener) {
        pickFile(activity, listener, "application/xml", ".kdm");
    }
    
    /**
     * 通用文件选择方法
     */
    private static void pickFile(AppCompatActivity activity, FilePickListener listener, 
                               String mimeType, String fileExtension) {
        try {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType(mimeType);
            
            ActivityResultLauncher<Intent> launcher = activity.registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == Activity.RESULT_OK && result.getData() != null) {
                        Uri uri = result.getData().getData();
                        processSelectedFile(activity, uri, listener, fileExtension);
                    } else {
                        listener.onError("文件选择取消");
                    }
                });
            
            launcher.launch(intent);
            
        } catch (Exception e) {
            Log.e(TAG, "文件选择错误: " + e.getMessage());
            listener.onError("文件选择失败: " + e.getMessage());
        }
    }
    
    /**
     * 处理选中的文件
     */
    private static void processSelectedFile(AppCompatActivity activity, Uri uri, 
                                          FilePickListener listener, String fileExtension) {
        try {
            // 获取文件名
            String fileName = getFileName(activity, uri);
            if (fileName == null) {
                listener.onError("无法获取文件名");
                return;
            }
            
            // 验证文件扩展名
            if (!fileName.toLowerCase().endsWith(fileExtension.toLowerCase())) {
                listener.onError("请选择正确的文件类型: " + fileExtension);
                return;
            }
            
            // 将文件复制到应用缓存目录
            String cachedFilePath = copyFileToCache(activity, uri, fileName);
            if (cachedFilePath != null) {
                listener.onFileSelected(cachedFilePath, fileName);
            } else {
                listener.onError("文件复制失败");
            }
            
        } catch (Exception e) {
            Log.e(TAG, "文件处理错误: " + e.getMessage());
            listener.onError("文件处理失败: " + e.getMessage());
        }
    }
    
    /**
     * 获取文件名
     */
    private static String getFileName(AppCompatActivity activity, Uri uri) {
        String fileName = null;
        try (android.database.Cursor cursor = activity.getContentResolver()
                .query(uri, null, null, null, null)) {
            
            if (cursor != null && cursor.moveToFirst()) {
                int nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if (nameIndex >= 0) {
                    fileName = cursor.getString(nameIndex);
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "获取文件名错误: " + e.getMessage());
        }
        return fileName;
    }
    
    /**
     * 将文件复制到缓存目录
     */
    private static String copyFileToCache(AppCompatActivity activity, Uri uri, String fileName) {
        File cacheDir = activity.getCacheDir();
        File outputFile = new File(cacheDir, fileName);
        
        try (InputStream inputStream = activity.getContentResolver().openInputStream(uri);
             FileOutputStream outputStream = new FileOutputStream(outputFile)) {
            
            if (inputStream == null) {
                return null;
            }
            
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, bytesRead);
            }
            
            return outputFile.getAbsolutePath();
            
        } catch (Exception e) {
            Log.e(TAG, "文件复制错误: " + e.getMessage());
            return null;
        }
    }
    
    /**
     * 检查文件是否存在并可读
     */
    public static boolean isFileAccessible(String filePath) {
        if (filePath == null || filePath.isEmpty()) {
            return false;
        }
        
        File file = new File(filePath);
        return file.exists() && file.canRead() && file.length() > 0;
    }
    
    /**
     * 获取文件大小（MB）
     */
    public static double getFileSizeMB(String filePath) {
        if (!isFileAccessible(filePath)) {
            return 0;
        }
        
        File file = new File(filePath);
        return file.length() / (1024.0 * 1024.0);
    }
    
    /**
     * 清理缓存文件
     */
    public static void clearCacheFiles(AppCompatActivity activity) {
        File cacheDir = activity.getCacheDir();
        if (cacheDir.exists() && cacheDir.isDirectory()) {
            File[] files = cacheDir.listFiles();
            if (files != null) {
                for (File file : files) {
                    if (file.getName().endsWith(".mxf") || file.getName().endsWith(".kdm")) {
                        file.delete();
                    }
                }
            }
        }
    }
}
