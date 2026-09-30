/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/io_windows.c
 * PURPOSE:
 *   Windows data-only image operations. Linux kernel builds remain Linux-host work; these
 *   adapters inspect regular files and create new bundles only.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Windows data-only image operations. Linux kernel builds remain Linux-host
 * work; these adapters inspect regular files and create new bundles only. */
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <windows.h>
#include <wchar.h>
#include "internal.h"
static UmiStatus Error(void) {
    DWORD e=GetLastError();
    return e==ERROR_FILE_NOT_FOUND||e==ERROR_PATH_NOT_FOUND?UMI_STATUS_NOT_FOUND: e==ERROR_ALREADY_EXISTS||e==ERROR_FILE_EXISTS?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
}
static wchar_t *Wide(const char *path) {
    if(OiAbsolute(path,0)!=UMI_STATUS_OK)return NULL;
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,NULL,0);
    if(n<1)return NULL;
    wchar_t *w=malloc((size_t)n*sizeof *w);
    if(!w)return NULL;
    if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,w,n)) {
        free(w);
        return NULL;
    }
    for(int i=0;i<n;++i)if(w[i]==L'/')w[i]=L'\\';
    return w;
}
static UmiStatus Parents(wchar_t *path) {
    for(size_t i=3;path[i];++i)if(path[i]==L'\\') {
        path[i]=0;
        DWORD a=GetFileAttributesW(path);
        path[i]=L'\\';
        if(a==INVALID_FILE_ATTRIBUTES)return Error();
        if(!(a&FILE_ATTRIBUTE_DIRECTORY)||(a&FILE_ATTRIBUTE_REPARSE_POINT))return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus OiRead(const char *path,size_t limit,unsigned char **data,size_t *size) {
    if(!data||!size||limit>UMI_OS_IMAGE_MAX_ARCHIVE+16384U)return UMI_STATUS_INVALID_ARGUMENT;
    *data=NULL;
    *size=0;
    wchar_t *w=Wide(path);
    if(!w)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus s=Parents(w);
    HANDLE h=INVALID_HANDLE_VALUE;
    if(s==UMI_STATUS_OK) {
        h=CreateFileW(w,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
        if(h==INVALID_HANDLE_VALUE)s=Error();
    }
    free(w);
    BY_HANDLE_FILE_INFORMATION before= {
        0
    }
    ,after= {
        0
    }
    ;
    uint64_t length=0;
    if(s==UMI_STATUS_OK&&(!GetFileInformationByHandle(h,&before)||GetFileType(h)!=FILE_TYPE_DISK|| (before.dwFileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))))s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK) {
        length=((uint64_t)before.nFileSizeHigh<<32)|before.nFileSizeLow;
        if(length>limit)s=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    unsigned char *p=NULL;
    size_t used=0;
    if(s==UMI_STATUS_OK) {
        p=malloc((size_t)length+1U);
        if(!p)s=UMI_STATUS_OUT_OF_MEMORY;
    }
    while(s==UMI_STATUS_OK&&used<length) {
        DWORD request=(DWORD)((length-used)>65536U?65536U:length-used),got=0;
        if(!ReadFile(h,p+used,request,&got,NULL)||!got)s=UMI_STATUS_IO_ERROR;
        else used+=got;
    }
    if(s==UMI_STATUS_OK&&(!GetFileInformationByHandle(h,&after)||before.nFileSizeHigh!=after.nFileSizeHigh||before.nFileSizeLow!=after.nFileSizeLow|| CompareFileTime(&before.ftLastWriteTime,&after.ftLastWriteTime)))s=UMI_STATUS_INVALID_STATE;
    if(h!=INVALID_HANDLE_VALUE&&!CloseHandle(h)&&s==UMI_STATUS_OK)s=UMI_STATUS_IO_ERROR;
    if(s==UMI_STATUS_OK) {
        p[used]=0;
        *data=p;
        *size=used;
    }
    else free(p);
    return s;
}
UmiStatus OiWrite(const char *path,const void *data,size_t size) {
    if(size&&!data)return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t *w=Wide(path);
    if(!w)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus s=Parents(w);
    HANDLE h=INVALID_HANDLE_VALUE;
    if(s==UMI_STATUS_OK) {
        h=CreateFileW(w,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
        if(h==INVALID_HANDLE_VALUE)s=Error();
    }
    free(w);
    size_t used=0;
    const unsigned char *p=data;
    while(s==UMI_STATUS_OK&&used<size) {
        DWORD request=(DWORD)(size-used>65536U?65536U:size-used),written=0;
        if(!WriteFile(h,p+used,request,&written,NULL)||!written)s=UMI_STATUS_IO_ERROR;
        else used+=written;
    }
    if(h!=INVALID_HANDLE_VALUE) {
        if(!FlushFileBuffers(h))s=UMI_STATUS_IO_ERROR;
        if(!CloseHandle(h))s=UMI_STATUS_IO_ERROR;
    }
    return s;
}
UmiStatus OiDirectory(const char *path,int create) {
    wchar_t *w=Wide(path);
    if(!w)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus s=Parents(w);
    if(s==UMI_STATUS_OK&&create&&!CreateDirectoryW(w,NULL))s=Error();
    if(s==UMI_STATUS_OK) {
        DWORD a=GetFileAttributesW(w);
        if(a==INVALID_FILE_ATTRIBUTES)s=Error();
        else if(!(a&FILE_ATTRIBUTE_DIRECTORY)||(a&FILE_ATTRIBUTE_REPARSE_POINT))s=UMI_STATUS_INVALID_ARGUMENT;
    }
    free(w);
    return s;
}
UmiStatus OiParents(const char *root,const char *relative) {
    if(UmiOsImageValidateName(relative)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_OS_IMAGE_PATH];
    UmiStatus s=OiJoin(root,relative,path);
    if(s!=UMI_STATUS_OK)return s;
    for(size_t i=strlen(root)+1U;path[i];++i)if(path[i]=='/') {
        path[i]=0;
        s=OiDirectory(path,1);
        if(s==UMI_STATUS_ALREADY_EXISTS)s=OiDirectory(path,0);
        path[i]='/';
        if(s!=UMI_STATUS_OK)return s;
    }
    return s;
}
UmiStatus OiInventory(const char *root,const char *const *names,size_t count) {
    UmiStatus s=OiDirectory(root,0);
    if(s!=UMI_STATUS_OK)return s;
    char pattern[UMI_OS_IMAGE_PATH];
    s=OiJoin(root,"*",pattern);
    if(s!=UMI_STATUS_OK)return s;
    wchar_t *w=Wide(pattern);
    if(!w)return UMI_STATUS_INVALID_ARGUMENT;
    WIN32_FIND_DATAW f;
    HANDLE h=FindFirstFileW(w,&f);
    free(w);
    if(h==INVALID_HANDLE_VALUE)return Error();
    size_t found=0;
    do {
        if(!wcscmp(f.cFileName,L".")||!wcscmp(f.cFileName,L".."))continue;
        char text[UMI_OS_IMAGE_NAME];
        if(!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,f.cFileName,-1,text,sizeof text,NULL,NULL)) {
            s=UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        int known=0;
        for(size_t i=0;i<count;++i)if(!strcmp(text,names[i]))known=1;
        if(!known||(f.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))) {
            s=UMI_STATUS_INVALID_STATE;
            break;
        }
        ++found;
    }
    while(FindNextFileW(h,&f));
    if(s==UMI_STATUS_OK&&GetLastError()!=ERROR_NO_MORE_FILES)s=UMI_STATUS_IO_ERROR;
    if(!FindClose(h))s=UMI_STATUS_IO_ERROR;
    return s==UMI_STATUS_OK&&found!=count?UMI_STATUS_INVALID_STATE:s;
}
int OiNormalLinuxUser(void) {
    return 0;
}
UmiStatus OiNativeProgram(const char *path) {
    unsigned char *bytes=NULL;
    size_t n=0;
    UmiStatus s=OiRead(path,UMI_OS_IMAGE_MAX_ARCHIVE,&bytes,&n);
    if(s==UMI_STATUS_OK) {
        if(n<64U||memcmp(bytes,"MZ",2))s=UMI_STATUS_INVALID_ARGUMENT;
        else {
            uint32_t offset=OiLe32(bytes+60);
            if(offset>n||n-offset<24U||memcmp(bytes+offset,"PE\0\0",4))s=UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    free(bytes);
    return s;
}
UmiStatus OiLock(const char *root,void **outLock) {
    if(!outLock)return UMI_STATUS_INVALID_ARGUMENT;
    *outLock=NULL;
    char path[UMI_OS_IMAGE_PATH];
    UmiStatus s=OiJoin(root,".native-image.lock",path);
    if(s!=UMI_STATUS_OK)return s;
    wchar_t *w=Wide(path);
    if(!w)return UMI_STATUS_INVALID_ARGUMENT;
    s=Parents(w);
    HANDLE h=INVALID_HANDLE_VALUE;
    if(s==UMI_STATUS_OK) {
        h=CreateFileW(w,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_FLAG_OPEN_REPARSE_POINT,NULL);
        if(h==INVALID_HANDLE_VALUE)s=GetLastError()==ERROR_SHARING_VIOLATION?UMI_STATUS_BUSY:Error();
    }
    free(w);
    if(s==UMI_STATUS_OK) {
        BY_HANDLE_FILE_INFORMATION f;
        if(!GetFileInformationByHandle(h,&f)||f.nFileSizeHigh||f.nFileSizeLow||f.nNumberOfLinks!=1U|| (f.dwFileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))) {
            (void)CloseHandle(h);
            return UMI_STATUS_INVALID_STATE;
        }
        *outLock=h;
    }
    return s;
}
void OiUnlock(void *lock) {
    if(lock)(void)CloseHandle((HANDLE)lock);
}
