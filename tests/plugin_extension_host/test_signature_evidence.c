/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_signature_evidence.c
 *
 * PURPOSE:
 *   Exercise capture signature-verification evidence supplied by the Framework security layer.
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
#include "umicom/plugin/extension_host/signature_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/plugin/extension_host/signature_evidence.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPluginExtensionHostSignatureEvidenceTransferEqual(const UmiPluginExtensionHostSignatureEvidence *a, const UmiPluginExtensionHostSignatureEvidence *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->id, b->id) == 0 &&
        strcmp(a->subject, b->subject) == 0 &&
        a->version == b->version &&
        a->risk == b->risk &&
        a->flags == b->flags &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPluginExtensionHostSignatureEvidenceTransferTails(UmiPluginExtensionHostSignatureEvidence *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->subject) + 1U;
        memset(value->subject + used, 0xa5, sizeof(value->subject) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPluginExtensionHostSignatureEvidenceTransferMalformed(const UmiPluginExtensionHostSignatureEvidence *sample)
{
    (void)sample;
    {
        UmiPluginExtensionHostSignatureEvidence invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_plugin_extension_host_signature_evidence_validate(&invalid) != UMI_STATUS_OK) ||
            umi_plugin_extension_host_signature_evidence_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPluginExtensionHostSignatureEvidence invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject, 'x', sizeof(invalid.subject));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_plugin_extension_host_signature_evidence_validate(&invalid) != UMI_STATUS_OK) ||
            umi_plugin_extension_host_signature_evidence_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPluginExtensionHostSignatureEvidenceTransferCases, UmiPluginExtensionHostSignatureEvidence,
    umi_plugin_extension_host_signature_evidence_archive_encode, umi_plugin_extension_host_signature_evidence_archive_decode,
    UmiPluginExtensionHostSignatureEvidenceTransferEqual, UmiPluginExtensionHostSignatureEvidenceTransferTails, UmiPluginExtensionHostSignatureEvidenceTransferMalformed)

int main(void)
{
    UmiPluginExtensionHostSignatureEvidence value; umi_plugin_extension_host_signature_evidence_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_signature_evidence_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_signature_evidence_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiPluginExtensionHostSignatureEvidenceTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_plugin_extension_host_signature_evidence_fingerprint(&value) == 0U) return 3;
    return 0;
}
