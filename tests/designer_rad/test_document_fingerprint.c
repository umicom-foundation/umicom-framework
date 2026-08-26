/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_document_fingerprint.c
 *
 * PURPOSE:
 *   Validate calculate deterministic FNV-1a fingerprints for designer manifests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/document_fingerprint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDocumentManifest m;uint64_t a,b;CHECK(umi_rad_document_manifest_init(&m)==UMI_STATUS_OK);a=umi_rad_document_fingerprint_compute(&m);b=umi_rad_document_fingerprint_compute(&m);CHECK(a!=0U&&a==b);m.component_count++;CHECK(umi_rad_document_fingerprint_compute(&m)!=a);return 0;}
