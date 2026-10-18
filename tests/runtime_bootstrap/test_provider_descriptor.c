/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_provider_descriptor.c
 *
 * PURPOSE:
 *   Implement the test provider descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_provider_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe modules that contribute replaceable service implementations.
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/provider_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/runtime/bootstrap/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBootstrapProviderDescriptorTransferEqual(const UmiBootstrapProviderDescriptor *a, const UmiBootstrapProviderDescriptor *b)
{
    return strcmp(a->provider_id, b->provider_id) == 0 &&
        strcmp(a->module_id, b->module_id) == 0 &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBootstrapProviderDescriptorTransferTails(UmiBootstrapProviderDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->module_id) + 1U;
        memset(value->module_id + used, 0xa5, sizeof(value->module_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBootstrapProviderDescriptorTransferMalformed(const UmiBootstrapProviderDescriptor *sample)
{
    (void)sample;
    {
        UmiBootstrapProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_provider_descriptor_valid(&invalid)) ||
            umi_bootstrap_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBootstrapProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.module_id, 'x', sizeof(invalid.module_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_provider_descriptor_valid(&invalid)) ||
            umi_bootstrap_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated module_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBootstrapProviderDescriptorTransferCases, UmiBootstrapProviderDescriptor,
    umi_bootstrap_provider_descriptor_archive_encode, umi_bootstrap_provider_descriptor_archive_decode,
    UmiBootstrapProviderDescriptorTransferEqual, UmiBootstrapProviderDescriptorTransferTails, UmiBootstrapProviderDescriptorTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiBootstrapProviderDescriptorTransferCasesWithWorkspace, UmiBootstrapProviderDescriptor,
    umi_bootstrap_provider_descriptor_archive_encode, umi_bootstrap_provider_descriptor_archive_decode_with_workspace, UmiBootstrapProviderDescriptorTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiBootstrapProviderDescriptorConditionalArchiveCases, UmiBootstrapProviderDescriptor,
    umi_bootstrap_provider_descriptor_archive_encode, umi_bootstrap_provider_descriptor_archive_decode_if_unchanged,
    UmiBootstrapProviderDescriptorTransferEqual, UmiBootstrapProviderDescriptorTransferTails, UmiBootstrapProviderDescriptorTransferMalformed)



/* A rejected replacement must preserve the previously accepted provider descriptor value. */
static int UmiBootstrapProviderDescriptorConstructionCases(void)
{
    UmiBootstrapProviderDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_provider_descriptor_init(value, "provider.alpha", "module.alpha", 5) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->provider_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_provider_descriptor_init(value, oversized, "module.alpha", 5) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_provider_descriptor_init(value, value->provider_id, "module.alpha", 5) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->provider_id, "provider.alpha") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->module_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_provider_descriptor_init(value, "provider.alpha", oversized, 5) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_provider_descriptor_init(value, "provider.alpha", value->module_id, 5) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->module_id, "module.alpha") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapProviderDescriptorConstructionCases() != 0) return 1;


    UmiBootstrapProviderDescriptor d;
    CHECK(umi_bootstrap_provider_descriptor_init(&d,"provider.alpha","module.alpha",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_provider_descriptor_valid(&d));
    if (UmiBootstrapProviderDescriptorTransferCases(&d) != 0) return 1;
    if (UmiBootstrapProviderDescriptorTransferCasesWithWorkspace(&d) != 0) return 1;
    if (UmiBootstrapProviderDescriptorConditionalArchiveCases(&d) != 0) return 1;

    return 0;
}
