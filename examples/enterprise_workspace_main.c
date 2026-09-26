/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/enterprise_workspace_main.c
 * PURPOSE: Follow the same reviewed import from the command line, using memory only.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/enterprise_workspace/practice.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* This example uses real policy, parser and transaction implementations.
 * Practice actors explain duties; they do not authenticate a real person. */
int main(void)
{
    UmiEnterprisePracticeAccess *access=NULL;UmiDataServer *data=NULL;
    UmiEnterpriseWorkspace *workspace=NULL;UmiEnterpriseRow row;
    const char csv[]="item_id,label,quantity\nnotebooks,Notebooks,12\npencils,Pencils,8\n";
    UmiStatus status=UmiEnterprisePracticeAccessCreate(&access);
    if(status==UMI_STATUS_OK)status=umi_data_server_create_memory(&data);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceOpen(data,UmiEnterprisePracticeAuthorisation(access),&workspace);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceSetRecipe(workspace,UmiEnterprisePracticeActor(4),"stock.csv",true);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceCreateDataset(workspace,UmiEnterprisePracticeActor(1),"supplies","Workshop supplies");
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspacePrepare(workspace,UmiEnterprisePracticeActor(1),"stock-opening","supplies","stock.csv",csv,strlen(csv),NULL);
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceReview(workspace,UmiEnterprisePracticeActor(2),"stock-opening",true,"Checked against the stock sheet");
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceExecute(workspace,UmiEnterprisePracticeActor(3),"stock-opening");
    for(size_t i=0U;status==UMI_STATUS_OK&&i<2U;++i){status=UmiEnterpriseWorkspaceRowAt(workspace,"supplies",i,&row);
        if(status==UMI_STATUS_OK)(void)printf("%s | quantity %" PRIu64 " | source job %s\n",row.id,row.quantity,row.sourceJob);}
    /* Exact repeated execution returns its saved result without importing again. */
    if(status==UMI_STATUS_OK)status=UmiEnterpriseWorkspaceExecute(workspace,UmiEnterprisePracticeActor(3),"stock-opening");
    if(status==UMI_STATUS_OK)(void)puts("Practice complete. Two rows imported once. No network request or file write was made.");
    else(void)fprintf(stderr,"Enterprise practice failed with status %d.\n",(int)status);
    UmiEnterpriseWorkspaceDestroy(workspace);umi_data_server_destroy(data);UmiEnterprisePracticeAccessDestroy(access);
    return status==UMI_STATUS_OK?0:1;
}
