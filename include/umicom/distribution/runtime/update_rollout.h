/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/update_rollout.h
 *
 * PURPOSE:
 *   deterministic staged-rollout cohort selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_UPDATE_ROLLOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_UPDATE_ROLLOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the dr update rollout bucket operation used by this module and its client
 * applications.
 */
uint32_t umi_dr_update_rollout_bucket(const char *installation_id); bool umi_dr_update_rollout_selected(const char *installation_id,uint32_t percentage);

#ifdef __cplusplus
}
#endif
#endif
