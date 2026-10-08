/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_internal.h
 * PURPOSE: Share the bounded archive codec without exporting storage details.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_INTERNAL_H
#define UMICOM_TESTING_ARCHIVE_INTERNAL_H
#include "umicom/testing/archive.h"
/* Hex text is portable and delimiter-safe. No native struct layout or padding is
 * stored. The largest record is one diagnostic tail, not an entire result set. */
#define UMI_TEST_ARCHIVE_WIRE_CAPACITY                                                                       \
    (2U * (UMI_TEST_OUTPUT_CAPACITY + UMI_CTEST_JOB_PATH_CAPACITY + UMI_TEST_NAME_CAPACITY +                 \
           UMI_TEST_ID_CAPACITY + UMI_CTEST_JOB_CONFIGURATION_CAPACITY) +                                    \
     2048U)
bool UmiTestArchiveOriginValid(const UmiTestArchiveOrigin *origin);
bool UmiTestArchiveEntryValid(const UmiTestArchiveEntry *entry);
bool UmiTestArchiveRequestValid(const UmiCtestJobRequest *request);
bool UmiTestArchiveResultValid(const UmiTestResult *result);
UmiStatus UmiTestArchiveEncodeEntry(const UmiTestArchiveEntry *entry, char *wire, size_t capacity);
UmiStatus UmiTestArchiveDecodeEntry(const char *wire, UmiTestArchiveEntry *out_entry);
UmiStatus UmiTestArchiveEncodeRequest(const UmiCtestJobRequest *request, char *wire, size_t capacity);
UmiStatus UmiTestArchiveDecodeRequest(const char *wire, UmiCtestJobRequest *out_request);
UmiStatus UmiTestArchiveEncodeResult(const UmiTestResult *result, char *wire, size_t capacity);
UmiStatus UmiTestArchiveDecodeResult(const char *wire, UmiTestResult *out_result);
/* Private chunk I/O requires an already-owned archive transaction. */
UmiStatus UmiTestArchiveValueWrite(UmiDataServer *server, const char *key, const char *wire);
UmiStatus UmiTestArchiveValueRead(UmiDataServer *server, const char *key, char *wire, size_t capacity);
UmiStatus UmiTestArchiveValueDelete(UmiDataServer *server, const char *key);
#endif
