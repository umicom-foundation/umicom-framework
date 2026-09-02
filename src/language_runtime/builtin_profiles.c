/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/builtin_profiles.c
 *
 * PURPOSE:
 *   Enumerate/register server profiles and resolve comma-delimited language coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/builtin_profiles.h"
#include <ctype.h>
#include <string.h>
#include "umicom/language_runtime/profiles/profiles.h"
typedef const UmiLanguageServerProfile*(*F)(void);static const F FS[]={ umi_language_runtime_profile_clangd,
    umi_language_runtime_profile_pyright,
    umi_language_runtime_profile_rust_analyzer,
    umi_language_runtime_profile_zls,
    umi_language_runtime_profile_jdtls,
    umi_language_runtime_profile_typescript,
    umi_language_runtime_profile_bash,
    umi_language_runtime_profile_cmake,
    umi_language_runtime_profile_marksman,
    umi_language_runtime_profile_html,
    umi_language_runtime_profile_css,
    umi_language_runtime_profile_json,
    umi_language_runtime_profile_yaml,
    umi_language_runtime_profile_php,
    umi_language_runtime_profile_sqls };
/* Provide the has operation used by this module and its client applications. */
static int has(const char*l,const char*x){const char*p=l;size_t xn=strlen(x);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(p&&*p){const char*e;size_t n;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p==','||isspace((unsigned char)*p))p++;e=p;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*e&&*e!=',')e++;n=(size_t)(e-p);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n&&isspace((unsigned char)p[n-1]))n--;/* Use the stable identifier comparison to choose the matching record or policy. */ if(n==xn&&strncmp(p,x,n)==0)return 1;p=e;}return 0;}
/*
 * Return the number of records represented by language runtime builtin profile without
 * changing their state.
 */
size_t umi_language_runtime_builtin_profile_count(void){return sizeof(FS)/sizeof(FS[0]);}
/*
 * Find language runtime builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_at(size_t i){return i<umi_language_runtime_builtin_profile_count()?FS[i]():NULL;}
/*
 * Find language runtime builtin profile while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_find(const char*id){size_t i;/* Use the stable identifier comparison to choose the matching record or policy. */ if(!id)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_runtime_builtin_profile_count();i++)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(FS[i]()->id,id)==0)return FS[i]();return NULL;}
/*
 * Provide the language runtime builtin profile for language operation used by this module
 * and its client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_for_language(const char*id){char n[128];size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_normalize_language_id(id,n,sizeof(n))!=UMI_STATUS_OK)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_runtime_builtin_profile_count();i++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(FS[i]()->enabled&&has(FS[i]()->language_ids,n))return FS[i]();return NULL;}
/*
 * Provide the language runtime register builtin profiles operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_register_builtin_profiles(UmiLanguageService*l){size_t i;UmiLanguageServerProfileRegistry*r;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!l)return UMI_STATUS_INVALID_ARGUMENT;r=umi_language_service_server_profiles(l);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!r)return UMI_STATUS_INVALID_STATE;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<umi_language_runtime_builtin_profile_count();i++){UmiStatus s=umi_language_server_profile_registry_upsert(r,FS[i]());/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
