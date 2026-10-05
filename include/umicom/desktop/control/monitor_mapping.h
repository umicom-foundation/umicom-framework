/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/monitor_mapping.h
 *
 * PURPOSE:
 *   Describe persisted-to-current monitor mapping decisions.
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
#ifndef UMICOM_DESKTOP_CONTROL_MONITOR_MAPPING_H
#define UMICOM_DESKTOP_CONTROL_MONITOR_MAPPING_H

#include "umicom/desktop/control/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_MONITOR_MAPPING_API_VERSION 1U

/**
 * Represent the desktop monitor mapping snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopMonitorMappingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopMonitorMappingSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_monitor_mapping_init(UmiDesktopMonitorMappingSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_monitor_mapping_validate(const UmiDesktopMonitorMappingSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_monitor_mapping_set_subject(UmiDesktopMonitorMappingSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_monitor_mapping_set_detail(UmiDesktopMonitorMappingSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_monitor_mapping_set_state(UmiDesktopMonitorMappingSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_monitor_mapping_set_priority(UmiDesktopMonitorMappingSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_monitor_mapping_set_enabled(UmiDesktopMonitorMappingSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_monitor_mapping_same_identity(const UmiDesktopMonitorMappingSnapshot *left, const UmiDesktopMonitorMappingSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
uint32_t umi_desktop_monitor_mapping_confidence(bool exact_identity, bool same_geometry, bool primary_fallback);

/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_desktop_monitor_mapping_init_checked(UmiDesktopMonitorMappingSnapshot *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_desktop_monitor_mapping_archive_encode(const UmiDesktopMonitorMappingSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_desktop_monitor_mapping_archive_decode(const void *bytes, size_t byte_count,
    UmiDesktopMonitorMappingSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
