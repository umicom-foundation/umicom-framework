/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/self_host_gate.c
 *
 * PURPOSE:
 *   Evaluate evidence for moving Umicc from experimental bootstrap use toward controlled self-hosting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/self_host_gate.h"
UmiNativeSelfHostAssessment umi_nc_self_host_assess(const UmiNativeSelfHostEvidence *e){UmiNativeSelfHostAssessment a={0};if(e==NULL)return a;const bool caps[]={e->lexer,e->preprocessor,e->parser,e->semantics,e->ir,e->optimizer,e->codegen,e->object_writer,e->linker_bridge,e->diagnostics};a.capability_total=sizeof(caps)/sizeof(caps[0]);for(size_t i=0U;i<a.capability_total;i++)if(caps[i])a.capability_count++;a.coverage=a.capability_total==0U?0.0:(double)a.capability_count/(double)a.capability_total;bool tests=e->required_tests!=0U&&e->passing_tests>=e->required_tests;if(a.coverage>=0.5)a.stage=UMI_NC_SELFHOST_BOOTSTRAP;if(a.coverage>=0.9&&tests&&e->blocker_count==0U)a.stage=UMI_NC_SELFHOST_CANDIDATE;if(a.coverage>=1.0&&tests&&e->blocker_count==0U){a.stage=UMI_NC_SELFHOST_SELFHOST;a.ready_for_default=true;}return a;}
