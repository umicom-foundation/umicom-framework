/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/layout_compare.h
 *
 * PURPOSE:
 *   Represent layout comparison fingerprints and change summary state.
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
#ifndef UMICOM_DESKTOP_CONTROL_LAYOUT_COMPARE_H
#define UMICOM_DESKTOP_CONTROL_LAYOUT_COMPARE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_LAYOUT_COMPARE_API_VERSION 1U

typedef struct UmiDesktopLayoutCompareSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopLayoutCompareSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_layout_compare_init(UmiDesktopLayoutCompareSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_layout_compare_validate(const UmiDesktopLayoutCompareSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_layout_compare_set_subject(UmiDesktopLayoutCompareSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_layout_compare_set_detail(UmiDesktopLayoutCompareSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_layout_compare_set_state(UmiDesktopLayoutCompareSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_layout_compare_set_priority(UmiDesktopLayoutCompareSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_layout_compare_set_enabled(UmiDesktopLayoutCompareSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_layout_compare_same_identity(const UmiDesktopLayoutCompareSnapshot *left, const UmiDesktopLayoutCompareSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_layout_compare_fingerprints_equal(uint64_t left_fingerprint, uint64_t right_fingerprint);

#ifdef __cplusplus
}
#endif
#endif
