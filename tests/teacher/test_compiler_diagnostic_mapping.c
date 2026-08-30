/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_compiler_diagnostic_mapping.c
 *
 * PURPOSE:
 *   Implement the test compiler diagnostic mapping behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/teacher/compiler_diagnostic_mapping.h"
int main(void) { UmiTeacherCompilerDiagnosticMapping m; umi_teacher_compiler_diagnostic_mapping_init(&m); umi_teacher_compiler_diagnostic_mapping_record(&m,2,1); umi_teacher_compiler_diagnostic_mapping_record(&m,1,2); if(umi_teacher_compiler_diagnostic_mapping_learning_penalty(&m)!=19U) return 1; if(strcmp(umi_teacher_compiler_diagnostic_mapping_category_text(3),"linker")!=0) return 2; return 0; }
