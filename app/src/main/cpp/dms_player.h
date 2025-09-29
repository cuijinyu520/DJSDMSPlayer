#ifndef DMS_PLAYER_H
#define DMS_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// DMS player context structure
struct DmsContext {
    bool isInitialized;      // 标识播放器是否已初始化
    bool hasActiveMxf;       // 标识是否存在活动的MXF文件
    int64_t currentPosition; // 当前播放位置（以毫秒为单位）
    int64_t duration;        // 媒体总时长（以毫秒为单位）
    bool isPlaying;          // 播放状态标识
    char* mxfPath;           // MXF文件路径
    char* kdmPath;           // KDM文件路径
    void* playerHandle;      // 播放器句柄
    // Add other context fields as needed
};

// Function declarations
int dms_player_init(struct DmsContext* ctx);                    // 初始化DMS播放器
int dms_player_uninit(struct DmsContext* ctx);                  // 反初始化DMS播放器
int dms_player_load_mxf(struct DmsContext* ctx, const char* mxfPath);  // 加载MXF文件
int dms_player_load_kdm(struct DmsContext* ctx, const char* kdmPath);  // 加载KDM文件
int dms_player_play(struct DmsContext* ctx);                    // 开始播放
int dms_player_stop(struct DmsContext* ctx);                    // 停止播放
int dms_player_pause(struct DmsContext* ctx);                   // 暂停播放
int dms_player_resume(struct DmsContext* ctx);                  // 恢复播放
int dms_player_seek(struct DmsContext* ctx, int64_t position);  // 跳转到指定位置
int64_t dms_player_get_position(struct DmsContext* ctx);        // 获取当前播放位置
int64_t dms_player_get_duration(struct DmsContext* ctx);        // 获取媒体总时长
bool dms_player_is_playing(struct DmsContext* ctx);             // 检查是否正在播放

#ifdef __cplusplus
}
#endif

#endif // DMS_PLAYER_H
