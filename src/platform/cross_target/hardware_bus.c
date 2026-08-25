/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/hardware_bus.c
 *
 * PURPOSE:
 *   Describe discoverable hardware buses and address/interrupt translation capabilities.
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

#include "umicom/platform/cross_target/hardware_bus.h"

UmiStatus umi_ct_hardware_bus_validate(const UmiCtHardwareBus*b){if(b==NULL||!umi_ct_id_valid(b->bus_id)||b->type<UMI_CT_BUS_PLATFORM||b->type>UMI_CT_BUS_SPI||(b->address_bits!=32U&&b->address_bits!=64U))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
