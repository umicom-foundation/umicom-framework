/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/environment_semantics.c
 *
 * PURPOSE:
 *   Describe process environment key comparison, list separators and encoding expectations across targets.
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

#include "umicom/platform/cross_target/environment_semantics.h"

#include <ctype.h>
#include <string.h>
/*
 * Provide the ct environment semantics default operation used by this module and its
 * client applications.
 */
UmiCtEnvironmentSemantics umi_ct_environment_semantics_default(UmiCtOperatingSystem os){UmiCtEnvironmentSemantics s={':',true,true,true};/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_WINDOWS){s.list_separator=';';s.key_case_sensitive=false;}/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_BARE_METAL)s.environment_available=false;return s;}
/*
 * Provide the ct environment key equal operation used by this module and its client
 * applications.
 */
bool umi_ct_environment_key_equal(const UmiCtEnvironmentSemantics*s,const char*a,const char*b){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||a==NULL||b==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->key_case_sensitive)return strcmp(a,b)==0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;a[i]&&b[i];++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tolower((unsigned char)a[i])!=tolower((unsigned char)b[i]))return false;return a[i]=='\0'&&b[i]=='\0';}
