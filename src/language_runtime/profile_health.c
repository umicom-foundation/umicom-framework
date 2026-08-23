/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profile_health.c
 *
 * PURPOSE:
 *   Implement language-server executable health reporting through Framework discovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profile_health.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_language_runtime_profile_health_probe(const UmiLanguageServerProfile*p,UmiLanguageRuntimeProfileHealth*out){UmiStatus s;if(!p||!out||!p->id[0]||!p->executable[0])return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));snprintf(out->profile_id,sizeof(out->profile_id),"%s",p->id);snprintf(out->executable,sizeof(out->executable),"%s",p->executable);s=umi_toolchain_find_on_path(p->executable,out->resolved_path,sizeof(out->resolved_path));out->status=s;out->available=s==UMI_STATUS_OK;return UMI_STATUS_OK;}
