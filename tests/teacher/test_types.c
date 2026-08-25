#include <string.h>
#include "umicom/teacher/types.h"
int main(void) {
    if (strcmp(umi_teacher_language_text(UMI_TEACHER_LANGUAGE_C23), "c23") != 0) return 1;
    if (umi_teacher_percent(3U, 4U) != 75U) return 2;
    if (umi_teacher_clamp_score(140U) != 100U) return 3;
    return 0;
}
