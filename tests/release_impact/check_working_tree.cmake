#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_impact/check_working_tree.cmake
# Purpose: Check real Git path discovery and preview immutability in owned fixtures.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED TOOL OR NOT DEFINED DEST OR NOT DEFINED ROOT OR NOT DEFINED CASE)
    message(FATAL_ERROR "TOOL, DEST, ROOT and CASE are required")
endif()
find_program(_git NAMES git REQUIRED)
include("${ROOT}/cmake/UmicomReleaseInventoryFormat.cmake")
# Each run owns a new fixture. No reset, clean, network operation or deletion is
# needed, and an interrupted test leaves its repository available for diagnosis.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _token)
set(_run "${DEST}/${_token}")
set(_repo "${_run}/repository")
file(MAKE_DIRECTORY "${_repo}" "${_run}/empty-hooks")
file(WRITE "${_run}/empty-git-config" "")
set(_environment
    --unset=GIT_DIR --unset=GIT_WORK_TREE --unset=GIT_INDEX_FILE --unset=GIT_COMMON_DIR
    --unset=GIT_OBJECT_DIRECTORY --unset=GIT_ALTERNATE_OBJECT_DIRECTORIES
    --unset=GIT_CONFIG_PARAMETERS --unset=GIT_CONFIG_COUNT --unset=GIT_TEMPLATE_DIR
    "GIT_CONFIG_NOSYSTEM=1" "GIT_CONFIG_GLOBAL=${_run}/empty-git-config")
function(fixture_git)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E env ${_environment}
        "${_git}" --no-optional-locks -C "${_repo}"
        -c "core.hooksPath=${_run}/empty-hooks" -c commit.gpgSign=false
        -c user.name=Fixture -c user.email=fixture@example.invalid ${ARGN}
        RESULT_VARIABLE _code OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 20)
    if(NOT _code STREQUAL "0")
        message(FATAL_ERROR "Fixture Git failed: ${_error}")
    endif()
    set(FIXTURE_GIT_OUTPUT "${_output}" PARENT_SCOPE)
endfunction()
fixture_git(init --initial-branch=main "--template=${_run}/empty-hooks")
file(WRITE "${_repo}/a.c" "int sample(void) { return 1; }
")
fixture_git(add -- a.c)
fixture_git(commit -m "Create synthetic source fixture")
fixture_git(rev-parse HEAD)
string(STRIP "${FIXTURE_GIT_OUTPUT}" _base)
set(_since "${_base}")
set(_expected 0)
if(CASE STREQUAL "staged")
    file(APPEND "${_repo}/a.c" "/* staged edit */
")
    fixture_git(add -- a.c)
elseif(CASE STREQUAL "unstaged")
    file(APPEND "${_repo}/a.c" "/* unstaged edit */
")
elseif(CASE STREQUAL "rename")
    fixture_git(mv -- a.c "renamed source.c")
elseif(CASE STREQUAL "untracked")
    file(WRITE "${_repo}/new source.c" "int new_source(void) { return 2; }
")
elseif(CASE STREQUAL "since-committed" OR CASE STREQUAL "since-mixed" OR CASE STREQUAL "since-replacement")
    file(APPEND "${_repo}/a.c" "/* committed edit */\n")
    fixture_git(add -- a.c)
    fixture_git(commit -m "Change synthetic source fixture")
    if(CASE STREQUAL "since-replacement")
        fixture_git(rev-parse HEAD)
        string(STRIP "${FIXTURE_GIT_OUTPUT}" _replacement)
        fixture_git(replace "${_base}" "${_replacement}")
    endif()
    if(CASE STREQUAL "since-mixed")
        file(WRITE "${_repo}/new.c" "int added(void) { return 3; }\n")
    endif()
elseif(CASE STREQUAL "since-rename")
    fixture_git(mv -- a.c "committed rename.c")
    fixture_git(commit -m "Rename synthetic source fixture")
elseif(CASE STREQUAL "since-gitlink")
    fixture_git(update-index --add --cacheinfo "160000,${_base},child")
    fixture_git(commit -m "Record synthetic gitlink fixture")
elseif(CASE STREQUAL "since-invalid")
    set(_since main)
    set(_expected 2)
elseif(CASE STREQUAL "since-missing")
    set(_since 0000000000000000000000000000000000000000)
    set(_expected 2)
elseif(CASE STREQUAL "since-blob")
    fixture_git(rev-parse HEAD:a.c)
    string(STRIP "${FIXTURE_GIT_OUTPUT}" _since)
    set(_expected 2)
elseif(NOT CASE STREQUAL "clean" AND NOT CASE STREQUAL "since-same")
    message(FATAL_ERROR "Unknown working-tree case")
endif()
set(_input "${_run}/inventory.tsv")
string(HEX "${_repo}" _source)
string(HEX "${_run}/build" _build)
file(WRITE "${_input}" "UMICOM-RELEASE-INVENTORY	1
context	cmake	0123456789abcdef0123456789abcdef	${_source}	${_build}	4465627567
")
_umicom_inventory_row("${_input}" target base "${_repo}" configured "impact-version=1
complete=1
impact-end
")
_umicom_inventory_row("${_input}" source "${_repo}/a.c" base present "synthetic")
_umicom_inventory_row("${_input}" test "fixture.sample" "${_repo}" registered "impact-version=1
complete=1
target=62617365
impact-end
")
fixture_git(status --porcelain=v1 --untracked-files=all)
set(_status_before "${FIXTURE_GIT_OUTPUT}")
fixture_git(rev-parse HEAD)
set(_head_before "${FIXTURE_GIT_OUTPUT}")
file(SHA256 "${_repo}/.git/index" _index_before)
file(SHA256 "${_repo}/.git/HEAD" _branch_before)
file(SHA256 "${_input}" _inventory_before)
file(GLOB _source_files "${_repo}/*.c")
set(_hashes_before)
foreach(_file IN LISTS _source_files)
    file(SHA256 "${_file}" _hash)
    list(APPEND _hashes_before "${_hash}")
endforeach()
set(_arguments --inventory "${_input}" --working-tree --git "${_git}")
if(CASE MATCHES "^since-")
    list(APPEND _arguments --since "${_since}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -E env ${_environment}
    "${TOOL}" impact ${_arguments}
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 40)
file(WRITE "${_run}/stdout.txt" "${_output}")
file(WRITE "${_run}/stderr.txt" "${_error}")
if(NOT _result STREQUAL "${_expected}")
    message(FATAL_ERROR "Working-tree preview failed; read ${_run}/stderr.txt")
endif()
fixture_git(status --porcelain=v1 --untracked-files=all)
if(NOT FIXTURE_GIT_OUTPUT STREQUAL _status_before)
    message(FATAL_ERROR "Preview changed Git status")
endif()
fixture_git(rev-parse HEAD)
if(NOT FIXTURE_GIT_OUTPUT STREQUAL _head_before)
    message(FATAL_ERROR "Preview changed the committed revision")
endif()
file(SHA256 "${_repo}/.git/index" _index_after)
file(SHA256 "${_repo}/.git/HEAD" _branch_after)
file(SHA256 "${_input}" _inventory_after)
if(NOT _index_before STREQUAL _index_after OR NOT _branch_before STREQUAL _branch_after OR
   NOT _inventory_before STREQUAL _inventory_after)
    message(FATAL_ERROR "Preview modified the index, branch identity or inventory")
endif()
set(_hashes_after)
foreach(_file IN LISTS _source_files)
    file(SHA256 "${_file}" _hash)
    list(APPEND _hashes_after "${_hash}")
endforeach()
if(NOT _hashes_before STREQUAL _hashes_after)
    message(FATAL_ERROR "Preview modified source bytes")
endif()
if(_expected EQUAL 2)
    if(NOT _error MATCHES "Impact preview refused" OR _output MATCHES "PREVIEW ONLY")
        message(FATAL_ERROR "Invalid or unavailable commit baseline published a usable preview")
    endif()
elseif(CASE STREQUAL "clean" OR CASE STREQUAL "since-same")
    if(NOT _output MATCHES "Changed paths: 0; selected targets: 0; selected tests: 0" OR
       NOT _output MATCHES "No changed paths selected")
        message(FATAL_ERROR "Clean-tree preview invented work or lost its qualification disclaimer")
    endif()
elseif(CASE STREQUAL "rename" OR CASE STREQUAL "since-rename" OR CASE STREQUAL "since-mixed")
    if(NOT _output MATCHES "Changed paths: 2" OR NOT _output MATCHES "changed path has no configured owner")
        message(FATAL_ERROR "Rename did not preserve both paths and widen unknown ownership")
    endif()
elseif(CASE STREQUAL "since-gitlink")
    if(NOT _output MATCHES "Full selection requested")
        message(FATAL_ERROR "Committed gitlink change did not require broader coverage")
    endif()
elseif(CASE STREQUAL "untracked")
    if(NOT _output MATCHES "Changed paths: 1" OR NOT _output MATCHES "changed path has no configured owner")
        message(FATAL_ERROR "Untracked path was omitted")
    endif()
elseif(NOT _output MATCHES "Changed paths: 1; selected targets: 1; selected tests: 1")
    message(FATAL_ERROR "Staged or unstaged source was not selected")
endif()

if(CASE MATCHES "^since-" AND _expected EQUAL 0 AND NOT _output MATCHES "Commit baseline:")
    message(FATAL_ERROR "Compared commit identities were omitted")
endif()
