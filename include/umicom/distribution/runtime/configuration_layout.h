/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/configuration_layout.h
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_CONFIGURATION_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr configuration layout data shared with callers of this public contract.
 */
typedef struct UmiDrConfigurationLayout { char id[UMI_DR_ID_CAPACITY]; char system_root[UMI_DR_PATH_CAPACITY]; char user_root[UMI_DR_PATH_CAPACITY]; bool portable; } UmiDrConfigurationLayout;
/**
 * Initialise dr configuration layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value);
/**
 * Check that dr configuration layout satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value);
/**
 * Provide the dr configuration layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value);

#ifdef __cplusplus
}
#endif
#endif
