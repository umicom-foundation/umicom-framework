/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/editor_config_fixture.h
 *
 * PURPOSE:
 *   Exercise configuration text and pattern boundaries with explicit memory-only expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_EDITOR_CONFIG_FIXTURE_H
#define UMICOM_TEST_EDITOR_CONFIG_FIXTURE_H
#include "umicom/editor/editor_config.h"
#include "umicom/editor/editor_config_result.h"
#include "umicom/editor/editor_config_settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
/* Memory-only fixtures own their parse tree until resolution has copied it. */
static inline UmiStatus ResolveOne(const char *text, const char *file,
                                   UmiEditorConfigResult **out) {
  UmiEditorConfig *config = NULL;
  UmiStatus status = UmiEditorConfigParse(text, strlen(text), &config, NULL);
  if (status == UMI_STATUS_OK) {
    UmiEditorConfigLayer layer = {config, file, ".editorconfig"};
    status = UmiEditorConfigResolve(&layer, 1U, out);
  }
  UmiEditorConfigDestroy(config);
  return status;
}
#endif
