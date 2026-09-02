/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_problem_report.c
 *
 * PURPOSE:
 *   Implement the test language problem report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language problem report test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/language/problem_report.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageDiagnosticRegistry*r=NULL;UmiLanguageDiagnosticSnapshot d={0};UmiLanguageProblemReport p;strcpy(d.id,"d1");strcpy(d.message,"bad");d.severity=1;assert(umi_language_diagnostic_registry_create(&r)==UMI_STATUS_OK);assert(umi_language_diagnostic_registry_upsert(r,&d)==UMI_STATUS_OK);assert(umi_language_problem_report_build(r,&p)==UMI_STATUS_OK);assert(p.total==1U&&p.errors==1U);umi_language_diagnostic_registry_destroy(r);return 0;}
