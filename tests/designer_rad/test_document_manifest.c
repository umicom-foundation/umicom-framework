/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_document_manifest.c
 *
 * PURPOSE:
 *   Validate summarise the pages, forms, components and bindings in a visual document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/document_manifest.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDocumentManifest item;CHECK(umi_rad_document_manifest_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_document_manifest_is_valid(&item));return 0;}
