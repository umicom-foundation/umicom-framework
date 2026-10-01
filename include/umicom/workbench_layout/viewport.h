/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/viewport.h
 * PURPOSE: Describe and transform bounded workbench viewport geometry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Resolve an existing semantic layout into bounded logical-pixel rectangles.
 * This is a view projection, not a second document, persistence or docking model.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WORKBENCH_LAYOUT_VIEWPORT_H
#define UMICOM_WORKBENCH_LAYOUT_VIEWPORT_H
#include "umicom/workbench_layout/document.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_VIEWPORT_MAX_DEPTH 64U

typedef struct UmiWorkbenchViewportOptions {
    int32_t splitGap;
    int32_t tabHeight;
} UmiWorkbenchViewportOptions;

typedef struct UmiWorkbenchViewportSlot {
    UmiWorkbenchLayoutRect bounds;
    UmiWorkbenchLayoutRect decoration; /* splitter or tab strip; zero otherwise */
    UmiWorkbenchLayoutSize minimum;
    size_t activeChild; /* position in child_indices, never a node index */
    bool visible;
    bool minimumViolated;
} UmiWorkbenchViewportSlot;

typedef struct UmiWorkbenchViewportPlan {
    size_t nodeCount;
    size_t focusCount;
    size_t minimumViolations;
    UmiWorkbenchLayoutSize minimum;
    UmiWorkbenchViewportSlot slots[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t focusOrder[UMI_WORKBENCH_LAYOUT_MAX_NODES];
} UmiWorkbenchViewportPlan;

typedef struct UmiWorkbenchViewportDiagnostic {
    size_t nodeIndex;
    UmiStatus status;
    char message[192];
} UmiWorkbenchViewportDiagnostic;

UmiWorkbenchViewportOptions UmiWorkbenchViewportDefaults(void);
/* Pure, allocation-free and re-entrant for distinct outputs. The caller keeps
 * the canonical document stable during the call. Inputs and outputs must not
 * overlap. On failure outPlan is zeroed; no partially valid plan is published.
 *
 * Supported: one window (or floating-window root), nested binary splits,
 * tab groups, and panel/editor/empty leaves. Separate top-level windows must be
 * projected separately; nested windows return UNAVAILABLE. All nodes must be
 * reachable exactly once, including hidden and inactive branches. UTF-8 strings,
 * child indices, parent links, finite ratios and bounded arithmetic are checked.
 *
 * Units are logical pixels. DPI conversion remains with the toolkit. Hidden
 * children consume no gap. AUTO currently means visible. A hidden active tab
 * falls back to the first visible child WITHOUT editing the document. Minimum
 * sizes clamp a feasible split; insufficient space produces an explicit flag,
 * never negative rectangles. Preferred sizes and monitor placement are not used.
 */
UmiStatus UmiWorkbenchViewportBuild(const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutRect viewport, const UmiWorkbenchViewportOptions *options,
    UmiWorkbenchViewportPlan *outPlan, UmiWorkbenchViewportDiagnostic *diagnostic);
/* Return the next non-empty visible leaf in semantic tree order. Unknown or
 * absent currentNode starts at the first (last for reverse). No widgets focused.
 */
size_t UmiWorkbenchViewportNextFocus(const UmiWorkbenchViewportPlan *plan,
    size_t currentNode, bool reverse);
#ifdef __cplusplus
}
#endif
#endif
