/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Seekable, pinned native handles. No shell, interpreter or device inference.
 * POSIX parent walks reject links; Windows also rejects reparse components,
 * but callers must keep parent directories private against concurrent rename. */

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _FILE_OFFSET_BITS 64
#endif
#include "internal.h"
#include "umicom/setup_centre/files.h"
#ifndef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#endif
UmiStatus BmPath(const char *path)
{
    if(UmiSetupValidateAbsolute(path)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    /* A leading slash alone is relative to the current Windows drive, not a
     * self-contained host path. UNC and arbitrary device namespaces are outside
     * the regular-file workflow; devices use their separate strict adapter. */
    if(strlen(path)<3U||path[1]!=':'||(path[2]!='/'&&path[2]!='\\'))return UMI_STATUS_INVALID_ARGUMENT;
#else
    if(path[0]!='/')return UMI_STATUS_INVALID_ARGUMENT;
#endif
    return UMI_STATUS_OK;
}

#ifdef _WIN32
static UmiStatus Error(void)
{
    DWORD e=GetLastError();
    if (e==ERROR_FILE_EXISTS||e==ERROR_ALREADY_EXISTS)return UMI_STATUS_ALREADY_EXISTS;
    if (e==ERROR_FILE_NOT_FOUND||e==ERROR_PATH_NOT_FOUND)return UMI_STATUS_NOT_FOUND;
    if (e==ERROR_ACCESS_DENIED||e==ERROR_SHARING_VIOLATION)return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}

static UmiStatus MediaOpenRegularFile(const char *path,int create,BmFile **out,uint64_t *size)
{
    *out=NULL;
    if (BmPath(path)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t wide[UMI_BOOT_MEDIA_PATH];
    if (!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,UMI_BOOT_MEDIA_PATH))return UMI_STATUS_INVALID_ARGUMENT;

    /* Check every existing parent with the canonical bootstrap path rules. */
    char parent[UMI_BOOT_MEDIA_PATH];
    strcpy(parent,path);
    char *a=strrchr(parent,'/'),*b=strrchr(parent,'\\');
    if (b&&(!a||b>a))a=b;
    if (!a)return UMI_STATUS_INVALID_ARGUMENT;
    if (a==parent+2&&parent[1]==':')a[1]=0;
    else *a=0;
    UmiStatus s=UmiSetupFileCheck(parent,1,NULL);
    if (s!=UMI_STATUS_OK)return s;
    HANDLE h=CreateFileW(wide,create?(GENERIC_READ|GENERIC_WRITE):GENERIC_READ,
        create?0U:FILE_SHARE_READ,NULL,create?CREATE_NEW:OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT,NULL);
    if (h==INVALID_HANDLE_VALUE)return Error();
    BY_HANDLE_FILE_INFORMATION info;
    LARGE_INTEGER n;
    if (GetFileType(h)!=FILE_TYPE_DISK||!GetFileInformationByHandle(h,&info)||!GetFileSizeEx(h,&n)||n.QuadPart<0||
        (info.dwFileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))||info.nNumberOfLinks!=1U) {
        CloseHandle(h);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    BmFile *f=calloc(1,sizeof *f);
    if (!f) {
        CloseHandle(h);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    f->handle=h;
    f->bytes=(uint64_t)n.QuadPart;
    f->writable=create;
    *out=f;
    if (size)*size=f->bytes;
    return UMI_STATUS_OK;
}

#else
static UmiStatus Error(void)
{
    if (errno==EEXIST)return UMI_STATUS_ALREADY_EXISTS;
    if (errno==ENOENT)return UMI_STATUS_NOT_FOUND;
    if (errno==EACCES||errno==EPERM||errno==ELOOP||errno==EBUSY||errno==EWOULDBLOCK)return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}

static UmiStatus MediaOpenRegularFile(const char *path,int create,BmFile **out,uint64_t *size)
{
    *out=NULL;
    if (BmPath(path)!=UMI_STATUS_OK||path[0]!='/')return UMI_STATUS_INVALID_ARGUMENT;
    char copy[UMI_BOOT_MEDIA_PATH];
    strcpy(copy,path+1);
    int dir=open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (dir<0)return Error();
    char *part=copy,*slash;
    while ((slash=strchr(part,'/'))!=NULL) {
        *slash=0;
        int next=openat(dir,part,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
        close(dir);
        if (next<0)return Error();
        dir=next;
        part=slash+1;
    }
    int fd=openat(dir,part,(create?(O_RDWR|O_CREAT|O_EXCL):O_RDONLY)|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK,0600);
    close(dir);
    if (fd<0)return Error();
    struct stat st;
    if (fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_nlink!=1||st.st_size<0) {
        close(fd);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (flock(fd,(create?LOCK_EX:LOCK_SH)|LOCK_NB)) {
        close(fd);
        return Error();
    }
    BmFile *f=calloc(1,sizeof *f);
    if (!f) {
        close(fd);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    f->fd=fd;
    f->bytes=(uint64_t)st.st_size;
    f->writable=create;
    *out=f;
    if (size)*size=f->bytes;
    return UMI_STATUS_OK;
}

#endif
UmiStatus BmSourceOpen(const char *path,BmFile **out,uint64_t *size) {
    if (!out||!size)return UMI_STATUS_INVALID_ARGUMENT;
    return MediaOpenRegularFile(path,0,out,size);
}

UmiStatus BmCreate(const char *path,BmFile **out) {
    if (!out)return UMI_STATUS_INVALID_ARGUMENT;
    return MediaOpenRegularFile(path,1,out,NULL);
}

UmiStatus BmRead(BmFile *f,uint64_t at,void *data,size_t n)
{
    if (!f||(!data&&n)||at>f->bytes||(uint64_t)n>f->bytes-at||at>INT64_MAX)return UMI_STATUS_INVALID_ARGUMENT;
    size_t done=0;

#ifdef _WIN32
    LARGE_INTEGER p;
    p.QuadPart=(LONGLONG)at;
    if (!SetFilePointerEx(f->handle,p,NULL,FILE_BEGIN))return Error();

#endif
    while (done<n) {
#ifdef _WIN32
        DWORD take=(DWORD)((n-done)>UINT32_MAX?UINT32_MAX:n-done),got=0;
        if (!ReadFile(f->handle,(unsigned char*)data+done,take,&got,NULL))return Error();
        if (!got)return UMI_STATUS_IO_ERROR;
        done+=got;

#else
        ssize_t got=pread(f->fd,(unsigned char*)data+done,n-done,(off_t)(at+done));
        if (got<0&&errno==EINTR)continue;
        if (got<=0)return UMI_STATUS_IO_ERROR;
        done+=(size_t)got;

#endif
    }
    return UMI_STATUS_OK;
}

UmiStatus BmWrite(BmFile *f,uint64_t at,const void *data,size_t n)
{
    if (!f||!f->writable||(!data&&n)||at>INT64_MAX||(uint64_t)n>(uint64_t)INT64_MAX-at)return UMI_STATUS_INVALID_ARGUMENT;
    size_t done=0;

#ifdef _WIN32
    LARGE_INTEGER p;
    p.QuadPart=(LONGLONG)at;
    if (!SetFilePointerEx(f->handle,p,NULL,FILE_BEGIN))return Error();

#endif
    while (done<n) {
#ifdef _WIN32
        DWORD take=(DWORD)((n-done)>UINT32_MAX?UINT32_MAX:n-done),put=0;
        if (!WriteFile(f->handle,(const unsigned char*)data+done,take,&put,NULL))return Error();
        if (!put)return UMI_STATUS_IO_ERROR;
        done+=put;

#else
        ssize_t put=pwrite(f->fd,(const unsigned char*)data+done,n-done,(off_t)(at+done));
        if (put<0&&errno==EINTR)continue;
        if (put<=0)return UMI_STATUS_IO_ERROR;
        done+=(size_t)put;

#endif
    }
    if (at+n>f->bytes)f->bytes=at+n;
    return UMI_STATUS_OK;
}

UmiStatus BmFlush(BmFile *f)
{
    if (!f)return UMI_STATUS_INVALID_ARGUMENT;

#ifdef _WIN32
    return FlushFileBuffers(f->handle)?UMI_STATUS_OK:Error();

#else
    return fsync(f->fd)==0?UMI_STATUS_OK:Error();

#endif
}

void BmClose(BmFile *f)
{
    if (!f)return;

#ifdef _WIN32
    for (size_t i=0;i<f->volumeCount;++i)CloseHandle(f->volumes[i]);
    CloseHandle(f->handle);

#else
    close(f->fd);

#endif
    free(f);
}

UmiStatus BmHash(BmFile *f,uint64_t bytes,char hex[65],UmiBootMediaProgress cb,void *ctx,UmiBootMediaReport *r)
{
    unsigned char *data=malloc(BM_CHUNK);
    if (!data)return UMI_STATUS_OUT_OF_MEMORY;
    UmiNativeSha256 hash;
    UmiNativeSha256Init(&hash);
    UmiStatus s=UMI_STATUS_OK;
    for (uint64_t at=0;at<bytes&&s==UMI_STATUS_OK;) {
        size_t n=bytes-at>BM_CHUNK?BM_CHUNK:(size_t)(bytes-at);
        s=BmTick(r,r?r->phase:UMI_BOOT_MEDIA_INSPECT,at,bytes,cb,ctx);
        if (s==UMI_STATUS_OK)s=BmRead(f,at,data,n);
        if (s==UMI_STATUS_OK)s=UmiNativeSha256Update(&hash,data,n);
        at+=n;
    }
    unsigned char digest[32];
    if (s==UMI_STATUS_OK)s=UmiNativeSha256Final(&hash,digest);
    if (s==UMI_STATUS_OK)UmiNativeSha256Hex(digest,hex);
    free(data);
    return s;
}
