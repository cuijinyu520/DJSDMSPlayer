/*
 * DMS Device Register Library
 * Copyright(C) DMCC, Apr. 2025
 *
 * @file    libdmsreg.h
 * @version 1.0.0.7
 * @author  Liu Jiannan
 * @email   liujiannan@dmcc.org.cn
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#if !defined __cplusplus
#define nullptr	NULL
#elif __cplusplus < 201103L
#define nullptr	NULL
#endif

/**
 * @brief 错误代码定义
 */
#define DMSREG_RESULT_SUCCESS						0x00000000		// 正确、有效等
#define DMSREG_RESULT_UNKNOWN_ERROR					0x80000000		// 未知错误
#define DMSREG_RESULT_LIB_NOT_INITIALIZED			0x80000001		// 库未初始化
#define DMSREG_RESULT_LIB_INITIALIZED				0x80000002		// 库已初始化
#define DMSREG_RESULT_OPEN_HID_FILE_ERROR			0x80000003		// 打开HID文件错误
#define DMSREG_RESULT_VALIDATE_HID_ERROR			0x80000004		// 验证HID错误
#define DMSREG_RESULT_OPEN_AUTH_DB_ERROR			0x80000005		// 打开授权数据库错误
#define DMSREG_RESULT_LOAD_KEY_ERROR				0x80000006		// 载入密钥错误
#define DMSREG_RESULT_NO_CERT_LOADED				0x80000007		// 没有证书载入
#define DMSREG_RESULT_SIGN_MESSAGE_ERROR			0x80000008		// 签名错误
#define DMSREG_RESULT_VERIFY_MESSAGE_ERROR			0x80000009		// 验签错误
#define DMSREG_RESULT_PARSE_MESSAGE_ERROR			0x8000000A		// 解析消息错误
#define DMSREG_RESULT_MESSAGE_TIME_ERROR			0x8000000B		// 消息时间错误
#define DMSREG_RESULT_UPDATE_PARAM_ERROR			0x8000000C		// 更新参数错误
#define DMSREG_RESULT_DATABASE_UPDATED				0x8000000D		// 数据库已更新

/**
* @brief 事件定义
*/
#define EVENT_REGISTER_SUCCESS						0x00000000		// 注册成功
#define EVENT_REGISTER_FAIL							0x80000000		// 注册失败

/**
* @brief 配置KEY代码定义
*/
#define PARAM_REMOTE_ADDRESS						1				// 远程播映服务器地址
#define PARAM_REMOTE_PORT							2				// 远程播映服务器端口
#define PARAM_REMOTE_TOPIC_PUSH						3				// 远程播映服务上行主题，设备->服务器
#define PARAM_REMOTE_TOPIC_ACCEPT					4				// 远程播映服务下行主题，服务器->设备，设备需订阅下行主题
#define PARAM_REMOTE_USERNAME						5				// 远程播映服务器用户名
#define PARAM_REMOTE_PASSWORD						6				// 远程播映服务器密码
#define PARAM_REMOTE_LOCK_FLAG						7				// 锁定标志
#define PARAM_REMOTE_LOG_REPORT_URL					8				// 放映日志上报地址
#define PARAM_REMOTE_HEART_BEAT_INTERVAL			9				// 心跳间隔，以秒为单位


#ifdef __cplusplus
extern "C"
{
#endif


/**
* @brief 获取库版本号
* @param 无
* @return 版本号字符串
*/
const char* _dms_get_library_version();


/**
 * @brief 注册事件处理回调函数
 * @param iEvent 事件
 * @param pData 事件附加信息
 * @return 无
 */
typedef void (*EventHandlerFun)(int iEvent, void* pData);


/**
 * @brief 事件处理注册函数
 * @param pEventHandler 注册事件处理回调函数指针
 * @return 无
 */
void _dms_register_event_handler(EventHandlerFun pEventHandler);


/**
 * @brief 注册消息处理函数
 * @param pRequestMessage 请求消息缓存指针
 * @param iRequestMessageLength 请求消息缓存长度
 * @param pResponseMessage 返回响应消息缓存指针，需要使用_dms_free_message进行释放
 * @return 错误返回DMSREG_RESULT_UNKNOWN_ERROR，正确返回响应消息缓存长度
 */
int _dms_process_message(const uint8_t* pRequestMessage, int iRequestMessageLength, uint8_t** pResponseMessage);


/**
 * @brief 释放消息缓存
 * @param pMessage 消息缓存指针
 * @return 无
 */
void _dms_free_message(uint8_t* pMessage);


/**
 * @brief 设置是否输出日志
 * @param bEnableFlag为true时输出日志，false时不输出日志
 * @return 无
 */
void _dms_enable_log(bool bEnableFlag);


/**
 * @brief 更新注册文件到1.0.1.37
 * @brief 调用此函数前不能调用_dms_remote_initialize函数和libdms库中的_dms_library_initialize函数
 * @param 无
 * @return 正确返回DMS_RESULT_SUCCESS，错误返回错误代码；如在已升级注册文件，函数将返回DMSREG_RESULT_DATABASE_UPDATED
 */
int _dms_update_1_0_1_37();


/**
 * @brief 初始化库
 * @brief 所有_dms_remote开头函数都要要初始化之后使用，初始化之后_dms_process_message将无法使用
 * @param 无
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_remote_initialize();


/**
 * @brief 释放库
 * @param 无
 * @return 无
 */
void _dms_remote_uninitialize();


/**
 * @brief 远程播映消息签名函数
 * @param szRemoteMessage 消息字符串
 * @param pSignedRemoteMessage 返回签名后的消息字符串指针，需要使用_dms_remote_free_message进行释放
 * @return 错误返回错误代码（错误代码小于0），正确返回签名长度
 */
int _dms_remote_sign_message(const char* szRemoteMessage, char** pSignedRemoteMessage);


/**
 * @brief 远程播映消息签名验证函数
 * @param szRemoteMessage 消息缓存指针
 * @return 错误返回错误代码（错误代码小于0），正确返回DMSREG_RESULT_SUCCESS
 */
int _dms_remote_verify_message(const char* szRemoteMessage);


/**
 * @brief 释放消息缓存
 * @param szMessage 消息字符串
 * @return 无
 */
void _dms_remote_free_message(char* szMessage);


/**
 * @brief 获取参数
 * @param iParamKey 配置KEY代码，参见PARAM_REMOTE_定义
 * @return 配置字符串，如果配置应为整形数字也同样以字符串返回
 * @return 返回字符串缓存在下一次调用_dms_remote_get_param时会被覆盖
 */
const char* _dms_remote_get_param(int iParamKey);


#ifdef __cplusplus
}
#endif
