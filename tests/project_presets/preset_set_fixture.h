/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/preset_set_fixture.h
 *
 * PURPOSE:
 *   Provide reusable in-memory preset declarations for related-stage tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PRESET_SET_FIXTURE_H
#define UMICOM_TEST_PRESET_SET_FIXTURE_H
#include "umicom/developer_project/preset_set.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static const char preset_set_document[] =
    "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],"
    "\"buildPresets\":[{\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],"
    "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}";
static inline UmiStatus PresetSetFixture(const char *json, UmiProjectPresetCatalogue **out) {
  return UmiProjectPresetCatalogueCreate(json, strlen(json), NULL, 0U, out);
}
#endif
