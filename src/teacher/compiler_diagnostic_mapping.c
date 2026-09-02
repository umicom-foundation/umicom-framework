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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/compiler_diagnostic_mapping.h"
#include <string.h>
/*
 * Initialise teacher compiler diagnostic mapping from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_compiler_diagnostic_mapping_init(UmiTeacherCompilerDiagnosticMapping *mapping) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(mapping!=NULL) memset(mapping,0,sizeof(*mapping));
    }
/*
 * Provide the teacher compiler diagnostic mapping record operation used by this module and
 * its client applications.
 */
void umi_teacher_compiler_diagnostic_mapping_record(UmiTeacherCompilerDiagnosticMapping *mapping,int severity,int category) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(mapping==NULL) return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(severity>=2) ++mapping->error_count;
    else /* Apply this branch only when its contract condition is satisfied. */ if(severity==1) ++mapping->warning_count;
    /* Use this fallback path when the earlier condition does not apply. */
    else ++mapping->note_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if(category==1) ++mapping->syntax_count;
    else /* Apply this branch only when its contract condition is satisfied. */ if(category==2) ++mapping->type_count;
    else /* Apply this branch only when its contract condition is satisfied. */ if(category==3) ++mapping->linker_count;
    }
/*
 * Provide the teacher compiler diagnostic mapping learning penalty operation used by this
 * module and its client applications.
 */
uint32_t umi_teacher_compiler_diagnostic_mapping_learning_penalty(const UmiTeacherCompilerDiagnosticMapping *mapping) { uint64_t p;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(mapping==NULL) return 0U;
    p=(uint64_t)mapping->error_count*15U+(uint64_t)mapping->warning_count*4U+(uint64_t)mapping->note_count;
    return p>100U?100U:(uint32_t)p;
    }
/*
 * Provide the teacher compiler diagnostic mapping category text operation used by this
 * module and its client applications.
 */
const char *umi_teacher_compiler_diagnostic_mapping_category_text(int category) { /* Apply this branch only when its contract condition is satisfied. */ if(category==1) return "syntax";
    /* Apply this branch only when its contract condition is satisfied. */
    if(category==2) return "type";
    /* Apply this branch only when its contract condition is satisfied. */
    if(category==3) return "linker";
    return "general";
    }
