
// AliOssUploadDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAliOssUploadDlg 对话框
class CAliOssUploadDlg : public CDialogEx
{
// 构造
public:
	CAliOssUploadDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ALIOSSUPLOAD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnDropFiles(HDROP hDropInfo);
	
	CListCtrl m_listsrc;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void copyToFile(CString src,CString desc);
	afx_msg void OnBnClickedButtonUpload();
	CListBox m_listLog;
	afx_msg void OnBnClickedButtonOssInfo();
	CString m_szOssPath;
	BOOL m_bIsGzipCompress;
};
