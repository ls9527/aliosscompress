#include "stdafx.h"
#include "StrUtil.h"

void convertWCharToChar(CString str1,char *ptxtTemp)
{
	int len =WideCharToMultiByte(CP_ACP,0,str1,-1,NULL,0,NULL,NULL);  
	WideCharToMultiByte(CP_ACP,0,str1,-1,ptxtTemp,len,NULL,NULL );  
}
