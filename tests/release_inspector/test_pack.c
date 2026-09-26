/* Umicom Foundation | Sammy Hegab | MIT
 * Exercise the production C packer, selective install and inspector together.
 * Inputs are actual Clang/LLD PE files, NOT executable Windows acceptance here. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "test_support.h"
int TestPackLinkedPe(const char *app,const char *dll,const char *bootstrap)
{
#ifndef _WIN32
    char root[UMI_SETUP_PATH_CAPACITY]="/tmp/umicom-linked-release-XXXXXX";
    CHECK(mkdtemp(root));UmiSetupReport io={0};char dllHash[65];uint64_t bytes;
    OK(ScDigest(dll,dllHash,&bytes,NULL,NULL,&io));ScText list;ScTextInit(&list);
    ScPrint(&list,"UMICOM_SETUP_INPUT\t1\n");
    const char *ids[]={"notes","review"};
    for(size_t i=0;i<2U;++i) {
        char executable[UMI_SETUP_PATH_CAPACITY],report[UMI_SETUP_PATH_CAPACITY],leaf[128],hash[65];
        (void)snprintf(leaf,sizeof leaf,"%s.exe",ids[i]);OK(ScJoin(root,leaf,executable));
        OK(ScDigest(app,hash,&bytes,NULL,NULL,&io));OK(ScCopy(app,executable,hash,bytes,NULL,NULL,&io));
        (void)snprintf(leaf,sizeof leaf,"%s.runtime.cmake",ids[i]);OK(ScJoin(root,leaf,report));
        ScText content;ScTextInit(&content);
        ScPrint(&content,"set(UMI_REPORT_EXECUTABLE [==[%s]==])\nset(UMI_REPORT_EXECUTABLE_SHA256 [==[%s]==])\n"
            "set(UMI_REPORT_TARGET [==[%s]==])\nset(UMI_REPORT_PRODUCT [==[%s]==])\n"
            "set(UMI_REPORT_FILES [==[bin/umicom-practice-storage.dll|%s|%s]==])\n",executable,hash,ids[i],ids[i],dll,dllHash);
        OK(content.status);OK(ScWriteNew(report,content.data,content.size,&io));ScTextFree(&content);
        ScPrint(&list,"app\t%s\t%s\t%s\n",ids[i],ids[i],report);
    }
    char inputs[UMI_SETUP_PATH_CAPACITY],release[UMI_SETUP_PATH_CAPACITY],installed[UMI_SETUP_PATH_CAPACITY];
    OK(ScJoin(root,"inputs.tsv",inputs));OK(list.status);OK(ScWriteNew(inputs,list.data,list.size,&io));ScTextFree(&list);
    OK(ScJoin(root,"release",release));OK(ScJoin(root,"installed caf\xc3\xa9",installed));
    OK(UmiSetupPack(inputs,release,bootstrap,NULL,NULL,&io));
    UmiSetupBundle *bundle=NULL;OK(UmiSetupBundleOpen(release,&bundle,&io));
    UmiReleaseInspection result={0};OK(UmiReleaseInspectBundle(bundle,3U,NULL,NULL,&result));
    CHECK(result.complete && !result.issues && !result.runtimeTested && result.privateImports==2U);
    char fingerprint[65];OK(UmiSetupReview(bundle,1U,installed,fingerprint,NULL,NULL,&io));
    OK(UmiSetupInstall(bundle,1U,installed,fingerprint,NULL,NULL,&io));UmiSetupBundleDestroy(bundle);
    OK(UmiReleaseInspectInstallation(installed,NULL,NULL,&result));
    CHECK(result.imagesChecked==3U && result.privateImports==1U && !result.runtimeTested);
    char absent[UMI_SETUP_PATH_CAPACITY];OK(ScJoin(installed,"bin/review.exe",absent));
    CHECK(ScFileRegular(absent,0,&io)==UMI_STATUS_NOT_FOUND);
    CHECK(UmiSetupPack(inputs,release,bootstrap,NULL,NULL,&io)==UMI_STATUS_ALREADY_EXISTS);
    printf("PASS native pack/install/inspect with linker-generated PE files. No Windows execution.\n%s\n",root);
    return 0;
#else
    (void)app;(void)dll;(void)bootstrap;return 77;
#endif
}
