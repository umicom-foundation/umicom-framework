/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_property_commit.c
 *
 * PURPOSE:
 *   Validate record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/property_commit.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadPropertyCommit item;CHECK(umi_rad_property_commit_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_commit_is_valid(&item));return 0;}
