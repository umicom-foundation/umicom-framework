/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/command_history.h
 *
 * PURPOSE:
 *   Capture command invocation history metadata for ranking and audit.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_COMMAND_HISTORY_H
#define UMICOM_DESKTOP_CONTROL_COMMAND_HISTORY_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_COMMAND_HISTORY_API_VERSION 1U

typedef struct UmiDesktopCommandHistorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopCommandHistorySnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_command_history_init(UmiDesktopCommandHistorySnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_command_history_validate(const UmiDesktopCommandHistorySnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_command_history_set_subject(UmiDesktopCommandHistorySnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_command_history_set_detail(UmiDesktopCommandHistorySnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_command_history_set_state(UmiDesktopCommandHistorySnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_command_history_set_priority(UmiDesktopCommandHistorySnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_command_history_set_enabled(UmiDesktopCommandHistorySnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_command_history_same_identity(const UmiDesktopCommandHistorySnapshot *left, const UmiDesktopCommandHistorySnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
