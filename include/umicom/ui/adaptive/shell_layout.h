/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/shell_layout.h
 *
 * PURPOSE:
 *   Hold a bounded semantic application-shell layout shared by all frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SHELL_LAYOUT_H
#define UMICOM_UI_ADAPTIVE_SHELL_LAYOUT_H
#include "umicom/ui/adaptive/shell_region.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive shell layout data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveShellLayout {
    char layout_id[UMI_ADAPTIVE_ID_CAPACITY];
    UmiAdaptiveShellRegion regions[UMI_ADAPTIVE_MAX_ITEMS];
    size_t count;
} UmiAdaptiveShellLayout;
/* Initialise an empty semantic shell layout. */
UmiStatus umi_adaptive_shell_layout_init(UmiAdaptiveShellLayout *layout, const char *layout_id);
/* Add a uniquely identified shell region by value. */
UmiStatus umi_adaptive_shell_layout_add(UmiAdaptiveShellLayout *layout,
                                        const UmiAdaptiveShellRegion *region);
/* Find a shell region by stable region identifier. */
const UmiAdaptiveShellRegion *umi_adaptive_shell_layout_find(const UmiAdaptiveShellLayout *layout,
                                                             const char *region_id);

#ifdef __cplusplus
}
#endif
#endif
