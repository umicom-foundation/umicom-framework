/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/builtin_profiles.c
 *
 * PURPOSE:
 *   Enumerate/register server profiles and resolve comma-delimited language coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
static int has(const char*l,const char*x){const char*p=l;size_t xn=strlen(x);while(p&&*p){const char*e;size_t n;while(*p==','||isspace((unsigned char)*p))p++;e=p;while(*e&&*e!=',')e++;n=(size_t)(e-p);while(n&&isspace((unsigned char)p[n-1]))n--;if(n==xn&&strncmp(p,x,n)==0)return 1;p=e;}return 0;}
size_t umi_language_runtime_builtin_profile_count(void){return sizeof(FS)/sizeof(FS[0]);}
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_at(size_t i){return i<umi_language_runtime_builtin_profile_count()?FS[i]():NULL;}
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_find(const char*id){size_t i;if(!id)return NULL;for(i=0;i<umi_language_runtime_builtin_profile_count();i++)if(strcmp(FS[i]()->id,id)==0)return FS[i]();return NULL;}
const UmiLanguageServerProfile *umi_language_runtime_builtin_profile_for_language(const char*id){char n[128];size_t i;if(umi_language_runtime_normalize_language_id(id,n,sizeof(n))!=UMI_STATUS_OK)return NULL;for(i=0;i<umi_language_runtime_builtin_profile_count();i++)if(FS[i]()->enabled&&has(FS[i]()->language_ids,n))return FS[i]();return NULL;}
UmiStatus umi_language_runtime_register_builtin_profiles(UmiLanguageService*l){size_t i;UmiLanguageServerProfileRegistry*r;if(!l)return UMI_STATUS_INVALID_ARGUMENT;r=umi_language_service_server_profiles(l);if(!r)return UMI_STATUS_INVALID_STATE;for(i=0;i<umi_language_runtime_builtin_profile_count();i++){UmiStatus s=umi_language_server_profile_registry_upsert(r,FS[i]());if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
