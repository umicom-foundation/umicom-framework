/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_crash_record.h
 *
 * PURPOSE:
 *   Capture crash/restart evidence without coupling Desk to one process backend.
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
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_CRASH_RECORD_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_CRASH_RECORD_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_CRASH_RECORD_API_VERSION 1U

typedef struct UmiDesktopApplicationCrashRecordSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationCrashRecordSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_crash_record_init(UmiDesktopApplicationCrashRecordSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_crash_record_validate(const UmiDesktopApplicationCrashRecordSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_crash_record_set_subject(UmiDesktopApplicationCrashRecordSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_crash_record_set_detail(UmiDesktopApplicationCrashRecordSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_crash_record_set_state(UmiDesktopApplicationCrashRecordSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_crash_record_set_priority(UmiDesktopApplicationCrashRecordSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_crash_record_set_enabled(UmiDesktopApplicationCrashRecordSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_crash_record_same_identity(const UmiDesktopApplicationCrashRecordSnapshot *left, const UmiDesktopApplicationCrashRecordSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
