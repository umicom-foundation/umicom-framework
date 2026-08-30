/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/command_ranking.h
 *
 * PURPOSE:
 *   Store global command ranking policy independent from a concrete palette widget.
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
#ifndef UMICOM_DESKTOP_CONTROL_COMMAND_RANKING_H
#define UMICOM_DESKTOP_CONTROL_COMMAND_RANKING_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_COMMAND_RANKING_API_VERSION 1U

typedef struct UmiDesktopCommandRankingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopCommandRankingSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_command_ranking_init(UmiDesktopCommandRankingSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_command_ranking_validate(const UmiDesktopCommandRankingSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_command_ranking_set_subject(UmiDesktopCommandRankingSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_command_ranking_set_detail(UmiDesktopCommandRankingSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_command_ranking_set_state(UmiDesktopCommandRankingSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_command_ranking_set_priority(UmiDesktopCommandRankingSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_command_ranking_set_enabled(UmiDesktopCommandRankingSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_command_ranking_same_identity(const UmiDesktopCommandRankingSnapshot *left, const UmiDesktopCommandRankingSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
uint32_t umi_desktop_command_ranking_score(uint32_t base_priority, uint32_t invocation_count, bool context_match);

#ifdef __cplusplus
}
#endif
#endif
