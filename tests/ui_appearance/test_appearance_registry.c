/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_registry.c
 *
 * PURPOSE:
 *   Verify store bounded resolved appearance profiles for applications, workspaces and previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_registry.h"
int main(void) { UmiAppearanceAppearanceRegistry registry; UmiAppearanceAppearanceProfile item; if(umi_appearance_registry_init(&registry)!=UMI_STATUS_OK) return 1; if(umi_appearance_profile_init(&item)!=UMI_STATUS_OK) return 2; if(umi_appearance_registry_upsert(&registry,&item)!=UMI_STATUS_OK) return 3; if(umi_appearance_registry_count(&registry)!=1U) return 4; if(umi_appearance_registry_find(&registry,item.profile_id)==NULL) return 5; return 0; }
