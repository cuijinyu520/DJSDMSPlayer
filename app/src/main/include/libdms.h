/*
 * DMS Digital Cinema Package Library
 * Copyright(C) DMCC, Sep. 2025
 *
 * @file    libdms.h
 * @version 1.0.1.38
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
#define DMS_RESULT_SUCCESS							0x00000000		// 正确、有效等
#define DMS_RESULT_UNKNOWN_ERROR					0x80000000		// 未知错误
#define DMS_RESULT_OPEN_AUTH_DB_ERROR				0x80000001		// 打开授权数据库错误
#define DMS_RESULT_LOAD_KEY_PAIR_ERROR				0x80000002		// 载入密钥对错误
#define DMS_RESULT_LIB_NOT_INITIALIZED				0x80000003		// 库未初始化
#define DMS_RESULT_NO_KDM_SELECTED					0x80000004		// 未选择KDM
#define DMS_RESULT_KDM_NOT_EXIST					0x80000005		// KDM文件不存在
#define DMS_RESULT_KDM_SIGNATURE_ERROR				0x80000006		// KDM签名错误
#define DMS_RESULT_PARSE_KDM_ERROR					0x80000007		// KDM解析错误
#define DMS_RESULT_KDM_GET_NOTBEFORE_ERROR			0x80000008		// 获取KDM授权起始时间错误
#define DMS_RESULT_KDM_GET_NOTAFTER_ERROR			0x80000009		// 获取KDM授权结束时间错误
#define DMS_RESULT_KDM_INACTIVE						0x8000000A		// KDM未生效
#define DMS_RESULT_KDM_EXPIRE						0x8000000B		// KDM过期
#define DMS_RESULT_KDM_GET_RECIPIENT_ERROR			0x8000000C		// 获取KDM接收者错误
#define DMS_RESULT_KDM_RECIPIENT_ERROR				0x8000000D		// KDM接收者与设备不匹配
#define DMS_RESULT_KDM_INSUFFICIENT_SESSION_COUNT	0x8000000E		// KDM授权场次数不足
#define DMS_RESULT_KDM_DECRYPT_ERROR				0x8000000F		// KDM解密错误
#define DMS_RESULT_NO_MXF_OPENED					0x80000010		// 没有打开的MXF文件
#define DMS_RESULT_MXF_PARSE_ERROR					0x80000011		// MXF解析错误
#define DMS_RESULT_NO_ENCRYPT_CONTEXT				0x80000012		// 未设置加密上下文，加密轨迹文件未设置KDM
#define DMS_RESULT_NO_PICTURE_ESSENCE_FOUND			0x80000013		// 没有找到图像实体，当Mxf到结尾时也返回此错误代码
#define DMS_RESULT_WRITE_MOVIE_PLAY_LOG_ERROR		0x80000014		// 写入播放日志错误
#define DMS_RESULT_DCP_PATH_NOT_EXIST				0x80000015		// DCP目录不存在
#define DMS_RESULT_NO_DCP_OPENED					0x80000016		// 没有打开的DCP
#define DMS_RESULT_VOLINDEX_NOT_EXIST				0x80000017		// Volindex不存在
#define DMS_RESULT_ASSETMAP_NOT_EXIST				0x80000018		// AssetMap不存在
#define DMS_RESULT_LOAD_ASSETMAP_ERROR				0x80000019		// 载入AssetMap错误
#define DMS_RESULT_PKL_NOT_FOUND					0x8000001A		// PackingList未找到
#define DMS_RESULT_PKL_NOT_EXIST					0x8000001B		// PackingList不存在
#define DMS_RESULT_LOAD_PKL_ERROR					0x8000001C		// 载入PackingList错误
#define DMS_RESULT_CPL_NOT_EXIST					0x8000001D		// CompositionPlaylist不存在
#define DMS_RESULT_LOAD_CPL_ERROR					0x8000001E		// 载入CompositionPlaylist错误
#define DMS_RESULT_MXF_NOT_EXIST					0x8000001F		// MXF不存在
#define DMS_RESULT_KEY_NOT_FOUND					0x80000020		// 密钥未找到，MXF与KDM不匹配
#define DMS_RESULT_KDM_NOT_MATCH_WITH_DCP			0x80000021		// DCP与KDM不匹配
#define DMS_RESULT_REEL_NOT_EXIST					0x80000022		// 分本不存在
#define DMS_RESULT_LOGO_PLAYED						0x80000023		// 片头已播放
#define DMS_RESULT_INIT_FILTER_ERROR				0x80000024		// 初始化过滤器失败
#define DMS_RESULT_LOGO_NOT_EXIST					0x80000025		// LOGO不存在
#define DMS_RESULT_PARSE_LOGO_ERROR					0x80000026		// 解析LOGO错误
#define DMS_RESULT_NULL_POINTER_ERROR				0x80000027		// 空指针错误
#define DMS_RESULT_PLAY_FINISHED					0x80000028		// 播放结束
#define DMS_RESULT_PREVIEW_FINISHED					0x80000029		// 预览结束
#define DMS_RESULT_BREAKPOINT_FINISHED				0x8000002A		// 断点续播结束
#define DMS_RESULT_VALIDATE_HID_ERROR				0x8000002B		// 验证HID错误
#define DMS_RESULT_ILLEGAL_MODE						0x8000002C		// 错误工作模式
#define DMS_RESULT_LIB_INITIALIZED					0x8000002D		// 库已初始化
#define DMS_RESULT_LOAD_PARAMETER_ERROR				0x8000002E		// 载入参数错误
#define DMS_RESULT_DEVICE_LOCKED					0x8000002F		// 设备已锁定
#define DMS_RESULT_REPORT_LOG_ERROR					0x80000030		// 上报日志错误
#define DMS_RESULT_REPORT_LOG_USER_BREAK			0x80000031		// 用户终端日志上报

// 工作模式定义
#define DMS_MODE_PLAY								0x00000001		// 播放模式
#define DMS_MODE_PARSE								0x00000002		// 解析模式，此模式下GetNextPictureDataUnit、GotoPicturePos、
// SetPlaybackEnded函数无法使用

/**
 * @brief 数据单元结构
 */
typedef struct tagDmsDataUnit
{
    int64_t Pos;		// 码流位置
    int64_t	PTS;		// 显示时间戳
    uint32_t Length;	// 数据单元长度
    uint8_t* Data;		// 数据单元指针
} DmsDataUnit, * DmsDataUnitPtr;


/**
 * @brief Dms影片扩展信息结构
 */
typedef struct tagDmsMovieExtension
{
    char CplId[46];					// CPL UUID
    char* Title;					// 影片名称
    char* Director;					// 导演
    char* Editor;					// 编剧
    char* Cast;						// 主演
    char* Label;					// 类型
    char* Country;					// 国家
    uint16_t Duration;				// 片长(单位：秒)
    char* Intro;					// 简介
    uint16_t ManufactureDate;		// 出品年代
    char PosterFileType[6];			// 海报文件类型（文件扩展名，不带点）
    uint32_t PosterLength;			// 海报长度
    uint8_t* Poster;				// 海报
}DmsMovieExtension, * DmsMovieExtensionPtr;

/**
 * @brief 证书信息结构
 */
typedef struct tagCertificateInfomation
{
    uint64_t DeviceSerial;	// 设备序列号
    char* IssuerName;		// 签发者名称
    char* SerialNumber;		// 证书序列号
    char* SubjectName;		// 主题名
    time_t NotBefore;		// 有效期开始时间
    time_t NotAfter;		// 有效期结束时间
}CertificateInfomation, * CertificateInfomationPtr;


/**
 * @brief 密钥分发消息（KDM）信息结构
 */
typedef struct tagKdmInfomation
{
    char* Id;						// 密钥分发消息（KDM）标识
    char* RecipientSubjectName;		// 接收者主题名称
    char* CplId;					// 对应复合播放列表（CPL）标识
    char* ContentTitle;				// 影片名称
    char NotValidBefore[26];		// 授权开始时间
    char NotValidAfter[26];			// 授权结束时间
    uint32_t SessionCount;			// 授权场次数
    uint32_t RemainSessionCount;	// 剩余场次数，当KDM接收者与设备不匹配时值为0
    int ValidateTimeWindowResult;	// DMS_RESULT_SUCCESS：KDM有效期在时间窗范围内
    // DMS_RESULT_KDM_INACTIVE：KDM未生效
    // DMS_RESULT_KDM_EXPIRE：KDM过期
    int ValidateRecipientResult;	// DMS_RESULT_SUCCESS：KDM接收者与设备匹配
    // DMS_RESULT_KDM_RECIPIENT_ERROR：KDM接收者与设备不匹配
}KdmInfomation, * KdmInfomationPtr;


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
 * @brief 获取定位回调函数
 * @param fLongitude 输出经度
 * @param fLatitude 输出纬度
 * @param szCarrier 运营商名称，最大长度50字节
 * @param szLBS 基站信息，最大长度50字节
 * @return 正确返回0，否则返回错误代码（错误代码应小于0）
 */
typedef int (*GetLocationFun)(float* fLongitude, float* fLatitude, char* szCarrier, char* szLBS);


/**
 * @brief 初始化库
 * @param iMode工作模式，DMS_MODE_PLAY播放模式，DMS_MODE_PARSE解析模式
 * @param pGetLocation获取定位回调函数指针，如果未提供输入nullptr
 * @param bEnableLogFlag为true时输出日志，false时不输出日志
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_library_initialize(int iMode, GetLocationFun pGetLocation, bool bEnableLogFlag);


/**
 * @brief 释放库
 * @param 无
 * @return 无
 */
void _dms_library_uninitialize();


/**
 * @brief 设置是否输出日志，_dms_library_initialize之后调用生效
 * @param bEnableFlag为true时输出日志，false时不输出日志
 * @return 无
 */
void _dms_enable_log(bool bEnableFlag);


/**
 * @brief 获取证书信息
 * @param ppCertificateInfomationPtr 输出CertificateInfomation指针
 * @return 正确返回DMS_RESULT_SUCCESS，证书信息通过参数指针返回；否则返回错误代码，存在错误时参数返回nullptr。
 */
int _dms_get_certificate_infomation(CertificateInfomationPtr* ppCertificateInfomationPtr);


/**
 * @brief 释放KDM信息
 * @param ppKdmInfomation KdmInfomation指针
 * @return 无
 */
void _dms_free_kdm_infomation(KdmInfomationPtr* ppKdmInfomation);


/**
 * @brief 验证KDM信息
 * @param szKdmPathname KDM文件完整路径
 * @param ppKdmInfomation 输出KdmInfomation指针
 * @return 正确返回DMS_RESULT_SUCCESS，证书信息通过参数指针返回；否则返回错误代码，存在错误时参数返回nullptr。
 */
int _dms_validate_kdm(const char* szKdmPathname, KdmInfomationPtr* ppKdmInfomation);


/**
 * @brief 绑定KDM
 * @param szKdmPathname KDM文件完整路径
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_bind_kdm(const char* szKdmPathname);


/**
 * @brief 获取DCP包信息
 * @param szPathname DCP包路径
 * @param ppDmsMovieExtension 返回Dms影片扩展信息指针
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_get_dcp_info(const char* szPathname, DmsMovieExtensionPtr* ppDmsMovieExtension);


/**
 * @brief 打开DCP包，如果DCP包中的轨迹文件是加密的需调用_dms_bind_kdm绑定KDM
 * @param szPathname DCP包路径
 * @param szSessionId 放映场次标识，同一场次应与农村公益电影公共服务平台远程播映接口（MQTT）终端放映日志列表（playlogList）
 *                    playlog对象的sessionId字段值相同，与_dms_set_playback_ended中的sessionId参数也应相同
 *                    当使用断点续播时，续播使用的sessionId应与之前播放开始时的sessionId一致
 *	                  UUID类型，格式为urn:uuid:4hexOctet-2hexOctet-2hexOctet-2hexOctet-6hexOctet
 *                    参数例：urn:uuid:dd4e577a-22c4-4ca5-95c7-0a479683e60c
 * @param bPreviewModeFlag 预览模式标志，如果为true开启预览模式，可以预览5分钟正片不扣场次，正常播放填写false
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_open_dcp(const char* szPathname, const char* szSessionId, bool bPreviewModeFlag);


/**
 * @brief 关闭DCP包
 * @param 无
 * @return 无
 */
void _dms_close_dcp();


/**
 * @brief 获取分本数量
 * @param 无
 * @return 正确返回分本数量（应大于零），否则返回错误代码（小于零）
 */
int _dms_get_reel_count();


/**
 * @brief 选择分本
 * @param iReelNumber，分本编号
 * @return 正确返回DMS_RESULT_SUCCESS，否则返回错误代码
 */
int _dms_select_reel(int iReelNumber);


/**
 * @brief 获取图像mxf文件ID信息，需要先_dms_open_dcp才可获得
 * @param 无
 * @return 正确返回mxf文件ID信息（字符串），错误返回nullptr
 */
const char* _dms_get_picture_mxf_id();


/**
 * @brief 读取Dms影片扩展信息，需要先_dms_open_dcp才可获得信息
 * @param 无
 * @return 正确返回Dms影片扩展信息指针，错误返回nullptr
 */
DmsMovieExtensionPtr _dms_get_movie_extension();


/**
 * @brief 释放Dms影片扩展信息
 * @param ppDmsMovieExtension DmsMovieExtension指针
 * @return 无
 */
void _dms_free_movie_extension(DmsMovieExtensionPtr* ppDmsMovieExtension);


/**
 * @brief 读取数据单元，需要先_dms_open_dcp才可读取（加密影片还需要_dms_bind_kdm）
 * @param ppDmsDataUnit 返回DmsDataUnit指针
 * @return 正确返回DMS_RESULT_SUCCESS，错误返回错误代码
 */
int _dms_get_next_picture_unit(DmsDataUnitPtr* ppDmsDataUnit);


/**
 * @brief 跳转到指定位置，需要先_dms_open_dcp才可操作
 * @param iPos 位置，注意位置需要根据DmsDataUnitPtr中返回的iPos进行设置
 * @param bBreakpointFlag 断点续播标志，true表示为断点续播，false表示非断点续播
 * @return 正确DMS_RESULT_SUCCESS，错误返回错误代码
 */
int _dms_goto_pos(int64_t iPos, bool bBreakpointFlag);


/**
 * @brief 释放数据单元
 * @param ppDmsDataUnit DmsDataUnit指针
 * @return 无
 */
void _dms_free_data_unit(DmsDataUnitPtr* ppDmsDataUnit);


/**
 * @brief 设置电影播放结束(只产生放映日志)，在影片播放完成后，DCP关闭前调用；影片预览不用调用
 * @param szStartDatetime 播放开始日期时间（UTC时间），格式 yyyy-mm-dd HH:MM:SS
 * @param szEndDatetime 播放结束日期时间（UTC时间），格式 yyyy-mm-dd HH:MM:SS
 * @return 正确DMS_RESULT_SUCCESS，错误返回错误代码
 */
int _dms_set_playback_ended(const char* szStartDatetime, const char* szEndDatetime);


/**
 * @brief 设置电影播放断点(只产生放映日志)，电影正常播放时应实时记录放映点，在出现断电的情况下再次开机时
 *        调用_dms_set_playback_break记录断电前记录的断点时间以及放映场次标识
 *        调用_dms_set_playback_break前，需要先调用_dms_open_dcp打开DCP包，并且调用_dms_bind_kdm绑定KDM
 * @param szDatetime 播放断点日期时间（UTC时间），格式 yyyy-mm-dd HH:MM:SS
 * @return 正确DMS_RESULT_SUCCESS，错误返回错误代码
 */
int _dms_set_playback_break(const char* szDatetime);


/**
 * @brief 日志上报通知回调函数
 * @param iTotalCount 总记录数
 * @param iCurrent 当前记录，记录从1开始
 * @return 正确返回0继续发送下一条记录，错误返回-1则中断日志上报_dms_report_log返回DMS_RESULT_REPORT_LOG_USER_BREAK
 */
typedef int (*ReportProgressFun)(int iTotalCount, int iCurrent);


/**
 * @brief 上报日志，可手动上报日志，解决部分设备厂商不允许对接库常驻内存的问题
 * @brief 在未调用_dms_library_initialize情况下可用
 * @brief 如果调用了_dms_library_initialize，则需要调用_dms_library_uninitialize后可用
 * @param pReportProgress日志上报通知回调函数指针，如果未提供输入nullptr
 * @return 上报成功返回DMS_RESULT_SUCCESS，失败返回错误代码
 * @return 当ReportProgressFun回调函数返回-1时，立刻中断上报并返回DMS_RESULT_REPORT_LOG_USER_BREAK
 */
int _dms_report_log(ReportProgressFun pReportProgress);


#ifdef __cplusplus
}
#endif