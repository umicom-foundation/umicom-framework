/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transport_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transport profile Integration Fabric capability.
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
#include "umicom/integration/fabric/transport_profile.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/transport_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricTransportProfileTransferEqual(const UmiFabricTransportProfile *a, const UmiFabricTransportProfile *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        a->max_frame_bytes == b->max_frame_bytes &&
        a->heartbeat_ms == b->heartbeat_ms &&
        a->idle_timeout_ms == b->idle_timeout_ms &&
        a->compression_allowed == b->compression_allowed &&
        a->tls_required == b->tls_required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricTransportProfileTransferTails(UmiFabricTransportProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricTransportProfileTransferMalformed(const UmiFabricTransportProfile *sample)
{
    (void)sample;
    {
        UmiFabricTransportProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_transport_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_transport_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricTransportProfileTransferCases, UmiFabricTransportProfile,
    umi_fabric_transport_profile_archive_encode, umi_fabric_transport_profile_archive_decode,
    UmiFabricTransportProfileTransferEqual, UmiFabricTransportProfileTransferTails, UmiFabricTransportProfileTransferMalformed)

int main(void) {
    UmiFabricTransportProfile item;
    CHECK(umi_fabric_transport_profile_init(&item,"secure",1048576U,1000U,5000U,true,true)==UMI_STATUS_OK);
    if (UmiFabricTransportProfileTransferCases(&item) != 0) return 1;

    CHECK(item.tls_required);
    return 0;
}
