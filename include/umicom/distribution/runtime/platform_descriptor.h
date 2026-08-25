/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/platform_descriptor.h
 *
 * PURPOSE:
 *   runtime operating-system descriptors and minimum platform revision requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_DESCRIPTOR_H
#define UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrPlatformDescriptor { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrVersion minimum_version; bool desktop; bool server; } UmiDrPlatformDescriptor;
void umi_dr_platform_descriptor_init(UmiDrPlatformDescriptor *value);
bool umi_dr_platform_descriptor_valid(const UmiDrPlatformDescriptor *value);
uint64_t umi_dr_platform_descriptor_fingerprint(const UmiDrPlatformDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
