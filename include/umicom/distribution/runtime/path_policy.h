/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/path_policy.h
 *
 * PURPOSE:
 *   portable path validation rejecting traversal and unsafe absolute package paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PATH_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_PATH_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool umi_dr_path_policy_relative_safe(const char *path); bool umi_dr_path_policy_has_traversal(const char *path);

#ifdef __cplusplus
}
#endif
#endif
