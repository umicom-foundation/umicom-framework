/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_factory_descriptor.c
 *
 * PURPOSE:
 *   Implement the test factory descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_factory_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe named factories that create services for the canonical service registry.
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/service_key.h"
#include "umicom/runtime/bootstrap/factory_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/runtime/bootstrap/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBootstrapFactoryDescriptorTransferEqual(const UmiBootstrapFactoryDescriptor *a, const UmiBootstrapFactoryDescriptor *b)
{
    return strcmp(a->factory_id, b->factory_id) == 0 &&
        strcmp(a->produces.service_id, b->produces.service_id) == 0 &&
        strcmp(a->produces.qualifier, b->produces.qualifier) == 0 &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBootstrapFactoryDescriptorTransferTails(UmiBootstrapFactoryDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->factory_id) + 1U;
        memset(value->factory_id + used, 0xa5, sizeof(value->factory_id) - used);
    }
    {
        size_t used = strlen(value->produces.service_id) + 1U;
        memset(value->produces.service_id + used, 0xa5, sizeof(value->produces.service_id) - used);
    }
    {
        size_t used = strlen(value->produces.qualifier) + 1U;
        memset(value->produces.qualifier + used, 0xa5, sizeof(value->produces.qualifier) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBootstrapFactoryDescriptorTransferMalformed(const UmiBootstrapFactoryDescriptor *sample)
{
    (void)sample;
    {
        UmiBootstrapFactoryDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.factory_id, 'x', sizeof(invalid.factory_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_factory_descriptor_valid(&invalid)) ||
            umi_bootstrap_factory_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated factory_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBootstrapFactoryDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.produces.service_id, 'x', sizeof(invalid.produces.service_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_factory_descriptor_valid(&invalid)) ||
            umi_bootstrap_factory_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated produces.service_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBootstrapFactoryDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.produces.qualifier, 'x', sizeof(invalid.produces.qualifier));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bootstrap_factory_descriptor_valid(&invalid)) ||
            umi_bootstrap_factory_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated produces.qualifier was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBootstrapFactoryDescriptorTransferCases, UmiBootstrapFactoryDescriptor,
    umi_bootstrap_factory_descriptor_archive_encode, umi_bootstrap_factory_descriptor_archive_decode,
    UmiBootstrapFactoryDescriptorTransferEqual, UmiBootstrapFactoryDescriptorTransferTails, UmiBootstrapFactoryDescriptorTransferMalformed)

int main(void) {

    UmiBootstrapServiceKey key; UmiBootstrapFactoryDescriptor d;
    CHECK(umi_bootstrap_service_key_init(&key,"svc.alpha","")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_descriptor_init(&d,"factory.alpha",&key,10)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_factory_descriptor_valid(&d));
    if (UmiBootstrapFactoryDescriptorTransferCases(&d) != 0) return 1;

    return 0;
}
