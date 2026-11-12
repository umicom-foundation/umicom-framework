/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_centre_fixture.h
 *
 * PURPOSE:
 *   Share serial, off-stack compiler-centre metadata fixtures without launching a driver.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_COMPILER_CENTRE_FIXTURE_H
#define UMICOM_TEST_COMPILER_CENTRE_FIXTURE_H
#include "umicom/compiler/compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CENTRE_REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"centre fixture %s:%d\n",__FILE__,__LINE__); return 1; } } while (0)
/* Large inline owners live off the process stack. The fixture is serial and its
 * provider executable deliberately carries no availability qualification. */
static int CentreFixture(UmiCompilerCentre *centre)
{
    static UmiCompilerProfile profile;
    static UmiCompilerProjectManifest manifest;
    UmiCompilerUnitManifest unit = {0};
    UmiCompilerTarget target;
    umi_compiler_centre_init(centre);
    UmiCompilerProvider provider = umi_compiler_gcc_provider("unverified-gcc","x86_64-unknown-linux-gnu");
    CENTRE_REQUIRE(umi_compiler_registry_register(&centre->providers,&provider) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_target_parse("x86_64-unknown-linux-gnu",&target) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_profile_init(&profile,"debug","Debug","gcc",UMI_COMPILER_LANGUAGE_C,&target) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&profile) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_manifest_init(&manifest,"project","Project",target.triple) == UMI_STATUS_OK);
    (void)strcpy(unit.unit_id,"main"); (void)strcpy(unit.source,"src/main.c"); (void)strcpy(unit.output,"build/main.o");
    unit.language = UMI_COMPILER_LANGUAGE_C;
    CENTRE_REQUIRE(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&manifest) == UMI_STATUS_OK);
    return 0;
}
#endif
