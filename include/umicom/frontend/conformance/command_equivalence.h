/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/command_equivalence.h
 *
 * PURPOSE:
 *   command-surface parity scoring and missing-command blocker reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_COMMAND_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_COMMAND_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc command equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcCommandEquivalence { size_t required_commands; size_t implemented_commands; size_t blockers; } UmiFcCommandEquivalence;
/**
 * Provide the fc command equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_command_equivalence_evaluate(const UmiFcCommandEquivalence *item);
/**
 * Provide the fc command equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_command_equivalence_passes(const UmiFcCommandEquivalence *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
