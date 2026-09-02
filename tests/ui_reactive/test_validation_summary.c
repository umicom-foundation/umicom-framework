/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_summary.c
 *
 * PURPOSE:
 *   Exercise the validation summary reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveValidationSummary s; umi_ui_reactive_validation_summary_init(&s); s.valid_count=3U;s.warning_count=1U;return umi_ui_reactive_validation_summary_score(&s)>0U?0:1; }
