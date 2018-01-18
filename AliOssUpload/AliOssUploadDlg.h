
// AliOssUploadDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAliOssUploadDlg �Ի���
class CAliOssUploadDlg : public CDialogEx
{
// ����
public:
	CAliOssUploadDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ALIOSSUPLOAD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
