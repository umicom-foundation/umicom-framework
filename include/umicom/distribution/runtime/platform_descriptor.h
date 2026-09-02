/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/platform_descriptor.h
 *
 * PURPOSE:
 *   runtime operating-system descriptors and minimum platform revision requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dr platform descriptor data shared with callers of this public contract.
 */
typedef struct UmiDrPlatformDescriptor { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrVersion minimum_version; bool desktop; bool server; } UmiDrPlatformDescriptor;
/**
 * Initialise dr platform descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_platform_descriptor_init(UmiDrPlatformDescriptor *value);
/**
 * Check that dr platform descriptor satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_platform_descriptor_valid(const UmiDrPlatformDescriptor *value);
/**
 * Provide the dr platform descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_platform_descriptor_fingerprint(const UmiDrPlatformDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
