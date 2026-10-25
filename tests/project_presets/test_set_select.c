/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_set_select.c
 *
 * PURPOSE:
 *   Verify complete preset-name selection preserves unrelated settings and every failed output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "preset_set_fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {"different-names", "in-place",    "unrelated-fields", "owned",
                         "missing",         "ambiguous",   "incomplete",       "legacy",
                         "invalid-profile", "null-output", "null-profile",     "hidden"};
  bool known = false;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  CHECK(known);
  const char *json = preset_set_document;
  UmiStatus wanted = UMI_STATUS_OK;
  if (strcmp(name, "missing") == 0) {
    json = "{\"version\":6,\"configurePresets\":[{\"name\":\"c\"}]}";
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (strcmp(name, "ambiguous") == 0) {
    json = "{\"version\":6,\"configurePresets\":[{\"name\":\"c\"}],"
           "\"buildPresets\":[{\"name\":\"a\",\"configurePreset\":\"c\"},{\"name\":\"b\","
           "\"configurePreset\":\"c\"}],"
           "\"testPresets\":[{\"name\":\"t\",\"configurePreset\":\"c\"}]}";
    wanted = UMI_STATUS_ALREADY_EXISTS;
  }
  if (strcmp(name, "incomplete") == 0) {
    json = "{\"version\":6,\"include\":[\"shared.json\"],\"configurePresets\":[{\"name\":\"c\"}]}";
    wanted = UMI_STATUS_NOT_IMPLEMENTED;
  }
  if (strcmp(name, "hidden") == 0) {
    json = "{\"version\":6,\"configurePresets\":[{\"name\":\"c\",\"hidden\":true}]}";
    wanted = UMI_STATUS_PERMISSION_DENIED;
  }
  UmiProjectPresetCatalogue *catalogue = NULL;
  CHECK(PresetSetFixture(json, &catalogue) == UMI_STATUS_OK);
  UmiBuildProfile profile, output, expected;
  umi_build_profile_init(&profile);
  strcpy(profile.configure_preset, "prior-configure");
  strcpy(profile.build_preset, "prior-build");
  strcpy(profile.test_preset, "prior-test");
  if (strcmp(name, "unrelated-fields") == 0) {
    strcpy(profile.generator, "Ninja");
    strcpy(profile.build_target, "notes");
    strcpy(profile.run_arguments, "--version");
    strcpy(profile.configure_definitions, "NOTES_FEATURE=ON");
    profile.parallel_jobs = 3U;
  }
  if (strcmp(name, "legacy") == 0) {
    strcpy(profile.preset, "shared");
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "invalid-profile") == 0) {
    memset(profile.profile_id, 'x', sizeof profile.profile_id);
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  expected = profile;
  output = profile;
  unsigned char before[sizeof output];
  memcpy(before, &output, sizeof output);
  if (strcmp(name, "null-output") == 0) {
    CHECK(UmiProjectPresetSetSelect(catalogue, 0U, &profile, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (strcmp(name, "null-profile") == 0) {
    CHECK(UmiProjectPresetSetSelect(catalogue, 0U, NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &output, sizeof output) == 0);
  } else {
    const UmiBuildProfile *input = strcmp(name, "in-place") == 0 ? &output : &profile;
    CHECK(UmiProjectPresetSetSelect(catalogue, 0U, input, &output) == wanted);
    if (wanted == UMI_STATUS_OK) {
      strcpy(expected.configure_preset, "configure-notes");
      strcpy(expected.build_preset, "compile-notes");
      strcpy(expected.test_preset, "verify-notes");
      CHECK(umi_build_profile_equal(&output, &expected));
      if (strcmp(name, "in-place") != 0)
        CHECK(memcmp(before, &profile, sizeof profile) == 0);
    } else
      CHECK(memcmp(before, &output, sizeof output) == 0);
  }
  UmiProjectPresetCatalogueDestroy(catalogue);
  if (strcmp(name, "owned") == 0)
    CHECK(strcmp(output.test_preset, "verify-notes") == 0 &&
          strcmp(output.build_preset, "compile-notes") == 0);
  return 0;
}
