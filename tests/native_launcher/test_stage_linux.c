/*-----------------------------------------------------------------------------
 * Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Actual Linux filesystem operations on private disposable test directories.
 * The copied native child is an inert fixture; staging must NEVER execute it.
 * Failed test directories remain for inspection. No user input tree is erased.
 *---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 200809L
#include "test_support.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
typedef struct Fixture {char root[512],desk[1024],launcher[1024],output[1024];UmiNativeStageConfig config;} Fixture;
static int Write(const char *path,const void *bytes,size_t length,mode_t mode)
{
    int f=open(path,O_WRONLY|O_CREAT|O_TRUNC,mode);if(f<0)return 0;const unsigned char *p=bytes;size_t at=0U;
    while(at<length){ssize_t count=write(f,p+at,length-at);if(count<=0){close(f);return 0;}at+=(size_t)count;}
    return close(f)==0;
}
static int Copy(const char *source,const char *destination)
{
    FILE *in=fopen(source,"rb"),*out=NULL;if(in==NULL)return 0;out=fopen(destination,"wb");if(out==NULL){fclose(in);return 0;}
    unsigned char bytes[65536];size_t count;int ok=1;
    while((count=fread(bytes,1U,sizeof bytes,in))!=0U)if(fwrite(bytes,1U,count,out)!=count){ok=0;break;}
    if(ferror(in))ok=0;
    if(fclose(in)!=0)ok=0;
    if(fclose(out)!=0)ok=0;
    return ok&&chmod(destination,0700)==0;
}
static int Prepare(Fixture *f)
{
    memset(f,0,sizeof *f);strcpy(f->root,"/tmp/umicom-native-stage-XXXXXX");if(mkdtemp(f->root)==NULL)return 0;
    (void)snprintf(f->desk,sizeof f->desk,"%s/desk",f->root);(void)snprintf(f->launcher,sizeof f->launcher,"%s/launcher",f->root);
    (void)snprintf(f->output,sizeof f->output,"%s/review",f->root);
    if(TestBinary==NULL||!Copy(TestBinary,f->desk)||!Copy(TestBinary,f->launcher))return 0;
    f->config=(UmiNativeStageConfig){f->desk,f->launcher,f->output,NULL};return 1;
}
static int RemoveTree(const char *path)
{
    struct stat st;if(lstat(path,&st)!=0)return errno==ENOENT;
    if(!S_ISDIR(st.st_mode))return unlink(path)==0;
    DIR *d=opendir(path);if(d==NULL)return 0;struct dirent *e;int ok=1;
    while((e=readdir(d))!=NULL){if(strcmp(e->d_name,".")==0||strcmp(e->d_name,"..")==0)continue;char child[2048];
        int n=snprintf(child,sizeof child,"%s/%s",path,e->d_name);if(n<0||(size_t)n>=sizeof child||!RemoveTree(child)){ok=0;break;}}
    closedir(d);return ok&&rmdir(path)==0;
}
static int Stage(Fixture *f,UmiNativeStageReport *report)
{
    UmiNativeStagePlan *plan=NULL;UmiStatus status=UmiNativeStagePlanCreate(&f->config,&plan,report);
    if(status==UMI_STATUS_OK)status=UmiNativeStageCommit(plan,NULL,report);
    UmiNativeStagePlanDestroy(plan);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"stage [%s]: %s\n",report->phase,report->detail);
    return status==UMI_STATUS_OK;
}
static int Mutate(const char *path)
{
    int file=open(path,O_RDWR);if(file<0)return 0;unsigned char value;
    if(lseek(file,-1,SEEK_END)<0||read(file,&value,1U)!=1||lseek(file,-1,SEEK_END)<0){close(file);return 0;}
    value^=1U;int ok=write(file,&value,1U)==1;close(file);return ok;
}
static int ReadManifest(Fixture *f,UmiNativeStageManifest *m)
{
    char path[2048],text[UMI_NATIVE_STAGE_TEXT_CAPACITY];(void)snprintf(path,sizeof path,"%s/manifest.json",f->output);
    FILE *in=fopen(path,"rb");if(in==NULL)return 0;size_t n=fread(text,1U,sizeof text,in);fclose(in);
    return UmiNativeStageManifestRead(text,n,m)==UMI_STATUS_OK;
}
static int WriteManifest(Fixture *f,const UmiNativeStageManifest *m)
{
    char path[2048],text[UMI_NATIVE_STAGE_TEXT_CAPACITY];size_t n;
    (void)snprintf(path,sizeof path,"%s/manifest.json",f->output);
    return UmiNativeStageManifestWrite(m,text,sizeof text,&n)==UMI_STATUS_OK&&Write(path,text,n,0600);
}
int TestStagePlan(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);CHECK(plan!=NULL&&r.outputCreated==0);
    CHECK(access(f.output,F_OK)!=0&&errno==ENOENT);CHECK(strlen(UmiNativeStagePlanFingerprint(plan))==64U);
    UmiNativeStagePlanDestroy(plan);CHECK(RemoveTree(f.root));return 0;
}
int TestStageRoundtrip(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));CHECK(r.completed&&r.outputCreated);
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);CHECK(r.completed&&!r.outputCreated);
    UmiNativeStageManifest m;CHECK(ReadManifest(&f,&m));CHECK(strcmp(m.desk.path,f.desk)==0);CHECK(RemoveTree(f.root));return 0;
}
int TestStageExpected(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);char expected[65];strcpy(expected,UmiNativeStagePlanFingerprint(plan));
    CHECK(UmiNativeStageCommit(plan,expected,&r)==UMI_STATUS_OK);CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);
    UmiNativeStagePlanDestroy(plan);CHECK(RemoveTree(f.root));return 0;
}
int TestStageWrongReview(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);CHECK(UmiNativeStageCommit(plan,"wrong",&r)==UMI_STATUS_BUSY);
    CHECK(!r.outputCreated&&access(f.output,F_OK)!=0);CHECK(UmiNativeStageCommit(plan,NULL,&r)==UMI_STATUS_INVALID_STATE);
    UmiNativeStagePlanDestroy(plan);CHECK(RemoveTree(f.root));return 0;
}
int TestStageStalePlan(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);CHECK(Mutate(f.desk));CHECK(UmiNativeStageCommit(plan,NULL,&r)==UMI_STATUS_BUSY);
    CHECK(!r.outputCreated&&access(f.output,F_OK)!=0);UmiNativeStagePlanDestroy(plan);CHECK(RemoveTree(f.root));return 0;
}
int TestStageOverwrite(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_ALREADY_EXISTS);CHECK(plan==NULL);
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);CHECK(RemoveTree(f.root));return 0;
}
int TestStageDirectoryRace(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);
    CHECK(mkdir(f.output,0700)==0);CHECK(UmiNativeStageCommit(plan,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);CHECK(!r.outputCreated);
    UmiNativeStagePlanDestroy(plan);CHECK(RemoveTree(f.root));return 0;
}
int TestStageInputSymlink(void)
{
    Fixture f;CHECK(Prepare(&f));CHECK(unlink(f.desk)==0&&symlink(f.launcher,f.desk)==0);UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)!=UMI_STATUS_OK&&plan==NULL);CHECK(!r.outputCreated);CHECK(RemoveTree(f.root));return 0;
}
int TestStageParentSymlink(void)
{
    Fixture f;CHECK(Prepare(&f));char linked[1024],desk[1200];(void)snprintf(linked,sizeof linked,"%s/link",f.root);CHECK(symlink(f.root,linked)==0);
    (void)snprintf(desk,sizeof desk,"%s/desk",linked);f.config.deskPath=desk;UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)!=UMI_STATUS_OK);CHECK(plan==NULL);CHECK(RemoveTree(f.root));return 0;
}
int TestStageFifo(void)
{
    Fixture f;CHECK(Prepare(&f));CHECK(unlink(f.desk)==0&&mkfifo(f.desk,0700)==0);UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
    CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)!=UMI_STATUS_OK);CHECK(plan==NULL);CHECK(RemoveTree(f.root));return 0;
}
int TestStagePermissions(void)
{
    Fixture f;CHECK(Prepare(&f));mode_t bad[]={0777,06700,0600};
    for(size_t i=0U;i<3U;++i){CHECK(chmod(f.desk,bad[i])==0);UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;
        CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_PERMISSION_DENIED);CHECK(plan==NULL);}
    CHECK(RemoveTree(f.root));return 0;
}
int TestStageOversize(void)
{
    Fixture f;CHECK(Prepare(&f));int file=open(f.desk,O_WRONLY);CHECK(file>=0);CHECK(ftruncate(file,(off_t)(UMI_NATIVE_STAGE_BINARY_LIMIT+1U))==0);CHECK(close(file)==0);
    UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(plan==NULL);CHECK(RemoveTree(f.root));return 0;
}
int TestStageMachineMismatch(void)
{
    Fixture f;CHECK(Prepare(&f));int file=open(f.launcher,O_WRONLY);CHECK(file>=0);CHECK(lseek(file,18,SEEK_SET)==18);unsigned char machine[2]={243U,0U};
#if defined(__riscv)
    machine[0]=62U;
#endif
    CHECK(write(file,machine,2U)==2);close(file);
    UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_INVALID_ARGUMENT);CHECK(plan==NULL);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyChangedBinary(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));CHECK(Mutate(f.launcher));CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_BUSY);CHECK(!r.completed);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyChangedEntry(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/%s",f.output,UMI_NATIVE_STAGE_ENTRY_NAME);
    CHECK(Mutate(path));CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_BUSY);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyRehashedEntry(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/%s",f.output,UMI_NATIVE_STAGE_ENTRY_NAME);
    const char *attack="[Desktop Entry]\nExec=/bin/false\n";CHECK(Write(path,attack,strlen(attack),0600));UmiNativeStageManifest m;CHECK(ReadManifest(&f,&m));
    CHECK(UmiNativeSha256Buffer(attack,strlen(attack),m.entryHash)==UMI_STATUS_OK);CHECK(WriteManifest(&f,&m));
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_INVALID_STATE);CHECK(strcmp(r.phase,"entry")==0);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyRehashedPng(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/%s",f.output,UMI_NATIVE_STAGE_ICON_NAME);
    const char *invalid="not a PNG";CHECK(Write(path,invalid,strlen(invalid),0600));UmiNativeStageManifest m;CHECK(ReadManifest(&f,&m));
    CHECK(UmiNativeSha256Buffer(invalid,strlen(invalid),m.iconHash)==UMI_STATUS_OK);CHECK(WriteManifest(&f,&m));CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_PARSE_ERROR);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyExtra(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/extra",f.output);CHECK(Write(path,"x",1U,0600));
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_INVALID_STATE);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyMissing(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/manifest.json",f.output);CHECK(unlink(path)==0);
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_INVALID_STATE);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifyMoved(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char moved[1024];(void)snprintf(moved,sizeof moved,"%s/moved",f.root);CHECK(rename(f.output,moved)==0);
    CHECK(UmiNativeStageVerify(moved,&r)==UMI_STATUS_INVALID_STATE);CHECK(strcmp(r.phase,"root")==0);CHECK(RemoveTree(f.root));return 0;
}
int TestStageVerifySymlink(void)
{
    Fixture f;CHECK(Prepare(&f));UmiNativeStageReport r;CHECK(Stage(&f,&r));char path[2048];(void)snprintf(path,sizeof path,"%s/manifest.json",f.output);CHECK(unlink(path)==0);CHECK(symlink(f.desk,path)==0);
    CHECK(UmiNativeStageVerify(f.output,&r)!=UMI_STATUS_OK);CHECK(RemoveTree(f.root));return 0;
}
int TestStagePartialWrite(void)
{
    Fixture f;CHECK(Prepare(&f));pid_t child=fork();CHECK(child>=0);
    if(child==0){UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;if(UmiNativeStagePlanCreate(&f.config,&plan,&r)!=UMI_STATUS_OK)_exit(2);
        struct rlimit limit={0U,0U};(void)signal(SIGXFSZ,SIG_IGN);if(setrlimit(RLIMIT_FSIZE,&limit)!=0)_exit(3);
        UmiStatus status=UmiNativeStageCommit(plan,NULL,&r);UmiNativeStagePlanDestroy(plan);_exit(status!=UMI_STATUS_OK&&r.outputCreated&&!r.completed?0:4);}
    int result;CHECK(waitpid(child,&result,0)==child&&WIFEXITED(result)&&WEXITSTATUS(result)==0);
    CHECK(access(f.output,F_OK)==0);char path[2048];(void)snprintf(path,sizeof path,"%s/manifest.json",f.output);CHECK(access(path,F_OK)!=0);
    UmiNativeStageReport r;CHECK(UmiNativeStageVerify(f.output,&r)!=UMI_STATUS_OK);CHECK(RemoveTree(f.root));return 0;
}
int TestStageUnicode(void)
{
    Fixture f;CHECK(Prepare(&f));char renamed[1024];(void)snprintf(renamed,sizeof renamed,"%s/Desk \"quoted\" $value \\ ; caf\xc3\xa9-\xf0\x9f\x93\x9d",f.root);
    CHECK(rename(f.desk,renamed)==0);strcpy(f.desk,renamed);UmiNativeStageReport r;CHECK(Stage(&f,&r));CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);CHECK(RemoveTree(f.root));return 0;
}
int TestStageIconSnapshot(void)
{
    Fixture f;CHECK(Prepare(&f));char path[1024];(void)snprintf(path,sizeof path,"%s/source-icon.png",f.root);size_t n;const unsigned char *icon=UmiNativeBrandIcon(&n);CHECK(Write(path,icon,n,0600));
    f.config.iconPath=path;UmiNativeStageReport r;UmiNativeStagePlan *plan=NULL;CHECK(UmiNativeStagePlanCreate(&f.config,&plan,&r)==UMI_STATUS_OK);
    CHECK(Write(path,"changed",7U,0600));CHECK(UmiNativeStageCommit(plan,NULL,&r)==UMI_STATUS_OK);UmiNativeStagePlanDestroy(plan);
    CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);CHECK(RemoveTree(f.root));return 0;
}
int TestStageDigest(void)
{
    Fixture f;CHECK(Prepare(&f));char path[1024];(void)snprintf(path,sizeof path,"%s/digest.txt",f.root);CHECK(Write(path,"abc",3U,0600));
    char hash[65];uint64_t bytes;UmiNativeStageReport r;CHECK(UmiNativeStageDigestFile(path,hash,&bytes,&r)==UMI_STATUS_OK);
    CHECK(bytes==3U&&strcmp(hash,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")==0);CHECK(RemoveTree(f.root));return 0;
}
static size_t Descriptors(void){size_t n=0U;DIR*d=opendir("/proc/self/fd");if(d==NULL)return 0U;while(readdir(d)!=NULL)++n;closedir(d);return n;}
int TestStageDescriptorLifetime(void)
{
    Fixture f;CHECK(Prepare(&f));size_t before=Descriptors();CHECK(before>0U);UmiNativeStageReport r;
    for(unsigned i=0U;i<100U;++i){UmiNativeStagePlan *p=NULL;CHECK(UmiNativeStagePlanCreate(&f.config,&p,&r)==UMI_STATUS_OK);UmiNativeStagePlanDestroy(p);}
    CHECK(Stage(&f,&r));for(unsigned i=0U;i<50U;++i)CHECK(UmiNativeStageVerify(f.output,&r)==UMI_STATUS_OK);
    CHECK(Descriptors()==before);CHECK(RemoveTree(f.root));return 0;
}
