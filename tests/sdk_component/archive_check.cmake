#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/sdk_component/archive_check.cmake
#
# PURPOSE:
#   Verify archive identity and consume its relocated SDK contents.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# The caller owns a fresh random run directory; retain all artifacts and logs.
file(GLOB _archives "${_producer}/sdk-packages/${CONFIG}/*.zip")
list(LENGTH _archives _archive_count)
if(NOT _archive_count EQUAL 1)
    message(FATAL_ERROR "SDK archive target did not produce exactly one ZIP")
endif()
list(GET _archives 0 _archive)
if(NOT EXISTS "${_archive}.sha256")
    message(FATAL_ERROR "SDK archive checksum is missing")
endif()
file(SHA256 "${_archive}" _actual_digest)
file(READ "${_archive}.sha256" _checksum)
string(SUBSTRING "${_checksum}" 0 64 _recorded_digest)
if(NOT _actual_digest STREQUAL _recorded_digest)
    message(FATAL_ERROR "SDK archive checksum does not match the package bytes")
endif()
set(_extract "${_run}/extracted SDK")
file(MAKE_DIRECTORY "${_extract}")
file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${_extract}")
file(GLOB _roots LIST_DIRECTORIES TRUE "${_extract}/*")
list(LENGTH _roots _root_count)
if(NOT _root_count EQUAL 1)
    message(FATAL_ERROR "SDK archive must have one top-level directory")
endif()
list(GET _roots 0 _prefix)
if(NOT IS_DIRECTORY "${_prefix}" OR NOT EXISTS "${_prefix}/include/sdk_fixture.h")
    message(FATAL_ERROR "SDK archive omitted the installed public header")
endif()
# check.cmake continues with artifact admission, imported target resolution,
# actual consumer linking and execution from this extracted prefix.
