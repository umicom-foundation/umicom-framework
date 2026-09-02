/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/document_manifest.c
 *
 * PURPOSE:
 *   Summarise the pages, forms, components and bindings in a visual document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/document_manifest.h"
#include <string.h>
/*
 * Initialise rad document manifest from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_document_manifest_init(UmiRadDocumentManifest *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "document_manifest");
    return UMI_STATUS_OK;
}
/*
 * Check that rad document manifest satisfies its contract before another service relies on
 * it.
 */
int umi_rad_document_manifest_is_valid(const UmiRadDocumentManifest *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->application_id);}
