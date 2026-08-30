/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/token_registry.c
 *
 * PURPOSE:
 *   Register theme-mode-specific token sets for application and frontend resolution.
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

#include "umicom/ui/design/token_registry.h"

UmiStatus umi_design_token_registry_set(UmiDesignTokenRegistry *registry,UmiDesignThemeMode mode,const UmiDesignTokenSet *set){size_t i;if(registry==NULL||set==NULL||set->id[0]=='\0'||mode<UMI_DESIGN_THEME_LIGHT||mode>UMI_DESIGN_THEME_HIGH_CONTRAST)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<registry->count;++i)if(registry->entries[i].mode==mode){registry->entries[i].set=*set;return UMI_STATUS_OK;}if(registry->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;registry->entries[registry->count].mode=mode;registry->entries[registry->count].set=*set;++registry->count;return UMI_STATUS_OK;}
UmiStatus umi_design_token_registry_get(const UmiDesignTokenRegistry *registry,UmiDesignThemeMode mode,UmiDesignTokenSet *out_set){size_t i;if(registry==NULL||out_set==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<registry->count;++i)if(registry->entries[i].mode==mode){*out_set=registry->entries[i].set;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
