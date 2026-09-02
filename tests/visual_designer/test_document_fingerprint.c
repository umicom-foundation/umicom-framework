/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_document_fingerprint.c
 *
 * PURPOSE:
 *   Validate calculate deterministic FNV-1a fingerprints for designer manifests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/document_fingerprint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDocumentManifest m;uint64_t a,b;CHECK(umi_rad_document_manifest_init(&m)==UMI_STATUS_OK);a=umi_rad_document_fingerprint_compute(&m);b=umi_rad_document_fingerprint_compute(&m);CHECK(a!=0U&&a==b);m.component_count++;CHECK(umi_rad_document_fingerprint_compute(&m)!=a);return 0;}
