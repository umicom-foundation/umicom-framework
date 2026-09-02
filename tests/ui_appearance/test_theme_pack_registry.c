/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_pack_registry.c
 *
 * PURPOSE:
 *   Verify catalogue versioned theme packs while preserving stable identity and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAppearanceThemePackRegistry registry; UmiAppearanceThemePack item; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_theme_pack_registry_init(&registry)!=UMI_STATUS_OK) return 1; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_theme_pack_init(&item)!=UMI_STATUS_OK) return 2; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_theme_pack_registry_upsert(&registry,&item)!=UMI_STATUS_OK) return 3; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_theme_pack_registry_count(&registry)!=1U) return 4; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_theme_pack_registry_find(&registry,item.pack_id)==NULL) return 5; return 0; }
