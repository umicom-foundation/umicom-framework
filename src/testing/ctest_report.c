/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report.c
 *
 * PURPOSE:
 *   Own saved CTest report parsing independently of live process execution.
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

/* Keep one parser and one set of outcome rules. A static linker can now
 * extract the report object without bringing in the live CTest runner. */
#include "ctest_report.inc"
