/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_feature_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Evaluate feature-pack presence without application-specific branches.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/collection_binding.h"
#include "umicom/runtime/bootstrap/feature_condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapIdList l; umi_bootstrap_collection_binding_init(&l);
    CHECK(umi_bootstrap_collection_binding_add(&l,"feature.ai")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_feature_condition_match(&l,"feature.ai"));
    return 0;
}
