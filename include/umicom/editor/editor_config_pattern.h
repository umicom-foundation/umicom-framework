/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_config_pattern.h
 *
 * PURPOSE:
 *   Define bounded EditorConfig pattern matching for project setting review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_PATTERN_H
#define UMICOM_EDITOR_CONFIG_PATTERN_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Match a case-sensitive EditorConfig section against a relative UTF-8 file
 * path using forward slashes. A pattern without a slash outside character
 * classes matches the basename. Supports *, **, ?, literal character classes,
 * [!classes], backslash escapes, brace alternatives and ascending integer
 * ranges within signed 32-bit bounds. A recursive-star directory segment also matches zero
 * directories. No filesystem access. Paths are at most 2048 bytes and reject empty/dot/parent
 * components, NUL, backslashes and absolute paths. Pattern length is 1..1024 bytes. Expansion is
 * bounded to 256 alternatives and 8 brace-expansion steps; exceeding a bound returns
 * CAPACITY_EXCEEDED, never an approximate match. A malformed pattern returns PARSE_ERROR. Output is
 * unchanged on failure. These bounded helpers have not been certified against upstream core tests.
 */
UmiStatus UmiEditorConfigPatternMatch(const char *pattern, const char *relative_path,
                                      int *out_match);
#ifdef __cplusplus
}
#endif
#endif
