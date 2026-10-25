/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_settings_changes.c
 *
 * PURPOSE:
 *   Check all-or-nothing settings publication, coherent reads and stale-review rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/settings_changes.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      result = 1;                                                                                  \
      goto done;                                                                                   \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *cases[] = {"read",         "apply",          "normalised",     "noop",
                         "stale-first",  "stale-last",     "invalid-first",  "invalid-last",
                         "unknown",      "duplicate",      "null-member",    "long-value",
                         "read-unknown", "read-duplicate", "read-arguments", "apply-arguments",
                         "independent",  "repeat"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  int result = 0;
  UmiSettings *settings = NULL;
  const UmiSettingDefinition schema[] = {
      {"width", UMI_SETTING_INTEGER, "4", "Width", 1, 16, 0, 0, 1, 0},
      {"spaces", UMI_SETTING_BOOLEAN, "true", "Spaces", 0, 0, 0, 0, 0, 0},
      {"label", UMI_SETTING_STRING, "draft", "Name", 0, 0, 0, 0, 0, 0}};
  CHECK(umi_settings_create(schema, 3U, &settings) == UMI_STATUS_OK);
  const char *keys[] = {"width", "spaces"};
  UmiSettingValue values[2], before[2];
  memset(values, 0x5a, sizeof values);
  memcpy(before, values, sizeof values);
  UmiSettingChange changes[] = {{"width", "4", "2"}, {"spaces", "true", "false"}};
  UmiStatus expected = UMI_STATUS_OK, status;
  if (!strcmp(name, "read")) {
    CHECK(UmiSettingsReadValues(settings, keys, 2U, values) == UMI_STATUS_OK);
    CHECK(!strcmp(values[0].text, "4") && !strcmp(values[1].text, "true"));
    goto done;
  }
  if (!strcmp(name, "read-unknown") || !strcmp(name, "read-duplicate")) {
    keys[1] = !strcmp(name, "read-unknown") ? "missing" : "width";
    expected = !strcmp(name, "read-unknown") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_ARGUMENT;
    CHECK(UmiSettingsReadValues(settings, keys, 2U, values) == expected);
    CHECK(!memcmp(values, before, sizeof values));
    goto done;
  }
  if (!strcmp(name, "read-arguments")) {
    CHECK(UmiSettingsReadValues(NULL, keys, 2U, values) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsReadValues(settings, NULL, 2U, values) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsReadValues(settings, keys, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsReadValues(settings, keys, 0U, values) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsReadValues(settings, keys, 257U, values) == UMI_STATUS_INVALID_ARGUMENT);
    keys[1] = NULL;
    CHECK(UmiSettingsReadValues(settings, keys, 2U, values) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(values, before, sizeof values));
    goto done;
  }
  if (!strcmp(name, "apply-arguments")) {
    CHECK(UmiSettingsApplyChanges(NULL, changes, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsApplyChanges(settings, NULL, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsApplyChanges(settings, changes, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiSettingsApplyChanges(settings, changes, 257U) == UMI_STATUS_INVALID_ARGUMENT);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    changes[0].key = NULL;
  }
  if (!strcmp(name, "normalised")) {
    changes[0].expected_value = "04";
    changes[0].value = "02";
    changes[1].expected_value = "YES";
    changes[1].value = "off";
  }
  if (!strcmp(name, "noop")) {
    changes[0].value = "4";
    changes[1].value = "true";
  }
  if (!strcmp(name, "stale-first")) {
    changes[0].expected_value = "8";
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "stale-last")) {
    changes[1].expected_value = "false";
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "invalid-first")) {
    changes[0].value = "0";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "invalid-last")) {
    changes[1].value = "maybe";
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "unknown")) {
    changes[1].key = "missing";
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "duplicate")) {
    changes[1] = changes[0];
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "null-member")) {
    changes[1].expected_value = NULL;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  char longText[UMI_SETTING_VALUE_CAPACITY + 1U];
  memset(longText, 'x', sizeof longText);
  longText[sizeof longText - 1U] = '\0';
  if (!strcmp(name, "long-value")) {
    changes[1] = (UmiSettingChange){"label", "draft", longText};
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (!strcmp(name, "independent"))
    CHECK(umi_settings_set_text(settings, "label", "changed") == UMI_STATUS_OK);
  status = UmiSettingsApplyChanges(settings, changes, 2U);
  CHECK(status == expected);
  if (!strcmp(name, "repeat"))
    CHECK(UmiSettingsApplyChanges(settings, changes, 2U) == UMI_STATUS_INVALID_STATE);
  CHECK(UmiSettingsReadValues(settings, keys, 2U, values) == UMI_STATUS_OK);
  int published = expected == UMI_STATUS_OK && strcmp(name, "noop");
  CHECK(!strcmp(values[0].text, published ? "2" : "4"));
  CHECK(!strcmp(values[1].text, published ? "false" : "true"));
  char label[32];
  CHECK(umi_settings_get_text(settings, "label", label, sizeof label) == UMI_STATUS_OK);
  CHECK(!strcmp(label, !strcmp(name, "independent") ? "changed" : "draft"));
done:
  umi_settings_destroy(settings);
  return result;
}
