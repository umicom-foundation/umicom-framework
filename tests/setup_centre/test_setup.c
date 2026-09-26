/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Fixture executables, gzip and kernel headers are synthetic data unless a
 * test explicitly names a real host executable. No test claims a guest boot. */

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "test_support.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
static int SourceReport(const Fixture *f,const char *id,const char *title,const char *exe)
{

    char eh[65],rh[65],report[4096];
    uint64_t size;
    UmiSetupReport r= {
        0
    };
    OK(ScDigest(exe,eh,&size,NULL,NULL,&r));
    OK(ScDigest(f->runtime,rh,&size,NULL,NULL,&r));

    char relative[100];
    (void)snprintf(relative,sizeof relative,"%s.runtime.cmake",id);
    OK(ScJoin(f->root,relative,report));

    ScText text;
    ScTextInit(&text);
    ScPrint(&text,"set(UMI_REPORT_EXECUTABLE [==[%s]==])\nset(UMI_REPORT_EXECUTABLE_SHA256 [==[%s]==])\nset(UMI_REPORT_TARGET [==[%s]==])\nset(UMI_REPORT_PRODUCT [==[%s]==])\nset(UMI_REPORT_FILES [==[bin/runtime.dll|%s|%s]==])\n",exe,eh,id,title,f->runtime,rh);

    OK(text.status);
    OK(ScWriteNew(report,text.data,text.size,&r));
    ScTextFree(&text);
    return 0;

}

int FixtureCreate(Fixture *f)
{

    memset(f,0,sizeof *f);

#ifdef _WIN32
    wchar_t temp[2048];
    DWORD n=GetTempPathW(2048,temp);
    CHECK(n>0&&n<2048);
    char base[4096];
    CHECK(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,temp,-1,base,sizeof base,NULL,NULL)>0);
    size_t length=strlen(base);
    while(length&& (base[length-1U]=='/'||base[length-1U]=='\\'))base[--length]=0;

    (void)snprintf(f->root,sizeof f->root,"%s/umicom-setup-%lu-%llu",base,(unsigned long)GetCurrentProcessId(),(unsigned long long)GetTickCount64());
    UmiSetupReport initial= {
        0
    };
    OK(ScMakeDirectory(f->root,&initial));

#else
    strcpy(f->root,"/tmp/umicom-setup-XXXXXX");
    CHECK(mkdtemp(f->root)!=NULL);

#endif
    OK(ScJoin(f->root,"bundle",f->bundle));
    OK(ScJoin(f->root,"installed",f->destination));
    OK(ScJoin(f->root,"bank.exe",f->bank));
    OK(ScJoin(f->root,"studio.exe",f->studio));
    OK(ScJoin(f->root,"runtime.dll",f->runtime));
    OK(ScJoin(f->root,"list.tsv",f->list));

    UmiSetupReport r= {
        0
    };
    unsigned char *payload=malloc(200000U);
    CHECK(payload);
    memset(payload,0x45,200000U);
    OK(ScWriteNew(f->bank,payload,200000U,&r));
    free(payload);

    OK(ScWriteNew(f->studio,"studio fixture",14,&r));
    OK(ScWriteNew(f->runtime,"runtime fixture",15,&r));
    CHECK(!SourceReport(f,"bank","Umicom Bank",f->bank));
    CHECK(!SourceReport(f,"studio","Umicom Studio",f->studio));

    ScText list;
    ScTextInit(&list);
    ScPrint(&list,"UMICOM_SETUP_INPUT\t1\napp\tbank\tUmicom Bank\t%s/bank.runtime.cmake\napp\tstudio\tUmicom Studio\t%s/studio.runtime.cmake\n",f->root,f->root);
    OK(ScWriteNew(f->list,list.data,list.size,&r));
    ScTextFree(&list);
    return 0;

}

int TestPaths(const char *name)
{

    if(!strcmp(name,"paths.valid")) {
        const char *paths[]= {
            "bin/umicom-desk.exe","share/Umicom guide.html","share/caf\xc3\xa9.txt","a/b/c","bin/runtime.dll"
        };
        for(size_t i=0; i<5; ++i)OK(UmiSetupValidateRelative(paths[i]));
    }

    else if(!strcmp(name,"paths.unsafe")) {
        const char *paths[]= {
            "../x","a/../b","a//b","/bin/x","a\\b","C:/x","x.","x ","nul","con.txt","LPT9.bin","foo:bar","a\nb","a\tb","a/",".","a/./b","aux.txt","COM1/foo","?","*"
        };
        for(size_t i=0; i<sizeof paths/sizeof paths[0]; ++i)CHECK(UmiSetupValidateRelative(paths[i])!=UMI_STATUS_OK);
    }

    else if(!strcmp(name,"paths.unicode")) {
        const char *bad[]= {
            "\xc0\xaf","\xe0\x80\xaf","\xed\xa0\x80","\xf4\x90\x80\x80","\xe2\x82","\x80"
        };
        for(size_t i=0; i<6; ++i)CHECK(!ScUtf8(bad[i]));
        CHECK(ScUtf8("Umicom \xe2\x82\xac"));
    }

    else if(!strcmp(name,"paths.absolute")) {
        OK(UmiSetupValidateAbsolute("/tmp/umicom"));
        OK(UmiSetupValidateAbsolute("C:\\Users\\Public\\Umicom Apps"));
        CHECK(UmiSetupValidateAbsolute("relative")!=0);
        CHECK(UmiSetupValidateAbsolute("\\\\.\\PhysicalDrive1")!=0);
        CHECK(UmiSetupValidateAbsolute("//server/share")!=0);
        CHECK(UmiSetupValidateAbsolute("C:relative")!=0);
    }

    else if(!strcmp(name,"paths.capacity")) {
        char path[4097];
        memset(path,'a',sizeof path-1);
        path[0]='/';
        path[sizeof path-1]=0;
        CHECK(UmiSetupValidateAbsolute(path)!=0);
        char out[4096];
        CHECK(ScJoin(NULL,"x",out)!=0);
        CHECK(UmiSetupValidateRelative(NULL)!=0);
    }

    else if(!strcmp(name,"numbers.overflow")) {
        uint64_t n;
        CHECK(ScNumber("18446744073709551615",&n)&&n==UINT64_MAX);
        CHECK(!ScNumber("18446744073709551616",&n));
        CHECK(!ScNumber("-1",&n));
        CHECK(!ScNumber("01",&n));
        CHECK(!ScNumber("",&n));
    }

    else return 2;
    return 0;

}

static const char GOOD[]="UMICOM_SUITE\t1\napp\tbank\tUmicom Bank\tbin/bank.exe\nfile\tbank\t1\tca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb\tbin/bank.exe\n";

int TestCatalogue(const char *name)
{

    UmiSetupBundle *b=calloc(1,sizeof *b);
    CHECK(b);
    UmiStatus s;

    if(!strcmp(name,"catalogue.roundtrip")) {
        OK(ScDecode(GOOD,sizeof GOOD-1,b,0));
        CHECK(b->appCount==1&&b->fileCount==1);
        ScText t;
        ScTextInit(&t);
        OK(ScEncode(b,1,0,&t));
        CHECK(t.size==sizeof GOOD-1&&!memcmp(t.data,GOOD,t.size));
        ScTextFree(&t);
    }

    else if(!strcmp(name,"catalogue.duplicate")) {
        ScText t;
        ScTextInit(&t);
        ScPrint(&t,"%s%s",GOOD,strstr(GOOD,"file\t"));
        CHECK(ScDecode(t.data,t.size,b,0)!=0);
        ScTextFree(&t);
    }

    else if(!strcmp(name,"catalogue.unknown")) {
        const char *bad="UMICOM_SUITE\t1\nexecute\tanything\n";
        CHECK(ScDecode(bad,strlen(bad),b,0)!=0);
    }

    else if(!strcmp(name,"catalogue.missing_entry")) {
        const char *bad="UMICOM_SUITE\t1\napp\tbank\tBank\tbin/bank.exe\n";
        CHECK(ScDecode(bad,strlen(bad),b,0)!=0);
    }

    else if(!strcmp(name,"catalogue.overflow")) {
        ScText t;
        ScTextInit(&t);
        ScPrint(&t,"UMICOM_SUITE\t1\napp\tbank\tBank\tbin/bank.exe\nfile\tbank\t18446744073709551615\t%s\tbin/bank.exe\n","ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
        CHECK(ScDecode(t.data,t.size,b,0)!=0);
        ScTextFree(&t);
    }

    else if(!strcmp(name,"catalogue.case_collision")||!strcmp(name,"catalogue.parent_collision")) {
        ScFile a= {
            0
        },c= {
            0
        };
        strcpy(a.relative,!strcmp(name,"catalogue.case_collision")?"bin/a":"Bin");
        strcpy(a.hash,"ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
        a.owner=64;
        a.bytes=1;
        c=a;
        strcpy(c.relative,!strcmp(name,"catalogue.case_collision")?"bin/A":"bin/a");
        OK(ScAddFile(b,&a));
        CHECK(ScAddFile(b,&c)!=UMI_STATUS_OK);
    }

    else if(!strcmp(name,"catalogue.reserved")) {
        ScFile f={0};strcpy(f.relative,"UMICOM-INSTALL.UMI");
        strcpy(f.hash,"ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
        f.owner=64;f.bytes=1;CHECK(ScAddFile(b,&f)==UMI_STATUS_PARSE_ERROR);
    }
    else if(!strcmp(name,"catalogue.nonadjacent_parent")) {
        ScFile f={0};strcpy(f.relative,"a");
        strcpy(f.hash,"ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
        f.owner=64;f.bytes=1;OK(ScAddFile(b,&f));
        strcpy(f.relative,"a-middle");OK(ScAddFile(b,&f));
        strcpy(f.relative,"A/b");CHECK(ScAddFile(b,&f)==UMI_STATUS_INVALID_STATE);
    }
    else if(!strcmp(name,"catalogue.sixty_four_apps")) {
        ScText text;ScTextInit(&text);ScPrint(&text,"UMICOM_SUITE\t1\n");
        for(unsigned i=0;i<64U;++i)ScPrint(&text,"app\tapp%u\tApplication %u\tbin/app%u.exe\n",i,i,i);
        for(unsigned i=0;i<64U;++i)ScPrint(&text,"file\tapp%u\t1\tca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb\tbin/app%u.exe\n",i,i);
        OK(text.status);OK(ScDecode(text.data,text.size,b,0));
        CHECK(UmiSetupAllApplications(b)==UINT64_MAX);
        ScText selected;ScTextInit(&selected);OK(ScEncode(b,UINT64_C(1)<<63U,1,&selected));
        CHECK(strstr(selected.data,"app63")!=NULL&&strstr(selected.data,"app62")==NULL);
        ScTextFree(&text);ScTextFree(&selected);
    }
    else if(!strcmp(name,"catalogue.mutations")) {
        uint32_t seed=0x1479U;
        for(unsigned i=0; i<10000U; ++i) {
            char sample[sizeof GOOD];
            memcpy(sample,GOOD,sizeof sample);
            seed=seed*1664525U+1013904223U;
            size_t position=seed%(sizeof GOOD-1U);
            sample[position]=(char)(seed>>24U);
            s=ScDecode(sample,sizeof GOOD-1U,b,0);
            CHECK(s>=UMI_STATUS_OK&&s<=UMI_STATUS_BUSY);
            UmiSetupBundleDestroy(b);
            b=calloc(1,sizeof *b);
            CHECK(b);
        }
    }

    else {
        UmiSetupBundleDestroy(b);
        return 2;
    }
    UmiSetupBundleDestroy(b);
    return 0;

}

const char *TestProgram=NULL;

int main(int argc,char **argv)
{

    if(argc!=2)return 2;
    TestProgram=argv[0];
    const char *name=argv[1];
    int result;

    if(!strncmp(name,"paths.",6)||!strncmp(name,"numbers.",8))result=TestPaths(name);

    else if(!strncmp(name,"catalogue.",10))result=TestCatalogue(name);

    else if(!strncmp(name,"vm.",3))result=TestVm(name);

    else if(!strncmp(name,"media.",6))result=TestMedia(name);

    else result=TestInstall(name);

    if(result==0)printf("PASS %s\n",name);
    return result;

}
