/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic image-header fixtures. Callback tests are not QEMU/GRUB execution. */

#include "test_support.h"
static int Images(const Fixture *f,char kernel[4096],char initrd[4096],int riscv)
{

    OK(ScJoin(f->root,riscv?"riscv-kernel":"x86-kernel",kernel));
    OK(ScJoin(f->root,riscv?"riscv-rootfs.gz":"x86-rootfs.gz",initrd));

    unsigned char data[1024]= {
        0
    };
    if(riscv) {
        memcpy(data+48,"RISCV\0\0\0",8);
        memcpy(data+56,"RSC\x05",4);
    }
    else {
        data[0x1fe]=0x55;
        data[0x1ff]=0xaa;
        memcpy(data+0x202,"HdrS",4);
        data[0x236]=1;
    }

    UmiSetupReport r= {
        0
    };
    OK(ScWriteNew(kernel,data,sizeof data,&r));
    const unsigned char gzip[10]= {
        0x1f,0x8b,8,0,0,0,0,0,0,0
    };
    OK(ScWriteNew(initrd,gzip,sizeof gzip,&r));
    return 0;

}

typedef struct Execution {
    int calls;
    int code;
    UmiStatus status;
    int sawNoNetwork;
    int sawSerial;
    int makeIso;
}
Execution;

static UmiStatus Execute(const char *program,const char *const *args,size_t count,const char *directory,void *opaque,int *exitCode)
{

    Execution *e=opaque;
    (void)program;
    (void)directory;
    ++e->calls;
    *exitCode=e->code;

    for(size_t i=0; i+1U<count; ++i) {
        if(!strcmp(args[i],"-nic")&&!strcmp(args[i+1U],"none"))e->sawNoNetwork=1;
        if(!strcmp(args[i],"-serial"))e->sawSerial=1;
    }

    if(e->makeIso&&count==3U) {
        unsigned char image[20U*2048U]= {
            0
        };
        unsigned char *p=image+16U*2048U;
        p[0]=1;
        memcpy(p+1,"CD001",5);
        p[6]=1;
        p[80]=20;
        p[128]=0;
        p[129]=8;

        p=image+17U*2048U;
        p[0]=0;
        memcpy(p+1,"CD001",5);
        p[6]=1;
        memcpy(p+7,"EL TORITO SPECIFICATION",23);
        p[71]=19;

        p=image+18U*2048U;
        p[0]=255;
        memcpy(p+1,"CD001",5);
        p[6]=1;

        p=image+19U*2048U;
        p[0]=1;
        p[28]=0xaa;
        p[29]=0x55;
        p[30]=0x55;
        p[31]=0xaa;
        p[32]=0x88;

        UmiSetupReport r= {
            0
        };
        return ScWriteNew(args[1],image,sizeof image,&r);
    }

    return e->status;

}

int TestVm(const char *name)
{

    Fixture f;
    CHECK(!FixtureCreate(&f));
    char kernel[4096],initrd[4096];
    int riscv=!strcmp(name,"vm.riscv");
    CHECK(!Images(&f,kernel,initrd,riscv));
    UmiSetupVmConfig c= {
        TestProgram,kernel,initrd,riscv?"riscv64":"x86_64",512,1,0,0
    };

    UmiSetupVmPlan *p=malloc(sizeof *p);
    CHECK(p);
    UmiSetupReport r= {
        0
    };
    Execution e= {
        0
    };

    if(!strcmp(name,"vm.limits")) {
        c.memoryMiB=255;
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_INVALID_ARGUMENT);
        c.memoryMiB=8193;
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_INVALID_ARGUMENT);
        c.memoryMiB=512;
        c.processors=9;
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_INVALID_ARGUMENT);
    }

    else if(!strcmp(name,"vm.bad_kernel")) {
        c.architecture="riscv64";
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_PARSE_ERROR);
    }

    else if(!strcmp(name,"vm.no_script")) {
        c.qemu=f.runtime;
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_PARSE_ERROR);
    }

    else if(!strcmp(name,"vm.comma")) {
        c.initrd="/tmp/one,two";
        CHECK(UmiSetupVmReview(&c,p,&r)==UMI_STATUS_INVALID_ARGUMENT);
    }

    else {
        if(!strcmp(name,"vm.interactive")) {
            c.interactive=1;
            c.recovery=1;
        }
        OK(UmiSetupVmReview(&c,p,&r));
        CHECK(e.calls==0&&ScHashValid(p->fingerprint));

        if(!strcmp(name,"vm.plan")||!strcmp(name,"vm.riscv")||!strcmp(name,"vm.interactive")) {
            int display=0,vc=0,rd=0;
            for(size_t i=0; i<p->argumentCount; ++i) {
                CHECK(strcmp(p->arguments[i],"-drive"));
                CHECK(strcmp(p->arguments[i],"-fsdev"));
                if(!strcmp(p->arguments[i],"-display"))display=1;
                if(!strcmp(p->arguments[i],"vc"))vc=1;
                if(strstr(p->arguments[i],"umicom.recovery=1"))rd=1;
            }
            CHECK(c.interactive?(vc&&rd&&!display):display);
        }

        else if(!strcmp(name,"vm.stale")) {
            FILE *file=fopen(initrd,"ab");
            CHECK(file);
            fputc(0,file);
            fclose(file);
            CHECK(UmiSetupVmRun(p,p->fingerprint,Execute,&e,&r)==UMI_STATUS_INVALID_STATE);
            CHECK(e.calls==0);
        }

        else if(!strcmp(name,"vm.wrong_review")) {
            char hash[65];
            strcpy(hash,p->fingerprint);
            hash[0]=hash[0]=='a'?'b':'a';
            CHECK(UmiSetupVmRun(p,hash,Execute,&e,&r)!=0);
            CHECK(e.calls==0);
        }

        else {
            if(!strcmp(name,"vm.failed_exit"))e.code=1;
            if(!strcmp(name,"vm.cancelled"))e.status=UMI_STATUS_CANCELLED;

            UmiStatus s=UmiSetupVmRun(p,p->fingerprint,Execute,&e,&r);
            CHECK(e.calls==1&&e.sawNoNetwork&&e.sawSerial);
            if(e.code)CHECK(s==UMI_STATUS_IO_ERROR&&!r.completed);
            else if(e.status)CHECK(s==e.status&&!r.completed);
            else CHECK(s==UMI_STATUS_OK&&r.completed);
        }

    }
    free(p);
    return 0;

}

int TestMedia(const char *name)
{

    Fixture f;
    CHECK(!FixtureCreate(&f));
    char kernel[4096],initrd[4096];
    CHECK(!Images(&f,kernel,initrd,0));
    UmiSetupReport r= {
        0
    };
    OK(UmiSetupMediaPrepare(kernel,initrd,f.destination,&r));
    CHECK(r.completed);

    if(!strcmp(name,"media.prepare")) {
        char path[4096],*text=NULL;
        size_t length=0;
        OK(ScJoin(f.destination,"tree/boot/grub/grub.cfg",path));
        OK(ScRead(path,4096,&text,&length,&r));
        CHECK(strstr(text,"umicom.recovery=1")&&strstr(text,"umicom.recovery=0"));
        free(text);
        OK(ScJoin(f.destination,"UmicomOS.iso",path));
        CHECK(ScFileRegular(path,0,&r)==UMI_STATUS_NOT_FOUND);
    }

    else if(!strcmp(name,"media.exists")) {
        CHECK(UmiSetupMediaPrepare(kernel,initrd,f.destination,&r)==UMI_STATUS_ALREADY_EXISTS);
    }

    else if(!strcmp(name,"media.bad_arch")) {
        char rv[4096],ri[4096];
        CHECK(!Images(&f,rv,ri,1));
        char out[4096];
        OK(ScJoin(f.root,"rvmedia",out));
        CHECK(UmiSetupMediaPrepare(rv,ri,out,&r)==UMI_STATUS_PARSE_ERROR);
    }

    else {

#ifndef __linux__
        return 77;

#else
        char output[4096];
        OK(ScJoin(f.root,"iso-output",output));
        Execution e= {
            0
        };

        if(!strcmp(name,"media.no_script")) {
            CHECK(UmiSetupMediaBuildIso(f.destination,f.runtime,output,Execute,&e,&r)==UMI_STATUS_PARSE_ERROR);
            CHECK(!e.calls && !r.completed);
        }
        else if(!strcmp(name,"media.tampered")) {
            char cfg[4096];
            OK(ScJoin(f.destination,"tree/boot/grub/grub.cfg",cfg));
            FILE *file=fopen(cfg,"ab");
            CHECK(file);
            fputs("malicious",file);
            fclose(file);
            CHECK(UmiSetupMediaBuildIso(f.destination,TestProgram,output,Execute,&e,&r)==UMI_STATUS_INVALID_STATE);
            CHECK(!e.calls);
        }

        else {
            if(!strcmp(name,"media.iso_structure"))e.makeIso=1;
            else if(!strcmp(name,"media.producer_failure"))e.code=1;

            UmiStatus s=UmiSetupMediaBuildIso(f.destination,TestProgram,output,Execute,&e,&r);
            CHECK(e.calls==1);
            if(e.makeIso) {
                CHECK(s==UMI_STATUS_OK&&r.completed);
                char sourceReceipt[4096],copiedReceipt[4096],sourceHash[65],copiedHash[65];
                uint64_t sourceBytes=0,copiedBytes=0;
                OK(ScJoin(f.destination,"media.umi",sourceReceipt));
                OK(ScJoin(output,"media.umi",copiedReceipt));
                OK(ScDigest(sourceReceipt,sourceHash,&sourceBytes,NULL,NULL,&r));
                OK(ScDigest(copiedReceipt,copiedHash,&copiedBytes,NULL,NULL,&r));
                CHECK(sourceBytes==copiedBytes&&!strcmp(sourceHash,copiedHash));
            }
            else CHECK(s!=UMI_STATUS_OK&&!r.completed);
        }

#endif
    }
    return 0;

}
