
// AliOssUploadDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AliOssUpload.h"
#include "AliOssUploadDlg.h"
#include "afxdialogex.h"

extern "C"
{
#include "oss_config.h"
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"

};

#include <zlib.h>
#include "GZip.h"
#include "StrUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAliOssUploadDlg �Ի���



CAliOssUploadDlg::CAliOssUploadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAliOssUploadDlg::IDD, pParent)
	, m_szOssPath(_T(""))
	, m_bIsGzipCompress(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAliOssUploadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_SRC, m_listsrc);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
	DDX_Text(pDX, IDC_EDIT_OSS_PATH, m_szOssPath);
	DDX_Check(pDX, IDC_CHECK_GZIP, m_bIsGzipCompress);
}

BEGIN_MESSAGE_MAP(CAliOssUploadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CAliOssUploadDlg::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_OSS_INFO, &CAliOssUploadDlg::OnBnClickedButtonOssInfo)
END_MESSAGE_MAP()


// CAliOssUploadDlg ��Ϣ�������

BOOL CAliOssUploadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	/*
	LVCOLUMN lc = {};
	lc.pszText = _T("·��");
	lc.fmt =  LVCFMT_LEFT  ;
	lc.cx = 500;
	lc.mask = LVCF_TEXT |LVCF_WIDTH;*/
	m_listsrc.InsertColumn(0,_T("·��"),LVCFMT_LEFT,450,0);
	m_listsrc.InsertColumn(1,_T("״̬"),LVCFMT_LEFT,100,0);



	CString buff;
	int buffLen = 200;
	CString curDir;
	DWORD newLength = GetModuleFileName(NULL,curDir.GetBuffer(MAX_PATH),MAX_PATH);
	curDir.ReleaseBuffer(newLength);
	curDir = curDir.Left(curDir.ReverseFind('\\')+1) + _T("config.ini");
	char fileName[MAX_PATH];

	convertWCharToChar(curDir,fileName);
	GetPrivateProfileStringA("oss","OSS_ENDPOINT","11",OSS_ENDPOINT,buffLen,fileName);
	GetPrivateProfileStringA("oss","ACCESS_KEY_ID","",ACCESS_KEY_ID,buffLen,fileName);
	GetPrivateProfileStringA("oss","ACCESS_KEY_SECRET","",ACCESS_KEY_SECRET,buffLen,fileName);
	GetPrivateProfileStringA("oss","BUCKET_NAME","",BUCKET_NAME,buffLen,fileName);
	CString zipBuf;
	GetPrivateProfileString(_T("system"),_T("gzip"),_T(""),zipBuf.GetBuffer(buffLen),buffLen,curDir);
	zipBuf.ReleaseBuffer();
	m_bIsGzipCompress = "1"==zipBuf;
	
	GetPrivateProfileString(_T("system"),_T("oss_dir"),_T(""),m_szOssPath.GetBuffer(buffLen),buffLen,curDir);
	UpdateData(FALSE);




	//��ʼ��oss
	if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
		exit(1);
	}


	ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME); 
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAliOssUploadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAliOssUploadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAliOssUploadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAliOssUploadDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int DropCount=DragQueryFile(hDropInfo,-1,NULL,0);//ȡ�ñ��϶��ļ�����Ŀ  
	for(int i=0;i< DropCount;i++)  
	{  
		WCHAR wcStr[MAX_PATH];  
		DragQueryFile(hDropInfo,i,wcStr,MAX_PATH);//�����ҷ�ĵ�i���ļ����ļ���  

		int itemCount = m_listsrc.GetItemCount();
		m_listsrc.InsertItem(itemCount,wcStr,NULL);
		m_listsrc.SetItem(itemCount,1,LVIF_TEXT,_T("δ�ϴ�"),0,0,0,0);
		
		
		//m_listsrc.addItem.AddString(wcStr);  
	}   
	DragFinish(hDropInfo);  //�ϷŽ�����,�ͷ��ڴ�  

	m_listsrc.EnsureVisible(m_listsrc.GetItemCount()-1,FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}





BOOL CAliOssUploadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(WM_DROPFILES == pMsg->message){
		//int a = 1;
		SendMessage(WM_DROPFILES,pMsg->wParam,pMsg->lParam);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void init_sample_config(oss_config_t *config, int is_cname)
{
	aos_str_set(&config->endpoint, OSS_ENDPOINT);
	aos_str_set(&config->access_key_id, ACCESS_KEY_ID);
	aos_str_set(&config->access_key_secret, ACCESS_KEY_SECRET);
	config->is_cname = is_cname;
}
void init_sample_request_options(oss_request_options_t *options, int is_cname)
{
	options->config = oss_config_create(options->pool);
	init_sample_config(options->config, is_cname);
	options->ctl = aos_http_controller_create(options->pool, 0);
}

BOOL uploadFile(CString* localFilename,CString* objName,CString* str, BOOL isZip){
	BOOL bRet = FALSE;
	aos_pool_t *p = NULL;
	aos_string_t bucket;
	aos_string_t object;
	int is_cname = 0;
	aos_table_t *headers = NULL;
	aos_table_t *resp_headers = NULL;
	oss_request_options_t *options = NULL;
	
	
	char filename[MAX_PATH];
	convertWCharToChar(localFilename->GetBuffer(localFilename->GetLength()),filename);
	char objNameBuff[MAX_PATH];
	convertWCharToChar(objName->GetBuffer(objName->GetLength()),objNameBuff);

	aos_status_t *s = NULL;
	aos_string_t file;
	/**/
	aos_pool_create(&p, NULL);
	options = oss_request_options_create(p);
	init_sample_request_options(options, is_cname);
	headers = aos_table_make(options->pool, 1);
	apr_table_set(headers, OSS_CONTENT_TYPE, "application/javascript");
	if(isZip){
		apr_table_set(headers, "Content-Encoding", "gzip");
	}
	
	aos_str_set(&bucket, BUCKET_NAME);
	aos_str_set(&object, objNameBuff);
	aos_str_set(&file, filename);

	s = oss_put_object_from_file(options, &bucket, &object, &file, 
		headers, &resp_headers);
	
	if (aos_status_is_ok(s)) {
		str->Format(_T("�ļ��ϴ��ɹ�,%S"),filename);
		bRet = TRUE;
	} else {
		str->Format(_T("���ļ��ϴ�ʧ��, code:%d, error_code:%S, error_msg:%S, request_id:%S\n"),
			s->code, s->error_code, s->error_msg, s->req_id);
	}

	aos_pool_destroy(p);
	return bRet;
}

BOOL CreateIfNotExist(CString strPath){
	WIN32_FIND_DATA wfd;  
	BOOL bValue=FALSE;  
	HANDLE hFind=::FindFirstFile(strPath,&wfd);  
	if( (hFind!=INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )//�ļ��ҵ�  
	{  
		bValue = TRUE;  
	}  
	::FindClose(hFind);

	if(bValue){
		return bValue;
	}

	SECURITY_ATTRIBUTES attrib;  
	attrib.bInheritHandle = FALSE;  
	attrib.lpSecurityDescriptor = NULL;  
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);  
	return ::CreateDirectory(strPath, &attrib);  
}

void CAliOssUploadDlg::copyToFile(CString src,CString desc){
	CFile file(src,CFile::modeRead|CFile::typeBinary);
	CFile descFile(desc,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone|CFile::typeBinary);

	char buffer[0x10000];//64KB buff
	
	DWORD readLen = 0;
	while(readLen = file.Read(buffer,0x10000),readLen>0){
		descFile.Write(buffer,readLen);
	}

	file.Close();
	descFile.Close();


}
void CAliOssUploadDlg::OnBnClickedButtonUpload()
{
	UpdateData(TRUE);
	int itemCount = m_listsrc.GetItemCount();
	for(int i = itemCount;i>0;i--){
		int index = i -1;
		CString localFilename = m_listsrc.GetItemText(index,0);
		

		CFile file(localFilename,CFile::modeRead);
		
		CString fileName = file.GetFileName();
		file.Close();
		
		CString info;
		char buf11[MAX_PATH],buf22[MAX_PATH];
		convertWCharToChar(localFilename,buf11);
		convertWCharToChar(fileName,buf22);
		CString curDir;
		DWORD newLength = GetModuleFileName(NULL,curDir.GetBuffer(MAX_PATH),MAX_PATH);
		curDir.ReleaseBuffer(newLength);
		curDir = curDir.Left(curDir.ReverseFind('\\')+1);
		CString tmpFile = curDir + "TMP\\";
		CreateIfNotExist(tmpFile);

		tmpFile = tmpFile+"\\"+fileName;
		if(m_bIsGzipCompress){
			convertToFile(localFilename,tmpFile);
		}else{
			copyToFile(localFilename,tmpFile);
		}
		CString uploadFileName = fileName;
		if(m_szOssPath.GetLength()>0){
			uploadFileName = m_szOssPath+"/"+fileName;
		}
		BOOL bUpload = uploadFile(&tmpFile,&uploadFileName,&info, m_bIsGzipCompress);

		
		m_listLog.AddString(info);
		if(bUpload){
			m_listsrc.SetItemText(index,1,TEXT("�ϴ��ɹ�"));
			m_listsrc.DeleteItem(index);
		}else{
			m_listsrc.SetItemText(index,1,info);
		}
		
		
	}
	
	
}


void CAliOssUploadDlg::OnBnClickedButtonOssInfo()
{

}
