#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/process_channel_recording/CMakeLists.txt
#
# PURPOSE:
#   Qualify native channel recording without composing the full Framework or GUI.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomProcessChannelRecording LANGUAGES C)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomProcessChannelRecordingTests.cmake")
