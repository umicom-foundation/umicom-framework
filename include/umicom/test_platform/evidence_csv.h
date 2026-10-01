/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/evidence_csv.h
 * PURPOSE: Export a previously captured selected-test evidence object.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_EVIDENCE_CSV_H
#define UMICOM_TEST_PLATFORM_EVIDENCE_CSV_H
#include "umicom/test_platform/evidence.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Export a previously captured selected-test evidence object.
 * Metadata discloses the exact item/session filter, source revisions and
 * retained/matching counts. Results and output use separate record types in
 * a single rectangular table; unrelated columns are empty. All captured
 * records are emitted newest first within each type. No test is launched,
 * result inferred or live registry borrowed. The caller destroys successful
 * output; failure sets *outDocument=NULL. Invalid UTF-8/control characters
 * are rejected rather than silently rewriting diagnostic evidence. */
UmiStatus UmiTestEvidenceExportCsv(const UmiTestEvidence *evidence,
    UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
