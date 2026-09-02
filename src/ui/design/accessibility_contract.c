/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/accessibility_contract.c
 *
 * PURPOSE:
 *   Capture keyboard, focus, labelling and minimum-target accessibility requirements per component.
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

#include "umicom/ui/design/accessibility_contract.h"

#include <string.h>
/*
 * Initialise design accessibility contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_accessibility_contract_init(UmiDesignAccessibilityContract *contract,const char *accessible_name,int32_t min_target_width,int32_t min_target_height){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(contract==NULL||accessible_name==NULL||min_target_width<=0||min_target_height<=0)return UMI_STATUS_INVALID_ARGUMENT;memset(contract,0,sizeof *contract);s=umi_design_copy_text(contract->accessible_name,sizeof contract->accessible_name,accessible_name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;contract->keyboard_operable=1;contract->focus_visible=1;contract->screen_reader_named=1;contract->min_target_width=min_target_width;contract->min_target_height=min_target_height;return UMI_STATUS_OK;}
/*
 * Provide the design accessibility contract passes operation used by this module and its
 * client applications.
 */
int umi_design_accessibility_contract_passes(const UmiDesignAccessibilityContract *contract){return contract!=NULL&&contract->accessible_name[0]!='\0'&&contract->keyboard_operable&&contract->focus_visible&&contract->screen_reader_named&&contract->min_target_width>=24&&contract->min_target_height>=24?1:0;}
