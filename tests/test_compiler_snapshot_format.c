/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_snapshot_format.c
 *
 * PURPOSE:
 *   Verify complete compiler snapshot text, bounded input and unchanged output on every refused formatting request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/centre.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { (void)fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c); return 1; } } while (0)
int main(void)
{
    UmiCompilerCentreSnapshot snapshot = {0}, saved_snapshot;
    char output[1024],saved[1024];
    snapshot.providers = 2U;snapshot.available_providers = 1U;snapshot.profiles = 1U;
    snapshot.manifests = 1U;snapshot.units = 2U;snapshot.compilation_commands = 2U;
    (void)strcpy(snapshot.active_profile_id,"project-debug");
    const char *expected = "Umicom Compiler Centre\nProviders: 2 (1 available)\nProfiles: 1\n"
        "Projects: 1\nUnits: 2\nCompilation commands: 2\nActive profile: project-debug\n";
    (void)memset(output,0x5a,sizeof(output));
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,sizeof(output)) == UMI_STATUS_OK);
    CHECK(strcmp(output,expected) == 0);
    CHECK(output[strlen(expected)+1U] == 0x5a);
    (void)memcpy(saved,output,sizeof(saved));
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,strlen(expected)) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(saved,output,sizeof(saved)) == 0);
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,strlen(expected)+1U) == UMI_STATUS_OK);
    snapshot.available_providers = 3U;
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(saved,output,sizeof(saved)) == 0);
    snapshot.available_providers = 1U;
    snapshot.compilation_commands = UMI_POLYGLOT_DATABASE_MAX + 1U;
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(saved,output,sizeof(saved)) == 0);
    snapshot.compilation_commands = 2U;
    (void)memset(snapshot.active_profile_id,'x',sizeof(snapshot.active_profile_id));
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,sizeof(output)) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(saved,output,sizeof(saved)) == 0);
    (void)strcpy(snapshot.active_profile_id,"project-debug");
    (void)memcpy(&saved_snapshot,&snapshot,sizeof(snapshot));
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,snapshot.active_profile_id,sizeof(snapshot.active_profile_id)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&saved_snapshot,&snapshot,sizeof(snapshot)) == 0);
    CHECK(umi_compiler_centre_format_snapshot(NULL,output,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,NULL,sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved,output,sizeof(saved)) == 0);
    snapshot.revision = UINT64_MAX;
    CHECK(umi_compiler_centre_format_snapshot(&snapshot,output,sizeof(output)) == UMI_STATUS_OK);
    return 0;
}
