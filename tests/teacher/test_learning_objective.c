#include "umicom/teacher/learning_objective.h"
int main(void) {
    UmiTeacherLearningObjective value;
    if (umi_teacher_learning_objective_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_learning_objective_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_learning_objective_priority(&value, 60U) != 70U) return 3;
    return 0;
}
