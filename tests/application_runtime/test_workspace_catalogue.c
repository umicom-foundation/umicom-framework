/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_workspace_catalogue.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_workspace_catalogue(void){ UmiApplicationWorkspaceEntry e; assert(umi_application_workspace_catalogue_count()>0U); assert(umi_application_workspace_catalogue_at(0U,&e)==UMI_STATUS_OK); assert(e.layout!=NULL); return 0; }
