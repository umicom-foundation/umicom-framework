#include "umicom/teacher/mastery_evidence.h"
int main(void) {
    UmiTeacherMasteryEvidence value;
    if (umi_teacher_mastery_evidence_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_mastery_evidence_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_mastery_evidence_priority(&value, 60U) != 70U) return 3;
    return 0;
}
