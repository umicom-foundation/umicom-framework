#include "umicom/teacher/assessment_feedback.h"
int main(void) {
    UmiTeacherAssessmentFeedback value;
    if (umi_teacher_assessment_feedback_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_assessment_feedback_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_assessment_feedback_priority(&value, 60U) != 70U) return 3;
    return 0;
}
