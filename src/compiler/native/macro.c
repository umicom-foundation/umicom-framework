/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/macro.c
 *
 * PURPOSE:
 *   Describe object-like and function-like preprocessor macros with stable fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro.h"
#include <ctype.h>
#include <string.h>
/* Provide the valid name operation used by this module and its client applications. */
static bool valid_name(const char *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||(*s!='_'&&isalpha((unsigned char)*s)==0)) return false; /* Visit each bounded item once so every record receives the same rule. */ for(s++;*s!='\0';s++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(*s!='_'&&isalnum((unsigned char)*s)==0) return false; return true; }
/*
 * Provide the nc macro compute fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_macro_compute_fingerprint(const UmiNativeMacro *m){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL) return 0U; uint64_t h=umi_nc_hash_text(m->name)^umi_nc_hash_text(m->replacement); /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<m->parameter_count;i++) h^=umi_nc_hash_text(m->parameters[i])+(h<<6U)+(h>>2U); h^=(uint64_t)m->function_like<<1U; h^=(uint64_t)m->variadic<<2U; return h; }
/*
 * Initialise nc macro from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_macro_init(UmiNativeMacro *m,const char *name,const char *replacement,bool function_like){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||name==NULL||replacement==NULL||!valid_name(name)) return UMI_STATUS_INVALID_ARGUMENT; memset(m,0,sizeof(*m)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(m->name,sizeof(m->name),name)!=UMI_STATUS_OK||umi_nc_copy_text(m->replacement,sizeof(m->replacement),replacement)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; m->function_like=function_like; m->fingerprint=umi_nc_macro_compute_fingerprint(m); return UMI_STATUS_OK; }
/*
 * Provide the nc macro add parameter operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_add_parameter(UmiNativeMacro *m,const char *name){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||!valid_name(name)) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->parameter_count>=UMI_NC_MAX_MACRO_PARAMETERS) return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(m->parameters[m->parameter_count],sizeof(m->parameters[0]),name)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; m->parameter_count++; m->function_like=true; m->fingerprint=umi_nc_macro_compute_fingerprint(m); return UMI_STATUS_OK; }
/* Check that nc macro satisfies its contract before another service relies on it. */
UmiStatus umi_nc_macro_validate(const UmiNativeMacro *m){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||!valid_name(m->name)) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!m->function_like&&m->parameter_count!=0U) return UMI_STATUS_INVALID_STATE; return UMI_STATUS_OK; }
