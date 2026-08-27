/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/toolchain_readiness.h
 *
 * PURPOSE:
 *   Evaluate installed-tool readiness for existing language-aware developer toolchain bindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_TOOLCHAIN_READINESS_H
#define UMICOM_DEVELOPER_TOOLCHAIN_READINESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/command_line.h"
#include "umicom/developer/toolchain_binding.h"
#include "umicom/toolchain/discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*UmiDeveloperToolAvailabilityProbe)(
    const char *program,
    void *user_data);

typedef struct UmiDeveloperToolchainReadiness {
    uint32_t struct_size;
    char binding_id[128];
    char language_id[UMI_LANGUAGE_PROFILE_ID_CAPACITY];
    size_t supported_operation_count;
    size_t ready_operation_count;
    size_t missing_operation_count;
    int build_ready;
    int run_ready;
    int debug_ready;
    int test_ready;
    int format_ready;
    int lint_ready;
    int package_ready;
} UmiDeveloperToolchainReadiness;

/*
 * Return true when at least one semicolon-delimited command candidate can be
 * resolved. Each candidate is parsed by the existing safe developer command
 * parser; this function does not implement a second command-line grammar.
 */
int umi_developer_toolchain_command_available(
    const char *command_candidates,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data);

/*
 * Evaluate every operation advertised by an existing toolchain binding.
 * Passing probe=NULL uses umi_toolchain_find_on_path() for executable lookup.
 * The binding and registry are never modified.
 */
UmiStatus umi_developer_toolchain_binding_readiness(
    const UmiDeveloperToolchainBindingSnapshot *binding,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data,
    UmiDeveloperToolchainReadiness *out_readiness);

#ifdef __cplusplus
}
#endif
#endif
