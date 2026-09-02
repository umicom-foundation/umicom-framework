/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_data_model.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data model cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/data_model.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtTypeWidths w;CHECK(umi_ct_data_model_widths(UMI_CT_DATA_LP64,&w)==UMI_STATUS_OK);CHECK(w.long_bits==64U&&w.pointer_bits==64U);CHECK(umi_ct_data_model_widths(UMI_CT_DATA_LLP64,&w)==UMI_STATUS_OK);CHECK(w.long_bits==32U&&w.pointer_bits==64U);return 0;}
