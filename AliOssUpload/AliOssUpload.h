
// AliOssUpload.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAliOssUploadApp:
// �йش����ʵ�֣������ AliOssUpload.cpp
//

class CAliOssUploadApp : public CWinApp
{
public:
	CAliOssUploadApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAliOssUploadApp theApp;