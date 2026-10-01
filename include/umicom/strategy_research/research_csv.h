/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/research_csv.h
 * PURPOSE: Export strategy research observations using the shared CSV contract.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Strict six-column quote interchange for a single, caller-supplied instrument.
 * C numeric locale is required; the parser never changes process locale. */
#ifndef UMICOM_RESEARCH_CSV_H
#define UMICOM_RESEARCH_CSV_H
#include "umicom/strategy_research/research_replay.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RESEARCH_CSV_BYTE_LIMIT (16U*1024U*1024U)
/** Exact header: sequence,time_ms,bid,ask,bid_size,ask_size. LF/CRLF accepted.
 * No quoted fields, blank rows, BOM, comments, NULs or implicit reordering.
 * Parse success is structural only: ReplayCreate validates sequence/identity.
 * Caller owns *outEvents and frees it with free(). outErrorLine is 1-based. */
UmiStatus UmiResearchParseCsv(const char *text,size_t bytes,const UmiInstrument *instrument,
    UmiResearchObservation **outEvents,size_t *outCount,size_t *outErrorLine);
#ifdef __cplusplus
}
#endif
#endif
