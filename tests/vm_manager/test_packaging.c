/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_packaging.c
 * PURPOSE:
 *   Original native packer + inspector, with synthetic PE DATA. No Windows executable,
 *   application or QEMU binary is executed by these tests.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Original native packer + inspector, with synthetic PE DATA. No Windows
 * executable, application or QEMU binary is executed by these tests. */
#define _GNU_SOURCE
#include "../release_inspector/test_support.h"
#include <unistd.h>
static int Put(const char*root,const char*name,const void*data,size_t size,char out[4096]){
    OK(ScParents(root,name,NULL));
    OK(ScJoin(root,name,out));
    OK(ScWriteNew(out,data,size,NULL));
    return 0;
}
int main(int argc,char**argv){
    CHECK(argc==2);
    const char*mode=argv[1];
    char root[]="/tmp/umicom-vm-pack-XXXXXX";
    CHECK(mkdtemp(root));
    unsigned char pe[TEST_PE_BYTES];
    ScText inputs;
    ScTextInit(&inputs);
    ScPrint(&inputs,"UMICOM_SETUP_INPUT\t1\n");
    const char*ids[]={
        "notes","umicom-vm-manager"
    };
    for(size_t i=0;i<2;++i){
        TestPe(pe,0,NULL,NULL);
        char leaf[128],app[4096],report[4096],hash[65];
        uint64_t bytes;
        snprintf(leaf,sizeof leaf,"%s.exe",ids[i]);
        CHECK(!Put(root,leaf,pe,sizeof pe,app));
        OK(ScDigest(app,hash,&bytes,NULL,NULL,NULL));
        ScText text;
        ScTextInit(&text);
        ScPrint(&text,"set(UMI_REPORT_EXECUTABLE [==[%s]==])\nset(UMI_REPORT_EXECUTABLE_SHA256 [==[%s]==])\nset(UMI_REPORT_TARGET [==[%s]==])\nset(UMI_REPORT_PRODUCT [==[%s]==])\nset(UMI_REPORT_FILES [==[]==])\n",app,hash,ids[i],ids[i]);
        snprintf(leaf,sizeof leaf,"%s.runtime.cmake",ids[i]);
        CHECK(!Put(root,leaf,text.data,text.size,report));
        ScTextFree(&text);
        ScPrint(&inputs,"app\t%s\t%s\t%s\n",ids[i],ids[i],report);
    }
    char consumer[4096],dll[4096];
    TestPe(pe,0,"private.dll",NULL);
    CHECK(!Put(root,"qemu-fixture.exe",pe,sizeof pe,consumer));
    TestPe(pe,1,NULL,NULL);
    CHECK(!Put(root,"private.dll",pe,sizeof pe,dll));
    const char*prefix=!strcmp(mode,"case_namespace")?"Share/Umicom/Qemu":"share/umicom/qemu";
    ScPrint(&inputs,"owned\t%s\t%s/bin/qemu-system.exe\t%s\n",!strcmp(mode,"unknown_owner")?"missing-app":"umicom-vm-manager",prefix,consumer);
    if(!strcmp(mode,"global_not_private"))ScPrint(&inputs,"data\tbin/private.dll\t%s\n",dll);
    else ScPrint(&inputs,"owned\t%s\t%s/bin/private.dll\t%s\n",!strcmp(mode,"wrong_owner")?"notes":"umicom-vm-manager",prefix,dll);
    char list[4096],release[4096],installed[4096];
    CHECK(!Put(root,"inputs.tsv",inputs.data,inputs.size,list));
    ScTextFree(&inputs);
    OK(ScJoin(root,"release",release));
    UmiSetupReport report;
    UmiStatus status=UmiSetupPack(list,release,NULL,NULL,NULL,&report);
    if(!strcmp(mode,"unknown_owner")){
        CHECK(status==UMI_STATUS_PARSE_ERROR);
        return 0;
    }
    OK(status);
    UmiSetupBundle*b=NULL;
    OK(UmiSetupBundleOpen(release,&b,&report));
    UmiReleaseInspection result;
    if(!strcmp(mode,"global_not_private")||!strcmp(mode,"wrong_owner")){
        CHECK(UmiReleaseInspectBundle(b,2,NULL,NULL,&result)!=UMI_STATUS_OK&&result.issues>0);
        UmiSetupBundleDestroy(b);
        return 0;
    }
    OK(UmiReleaseInspectBundle(b,2,NULL,NULL,&result));
    CHECK(result.privateImports==1&&!result.runtimeTested);
    OK(ScJoin(root,"notes-only",installed));
    char fingerprint[65];
    OK(UmiSetupReview(b,1,installed,fingerprint,NULL,NULL,&report));
    OK(UmiSetupInstall(b,1,installed,fingerprint,NULL,NULL,&report));
    char absent[4096];
    OK(ScJoin(installed,"share/umicom/qemu/bin/qemu-system.exe",absent));
    CHECK(ScFileRegular(absent,0,NULL)==UMI_STATUS_NOT_FOUND);
    OK(UmiReleaseInspectInstallation(installed,NULL,NULL,&result));
    CHECK(result.imagesChecked==1);
    UmiSetupBundleDestroy(b);
    puts("Optional owned component stays out of a Notes-only installation.");
    return 0;
}
