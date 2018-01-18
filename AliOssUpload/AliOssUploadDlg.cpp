
// AliOssUploadDlg.cpp : 实现文件
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





// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CAliOssUploadDlg 对话框



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


// CAliOssUploadDlg 消息处理程序

BOOL CAliOssUploadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	/*
	LVCOLUMN lc = {};
	lc.pszText = _T("路径");
	lc.fmt =  LVCFMT_LEFT  ;
	lc.cx = 500;
	lc.mask = LVCF_TEXT |LVCF_WIDTH;*/
	m_listsrc.InsertColumn(0,_T("路径"),LVCFMT_LEFT,450,0);
	m_listsrc.InsertColumn(1,_T("状态"),LVCFMT_LEFT,100,0);



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




	//初始化oss
	if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
		exit(1);
	}


	ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME); 
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAliOssUploadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAliOssUploadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAliOssUploadDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int DropCount=DragQueryFile(hDropInfo,-1,NULL,0);//取得被拖动文件的数目  
	for(int i=0;i< DropCount;i++)  
	{  
		WCHAR wcStr[MAX_PATH];  
		DragQueryFile(hDropInfo,i,wcStr,MAX_PATH);//获得拖曳的第i个文件的文件名  

		int itemCount = m_listsrc.GetItemCount();
		m_listsrc.InsertItem(itemCount,wcStr,NULL);
		m_listsrc.SetItem(itemCount,1,LVIF_TEXT,_T("未上传"),0,0,0,0);
		
		
		//m_listsrc.addItem.AddString(wcStr);  
	}   
	DragFinish(hDropInfo);  //拖放结束后,释放内存  

	m_listsrc.EnsureVisible(m_listsrc.GetItemCount()-1,FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}





BOOL CAliOssUploadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
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
		str->Format(_T("文件上传成功,%S"),filename);
		bRet = TRUE;
	} else {
		str->Format(_T("从文件上传失败, code:%d, error_code:%S, error_msg:%S, request_id:%S\n"),
			s->code, s->error_code, s->error_msg, s->req_id);
	}

	aos_pool_destroy(p);
	return bRet;
}

BOOL CreateIfNotExist(CString strPath){
	WIN32_FIND_DATA wfd;  
	BOOL bValue=FALSE;  
	HANDLE hFind=::FindFirstFile(strPath,&wfd);  
	if( (hFind!=INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )//文件找到  
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
			m_listsrc.SetItemText(index,1,TEXT("上传成功"));
			m_listsrc.DeleteItem(index);
		}else{
			m_listsrc.SetItemText(index,1,info);
		}
		
		
	}
	
	
}


void CAliOssUploadDlg::OnBnClickedButtonOssInfo()
{

}
