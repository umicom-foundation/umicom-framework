/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/config.h
 *
 * PURPOSE:
 *   Bundle the scan root, profile and optional baseline or suppression inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_CONFIG_H
#define UMICOM_CODEGUARD_CONFIG_H
#include "umicom/codeguard/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard config data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardConfig {
    const char *root;
    UmiCodeGuardProfile profile;
    const char *baseline_path;
    const char *suppression_path;
} UmiCodeGuardConfig;
/**
 * Provide the codeguard config default operation used by this module and its client
 * applications.
 */
UmiCodeGuardConfig umi_codeguard_config_default(const char *root);
#ifdef __cplusplus
}
#endif
#endif
