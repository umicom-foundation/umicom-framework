/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_pack_registry.c
 *
 * PURPOSE:
 *   Verify catalogue versioned theme packs while preserving stable identity and revision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack_registry.h"
int main(void) { UmiAppearanceThemePackRegistry registry; UmiAppearanceThemePack item; if(umi_appearance_theme_pack_registry_init(&registry)!=UMI_STATUS_OK) return 1; if(umi_appearance_theme_pack_init(&item)!=UMI_STATUS_OK) return 2; if(umi_appearance_theme_pack_registry_upsert(&registry,&item)!=UMI_STATUS_OK) return 3; if(umi_appearance_theme_pack_registry_count(&registry)!=1U) return 4; if(umi_appearance_theme_pack_registry_find(&registry,item.pack_id)==NULL) return 5; return 0; }
