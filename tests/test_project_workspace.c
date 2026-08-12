/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/project/project.h"
int main(void){UmiProjectWorkspace *p=NULL; UmiProjectWorkspaceSnapshot s; if(umi_project_workspace_create(&p)!=UMI_STATUS_OK)return 1; if(umi_project_workspace_snapshot(p,&s)!=UMI_STATUS_OK||s.item_count!=0U)return 2; umi_project_workspace_destroy(p); return 0;}
