/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_storage.c
 * PURPOSE: Refuse silent repairs of modified storage and retain the previous committed state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int TestStorage(const char *name,const char *path)
{
    TestFixture f;UmiEnterpriseRecoveryReview *review=NULL;
    bool sql=strncmp(name,"sqlite_",7U)==0;
    int opened=TestOpen(&f,sql?path:NULL);if(opened!=0)return opened;
    REQUIRE(TestStale(&f)==0);uint64_t before=TestRevision(&f);
    OK(UmiEnterpriseRecoveryInspect(f.workspace,ACT(1),"delivery",&review,NULL));
    char chunk[4096],head[256];
    OK(umi_data_server_get(f.data,"enterprise.workspace/head",head,sizeof(head)));
    OK(umi_data_server_get(f.data,"enterprise.workspace/chunk/0000",chunk,sizeof(chunk)));
    UmiStatus expected=UMI_STATUS_BUSY;
    if(strstr(name,"unknown")!=NULL){OK(umi_data_server_set(f.data,"enterprise.workspace/unexpected","x"));expected=UMI_STATUS_PARSE_ERROR;}
    else if(strstr(name,"missing")!=NULL){OK(umi_data_server_delete(f.data,"enterprise.workspace/chunk/0000"));expected=UMI_STATUS_PARSE_ERROR;}
    else if(strcmp(name,"storage_namespace")==0){OK(umi_data_server_set(f.data,"another.module/value","untouched"));expected=UMI_STATUS_OK;}
    else if(strcmp(name,"sqlite_stale_writer")==0){
        UmiDataServer *otherData=NULL;UmiEnterpriseWorkspace *other=NULL;
        OK(umi_data_server_create_sqlite(path,&otherData));OK(UmiEnterpriseWorkspaceOpen(otherData,UmiEnterprisePracticeAuthorisation(f.access),&other));
        OK(UmiEnterpriseWorkspaceSetPaused(other,ACT(4),true));UmiEnterpriseWorkspaceDestroy(other);umi_data_server_destroy(otherData);
    }else{
        size_t length=strlen(chunk);REQUIRE(length>10U);chunk[length/2U]=chunk[length/2U]=='Q'?'R':'Q';
        OK(umi_data_server_set(f.data,"enterprise.workspace/chunk/0000",chunk));
    }
    UmiStatus status;
    if(strcmp(name,"storage_noop")==0)status=UmiEnterpriseWorkspaceSetRecipe(f.workspace,ACT(4),"stock.csv",true);
    else if(strcmp(name,"storage_reprepare")==0||sql)status=UmiEnterpriseRecoveryPrepare(f.workspace,ACT(1),review,"recovered",NULL);
    else status=UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true);
    REQUIRE(status==expected);
    if(expected!=UMI_STATUS_OK){
        REQUIRE(TestRevision(&f)==before);REQUIRE(!umi_data_server_in_transaction(f.data));
        if(strcmp(name,"sqlite_stale_writer")!=0){char after[256];OK(umi_data_server_get(f.data,"enterprise.workspace/head",after,sizeof(after)));REQUIRE(strcmp(head,after)==0);}
        if(strstr(name,"missing")==NULL&&strstr(name,"unknown")==NULL&&strcmp(name,"sqlite_stale_writer")!=0){char after[4096];OK(umi_data_server_get(f.data,"enterprise.workspace/chunk/0000",after,sizeof(after)));REQUIRE(strcmp(after,chunk)==0);}
    }else {char value[32];OK(umi_data_server_get(f.data,"another.module/value",value,sizeof(value)));REQUIRE(strcmp(value,"untouched")==0);}
    UmiEnterpriseRecoveryReviewDestroy(review);TestClose(&f);return 0;
}
