#include "umicom/teacher/skill_gap.h"
int main(void) { UmiTeacherSkillGap r; uint32_t score; umi_teacher_skill_gap_init(&r); score=umi_teacher_skill_gap_compute(&r,100U,80U,60U); if(score!=89U) return 1; if(!umi_teacher_skill_gap_passes(&r,score)) return 2; if(umi_teacher_skill_gap_configure(&r,50U,20U,20U,70U)!=UMI_STATUS_INVALID_ARGUMENT) return 3; return 0; }
