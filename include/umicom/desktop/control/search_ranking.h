/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/search_ranking.h
 *
 * PURPOSE:
 *   Store search-ranking policy weights shared by global search consumers.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_SEARCH_RANKING_H
#define UMICOM_DESKTOP_CONTROL_SEARCH_RANKING_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SEARCH_RANKING_API_VERSION 1U

/**
 * Represent the desktop search ranking snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopSearchRankingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSearchRankingSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_search_ranking_init(UmiDesktopSearchRankingSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_search_ranking_validate(const UmiDesktopSearchRankingSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_search_ranking_set_subject(UmiDesktopSearchRankingSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_search_ranking_set_detail(UmiDesktopSearchRankingSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_search_ranking_set_state(UmiDesktopSearchRankingSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_search_ranking_set_priority(UmiDesktopSearchRankingSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_search_ranking_set_enabled(UmiDesktopSearchRankingSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_search_ranking_same_identity(const UmiDesktopSearchRankingSnapshot *left, const UmiDesktopSearchRankingSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
uint32_t umi_desktop_search_ranking_score(const char *query, const char *label, uint32_t base_priority);

#ifdef __cplusplus
}
#endif
#endif
