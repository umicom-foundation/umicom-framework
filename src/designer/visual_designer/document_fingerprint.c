/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/document_fingerprint.c
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
#include "umicom/designer/visual_designer/document_fingerprint.h"
/* Provide the mix operation used by this module and its client applications. */
static uint64_t mix(uint64_t h,uint64_t v){unsigned i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<8U;i++){h^=(v>>(i*8U))&UINT64_C(255);h*=UINT64_C(1099511628211);}return h;}
/*
 * Provide the visual designer document fingerprint compute operation used by this module and its
 * client applications.
 */
uint64_t umi_rad_document_fingerprint_compute(const UmiRadDocumentManifest *m){const unsigned char *p;uint64_t h=UINT64_C(1469598103934665603);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return 0U;p=(const unsigned char*)m->application_id;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p!=0U){h^=(uint64_t)*p++;h*=UINT64_C(1099511628211);}h=mix(h,(uint64_t)m->page_count);h=mix(h,(uint64_t)m->form_count);h=mix(h,(uint64_t)m->component_count);h=mix(h,(uint64_t)m->binding_count);h=mix(h,m->revision);return h;}
