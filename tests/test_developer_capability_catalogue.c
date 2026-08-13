/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_capability_catalogue.c
 *
 * PURPOSE:
 *   Verify complete, unique indexed access to the 120-capability developer
 *   roadmap and exercise the portable worker spin-hint boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/developer.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    size_t index;
    assert(umi_developer_capability_builtin_count() == 120U);
    for (index = 0U; index < umi_developer_capability_builtin_count(); ++index) {
        const UmiDeveloperCapabilityDescriptor *left =
            umi_developer_capability_builtin_at(index);
        size_t other;
        assert(left != NULL);
        for (other = index + 1U;
             other < umi_developer_capability_builtin_count(); ++other) {
            const UmiDeveloperCapabilityDescriptor *right =
                umi_developer_capability_builtin_at(other);
            assert(right != NULL);
            assert(strcmp(left->capability_id, right->capability_id) != 0);
        }
    }
    assert(umi_developer_capability_builtin_at(120U) == NULL);
    umi_developer_spin_hint(4U);
    return 0;
}
