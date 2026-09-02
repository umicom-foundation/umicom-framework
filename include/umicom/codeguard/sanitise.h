/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/sanitise.h
 *
 * PURPOSE:
 *   Remove comments and string contents before code-pattern rules are
 *   evaluated.
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
#ifndef UMICOM_CODEGUARD_SANITISE_H
#define UMICOM_CODEGUARD_SANITISE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard sanitise code line operation used by this module and its client
 * applications.
 */
void umi_codeguard_sanitise_code_line(const char *input, char *output, size_t capacity,
                                      int *in_block_comment);
/**
 * Provide the codeguard trim operation used by this module and its client applications.
 */
void umi_codeguard_trim(char *text);
#ifdef __cplusplus
}
#endif
#endif
