/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/evidence_text.h
 * PURPOSE: Format captured test results for shared text presentation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_EVIDENCE_TEXT_H
#define UMICOM_TEST_PLATFORM_EVIDENCE_TEXT_H
#include "umicom/test_platform/evidence.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TEST_EVIDENCE_TEXT_MAX_BYTES (1024U * 1024U)
/* Format an immutable capture without consulting live registries or executing a
 * test. The report includes retained/matching counts, distinct outcome labels,
 * run identities and an explicit no-result message. Ordering matches capture.
 *
 * On success the caller owns a NUL-terminated allocation and optional outBytes
 * excludes its terminator. Release it with UmiTestEvidenceTextFree. On failure
 * *outText is NULL and outBytes is unchanged. NULL outText/evidence is invalid.
 * Process output bytes are preserved, including invalid UTF-8; a GUI must make
 * a valid display copy rather than changing the underlying evidence. The size
 * limit excludes the terminator. This is plain text, not HTML or a shell script. */
UmiStatus UmiTestEvidenceFormatText(const UmiTestEvidence *evidence,
    char **outText, size_t *outBytes);
void UmiTestEvidenceTextFree(char *text);
#ifdef __cplusplus
}
#endif
#endif
