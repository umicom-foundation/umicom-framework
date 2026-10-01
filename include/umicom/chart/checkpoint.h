/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/checkpoint.h
 * PURPOSE: Save and recover complete chart documents through transactional Data Server records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CHECKPOINT_H
#define UMICOM_CHART_CHECKPOINT_H
#include "umicom/chart/document.h"
#include "umicom/data/data_server.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartCheckpointReport {
    uint64_t storage_revision;
    uint64_t checkpoint_revision; /* Revision of the returned primary or recovery document. */
    uint64_t source_revision;
    uint64_t saved_at_ms;
    size_t drawing_count;
    bool storage_revision_known;
    bool durable;
    bool recovered_last_good;
    UmiStatus primary_status;
    char digest[65];
} UmiChartCheckpointReport;
/* Scope is a nonempty stable identity of at most 127 bytes, not a path. The
 * document pane identity completes the storage namespace. A borrowed server
 * must outlive the synchronous owning-thread call. Caller transactions return
 * BUSY. SQLite files provide restart durability; memory and :memory: do not.
 * Save replaces primary only when expectedRevision matches its validated
 * revision (zero if absent). A valid former primary becomes last-good. All
 * rows and both manifests are written in one transaction. Failures publish
 * neither a partial chart nor a new revision. Corrupt primary data cannot be
 * overwritten through this API. No market/order data or credentials are saved.
 * All 4096 drawing slots are supported; backend capacity failures are explicit.
 * Coordinate encoding requires IEEE-754 binary64 doubles, otherwise UNAVAILABLE.
 * Report is optional, must not alias other arguments, and is written on return. */
UmiStatus UmiChartCheckpointSave(UmiDataServer *server, const char *scope,
    const UmiChartDocument *document, uint64_t expectedRevision, uint64_t savedAtMs,
    UmiChartCheckpointReport *outReport);
/* Read and validate a complete owned document in a consistent transaction.
 * Corrupt/missing primary may recover last-good; I/O, allocation and BUSY
 * failures do not silently fall back. Recovery does not write storage.
 * A recovery document carries its own digest/checkpoint_revision; storage_revision remains
 * the current primary's CAS revision only when trustworthy, so callers must
 * check storage_revision_known before offering Save. Failure sets output NULL.
 * Digest is unkeyed corruption evidence, not authentication of a trusted author. */
UmiStatus UmiChartCheckpointLoad(UmiDataServer *server, const char *scope,
    const char *paneId, UmiChartDocument **outDocument, UmiChartCheckpointReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
