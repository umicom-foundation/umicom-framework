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

/* Provide the print usage operation used by this module and its client applications. */
static void print_usage(const char *program) {
  (void)printf("Usage: %s [--format text|markdown|json] [--domain ID] [--check]\n"
               "\n"
               "Exports all 132 Framework component governance records. --check "
               "returns a non-zero exit code when candidate or stable components "
               "lack required evidence or frontend conformance.\n",
               program);
}

/* Provide the parse format operation used by this module and its client applications. */
static int parse_format(const char *value, UmiComponentReportFormat *out_format) {
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(value, "text") == 0) {
    *out_format = UMI_COMPONENT_REPORT_TEXT;
  } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(value, "markdown") == 0) {
    *out_format = UMI_COMPONENT_REPORT_MARKDOWN;
  } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(value, "json") == 0) {
    *out_format = UMI_COMPONENT_REPORT_JSON;
  } /* Use this fallback path when the earlier condition does not apply. */ else {
    return 0;
  }
  return 1;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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

  /* Visit each bounded item once so every record receives the same rule. */
  for (argument_index = 1; argument_index < argc; ++argument_index) {
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[argument_index], "--help") == 0 || strcmp(argv[argument_index], "-h") == 0) {
      print_usage(argv[0]);
      return 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[argument_index], "--check") == 0) {
      check_release = 1;
      continue;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[argument_index], "--format") == 0 && argument_index + 1 < argc) {
      argument_index += 1;
      /* Apply this branch only when its contract condition is satisfied. */
      if (!parse_format(argv[argument_index], &format)) {
        (void)fprintf(stderr, "Unknown report format: %s\n", argv[argument_index]);
        return 2;
      }
      continue;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(argv[argument_index], "--domain") == 0 && argument_index + 1 < argc) {
      domain_id = argv[++argument_index];
      continue;
    }
    (void)fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[argument_index]);
    print_usage(argv[0]);
    return 2;
  }

  status = umi_component_inventory_build(NULL, 0U, &inventory);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component inventory: %s\n", umi_status_text(status));
    return 3;
  }
  umi_component_query_init(&query);
  query.domain_id = domain_id;
  status = umi_component_inventory_report_write(&inventory, &query, format, NULL, 0U, &required);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component report sizing: %s\n", umi_status_text(status));
    return 4;
  }
  output = malloc(required);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (output == NULL) {
    (void)fprintf(stderr, "component report allocation failed\n");
    return 5;
  }
  status =
      umi_component_inventory_report_write(&inventory, &query, format, output, required, &required);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)fprintf(stderr, "component report: %s\n", umi_status_text(status));
    free(output);
    return 6;
  }
  (void)fputs(output, stdout);
  free(output);

  /* Apply this branch only when its contract condition is satisfied. */
  if (check_release) {
    UmiComponentReleasePolicy policy;
    UmiComponentReleaseReport report;
    umi_component_release_policy_init(&policy);
    status = umi_component_release_gate_evaluate(&inventory, &policy, &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
      (void)fprintf(stderr, "component release gate: %s\n", umi_status_text(status));
      return 7;
    }
    (void)fprintf(stderr,
                  "component release gate: evaluated=%zu blockers=%zu warnings=%zu "
                  "passed=%s\n",
                  report.evaluated_component_count, report.blocker_count, report.warning_count,
                  report.passed ? "yes" : "no");
    /* Apply this branch only when its contract condition is satisfied. */
    if (!report.passed)
      return 8;
  }
  return 0;
}
