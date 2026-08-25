#include "umicom/teacher/compiler_result.h"
int main(void) { UmiTeacherCompilerResult s; umi_teacher_compiler_result_init(&s); if (umi_teacher_compiler_result_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_compiler_result_set_units(&s,4U,4U); if (umi_teacher_compiler_result_completion(&s)!=100U) return 2; if (!umi_teacher_compiler_result_complete(&s)) return 3; return 0; }
