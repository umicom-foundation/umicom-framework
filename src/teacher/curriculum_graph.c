/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/curriculum_graph.c
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
#include "umicom/teacher/curriculum_graph.h"
#include <string.h>
void umi_teacher_curriculum_graph_init(UmiTeacherCurriculumGraph *graph,uint16_t node_count) { if(graph!=NULL) { memset(graph,0,sizeof(*graph));
    graph->node_count=node_count;
    } }
UmiStatus umi_teacher_curriculum_graph_add(UmiTeacherCurriculumGraph *graph,uint16_t prerequisite,uint16_t dependent) { size_t i;
    if(graph==NULL||prerequisite>=graph->node_count||dependent>=graph->node_count||prerequisite==dependent) return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<graph->edge_count;++i) if(graph->edges[i].prerequisite==prerequisite&&graph->edges[i].dependent==dependent) return UMI_STATUS_ALREADY_EXISTS;
    if(graph->edge_count>=UMI_TEACHER_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->edges[graph->edge_count].prerequisite=prerequisite;
    graph->edges[graph->edge_count].dependent=dependent;
    ++graph->edge_count;
    return UMI_STATUS_OK;
    }
static int umi_teacher_curriculum_graph_visit(const UmiTeacherCurriculumGraph *graph,uint16_t node,uint8_t *marks) { size_t i;
    if(marks[node]==1U) return 1;
    if(marks[node]==2U) return 0;
    marks[node]=1U;
    for(i=0U;i<graph->edge_count;++i) if(graph->edges[i].prerequisite==node && umi_teacher_curriculum_graph_visit(graph,graph->edges[i].dependent,marks)) return 1;
    marks[node]=2U;
    return 0;
    }
int umi_teacher_curriculum_graph_has_cycle(const UmiTeacherCurriculumGraph *graph) { uint8_t marks[UINT16_MAX < 256 ? UINT16_MAX : 256];
    uint16_t i;
    if(graph==NULL || graph->node_count>256U) return 1;
    memset(marks,0,sizeof(marks));
    for(i=0U;i<graph->node_count;++i) if(umi_teacher_curriculum_graph_visit(graph,i,marks)) return 1;
    return 0;
    }
int umi_teacher_curriculum_graph_prerequisite_satisfied(const UmiTeacherCurriculumGraph *graph,uint16_t node,const uint8_t *completed,size_t completed_count) { size_t i;
    if(graph==NULL||completed==NULL||node>=graph->node_count||completed_count<graph->node_count) return 0;
    for(i=0U;i<graph->edge_count;++i) if(graph->edges[i].dependent==node && completed[graph->edges[i].prerequisite]==0U) return 0;
    return 1;
    }
