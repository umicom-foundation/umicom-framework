/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_set_inspect.c
 *
 * PURPOSE:
 *   Check related-stage matching without guessing missing, hidden or inherited metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "preset_set_fixture.h"
typedef struct SetCase {
  const char *name, *json;
  size_t index, builds, tests;
  bool incomplete, deferred;
  UmiStatus status;
} SetCase;
static const SetCase cases[] = {
    {"related",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, false, false, UMI_STATUS_OK},
    {"missing-build",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 1U, false, false, UMI_STATUS_OK},
    {"missing-test",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[]}",
     0U, 1U, 0U, false, false, UMI_STATUS_OK},
    {"ambiguous-build",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"},{\"name\":\"compile-"
     "optimized\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{\"name\":\"verify-"
     "notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 2U, 1U, false, false, UMI_STATUS_OK},
    {"ambiguous-test",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"},{\"name\":\"verify-"
     "integration\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 2U, false, false, UMI_STATUS_OK},
    {"hidden-configure",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\",\"hidden\":true}],"
     "\"buildPresets\":[{\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 0U, false, false, UMI_STATUS_PERMISSION_DENIED},
    {"false-configure",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\",\"condition\":false}],"
     "\"buildPresets\":[{\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 0U, false, false, UMI_STATUS_PERMISSION_DENIED},
    {"hidden-build",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\",\"hidden\":true}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 1U, false, false, UMI_STATUS_OK},
    {"false-build",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\",\"condition\":false}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 1U, false, false, UMI_STATUS_OK},
    {"hidden-test",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\",\"hidden\":true}]}",
     0U, 1U, 0U, false, false, UMI_STATUS_OK},
    {"false-test",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\",\"condition\":false}]}",
     0U, 1U, 0U, false, false, UMI_STATUS_OK},
    {"unrelated",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"},{\"name\":\"other-"
     "build\",\"configurePreset\":\"another\"}],\"testPresets\":[{\"name\":\"verify-notes\","
     "\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, false, false, UMI_STATUS_OK},
    {"case-sensitive",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"Configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 0U, 1U, false, false, UMI_STATUS_OK},
    {"includes",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}],\"include\":[\"shared."
     "json\"]}",
     0U, 1U, 1U, true, false, UMI_STATUS_OK},
    {"configure-inherits",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\",\"inherits\":\"base\"}],"
     "\"buildPresets\":[{\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, true, false, UMI_STATUS_OK},
    {"build-inherits",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\",\"inherits\":\"base\"}],"
     "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, true, false, UMI_STATUS_OK},
    {"hidden-inherits",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"},{\"name\":\"hidden-"
     "base\",\"hidden\":true,\"inherits\":\"base\"}],\"testPresets\":[{\"name\":\"verify-notes\","
     "\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, false, false, UMI_STATUS_OK},
    {"unrelated-inherits",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"},{\"name\":\"unknown\","
     "\"inherits\":\"base\"}],\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":"
     "\"configure-notes\"}]}",
     0U, 1U, 1U, true, false, UMI_STATUS_OK},
    {"configure-condition",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\",\"condition\":{\"type\":"
     "\"equals\",\"lhs\":\"a\",\"rhs\":\"b\"}}],\"buildPresets\":[{\"name\":\"compile-notes\","
     "\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{\"name\":\"verify-notes\","
     "\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, false, true, UMI_STATUS_OK},
    {"build-condition",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\",\"condition\":{\"type\":"
     "\"equals\",\"lhs\":\"a\",\"rhs\":\"b\"}}],\"testPresets\":[{\"name\":\"verify-notes\","
     "\"configurePreset\":\"configure-notes\"}]}",
     0U, 1U, 1U, false, true, UMI_STATUS_OK},
    {"wrong-stage",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     1U, 0U, 0U, false, false, UMI_STATUS_INVALID_ARGUMENT},
    {"invalid-index",
     "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],\"buildPresets\":[{"
     "\"name\":\"compile-notes\",\"configurePreset\":\"configure-notes\"}],\"testPresets\":[{"
     "\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}",
     128U, 0U, 0U, false, false, UMI_STATUS_NOT_FOUND}};
int main(int argc, char **argv) {
  CHECK(argc == 2);
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
    const SetCase *test = &cases[i];
    if (strcmp(argv[1], test->name) != 0)
      continue;
    UmiProjectPresetCatalogue *catalogue = NULL;
    CHECK(PresetSetFixture(test->json, &catalogue) == UMI_STATUS_OK);
    UmiProjectPresetSetSummary summary;
    memset(&summary, 0x5a, sizeof summary);
    unsigned char before[sizeof summary];
    memcpy(before, &summary, sizeof summary);
    CHECK(UmiProjectPresetSetInspect(catalogue, test->index, &summary) == test->status);
    if (test->status == UMI_STATUS_OK) {
      CHECK(summary.configure_index == test->index);
      CHECK(summary.build_count == test->builds && summary.test_count == test->tests);
      CHECK(summary.discovery_incomplete == test->incomplete &&
            summary.conditions_deferred == test->deferred);
      CHECK((summary.build_index == SIZE_MAX) == (test->builds != 1U));
      CHECK((summary.test_index == SIZE_MAX) == (test->tests != 1U));
    } else
      CHECK(memcmp(before, &summary, sizeof summary) == 0);
    UmiProjectPresetCatalogueDestroy(catalogue);
    return 0;
  }
  if (strcmp(argv[1], "arguments") == 0) {
    UmiProjectPresetSetSummary summary;
    CHECK(UmiProjectPresetSetInspect(NULL, 0U, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    UmiProjectPresetCatalogue *catalogue = NULL;
    CHECK(PresetSetFixture(preset_set_document, &catalogue) == UMI_STATUS_OK);
    CHECK(UmiProjectPresetSetInspect(catalogue, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiProjectPresetCatalogueDestroy(catalogue);
    return 0;
  }
  return 2;
}
