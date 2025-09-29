#include <jni.h>
#include <android/log.h>
#include <string>

#include "dms_player.h"
#include "../include/libdms.h"

// 定义日志标签和宏
#define LOG_TAG "DMS_JNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

// 全局引用Java类
static jclass gKdmInfoClass = nullptr;     // KDM信息类的全局引用
static jclass gMxfInfoClass = nullptr;     // MXF信息类的全局引用

// KdmInfo类的字段ID
static jfieldID gKdmInfo_id;                    // KDM ID字段
static jfieldID gKdmInfo_recipientSubjectName;  // 接收者主题名称字段
static jfieldID gKdmInfo_cplId;                 // CPL ID字段
static jfieldID gKdmInfo_contentTitle;          // 内容标题字段
static jfieldID gKdmInfo_notValidBefore;        // 有效期开始时间字段
static jfieldID gKdmInfo_notValidAfter;         // 有效期结束时间字段
static jfieldID gKdmInfo_sessionCount;          // 会话次数字段
static jfieldID gKdmInfo_remainSessionCount;    // 剩余会话次数字段
static jfieldID gKdmInfo_validateTimeWindowResult;   // 时间窗口验证结果字段
static jfieldID gKdmInfo_validateRecipientResult;    // 接收者验证结果字段

// MxfInfo类的字段ID
static jfieldID gMxfInfo_width;          // 视频宽度字段
static jfieldID gMxfInfo_height;         // 视频高度字段
static jfieldID gMxfInfo_frameRate;      // 帧率字段
static jfieldID gMxfInfo_duration;       // 持续时间字段
static jfieldID gMxfInfo_codec;          // 编解码器字段
static jfieldID gMxfInfo_isEncrypted;    // 是否加密字段

/**
 * 初始化DMS播放器
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @return 初始化成功返回JNI_TRUE，失败返回JNI_FALSE
 */
JNIEXPORT jboolean JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_initialize(JNIEnv* env, jobject thiz) {
    // 初始化DMS库
    int result = _dms_library_initialize(DMS_MODE_PLAY, nullptr, true);

    if (result != DMS_RESULT_SUCCESS) {
        LOGE("Failed to initialize DMS library: 0x%08x", result);
        return JNI_FALSE;
    }

    // 创建并存储本地上下文
    DmsContext* context = new DmsContext();
    context->isInitialized = true;
    context->hasActiveMxf = false;
    context->currentPosition = 0;

    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    env->SetLongField(thiz, fieldId, reinterpret_cast<jlong>(context));

    // 缓存Java类和字段ID
    if (gKdmInfoClass == nullptr) {
        jclass localKdmInfoClass = env->FindClass("com/djs/djsdmsplayer/DmsPlayer$KdmInfo");
        gKdmInfoClass = static_cast<jclass>(env->NewGlobalRef(localKdmInfoClass));
        env->DeleteLocalRef(localKdmInfoClass);

        gKdmInfo_id = env->GetFieldID(gKdmInfoClass, "id", "Ljava/lang/String;");
        gKdmInfo_recipientSubjectName = env->GetFieldID(gKdmInfoClass, "recipientSubjectName", "Ljava/lang/String;");
        gKdmInfo_cplId = env->GetFieldID(gKdmInfoClass, "cplId", "Ljava/lang/String;");
        gKdmInfo_contentTitle = env->GetFieldID(gKdmInfoClass, "contentTitle", "Ljava/lang/String;");
        gKdmInfo_notValidBefore = env->GetFieldID(gKdmInfoClass, "notValidBefore", "Ljava/lang/String;");
        gKdmInfo_notValidAfter = env->GetFieldID(gKdmInfoClass, "notValidAfter", "Ljava/lang/String;");
        gKdmInfo_sessionCount = env->GetFieldID(gKdmInfoClass, "sessionCount", "I");
        gKdmInfo_remainSessionCount = env->GetFieldID(gKdmInfoClass, "remainSessionCount", "I");
        gKdmInfo_validateTimeWindowResult = env->GetFieldID(gKdmInfoClass, "validateTimeWindowResult", "I");
        gKdmInfo_validateRecipientResult = env->GetFieldID(gKdmInfoClass, "validateRecipientResult", "I");
    }

    if (gMxfInfoClass == nullptr) {
        jclass localMxfInfoClass = env->FindClass("com/djs/djsdmsplayer/DmsPlayer$MxfInfo");
        gMxfInfoClass = static_cast<jclass>(env->NewGlobalRef(localMxfInfoClass));
        env->DeleteLocalRef(localMxfInfoClass);

        gMxfInfo_width = env->GetFieldID(gMxfInfoClass, "width", "I");
        gMxfInfo_height = env->GetFieldID(gMxfInfoClass, "height", "I");
        gMxfInfo_frameRate = env->GetFieldID(gMxfInfoClass, "frameRate", "F");
        gMxfInfo_duration = env->GetFieldID(gMxfInfoClass, "duration", "J");
        gMxfInfo_codec = env->GetFieldID(gMxfInfoClass, "codec", "Ljava/lang/String;");
        gMxfInfo_isEncrypted = env->GetFieldID(gMxfInfoClass, "isEncrypted", "Z");
    }

    return JNI_TRUE;
}

/**
 * 反初始化DMS播放器
 * @param env JNI环境指针
 * @param thiz Java对象引用
 */
JNIEXPORT void JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_uninitialize(JNIEnv* env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context != nullptr) {
        if (context->hasActiveMxf) {
            _dms_close_dcp(); // 暂时使用DCP关闭函数
        }

        _dms_library_uninitialize();
        delete context;
        env->SetLongField(thiz, fieldId, 0LL);
    }

// 释放全局引用
    if (gKdmInfoClass != nullptr) {
        env->DeleteGlobalRef(gKdmInfoClass);
        gKdmInfoClass = nullptr;
    }

    if (gMxfInfoClass != nullptr) {
        env->DeleteGlobalRef(gMxfInfoClass);
        gMxfInfoClass = nullptr;
    }
}

/**
 * 验证KDM文件
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @param kdm_path KDM文件路径
 * @return KDM信息对象，验证失败返回nullptr
 */
JNIEXPORT jobject JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_validateKdm(JNIEnv* env, jobject thiz, jstring kdm_path) {
    const char* kdmPath = env->GetStringUTFChars(kdm_path, nullptr);

    KdmInfomationPtr kdmInfoPtr = nullptr;
    int result = _dms_validate_kdm(kdmPath, &kdmInfoPtr);

    if (result != DMS_RESULT_SUCCESS || kdmInfoPtr == nullptr) {
        LOGE("KDM validation failed: 0x%08x", result);
        env->ReleaseStringUTFChars(kdm_path, kdmPath);
        return nullptr;
    }

// 创建Java KdmInfo对象
    jobject kdmInfo = env->NewObject(gKdmInfoClass,
                                     env->GetMethodID(gKdmInfoClass, "<init>", "()V"));

// 设置字段值
    if (kdmInfoPtr->Id != nullptr) {
        jstring id = env->NewStringUTF(kdmInfoPtr->Id);
        env->SetObjectField(kdmInfo, gKdmInfo_id, id);
        env->DeleteLocalRef(id);
    }

    if (kdmInfoPtr->RecipientSubjectName != nullptr) {
        jstring recipient = env->NewStringUTF(kdmInfoPtr->RecipientSubjectName);
        env->SetObjectField(kdmInfo, gKdmInfo_recipientSubjectName, recipient);
        env->DeleteLocalRef(recipient);
    }

    if (kdmInfoPtr->CplId != nullptr) {
        jstring cplId = env->NewStringUTF(kdmInfoPtr->CplId);
        env->SetObjectField(kdmInfo, gKdmInfo_cplId, cplId);
        env->DeleteLocalRef(cplId);
    }

    if (kdmInfoPtr->ContentTitle != nullptr) {
        jstring contentTitle = env->NewStringUTF(kdmInfoPtr->ContentTitle);
        env->SetObjectField(kdmInfo, gKdmInfo_contentTitle, contentTitle);
        env->DeleteLocalRef(contentTitle);
    }

    if (kdmInfoPtr->NotValidBefore[0] != '\0') {
        jstring notValidBefore = env->NewStringUTF(kdmInfoPtr->NotValidBefore);
        env->SetObjectField(kdmInfo, gKdmInfo_notValidBefore, notValidBefore);
        env->DeleteLocalRef(notValidBefore);
    }

    if (kdmInfoPtr->NotValidAfter[0] != '\0') {
        jstring notValidAfter = env->NewStringUTF(kdmInfoPtr->NotValidAfter);
        env->SetObjectField(kdmInfo, gKdmInfo_notValidAfter, notValidAfter);
        env->DeleteLocalRef(notValidAfter);
    }

// 设置整型字段值，进行类型转换以避免警告
    env->SetIntField(kdmInfo, gKdmInfo_sessionCount,  static_cast<jint>(kdmInfoPtr->SessionCount));
    env->SetIntField(kdmInfo, gKdmInfo_remainSessionCount, static_cast<jint>(kdmInfoPtr->RemainSessionCount));
    env->SetIntField(kdmInfo, gKdmInfo_validateTimeWindowResult, kdmInfoPtr->ValidateTimeWindowResult);
    env->SetIntField(kdmInfo, gKdmInfo_validateRecipientResult, kdmInfoPtr->ValidateRecipientResult);

    _dms_free_kdm_infomation(&kdmInfoPtr);
    env->ReleaseStringUTFChars(kdm_path, kdmPath);

    return kdmInfo;
}

/**
 * 绑定KDM文件
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @param kdm_path KDM文件路径
 * @return 绑定成功返回JNI_TRUE，失败返回JNI_FALSE
 */
JNIEXPORT jboolean JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_bindKdm(JNIEnv* env, jobject thiz, jstring kdm_path) {
    const char* kdmPath = env->GetStringUTFChars(kdm_path, nullptr);
    int result = _dms_bind_kdm(kdmPath);
    env->ReleaseStringUTFChars(kdm_path, kdmPath);

    return (result == DMS_RESULT_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

/**
 * 打开MXF文件
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @param mxf_path MXF文件路径
 * @return MXF信息对象，打开失败返回nullptr
 */
JNIEXPORT jobject JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_openMxf(JNIEnv* env, jobject thiz, jstring mxf_path) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context == nullptr || !context->isInitialized) {
        LOGE("DMS player not initialized");
        return nullptr;
    }

    const char* mxfPath = env->GetStringUTFChars(mxf_path, nullptr);

// 暂时使用DCP函数打开MXF文件
// 这是一个临时解决方案，直到我们有直接的MXF支持
    int result = _dms_open_dcp(mxfPath, "dummy-session-id", false);

    if (result != DMS_RESULT_SUCCESS) {
        LOGE("Failed to open MXF file: 0x%08x", result);
        env->ReleaseStringUTFChars(mxf_path, mxfPath);
        return nullptr;
    }

    context->hasActiveMxf = true;
    env->ReleaseStringUTFChars(mxf_path, mxfPath);

// 创建Java MxfInfo对象并设置默认数据
// 在实际实现中，我们会从MXF文件中提取这些信息
    jobject mxfInfo = env->NewObject(gMxfInfoClass,
                                     env->GetMethodID(gMxfInfoClass, "<init>", "()V"));

    env->SetIntField(mxfInfo, gMxfInfo_width, 1920);
    env->SetIntField(mxfInfo, gMxfInfo_height, 1080);
    env->SetFloatField(mxfInfo, gMxfInfo_frameRate, 24.0f);
    env->SetLongField(mxfInfo, gMxfInfo_duration, 120 * 1000000L); // 120秒，单位为微秒
    env->SetBooleanField(mxfInfo, gMxfInfo_isEncrypted, true);

    jstring codec = env->NewStringUTF("JPEG2000");
    env->SetObjectField(mxfInfo, gMxfInfo_codec, codec);
    env->DeleteLocalRef(codec);

    return mxfInfo;
}

/**
 * 关闭MXF文件
 * @param env JNI环境指针
 * @param thiz Java对象引用
 */
JNIEXPORT void JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_closeMxf(JNIEnv* env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context != nullptr && context->hasActiveMxf) {
        _dms_close_dcp();
        context->hasActiveMxf = false;
        context->currentPosition = 0;
    }
}

/**
 * 开始播放
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @return 开始成功返回JNI_TRUE，失败返回JNI_FALSE
 */
JNIEXPORT jboolean JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_startPlayback(JNIEnv* env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context == nullptr || !context->hasActiveMxf) {
        LOGE("No active MXF file");
        return JNI_FALSE;
    }

// 重置位置到开始
    context->currentPosition = 0;

    return JNI_TRUE;
}

/**
 * 停止播放
 * @param env JNI环境指针
 * @param thiz Java对象引用
 */
JNIEXPORT void JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_stopPlayback(JNIEnv* env, jobject thiz) {
// 暂时不需要特殊操作
}

/**
 * 获取下一帧数据
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @param buffer Java字节数组缓冲区
 * @return 实际复制的字节数，出错返回-1
 */
JNIEXPORT jint JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_getNextFrame(JNIEnv* env, jobject thiz, jbyteArray buffer) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context == nullptr || !context->hasActiveMxf) {
        LOGE("No active MXF file");
        return -1;
    }

    jbyte* bufferPtr = env->GetByteArrayElements(buffer, nullptr);
    jsize bufferLength = env->GetArrayLength(buffer);

    DmsDataUnitPtr dataUnit = nullptr;
    int result = _dms_get_next_picture_unit(&dataUnit);

    if (result != DMS_RESULT_SUCCESS || dataUnit == nullptr) {
        env->ReleaseByteArrayElements(buffer, bufferPtr, 0);
        return -1; // 流结束或出错
    }

// 将帧数据复制到Java缓冲区
    int bytesToCopy = dataUnit->Length;
    if (bytesToCopy > bufferLength) {
        LOGE("Buffer too small for frame data: %d > %d", bytesToCopy, bufferLength);
        bytesToCopy = bufferLength;
    }

    memcpy(bufferPtr, dataUnit->Data, bytesToCopy);
    context->currentPosition = dataUnit->Pos;

    _dms_free_data_unit(&dataUnit);
    env->ReleaseByteArrayElements(buffer, bufferPtr, 0);

    return bytesToCopy;
}

/**
 * 跳转到指定位置
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @param positionUs 目标位置（微秒）
 */
JNIEXPORT void JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_seekTo(JNIEnv* env, jobject thiz, jlong positionUs) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

    if (context == nullptr || !context->hasActiveMxf) {
        LOGE("No active MXF file");
        return;
    }

// 将微秒转换为字节位置（这是一个简化的处理方式）
// 在实际实现中，我们需要将时间映射到文件位置
    int64_t targetPosition = (positionUs * 1000000) / 24; // 假设24fps和恒定比特率

    int result = _dms_goto_pos(targetPosition, false);
    if (result == DMS_RESULT_SUCCESS) {
        context->currentPosition = targetPosition;
    } else {
        LOGE("Seek failed: 0x%08x", result);
    }
}

/**
 * 获取媒体持续时间
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @return 持续时间（微秒）
 */
JNIEXPORT jlong JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_getDuration(JNIEnv* env, jobject thiz) {
// 返回持续时间（微秒）- 示例为2分钟
    return 120 * 1000000L;
}

/**
 * 获取帧率
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @return 帧率
 */
JNIEXPORT jfloat JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_getFrameRate(JNIEnv* env, jobject thiz) {
    return 24.0f;
}

/**
 * 检查文件是否加密
 * @param env JNI环境指针
 * @param thiz Java对象引用
 * @return 加密返回JNI_TRUE，未加密返回JNI_FALSE
 */
JNIEXPORT jboolean JNICALL
Java_com_djs_djsdmsplayer_DmsPlayer_isEncrypted(JNIEnv* env, jobject thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(clazz, "nativePtr", "J");
    DmsContext* context = reinterpret_cast<DmsContext*>(env->GetLongField(thiz, fieldId));

// 为了演示目的，假设如果绑定了KDM则文件已加密
    return (context != nullptr && context->hasActiveMxf) ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
