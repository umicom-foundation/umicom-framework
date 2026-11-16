/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/comfy_recovery_fixture.h
 *
 * PURPOSE:
 *   Construct offline native task ownership fixtures shared by submission and history checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_COMFY_RECOVERY_FIXTURE_H
#define UMICOM_TEST_COMFY_RECOVERY_FIXTURE_H
#include "umicom/media_generation/comfy_recovery.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr,"%d: %s\n",__LINE__,#e); return 1; } } while(0)
static int recovery_task_fixture(UmiProviderTaskPhase phase, UmiProviderTaskReceipt *out)
{
    UmiProviderConnection c = {0}; UmiProviderTaskContext context = {0};
    strcpy(c.id,"local-comfy"); strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Local ComfyUI"); strcpy(c.endpoint,"http://127.0.0.1:8188");
    strcpy(c.model,"workflow"); c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;
    c.timeout_ms=30000; c.enabled=true;
    strcpy(context.application_id,"media"); strcpy(context.profile_id,"owner");
    strcpy(context.local_id,"scene"); strcpy(context.operation_id,UMI_COMFY_TASK_OPERATION);
    context.request_sha256[0]=1;
    CHECK(UmiProviderTaskPrepare(&context,&c,1,out)==UMI_STATUS_OK);
    if(phase==UMI_PROVIDER_TASK_PREPARED) return 0;
    CHECK(UmiProviderTaskAdvance(out,out->revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,out)==UMI_STATUS_OK);
    if(phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN) return 0;
    CHECK(UmiProviderTaskConfirm(out,out->revision,"job-1",out)==UMI_STATUS_OK);
    return 0;
}
#endif
