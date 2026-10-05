/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_deployment_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for deployment target, scope, rollout and update-channel profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/distribution/runtime/deployment_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/deployment_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrDeploymentProfileTransferEqual(const UmiDrDeploymentProfile *a, const UmiDrDeploymentProfile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->target, b->target) == 0 &&
        a->scope == b->scope &&
        a->channel == b->channel &&
        a->rollout_percent == b->rollout_percent &&
        a->unattended == b->unattended;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrDeploymentProfileTransferTails(UmiDrDeploymentProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->target) + 1U;
        memset(value->target + used, 0xa5, sizeof(value->target) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrDeploymentProfileTransferMalformed(const UmiDrDeploymentProfile *sample)
{
    (void)sample;
    {
        UmiDrDeploymentProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_deployment_profile_valid(&invalid)) ||
            umi_dr_deployment_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDeploymentProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target, 'x', sizeof(invalid.target));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_deployment_profile_valid(&invalid)) ||
            umi_dr_deployment_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrDeploymentProfileTransferCases, UmiDrDeploymentProfile,
    umi_dr_deployment_profile_archive_encode, umi_dr_deployment_profile_archive_decode,
    UmiDrDeploymentProfileTransferEqual, UmiDrDeploymentProfileTransferTails, UmiDrDeploymentProfileTransferMalformed)

int main(void) {
    UmiDrDeploymentProfile value; umi_dr_deployment_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"desktop")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.target,sizeof(value.target),"local")==UMI_STATUS_OK); value.scope=UMI_DR_SCOPE_USER; value.channel=UMI_DR_CHANNEL_STABLE; value.rollout_percent=100U; CHECK(umi_dr_deployment_profile_valid(&value));
    if (UmiDrDeploymentProfileTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_deployment_profile_fingerprint(&value) != 0U);
    return 0;
}
