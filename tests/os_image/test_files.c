/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_image/test_files.c
 * PURPOSE:
 *   Linux filesystem and controlled tool-transport cases. Test directories are private and
 *   contain no user data. The child is explicitly inert, not QEMU.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linux filesystem and controlled tool-transport cases. Test directories are
 * private and contain no user data. The child is explicitly inert, not QEMU. */
#define _GNU_SOURCE
#include "fixtures.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
static int Put(const char *root,const char *name,const void *data,size_t size) {
    CHECK(OiParents(root,name)==UMI_STATUS_OK);
    char path[UMI_OS_IMAGE_PATH];
    CHECK(OiJoin(root,name,path)==UMI_STATUS_OK);
    CHECK(OiWrite(path,data,size)==UMI_STATUS_OK);
    return 0;
}
static int Overwrite(const char *root,const char *name,const char *text) {
    char path[UMI_OS_IMAGE_PATH];
    CHECK(OiJoin(root,name,path)==UMI_STATUS_OK);
    FILE *f=fopen(path,"wb");
    CHECK(f);
    CHECK(fwrite(text,1,strlen(text),f)==strlen(text));
    CHECK(!fclose(f));
    return 0;
}
static int Tool(const char *program,const char *root,const char *const *args,size_t count,OiText *out) {
    UmiProcessResult *r=calloc(1,sizeof *r);
    CHECK(r);
    UmiStatus s=OiCapture(program,args,count,root,10000,NULL,out,r);
    if(s!=UMI_STATUS_OK)fprintf(stderr,"tool: %s\n",r->output);
    free(r);
    CHECK(s==UMI_STATUS_OK);
    return 0;
}
static int Fixture(const char *root,const char *git,char os[UMI_OS_IMAGE_PATH],char fw[UMI_OS_IMAGE_PATH],char br[UMI_OS_IMAGE_PATH]) {
    CHECK(OiJoin(root,"umicomOS",os)==UMI_STATUS_OK);
    CHECK(OiJoin(root,"framework",fw)==UMI_STATUS_OK);
    CHECK(OiJoin(root,"buildroot-fixture",br)==UMI_STATUS_OK);
    CHECK(OiDirectory(os,1)==UMI_STATUS_OK);
    CHECK(OiDirectory(fw,1)==UMI_STATUS_OK);
    CHECK(OiDirectory(br,1)==UMI_STATUS_OK);
    CHECK(!Put(br,"README","Inert native pipeline fixture. Not Buildroot.\n",sizeof "Inert native pipeline fixture. Not Buildroot.\n"-1U));
    const char *init[]= {
        "init","-q",br
    }
    ;
    OiText t;
    OiTextInit(&t);
    CHECK(!Tool(git,root,init,3,&t));
    OiTextClear(&t);
    const char *add[]= {
        "-C",br,"add","README"
    }
    ;
    CHECK(!Tool(git,root,add,4,&t));
    OiTextClear(&t);
    const char *commit[]= {
        "-C",br,"-c","user.name=Umicom Fixture","-c","user.email=fixture@example.invalid","-c","commit.gpgsign=false","commit","-qm","inert fixture"
    }
    ;
    CHECK(!Tool(git,root,commit,11,&t));
    OiTextClear(&t);
    const char *head[]= {
        "-C",br,"rev-parse","HEAD"
    }
    ;
    CHECK(!Tool(git,root,head,4,&t));
    CHECK(t.size==41U);
    t.data[40]=0;
    char id[41];
    memcpy(id,t.data,41);
    OiTextClear(&t);
    CHECK(!Put(fw,"notice","Framework fixture\n",18));
    const char *overlay[]= {
        "etc/group","etc/os-release","etc/passwd","etc/shadow","etc/umicom/boot.conf"
    }
    ;
    for(size_t i=0;i<5;++i) {
        char rel[UMI_OS_IMAGE_NAME];
        int n=snprintf(rel,sizeof rel,"rootfs/foundation/%s",overlay[i]);
        CHECK(n>0&&(size_t)n<sizeof rel);
        CHECK(!Put(os,rel,"test configuration\n",19));
    }
    char archiveHash[65];
    CHECK(UmiNativeSha256Buffer(TEST_ARCHIVE_TEXT,sizeof TEST_ARCHIVE_TEXT-1U,archiveHash)==UMI_STATUS_OK);
    OiPrint(&t,"UMICOM_OS_PROFILE\t1\nbuildroot\t%s\nlinux\t6.12.109\nlinux-sha256\t%s\nfile\tframework/notice\nfile\tumicomOS/image/native/profile.umi\n",id,archiveHash);
    for(size_t i=0;i<5;++i)OiPrint(&t,"file\tumicomOS/rootfs/foundation/%s\n",overlay[i]);
    CHECK(!Put(os,"image/native/profile.umi",t.data,t.size));
    OiTextClear(&t);
    return 0;
}
static int Pipeline(const char *name,const char *root,const char *child,const char *git) {
    if(!git[0])return 77;
    if(!OiNormalLinuxUser()) {
        fputs("Run native build-pipeline tests as a normal Linux account.\n",stderr);
        return 77;
    }
    char os[UMI_OS_IMAGE_PATH],fw[UMI_OS_IMAGE_PATH],br[UMI_OS_IMAGE_PATH],prepared[UMI_OS_IMAGE_PATH],bundle[UMI_OS_IMAGE_PATH];
    CHECK(!Fixture(root,git,os,fw,br));
    CHECK(OiJoin(root,"prepared",prepared)==UMI_STATUS_OK);
    CHECK(OiJoin(root,"bundle",bundle)==UMI_STATUS_OK);
    UmiOsImageReport r;
    UmiOsImageArch arch=!strcmp(name,"pipeline_riscv")?UMI_OS_IMAGE_RISCV64:UMI_OS_IMAGE_X86_64;
    if(!strcmp(name,"pipeline_dirty_buildroot")) {
        CHECK(!Put(br,"untracked","x",1));
        CHECK(UmiOsImagePrepare(os,fw,br,prepared,arch,git,&r)==UMI_STATUS_INVALID_STATE);
        CHECK(!r.outputCreated);
        return 0;
    }
    if(!strcmp(name,"pipeline_overlap")) {
        CHECK(UmiOsImagePrepare(os,fw,br,fw,arch,git,&r)!=UMI_STATUS_OK);
        return 0;
    }
    CHECK(UmiOsImagePrepare(os,fw,br,prepared,arch,git,&r)==UMI_STATUS_OK);
    CHECK(r.completed&&!r.processLaunched);
    if(!strcmp(name,"pipeline_existing_prepare")) {
        CHECK(UmiOsImagePrepare(os,fw,br,prepared,arch,git,&r)==UMI_STATUS_ALREADY_EXISTS);
        return 0;
    }
    if(!strcmp(name,"pipeline_changed_input")) {
        CHECK(!Overwrite(prepared,"inputs/framework/notice","changed\n"));
        CHECK(UmiOsImageConfigure(prepared,child,NULL,&r)==UMI_STATUS_INVALID_STATE);
        CHECK(!r.processLaunched);
        return 0;
    }
    if(!strcmp(name,"pipeline_lock")) {
        void *lock=NULL;
        CHECK(OiLock(prepared,&lock)==UMI_STATUS_OK);
        CHECK(UmiOsImageConfigure(prepared,child,NULL,&r)==UMI_STATUS_BUSY);
        OiUnlock(lock);
        return 0;
    }
    if(!strcmp(name,"pipeline_pack_before_build")) {
        CHECK(UmiOsImagePack(prepared,bundle,&r)!=UMI_STATUS_OK);
        CHECK(!r.outputCreated);
        return 0;
    }
    UmiStatus status=UmiOsImageConfigure(prepared,child,NULL,&r);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"configure: %d %s\n",status,r.detail);
    CHECK(status==UMI_STATUS_OK);
    if(!strcmp(name,"pipeline_changed_config")) {
        CHECK(!Overwrite(prepared,"build/.config","BR2_INIT_NONE=y\n"));
        CHECK(UmiOsImageBuild(prepared,child,2,NULL,&r)==UMI_STATUS_INVALID_STATE);
        CHECK(!r.processLaunched);
        return 0;
    }
    status=UmiOsImageBuild(prepared,child,2,NULL,&r);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"build: %d %s\n",status,r.detail);
    CHECK(status==UMI_STATUS_OK);
    if(!strcmp(name,"pipeline_repeated_build")) {
        CHECK(UmiOsImageBuild(prepared,child,2,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(!r.processLaunched);
        return 0;
    }
    if(!strcmp(name,"pipeline_changed_artifact")) {
        CHECK(!Overwrite(prepared,"build/target/init","broken\n"));
        CHECK(UmiOsImagePack(prepared,bundle,&r)!=UMI_STATUS_OK);
        CHECK(!r.outputCreated);
        return 0;
    }
    if(!strcmp(name,"pipeline_changed_kernel_archive")) {
        CHECK(!Overwrite(prepared,"downloads/linux/linux-6.12.109.tar.xz","changed source archive\n"));
        CHECK(UmiOsImagePack(prepared,bundle,&r)!=UMI_STATUS_OK);
        CHECK(!r.outputCreated);
        return 0;
    }
    if(!strcmp(name,"pipeline_configuration_duplicate")) {
        char path[UMI_OS_IMAGE_PATH];
        CHECK(OiJoin(prepared,"build/.config",path)==UMI_STATUS_OK);
        FILE *file=fopen(path,"ab");
        CHECK(file);
        CHECK(fputs("BR2_INIT_NONE=y\n",file)>=0);
        CHECK(!fclose(file));
        CHECK(UmiOsImagePack(prepared,bundle,&r)!=UMI_STATUS_OK);
        CHECK(!r.outputCreated);
        return 0;
    }
    status=UmiOsImagePack(prepared,bundle,&r);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"pack: %d %s\n",status,r.detail);
    CHECK(status==UMI_STATUS_OK);
    CHECK(r.files==4U);
    CHECK(UmiOsImageVerify(bundle,&r)==UMI_STATUS_OK);
    CHECK(!r.processLaunched);
    if(!strcmp(name,"pipeline_extra_bundle_file")) {
        CHECK(!Put(bundle,"extra","x",1));
        CHECK(UmiOsImageVerify(bundle,&r)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"pipeline_changed_archive")) {
        CHECK(!Overwrite(bundle,"umicom-rootfs.cpio.gz","broken"));
        CHECK(UmiOsImageVerify(bundle,&r)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"pipeline_existing_pack")) {
        CHECK(UmiOsImagePack(prepared,bundle,&r)==UMI_STATUS_ALREADY_EXISTS);
        return 0;
    }
    if(!strcmp(name,"pipeline_legal_info")) {
        CHECK(UmiOsImageLegalInfo(prepared,child,NULL,&r)==UMI_STATUS_OK);
        return 0;
    }
    char result[UMI_OS_IMAGE_PATH];
    CHECK(OiJoin(root,"result",result)==UMI_STATUS_OK);
    if(!strcmp(name,"pipeline_missing_qemu")) {
        CHECK(UmiOsImageBoot(bundle,"/usr/bin/umicom-no-such-qemu",UMI_OS_IMAGE_NORMAL,5,result,NULL,&r)==UMI_STATUS_UNAVAILABLE);
        CHECK(!r.processLaunched&&!r.completed);
        return 0;
    }
    if(!strcmp(name,"pipeline_boot_precancel")) {
        UmiCancellationToken *stop=NULL;
        CHECK(umi_cancellation_token_create(&stop)==UMI_STATUS_OK);
        umi_cancellation_token_request(stop);
        CHECK(UmiOsImageBoot(bundle,child,UMI_OS_IMAGE_NORMAL,5,result,stop,&r)==UMI_STATUS_CANCELLED);
        CHECK(!r.outputCreated&&!r.processLaunched);
        umi_cancellation_token_destroy(stop);
        return 0;
    }
    /* Exercise transport, not a guest. The path explicitly selects the inert C
     * child; the test name and evidence never describe this as a QEMU boot. */
    status=UmiOsImageBoot(bundle,child,!strcmp(name,"pipeline_inert_recovery_transport")?UMI_OS_IMAGE_RECOVERY:UMI_OS_IMAGE_NORMAL,5,result,NULL,&r);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"inert transport: %d %s\n",status,r.detail);
    CHECK(status==UMI_STATUS_OK);
    CHECK(r.processLaunched&&r.completed);
    CHECK(UmiOsImageBoot(bundle,child,UMI_OS_IMAGE_NORMAL,5,result,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
static void *StopSoon(void *token) {
    struct timespec t= {
        0,50000000L
    }
    ;
    (void)nanosleep(&t,NULL);
    umi_cancellation_token_request(token);
    return NULL;
}
static int Native(const char *name,const char *root,const char *child,const char *gzip,const char *cpio) {
    char path[UMI_OS_IMAGE_PATH],other[UMI_OS_IMAGE_PATH];
    CHECK(OiJoin(root,"file",path)==UMI_STATUS_OK);
    CHECK(OiJoin(root,"other",other)==UMI_STATUS_OK);
    if(!strcmp(name,"io_exclusive")) {
        CHECK(OiWrite(path,"old",3)==UMI_STATUS_OK);
        CHECK(OiWrite(path,"new",3)==UMI_STATUS_ALREADY_EXISTS);
        unsigned char *d=NULL;
        size_t n=0;
        CHECK(OiRead(path,20,&d,&n)==UMI_STATUS_OK);
        CHECK(n==3&&!memcmp(d,"old",3));
        free(d);
        return 0;
    }
    if(!strcmp(name,"io_symlink")) {
        CHECK(OiWrite(other,"old",3)==UMI_STATUS_OK);
        CHECK(!symlink(other,path));
        unsigned char *d=NULL;
        size_t n=0;
        CHECK(OiRead(path,20,&d,&n)!=UMI_STATUS_OK);
        CHECK(!d);
        return 0;
    }
    if(!strcmp(name,"io_symlink_parent")) {
        CHECK(!symlink(root,path));
        char nested[UMI_OS_IMAGE_PATH];
        CHECK(OiJoin(path,"file",nested)==UMI_STATUS_OK);
        CHECK(OiWrite(nested,"x",1)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"io_fifo")) {
        CHECK(!mkfifo(path,0600));
        unsigned char *d=NULL;
        size_t n=0;
        CHECK(OiRead(path,20,&d,&n)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(name,"io_size_limit")) {
        CHECK(OiWrite(path,"abc",3)==UMI_STATUS_OK);
        unsigned char *d=NULL;
        size_t n=0;
        CHECK(OiRead(path,2,&d,&n)==UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    if(!strcmp(name,"io_lock")) {
        void *a=NULL,*b=NULL;
        CHECK(OiLock(root,&a)==UMI_STATUS_OK);
        CHECK(OiLock(root,&b)==UMI_STATUS_BUSY);
        OiUnlock(a);
        CHECK(OiLock(root,&b)==UMI_STATUS_OK);
        OiUnlock(b);
        return 0;
    }
    if(!strcmp(name,"archive_external_tools")) {
        if(!gzip[0]||!cpio[0])return 77;
        UmiOsImageEntry e[]= {
            {
                "etc",OI_DIR|0755U,0,0,NULL,0
            }
            , {
                "etc/notice",OI_REG|0444U,0,0,(const unsigned char*)"Umicom",6
            }
        }
        ;
        unsigned char *d=NULL;
        size_t n=0;
        CHECK(UmiOsImageArchiveBuild(e,2,&d,&n)==UMI_STATUS_OK);
        CHECK(OiWrite(path,d,n)==UMI_STATUS_OK);
        free(d);
        OiText t;
        OiTextInit(&t);
        const char *g[]= {
            "-cd",path
        }
        ;
        CHECK(!Tool(gzip,root,g,2,&t));
        CHECK(OiWrite(other,t.data,t.size)==UMI_STATUS_OK);
        OiTextClear(&t);
        const char *c[]= {
            "-it","--quiet","-F",other
        }
        ;
        CHECK(!Tool(cpio,root,c,4,&t));
        CHECK(t.size==15U&&!memcmp(t.data,"etc\netc/notice\n",15));
        OiTextClear(&t);
        return 0;
    }
    OiText t;
    OiTextInit(&t);
    UmiProcessResult *r=calloc(1,sizeof *r);
    CHECK(r);
    UmiCancellationToken *cancel=NULL;
    CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
    const char *arg=!strcmp(name,"process_timeout")||!strcmp(name,"process_cancel")?"--wait":!strcmp(name,"process_failure")?"--fail":!strcmp(name,"process_output_cap")?"--large":"space separated argument";
    if(!strcmp(name,"process_precancel"))umi_cancellation_token_request(cancel);
    pthread_t thread;
    if(!strcmp(name,"process_cancel"))CHECK(!pthread_create(&thread,NULL,StopSoon,cancel));
    UmiStatus s=OiCapture(child,&arg,1,root,!strcmp(name,"process_timeout")?50U:3000U,cancel,&t,r);
    if(!strcmp(name,"process_cancel"))CHECK(!pthread_join(thread,NULL));
    if(!strcmp(name,"process_timeout"))CHECK(s==UMI_STATUS_TIMEOUT&&r->timed_out&&r->launched);
    else if(!strcmp(name,"process_cancel"))CHECK(s==UMI_STATUS_CANCELLED&&r->cancelled&&r->launched);
    else if(!strcmp(name,"process_precancel"))CHECK(s==UMI_STATUS_CANCELLED&&!r->launched);
    else if(!strcmp(name,"process_failure"))CHECK(s!=UMI_STATUS_OK&&r->exit_code==17);
    else if(!strcmp(name,"process_output_cap"))CHECK(s==UMI_STATUS_CAPACITY_EXCEEDED&&t.size<=UMI_OS_IMAGE_MAX_LOG);
    else CHECK(s==UMI_STATUS_OK&&t.size==28U&&!memcmp(t.data,"24:space separated argument\n",28));
    umi_cancellation_token_destroy(cancel);
    free(r);
    OiTextClear(&t);
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==6);
    char root[]="/tmp/umicom-os-image-test-XXXXXX";
    CHECK(mkdtemp(root));
    if(!strncmp(argv[1],"pipeline_",9))return Pipeline(argv[1],root,argv[2],argv[3]);
    return Native(argv[1],root,argv[2],argv[4],argv[5]);
}
