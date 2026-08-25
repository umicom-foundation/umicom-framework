/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_condition_set.c
 *
 * PURPOSE:
 *   Focused regression coverage for Combine conditions using explicit ALL or ANY semantics.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/condition.h"
#include "umicom/runtime/bootstrap/condition_set.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapConditionSet s; UmiBootstrapCondition c; bool results[2]={true,false};
    umi_bootstrap_condition_set_init(&s,UMI_BOOTSTRAP_CONDITION_ALL);
    CHECK(umi_bootstrap_condition_init(&c,UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT,"app.mode","",false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_condition_set_add(&s,&c)==UMI_STATUS_OK);
    CHECK(!umi_bootstrap_condition_set_reduce(results,2U,UMI_BOOTSTRAP_CONDITION_ALL));
    CHECK(umi_bootstrap_condition_set_reduce(results,2U,UMI_BOOTSTRAP_CONDITION_ANY));
    return 0;
}
