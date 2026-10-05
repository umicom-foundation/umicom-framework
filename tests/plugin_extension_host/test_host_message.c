/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_message.c
 *
 * PURPOSE:
 *   Exercise describe one versioned extension-host protocol message.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
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
#include "umicom/plugin/extension_host/host_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/plugin/extension_host/host_message.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPluginExtensionHostHostMessageTransferEqual(const UmiPluginExtensionHostHostMessage *a, const UmiPluginExtensionHostHostMessage *b)
{
    return a->protocol_version == b->protocol_version &&
        a->type == b->type &&
        a->sequence == b->sequence &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->payload, b->payload) == 0 &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPluginExtensionHostHostMessageTransferTails(UmiPluginExtensionHostHostMessage *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->payload) + 1U;
        memset(value->payload + used, 0xa5, sizeof(value->payload) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPluginExtensionHostHostMessageTransferMalformed(const UmiPluginExtensionHostHostMessage *sample)
{
    (void)sample;
    {
        UmiPluginExtensionHostHostMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_plugin_extension_host_host_message_valid(&invalid)) ||
            umi_plugin_extension_host_host_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPluginExtensionHostHostMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payload, 'x', sizeof(invalid.payload));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_plugin_extension_host_host_message_valid(&invalid)) ||
            umi_plugin_extension_host_host_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payload was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPluginExtensionHostHostMessageTransferCases, UmiPluginExtensionHostHostMessage,
    umi_plugin_extension_host_host_message_archive_encode, umi_plugin_extension_host_host_message_archive_decode,
    UmiPluginExtensionHostHostMessageTransferEqual, UmiPluginExtensionHostHostMessageTransferTails, UmiPluginExtensionHostHostMessageTransferMalformed)

int main(void) { UmiPluginExtensionHostHostMessage m; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_message_build(&m,1U,2U,3U,"s","hello")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_host_message_valid(&m)) return 2;
    if (UmiPluginExtensionHostHostMessageTransferCases(&m) != 0) return 1;
 m.sequence=4U; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_message_valid(&m)) return 3; return 0; }
