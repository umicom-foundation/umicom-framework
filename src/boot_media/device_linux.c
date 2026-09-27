/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linux removable-USB adapter. Kernel sysfs is the inventory authority. Whole
 * disks only; mounted partitions, swap, holders and unknown observations block
 * acquisition. There is no umount, sudo, shell, device-node creation or retry.
 *---------------------------------------------------------------------------*/

#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include "internal.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/file.h>
#include <unistd.h>
#include <linux/fs.h>
#include <inttypes.h>
static int ValidName(const char *n)
{
    if (!n||!*n||strlen(n)>64U)return 0;
    for (size_t i=0;n[i];++i)if (!((n[i]>='a'&&n[i]<='z')||(n[i]>='0'&&n[i]<='9')||n[i]=='-'||n[i]=='_'))return 0;
    return 1;
}

static int Text(const char *p,char *s,size_t cap)
{
    int fd=open(p,O_RDONLY|O_CLOEXEC);
    if (fd<0)return 0;
    ssize_t n=read(fd,s,cap-1U);
    char extra;
    ssize_t more=n>=0?read(fd,&extra,1):1;
    close(fd);
    if (n<0||more!=0)return 0;
    s[(size_t)n]=0;
    size_t k=(size_t)n;
    while (k&&(s[k-1]=='\n'||s[k-1]=='\r'||s[k-1]==' '||s[k-1]=='\t'))s[--k]=0;
    for (size_t i=0;i<k;++i)if ((unsigned char)s[i]<32U||(unsigned char)s[i]>126U)return 0;
    return 1;
}

static int Number(const char *p,uint64_t *v)
{
    char s[64];
    if (!Text(p,s,sizeof s)||!*s)return 0;
    uint64_t n=0;
    for (size_t i=0;s[i];++i) {
        if (s[i]<'0'||s[i]>'9'||n>(UINT64_MAX-(unsigned)(s[i]-'0'))/10U)return 0;
        n=n*10U+(unsigned)(s[i]-'0');
    }
    *v=n;
    return 1;
}

static int HasEntries(const char *p)
{
    DIR*d=opendir(p);
    if (!d)return -1;
    int result=0;
    struct dirent *e;
    errno=0;
    while ((e=readdir(d))!=NULL)if (strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {
        result=1;
        break;
    }
    if (errno)result=-1;
    closedir(d);
    return result;
}

static int Contains(dev_t *ids,size_t count,dev_t id) {
    for (size_t i=0;i<count;++i)if (ids[i]==id)return 1;
    return 0;
}

static int DeviceNumber(const char *base,dev_t *id)
{
    char path[PATH_MAX],text[64];
    int n=snprintf(path,sizeof path,"%s/dev",base);
    unsigned a,b;
    char extra;
    if (n<0||(size_t)n>=sizeof path||!Text(path,text,sizeof text)||sscanf(text,"%u:%u%c",&a,&b,&extra)!=2)return 0;
    *id=makedev(a,b);
    return 1;
}

static int Usage(const char *base)
{
    dev_t ids[65];
    size_t count=1;
    if (!DeviceNumber(base,&ids[0]))return -1;
    char path[PATH_MAX];
    int n=snprintf(path,sizeof path,"%s/holders",base);
    if (n<0||(size_t)n>=sizeof path||HasEntries(path)!=0)return 1;
    DIR*d=opendir(base);
    if (!d)return -1;
    struct dirent*e;
    int result=0;
    while ((e=readdir(d))!=NULL) {
        if (!ValidName(e->d_name))continue;
        n=snprintf(path,sizeof path,"%s/%s/partition",base,e->d_name);
        if (n<0||(size_t)n>=sizeof path) {
            result=-1;
            break;
        }
        if (access(path,F_OK))continue;
        char child[PATH_MAX];
        n=snprintf(child,sizeof child,"%s/%s",base,e->d_name);
        if (n<0||(size_t)n>=sizeof child||count==65U||!DeviceNumber(child,&ids[count++])) {
            result=-1;
            break;
        }
        n=snprintf(path,sizeof path,"%s/holders",child);
        if (n<0||(size_t)n>=sizeof path||HasEntries(path)!=0) {
            result=1;
            break;
        }
    }
    closedir(d);
    if (result)return result;
    FILE *mounts=fopen("/proc/self/mountinfo","r");
    if (!mounts)return -1;
    char line[8192];
    while (fgets(line,sizeof line,mounts)) {
        unsigned a,b;
        if (!strchr(line,'\n')||sscanf(line,"%*u %*u %u:%u",&a,&b)!=2) {
            result=-1;
            break;
        }
        if (Contains(ids,count,makedev(a,b))) {
            result=1;
            break;
        }
    }
    if (ferror(mounts))result=-1;
    fclose(mounts);
    if (result)return result;
    FILE *swaps=fopen("/proc/swaps","r");
    if (!swaps)return -1;
    if (!fgets(line,sizeof line,swaps)) {
        fclose(swaps);
        return -1;
    }
    while (fgets(line,sizeof line,swaps)) {
        char file[4096];
        struct stat st;
        if (!strchr(line,'\n')||sscanf(line,"%4095s",file)!=1||strchr(file,'\\')||stat(file,&st)) {
            result=-1;
            break;
        }
        if (Contains(ids,count,S_ISBLK(st.st_mode)?st.st_rdev:st.st_dev)) {
            result=1;
            break;
        }
    }
    if (ferror(swaps))result=-1;
    fclose(swaps);
    return result;
}

UmiStatus BmDeviceCheck(const char *path,UmiBootMediaDevice *out)
{
    if (!path||!out||strncmp(path,"/dev/",5)||!ValidName(path+5))return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    strcpy(out->path,path);
    char base[PATH_MAX],p[PATH_MAX],physical[PATH_MAX];
    int n=snprintf(base,sizeof base,"/sys/class/block/%s",path+5);
    if (n<0||(size_t)n>=sizeof base)return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!realpath(base,physical))return UMI_STATUS_NOT_FOUND;
    n=snprintf(p,sizeof p,"%s/partition",base);
    if (n<0||(size_t)n>=sizeof p)return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!access(p,F_OK))return UMI_STATUS_PERMISSION_DENIED;
    out->usb=strstr(physical,"/usb")!=NULL;
    uint64_t removable=0,sectors=0,logical=0,seq=0,ro=1;

#define FIELD(name,variable) do { n=snprintf(p,sizeof p,"%s/%s",base,name);if(n<0||(size_t)n>=sizeof p||!Number(p,&variable))return UMI_STATUS_UNAVAILABLE; } while(0)
    FIELD("removable",removable);
    FIELD("size",sectors);
    FIELD("queue/logical_block_size",logical);
    FIELD("diskseq",seq);
    FIELD("ro",ro);

#undef FIELD
    if (sectors>UINT64_MAX/512U||logical>UINT32_MAX)return UMI_STATUS_PARSE_ERROR;
    out->bytes=sectors*512U;
    out->sectorBytes=(uint32_t)logical;
    out->removable=removable==1;
    out->readOnly=ro!=0;
    if (!out->usb||!out->removable)return UMI_STATUS_PERMISSION_DENIED;
    n=snprintf(p,sizeof p,"%s/device/model",base);
    if (n<0||(size_t)n>=sizeof p||!Text(p,out->model,sizeof out->model)||!out->model[0])return UMI_STATUS_UNAVAILABLE;

    /* Walk only this sysfs ancestry for a hardware serial, not arbitrary disks. */
    char ancestor[PATH_MAX];
    strcpy(ancestor,physical);
    for (unsigned depth=0;depth<20U&&!out->serial[0];++depth) {
        n=snprintf(p,sizeof p,"%s/serial",ancestor);
        if (n>=0&&(size_t)n<sizeof p)(void)Text(p,out->serial,sizeof out->serial);
        char *end=strrchr(ancestor,'/');
        if (!end||end<=ancestor+4)break;
        *end=0;
    }
    dev_t id;
    if (!DeviceNumber(base,&id)||!seq)return UMI_STATUS_UNAVAILABLE;
    n=snprintf(out->identity,sizeof out->identity,"linux:%u:%u:%" PRIu64 ":%.127s",major(id),minor(id),seq,
        out->serial);
    if (n<0||(size_t)n>=sizeof out->identity)return UMI_STATUS_CAPACITY_EXCEEDED;
    int usage=Usage(base);
    out->inUse=usage!=0;
    out->protectedDevice=usage<0;
    if (!out->serial[0]) {
        out->protectedDevice=1;
        strcpy(out->reason,"No stable serial was exposed by this USB device.");
    }
    else if (usage<0)strcpy(out->reason,"Mount, swap or holder information could not be completed.");
    else if (usage)strcpy(out->reason,"A partition is mounted, swap is active, or a kernel holder owns the disk.");
    else if (out->readOnly)strcpy(out->reason,"The kernel reports this disk as read-only.");
    else strcpy(out->reason,"No mount, swap or holder found; exclusive acquisition is still required.");
    return UMI_STATUS_OK;
}

UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *r)
{
    if (!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    DIR*d=opendir("/sys/class/block");
    if (!d)return BmMessage(r,UMI_STATUS_UNAVAILABLE,"Linux block inventory is unavailable.");
    struct dirent *e;
    while ((e=readdir(d))!=NULL) {
        if (!ValidName(e->d_name))continue;
        char path[128];
        int n=snprintf(path,sizeof path,"/dev/%s",e->d_name);
        if (n<0||(size_t)n>=sizeof path)continue;
        UmiBootMediaDevice item;
        UmiStatus s=BmDeviceCheck(path,&item);
        if (s!=UMI_STATUS_OK)continue;
        if (out->count==UMI_BOOT_MEDIA_DEVICES) {
            ++out->omitted;
            continue;
        }
        out->devices[out->count++]=item;
    }
    closedir(d);
    return BmMessage(r,out->omitted?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK,"Read-only inventory: %zu identified removable USB disks.",
        out->count);
}

UmiStatus BmDeviceOpen(const UmiBootMediaDevice *expected,int write,BmFile **out)
{
    if (!expected||!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    if (write&&!UmiBootMediaDeviceWritesEnabled())return UMI_STATUS_UNAVAILABLE;
    UmiBootMediaDevice actual;
    UmiStatus s=BmDeviceCheck(expected->path,&actual);
    if (s!=UMI_STATUS_OK)return s;
    if (!BmSameDevice(expected,&actual)||actual.inUse||actual.protectedDevice||actual.readOnly)return UMI_STATUS_PERMISSION_DENIED;
    int fd=open(expected->path,(write?O_RDWR:O_RDONLY)|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK|O_EXCL);
    if (fd<0)return UMI_STATUS_PERMISSION_DENIED;
    struct stat st;
    uint64_t bytes=0,seq=0;
    int sector=0;
    if (fstat(fd,&st)||!S_ISBLK(st.st_mode)||ioctl(fd,BLKGETSIZE64,&bytes)||ioctl(fd,BLKSSZGET,&sector)||flock(fd,
        LOCK_EX|LOCK_NB))s=UMI_STATUS_PERMISSION_DENIED;

#ifdef BLKGETDISKSEQ
    if (s==UMI_STATUS_OK&&ioctl(fd,BLKGETDISKSEQ,&seq))s=UMI_STATUS_UNAVAILABLE;

#else
    s=UMI_STATUS_UNAVAILABLE;

#endif
    if (s==UMI_STATUS_OK) {
        char id[UMI_BOOT_MEDIA_ID];
        int n=snprintf(id,sizeof id,"linux:%u:%u:%" PRIu64 ":%.127s",major(st.st_rdev),minor(st.st_rdev),seq,actual.serial);
        if (n<0||(size_t)n>=sizeof id||strcmp(id,expected->identity)||bytes!=expected->bytes||sector<0||(uint32_t)sector!=expected->sectorBytes)s=UMI_STATUS_INVALID_STATE;
    }
    if (s==UMI_STATUS_OK)s=BmDeviceCheck(expected->path,&actual);
    if (s==UMI_STATUS_OK&&(!BmSameDevice(expected,&actual)||actual.inUse||actual.protectedDevice))s=UMI_STATUS_PERMISSION_DENIED;
    if (s!=UMI_STATUS_OK) {
        close(fd);
        return s;
    }
    BmFile*f=calloc(1,sizeof *f);
    if (!f) {
        close(fd);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    f->fd=fd;
    f->bytes=bytes;
    f->writable=write;
    *out=f;
    return UMI_STATUS_OK;
}
