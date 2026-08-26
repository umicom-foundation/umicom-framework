/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_slot.c
 *
 * PURPOSE:
 *   Describe child-content slots used by compound semantic components and visual designers.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_slot.h"

#include <string.h>
UmiStatus umi_design_component_slot_init(UmiDesignComponentSlot *slot,const char *id,UmiUiComponentKind accepted_kind,uint16_t minimum_children,uint16_t maximum_children){UmiStatus s;if(slot==NULL||id==NULL||accepted_kind<UMI_UI_COMPONENT_WINDOW||accepted_kind>UMI_UI_COMPONENT_CUSTOM||maximum_children<minimum_children)return UMI_STATUS_INVALID_ARGUMENT;memset(slot,0,sizeof *slot);s=umi_design_copy_text(slot->id,sizeof slot->id,id);if(s!=UMI_STATUS_OK)return s;slot->accepted_kind=accepted_kind;slot->minimum_children=minimum_children;slot->maximum_children=maximum_children;return UMI_STATUS_OK;}
int umi_design_component_slot_accepts_count(const UmiDesignComponentSlot *slot,uint16_t child_count){return slot!=NULL&&child_count>=slot->minimum_children&&child_count<=slot->maximum_children?1:0;}
