/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/curriculum_graph.h
 *
 * PURPOSE:
 *   Represent prerequisite relationships and detect curriculum cycles.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_CURRICULUM_GRAPH_H
#define UMICOM_TEACHER_CURRICULUM_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherCurriculumGraphEdge { uint16_t prerequisite; uint16_t dependent; } UmiTeacherCurriculumGraphEdge;
typedef struct UmiTeacherCurriculumGraph { UmiTeacherCurriculumGraphEdge edges[UMI_TEACHER_MEDIUM_CAPACITY]; size_t edge_count; uint16_t node_count; } UmiTeacherCurriculumGraph;
void umi_teacher_curriculum_graph_init(UmiTeacherCurriculumGraph *graph, uint16_t node_count);
UmiStatus umi_teacher_curriculum_graph_add(UmiTeacherCurriculumGraph *graph, uint16_t prerequisite, uint16_t dependent);
int umi_teacher_curriculum_graph_has_cycle(const UmiTeacherCurriculumGraph *graph);
int umi_teacher_curriculum_graph_prerequisite_satisfied(const UmiTeacherCurriculumGraph *graph, uint16_t node, const uint8_t *completed, size_t completed_count);

#ifdef __cplusplus
}
#endif

#endif
