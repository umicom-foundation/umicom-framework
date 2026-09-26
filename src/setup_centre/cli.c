/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native CLI over the same reviewed contracts as the graphical setup tool. */

#include "internal.h"
#include <inttypes.h>
static void Help(void)
{

    puts("Umicom Setup Centre\n"
    "  pack --list ABSOLUTE --output NEW_ABSOLUTE [--bootstrap ABSOLUTE]\n"
    "  list --bundle ABSOLUTE\n"
    "  review --bundle ABSOLUTE --destination NEW_ABSOLUTE [--apps all|id,id]\n"
    "  install --bundle ABSOLUTE --destination NEW_ABSOLUTE --expect-plan SHA256 [--apps all|id,id]\n"
    "  verify --installation ABSOLUTE\n"
    "  vm-plan|vm-run --qemu ABSOLUTE --kernel ABSOLUTE --initrd ABSOLUTE\n"
    "    --arch x86_64|riscv64 [--memory 512] [--cpus 1] [--recovery yes|no]\n"
    "    [--interactive yes|no] [--expect-plan SHA256 (required for vm-run)]\n"
    "  media-stage --kernel ABSOLUTE --initrd ABSOLUTE --output NEW_ABSOLUTE\n"
    "  media-iso --prepared ABSOLUTE --grub ABSOLUTE --output NEW_ABSOLUTE\n"
    "No raw USB writing, optical burning, automatic downloading or scripts.\n"
    "vm-run/media-iso require the canonical process adapter in this build.");

}

typedef struct Options {
    const char *key[24];
    const char *value[24];
    size_t count;
}
Options;

static const char *Get(const Options *o,const char *key) {
    for(size_t i=0; i<o->count; ++i)if(!strcmp(o->key[i],key))return o->value[i];
    return NULL;
}

static int Allowed(const Options *o,const char *list)
{
    for(size_t i=0; i<o->count; ++i) {
        char pattern[100];
        (void)snprintf(pattern,sizeof pattern," %s ",o->key[i]);
        if(!strstr(list,pattern))return 0;
    }
    return 1;
}

static uint64_t Selection(const UmiSetupBundle *b,const char *text)
{
    if(!text||!strcmp(text,"all"))return UmiSetupAllApplications(b);
    if(strlen(text)>4095U)return 0;
    char data[4096];
    strcpy(data,text);
    char *p=data;
    uint64_t mask=0;

    while(p) {
        char *next=strchr(p,',');
        if(next)*next++=0;
        int found=0;
        for(size_t i=0; i<b->appCount; ++i)if(!strcmp(p,b->apps[i].id)) {
            uint64_t bit=UINT64_C(1)<<i;
            if(mask&bit)return 0;
            mask|=bit;
            found=1;
            break;
        }
        if(!found)return 0;
        p=next;
    }
    return mask;
}

int UmiSetupMainWithExecutor(int argc,char **argv,UmiSetupExecute execute,void *context)
{

    if(argc<1 || argv==NULL)return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL)return 2;
    if(argc==2&&!strcmp(argv[1],"--help")) {
        Help();
        return 0;
    }

    if(argc==2&&!strcmp(argv[1],"--self-test")) {
        char hash[65];
        UmiStatus s=UmiNativeSha256Buffer("abc",3,hash);
        if(s||strcmp(hash,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"))return 1;
        puts("Native setup self-test passed. No installation, process or network activity.");
        return 0;
    }

    if(argc<2||(argc%2)!=0) {
        Help();
        return 2;
    }
    Options o= {
        0
    };

    for(int i=2; i<argc; i+=2) {
        if(o.count==24U||strlen(argv[i])>=80U||strncmp(argv[i],"--",2)||Get(&o,argv[i])) {
            Help();
            return 2;
        }
        o.key[o.count]=argv[i];
        o.value[o.count++]=argv[i+1];
    }

    const char *command=argv[1];
    UmiSetupReport r= {
        0
    };
    UmiStatus s=UMI_STATUS_INVALID_ARGUMENT;

    if(!strcmp(command,"pack")&&Allowed(&o," --list  --output  --bootstrap "))s=UmiSetupPack(Get(&o,"--list"),Get(&o,"--output"),Get(&o,"--bootstrap"),NULL,NULL,&r);

    else if(!strcmp(command,"verify")&&Allowed(&o," --installation "))s=UmiSetupVerifyInstallation(Get(&o,"--installation"),&r);

    else if((!strcmp(command,"list")||!strcmp(command,"review")||!strcmp(command,"install"))&&Allowed(&o," --bundle  --destination  --apps  --expect-plan ")) {

        UmiSetupBundle *b=NULL;
        s=UmiSetupBundleOpen(Get(&o,"--bundle"),&b,&r);

        if(s==UMI_STATUS_OK) {
            uint64_t selection=Selection(b,Get(&o,"--apps"));
            char hash[65];

            if(!strcmp(command,"list")) {
                for(size_t i=0; i<b->appCount; ++i)printf("%s\t%s\t%s\n",b->apps[i].id,b->apps[i].title,b->apps[i].entry);
            }

            else if(!strcmp(command,"review")) {
                s=UmiSetupReview(b,selection,Get(&o,"--destination"),hash,NULL,NULL,&r);
                if(s==UMI_STATUS_OK)printf("Plan: %s\nBytes: %" PRIu64 "\n",hash,r.bytesPlanned);
            }

            else s=UmiSetupInstall(b,selection,Get(&o,"--destination"),Get(&o,"--expect-plan"),NULL,NULL,&r);
        }

        UmiSetupBundleDestroy(b);

    }
    else if((!strcmp(command,"vm-plan")||!strcmp(command,"vm-run"))&&Allowed(&o," --qemu  --kernel  --initrd  --arch  --memory  --cpus  --recovery  --interactive  --expect-plan ")) {

        uint64_t memory=512,cpus=1;
        const char *value=Get(&o,"--memory");
        int valid=!value||ScNumber(value,&memory);
        value=Get(&o,"--cpus");
        valid=valid&&(!value||ScNumber(value,&cpus));

        const char *recovery=Get(&o,"--recovery"),*interactive=Get(&o,"--interactive");

        valid=valid&&(!recovery||!strcmp(recovery,"yes")||!strcmp(recovery,"no"))&&(!interactive||!strcmp(interactive,"yes")||!strcmp(interactive,"no"));

        if(valid&&memory<=8192U&&cpus<=8U) {
            UmiSetupVmConfig c= {
                Get(&o,"--qemu"),Get(&o,"--kernel"),Get(&o,"--initrd"),Get(&o,"--arch"),(unsigned)memory,(unsigned)cpus,recovery&&!strcmp(recovery,"yes"),interactive&&!strcmp(interactive,"yes")
            };

            UmiSetupVmPlan *p=malloc(sizeof *p);
            if(!p)s=UMI_STATUS_OUT_OF_MEMORY;
            else {
                s=UmiSetupVmReview(&c,p,&r);
                if(s==UMI_STATUS_OK) {
                    if(!strcmp(command,"vm-plan")) {
                        printf("Plan: %s\nProgram: %s\n",p->fingerprint,p->program);
                        for(size_t i=0; i<p->argumentCount; ++i)printf("Argument %zu: %s\n",i,p->arguments[i]);
                    }

                    else if(!execute) {
                        s=UMI_STATUS_UNAVAILABLE;
                        ScReport(&r,s,"This build has no process adapter. Build with the canonical Framework platform.");
                    }

                    else s=UmiSetupVmRun(p,Get(&o,"--expect-plan"),execute,context,&r);
                }
                free(p);
            }
        }

    }
    else if(!strcmp(command,"media-stage")&&Allowed(&o," --kernel  --initrd  --output "))s=UmiSetupMediaPrepare(Get(&o,"--kernel"),Get(&o,"--initrd"),Get(&o,"--output"),&r);

    else if(!strcmp(command,"media-iso")&&Allowed(&o," --prepared  --grub  --output "))s=execute?UmiSetupMediaBuildIso(Get(&o,"--prepared"),Get(&o,"--grub"),Get(&o,"--output"),execute,context,&r):UMI_STATUS_UNAVAILABLE;

    if(s!=UMI_STATUS_OK) {
        fprintf(stderr,"Not completed: %s. %s\n",UmiSetupStatusText(s),r.detail);
        return s==UMI_STATUS_UNAVAILABLE?77:1;
    }

    puts(r.detail);
    return 0;

}

int UmiSetupMain(int argc,char **argv) {
    return UmiSetupMainWithExecutor(argc,argv,NULL,NULL);
}
