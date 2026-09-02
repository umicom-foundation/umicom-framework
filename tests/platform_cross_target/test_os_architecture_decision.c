/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_os_architecture_decision.c
 *
 * PURPOSE:
 *   Keep the accepted Umicom OS production and research boundaries from
 *   changing accidentally as cross-target support grows.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/cross_target/os_architecture_decision.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                      \
    do {                                                                      \
        if (!(condition)) {                                                   \
            (void)fprintf(stderr, "CHECK failed: %s:%d: %s\n",              \
                          __FILE__, __LINE__, #condition);                    \
            return 1;                                                         \
        }                                                                     \
    } while (0)

/* Verify both the accepted record and the rules that protect privileged and
 * recovery layers from acquiring a Framework dependency. */
int main(void)
{
    UmiCtOsArchitectureDecision decision;

    /* Missing output storage must fail without attempting an allocation or
     * writing through a null pointer. */
    CHECK(umi_ct_umicom_os_architecture_decision_default(NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ct_umicom_os_architecture_decision_default(&decision) ==
          UMI_STATUS_OK);
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_OK);
    CHECK(strcmp(umi_ct_os_foundation_text(
                     decision.production_foundation),
                 "Linux LTS") == 0);
    CHECK(strcmp(umi_ct_os_foundation_text(
                     decision.research_foundation),
                 "Umicom microkernel") == 0);
    CHECK(strcmp(umi_ct_os_foundation_text(UMI_CT_OS_FOUNDATION_UNKNOWN),
                 "unknown") == 0);

    /* Each mutation models an accidental architecture regression and proves
     * that validation blocks it before the policy reaches a product surface. */
    decision.kernel_uses_framework = true;
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_INVALID_STATE);
    decision.kernel_uses_framework = false;
    decision.recovery_uses_framework = true;
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_INVALID_STATE);
    decision.recovery_uses_framework = false;
    decision.research_is_product_default = true;
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_INVALID_STATE);
    decision.research_is_product_default = false;
    decision.separate_kernel_repository = false;
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_INVALID_STATE);
    decision.separate_kernel_repository = true;
    decision.separate_distribution_repository = false;
    CHECK(umi_ct_umicom_os_architecture_decision_validate(&decision) ==
          UMI_STATUS_INVALID_STATE);
    return 0;
}
