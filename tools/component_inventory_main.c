/*-----------------------------------------------------------------------------
 * Umicom Framework Native Tool
 * File: tools/component_inventory_main.c
 *
 * PURPOSE:
 *   Export the Framework component-governance inventory in text, Markdown or
 *   JSON and optionally fail when release-scoped evidence gaps remain.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/application.h"

static void print_usage(const char *program) {
  (void)printf("Usage: %s [--format text|markdown|json] [--domain ID] [--check]\n"
               "\n"
               "Exports all 132 Framework component governance records. --check "
               "returns a non-zero exit code when candidate or stable components "
               "lack required evidence or frontend conformance.\n",
               program);
}

static int parse_format(const char *value, UmiComponentReportFormat *out_format) {
  if (strcmp(value, "text") == 0) {
    *out_format = UMI_COMPONENT_REPORT_TEXT;
  } else if (strcmp(value, "markdown") == 0) {
    *out_format = UMI_COMPONENT_REPORT_MARKDOWN;
  } else if (strcmp(value, "json") == 0) {
    *out_format = UMI_COMPONENT_REPORT_JSON;
  } else {
    return 0;
  }
  return 1;
}

int main(int argc, char **argv) {
  UmiComponentInventory inventory;
  UmiComponentQuery query;
  UmiComponentReportFormat format = UMI_COMPONENT_REPORT_TEXT;
  const char *domain_id = NULL;
  char *output;
  size_t required = 0U;
  int check_release = 0;
  int argument_index;
  UmiStatus status;

  for (argument_index = 1; argument_index < argc; ++argument_index) {
    if (strcmp(argv[argument_index], "--help") == 0 || strcmp(argv[argument_index], "-h") == 0) {
      print_usage(argv[0]);
      return 0;
    }
    if (strcmp(argv[argument_index], "--check") == 0) {
      check_release = 1;
      continue;
    }
    if (strcmp(argv[argument_index], "--format") == 0 && argument_index + 1 < argc) {
      argument_index += 1;
      if (!parse_format(argv[argument_index], &format)) {
        (void)fprintf(stderr, "Unknown report format: %s\n", argv[argument_index]);
        return 2;
      }
      continue;
    }
    if (strcmp(argv[argument_index], "--domain") == 0 && argument_index + 1 < argc) {
      domain_id = argv[++argument_index];
      continue;
    }
    (void)fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[argument_index]);
    print_usage(argv[0]);
    return 2;
  }

  status = umi_component_inventory_build(NULL, 0U, &inventory);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component inventory: %s\n", umi_status_text(status));
    return 3;
  }
  umi_component_query_init(&query);
  query.domain_id = domain_id;
  status = umi_component_inventory_report_write(&inventory, &query, format, NULL, 0U, &required);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component report sizing: %s\n", umi_status_text(status));
    return 4;
  }
  output = malloc(required);
  if (output == NULL) {
    (void)fprintf(stderr, "component report allocation failed\n");
    return 5;
  }
  status =
      umi_component_inventory_report_write(&inventory, &query, format, output, required, &required);
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component report: %s\n", umi_status_text(status));
    free(output);
    return 6;
  }
  (void)fputs(output, stdout);
  free(output);

  if (check_release) {
    UmiComponentReleasePolicy policy;
    UmiComponentReleaseReport report;
    umi_component_release_policy_init(&policy);
    status = umi_component_release_gate_evaluate(&inventory, &policy, &report);
    if (status != UMI_STATUS_OK) {
      (void)fprintf(stderr, "component release gate: %s\n", umi_status_text(status));
      return 7;
    }
    (void)fprintf(stderr,
                  "component release gate: evaluated=%zu blockers=%zu warnings=%zu "
                  "passed=%s\n",
                  report.evaluated_component_count, report.blocker_count, report.warning_count,
                  report.passed ? "yes" : "no");
    if (!report.passed)
      return 8;
  }
  return 0;
}
