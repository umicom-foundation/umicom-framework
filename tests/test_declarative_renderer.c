/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_renderer.c
 *
 * PURPOSE:
 *   Verify headless rendering produces deterministic semantic component output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeclApplicationPlan plan={0};char output[1024];
    assert(umi_decl_copy_text(plan.application_id,sizeof(plan.application_id),"org.umicom.demo")==UMI_STATUS_OK);
    plan.version=umi_decl_version_current();
    plan.components=(UmiDeclNode *)calloc(1U,sizeof(UmiDeclNode));
    assert(plan.components!=NULL);
    plan.component_count=1U;
    assert(umi_decl_node_init(&plan.components[0],"root","window","-")==UMI_STATUS_OK);
    assert(umi_decl_headless_render(&plan,output,sizeof(output))==UMI_STATUS_OK);
    assert(strstr(output,"root:window")!=NULL);
    umi_decl_plan_dispose(&plan);
    return EXIT_SUCCESS;
}
