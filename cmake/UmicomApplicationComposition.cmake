# Umicom Framework
# File: cmake/UmicomApplicationComposition.cmake
#
# PURPOSE:
#   Expose lightweight thin-application composition helpers to installed SDK consumers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCompilePolicy.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomThinApplicationRuntime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationProductisationContribution.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeApplication.cmake")
