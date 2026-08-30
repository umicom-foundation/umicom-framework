/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_layout.c
 *
 * PURPOSE:
 *   Define common row, column, stack and grid layout semantics for compound components.
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

#include "umicom/ui/design/component_layout.h"

UmiStatus umi_design_component_layout_init(UmiDesignComponentLayout *layout,UmiDesignComponentLayoutKind kind,uint16_t columns,double gap,int wrap){if(layout==NULL||kind<UMI_DESIGN_LAYOUT_ROW||kind>UMI_DESIGN_LAYOUT_GRID||!umi_design_number_valid(gap)||gap<0.0||(kind==UMI_DESIGN_LAYOUT_GRID&&columns==0U))return UMI_STATUS_INVALID_ARGUMENT;layout->kind=kind;layout->columns=kind==UMI_DESIGN_LAYOUT_GRID?columns:1U;layout->gap=gap;layout->wrap=wrap?1:0;return UMI_STATUS_OK;}
