#include <string.h>
#include "umicom/teacher/teacher_context.h"
int main(void) { UmiTeacherTeacherContext q; umi_teacher_teacher_context_init(&q); if(umi_teacher_teacher_context_append(&q,"a")!=UMI_STATUS_OK) return 1; if(umi_teacher_teacher_context_append(&q,"b")!=UMI_STATUS_OK) return 2; if(strcmp(umi_teacher_teacher_context_current(&q),"a")!=0) return 3; if(umi_teacher_teacher_context_next(&q)!=UMI_STATUS_OK || strcmp(umi_teacher_teacher_context_current(&q),"b")!=0) return 4; return 0; }
