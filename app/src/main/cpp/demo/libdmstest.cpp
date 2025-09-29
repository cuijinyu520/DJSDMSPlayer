/*
 * DMS Digital Cinema Package Library Demo
 * Copyright(C) DMCC, Mar.2024
 *
 * @file    libdmstest.cpp
 * @version 0.3
 * @author  Liu Jiannan
 * @email   liujiannan@dmcc.org.cn
 */
#include <stdio.h>
#include "../../include/libdms.h"

/*
 * 获取设备地理位置信息的模拟函数
 *
 * @param[out] fLongitude 经度值（输出参数）
 * @param[out] fLatitude  纬度值（输出参数）
 * @param[out] szCarrier  运营商名称（输出参数）
 * @param[out] szLBS      基站编号（输出参数）
 * @return                返回0表示成功
 */
int GetLocation(float* fLongitude, float* fLatitude, char* szCarrier, char* szLBS)
{
    *fLongitude = 39.26;
    *fLatitude = 115.25;
    sprintf(szCarrier, "CMCC");
    sprintf(szLBS, "001");
    return 0;
}

/*
 * 测试DMS库核心功能的主函数
 * 包括初始化、证书获取、KDM验证、DCP打开、绑定KDM、读取影片扩展信息等流程
 *
 * @param[in] szKdm KDM文件路径
 * @param[in] szDcp DCP文件夹路径
 * @return          返回0表示成功
 */
int LibTest(const char* szKdm, const char* szDcp)
{
    //证书信息结构
    CertificateInfomationPtr pCertificateInfomation = nullptr;
    //密钥分发消息（KDM）信息
    KdmInfomationPtr pKdmInfomation = nullptr;
    // 初始化DMS库
    printf("[Info][_dms_library_initialize result: 0x%08x]\n", _dms_library_initialize(DMS_MODE_PLAY, nullptr, true));

    // 获取设备证书信息
    printf("[Info][_dms_get_certificate_infomation result: 0x%08x]\n", _dms_get_certificate_infomation(&pCertificateInfomation));
    if (pCertificateInfomation)
    {
        printf("--> Certificate Infomation:\n");
        printf(" |->DeviceSerial: %lu\n", pCertificateInfomation->DeviceSerial);
        printf(" |->IssuerName: %s\n", pCertificateInfomation->IssuerName);
        printf(" |->SerialNumber: %s\n", pCertificateInfomation->SerialNumber);
        printf(" |->SubjectName: %s\n", pCertificateInfomation->SubjectName);
//        printf(" |->NotBefore: %s\n", FormateDatetimeSmpte(pCertificateInfomation->NotBefore).c_str());
//        printf(" |->NotAfter: %s\n", FormateDatetimeSmpte(pCertificateInfomation->NotAfter).c_str());
        printf(" |->NotBefore: %s\n", pCertificateInfomation->NotBefore);
        printf(" |->NotAfter: %s\n", pCertificateInfomation->NotAfter);
    }

    // 验证KDM文件有效性并解析其内容
    printf("[Info][_dms_validate_kdm result: 0x%08x]\n",
        _dms_validate_kdm(szKdm, &pKdmInfomation));
    if (pKdmInfomation)
    {
        printf("--> KDM Infomation:\n");
        printf(" |->Id: %s\n", pKdmInfomation->Id);
        printf(" |->RecipientSubjectName: %s\n", pKdmInfomation->RecipientSubjectName);
        printf(" |->CplId: %s\n", pKdmInfomation->CplId);
        printf(" |->ContentTitle: %s\n", pKdmInfomation->ContentTitle);
        printf(" |->NotValidBefore: %s\n", pKdmInfomation->NotValidBefore);
        printf(" |->NotValidAfter: %s\n", pKdmInfomation->NotValidAfter);
        printf(" |->SessionCount: %d\n", pKdmInfomation->SessionCount);
        printf(" |->RemainSessionCount: %d\n", pKdmInfomation->RemainSessionCount);
        printf(" |->ValidateTimeWindowResult: 0x%08x\n", pKdmInfomation->ValidateTimeWindowResult);
        printf(" |->ValidateRecipientResult: 0x%08x\n", pKdmInfomation->ValidateRecipientResult);
        _dms_free_kdm_infomation(&pKdmInfomation);
    }

    //打开DCP并获取影片扩展信息
    printf("[Info][_dms_open_dcp result: 0x%08x]\n", _dms_open_dcp(szDcp,"", true));
    DmsMovieExtensionPtr pDmsMovieExtension = _dms_get_movie_extension();
    if (pDmsMovieExtension)
    {
        printf("--> Dms Movie Extension\n");
        printf(" |->Title: %s\n", pDmsMovieExtension->Title);
        printf(" |->Director: %s\n", pDmsMovieExtension->Director);
        printf(" |->Editor: %s\n", pDmsMovieExtension->Editor);
        printf(" |->Cast: %s\n", pDmsMovieExtension->Cast);
        printf(" |->Label: %s\n", pDmsMovieExtension->Label);
        printf(" |->Country: %s\n", pDmsMovieExtension->Country);
        printf(" |->Duration: %d\n", pDmsMovieExtension->Duration);
        printf(" |->Intro: %s\n", pDmsMovieExtension->Intro);
        printf(" |->Manufacture Date: %d\n", pDmsMovieExtension->ManufactureDate);
        printf(" |->Poster File Type: %s\n", pDmsMovieExtension->PosterFileType);
        printf(" |->Poster Length: %d\n", pDmsMovieExtension->PosterLength);
    }
    _dms_free_movie_extension(&pDmsMovieExtension);

    // 将KDM与DCP进行绑定
    printf("[Info][_dms_bind_kdm result: 0x%08x]\n", _dms_bind_kdm(szKdm));

    // 获取下一帧图像数据单元
    DmsDataUnitPtr data = nullptr;
    int iReturn;
    iReturn = _dms_get_next_picture_unit(&data);
    printf("[Info][_dms_get_next_picture_unit result: 0x%08x]\n", iReturn);
    if (data) _dms_free_data_unit(&data);

    // 反初始化DMS库
    _dms_library_uninitialize();

    return 0;
}

/*
 * 程序入口函数
 * 显示程序版本信息并调用LibTest执行测试流程
 *
 * @param[in] argc 命令行参数个数
 * @param[in] argv 命令行参数数组
 * @return         返回0表示程序正常退出
 */
int main(int argc, char* argv[])
{
    printf("  ________  _____ ______   ________  ________      ____  __  __ ____    _     _ _                            _____         _\n");
    printf(" |\\   ___ \\|\\   _ \\  _   \\|\\   ____\\|\\   ____\\    |  _ \\|  \\/  / ___|  | |   (_) |__  _ __ __ _ _ __ _   _  |_   _|__  ___| |_\n");
    printf(" \\ \\  \\_|\\ \\ \\  \\\\\\__\\ \\  \\ \\  \\___|\\ \\  \\___|    | | | | |\\/| \\___ \\  | |   | | '_ \\| '__/ _` | '__| | | |   | |/ _ \\/ __| __|\n");
    printf("  \\ \\  \\ \\\\ \\ \\  \\\\|__| \\  \\ \\  \\    \\ \\  \\       | |_| | |  | |___) | | |___| | |_) | | | (_| | |  | |_| |   | |  __/\\__ \\ |_\n");
    printf("   \\ \\  \\_\\\\ \\ \\  \\    \\ \\  \\ \\  \\____\\ \\  \\____  |____/|_|  |_|____/  |_____|_|_.__/|_|  \\__,_|_|   \\__, |   |_|\\___||___/\\__|\n");
    printf("    \\ \\_______\\ \\__\\    \\ \\__\\ \\_______\\ \\_______\\  Version 0.3, For internal test use only          |___/\n");
    printf("     \\|_______|\\|__|     \\|__|\\|_______|\\|_______| (C)Copyrght DMCC, Mar.2025, Written by Liujiannan\n\n");

    LibTest(
        "/test/kdm/KDM_流浪地球演示片段_FOR_195206_cert_F16DDFF45F3D60E1E050A8C0030506F8.xml",
        "/test/dcp/流浪地球演示片段_8ed8c23e1bc4");
 
	return 0;
}
