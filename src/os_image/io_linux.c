/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/io_linux.c
 * PURPOSE:
 *   Descriptor-relative Linux I/O. The builder creates archives describing devices; it never
 *   opens or creates host device nodes, mounts or removes data.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Descriptor-relative Linux I/O. The builder creates archives describing
 * devices; it never opens or creates host device nodes, mounts or removes data. */
#define _GNU_SOURCE
#include "internal.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/file.h>
static UmiStatus Error(void) {
    return errno==ENOENT?UMI_STATUS_NOT_FOUND:errno==EEXIST?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
}
static int Parent(const char *path,char leaf[UMI_OS_IMAGE_PATH]) {
    if(OiAbsolute(path,0)!=UMI_STATUS_OK) {
        errno=EINVAL;
        return -1;
    }
    char copy[UMI_OS_IMAGE_PATH];
    strcpy(copy,path);
    int fd=open("/",O_DIRECTORY|O_RDONLY|O_CLOEXEC);
    if(fd<0)return -1;
    char *start=copy+1;
    for(;;) {
        char *slash=strchr(start,'/');
        if(!slash) {
            strcpy(leaf,start);
            return fd;
        }
        *slash=0;
        int next=openat(fd,start,O_DIRECTORY|O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
        int saved=errno;
        (void)close(fd);
        errno=saved;
        if(next<0)return -1;
        fd=next;
        start=slash+1;
    }
}
UmiStatus OiRead(const char *path,size_t limit,unsigned char **data,size_t *size) {
    if(!data||!size||limit>UMI_OS_IMAGE_MAX_ARCHIVE+16384U)return UMI_STATUS_INVALID_ARGUMENT;
    *data=NULL;
    *size=0;
    char leaf[UMI_OS_IMAGE_PATH];
    int parent=Parent(path,leaf);
    if(parent<0)return Error();
    int fd=openat(parent,leaf,O_RDONLY|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK);
    int saved=errno;
    (void)close(parent);
    errno=saved;
    if(fd<0)return Error();
    UmiStatus s=UMI_STATUS_OK;
    struct stat before,after;
    if(fstat(fd,&before)||!S_ISREG(before.st_mode)||before.st_size<0)s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK&&(uint64_t)before.st_size>(uint64_t)limit)s=UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *p=NULL;
    size_t n=0;
    if(s==UMI_STATUS_OK) {
        n=(size_t)before.st_size;
        p=malloc(n+1U);
        if(!p)s=UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t used=0;
    while(s==UMI_STATUS_OK&&used<n) {
        ssize_t got=read(fd,p+used,n-used);
        if(got<0&&errno==EINTR)continue;
        if(got<=0)s=UMI_STATUS_IO_ERROR;
        else used+=(size_t)got;
    }
    if(s==UMI_STATUS_OK) {
        unsigned char extra;
        ssize_t tail;
        do {
            tail=read(fd,&extra,1);
        }
        while(tail<0&&errno==EINTR);
        if(tail!=0||fstat(fd,&after)||before.st_size!=after.st_size|| before.st_mtim.tv_sec!=after.st_mtim.tv_sec||before.st_mtim.tv_nsec!=after.st_mtim.tv_nsec|| before.st_ctim.tv_sec!=after.st_ctim.tv_sec||before.st_ctim.tv_nsec!=after.st_ctim.tv_nsec)s=UMI_STATUS_INVALID_STATE;
    }
    if(close(fd)&&s==UMI_STATUS_OK)s=UMI_STATUS_IO_ERROR;
    if(s==UMI_STATUS_OK) {
        p[n]=0;
        *data=p;
        *size=n;
    }
    else free(p);
    return s;
}
UmiStatus OiWrite(const char *path,const void *data,size_t size) {
    if(size&&!data)return UMI_STATUS_INVALID_ARGUMENT;
    char leaf[UMI_OS_IMAGE_PATH];
    int parent=Parent(path,leaf);
    if(parent<0)return Error();
    int fd=openat(parent,leaf,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
    if(fd<0) {
        UmiStatus s=Error();
        (void)close(parent);
        return s;
    }
    const unsigned char *p=data;
    size_t used=0;
    UmiStatus s=UMI_STATUS_OK;
    while(used<size) {
        ssize_t n=write(fd,p+used,size-used);
        if(n<0&&errno==EINTR)continue;
        if(n<=0) {
            s=UMI_STATUS_IO_ERROR;
            break;
        }
        used+=(size_t)n;
    }
    if(fsync(fd))s=UMI_STATUS_IO_ERROR;
    if(close(fd))s=UMI_STATUS_IO_ERROR;
    if(fsync(parent))s=UMI_STATUS_IO_ERROR;
    if(close(parent))s=UMI_STATUS_IO_ERROR;
    return s;
}
UmiStatus OiDirectory(const char *path,int create) {
    char leaf[UMI_OS_IMAGE_PATH];
    int parent=Parent(path,leaf);
    if(parent<0)return Error();
    UmiStatus s=UMI_STATUS_OK;
    if(create&&mkdirat(parent,leaf,0700))s=Error();
    if(s==UMI_STATUS_OK) {
        int fd=openat(parent,leaf,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
        if(fd<0)s=Error();
        else if(close(fd))s=UMI_STATUS_IO_ERROR;
    }
    if(create&&s==UMI_STATUS_OK&&fsync(parent))s=UMI_STATUS_IO_ERROR;
    if(close(parent)&&s==UMI_STATUS_OK)s=UMI_STATUS_IO_ERROR;
    return s;
}
UmiStatus OiParents(const char *root,const char *relative) {
    if(UmiOsImageValidateName(relative)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_OS_IMAGE_PATH];
    UmiStatus s=OiJoin(root,relative,path);
    if(s!=UMI_STATUS_OK)return s;
    size_t start=strlen(root)+1U;
    for(size_t i=start;path[i];++i)if(path[i]=='/') {
        path[i]=0;
        s=OiDirectory(path,1);
        if(s==UMI_STATUS_ALREADY_EXISTS)s=OiDirectory(path,0);
        path[i]='/';
        if(s!=UMI_STATUS_OK)return s;
    }
    return UMI_STATUS_OK;
}
UmiStatus OiInventory(const char *root,const char *const *names,size_t count) {
    char leaf[UMI_OS_IMAGE_PATH];
    int parent=Parent(root,leaf);
    if(parent<0)return Error();
    int fd=openat(parent,leaf,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
    int saved=errno;
    (void)close(parent);
    errno=saved;
    if(fd<0)return Error();
    DIR *dir=fdopendir(fd);
    if(!dir) {
        (void)close(fd);
        return UMI_STATUS_IO_ERROR;
    }
    size_t found=0;
    UmiStatus s=UMI_STATUS_OK;
    errno=0;
    struct dirent *e;
    while((e=readdir(dir))!=NULL) {
        if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
        int known=0;
        for(size_t i=0;i<count;++i)if(!strcmp(names[i],e->d_name))known=1;
        struct stat st;
        if(!known||fstatat(fd,e->d_name,&st,AT_SYMLINK_NOFOLLOW)||!S_ISREG(st.st_mode)) {
            s=UMI_STATUS_INVALID_STATE;
            break;
        }
        ++found;
        errno=0;
    }
    if(errno&&s==UMI_STATUS_OK)s=UMI_STATUS_IO_ERROR;
    if(closedir(dir))s=UMI_STATUS_IO_ERROR;
    return s==UMI_STATUS_OK&&found!=count?UMI_STATUS_INVALID_STATE:s;
}
int OiNormalLinuxUser(void) {
    return geteuid()!=0;
}
UmiStatus OiNativeProgram(const char *path) {
    unsigned char *data=NULL;
    size_t size=0;
    UmiStatus s=OiRead(path,UMI_OS_IMAGE_MAX_ARCHIVE,&data,&size);
    if(s==UMI_STATUS_OK&&(size<64U||memcmp(data,"\177ELF",4)))s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK&&access(path,X_OK))s=UMI_STATUS_IO_ERROR;
    free(data);
    return s;
}
/* One open-file-description lock also serialises separate calls in one process.
 * It is persistent data, never unlinked while another process may hold it. */
UmiStatus OiLock(const char *root,void **outLock) {
    if(!outLock)return UMI_STATUS_INVALID_ARGUMENT;
    *outLock=NULL;
    char path[UMI_OS_IMAGE_PATH],leaf[UMI_OS_IMAGE_PATH];
    UmiStatus s=OiJoin(root,".native-image.lock",path);
    if(s!=UMI_STATUS_OK)return s;
    int parent=Parent(path,leaf);
    if(parent<0)return Error();
    int fd=openat(parent,leaf,O_RDWR|O_CREAT|O_NOFOLLOW|O_CLOEXEC,0600);
    int saved=errno;
    (void)close(parent);
    errno=saved;
    if(fd<0)return Error();
    struct stat st;
    if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_nlink!=1||st.st_uid!=geteuid()||st.st_size!=0) {
        (void)close(fd);
        return UMI_STATUS_INVALID_STATE;
    }
    if(flock(fd,LOCK_EX|LOCK_NB)) {
        s=errno==EWOULDBLOCK?UMI_STATUS_BUSY:UMI_STATUS_IO_ERROR;
        (void)close(fd);
        return s;
    }
    int *owned=malloc(sizeof *owned);
    if(!owned) {
        (void)close(fd);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *owned=fd;
    *outLock=owned;
    return UMI_STATUS_OK;
}
void OiUnlock(void *lock) {
    if(lock) {
        int *fd=lock;
        (void)close(*fd);
        free(fd);
    }
}
