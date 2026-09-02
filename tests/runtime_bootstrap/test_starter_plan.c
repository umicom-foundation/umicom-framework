/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_plan.c
 *
 * PURPOSE:
 *   Implement the test starter plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for Build deterministic, duplicate-free starter activation plans.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/starter_descriptor.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"
#include "umicom/runtime/bootstrap/starter_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapStarterDescriptor a,b; UmiBootstrapStarterCatalogue c; UmiBootstrapStarterPlan p;
    umi_bootstrap_starter_catalogue_init(&c); umi_bootstrap_starter_plan_init(&p);
    CHECK(umi_bootstrap_starter_descriptor_init(&a,"starter.low","feature.low",1)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_descriptor_init(&b,"starter.high","feature.high",10)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_add(&c,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_add(&c,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_plan_add(&p,&c,"starter.low")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_plan_add(&p,&c,"starter.high")==UMI_STATUS_OK);
    CHECK(strcmp(p.starter_ids[0],"starter.high")==0);
    return 0;
}
