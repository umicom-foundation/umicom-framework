/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_catalogue_filter.h
 *
 * PURPOSE:
 *   Store reusable application catalogue filter criteria.
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
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_CATALOGUE_FILTER_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_CATALOGUE_FILTER_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_CATALOGUE_FILTER_API_VERSION 1U

/**
 * Represent the desktop application catalogue filter snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiDesktopApplicationCatalogueFilterSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationCatalogueFilterSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_catalogue_filter_init(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_catalogue_filter_validate(const UmiDesktopApplicationCatalogueFilterSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_catalogue_filter_set_subject(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_catalogue_filter_set_detail(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_catalogue_filter_set_state(UmiDesktopApplicationCatalogueFilterSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_catalogue_filter_set_priority(UmiDesktopApplicationCatalogueFilterSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_catalogue_filter_set_enabled(UmiDesktopApplicationCatalogueFilterSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_catalogue_filter_same_identity(const UmiDesktopApplicationCatalogueFilterSnapshot *left, const UmiDesktopApplicationCatalogueFilterSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_application_catalogue_filter_matches(const char *application_id, const char *display_name, const char *query);

#ifdef __cplusplus
}
#endif
#endif
