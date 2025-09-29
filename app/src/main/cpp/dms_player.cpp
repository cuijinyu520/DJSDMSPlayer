#include "dms_player.h"
#include "libdms.h"
#include <stdlib.h>
#include <string.h>
#include <android/log.h>

#define LOG_TAG "DmsPlayer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Implementation of DMS player functions

/**
 * @brief 初始化DMS播放器上下文
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_init(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    // 初始化上下文成员
    ctx->isInitialized = false;
    ctx->hasActiveMxf = false;
    ctx->currentPosition = 0;
    ctx->duration = 0;
    ctx->isPlaying = false;
    ctx->mxfPath = nullptr;
    ctx->kdmPath = nullptr;
    ctx->playerHandle = nullptr;

    // 初始化DMS库
    int result = _dms_library_initialize(DMS_MODE_PLAY, nullptr, true);
    if (result != 0) {
        LOGE("Failed to initialize DMS library: 0x%08x", result);
        return result;
    }

    ctx->isInitialized = true;
    LOGI("DMS player initialized successfully");
    return 0;
}

/**
 * @brief 反初始化DMS播放器
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_uninit(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    // 释放路径字符串内存
    if (ctx->mxfPath) {
        free(ctx->mxfPath);
        ctx->mxfPath = nullptr;
    }

    if (ctx->kdmPath) {
        free(ctx->kdmPath);
        ctx->kdmPath = nullptr;
    }

    // 反初始化DMS库
    if (ctx->isInitialized) {
        _dms_library_uninitialize();
        ctx->isInitialized = false;
    }

    // 重置其他状态
    ctx->hasActiveMxf = false;
    ctx->currentPosition = 0;
    ctx->duration = 0;
    ctx->isPlaying = false;
    ctx->playerHandle = nullptr;

    LOGI("DMS player uninitialized successfully");
    return 0;
}

/**
 * @brief 加载MXF文件
 * @param ctx DMS播放器上下文指针
 * @param mxfPath MXF文件路径
 * @return 成功返回0，失败返回错误码
 */
int dms_player_load_mxf(struct DmsContext* ctx, const char* mxfPath) {
    if (!ctx || !mxfPath) {
        LOGE("Invalid parameters");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    // 释放旧的路径内存
    if (ctx->mxfPath) {
        free(ctx->mxfPath);
        ctx->mxfPath = nullptr;
    }

    // 复制新的路径
    ctx->mxfPath = (char*)malloc(strlen(mxfPath) + 1);
    if (!ctx->mxfPath) {
        LOGE("Failed to allocate memory for MXF path");
        return -3;
    }
    strcpy(ctx->mxfPath, mxfPath);

    // 打开DCP
    int result = _dms_open_dcp(mxfPath,"", true);
    if (result != 0) {
        LOGE("Failed to open DCP: 0x%08x", result);
        ctx->hasActiveMxf = false;
        return result;
    }

    ctx->hasActiveMxf = true;
    LOGI("MXF file loaded successfully: %s", mxfPath);
    return 0;
}

/**
 * @brief 加载KDM文件
 * @param ctx DMS播放器上下文指针
 * @param kdmPath KDM文件路径
 * @return 成功返回0，失败返回错误码
 */
int dms_player_load_kdm(struct DmsContext* ctx, const char* kdmPath) {
    if (!ctx || !kdmPath) {
        LOGE("Invalid parameters");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    // 释放旧的路径内存
    if (ctx->kdmPath) {
        free(ctx->kdmPath);
        ctx->kdmPath = nullptr;
    }

    // 复制新的路径
    ctx->kdmPath = (char*)malloc(strlen(kdmPath) + 1);
    if (!ctx->kdmPath) {
        LOGE("Failed to allocate memory for KDM path");
        return -3;
    }
    strcpy(ctx->kdmPath, kdmPath);

    // 绑定KDM
    int result = _dms_bind_kdm(kdmPath);
    if (result != 0) {
        LOGE("Failed to bind KDM: 0x%08x", result);
        return result;
    }

    LOGI("KDM file loaded successfully: %s", kdmPath);
    return 0;
}

/**
 * @brief 开始播放
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_play(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    if (!ctx->hasActiveMxf) {
        LOGE("No active MXF file");
        return -3;
    }

    // 在实际实现中，这里应该启动播放线程或调用播放函数
    ctx->isPlaying = true;
    ctx->currentPosition = 0;

    LOGI("Playback started");
    return 0;
}

/**
 * @brief 停止播放
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_stop(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    // 在实际实现中，这里应该停止播放线程并重置播放状态
    ctx->isPlaying = false;
    ctx->currentPosition = 0;

    LOGI("Playback stopped");
    return 0;
}

/**
 * @brief 暂停播放
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_pause(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    if (!ctx->isPlaying) {
        LOGI("Player is not playing");
        return 0;
    }

    // 在实际实现中，这里应该暂停播放线程
    ctx->isPlaying = false;

    LOGI("Playback paused");
    return 0;
}

/**
 * @brief 恢复播放
 * @param ctx DMS播放器上下文指针
 * @return 成功返回0，失败返回错误码
 */
int dms_player_resume(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    if (ctx->isPlaying) {
        LOGI("Player is already playing");
        return 0;
    }

    // 在实际实现中，这里应该恢复播放线程
    ctx->isPlaying = true;

    LOGI("Playback resumed");
    return 0;
}

/**
 * @brief 跳转到指定位置
 * @param ctx DMS播放器上下文指针
 * @param position 目标位置（毫秒）
 * @return 成功返回0，失败返回错误码
 */
int dms_player_seek(struct DmsContext* ctx, int64_t position) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    if (!ctx->isInitialized) {
        LOGE("Player not initialized");
        return -2;
    }

    if (position < 0) {
        LOGE("Invalid position");
        return -3;
    }

    // 在实际实现中，这里应该跳转到指定位置
    ctx->currentPosition = position;

    LOGI("Seek to position: %lld ms", (long long)position);
    return 0;
}

/**
 * @brief 获取当前播放位置
 * @param ctx DMS播放器上下文指针
 * @return 当前播放位置（毫秒）
 */
int64_t dms_player_get_position(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    return ctx->currentPosition;
}

/**
 * @brief 获取媒体总时长
 * @param ctx DMS播放器上下文指针
 * @return 媒体总时长（毫秒）
 */
int64_t dms_player_get_duration(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return -1;
    }

    return ctx->duration;
}

/**
 * @brief 检查是否正在播放
 * @param ctx DMS播放器上下文指针
 * @return 正在播放返回true，否则返回false
 */
bool dms_player_is_playing(struct DmsContext* ctx) {
    if (!ctx) {
        LOGE("Invalid context pointer");
        return false;
    }

    return ctx->isPlaying;
}
