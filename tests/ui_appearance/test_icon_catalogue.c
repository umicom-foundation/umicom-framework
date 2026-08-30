/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_catalogue.c
 *
 * PURPOSE:
 *   Verify catalogue reusable semantic icons independently of GTK, Qt or browser asset APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_catalogue.h"
int main(void) { UmiAppearanceIconCatalogue registry; UmiAppearanceIconDescriptor item; if(umi_appearance_icon_catalogue_init(&registry)!=UMI_STATUS_OK) return 1; if(umi_appearance_icon_descriptor_init(&item)!=UMI_STATUS_OK) return 2; if(umi_appearance_icon_catalogue_upsert(&registry,&item)!=UMI_STATUS_OK) return 3; if(umi_appearance_icon_catalogue_count(&registry)!=1U) return 4; if(umi_appearance_icon_catalogue_find(&registry,item.icon_id)==NULL) return 5; return 0; }
