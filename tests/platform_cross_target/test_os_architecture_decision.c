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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
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
#include "../value_archive/transfer_cases.h"

#include "umicom/platform/cross_target/os_architecture_decision.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCtOsArchitectureDecisionTransferEqual(const UmiCtOsArchitectureDecision *a, const UmiCtOsArchitectureDecision *b)
{
    return a->structure_size == b->structure_size &&
        a->api_version == b->api_version &&
        a->production_foundation == b->production_foundation &&
        a->portability_foundation == b->portability_foundation &&
        a->research_foundation == b->research_foundation &&
        a->kernel_uses_framework == b->kernel_uses_framework &&
        a->recovery_uses_framework == b->recovery_uses_framework &&
        a->normal_user_space_uses_framework == b->normal_user_space_uses_framework &&
        a->freestanding_subset_allowed == b->freestanding_subset_allowed &&
        a->separate_kernel_repository == b->separate_kernel_repository &&
        a->separate_distribution_repository == b->separate_distribution_repository &&
        a->research_is_product_default == b->research_is_product_default;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCtOsArchitectureDecisionTransferTails(UmiCtOsArchitectureDecision *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCtOsArchitectureDecisionTransferMalformed(const UmiCtOsArchitectureDecision *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCtOsArchitectureDecisionTransferCases, UmiCtOsArchitectureDecision,
    umi_ct_umicom_os_architecture_decision_archive_encode, umi_ct_umicom_os_architecture_decision_archive_decode,
    UmiCtOsArchitectureDecisionTransferEqual, UmiCtOsArchitectureDecisionTransferTails, UmiCtOsArchitectureDecisionTransferMalformed)

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
    if (UmiCtOsArchitectureDecisionTransferCases(&decision) != 0) return 1;

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
