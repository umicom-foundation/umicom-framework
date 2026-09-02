/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_anchor_constraint.c
 *
 * PURPOSE:
 *   Validate describe edge anchors for adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/anchor_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadAnchorConstraint item;CHECK(umi_rad_anchor_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_anchor_constraint_is_valid(&item));return 0;}
