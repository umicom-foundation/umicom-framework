/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/shell_designer.h
 *
 * PURPOSE:
 *   Own designer selection and dirty-state for adaptive application-shell regions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_SHELL_DESIGNER_H
#define UMICOM_DESIGNER_ADAPTIVE_SHELL_DESIGNER_H
#include "umicom/designer/adaptive/types.h"
#include "umicom/ui/adaptive/shell_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerShellDesigner {
    UmiAdaptiveShellProfile profile;
    char selected_region_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    uint64_t revision;
    int dirty;
} UmiDesignerShellDesigner;
/* Initialise a shell-designer session from one canonical shell profile. */
UmiStatus umi_designer_shell_designer_init(UmiDesignerShellDesigner *designer,
                                           const UmiAdaptiveShellProfile *profile);
/* Select an authored shell region and mark the session revision. */
UmiStatus umi_designer_shell_designer_select_region(UmiDesignerShellDesigner *designer,
                                                    const char *region_id);

#ifdef __cplusplus
}
#endif
#endif
