/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/breakpoint_designer.h
 *
 * PURPOSE:
 *   Own adaptive breakpoint selection and catalogue editing for visual authoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_BREAKPOINT_DESIGNER_H
#define UMICOM_DESIGNER_ADAPTIVE_BREAKPOINT_DESIGNER_H
#include "umicom/designer/adaptive/types.h"
#include "umicom/ui/adaptive/breakpoint_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBreakpointDesigner {
    UmiAdaptiveBreakpointCatalogue catalogue;
    char selected_breakpoint_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    uint64_t revision;
} UmiDesignerBreakpointDesigner;
/* Initialise an empty breakpoint-designer session. */
void umi_designer_breakpoint_designer_init(UmiDesignerBreakpointDesigner *designer);
/* Add and select one canonical Design-System breakpoint. */
UmiStatus umi_designer_breakpoint_designer_add(UmiDesignerBreakpointDesigner *designer,
                                               const UmiDesignBreakpoint *breakpoint);

#ifdef __cplusplus
}
#endif
#endif
