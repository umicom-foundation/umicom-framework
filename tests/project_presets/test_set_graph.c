/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_set_graph.c
 *
 * PURPOSE:
 *   Verify related-stage selection uses resolved include and inheritance metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/developer_project/preset_set.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  CHECK(strcmp(name, "inherited") == 0 || strcmp(name, "included") == 0 ||
        strcmp(name, "user") == 0 || strcmp(name, "false-condition") == 0 ||
        strcmp(name, "ambiguous") == 0);
  char root[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  const char *project =
      "{\"version\":9,\"include\":[\"shared.json\"],"
      "\"configurePresets\":[{\"name\":\"configure\",\"inherits\":\"configure-base\"}],"
      "\"buildPresets\":[{\"name\":\"build\",\"inherits\":\"build-base\"}],"
      "\"testPresets\":[{\"name\":\"test\",\"inherits\":\"test-base\"}]}";
  const char *shared =
      "{\"version\":9,\"configurePresets\":[{\"name\":\"configure-base\",\"hidden\":true}],"
      "\"buildPresets\":[{\"name\":\"build-base\",\"hidden\":true,\"configurePreset\":"
      "\"configure\"}],"
      "\"testPresets\":[{\"name\":\"test-base\",\"hidden\":true,\"configurePreset\":\"configure\"}]"
      "}";
  if (strcmp(name, "included") == 0) {
    project = "{\"version\":9,\"include\":[\"shared.json\"],\"configurePresets\":[{\"name\":"
              "\"configure\"}]}";
    shared =
        "{\"version\":9,\"buildPresets\":[{\"name\":\"build\",\"configurePreset\":\"configure\"}],"
        "\"testPresets\":[{\"name\":\"test\",\"configurePreset\":\"configure\"}]}";
  }
  if (strcmp(name, "false-condition") == 0)
    shared = "{\"version\":9,\"configurePresets\":[{\"name\":\"configure-base\",\"hidden\":true}],"
             "\"buildPresets\":[{\"name\":\"build-base\",\"hidden\":true,\"configurePreset\":"
             "\"configure\",\"condition\":false}],"
             "\"testPresets\":[{\"name\":\"test-base\",\"hidden\":true,\"configurePreset\":"
             "\"configure\"}]}";
  if (strcmp(name, "ambiguous") == 0)
    shared = "{\"version\":9,\"configurePresets\":[{\"name\":\"configure-base\",\"hidden\":true}],"
             "\"buildPresets\":[{\"name\":\"build-base\",\"hidden\":true,\"configurePreset\":"
             "\"configure\"},"
             "{\"name\":\"other\",\"configurePreset\":\"configure\"}],"
             "\"testPresets\":[{\"name\":\"test-base\",\"hidden\":true,\"configurePreset\":"
             "\"configure\"}]}";
  FixturePath(file, root,
              strcmp(name, "user") == 0 ? "CMakeUserPresets.json" : "CMakePresets.json");
  CHECK(umi_fs_write_text(file, project) == UMI_STATUS_OK);
  FixturePath(file, root, "shared.json");
  CHECK(umi_fs_write_text(file, shared) == UMI_STATUS_OK);
  UmiProjectPresetCatalogue *catalogue = NULL;
  UmiProjectPresetReadReport report;
  CHECK(UmiProjectPresetCatalogueReadExpanded(root, NULL, &catalogue, &report) == UMI_STATUS_OK);
  UmiProjectPresetSummary catalogueSummary;
  CHECK(UmiProjectPresetCatalogueSummary(catalogue, &catalogueSummary) == UMI_STATUS_OK);
  size_t configure = SIZE_MAX;
  for (size_t i = 0U; i < catalogueSummary.count; ++i) {
    UmiProjectPresetChoice choice;
    CHECK(UmiProjectPresetCatalogueAt(catalogue, i, &choice) == UMI_STATUS_OK);
    if (choice.stage == UMI_PROJECT_PRESET_CONFIGURE && strcmp(choice.name, "configure") == 0)
      configure = i;
  }
  CHECK(configure != SIZE_MAX);
  UmiProjectPresetSetSummary summary;
  CHECK(UmiProjectPresetSetInspect(catalogue, configure, &summary) == UMI_STATUS_OK);
  CHECK(!summary.discovery_incomplete);
  UmiBuildProfile profile, selected;
  umi_build_profile_init(&profile);
  selected = profile;
  UmiStatus wanted = strcmp(name, "false-condition") == 0 ? UMI_STATUS_NOT_FOUND
                     : strcmp(name, "ambiguous") == 0     ? UMI_STATUS_ALREADY_EXISTS
                                                          : UMI_STATUS_OK;
  CHECK(UmiProjectPresetSetSelect(catalogue, configure, &profile, &selected) == wanted);
  if (wanted == UMI_STATUS_OK) {
    CHECK(strcmp(selected.configure_preset, "configure") == 0 &&
          strcmp(selected.build_preset, "build") == 0 && strcmp(selected.test_preset, "test") == 0);
  } else
    CHECK(umi_build_profile_equal(&profile, &selected));
  UmiProjectPresetCatalogueDestroy(catalogue);
  return 0;
}
