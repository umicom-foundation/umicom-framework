/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_slot.h
 *
 * PURPOSE:
 *   Describe child-content slots used by compound semantic components and visual designers.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_SLOT_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_SLOT_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignComponentSlot { char id[UMI_DESIGN_ID_CAPACITY]; UmiUiComponentKind accepted_kind; uint16_t minimum_children; uint16_t maximum_children; } UmiDesignComponentSlot;
/* Initialise a compound-component slot with explicit child cardinality. */
UmiStatus umi_design_component_slot_init(UmiDesignComponentSlot *slot, const char *id, UmiUiComponentKind accepted_kind, uint16_t minimum_children, uint16_t maximum_children);
/* Return one when a proposed child count satisfies the slot cardinality. */
int umi_design_component_slot_accepts_count(const UmiDesignComponentSlot *slot, uint16_t child_count);

#ifdef __cplusplus
}
#endif

#endif
