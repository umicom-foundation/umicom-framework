/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer_native/publish.c
 *
 * PURPOSE:
 *   Publish reviewed generated sources without replacing an existing project.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/designer/native_project.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static int Separator(char c)
{
#ifdef _WIN32
    return c == '/' || c == '\\';
#else
    return c == '/';
#endif
}

static UmiStatus CheckDirectory(const char *path)
{
    size_t length=0U, start;
    if(path==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while(length<UMI_DESIGNER_NATIVE_PATH_LIMIT && path[length]!='\0') ++length;
    if(length==0U) return UMI_STATUS_INVALID_ARGUMENT;
    if(length>=UMI_DESIGNER_NATIVE_PATH_LIMIT-32U) return UMI_STATUS_CAPACITY_EXCEEDED;
#ifdef _WIN32
    if(length<4U || !((path[0]>='A' && path[0]<='Z') || (path[0]>='a' && path[0]<='z')) ||
        path[1]!=':' || !Separator(path[2])) return UMI_STATUS_INVALID_ARGUMENT;
    start=3U;
#else
    if(path[0]!='/' || length<2U) return UMI_STATUS_INVALID_ARGUMENT;
    start=1U;
#endif
    if(Separator(path[length-1U])) return UMI_STATUS_INVALID_ARGUMENT;
    for(size_t i=start;i<=length;++i) {
        unsigned char c=(unsigned char)path[i];
        if(i<length && c<32U) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
        if(i<length && (c==':' || c=='*' || c=='?' || c=='"' || c=='<' || c=='>' || c=='|'))
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
        if(i==length || Separator(path[i])) {
            size_t n=i-start;
            if(n==0U || (n==1U && path[start]=='.') ||
                (n==2U && path[start]=='.' && path[start+1U]=='.')) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
            if(path[i-1U]=='.' || path[i-1U]==' ') return UMI_STATUS_INVALID_ARGUMENT;
            char base[5]={0}; size_t b=0U;
            while(b<n && b<4U && path[start+b]!='.') {
                char x=path[start+b]; base[b]=(x>='a' && x<='z') ? (char)(x-'a'+'A') : x; ++b;
            }
            if((b==3U && (n==3U || path[start+3U]=='.') &&
                (strcmp(base,"CON")==0 || strcmp(base,"PRN")==0 || strcmp(base,"AUX")==0 || strcmp(base,"NUL")==0)) ||
               (b==4U && (n==4U || path[start+4U]=='.') && base[3]>='1' && base[3]<='9' &&
                (memcmp(base,"COM",3U)==0 || memcmp(base,"LPT",3U)==0))) return UMI_STATUS_INVALID_ARGUMENT;
#endif
            start=i+1U;
        }
    }
    return UMI_STATUS_OK;
}

#ifdef _WIN32
static UmiStatus WinError(DWORD error)
{
    if(error==ERROR_ALREADY_EXISTS || error==ERROR_FILE_EXISTS) return UMI_STATUS_ALREADY_EXISTS;
    if(error==ERROR_ACCESS_DENIED || error==ERROR_SHARING_VIOLATION) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
static UmiStatus ToWide(const char *text, wchar_t *out)
{
    return MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,out,
        (int)UMI_DESIGNER_NATIVE_PATH_LIMIT)>0 ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus WriteFileNew(const char *directory, const UmiDesignerNativeFileView *file)
{
    char path[UMI_DESIGNER_NATIVE_PATH_LIMIT];
    wchar_t wide[UMI_DESIGNER_NATIVE_PATH_LIMIT];
    int n=snprintf(path,sizeof path,"%s\\%s",directory,file->path);
    if(n<0 || (size_t)n>=sizeof path) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status=ToWide(path,wide);
    if(status!=UMI_STATUS_OK) return status;
    HANDLE handle=CreateFileW(wide,GENERIC_WRITE,0,NULL,CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
    if(handle==INVALID_HANDLE_VALUE) return WinError(GetLastError());
    size_t offset=0U;
    while(offset<file->length) {
        DWORD written=0;
        size_t chunk=file->length-offset;
        if(chunk>65536U) chunk=65536U;
        if(!WriteFile(handle,file->text+offset,(DWORD)chunk,&written,NULL) || written==0U) { status=UMI_STATUS_IO_ERROR; break; }
        offset+=(size_t)written;
    }
    if(status==UMI_STATUS_OK && !FlushFileBuffers(handle)) status=UMI_STATUS_IO_ERROR;
    if(!CloseHandle(handle) && status==UMI_STATUS_OK) status=UMI_STATUS_IO_ERROR;
    return status;
}
#else
static UmiStatus PosixError(int error)
{
    if(error==EEXIST) return UMI_STATUS_ALREADY_EXISTS;
    if(error==EACCES || error==EPERM) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
static UmiStatus WriteFileNew(int directory, const UmiDesignerNativeFileView *file)
{
    int fd=openat(directory,file->path,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
    if(fd<0) return PosixError(errno);
    UmiStatus status=UMI_STATUS_OK;
    size_t offset=0U;
    while(offset<file->length) {
        ssize_t n=write(fd,file->text+offset,file->length-offset);
        if(n<0 && errno==EINTR) continue;
        if(n<=0) { status=UMI_STATUS_IO_ERROR; break; }
        offset+=(size_t)n;
    }
    if(status==UMI_STATUS_OK && fsync(fd)!=0) status=UMI_STATUS_IO_ERROR;
    if(close(fd)!=0 && status==UMI_STATUS_OK) status=UMI_STATUS_IO_ERROR;
    return status;
}
#endif

UmiStatus UmiDesignerNativeProjectPublish(const UmiDesignerNativeProject *project,
    const char *newDirectory,UmiDesignerNativePublishResult *outResult)
{
    UmiStatus status;
    UmiDesignerNativeProjectSummary summary;
    if(outResult==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outResult=(UmiDesignerNativePublishResult){0};
    if(project==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status=CheckDirectory(newDirectory);
    if(status!=UMI_STATUS_OK) return status;
    status=UmiDesignerNativeProjectGetSummary(project,&summary);
    if(status!=UMI_STATUS_OK) return status;
#ifdef _WIN32
    wchar_t wide[UMI_DESIGNER_NATIVE_PATH_LIMIT];
    status=ToWide(newDirectory,wide);
    if(status!=UMI_STATUS_OK) return status;
    if(!CreateDirectoryW(wide,NULL)) return WinError(GetLastError());
    outResult->directoryCreated=1;
    /* Omitting FILE_SHARE_DELETE prevents the held directory being renamed or
     * replaced during normal cooperating publication. This is not a sandbox
     * against an actor who can modify arbitrary ancestors or security policy. */
    HANDLE directory=CreateFileW(wide,FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
    if(directory==INVALID_HANDLE_VALUE) return WinError(GetLastError());
    BY_HANDLE_FILE_INFORMATION info;
    if(!GetFileInformationByHandle(directory,&info) ||
        !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        (void)CloseHandle(directory); return UMI_STATUS_INVALID_STATE;
    }
#else
    if(mkdir(newDirectory,0700)!=0) return PosixError(errno);
    outResult->directoryCreated=1;
    int directory=open(newDirectory,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    if(directory<0) return PosixError(errno);
#endif
    for(size_t i=0U;i<summary.fileCount;++i) {
        UmiDesignerNativeFileView file;
        status=UmiDesignerNativeProjectFile(project,i,&file);
        if(status!=UMI_STATUS_OK) break;
#ifdef _WIN32
        status=WriteFileNew(newDirectory,&file);
#else
        status=WriteFileNew(directory,&file);
#endif
        if(status!=UMI_STATUS_OK) break;
        ++outResult->filesWritten;
        outResult->bytesWritten+=file.length;
    }
#ifdef _WIN32
    if(!CloseHandle(directory) && status==UMI_STATUS_OK) status=UMI_STATUS_IO_ERROR;
#else
    if(status==UMI_STATUS_OK && fsync(directory)!=0) status=UMI_STATUS_IO_ERROR;
    if(close(directory)!=0 && status==UMI_STATUS_OK) status=UMI_STATUS_IO_ERROR;
#endif
    outResult->complete=status==UMI_STATUS_OK;
    return status;
}
