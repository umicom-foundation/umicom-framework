/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/lease.c
 * PURPOSE:
 *   All managed disk users hold the SAME stable lock file. It is never deleted: unlinking a
 *   lock would permit two processes to lock different inodes.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * All managed disk users hold the SAME stable lock file. It is never deleted:
 * unlinking a lock would permit two processes to lock different inodes. */
#ifndef _WIN32
#define _GNU_SOURCE
#endif
#include "internal.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
uint64_t VmClock(void){
    return (uint64_t)GetTickCount64();
}
UmiStatus VmDiskLease(const char*root,void**out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiStatus s=UmiSetupFileCheck(root,1,NULL);
    char p[UMI_SETUP_PATH_CAPACITY];
    if(s==UMI_STATUS_OK)s=UmiSetupPathJoin(root,".umicom-vm-lock",p);
    if(s!=UMI_STATUS_OK)return s;
    wchar_t w[UMI_SETUP_PATH_CAPACITY];
    if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,p,-1,w,UMI_SETUP_PATH_CAPACITY))return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE h=CreateFileW(w,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
    if(h==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_SHARING_VIOLATION?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
    BY_HANDLE_FILE_INFORMATION info;
    if(!GetFileInformationByHandle(h,&info)||GetFileType(h)!=FILE_TYPE_DISK||(info.dwFileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))||info.nNumberOfLinks!=1U){
        CloseHandle(h);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    *out=h;
    return UMI_STATUS_OK;
}
void VmDiskUnlease(void*p){
    if(p)CloseHandle(p);
}
#elif defined(__linux__)
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
uint64_t VmClock(void){
    struct timespec t;
    if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;
    return (uint64_t)t.tv_sec*1000U+(uint64_t)t.tv_nsec/1000000U;
}
UmiStatus VmDiskLease(const char*root,void**out){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    UmiStatus s=UmiSetupFileCheck(root,1,NULL);
    if(s!=UMI_STATUS_OK)return s;
    int parent=open(root,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
    if(parent<0)return UMI_STATUS_IO_ERROR;
    struct stat st;
    if(fstat(parent,&st)||st.st_uid!=geteuid()||(st.st_mode&0022)){
        close(parent);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    int fd=openat(parent,".umicom-vm-lock",O_CREAT|O_RDWR|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK,0600);
    close(parent);
    if(fd<0)return UMI_STATUS_IO_ERROR;
    if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_uid!=geteuid()||st.st_nlink!=1){
        close(fd);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if(flock(fd,LOCK_EX|LOCK_NB)){
        close(fd);
        return UMI_STATUS_BUSY;
    }
    int*p=malloc(sizeof *p);
    if(!p){
        close(fd);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *p=fd;
    *out=p;
    return UMI_STATUS_OK;
}
void VmDiskUnlease(void*p){
    if(p){
        close(*(int*)p);
        free(p);
    }
}
#else
uint64_t VmClock(void){
    return 0;
}
UmiStatus VmDiskLease(const char*r,void**o){
    (void)r;
    if(o)*o=NULL;
    return UMI_STATUS_UNAVAILABLE;
}
void VmDiskUnlease(void*p){
    (void)p;
}
#endif
