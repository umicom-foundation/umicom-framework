/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/control_inventory.h
 *
 * PURPOSE:
 *   Describe the controls in a window without collecting their displayed text.
 *   A copied inventory helps developers find missing or ambiguous test targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_CONTROL_INVENTORY_H
#define UMICOM_UI_CONTROL_INVENTORY_H
#include "umicom/ui/interaction_recording.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_CONTROL_INVENTORY_MAX 4096U
#define UMI_UI_CONTROL_NO_PARENT SIZE_MAX

/** One control, copied in parent-before-child order. A scopeId identifies one
 * captured window; it is local to this inventory, not a persistent window ID.
 * Empty automationId means no directly assigned test ID. A child can inherit
 * its nearest tagged ancestor, as the interaction recorder already does.
 * roleName describes a control type, not its caption or document contents.
 * Flags are exactly 0/1. visible means effective ancestor visibility; mapped
 * additionally means GTK has mapped the widget. Neither proves non-occlusion.
 * A private record has no ID, roleName="private-control" and no children.
 */
typedef struct UmiUiControlRecord {
    size_t parentIndex;
    uint32_t scopeId;
    char automationId[UMI_UI_ID_CAPACITY];
    char roleName[UMI_UI_ID_CAPACITY];
    int visible;
    int mapped;
    int enabled;
    int focused;
    int interactive;
    int privateControl;
} UmiUiControlRecord;

/** Counts describe test instrumentation, not working features or handlers.
 * ambiguousTargets counts tagged records sharing an ID in one scope.
 * unaddressableInteractive counts non-private interactive records with no
 * tagged ancestor. inheritedTargets counts interactive child records that
 * use an ancestor's ID, rather than treating GTK implementation children as
 * missing controls. hidden/unmapped/disabled states are not automatic defects.
 */
typedef struct UmiUiControlInventorySummary {
    size_t controls;
    size_t windows;
    size_t interactive;
    size_t tagged;
    size_t inheritedTargets;
    size_t unaddressableInteractive;
    size_t ambiguousTargets;
    size_t privateControls;
} UmiUiControlInventorySummary;

typedef struct UmiUiControlInventory UmiUiControlInventory;

/** Allocate bounded, empty owner-thread storage (capacity 1..4096).
 * No widgets, filesystem, database or network are accessed. Failure clears
 * *outInventory. See examples/gui_recording/control_checks.c for a complete
 * Notes control map, a duplicate-ID exercise and JSON/HTML export.
 */
UmiStatus UmiUiControlInventoryCreate(size_t capacity,
    UmiUiControlInventory **outInventory);
/** Release copied records. NULL is harmless. */
void UmiUiControlInventoryDestroy(UmiUiControlInventory *inventory);
/** Validate and copy a record; its parent must already exist in the same
 * scope and cannot be private. A root uses UMI_UI_CONTROL_NO_PARENT and a
 * previously unused scopeId. A failed add leaves earlier records unchanged.
 * Duplicate IDs are retained for inspection, not silently renamed or dropped.
 */
UmiStatus UmiUiControlInventoryAdd(UmiUiControlInventory *inventory,
    const UmiUiControlRecord *record);
/** Return retained record count; NULL returns zero. */
size_t UmiUiControlInventoryCount(const UmiUiControlInventory *inventory);
/** Copy one zero-based record. The caller owns its output storage. */
UmiStatus UmiUiControlInventoryAt(const UmiUiControlInventory *inventory,
    size_t index, UmiUiControlRecord *outRecord);
/** Find the nearest tagged ancestor, including the record itself. A private
 * record or untagged root returns NOT_FOUND. Failure sets *outIndex to the
 * NO_PARENT sentinel; no widget is activated or focused. */
UmiStatus UmiUiControlInventoryTarget(const UmiUiControlInventory *inventory,
    size_t index, size_t *outIndex);
/** Compute instrumentation counts. No count is an application pass result. */
UmiStatus UmiUiControlInventorySummarise(const UmiUiControlInventory *inventory,
    UmiUiControlInventorySummary *outSummary);
/** Stream a read-only control map through the same bounded export sink used
 * by recordings. The caller closes its stream. Private contents and widget
 * values are absent. Do not mutate/destroy the inventory in the callback.
 * A write error stops later output; exports never perform I/O themselves.
 */
UmiStatus UmiUiControlInventoryWriteJson(const UmiUiControlInventory *inventory,
    UmiUiRecordingWriteFn write, void *context);
UmiStatus UmiUiControlInventoryWriteHtml(const UmiUiControlInventory *inventory,
    UmiUiRecordingWriteFn write, void *context);
#ifdef __cplusplus
}
#endif
#endif
