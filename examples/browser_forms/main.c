/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/browser_forms/main.c
 * PURPOSE: Demonstrate a native browser-form review without opening a network listener.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/structured_form.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "dispatch.count",
     .label = "Package count",
     .help_text = "Whole packages in this fictional dispatch.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER,
     .required = 1,
     .enforce_number_range = 1,
     .minimum_number = 1.0,
     .maximum_number = 1000.0},
    {.field_id = "dispatch.note",
     .label = "Dispatch note",
     .help_text = "Use fictional information only.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT}};
static const UmiStructuredFormField rules[] = {
    {"dispatch.count", "package.count", UMI_FORM_TEXT_PLAIN, 0, 0},
    {"dispatch.note", "", UMI_FORM_TEXT_PLAIN, 1, 1}};
static const UmiApplicationExperienceFormSchema base = {sizeof(base),
                                                        UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                        "example.dispatch",
                                                        "Dispatch review",
                                                        "A native C23 form rendered for a browser.",
                                                        "example.review",
                                                        fields,
                                                        2U};
static const UmiStructuredFormSchema schema = {&base, 1U, rules, 2U};
int main(int argc, char **argv) {
  if (argc > 2 || (argc == 2 && strcmp(argv[1], "--html") != 0))
    return 2;
  UmiStructuredForm *form = malloc(sizeof(*form));
  UmiStructuredFormEdit *edit = malloc(sizeof(*edit));
  char *html = malloc(UMI_WEB_FORM_HTML_MAX_BYTES);
  if (form == NULL || edit == NULL || html == NULL) {
    free(form);
    free(edit);
    free(html);
    return 1;
  }
  UmiStatus status = umi_structured_form_init(&schema, form);
  size_t length = 0U;
  /* A real host obtains a fresh session-bound token. This fixed token is only
   * for an offline demonstration; it is never sufficient for a public service. */
  if (status == UMI_STATUS_OK)
    status = umi_web_structured_form_render(&schema, form, "/dispatch/review", "offline_example",
                                            html, UMI_WEB_FORM_HTML_MAX_BYTES, &length);
  if (status == UMI_STATUS_OK && argc == 2) {
    puts("<!doctype html><html lang=\"en\"><meta charset=\"UTF-8\"><title>Dispatch form "
         "example</title><body>");
    puts("<p>Offline rendering example. No server is listening and the buttons cannot save.</p>");
    puts(html);
    puts("</body></html>");
  }
  const char *body = "form=example.dispatch&version=1&revision=1&csrf=offline_example&intent=submit"
                     "&state.dispatch.count=1&value.dispatch.count=12&unit.dispatch.count=package."
                     "count&reason.dispatch.count="
                     "&state.dispatch.note=1&value.dispatch.note=Fictional+dispatch&unit.dispatch."
                     "note=&reason.dispatch.note=";
  int submitting = 0;
  if (status == UMI_STATUS_OK)
    status = umi_web_structured_form_prepare(&schema, form, body, strlen(body), "offline_example",
                                             edit, &submitting);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_edit_apply(&schema, form, edit);
  if (status == UMI_STATUS_OK &&
      (form->revision != 2U || !submitting || form->answers[0].value.integer_value != 12))
    status = UMI_STATUS_INVALID_STATE;
  if (argc == 1)
    printf("Browser form review: %s; no data saved or sent.\n", umi_status_text(status));
  free(html);
  free(edit);
  free(form);
  return status == UMI_STATUS_OK ? 0 : 1;
}
