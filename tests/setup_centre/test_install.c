/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "test_support.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#endif
static int AlwaysCancel(const UmiSetupReport *r,void *context) {
    (void)r;
    (void)context;
    return 1;
}

static int CancelCopy(const UmiSetupReport *r,void *context) {
    (void)context;
    return strstr(r->detail,"Installing")&&r->bytesCompleted>=65536U;
}

static int Rewrite(const char *path,const char *text) {
    FILE *f=fopen(path,"wb");
    if(!f)return 0;
    int ok=fwrite(text,1,strlen(text),f)==strlen(text);
    return fclose(f)==0&&ok;
}

int TestInstall(const char *name)
{

    Fixture f;
    CHECK(!FixtureCreate(&f));
    UmiSetupReport r= {
        0
    };
    UmiSetupBundle *b=NULL;
    char fingerprint[65],path[4096];

    if(!strcmp(name,"pack.bad_source")) {
        CHECK(Rewrite(f.bank,"changed"));
        CHECK(UmiSetupPack(f.list,f.bundle,NULL,NULL,NULL,&r)!=0);
        CHECK(!r.outputCreated);
        return 0;
    }

    if(!strcmp(name,"pack.cancel")) {
        CHECK(UmiSetupPack(f.list,f.bundle,NULL,AlwaysCancel,NULL,&r)==UMI_STATUS_CANCELLED);
        CHECK(!r.outputCreated);
        return 0;
    }

    if(!strcmp(name,"pack.no_script_evaluation")) {
        char report[4096];
        OK(ScJoin(f.root,"bank.runtime.cmake",report));
        CHECK(Rewrite(report,"execute_process(COMMAND anything)\n"));
        CHECK(UmiSetupPack(f.list,f.bundle,NULL,NULL,NULL,&r)==UMI_STATUS_PARSE_ERROR);
        return 0;
    }

    if(!strcmp(name,"pack.blank_line")) {
        FILE *file=fopen(f.list,"ab");
        CHECK(file);
        fputs("\nexecute\tbad\n",file);
        fclose(file);
        CHECK(UmiSetupPack(f.list,f.bundle,NULL,NULL,NULL,&r)==UMI_STATUS_PARSE_ERROR);
        return 0;
    }

    OK(UmiSetupPack(f.list,f.bundle,NULL,NULL,NULL,&r));
    CHECK(r.completed);
    OK(UmiSetupBundleOpen(f.bundle,&b,&r));
    CHECK(UmiSetupApplicationCount(b)==2U&&b->fileCount==3U);

    if(!strcmp(name,"pack.deduplicate")) {
        CHECK(!strcmp(b->files[1].relative,"bin/runtime.dll"));
    }

    else if(!strcmp(name,"pack.destination_exists")) {
        CHECK(UmiSetupPack(f.list,f.bundle,NULL,NULL,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
    }

    else if(!strcmp(name,"review.no_writes")) {
        OK(UmiSetupReview(b,3,f.destination,fingerprint,NULL,NULL,&r));
        CHECK(r.bytesPlanned==200029U);
        CHECK(ScFileRegular(f.destination,1,&r)==UMI_STATUS_NOT_FOUND);
    }

    else if(!strcmp(name,"review.empty_selection")) {
        CHECK(UmiSetupReview(b,0,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
    }

    else if(!strcmp(name,"review.unknown_selection")) {
        CHECK(UmiSetupReview(b,4,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
    }

    else if(!strcmp(name,"review.cancel")) {
        CHECK(UmiSetupReview(b,3,f.destination,fingerprint,AlwaysCancel,NULL,&r)==UMI_STATUS_CANCELLED);
        CHECK(!r.outputCreated);
    }

    else if(!strcmp(name,"review.changed_catalogue")) {
        OK(ScJoin(f.bundle,UMI_SETUP_CATALOGUE,path));
        FILE *file=fopen(path,"ab");
        CHECK(file);
        fputc('\n',file);
        fclose(file);
        CHECK(UmiSetupReview(b,3,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_STATE);
    }

    else if(!strcmp(name,"review.changed_payload")) {
        OK(ScJoin(f.bundle,"payload/bin/studio.exe",path));
        CHECK(Rewrite(path,"changed"));
        CHECK(UmiSetupReview(b,3,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_STATE);
    }

    else if(!strcmp(name,"review.unselected_missing")) {
        OK(ScJoin(f.bundle,"payload/bin/studio.exe",path));
        CHECK(remove(path)==0);
        OK(UmiSetupReview(b,1,f.destination,fingerprint,NULL,NULL,&r));
    }

    else {

        uint64_t selected=!strcmp(name,"install.subset")?1U:3U;
        OK(UmiSetupReview(b,selected,f.destination,fingerprint,NULL,NULL,&r));

        if(!strcmp(name,"install.plan_mismatch")) {
            fingerprint[0]=fingerprint[0]=='a'?'b':'a';
            CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_STATE);
            CHECK(!r.outputCreated);
        }

        else if(!strcmp(name,"install.destination_changed")) {
            char other[4096];
            OK(ScJoin(f.root,"other",other));
            CHECK(UmiSetupInstall(b,selected,other,fingerprint,NULL,NULL,&r)==UMI_STATUS_INVALID_STATE);
        }

        else if(!strcmp(name,"install.exists")) {
            OK(ScMakeDirectory(f.destination,&r));
            CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
        }

        else if(!strcmp(name,"install.cancel_copy")) {
            CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,CancelCopy,NULL,&r)==UMI_STATUS_CANCELLED);
            CHECK(r.outputCreated&&!r.completed);
            OK(ScJoin(f.destination,UMI_SETUP_RECEIPT,path));
            CHECK(ScFileRegular(path,0,&r)==UMI_STATUS_NOT_FOUND);
        }

        else if(!strcmp(name,"install.cancel_before")) {
            CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,AlwaysCancel,NULL,&r)==UMI_STATUS_CANCELLED);
            CHECK(!r.outputCreated);
        }

        else if(!strcmp(name,"install.symlink")) {

#ifdef _WIN32
            UmiSetupBundleDestroy(b);
            return 77;

#else
            CHECK(symlink(f.root,f.destination)==0);
            CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);

#endif
        }

        else if(!strcmp(name,"install.parent_symlink")) {

#ifdef _WIN32
            UmiSetupBundleDestroy(b);
            return 77;

#else
            char symlinkPath[4096],dest[4096];
            OK(ScJoin(f.root,"link",symlinkPath));
            CHECK(symlink(f.root,symlinkPath)==0);
            OK(ScJoin(symlinkPath,"new",dest));
            OK(UmiSetupReview(b,selected,dest,fingerprint,NULL,NULL,&r));
            CHECK(UmiSetupInstall(b,selected,dest,fingerprint,NULL,NULL,&r)==UMI_STATUS_PERMISSION_DENIED);

#endif
        }

        else {
            OK(UmiSetupInstall(b,selected,f.destination,fingerprint,NULL,NULL,&r));
            CHECK(r.completed);
            OK(UmiSetupVerifyInstallation(f.destination,&r));

            if(!strcmp(name,"install.subset")) {
                OK(ScJoin(f.destination,"bin/studio.exe",path));
                CHECK(ScFileRegular(path,0,&r)==UMI_STATUS_NOT_FOUND);
            }

            else if(!strcmp(name,"verify.tamper")) {
                OK(ScJoin(f.destination,"bin/studio.exe",path));
                CHECK(Rewrite(path,"changed"));
                CHECK(UmiSetupVerifyInstallation(f.destination,&r)==UMI_STATUS_INVALID_STATE);
            }

            else if(!strcmp(name,"verify.missing")) {
                OK(ScJoin(f.destination,"bin/studio.exe",path));
                CHECK(remove(path)==0);
                CHECK(UmiSetupVerifyInstallation(f.destination,&r)==UMI_STATUS_NOT_FOUND);
            }

            else if(!strcmp(name,"verify.user_file")) {
                OK(ScJoin(f.destination,"personal-notes.txt",path));
                OK(ScWriteNew(path,"keep",4,&r));
                OK(UmiSetupVerifyInstallation(f.destination,&r));
            }

            else if(!strcmp(name,"install.repeat")) {
                CHECK(UmiSetupInstall(b,selected,f.destination,fingerprint,NULL,NULL,&r)==UMI_STATUS_ALREADY_EXISTS);
            }

            else if(strcmp(name,"install.all")) {
                UmiSetupBundleDestroy(b);
                return 2;
            }
        }

    }

    UmiSetupBundleDestroy(b);
    return 0;

}
