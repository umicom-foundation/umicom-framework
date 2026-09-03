/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/database/database_migrations.inc
 *
 * PURPOSE:
 *   Define the umicom.database.migrations reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.database.migrations", "database", "Migration Centre", "umicom.data.migrations",
         "Plan, validate, apply and evidence schema changes.", UMI_APPLICATION_COMPONENT_DASHBOARD,
         UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "sql nosql data query"),
