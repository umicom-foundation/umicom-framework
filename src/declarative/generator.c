/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/generator.c
 *
 * PURPOSE:
 *   Generate reviewable starter source that loads a declarative application rather than embedding widget construction in product code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/generator.h"
#include <stdio.h>
UmiStatus umi_decl_generate_c_main(const UmiDeclApplicationPlan *p,char *out,size_t cap){int n;if(p==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,cap,"#include <stdio.h>\n#include <umicom/umicom.h>\n\nint main(void)\n{\n    puts(\"%s\");\n    return 0;\n}\n",p->application_id);return n>=0&&(size_t)n<cap?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
UmiStatus umi_decl_generate_cmake(const UmiDeclApplicationPlan *p,char *out,size_t cap){int n;if(p==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,cap,"cmake_minimum_required(VERSION 3.24)\nproject(%s LANGUAGES C)\nfind_package(UmicomFramework CONFIG REQUIRED)\nadd_executable(app main.c)\ntarget_link_libraries(app PRIVATE Umicom::Framework)\n",p->application_id);return n>=0&&(size_t)n<cap?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
