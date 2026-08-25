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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/environment_semantics.h"

#include <ctype.h>
#include <string.h>
UmiCtEnvironmentSemantics umi_ct_environment_semantics_default(UmiCtOperatingSystem os){UmiCtEnvironmentSemantics s={':',true,true,true};if(os==UMI_CT_OS_WINDOWS){s.list_separator=';';s.key_case_sensitive=false;}if(os==UMI_CT_OS_BARE_METAL)s.environment_available=false;return s;}
bool umi_ct_environment_key_equal(const UmiCtEnvironmentSemantics*s,const char*a,const char*b){size_t i;if(s==NULL||a==NULL||b==NULL)return false;if(s->key_case_sensitive)return strcmp(a,b)==0;for(i=0U;a[i]&&b[i];++i)if(tolower((unsigned char)a[i])!=tolower((unsigned char)b[i]))return false;return a[i]=='\0'&&b[i]=='\0';}
