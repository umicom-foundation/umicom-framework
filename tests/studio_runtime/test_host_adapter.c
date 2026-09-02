/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_host_adapter.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime host adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio_runtime/host_adapter.h"
/*
 * Exercise title and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus title(void*u,const char*t){(void)u;(void)t;return UMI_STATUS_OK;}
/*
 * Exercise surface and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus surface(void*u,const UmiApplicationShellState*s){(void)u;(void)s;return UMI_STATUS_OK;}
/*
 * Exercise status and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus status(void*u,const UmiStudioRuntimeStatusModel*s){(void)u;(void)s;return UMI_STATUS_OK;}
/*
 * Exercise tabs and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus tabs(void*u,const UmiStudioRuntimeDocumentTabs*t){(void)u;(void)t;return UMI_STATUS_OK;}
/*
 * Exercise workflow and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus workflow(void*u,const UmiIdeWorkflowReport*r){(void)u;(void)r;return UMI_STATUS_OK;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeHostAdapter adapter={title,surface,status,tabs,workflow,NULL,NULL};
    assert(umi_studio_host_adapter_validate(&adapter)==UMI_STATUS_OK);
    adapter.update_status=NULL;
    assert(umi_studio_host_adapter_validate(&adapter)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

