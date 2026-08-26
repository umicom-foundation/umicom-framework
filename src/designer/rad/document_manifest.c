/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/document_manifest.c
 *
 * PURPOSE:
 *   Summarise the pages, forms, components and bindings in a visual document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/document_manifest.h"
#include <string.h>
UmiStatus umi_rad_document_manifest_init(UmiRadDocumentManifest *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "document_manifest");
    return UMI_STATUS_OK;
}
int umi_rad_document_manifest_is_valid(const UmiRadDocumentManifest *item){if(item==NULL)return 0;return umi_rad_id_valid(item->application_id);}
