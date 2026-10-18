/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_service_descriptor.c
 *
 * PURPOSE:
 *   Implement the test service descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_service_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe Framework services, ownership scope and lifetime without global variables.
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/service_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/runtime/bootstrap/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBootstrapServiceDescriptorTransferEqual(const UmiBootstrapServiceDescriptor *a, const UmiBootstrapServiceDescriptor *b)
{
    return strcmp(a->key.service_id, b->key.service_id) == 0 &&
        strcmp(a->key.qualifier, b->key.qualifier) == 0 &&
        strcmp(a->provider_id, b->provider_id) == 0 &&
        a->scope == b->scope &&
        a->lifetime == b->lifetime &&
        a->priority == b->priority &&
        a->flags == b->flags &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBootstrapServiceDescriptorTransferTails(UmiBootstrapServiceDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->key.service_id) + 1U;
        memset(value->key.service_id + used, 0xa5, sizeof(value->key.service_id) - used);
    }
    {
        size_t used = strlen(value->key.qualifier) + 1U;
        memset(value->key.qualifier + used, 0xa5, sizeof(value->key.qualifier) - used);
    }
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBootstrapServiceDescriptorTransferMalformed(const UmiBootstrapServiceDescriptor *sample)
{
    (void)sample;
    {
        UmiBootstrapServiceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key.service_id, 'x', sizeof(invalid.key.service_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_service_descriptor_valid(&invalid)) ||
            umi_bootstrap_service_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key.service_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBootstrapServiceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key.qualifier, 'x', sizeof(invalid.key.qualifier));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_service_descriptor_valid(&invalid)) ||
            umi_bootstrap_service_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key.qualifier was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBootstrapServiceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_service_descriptor_valid(&invalid)) ||
            umi_bootstrap_service_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBootstrapServiceDescriptorTransferCases, UmiBootstrapServiceDescriptor,
    umi_bootstrap_service_descriptor_archive_encode, umi_bootstrap_service_descriptor_archive_decode,
    UmiBootstrapServiceDescriptorTransferEqual, UmiBootstrapServiceDescriptorTransferTails, UmiBootstrapServiceDescriptorTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiBootstrapServiceDescriptorTransferCasesWithWorkspace, UmiBootstrapServiceDescriptor,
    umi_bootstrap_service_descriptor_archive_encode, umi_bootstrap_service_descriptor_archive_decode_with_workspace, UmiBootstrapServiceDescriptorTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiBootstrapServiceDescriptorConditionalArchiveCases, UmiBootstrapServiceDescriptor,
    umi_bootstrap_service_descriptor_archive_encode, umi_bootstrap_service_descriptor_archive_decode_if_unchanged,
    UmiBootstrapServiceDescriptorTransferEqual, UmiBootstrapServiceDescriptorTransferTails, UmiBootstrapServiceDescriptorTransferMalformed)



/* A rejected replacement must preserve the previously accepted service descriptor value. */
static int UmiBootstrapServiceDescriptorConstructionCases(void)
{
    UmiBootstrapServiceDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_service_descriptor_init(value, "svc.alpha", "", "provider.core", UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->provider_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_service_descriptor_init(value, "svc.alpha", "", oversized, UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_service_descriptor_init(value, "svc.alpha", "", value->provider_id, UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->provider_id, "provider.core") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapServiceDescriptorConstructionCases() != 0) return 1;


    UmiBootstrapServiceDescriptor d;
    CHECK(umi_bootstrap_service_descriptor_init(&d, "svc.alpha", "", "provider.core",
        UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_descriptor_valid(&d));
    if (UmiBootstrapServiceDescriptorTransferCases(&d) != 0) return 1;
    if (UmiBootstrapServiceDescriptorTransferCasesWithWorkspace(&d) != 0) return 1;
    if (UmiBootstrapServiceDescriptorConditionalArchiveCases(&d) != 0) return 1;

    CHECK(d.enabled);
    return 0;
}
