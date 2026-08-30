/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_descriptor.c
 *
 * PURPOSE:
 *   Describe a semantic component using canonical Umicom component kinds plus design-system metadata.
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

#include "umicom/ui/design/component_descriptor.h"

#include <string.h>
UmiStatus umi_design_component_descriptor_init(UmiDesignComponentDescriptor *descriptor,const char *id,const char *display_name,UmiUiComponentKind kind,UmiDesignSemanticRole default_role,int interactive){UmiStatus s;if(descriptor==NULL||id==NULL||display_name==NULL||kind<UMI_UI_COMPONENT_WINDOW||kind>UMI_UI_COMPONENT_CUSTOM||default_role<UMI_DESIGN_ROLE_NEUTRAL||default_role>UMI_DESIGN_ROLE_ACCENT)return UMI_STATUS_INVALID_ARGUMENT;memset(descriptor,0,sizeof *descriptor);s=umi_design_copy_text(descriptor->id,sizeof descriptor->id,id);if(s!=UMI_STATUS_OK)return s;s=umi_design_copy_text(descriptor->display_name,sizeof descriptor->display_name,display_name);if(s!=UMI_STATUS_OK)return s;descriptor->kind=kind;descriptor->default_role=default_role;descriptor->interactive=interactive?1:0;return UMI_STATUS_OK;}
int umi_design_component_descriptor_valid(const UmiDesignComponentDescriptor *descriptor){return descriptor!=NULL&&descriptor->id[0]!='\0'&&descriptor->display_name[0]!='\0'&&descriptor->kind>=UMI_UI_COMPONENT_WINDOW&&descriptor->kind<=UMI_UI_COMPONENT_CUSTOM?1:0;}
