/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/search_catalogue.h
 *
 * PURPOSE:
 *   Describe global search source catalogue entries without application-owned registries.
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
#ifndef UMICOM_DESKTOP_CONTROL_SEARCH_CATALOGUE_H
#define UMICOM_DESKTOP_CONTROL_SEARCH_CATALOGUE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SEARCH_CATALOGUE_API_VERSION 1U

typedef struct UmiDesktopSearchCatalogueSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSearchCatalogueSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_search_catalogue_init(UmiDesktopSearchCatalogueSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_search_catalogue_validate(const UmiDesktopSearchCatalogueSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_search_catalogue_set_subject(UmiDesktopSearchCatalogueSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_search_catalogue_set_detail(UmiDesktopSearchCatalogueSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_search_catalogue_set_state(UmiDesktopSearchCatalogueSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_search_catalogue_set_priority(UmiDesktopSearchCatalogueSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_search_catalogue_set_enabled(UmiDesktopSearchCatalogueSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_search_catalogue_same_identity(const UmiDesktopSearchCatalogueSnapshot *left, const UmiDesktopSearchCatalogueSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
