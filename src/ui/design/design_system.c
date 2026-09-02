/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/design_system.c
 *
 * PURPOSE:
 *   Aggregate typed tokens, palettes and semantic component descriptors into one canonical reusable design system.
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

#include "umicom/ui/design/design_system.h"

#include <string.h>
/*
 * Initialise design system from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_design_system_init(UmiDesignSystem *system,const char *id,UmiDesignThemeMode theme,UmiDesignDensity density){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(system==NULL||id==NULL||theme<UMI_DESIGN_THEME_LIGHT||theme>UMI_DESIGN_THEME_HIGH_CONTRAST||density<UMI_DESIGN_DENSITY_COMPACT||density>UMI_DESIGN_DENSITY_TOUCH)return UMI_STATUS_INVALID_ARGUMENT;memset(system,0,sizeof *system);s=umi_design_copy_text(system->id,sizeof system->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_design_token_set_init(&system->tokens,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;system->default_theme=theme;system->default_density=density;return UMI_STATUS_OK;}
/*
 * Provide the design system ready operation used by this module and its client
 * applications.
 */
int umi_design_system_ready(const UmiDesignSystem *system){return system!=NULL&&system->id[0]!='\0'&&system->default_theme>=UMI_DESIGN_THEME_LIGHT&&system->default_theme<=UMI_DESIGN_THEME_HIGH_CONTRAST&&system->default_density>=UMI_DESIGN_DENSITY_COMPACT&&system->default_density<=UMI_DESIGN_DENSITY_TOUCH?1:0;}
/*
 * Provide the design system touch operation used by this module and its client
 * applications.
 */
void umi_design_system_touch(UmiDesignSystem *system){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(system!=NULL)++system->revision;}
