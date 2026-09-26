/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/io.c
 * Purpose: Bounded regular-file I/O with exclusive creation. Never opens a raw
 * disk, named pipe, symlink, reparse point or interpreter. Existing directory
 * ancestors are inspected; Windows handles deny deletion while in use.
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "internal.h"
#include <errno.h>
#include <limits.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
typedef struct ScHandle {
    HANDLE file;
    HANDLE parents[256];
    size_t count;
}
ScHandle;

static void Close(ScHandle *h) {
    if(h->file!=INVALID_HANDLE_VALUE)CloseHandle(h->file);
    for(size_t i=0; i<h->count; ++i)CloseHandle(h->parents[i]);
    h->file=INVALID_HANDLE_VALUE;
    h->count=0;
}

static UmiStatus Error(UmiSetupReport *r,const char *what)
{
    DWORD e=GetLastError();
    UmiStatus s=e==ERROR_FILE_EXISTS||e==ERROR_ALREADY_EXISTS?UMI_STATUS_ALREADY_EXISTS:e==ERROR_FILE_NOT_FOUND||e==ERROR_PATH_NOT_FOUND?UMI_STATUS_NOT_FOUND:e==ERROR_ACCESS_DENIED||e==ERROR_SHARING_VIOLATION?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_IO_ERROR;
    if(r)r->systemError=(int)e;
    ScReport(r,s,"%s (Windows error %lu).",what,(unsigned long)e);
    return s;
}

static UmiStatus Wide(const char *path,wchar_t out[UMI_SETUP_PATH_CAPACITY+8U])
{
    if(UmiSetupValidateAbsolute(path)!=UMI_STATUS_OK||path[0]=='/')return UMI_STATUS_INVALID_ARGUMENT;

    out[0]=L'\\';
    out[1]=L'\\';
    out[2]=L'?';
    out[3]=L'\\';

    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,out+4,(int)UMI_SETUP_PATH_CAPACITY);

    if(n<=0)return UMI_STATUS_INVALID_ARGUMENT;
    for(int i=4; i<4+n; ++i)if(out[i]==L'/')out[i]=L'\\';
    return UMI_STATUS_OK;
}

static UmiStatus Parents(ScHandle *h,wchar_t *path,UmiSetupReport *r)
{

    for(size_t i=7U; path[i]; ++i)if(path[i]==L'\\') {

        if(h->count==256U)return UMI_STATUS_CAPACITY_EXCEEDED;

        path[i]=0;
        HANDLE p=CreateFileW(path,FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT,NULL);
        path[i]=L'\\';

        if(p==INVALID_HANDLE_VALUE)return Error(r,"Cannot open parent directory");

        BY_HANDLE_FILE_INFORMATION info;
        if(!GetFileInformationByHandle(p,&info)) {
            CloseHandle(p);
            return Error(r,"Cannot inspect directory");
        }

        if((info.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||!(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
            CloseHandle(p);
            return UMI_STATUS_PERMISSION_DENIED;
        }
        h->parents[h->count++]=p;

    }
    return UMI_STATUS_OK;

}

static UmiStatus Open(ScHandle *h,const char *path,int create,int directory,uint64_t *size,UmiSetupReport *r)
{

    memset(h,0,sizeof *h);
    h->file=INVALID_HANDLE_VALUE;
    wchar_t wide[UMI_SETUP_PATH_CAPACITY+8U];
    UmiStatus s=Wide(path,wide);
    if(s!=UMI_STATUS_OK)return s;

    s=Parents(h,wide,r);
    if(s!=UMI_STATUS_OK) {
        Close(h);
        return s;
    }

    h->file=CreateFileW(wide,create?GENERIC_WRITE:(directory?FILE_READ_ATTRIBUTES:GENERIC_READ),FILE_SHARE_READ,NULL,create?CREATE_NEW:OPEN_EXISTING,
    FILE_FLAG_OPEN_REPARSE_POINT|(directory?FILE_FLAG_BACKUP_SEMANTICS:0U),NULL);

    if(h->file==INVALID_HANDLE_VALUE) {
        s=Error(r,"Cannot open file");
        Close(h);
        return s;
    }

    BY_HANDLE_FILE_INFORMATION info;
    if(!GetFileInformationByHandle(h->file,&info)||GetFileType(h->file)!=FILE_TYPE_DISK) {
        s=Error(r,"Cannot inspect file");
        Close(h);
        return s;
    }

    if((info.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||((info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0U)!=!!directory) {
        Close(h);
        return UMI_STATUS_PERMISSION_DENIED;
    }

    if(size)*size=((uint64_t)info.nFileSizeHigh<<32U)|info.nFileSizeLow;
    return UMI_STATUS_OK;

}

static int Read(ScHandle *h,void *data,size_t cap,size_t *count) {
    DWORD n=0;
    if(!ReadFile(h->file,data,(DWORD)cap,&n,NULL))return 0;
    *count=n;
    return 1;
}

static int Write(ScHandle *h,const void *data,size_t count) {
    const unsigned char *p=data;
    while(count) {
        DWORD n=0;
        DWORD part=count>65536U?65536U:(DWORD)count;
        if(!WriteFile(h->file,p,part,&n,NULL)||n==0U)return 0;
        p+=n;
        count-=n;
    }
    return 1;
}

static int Flush(ScHandle *h) {
    return FlushFileBuffers(h->file)!=0;
}

UmiStatus ScMakeDirectory(const char *path,UmiSetupReport *r)
{
    ScHandle h= {
        0
    };
    h.file=INVALID_HANDLE_VALUE;
    wchar_t w[UMI_SETUP_PATH_CAPACITY+8U];
    UmiStatus s=Wide(path,w);
    if(s==UMI_STATUS_OK)s=Parents(&h,w,r);
    if(s==UMI_STATUS_OK&&!CreateDirectoryW(w,NULL))s=Error(r,"Cannot create a new directory");
    Close(&h);
    return s;
}

UmiStatus ScFreeSpace(const char *path,uint64_t required,UmiSetupReport *r)
{
    wchar_t w[UMI_SETUP_PATH_CAPACITY+8U];
    ULARGE_INTEGER available;
    UmiStatus s=Wide(path,w);
    if(s!=UMI_STATUS_OK)return s;
    if(!GetDiskFreeSpaceExW(w,&available,NULL,NULL))return Error(r,"Cannot read free space");
    if(available.QuadPart<required) {
        ScReport(r,UMI_STATUS_CAPACITY_EXCEEDED,"Not enough free space for the selected payload.");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
typedef struct ScHandle {
    int file;
    int parent;
}
ScHandle;

static void Close(ScHandle *h) {
    if(h->file>=0)(void)close(h->file);
    if(h->parent>=0)(void)close(h->parent);
    h->file=h->parent=-1;
}

static UmiStatus Error(UmiSetupReport *r,const char *what)
{
    int e=errno;
    UmiStatus s=e==EEXIST?UMI_STATUS_ALREADY_EXISTS:e==ENOENT?UMI_STATUS_NOT_FOUND:e==EACCES||e==EPERM||e==ELOOP||e==ENOTDIR?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_IO_ERROR;
    if(r)r->systemError=e;
    ScReport(r,s,"%s: %s.",what,strerror(e));
    return s;
}

static UmiStatus Parent(const char *path,int *out,char leaf[256],UmiSetupReport *r)
{

    if(UmiSetupValidateAbsolute(path)!=UMI_STATUS_OK||path[0]!='/'||!path[1])return UMI_STATUS_INVALID_ARGUMENT;

    int dir=open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if(dir<0)return Error(r,"Cannot open root");

    const char *p=path+1;

    while(*p) {
        const char *slash=strchr(p,'/');
        size_t n=slash?(size_t)(slash-p):strlen(p);
        if(n>255U) {
            close(dir);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        memcpy(leaf,p,n);
        leaf[n]=0;

        if(!slash) {
            *out=dir;
            return UMI_STATUS_OK;
        }
        int next=openat(dir,leaf,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
        if(next<0) {
            UmiStatus s=Error(r,"Cannot open directory without following links");
            close(dir);
            return s;
        }
        close(dir);
        dir=next;
        p=slash+1;
    }

    close(dir);
    return UMI_STATUS_INVALID_ARGUMENT;

}

static UmiStatus Open(ScHandle *h,const char *path,int create,int directory,uint64_t *size,UmiSetupReport *r)
{

    h->file=h->parent=-1;
    char leaf[256];
    UmiStatus s=Parent(path,&h->parent,leaf,r);
    if(s!=UMI_STATUS_OK)return s;

    int flags=O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK|(create?O_WRONLY|O_CREAT|O_EXCL:O_RDONLY)|(directory?O_DIRECTORY:0);

    h->file=openat(h->parent,leaf,flags,0600);
    if(h->file<0) {
        s=Error(r,"Cannot open file");
        Close(h);
        return s;
    }

    struct stat info;
    if(fstat(h->file,&info)!=0) {
        s=Error(r,"Cannot inspect file");
        Close(h);
        return s;
    }

    if((directory?!S_ISDIR(info.st_mode):!S_ISREG(info.st_mode))||info.st_size<0) {
        Close(h);
        return UMI_STATUS_PERMISSION_DENIED;
    }

    if(size)*size=(uint64_t)info.st_size;
    return UMI_STATUS_OK;

}

static int Read(ScHandle *h,void *data,size_t cap,size_t *count) {
    ssize_t n;
    do {
        n=read(h->file,data,cap);
    }
    while(n<0&&errno==EINTR);
    if(n<0)return 0;
    *count=(size_t)n;
    return 1;
}

static int Write(ScHandle *h,const void *data,size_t count) {
    const unsigned char *p=data;
    while(count) {
        ssize_t n=write(h->file,p,count);
        if(n<0&&errno==EINTR)continue;
        if(n<=0)return 0;
        p+=(size_t)n;
        count-=(size_t)n;
    }
    return 1;
}

static int Flush(ScHandle *h) {
    return fsync(h->file)==0;
}

UmiStatus ScMakeDirectory(const char *path,UmiSetupReport *r)
{
    int parent=-1;
    char leaf[256];
    UmiStatus s=Parent(path,&parent,leaf,r);
    if(s!=UMI_STATUS_OK)return s;
    if(mkdirat(parent,leaf,0700)!=0)s=Error(r,"Cannot create a new directory");
    close(parent);
    return s;
}

UmiStatus ScFreeSpace(const char *path,uint64_t required,UmiSetupReport *r)
{
    struct statvfs info;
    if(statvfs(path,&info)!=0)return Error(r,"Cannot read free space");
    uint64_t n=(uint64_t)info.f_bavail,unit=(uint64_t)info.f_frsize;
    if(unit&&n<=UINT64_MAX/unit&&n*unit<required) {
        ScReport(r,UMI_STATUS_CAPACITY_EXCEEDED,"Not enough free space for selected files.");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

#endif
UmiStatus ScFileRegular(const char *path,int directory,UmiSetupReport *r)
{
    ScHandle h;
    UmiStatus s=Open(&h,path,0,directory,NULL,r);
    if(s==UMI_STATUS_OK)Close(&h);
    return s;
}

UmiStatus ScHeaderRead(const char *path,unsigned char *bytes,size_t capacity,size_t *length,UmiSetupReport *r)
{
    ScHandle h;
    uint64_t size;
    UmiStatus s=Open(&h,path,0,0,&size,r);
    if(s!=UMI_STATUS_OK)return s;
    size_t n=0;
    if(!Read(&h,bytes,capacity,&n))s=Error(r,"Cannot read header");
    Close(&h);
    *length=n;
    return s;
}

UmiStatus ScRead(const char *path,size_t limit,char **data,size_t *length,UmiSetupReport *r)
{

    *data=NULL;
    *length=0;
    ScHandle h;
    uint64_t size=0;
    UmiStatus s=Open(&h,path,0,0,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    if(size>limit) {
        Close(&h);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char *p=malloc((size_t)size+1U);
    if(!p) {
        Close(&h);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    size_t used=0;
    while(used<(size_t)size) {
        size_t n=0;
        if(!Read(&h,p+used,(size_t)size-used,&n)||!n) {
            s=UMI_STATUS_IO_ERROR;
            break;
        }
        used+=n;
    }

    unsigned char extra;
    size_t n=0;
    if(s==UMI_STATUS_OK&&(!Read(&h,&extra,1,&n)||n))s=UMI_STATUS_INVALID_STATE;

    Close(&h);
    if(s!=UMI_STATUS_OK) {
        free(p);
        return s;
    }
    p[used]=0;
    *data=p;
    *length=used;
    return UMI_STATUS_OK;

}

UmiStatus ScWriteNew(const char *path,const void *data,size_t length,UmiSetupReport *r)
{
    ScHandle h;
    UmiStatus s=Open(&h,path,1,0,NULL,r);
    if(s!=UMI_STATUS_OK)return s;
    if(!Write(&h,data,length)||!Flush(&h))s=Error(r,"Cannot complete file write");
    Close(&h);
    return s;
}

UmiStatus ScDigest(const char *path,char hash[65],uint64_t *bytes,UmiSetupProgress progress,void *context,UmiSetupReport *r)
{

    ScHandle h;
    uint64_t size=0;
    UmiStatus s=Open(&h,path,0,0,&size,r);
    if(s!=UMI_STATUS_OK)return s;
    if(size>UMI_SETUP_MAX_FILE_BYTES) {
        Close(&h);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    UmiNativeSha256 sha;
    UmiNativeSha256Init(&sha);
    unsigned char chunk[65536],digest[32];
    uint64_t used=0;

    for(; ; ) {
        size_t n=0;
        if(progress&&progress(r,context)) {
            s=UMI_STATUS_CANCELLED;
            break;
        }
        if(!Read(&h,chunk,sizeof chunk,&n)) {
            s=Error(r,"Cannot read file");
            break;
        }
        if(!n)break;

        if((uint64_t)n>size-used) {
            s=UMI_STATUS_INVALID_STATE;
            break;
        }
        used+=n;
        s=UmiNativeSha256Update(&sha,chunk,n);
        if(s!=UMI_STATUS_OK)break;
    }

    Close(&h);
    if(s==UMI_STATUS_OK&&used!=size)s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Final(&sha,digest);

    if(s==UMI_STATUS_OK) {
        UmiNativeSha256Hex(digest,hash);
        if(bytes)*bytes=size;
    }
    return s;

}

UmiStatus ScCopy(const char *source,const char *destination,const char *expected,uint64_t expectedSize,UmiSetupProgress progress,void *context,UmiSetupReport *r)
{

    ScHandle input,output;
    uint64_t size=0;
    UmiStatus s=Open(&input,source,0,0,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    if(size!=expectedSize||size>UMI_SETUP_MAX_FILE_BYTES) {
        Close(&input);
        return UMI_STATUS_INVALID_STATE;
    }

    s=Open(&output,destination,1,0,NULL,r);
    if(s!=UMI_STATUS_OK) {
        Close(&input);
        return s;
    }

    UmiNativeSha256 sha;
    UmiNativeSha256Init(&sha);
    unsigned char chunk[65536],digest[32];
    uint64_t used=0;

    for(; ; ) {
        size_t n=0;
        if(progress&&progress(r,context)) {
            s=UMI_STATUS_CANCELLED;
            break;
        }
        if(!Read(&input,chunk,sizeof chunk,&n)) {
            s=Error(r,"Cannot read source");
            break;
        }
        if(!n)break;

        if((uint64_t)n>size-used) {
            s=UMI_STATUS_INVALID_STATE;
            break;
        }
        used+=n;
        s=UmiNativeSha256Update(&sha,chunk,n);
        if(s!=UMI_STATUS_OK)break;

        if(!Write(&output,chunk,n)) {
            s=Error(r,"Cannot write destination");
            break;
        }
        if(r)r->bytesCompleted+=n;
    }

    if(s==UMI_STATUS_OK&&used!=size)s=UMI_STATUS_INVALID_STATE;

    if(s==UMI_STATUS_OK)s=UmiNativeSha256Final(&sha,digest);

    if(s==UMI_STATUS_OK) {
        char hash[65];
        UmiNativeSha256Hex(digest,hash);
        if(strcmp(hash,expected))s=UMI_STATUS_INVALID_STATE;
    }

    if(s==UMI_STATUS_OK&&!Flush(&output))s=Error(r,"Cannot flush destination");

    Close(&output);
    Close(&input);
    if(s!=UMI_STATUS_OK)ScReport(r,s,"Copy did not complete. The new output is retained for inspection.");
    return s;

}

UmiStatus ScParents(const char *root,const char *relative,UmiSetupReport *r)
{

    if(UmiSetupValidateRelative(relative)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    char partial[UMI_SETUP_RELATIVE_CAPACITY],path[UMI_SETUP_PATH_CAPACITY];
    strcpy(partial,relative);

    for(char *p=partial; *p; ++p)if(*p=='/') {
        *p=0;
        UmiStatus s=ScJoin(root,partial,path);
        if(s==UMI_STATUS_OK)s=ScMakeDirectory(path,r);

        if(s==UMI_STATUS_ALREADY_EXISTS)s=ScFileRegular(path,1,r);
        *p='/';
        if(s!=UMI_STATUS_OK)return s;
    }

    return UMI_STATUS_OK;

}

/* Read a bounded region without mapping a potentially large image into RAM. */
UmiStatus ScReadAt(const char *path,uint64_t offset,void *data,size_t length,uint64_t *fileSize,UmiSetupReport *r)
{

    ScHandle h;
    uint64_t size=0;
    UmiStatus s=Open(&h,path,0,0,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    if(offset>size||(uint64_t)length>size-offset) {
        Close(&h);
        return UMI_STATUS_PARSE_ERROR;
    }

#ifdef _WIN32
    LARGE_INTEGER location;
    location.QuadPart=(LONGLONG)offset;
    if(!SetFilePointerEx(h.file,location,NULL,FILE_BEGIN))s=Error(r,"Cannot seek image");

#else
    if(offset>(uint64_t)INT64_MAX||lseek(h.file,(off_t)offset,SEEK_SET)<0)s=Error(r,"Cannot seek image");

#endif
    size_t used=0;
    while(used<length&&s==UMI_STATUS_OK) {
        size_t n=0;
        if(!Read(&h,(unsigned char *)data+used,length-used,&n)||n==0U)s=UMI_STATUS_IO_ERROR;
        else used+=n;
    }

    Close(&h);
    if(fileSize)*fileSize=size;
    return s;

}
