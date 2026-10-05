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

int main(void) {

    UmiBootstrapServiceDescriptor d;
    CHECK(umi_bootstrap_service_descriptor_init(&d, "svc.alpha", "", "provider.core",
        UMI_BOOTSTRAP_SCOPE_APPLICATION, UMI_BOOTSTRAP_LIFETIME_EAGER, 10) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_descriptor_valid(&d));
    if (UmiBootstrapServiceDescriptorTransferCases(&d) != 0) return 1;

    CHECK(d.enabled);
    return 0;
}
