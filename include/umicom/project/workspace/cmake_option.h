/* Umicom Framework | CMake option parsing | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_OPTION_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_OPTION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    UmiStatus umi_project_workspace_cmake_option_parse_bool(const char*value,bool*out_enabled);
#ifdef __cplusplus
}
#endif
#endif
