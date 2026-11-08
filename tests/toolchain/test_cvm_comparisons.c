/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_comparisons.c
 *
 * PURPOSE:
 *   Verify comparison preview status, result invalidation and trace ownership through the installed
 * Umicom C adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/cvm_trace.h"
#include <stdio.h>
#include <string.h>
#ifndef UMC_COMPARISON_PROFILE
#error "Comparison qualification requires an installed Umicom C SDK with comparison headers."
#endif
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *left, const char *right) { return strcmp(left, right) == 0; }
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  const char *cases[] = {"true",
                         "false",
                         "precedence",
                         "signed-extremes",
                         "syntax-reset",
                         "eager-fault",
                         "fuel-before-compare",
                         "fuel-before-return",
                         "ordinary-parity",
                         "trace-reset"};
  size_t known = 0U;
  for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
    known += (size_t)Is(mode, cases[i]);
  if (known != 1U)
    return 2;
  UmiCvmPreviewWorkspace workspace;
  UmiCvmPreviewResult result;
  UmiCvmPreviewTrace trace;
  const char *source = "int main(void) { return 1 < 2; }";
  CHECK(umi_cvm_preview_traced(source, strlen(source), 4U, &workspace, sizeof workspace, &result,
                               sizeof result, &trace, sizeof trace) == UMI_STATUS_OK);
  CHECK(result.completed && result.execution.value == 1 && trace.count == 4U);
  size_t fuel = UMC_PROGRAM_LIMIT;
  int32_t expected = 1;
  if (Is(mode, "false")) {
    source = "int main(void) { return 2 < 1; }";
    expected = 0;
  } else if (Is(mode, "precedence")) {
    source = "int main(void) { return 2 == 3 < 4; }";
    expected = 0;
  } else if (Is(mode, "signed-extremes"))
    source = "int main(void) { return (-2147483647 - 1) < 2147483647; }";
  else if (Is(mode, "syntax-reset"))
    source = "int main(void) {\n return 1 < = 2;\n}";
  else if (Is(mode, "eager-fault"))
    source = "int main(void) {\n return 0 == (1 / 0);\n}";
  else if (Is(mode, "fuel-before-compare"))
    fuel = 2U;
  else if (Is(mode, "fuel-before-return"))
    fuel = 3U;
  else if (Is(mode, "trace-reset")) {
    source = "int main(void) { return 9; }";
    expected = 9;
  }
  UmiStatus status =
      umi_cvm_preview_traced(source, strlen(source), fuel, &workspace, sizeof workspace, &result,
                             sizeof result, &trace, sizeof trace);
  if (Is(mode, "syntax-reset")) {
    CHECK(status == UMI_STATUS_PARSE_ERROR && result.phase == UMI_CVM_COMPILING);
    CHECK(!result.completed && result.execution.value == 0 && result.instruction_count == 0U &&
          trace.count == 0U);
    CHECK(result.diagnostic.status == UMC_SYNTAX_ERROR && result.diagnostic.line == 2U);
  } else if (Is(mode, "eager-fault")) {
    CHECK(status == UMI_STATUS_INVALID_STATE && result.phase == UMI_CVM_EXECUTING &&
          !result.completed);
    CHECK(result.execution.value == 0 && result.execution.instructions_executed == 0U);
    CHECK(trace.count == 4U && trace.steps[3].opcode == UMC_DIVIDE &&
          trace.steps[3].status == UMC_ARITHMETIC_ERROR);
    CHECK(result.diagnostic.line == 2U);
  } else if (fuel < 4U) {
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && result.phase == UMI_CVM_EXECUTING &&
          !result.completed);
    CHECK(result.execution.value == 0 && result.execution.instructions_executed == 0U &&
          trace.count == fuel + 1U);
    CHECK(trace.steps[fuel].opcode == (fuel == 2U ? UMC_LESS : UMC_RETURN));
    CHECK(trace.steps[fuel].status == UMC_FUEL_EXHAUSTED &&
          trace.steps[fuel].top == (fuel == 2U ? 2 : 1));
  } else {
    /* A false comparison is still a successfully completed preview. */
    CHECK(status == UMI_STATUS_OK && result.phase == UMI_CVM_COMPLETE && result.completed);
    CHECK(result.execution.value == expected && trace.count == result.instruction_count);
    CHECK(result.execution.instructions_executed == trace.count);
    CHECK(trace.steps[trace.count - 1U].opcode == UMC_RETURN &&
          trace.steps[trace.count - 1U].top == expected);
    if (Is(mode, "trace-reset"))
      CHECK(trace.count == 2U);
    if (Is(mode, "ordinary-parity")) {
      UmiCvmPreviewResult ordinary;
      CHECK(umi_cvm_preview(source, strlen(source), fuel, &workspace, sizeof workspace, &ordinary,
                            sizeof ordinary) == UMI_STATUS_OK);
      CHECK(ordinary.completed && ordinary.execution.value == result.execution.value);
      CHECK(ordinary.execution.instructions_executed == result.execution.instructions_executed);
    }
  }
  return 0;
}
