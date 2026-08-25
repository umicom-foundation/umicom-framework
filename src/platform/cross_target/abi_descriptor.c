/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/abi_descriptor.c
 *
 * PURPOSE:
 *   Describe data model, calling convention, stack alignment and floating-point ABI properties for cross-target compatibility checks.
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

#include "umicom/platform/cross_target/abi_descriptor.h"

UmiStatus umi_ct_abi_descriptor_validate(const UmiCtAbiDescriptor*d){if(d==NULL||d->abi_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(d->data_model<UMI_CT_DATA_ILP32||d->data_model>UMI_CT_DATA_LLP64)return UMI_STATUS_INVALID_ARGUMENT;if(d->pointer_bits!=32U&&d->pointer_bits!=64U)return UMI_STATUS_INVALID_ARGUMENT;if(d->stack_alignment==0U||(d->stack_alignment&(d->stack_alignment-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;if(d->data_model==UMI_CT_DATA_ILP32&&d->pointer_bits!=32U)return UMI_STATUS_INVALID_STATE;if((d->data_model==UMI_CT_DATA_LP64||d->data_model==UMI_CT_DATA_LLP64)&&d->pointer_bits!=64U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
