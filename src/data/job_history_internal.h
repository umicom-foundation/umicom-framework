/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/job_history_internal.h
 * PURPOSE: Define the private job record codec shared by persistence and validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DATA_JOB_HISTORY_INTERNAL_H
#define UMICOM_DATA_JOB_HISTORY_INTERNAL_H
#include "umicom/data/job_history.h"
/* Larger storage accommodates three bounded digests. Retain the previous
 * capacity for review; callers still receive unchanged output on overflow. */
#if 0
#define UMI_JOB_HISTORY_WIRE_CAPACITY 640U
#endif
#define UMI_JOB_HISTORY_WIRE_CAPACITY 896U
bool UmiJobHistoryIdentifier(const char *text, size_t capacity);
bool UmiJobHistoryCaption(const char *text, size_t capacity);
UmiStatus UmiJobHistoryEncode(const UmiJobHistoryEntry *entry, char *out, size_t capacity);
UmiStatus UmiJobHistoryDecode(const char *text, UmiJobHistoryEntry *out);
bool UmiJobHistoryReadNumber(const char **cursor, uint64_t *out, char delimiter);
#endif
