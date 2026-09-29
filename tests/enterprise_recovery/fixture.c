/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/fixture.c
 * PURPOSE: Exercise the real Data Server and canonical authorisation through public APIs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int TestOpen(TestFixture *f, const char *path)
{
    memset(f, 0, sizeof(*f));
    if (path != NULL) {
        /* Only CTest-owned paths under its build directory are passed here. */
        (void)remove(path);
        UmiStatus status=umi_data_server_create_sqlite(path,&f->data);
        if (status==UMI_STATUS_UNAVAILABLE) return 77;
        REQUIRE(status==UMI_STATUS_OK);
    } else OK(umi_data_server_create_memory(&f->data));
    OK(UmiEnterprisePracticeAccessCreate(&f->access));
    OK(UmiEnterpriseWorkspaceOpen(f->data,UmiEnterprisePracticeAuthorisation(f->access),&f->workspace));
    return 0;
}
void TestClose(TestFixture *f)
{
    UmiEnterpriseWorkspaceDestroy(f->workspace);
    UmiEnterprisePracticeAccessDestroy(f->access);
    umi_data_server_destroy(f->data); memset(f,0,sizeof(*f));
}
int TestSeed(TestFixture *f)
{
    OK(UmiEnterpriseWorkspaceSetRecipe(f->workspace,ACT(4),"stock.csv",true));
    OK(UmiEnterpriseWorkspaceCreateDataset(f->workspace,ACT(1),"supplies","Workshop supplies"));
    return 0;
}
int TestApply(TestFixture *f,const char *id,const char *csv)
{
    OK(UmiEnterpriseWorkspacePrepare(f->workspace,ACT(1),id,"supplies","stock.csv",csv,strlen(csv),NULL));
    OK(UmiEnterpriseWorkspaceReview(f->workspace,ACT(2),id,true,"Checked independently"));
    OK(UmiEnterpriseWorkspaceExecute(f->workspace,ACT(3),id));
    return 0;
}
uint64_t TestRevision(TestFixture *f)
{
    UmiEnterpriseSnapshot snapshot;
    return UmiEnterpriseWorkspaceSnapshot(f->workspace,&snapshot)==UMI_STATUS_OK?snapshot.revision:UINT64_MAX;
}
int TestStale(TestFixture *f)
{
    const char *old="item_id,label,quantity\na,Notebooks,12\n";
    const char *next="item_id,label,quantity\na,Notebooks,20\n";
    const char *count="item_id,label,quantity\na,Notebooks,15\n";
    REQUIRE(TestSeed(f)==0);REQUIRE(TestApply(f,"opening",old)==0);
    OK(UmiEnterpriseWorkspacePrepare(f->workspace,ACT(1),"delivery","supplies","stock.csv",next,strlen(next),NULL));
    OK(UmiEnterpriseWorkspaceReview(f->workspace,ACT(2),"delivery",true,"Checked at twelve"));
    REQUIRE(TestApply(f,"count",count)==0);
    STATUS(UmiEnterpriseWorkspaceExecute(f->workspace,ACT(3),"delivery"),UMI_STATUS_BUSY);
    return 0;
}
