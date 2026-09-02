/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_registry.c
 *
 * PURPOSE:
 *   Verify store bounded resolved appearance profiles for applications, workspaces and previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAppearanceAppearanceRegistry registry; UmiAppearanceAppearanceProfile item; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_registry_init(&registry)!=UMI_STATUS_OK) return 1; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_profile_init(&item)!=UMI_STATUS_OK) return 2; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_registry_upsert(&registry,&item)!=UMI_STATUS_OK) return 3; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_registry_count(&registry)!=1U) return 4; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_registry_find(&registry,item.profile_id)==NULL) return 5; return 0; }
