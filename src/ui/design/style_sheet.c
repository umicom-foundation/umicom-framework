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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/style_sheet.h"

/* Add design style sheet only after its inputs and available capacity have been checked. */
UmiStatus umi_design_style_sheet_add(UmiDesignStyleSheet *sheet,const UmiDesignStyleRule *rule){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sheet==NULL||rule==NULL||rule->component_kind<UMI_UI_COMPONENT_WINDOW||rule->component_kind>UMI_UI_COMPONENT_CUSTOM)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sheet->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;sheet->rules[sheet->count]=*rule;++sheet->count;return UMI_STATUS_OK;}
/*
 * Find design style sheet while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_design_style_sheet_find(const UmiDesignStyleSheet *sheet,UmiUiComponentKind kind,UmiDesignSemanticRole role,UmiDesignInteractionState state,UmiDesignStyleRule *out_rule){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sheet==NULL||out_rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<sheet->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sheet->rules[i].component_kind==kind&&sheet->rules[i].role==role&&sheet->rules[i].state==state){*out_rule=sheet->rules[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
