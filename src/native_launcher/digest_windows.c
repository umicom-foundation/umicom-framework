/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * UTF-8 to UTF-16 native Windows file hashing. No shell, PowerShell, Python,
 * executable loading or subprocess. Sharing excludes writers during the read.
 * This verifies bytes, not directory trust, executable provenance or signatures. */
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <windows.h>
#include "internal.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
static UmiStatus WinError(UmiNativeStageReport *r,const char *detail,DWORD error)
{
    UmiStatus status=UMI_STATUS_IO_ERROR;
    if(error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND)status=UMI_STATUS_NOT_FOUND;
    if(error==ERROR_ACCESS_DENIED||error==ERROR_SHARING_VIOLATION)status=UMI_STATUS_PERMISSION_DENIED;
    /* DWORD does not fit int on every value; keep the report representable. */
    return UmiNativeReportSet(r,status,"digest",detail,error<=(DWORD)INT_MAX?(int)error:0);
}
UmiStatus UmiNativeStageDigestFile(const char *path,char outHex[65],uint64_t *outBytes,UmiNativeStageReport *report)
{
    UmiNativeStageReport local;UmiNativeStageReport *r=report==NULL?&local:report;UmiNativeReportInit(r);
    if(outHex!=NULL)outHex[0]='\0';
    if(outBytes!=NULL)*outBytes=0U;
    if(path==NULL||outHex==NULL||outBytes==NULL)return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"digest","Input path and digest outputs are required.",0);
    size_t length=strlen(path);
    if(length<3U||length>=UMI_NATIVE_STAGE_PATH_CAPACITY||!((path[0]>='A'&&path[0]<='Z')||(path[0]>='a'&&path[0]<='z'))
        ||path[1]!=':'||(path[2]!='\\'&&path[2]!='/'))return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"path","Use an absolute local drive path, not UNC, a device name or a relative path.",0);
    for(size_t i=0U;i<length;++i)if((unsigned char)path[i]<32U||(unsigned char)path[i]==127U||(i>1U&&path[i]==':'))
        return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"path","Control characters and alternate data streams are not accepted.",0);
    wchar_t wide[UMI_NATIVE_STAGE_PATH_CAPACITY];
    if(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,(int)UMI_NATIVE_STAGE_PATH_CAPACITY)==0)
        return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"path","Path is not bounded valid UTF-8.",0);
    wchar_t root[4]={wide[0],L':',L'\\',0};
    if(GetDriveTypeW(root)==DRIVE_REMOTE)return UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"path","Use a local drive for this offline digest operation.",0);
    HANDLE file=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
    if(file==INVALID_HANDLE_VALUE)return WinError(r,"Could not open the file exclusively against writers.",GetLastError());
    BY_HANDLE_FILE_INFORMATION info,after;UmiStatus status=UMI_STATUS_OK;UmiNativeSha256 digest;UmiNativeSha256Init(&digest);
    if(GetFileType(file)!=FILE_TYPE_DISK||!GetFileInformationByHandle(file,&info)) {status=WinError(r,"Could not inspect a regular file.",GetLastError());goto done;}
    if((info.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))!=0U) {status=UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"digest","Directories and reparse-point files are not accepted.",0);goto done;}
    uint64_t expected=((uint64_t)info.nFileSizeHigh<<32U)|info.nFileSizeLow,total=0U;
    if(expected>UMI_NATIVE_STAGE_BINARY_LIMIT){status=UmiNativeReportSet(r,UMI_STATUS_CAPACITY_EXCEEDED,"digest","Input exceeds the 512 MiB limit.",0);goto done;}
    unsigned char buffer[65536];
    for(;;) {
        DWORD count=0U;
        if(!ReadFile(file,buffer,(DWORD)sizeof buffer,&count,NULL)){status=WinError(r,"File read failed.",GetLastError());goto done;}
        if(count==0U)break;
        if((uint64_t)count>UMI_NATIVE_STAGE_BINARY_LIMIT-total){status=UMI_STATUS_CAPACITY_EXCEEDED;goto done;}
        status=UmiNativeSha256Update(&digest,buffer,(size_t)count);if(status!=UMI_STATUS_OK)goto done;total+=(uint64_t)count;
    }
    if(!GetFileInformationByHandle(file,&after)){status=WinError(r,"Final file inspection failed.",GetLastError());goto done;}
    if(total!=expected||after.nFileSizeHigh!=info.nFileSizeHigh||after.nFileSizeLow!=info.nFileSizeLow
        ||CompareFileTime(&after.ftLastWriteTime,&info.ftLastWriteTime)!=0){status=UmiNativeReportSet(r,UMI_STATUS_BUSY,"digest","File changed during its read.",0);goto done;}
    unsigned char raw[32];status=UmiNativeSha256Final(&digest,raw);
    if(status==UMI_STATUS_OK){UmiNativeSha256Hex(raw,outHex);*outBytes=total;r->completed=1;UmiNativeReportSet(r,status,"digest","Local regular-file digest completed; no input was executed.",0);}
done:
    if(!CloseHandle(file)&&status==UMI_STATUS_OK)status=WinError(r,"Input close failed.",GetLastError());
    if(status!=UMI_STATUS_OK){outHex[0]='\0';*outBytes=0U;r->completed=0;}
    return status;
}
