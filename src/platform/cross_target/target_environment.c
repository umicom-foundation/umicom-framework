/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_environment.c
 *
 * PURPOSE:
 *   Parse target ABI/runtime environment names independently from operating-system identity.
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

#include "umicom/platform/cross_target/target_environment.h"

#include <string.h>
UmiStatus umi_ct_environment_parse(const char *text,UmiCtEnvironment *out_environment){if(text==NULL||out_environment==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strcmp(text,"gnu")==0)*out_environment=UMI_CT_ENV_GNU;else if(strcmp(text,"msvc")==0)*out_environment=UMI_CT_ENV_MSVC;else if(strcmp(text,"musl")==0)*out_environment=UMI_CT_ENV_MUSL;else if(strcmp(text,"none")==0)*out_environment=UMI_CT_ENV_NONE;else if(strcmp(text,"umicom")==0||strcmp(text,"native")==0)*out_environment=UMI_CT_ENV_UMICOM;else return UMI_STATUS_NOT_FOUND;return UMI_STATUS_OK;}
const char *umi_ct_environment_text(UmiCtEnvironment e){switch(e){case UMI_CT_ENV_GNU:return"gnu";case UMI_CT_ENV_MSVC:return"msvc";case UMI_CT_ENV_MUSL:return"musl";case UMI_CT_ENV_NONE:return"none";case UMI_CT_ENV_UMICOM:return"umicom";default:return"unknown";}}
