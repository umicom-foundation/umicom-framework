/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_request_identity.c
 *
 * PURPOSE:
 *   Distinguish compiler request metadata, preserve collection boundaries and reject malformed storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/cache.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"request identity line %d\n",__LINE__); return 1; } } while (0)
static UmiCompilerRequest baseline, changed;
int main(void)
{
    /* Explicit metadata keeps this fixture independent of the host target. */
    (void)strcpy(baseline.project_id,"project"); (void)strcpy(baseline.unit_id,"unit");
    (void)strcpy(baseline.source_path,"src/main.c"); (void)strcpy(baseline.output_path,"main.o");
    (void)strcpy(baseline.standard,"c23"); (void)strcpy(baseline.working_directory,"project");
    baseline.language = UMI_COMPILER_LANGUAGE_C; baseline.action = UMI_COMPILER_ACTION_COMPILE;
    (void)strcpy(baseline.target.triple,"x86_64-unknown-linux-gnu");
    (void)strcpy(baseline.target.architecture,"x86_64"); (void)strcpy(baseline.target.vendor,"unknown");
    (void)strcpy(baseline.target.operating_system,"linux"); (void)strcpy(baseline.target.environment,"gnu");
    baseline.target.object_format = UMI_COMPILER_OBJECT_ELF;
    baseline.target.pointer_width = 64U; baseline.target.little_endian = true;
    uint64_t original = umi_compiler_request_hash(&baseline);
    REQUIRE(original != 0U && umi_compiler_request_hash(NULL) == 0U);
    changed = baseline;
    REQUIRE(umi_compiler_request_hash(&changed) == original);
    /* Each independent public scalar field contributes to metadata identity. */
    for (unsigned field = 0U; field < 19U; ++field) {
        changed = baseline;
        switch (field) {
        case 0: (void)strcpy(changed.project_id,"other"); break;
        case 1: (void)strcpy(changed.unit_id,"other"); break;
        case 2: (void)strcpy(changed.source_path,"other.c"); break;
        case 3: (void)strcpy(changed.output_path,"other.o"); break;
        case 4: (void)strcpy(changed.working_directory,"other"); break;
        case 5: (void)strcpy(changed.standard,"c17"); break;
        case 6: changed.language = UMI_COMPILER_LANGUAGE_CPP; break;
        case 7: changed.action = UMI_COMPILER_ACTION_CHECK; break;
        case 8: (void)strcpy(changed.target.triple,"other"); break;
        case 9: (void)strcpy(changed.target.architecture,"other"); break;
        case 10: (void)strcpy(changed.target.vendor,"other"); break;
        case 11: (void)strcpy(changed.target.operating_system,"other"); break;
        case 12: (void)strcpy(changed.target.environment,"other"); break;
        case 13: changed.target.object_format = UMI_COMPILER_OBJECT_COFF; break;
        case 14: changed.target.pointer_width = 32U; break;
        case 15: changed.target.little_endian = false; break;
        case 16: changed.debug_information = true; break;
        case 17: changed.position_independent = true; break;
        case 18: changed.warnings_as_errors = true; break;
        }
        REQUIRE(umi_compiler_request_hash(&changed) != original);
    }
    uint64_t groups[3];
    for (unsigned group = 0U; group < 3U; ++group) {
        changed = baseline;
        if (group == 0U) { (void)strcpy(changed.include_directories[0],"shared"); changed.include_directory_count = 1U; }
        if (group == 1U) { (void)strcpy(changed.definitions[0],"shared"); changed.definition_count = 1U; }
        if (group == 2U) { (void)strcpy(changed.options[0],"shared"); changed.option_count = 1U; }
        groups[group] = umi_compiler_request_hash(&changed);
        REQUIRE(groups[group] != 0U && groups[group] != original);
    }
    REQUIRE(groups[0] != groups[1] && groups[1] != groups[2] && groups[0] != groups[2]);
    changed = baseline; changed.option_count = 2U;
    (void)strcpy(changed.options[0],"first"); (void)strcpy(changed.options[1],"second");
    uint64_t ordered = umi_compiler_request_hash(&changed);
    (void)strcpy(changed.options[0],"second"); (void)strcpy(changed.options[1],"first");
    REQUIRE(umi_compiler_request_hash(&changed) != ordered);
    /* Inactive bytes, including bytes after a text terminator, are not identity. */
    changed = baseline; (void)memset(changed.options[31],'x',sizeof(changed.options[31]));
    changed.source_path[sizeof(changed.source_path)-1U] = 'x';
    REQUIRE(umi_compiler_request_hash(&changed) == original);
    for (unsigned field = 0U; field < 7U; ++field) {
        changed = baseline;
        switch (field) {
        case 0: changed.include_directory_count = SIZE_MAX; break;
        case 1: changed.definition_count = UMI_COMPILER_MAX_OPTIONS+1U; break;
        case 2: changed.option_count = SIZE_MAX; break;
        case 3: (void)memset(changed.working_directory,'x',sizeof(changed.working_directory)); break;
        case 4: (void)memset(changed.target.vendor,'x',sizeof(changed.target.vendor)); break;
        case 5: changed.option_count = 1U; break; /* Active options cannot be empty. */
        case 6: changed.option_count = 1U; (void)memset(changed.options[0],'x',sizeof(changed.options[0])); break;
        }
        REQUIRE(umi_compiler_request_hash(&changed) == 0U);
    }
    return 0;
}
