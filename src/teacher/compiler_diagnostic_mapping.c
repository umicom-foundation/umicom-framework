/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/compiler_diagnostic_mapping.c
 *
 * PURPOSE:
 *   Map compiler diagnostic evidence to learner-facing categories.
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
#include "umicom/teacher/compiler_diagnostic_mapping.h"
#include <string.h>
void umi_teacher_compiler_diagnostic_mapping_init(UmiTeacherCompilerDiagnosticMapping *mapping) { if(mapping!=NULL) memset(mapping,0,sizeof(*mapping));
    }
void umi_teacher_compiler_diagnostic_mapping_record(UmiTeacherCompilerDiagnosticMapping *mapping,int severity,int category) { if(mapping==NULL) return;
    if(severity>=2) ++mapping->error_count;
    else if(severity==1) ++mapping->warning_count;
    else ++mapping->note_count;
    if(category==1) ++mapping->syntax_count;
    else if(category==2) ++mapping->type_count;
    else if(category==3) ++mapping->linker_count;
    }
uint32_t umi_teacher_compiler_diagnostic_mapping_learning_penalty(const UmiTeacherCompilerDiagnosticMapping *mapping) { uint64_t p;
    if(mapping==NULL) return 0U;
    p=(uint64_t)mapping->error_count*15U+(uint64_t)mapping->warning_count*4U+(uint64_t)mapping->note_count;
    return p>100U?100U:(uint32_t)p;
    }
const char *umi_teacher_compiler_diagnostic_mapping_category_text(int category) { if(category==1) return "syntax";
    if(category==2) return "type";
    if(category==3) return "linker";
    return "general";
    }
