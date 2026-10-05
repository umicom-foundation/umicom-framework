/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_dependency_gate.h
 *
 * PURPOSE:
 *   Represent dependency readiness before an application launch or restart is allowed.
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
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_DEPENDENCY_GATE_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_DEPENDENCY_GATE_H

#include "umicom/desktop/control/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_DEPENDENCY_GATE_API_VERSION 1U

/**
 * Represent the desktop application dependency gate snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiDesktopApplicationDependencyGateSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationDependencyGateSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_dependency_gate_init(UmiDesktopApplicationDependencyGateSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_dependency_gate_validate(const UmiDesktopApplicationDependencyGateSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_dependency_gate_set_subject(UmiDesktopApplicationDependencyGateSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_dependency_gate_set_detail(UmiDesktopApplicationDependencyGateSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_dependency_gate_set_state(UmiDesktopApplicationDependencyGateSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_dependency_gate_set_priority(UmiDesktopApplicationDependencyGateSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_dependency_gate_set_enabled(UmiDesktopApplicationDependencyGateSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_dependency_gate_same_identity(const UmiDesktopApplicationDependencyGateSnapshot *left, const UmiDesktopApplicationDependencyGateSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_application_dependency_gate_ready(uint32_t required_count, uint32_t ready_count, bool blocked);

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
UmiStatus umi_desktop_application_dependency_gate_init_checked(UmiDesktopApplicationDependencyGateSnapshot *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_desktop_application_dependency_gate_archive_encode(const UmiDesktopApplicationDependencyGateSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_desktop_application_dependency_gate_archive_decode(const void *bytes, size_t byte_count,
    UmiDesktopApplicationDependencyGateSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
