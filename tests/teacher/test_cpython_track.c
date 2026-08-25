#include "umicom/teacher/cpython_track.h"
int main(void) {
    UmiTeacherCpythonTrack value;
    if (umi_teacher_cpython_track_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    if (umi_teacher_cpython_track_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_teacher_cpython_track_priority(&value, 60U) != 70U) return 3;
    return 0;
}
