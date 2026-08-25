#include "umicom/teacher/study_streak.h"
int main(void) { UmiTeacherStudyStreak s; umi_teacher_study_streak_init(&s); if (umi_teacher_study_streak_record(&s,82U,1)!=UMI_STATUS_OK) return 1; umi_teacher_study_streak_set_units(&s,4U,4U); if (umi_teacher_study_streak_completion(&s)!=100U) return 2; if (!umi_teacher_study_streak_complete(&s)) return 3; return 0; }
