/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/native_launcher/io_linux.c
 * Purpose: Descriptor-relative, bounded native launcher staging on Linux.
 * Open every path component without following symlinks. Read only regular
 * files, stream binary digests, compare metadata before/after each read, and
 * publish the completion manifest last. This is not a sandbox against another
 * process with the same user identity, nor a signature or a persistent lock.
 *---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 200809L
#include "internal.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static UmiStatus Failure(UmiNativeStageReport *r,const char *phase,const char *detail,int error)
{
    UmiStatus status=UMI_STATUS_IO_ERROR;
    if(error==ENOENT)status=UMI_STATUS_NOT_FOUND;
    else if(error==EEXIST)status=UMI_STATUS_ALREADY_EXISTS;
    else if(error==EACCES||error==EPERM||error==ELOOP)status=UMI_STATUS_PERMISSION_DENIED;
    return UmiNativeReportSet(r,status,phase,detail,error);
}
static int TrustedDirectory(const struct stat *info)
{
    if(!S_ISDIR(info->st_mode)||(info->st_uid!=0U&&info->st_uid!=getuid()))return 0;
    /* A root-owned sticky temporary ancestor is permitted; the final staging
     * parent must still be private to the caller. No writable non-sticky
     * ancestor is accepted, so path replacement by another user is rejected. */
    return (info->st_mode&0022)==0 || (info->st_uid==0U&&(info->st_mode&01000)!=0);
}
static int Parent(const char *path,char leaf[256],UmiNativeStageReport *r,int requireTrusted)
{
    if(UmiNativeStageValidatePath(path,0)!=UMI_STATUS_OK) {
        UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"path","Use a canonical absolute UTF-8 path without links, dot segments or percent characters.",0);return -1;
    }
    int directory=open("/",O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if(directory<0){Failure(r,"path","Could not open the filesystem root.",errno);return -1;}
    const char *part=path+1;
    for(;;) {
        const char *slash=strchr(part,'/');size_t count=slash==NULL?strlen(part):(size_t)(slash-part);
        char component[256];memcpy(component,part,count);component[count]='\0';
        if(slash==NULL){memcpy(leaf,component,count+1U);return directory;}
        int next=openat(directory,component,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
        int error=errno;close(directory);
        if(next<0){Failure(r,"path","A parent directory is missing, inaccessible or a symbolic link.",error);return -1;}
        struct stat info;
        if(fstat(next,&info)!=0){error=errno;close(next);Failure(r,"path","Could not inspect a parent directory.",error);return -1;}
        if(requireTrusted && !TrustedDirectory(&info)){close(next);UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"path","A parent directory has unsafe ownership or write permissions.",0);return -1;}
        directory=next;part=slash+1;
    }
}
static int Same(const struct stat *a,const struct stat *b)
{
    return a->st_dev==b->st_dev&&a->st_ino==b->st_ino&&a->st_size==b->st_size
        &&a->st_mode==b->st_mode&&a->st_uid==b->st_uid&&a->st_gid==b->st_gid
        &&a->st_mtim.tv_sec==b->st_mtim.tv_sec&&a->st_mtim.tv_nsec==b->st_mtim.tv_nsec
        &&a->st_ctim.tv_sec==b->st_ctim.tv_sec&&a->st_ctim.tv_nsec==b->st_ctim.tv_nsec;
}
/* output may be NULL for a streaming binary read. Small text/icon reads return
 * a separately allocated buffer with a spare NUL byte, not a borrowed view. */
static UmiStatus ReadAt(int parent,const char *name,uint64_t limit,int executable,int ownedOutput,
    unsigned char **output,size_t *length,char hash[65],unsigned char header[64],UmiNativeStageReport *r)
{
    if(output!=NULL)*output=NULL;
    *length=0U;hash[0]='\0';
    int file=openat(parent,name,O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
    if(file<0)return Failure(r,"read","Could not open a regular input without following links.",errno);
    struct stat before,after;UmiStatus status=UMI_STATUS_OK;unsigned char *saved=NULL;
    if(fstat(file,&before)!=0){status=Failure(r,"read","Could not inspect the opened file.",errno);goto done;}
    if(!S_ISREG(before.st_mode)||before.st_size<0||(uint64_t)before.st_size>limit) {
        status=UmiNativeReportSet(r,UMI_STATUS_CAPACITY_EXCEEDED,"read","Input must be a regular file within its byte limit.",0);goto done;
    }
    if(executable && ((before.st_mode&0022)!=0 ||(before.st_mode&06000)!=0 ||(before.st_mode&0111)==0
        ||(before.st_uid!=0U&&before.st_uid!=getuid()))) {
        status=UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"executable","Binary must be executable, owned by this user or root, not set-id, and not group/other writable.",0);goto done;
    }
    if(ownedOutput && (before.st_uid!=getuid()||(before.st_mode&0022)!=0||(before.st_mode&07111)!=0||before.st_nlink!=1U)) {
        status=UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"stage","Staged files must be owned by this user, non-executable, not shared hardlinks and not group/other writable.",0);goto done;
    }
    if(output!=NULL) {
        if((uint64_t)before.st_size>=SIZE_MAX){status=UMI_STATUS_CAPACITY_EXCEEDED;goto done;}
        saved=malloc((size_t)before.st_size+1U);
        if(saved==NULL){status=UmiNativeReportSet(r,UMI_STATUS_OUT_OF_MEMORY,"read","Could not allocate the bounded input buffer.",0);goto done;}
    }
    UmiNativeSha256 digest;UmiNativeSha256Init(&digest);
    uint64_t total=0U;unsigned char chunk[65536];size_t headerUsed=0U;
    for(;;) {
        ssize_t got=read(file,chunk,sizeof chunk);
        if(got<0){if(errno==EINTR)continue;status=Failure(r,"read","Input read failed.",errno);goto done;}
        if(got==0)break;
        size_t count=(size_t)got;
        if((uint64_t)count>limit-total || (output!=NULL && (uint64_t)count>(uint64_t)before.st_size-total)) {
            status=UmiNativeReportSet(r,UMI_STATUS_BUSY,"read","Input grew while it was being read; no snapshot was accepted.",0);goto done;
        }
        if(saved!=NULL)memcpy(saved+(size_t)total,chunk,count);
        if(header!=NULL && headerUsed<64U){size_t take=64U-headerUsed;if(take>count)take=count;memcpy(header+headerUsed,chunk,take);headerUsed+=take;}
        status=UmiNativeSha256Update(&digest,chunk,count);if(status!=UMI_STATUS_OK)goto done;
        total+=(uint64_t)count;
    }
    if(fstat(file,&after)!=0){status=Failure(r,"read","Final input inspection failed.",errno);goto done;}
    if(!Same(&before,&after)||total!=(uint64_t)before.st_size) {
        status=UmiNativeReportSet(r,UMI_STATUS_BUSY,"read","Input changed during its read; review it again.",0);goto done;
    }
    unsigned char raw[32];status=UmiNativeSha256Final(&digest,raw);
    if(status==UMI_STATUS_OK){UmiNativeSha256Hex(raw,hash);*length=(size_t)total;if(saved!=NULL)saved[*length]=0U;}
done:
    if(close(file)!=0 && status==UMI_STATUS_OK)status=Failure(r,"read","Input close failed.",errno);
    if(status==UMI_STATUS_OK && output!=NULL)*output=saved;else free(saved);
    if(status!=UMI_STATUS_OK){*length=0U;hash[0]='\0';}
    return status;
}
static UmiStatus ReadPath(const char *path,uint64_t limit,int executable,unsigned char **out,size_t *length,
    char hash[65],unsigned char header[64],UmiNativeStageReport *r)
{
    char leaf[256];int parent=Parent(path,leaf,r,executable);if(parent<0)return r->status;
    UmiStatus status=ReadAt(parent,leaf,limit,executable,0,out,length,hash,header,r);close(parent);return status;
}
static UmiStatus Binary(const char *path,UmiNativeStageBinary *binary,UmiNativeStageReport *r)
{
    unsigned char header[64]={0};size_t length=0U;
    UmiStatus status=ReadPath(path,UMI_NATIVE_STAGE_BINARY_LIMIT,1,NULL,&length,binary->sha256,header,r);
    if(status!=UMI_STATUS_OK)return status;
    status=UmiNativeStageElfCheck(header,length<64U?length:64U,(uint64_t)length,&binary->machine);
    if(status!=UMI_STATUS_OK)return UmiNativeReportSet(r,status,"ELF","Select a structurally bounded 64-bit little-endian Linux ELF executable, not a script or Windows binary.",0);
    memcpy(binary->path,path,strlen(path)+1U);binary->bytes=(uint64_t)length;return UMI_STATUS_OK;
}
static UmiStatus Recheck(const UmiNativeStageManifest *m,UmiNativeStageReport *r)
{
    const UmiNativeStageBinary *values[2]={&m->desk,&m->launcher};
    for(size_t i=0U;i<2U;++i) {
        UmiNativeStageBinary current={0};UmiStatus status=Binary(values[i]->path,&current,r);
        if(status!=UMI_STATUS_OK)return status;
        if(current.bytes!=values[i]->bytes||current.machine!=values[i]->machine||strcmp(current.sha256,values[i]->sha256)!=0)
            return UmiNativeReportSet(r,UMI_STATUS_BUSY,"binary-check","A referenced executable changed. Create and review a new plan.",0);
    }
    return UMI_STATUS_OK;
}
static int DestinationParent(const char *path,char leaf[256],UmiNativeStageReport *r)
{
    int parent=Parent(path,leaf,r,1);if(parent<0)return -1;
    struct stat info;
    if(fstat(parent,&info)!=0){int error=errno;close(parent);Failure(r,"destination","Could not inspect destination parent.",error);return -1;}
    if(info.st_uid!=getuid()||(info.st_mode&0022)!=0) {
        close(parent);UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"destination","Choose a destination directly inside a directory owned by you and not writable by group/others.",0);return -1;
    }
    if(fstatat(parent,leaf,&info,AT_SYMLINK_NOFOLLOW)==0) {
        close(parent);UmiNativeReportSet(r,UMI_STATUS_ALREADY_EXISTS,"destination","Destination already exists. Choose a new directory; nothing is overwritten.",0);return -1;
    }
    if(errno!=ENOENT){int error=errno;close(parent);Failure(r,"destination","Could not check destination absence.",error);return -1;}
    return parent;
}
UmiStatus UmiNativeStagePlanCreate(const UmiNativeStageConfig *c,UmiNativeStagePlan **outPlan,UmiNativeStageReport *report)
{
    UmiNativeStageReport local;UmiNativeStageReport *r=report==NULL?&local:report;UmiNativeReportInit(r);
    if(outPlan!=NULL)*outPlan=NULL;
    if(c==NULL||outPlan==NULL||UmiNativeStageValidatePath(c->deskPath,0)!=UMI_STATUS_OK
        ||UmiNativeStageValidatePath(c->launcherPath,1)!=UMI_STATUS_OK||UmiNativeStageValidatePath(c->outputPath,0)!=UMI_STATUS_OK
        ||(c->iconPath!=NULL&&UmiNativeStageValidatePath(c->iconPath,0)!=UMI_STATUS_OK))
        return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"plan","Supply canonical absolute paths; the launcher executable path may not contain '='.",0);
    char leaf[256];int parent=DestinationParent(c->outputPath,leaf,r);if(parent<0)return r->status;close(parent);
    UmiNativeStagePlan *plan=calloc(1U,sizeof *plan);
    if(plan==NULL)return UmiNativeReportSet(r,UMI_STATUS_OUT_OF_MEMORY,"plan","Could not allocate a review plan.",0);
    memcpy(plan->manifest.root,c->outputPath,strlen(c->outputPath)+1U);
    UmiStatus status=Binary(c->deskPath,&plan->manifest.desk,r);
    if(status==UMI_STATUS_OK)status=Binary(c->launcherPath,&plan->manifest.launcher,r);
    if(status==UMI_STATUS_OK && plan->manifest.desk.machine!=plan->manifest.launcher.machine)
        status=UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"architecture","Desk and the session launcher target different architectures.",0);
    if(status==UMI_STATUS_OK && c->iconPath!=NULL)
        status=ReadPath(c->iconPath,UMI_NATIVE_STAGE_ICON_LIMIT,0,&plan->icon,&plan->iconLength,plan->manifest.iconHash,NULL,r);
    else if(status==UMI_STATUS_OK) {
        const unsigned char *brand=UmiNativeBrandIcon(&plan->iconLength);
        plan->icon=malloc(plan->iconLength);
        if(plan->icon==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
        else {memcpy(plan->icon,brand,plan->iconLength);status=UmiNativeSha256Buffer(plan->icon,plan->iconLength,plan->manifest.iconHash);}
    }
    if(status==UMI_STATUS_OK) {
        status=UmiNativeStagePngCheck(plan->icon,plan->iconLength);
        if(status!=UMI_STATUS_OK)UmiNativeReportSet(r,status,"icon","PNG signature, chunk bounds, ordering or CRC did not validate.",0);
    }
    if(status==UMI_STATUS_OK) {
        plan->entry=malloc(UMI_NATIVE_STAGE_TEXT_CAPACITY);
        if(plan->entry==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
        else status=UmiNativeStageRenderEntry(&plan->manifest,plan->entry,UMI_NATIVE_STAGE_TEXT_CAPACITY,&plan->entryLength);
    }
    if(status==UMI_STATUS_OK)status=UmiNativeSha256Buffer(plan->entry,plan->entryLength,plan->manifest.entryHash);
    if(status==UMI_STATUS_OK)status=UmiNativePlanFingerprint(plan);
    if(status!=UMI_STATUS_OK){UmiNativeStagePlanDestroy(plan);if(r->status==UMI_STATUS_OK)UmiNativeReportSet(r,status,"plan","Plan preparation did not complete.",0);return status;}
    *outPlan=plan;return UmiNativeReportSet(r,UMI_STATUS_OK,"plan","Inputs inspected; no output directory was created and no application was started.",0);
}
static UmiStatus WriteAt(int dir,const char *name,const void *bytes,size_t length,UmiNativeStageReport *r)
{
    int file=openat(dir,name,O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,0644);
    if(file<0)return Failure(r,"write","Could not exclusively create a staged file. Partial output is retained.",errno);
    const unsigned char *p=bytes;size_t used=0U;UmiStatus status=UMI_STATUS_OK;
    while(used<length) {
        ssize_t count=write(file,p+used,length-used);
        if(count<0&&errno==EINTR)continue;
        if(count<=0){status=Failure(r,"write","Staged write failed. The partial file and directory are retained.",count==0?EIO:errno);break;}
        used+=(size_t)count;
    }
    if(status==UMI_STATUS_OK&&fsync(file)!=0)status=Failure(r,"write","Staged file could not be synchronised.",errno);
    if(close(file)!=0&&status==UMI_STATUS_OK)status=Failure(r,"write","Staged file close failed.",errno);
    return status;
}
UmiStatus UmiNativeStageCommit(UmiNativeStagePlan *plan,const char *expected,UmiNativeStageReport *report)
{
    UmiNativeStageReport local;UmiNativeStageReport *r=report==NULL?&local:report;UmiNativeReportInit(r);
    if(plan==NULL||plan->consumed)return UmiNativeReportSet(r,UMI_STATUS_INVALID_STATE,"commit","A fresh plan is required for each commit attempt.",0);
    plan->consumed=1;
    if(expected!=NULL && (strlen(expected)!=64U||!UmiNativeHashValid(expected)||strcmp(expected,plan->fingerprint)!=0))
        return UmiNativeReportSet(r,UMI_STATUS_BUSY,"review","The supplied review fingerprint does not match this plan. No directory was created.",0);
    UmiStatus status=Recheck(&plan->manifest,r);if(status!=UMI_STATUS_OK)return status;
    char *manifest=malloc(UMI_NATIVE_STAGE_TEXT_CAPACITY);size_t manifestLength=0U;
    if(manifest==NULL)return UmiNativeReportSet(r,UMI_STATUS_OUT_OF_MEMORY,"commit","Could not allocate manifest output.",0);
    status=UmiNativeStageManifestWrite(&plan->manifest,manifest,UMI_NATIVE_STAGE_TEXT_CAPACITY,&manifestLength);
    if(status!=UMI_STATUS_OK){free(manifest);return status;}
    char leaf[256];int parent=DestinationParent(plan->manifest.root,leaf,r);
    if(parent<0){free(manifest);return r->status;}
    if(mkdirat(parent,leaf,0700)!=0){int error=errno;close(parent);free(manifest);return Failure(r,"create","Could not create a new review directory.",error);}
    r->outputCreated=1;
    int directory=openat(parent,leaf,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);int error=errno;
    if(directory<0){close(parent);free(manifest);return Failure(r,"create","New directory could not be opened; it is retained.",error);}
    struct stat info;
    if(fstat(directory,&info)!=0)status=Failure(r,"create","Could not inspect the new directory.",errno);
    else if(info.st_uid!=getuid()||(info.st_mode&0777)!=0700)status=UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"create","New directory is not a private 0700 directory; it is retained.",0);
    if(status==UMI_STATUS_OK)status=WriteAt(directory,UMI_NATIVE_STAGE_ICON_NAME,plan->icon,plan->iconLength,r);
    if(status==UMI_STATUS_OK)status=WriteAt(directory,UMI_NATIVE_STAGE_ENTRY_NAME,plan->entry,plan->entryLength,r);
    if(status==UMI_STATUS_OK)status=Recheck(&plan->manifest,r);
    /* The manifest is the completion record, not a promise of atomic multi-file
     * publication. A write/sync failure remains visible and retains evidence. */
    if(status==UMI_STATUS_OK)status=WriteAt(directory,UMI_NATIVE_STAGE_MANIFEST_NAME,manifest,manifestLength,r);
    if(status==UMI_STATUS_OK && (fsync(directory)!=0||fsync(parent)!=0))status=Failure(r,"sync","Directory synchronisation failed; re-verify the retained stage before use.",errno);
    close(directory);close(parent);free(manifest);
    if(status==UMI_STATUS_OK){r->completed=1;UmiNativeReportSet(r,status,"stage","Staged only. No application was started and no desktop setting was changed.",0);}
    return status;
}
static UmiStatus Inventory(int directory,UmiNativeStageReport *r)
{
    int copy=dup(directory);if(copy<0)return Failure(r,"inventory","Could not inspect stage inventory.",errno);
    DIR *stream=fdopendir(copy);if(stream==NULL){int error=errno;close(copy);return Failure(r,"inventory","Could not read stage inventory.",error);}
    unsigned mask=0U;UmiStatus status=UMI_STATUS_OK;struct dirent *entry;errno=0;
    while((entry=readdir(stream))!=NULL) {
        unsigned bit;
        if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)continue;
        if(strcmp(entry->d_name,UMI_NATIVE_STAGE_ICON_NAME)==0)bit=1U;
        else if(strcmp(entry->d_name,UMI_NATIVE_STAGE_ENTRY_NAME)==0)bit=2U;
        else if(strcmp(entry->d_name,UMI_NATIVE_STAGE_MANIFEST_NAME)==0)bit=4U;
        else {status=UmiNativeReportSet(r,UMI_STATUS_INVALID_STATE,"inventory","Unexpected file in the review directory. Exactly the icon, desktop entry and manifest are allowed.",0);break;}
        mask|=bit;errno=0;
    }
    int error=errno;closedir(stream);
    if(status!=UMI_STATUS_OK)return status;
    if(error!=0)return Failure(r,"inventory","Directory enumeration failed.",error);
    if(mask!=7U)return UmiNativeReportSet(r,UMI_STATUS_INVALID_STATE,"inventory","Stage is incomplete; an expected output is missing.",0);
    return UMI_STATUS_OK;
}
UmiStatus UmiNativeStageVerify(const char *path,UmiNativeStageReport *report)
{
    UmiNativeStageReport local;UmiNativeStageReport *r=report==NULL?&local:report;UmiNativeReportInit(r);
    char leaf[256];int parent=Parent(path,leaf,r,1);if(parent<0)return r->status;
    int directory=openat(parent,leaf,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);int error=errno;close(parent);
    if(directory<0)return Failure(r,"verify","Could not open the review directory without following links.",error);
    struct stat info;UmiStatus status=UMI_STATUS_OK;
    unsigned char *manifestBytes=NULL,*icon=NULL,*entryBytes=NULL;size_t manifestLength=0U,iconLength=0U,entryLength=0U;
    char unused[65],iconHash[65],entryHash[65];UmiNativeStageManifest *m=calloc(1U,sizeof *m);char *expected=malloc(UMI_NATIVE_STAGE_TEXT_CAPACITY);
    if(m==NULL||expected==NULL){status=UmiNativeReportSet(r,UMI_STATUS_OUT_OF_MEMORY,"verify","Could not allocate verification state.",0);goto done;}
    if(fstat(directory,&info)!=0){status=Failure(r,"verify","Could not inspect stage ownership.",errno);goto done;}
    if(info.st_uid!=getuid()||(info.st_mode&0777)!=0700){status=UmiNativeReportSet(r,UMI_STATUS_PERMISSION_DENIED,"verify","Stage directory must be owned by this user with private 0700 permissions.",0);goto done;}
    status=Inventory(directory,r);if(status!=UMI_STATUS_OK)goto done;
    status=ReadAt(directory,UMI_NATIVE_STAGE_MANIFEST_NAME,UMI_NATIVE_STAGE_TEXT_CAPACITY-1U,0,1,&manifestBytes,&manifestLength,unused,NULL,r);
    if(status!=UMI_STATUS_OK)goto done;
    status=UmiNativeStageManifestRead((const char *)manifestBytes,manifestLength,m);
    if(status!=UMI_STATUS_OK){UmiNativeReportSet(r,status,"manifest","Manifest has unknown, missing or duplicate fields, invalid Unicode/numbers, or an unsupported schema.",0);goto done;}
    if(strcmp(path,m->root)!=0){status=UmiNativeReportSet(r,UMI_STATUS_INVALID_STATE,"root","Stage was moved or records another location. Create a new stage at its final review location.",0);goto done;}
    status=ReadAt(directory,UMI_NATIVE_STAGE_ICON_NAME,UMI_NATIVE_STAGE_ICON_LIMIT,0,1,&icon,&iconLength,iconHash,NULL,r);
    if(status==UMI_STATUS_OK)status=ReadAt(directory,UMI_NATIVE_STAGE_ENTRY_NAME,UMI_NATIVE_STAGE_TEXT_CAPACITY-1U,0,1,&entryBytes,&entryLength,entryHash,NULL,r);
    if(status!=UMI_STATUS_OK)goto done;
    if(strcmp(iconHash,m->iconHash)!=0||strcmp(entryHash,m->entryHash)!=0){status=UmiNativeReportSet(r,UMI_STATUS_BUSY,"hash","Staged file bytes no longer match the recorded hashes.",0);goto done;}
    status=UmiNativeStagePngCheck(icon,iconLength);
    if(status!=UMI_STATUS_OK){UmiNativeReportSet(r,status,"icon","The staged PNG envelope is invalid.",0);goto done;}
    size_t expectedLength=0U;status=UmiNativeStageRenderEntry(m,expected,UMI_NATIVE_STAGE_TEXT_CAPACITY,&expectedLength);
    if(status!=UMI_STATUS_OK)goto done;
    if(expectedLength!=entryLength||memcmp(expected,entryBytes,entryLength)!=0) {
        status=UmiNativeReportSet(r,UMI_STATUS_INVALID_STATE,"entry","Desktop entry differs from the Framework-generated command, even if its recorded hash was changed.",0);goto done;
    }
    status=Recheck(m,r);
    if(status==UMI_STATUS_OK){r->completed=1;UmiNativeReportSet(r,status,"verify","Stage and referenced executable bytes agree. This is not a signature or a graphical-session test.",0);}
done:
    free(expected);free(m);free(manifestBytes);free(icon);free(entryBytes);close(directory);return status;
}
UmiStatus UmiNativeStageDigestFile(const char *path,char outHex[65],uint64_t *outBytes,UmiNativeStageReport *report)
{
    UmiNativeStageReport local;UmiNativeStageReport *r=report==NULL?&local:report;UmiNativeReportInit(r);
    if(outHex==NULL||outBytes==NULL)return UmiNativeReportSet(r,UMI_STATUS_INVALID_ARGUMENT,"digest","Digest output pointers are required.",0);
    *outBytes=0U;outHex[0]='\0';size_t count=0U;
    UmiStatus status=ReadPath(path,UMI_NATIVE_STAGE_BINARY_LIMIT,0,NULL,&count,outHex,NULL,r);
    if(status==UMI_STATUS_OK){*outBytes=(uint64_t)count;r->completed=1;UmiNativeReportSet(r,status,"digest","Regular-file digest completed. No input was executed.",0);}
    return status;
}
