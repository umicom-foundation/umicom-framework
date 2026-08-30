/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/data_model.c
 *
 * PURPOSE:
 *   Resolve portable C fundamental-type widths for ILP32, LP64 and LLP64 target data models.
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

#include "umicom/platform/cross_target/data_model.h"

UmiStatus umi_ct_data_model_widths(UmiCtDataModel m,UmiCtTypeWidths*w){if(w==NULL)return UMI_STATUS_INVALID_ARGUMENT;w->short_bits=16U;w->int_bits=32U;w->long_long_bits=64U;if(m==UMI_CT_DATA_ILP32){w->long_bits=32U;w->pointer_bits=32U;w->size_t_bits=32U;}else if(m==UMI_CT_DATA_LP64){w->long_bits=64U;w->pointer_bits=64U;w->size_t_bits=64U;}else if(m==UMI_CT_DATA_LLP64){w->long_bits=32U;w->pointer_bits=64U;w->size_t_bits=64U;}else return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
