/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_build_contract.c
 *
 * PURPOSE:
 *   Declare cross-build requirements while leaving actual compiler/tool discovery to the existing Toolchain subsystem.
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

#include "umicom/platform/cross_target/cross_build_contract.h"

UmiStatus umi_ct_cross_build_contract_validate(const UmiCtCrossBuildContract*c){if(c==NULL||!umi_ct_id_valid(c->contract_id)||c->target.architecture==UMI_CT_ARCH_UNKNOWN||c->required_toolchain_family[0]=='\0'||c->required_abi[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(c->target.operating_system==UMI_CT_OS_BARE_METAL&&!c->require_sysroot)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
