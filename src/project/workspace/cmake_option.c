/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/cmake_option.c
 *
 * PURPOSE:
 *   Implement the cmake option behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_option.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_option_parse_bool(const char*value,bool*out_enabled) {
    if(value==NULL||out_enabled==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(value,"ON")==0||strcmp(value,"1")==0||strcmp(value,"TRUE")==0) {
        *out_enabled=true;
        return UMI_STATUS_OK;
    }
    if(strcmp(value,"OFF")==0||strcmp(value,"0")==0||strcmp(value,"FALSE")==0) {
        *out_enabled=false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}
