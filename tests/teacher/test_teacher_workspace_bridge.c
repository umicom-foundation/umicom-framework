#include "umicom/teacher/teacher_workspace_bridge.h"
int main(void) {
    UmiTeacherTeacherWorkspaceBridge value;
    if (umi_teacher_teacher_workspace_bridge_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_teacher_workspace_bridge_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_teacher_workspace_bridge_priority(&value, 60U) != 70U) return 3;
    return 0;
}
