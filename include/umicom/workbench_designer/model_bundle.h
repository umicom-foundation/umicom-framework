/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/model_bundle.h
 *
 * PURPOSE:
 *   Capture one immutable presentation bundle containing the major Layout
 *   Designer projections consumed by frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MODEL_BUNDLE_H
#define UMICOM_WORKBENCH_DESIGNER_MODEL_BUNDLE_H

#include "umicom/workbench_designer/breadcrumbs.h"
#include "umicom/workbench_designer/browser_preview.h"
#include "umicom/workbench_designer/command_palette.h"
#include "umicom/workbench_designer/property_groups.h"
#include "umicom/workbench_designer/snapshot.h"
#include "umicom/workbench_designer/validation_gate.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer model bundle data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerModelBundle {
    UmiWorkbenchDesignerSnapshot service_snapshot;
    UmiWorkbenchDesignerBreadcrumbs breadcrumbs;
    UmiWorkbenchDesignerPropertyGroups property_groups;
    UmiWorkbenchDesignerValidationGate validation_gate;
    UmiWorkbenchDesignerCommandPalette command_palette;
    UmiWorkbenchDesignerBrowserPreview browser_preview;
    uint64_t captured_at_ms;
    uint64_t revision;
} UmiWorkbenchDesignerModelBundle;

/**
 * Initialise workbench designer model bundle from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_model_bundle_init(
    UmiWorkbenchDesignerModelBundle *bundle);
/**
 * Provide the workbench designer model bundle capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_model_bundle_capture(
    UmiWorkbenchDesignerModelBundle *bundle,
    const UmiWorkbenchDesignerController *controller,
    const UmiWorkbenchDesignerKeymap *keymap,
    uint64_t captured_at_ms);

#ifdef __cplusplus
}
#endif

#endif
