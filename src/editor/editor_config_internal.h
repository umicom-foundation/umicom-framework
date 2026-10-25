/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config_internal.h
 *
 * PURPOSE:
 *   Share private parser storage and validated text operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_INTERNAL_H
#define UMICOM_EDITOR_CONFIG_INTERNAL_H
#include "umicom/editor/editor_config.h"
#include <stdint.h>
typedef struct ConfigSection {
  char *pattern;
  size_t line;
} ConfigSection;
typedef struct ConfigPair {
  char *key, *value;
  size_t line, section;
} ConfigPair;
struct UmiEditorConfig {
  ConfigSection *sections;
  size_t section_count;
  ConfigPair *pairs;
  size_t pair_count;
  int root;
};
/* Private text helpers keep parser and glob admission on the same UTF-8 rules. */
UmiStatus UmiEditorConfigTextValidate(const char *text, size_t bytes);
size_t UmiEditorConfigScalar(const char *text, uint32_t *out);
int UmiEditorConfigEqualFolded(const char *left, const char *right);
#endif
