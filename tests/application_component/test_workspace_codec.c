/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_workspace_codec.c
 *
 * PURPOSE:
 *   Verify portable workspace text round trips, reserved-character escaping
 *   and explicit capacity failures without reading or writing real files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/component.h"

#include <assert.h>
#include <string.h>

#include "umicom/base/text.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const UmiApplicationComponentRecipe *recipe =
      umi_application_component_recipe_catalogue_find("org.umicom.workspace.studio.learning");
  UmiApplicationComponentWorkspaceDraft source;
  UmiApplicationComponentWorkspaceDraft decoded;
  char encoded[8192];
  char legacy[8192];
  char too_small[16];
  size_t encoded_length = 0U;

  assert(recipe != NULL);
  assert(umi_application_component_workspace_draft_init(&source, recipe) == UMI_STATUS_OK);
  assert(umi_text_copy(source.title, sizeof(source.title), "Studio | Learning % Notes") ==
         UMI_STATUS_OK);
  assert(umi_text_copy(source.description, sizeof(source.description), "First line\nSecond line") ==
         UMI_STATUS_OK);
  assert(umi_application_component_workspace_encode(&source, encoded, sizeof(encoded),
                                                    &encoded_length) == UMI_STATUS_OK);
  assert(strstr(encoded, "%7C") != NULL);
  assert(strstr(encoded, "%25") != NULL);
  assert(strstr(encoded, "%0A") != NULL);
  assert(umi_application_component_workspace_decode(encoded, encoded_length, &decoded) ==
         UMI_STATUS_OK);
  assert(strcmp(decoded.title, source.title) == 0);
  assert(strcmp(decoded.description, source.description) == 0);
  assert(strcmp(decoded.experience_profile_id, source.experience_profile_id) == 0);
  assert(decoded.slot_count == source.slot_count);
  assert(umi_text_copy(legacy, sizeof(legacy), encoded) == UMI_STATUS_OK);
  legacy[17] = '1';
  {
    char *experience_line = strstr(legacy, "experience|");
    char *next_line;
    assert(experience_line != NULL);
    next_line = strchr(experience_line, '\n');
    assert(next_line != NULL);
    (void)memmove(experience_line, next_line + 1, strlen(next_line + 1) + 1U);
  }
  assert(umi_application_component_workspace_decode(legacy, strlen(legacy), &decoded) ==
         UMI_STATUS_OK);
  assert(strcmp(decoded.experience_profile_id, source.experience_profile_id) == 0);
  assert(umi_application_component_workspace_encode(&source, too_small, sizeof(too_small),
                                                    &encoded_length) ==
         UMI_STATUS_CAPACITY_EXCEEDED);
  return 0;
}
