/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/style_sheet.c
 *
 * PURPOSE:
 *   Maintain a bounded stylesheet of semantic component rules for renderer-independent style resolution.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/style_sheet.h"

UmiStatus umi_design_style_sheet_add(UmiDesignStyleSheet *sheet,const UmiDesignStyleRule *rule){if(sheet==NULL||rule==NULL||rule->component_kind<UMI_UI_COMPONENT_WINDOW||rule->component_kind>UMI_UI_COMPONENT_CUSTOM)return UMI_STATUS_INVALID_ARGUMENT;if(sheet->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;sheet->rules[sheet->count]=*rule;++sheet->count;return UMI_STATUS_OK;}
UmiStatus umi_design_style_sheet_find(const UmiDesignStyleSheet *sheet,UmiUiComponentKind kind,UmiDesignSemanticRole role,UmiDesignInteractionState state,UmiDesignStyleRule *out_rule){size_t i;if(sheet==NULL||out_rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<sheet->count;++i)if(sheet->rules[i].component_kind==kind&&sheet->rules[i].role==role&&sheet->rules[i].state==state){*out_rule=sheet->rules[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
