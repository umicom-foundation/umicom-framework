/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/marquee.h
 *
 * PURPOSE:
 *   Represent rectangular marquee selection and apply containment or
 *   intersection policies to canvas items.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MARQUEE_H
#define UMICOM_WORKBENCH_DESIGNER_MARQUEE_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer marquee mode values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerMarqueeMode {
    UMI_WORKBENCH_DESIGNER_MARQUEE_CONTAIN = 1,
    UMI_WORKBENCH_DESIGNER_MARQUEE_INTERSECT = 2
} UmiWorkbenchDesignerMarqueeMode;

/**
 * Represent the workbench designer marquee data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMarquee {
    UmiWorkbenchDesignerPoint origin;
    UmiWorkbenchDesignerPoint current;
    UmiWorkbenchDesignerRect bounds;
    UmiWorkbenchDesignerMarqueeMode mode;
    bool active;
    bool extend_selection;
    uint64_t revision;
} UmiWorkbenchDesignerMarquee;

/**
 * Initialise workbench designer marquee from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_marquee_init(UmiWorkbenchDesignerMarquee *marquee);
/**
 * Provide the workbench designer marquee begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_marquee_begin(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint origin,
    UmiWorkbenchDesignerMarqueeMode mode,
    bool extend_selection);
/**
 * Provide the workbench designer marquee update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_marquee_update(
    UmiWorkbenchDesignerMarquee *marquee,
    UmiWorkbenchDesignerPoint current);
/**
 * Perform workbench designer marquee through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_marquee_apply(
    const UmiWorkbenchDesignerMarquee *marquee,
    const UmiWorkbenchDesignerCanvas *canvas,
    UmiWorkbenchDesignerSelection *selection);
/**
 * Provide the workbench designer marquee cancel operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_marquee_cancel(UmiWorkbenchDesignerMarquee *marquee);

#ifdef __cplusplus
}
#endif

#endif
