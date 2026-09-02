/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/layout_catalogue.h
 *
 * PURPOSE:
 *   Publish the public layout catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_CATALOGUE_H
#define UMICOM_DESKTOP_LAYOUT_CATALOGUE_H

#include "umicom/base/status.h"
#include "umicom/desktop/types.h"

/**
 * Represent the desktop layout catalogue data shared with callers of this public contract.
 */
typedef struct UmiDesktopLayoutCatalogue {
    UmiDesktopLayout layouts[UMI_DESKTOP_MAX_LAYOUTS];
    size_t count;
    char active_layout_id[UMI_DESKTOP_ID_CAPACITY];
    uint64_t revision;
} UmiDesktopLayoutCatalogue;

/**
 * Initialise desktop layout catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_layout_catalogue_init(UmiDesktopLayoutCatalogue *catalogue);
/**
 * Add desktop layout catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_desktop_layout_catalogue_add(
    UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopLayout *layout);
/**
 * Remove desktop layout catalogue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_desktop_layout_catalogue_remove(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
/**
 * Provide the desktop layout catalogue activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_layout_catalogue_activate(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
/**
 * Provide the desktop layout catalogue clone operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_catalogue_clone(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *source_layout_id,
    const char *layout_id,
    const char *name);
/**
 * Provide the desktop layout catalogue seed professional operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_layout_catalogue_seed_professional(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id);
/**
 * Find desktop layout catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesktopLayout *umi_desktop_layout_catalogue_find(
    const UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
/**
 * Provide the desktop layout catalogue active mutable operation used by this module and
 * its client applications.
 */
UmiDesktopLayout *umi_desktop_layout_catalogue_active_mutable(
    UmiDesktopLayoutCatalogue *catalogue);
/**
 * Provide the desktop layout catalogue active operation used by this module and its client
 * applications.
 */
const UmiDesktopLayout *umi_desktop_layout_catalogue_active(
    const UmiDesktopLayoutCatalogue *catalogue);

#endif
