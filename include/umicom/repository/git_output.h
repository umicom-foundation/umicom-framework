/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/git_output.h
 *
 * PURPOSE:
 *   Provide shared parsing helpers for captured Git output so probes do not duplicate text logic.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_REPOSITORY_GIT_OUTPUT_H
#define UMICOM_REPOSITORY_GIT_OUTPUT_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Trim CR/LF and surrounding whitespace from captured single-line output. */
UmiStatus umi_repository_git_output_trim(char *text);
/* Parse a non-negative decimal size emitted by Git. */
UmiStatus umi_repository_git_output_parse_size(const char *text, size_t *out_value);
/* Count non-empty logical lines without modifying the source buffer. */
size_t umi_repository_git_output_line_count(const char *text);
/* Read one logical line by index into caller-owned storage. */
UmiStatus umi_repository_git_output_line_at(
    const char *text, size_t index, char *out_line, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
