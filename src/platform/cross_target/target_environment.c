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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_environment.h"

#include <string.h>
/*
 * Read ct environment into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_ct_environment_parse(const char *text,UmiCtEnvironment *out_environment){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(text==NULL||out_environment==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(text,"gnu")==0)*out_environment=UMI_CT_ENV_GNU;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(text,"msvc")==0)*out_environment=UMI_CT_ENV_MSVC;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(text,"musl")==0)*out_environment=UMI_CT_ENV_MUSL;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(text,"none")==0)*out_environment=UMI_CT_ENV_NONE;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(text,"umicom")==0||strcmp(text,"native")==0)*out_environment=UMI_CT_ENV_UMICOM;/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_NOT_FOUND;return UMI_STATUS_OK;}
/*
 * Provide the ct environment text operation used by this module and its client
 * applications.
 */
const char *umi_ct_environment_text(UmiCtEnvironment e){/* Select the behaviour associated with the requested command or state value. */ switch(e){case UMI_CT_ENV_GNU:return"gnu";case UMI_CT_ENV_MSVC:return"msvc";case UMI_CT_ENV_MUSL:return"musl";case UMI_CT_ENV_NONE:return"none";case UMI_CT_ENV_UMICOM:return"umicom";default:return"unknown";}}
