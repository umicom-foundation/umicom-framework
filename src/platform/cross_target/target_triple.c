/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_triple.c
 *
 * PURPOSE:
 *   Parse and normalise runtime target triples used to bridge platform and existing compiler/toolchain contracts.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_triple.h"

#include "umicom/platform/cross_target/target_architecture.h"
#include "umicom/platform/cross_target/target_operating_system.h"
#include "umicom/platform/cross_target/target_environment.h"
#include <stdio.h>
#include <string.h>
/*
 * Read ct target triple into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_ct_target_triple_parse(const char *triple,UmiCtTarget *out){char tmp[UMI_CT_TRIPLE_CAPACITY];char *a,*v,*o,*e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(triple==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_copy(tmp,sizeof(tmp),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;memset(out,0,sizeof(*out));out->structure_size=(uint32_t)sizeof(*out);out->api_version=UMI_CT_API_VERSION;a=strtok(tmp,"-");v=strtok(NULL,"-");o=strtok(NULL,"-");e=strtok(NULL,"-");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||v==NULL||o==NULL)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_architecture_parse(a,&out->architecture)!=UMI_STATUS_OK)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_operating_system_parse(o,&out->operating_system)!=UMI_STATUS_OK)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL){out->environment=out->operating_system==UMI_CT_OS_WINDOWS?UMI_CT_ENV_MSVC:(out->operating_system==UMI_CT_OS_UMICOM?UMI_CT_ENV_UMICOM:UMI_CT_ENV_GNU);}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_environment_parse(e,&out->environment)!=UMI_STATUS_OK)return UMI_STATUS_PARSE_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_copy(out->vendor,sizeof(out->vendor),v)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ct_copy(out->triple,sizeof(out->triple),triple)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;out->pointer_bits=umi_ct_architecture_pointer_bits(out->architecture);out->endian=UMI_CT_ENDIAN_LITTLE;return UMI_STATUS_OK;}
/*
 * Provide the ct target triple format operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_target_triple_format(const UmiCtTarget *t,char *out,size_t cap){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,cap,"%s-%s-%s-%s",umi_ct_architecture_text(t->architecture),t->vendor,umi_ct_operating_system_text(t->operating_system),umi_ct_environment_text(t->environment));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
/*
 * Provide the ct target compatible operation used by this module and its client
 * applications.
 */
bool umi_ct_target_compatible(const UmiCtTarget *a,const UmiCtTarget *b){return a!=NULL&&b!=NULL&&a->architecture==b->architecture&&a->operating_system==b->operating_system&&a->environment==b->environment&&a->pointer_bits==b->pointer_bits&&a->endian==b->endian;}
