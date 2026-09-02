/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_document_manifest.c
 *
 * PURPOSE:
 *   Validate summarise the pages, forms, components and bindings in a visual document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/document_manifest.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDocumentManifest item;CHECK(umi_rad_document_manifest_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_document_manifest_is_valid(&item));return 0;}
