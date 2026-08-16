/*-----------------------------------------------------------------------------
 * Umicom Framework | Professional desktop layout catalogue
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_CATALOGUE_H
#define UMICOM_DESKTOP_LAYOUT_CATALOGUE_H

#include "umicom/base/status.h"
#include "umicom/desktop/types.h"

typedef struct UmiDesktopLayoutCatalogue {
    UmiDesktopLayout layouts[UMI_DESKTOP_MAX_LAYOUTS];
    size_t count;
    char active_layout_id[UMI_DESKTOP_ID_CAPACITY];
    uint64_t revision;
} UmiDesktopLayoutCatalogue;

void umi_desktop_layout_catalogue_init(UmiDesktopLayoutCatalogue *catalogue);
UmiStatus umi_desktop_layout_catalogue_add(
    UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopLayout *layout);
UmiStatus umi_desktop_layout_catalogue_remove(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
UmiStatus umi_desktop_layout_catalogue_activate(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
UmiStatus umi_desktop_layout_catalogue_clone(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *source_layout_id,
    const char *layout_id,
    const char *name);
UmiStatus umi_desktop_layout_catalogue_seed_professional(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id);
const UmiDesktopLayout *umi_desktop_layout_catalogue_find(
    const UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id);
UmiDesktopLayout *umi_desktop_layout_catalogue_active_mutable(
    UmiDesktopLayoutCatalogue *catalogue);
const UmiDesktopLayout *umi_desktop_layout_catalogue_active(
    const UmiDesktopLayoutCatalogue *catalogue);

#endif
