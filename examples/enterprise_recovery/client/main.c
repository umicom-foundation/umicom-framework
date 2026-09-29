/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/enterprise_recovery/client/main.c
 * PURPOSE: Use only installed public headers to capture a read-only dataset.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/enterprise_workspace/practice.h"
#include "umicom/enterprise_workspace/recovery.h"
#include <stdio.h>
int main(void)
{
    UmiDataServer *data=NULL;UmiEnterprisePracticeAccess *access=NULL;
    UmiEnterpriseWorkspace *workspace=NULL;UmiEnterpriseDatasetView *view=NULL;
    UmiEnterpriseRowQuery query;UmiEnterpriseRowPage page;
    UmiStatus status=umi_data_server_create_memory(&data);
    if(status==UMI_STATUS_OK)status=UmiEnterprisePracticeAccessCreate(&access);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceOpen(data,UmiEnterprisePracticeAuthorisation(access),&workspace);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceCreateDataset(workspace,UmiEnterprisePracticeActor(1U),"notes","Notebook supplies");
    UmiEnterpriseRowQueryInit(&query);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseDatasetViewCapture(workspace,UmiEnterprisePracticeActor(0U),"notes",&query,&view);
    UmiEnterpriseWorkspaceDestroy(workspace);UmiEnterprisePracticeAccessDestroy(access);umi_data_server_destroy(data);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseDatasetViewPage(view,0U,16U,&page);
    if(status==UMI_STATUS_OK&&(!page.complete||page.count!=0U))status=UMI_STATUS_INTERNAL_ERROR;
    UmiEnterpriseDatasetViewDestroy(view);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Public SDK client failed: %d\n",(int)status);return 1;}
    puts("Public SDK capture remains readable after its workspace closes. Memory only.");return 0;
}
