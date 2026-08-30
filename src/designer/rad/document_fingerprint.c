/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/document_fingerprint.c
 *
 * PURPOSE:
 *   Calculate deterministic FNV-1a fingerprints for designer manifests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/document_fingerprint.h"
static uint64_t mix(uint64_t h,uint64_t v){unsigned i;for(i=0U;i<8U;i++){h^=(v>>(i*8U))&UINT64_C(255);h*=UINT64_C(1099511628211);}return h;}
uint64_t umi_rad_document_fingerprint_compute(const UmiRadDocumentManifest *m){const unsigned char *p;uint64_t h=UINT64_C(1469598103934665603);if(m==NULL)return 0U;p=(const unsigned char*)m->application_id;while(*p!=0U){h^=(uint64_t)*p++;h*=UINT64_C(1099511628211);}h=mix(h,(uint64_t)m->page_count);h=mix(h,(uint64_t)m->form_count);h=mix(h,(uint64_t)m->component_count);h=mix(h,(uint64_t)m->binding_count);h=mix(h,m->revision);return h;}
