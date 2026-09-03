/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/database/database_query.inc
 *
 * PURPOSE:
 *   Define the umicom.database.query reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.database.query", "database", "Query Editor", "umicom.data",
         "Safe queries, parameters, plans, results and history.", UMI_APPLICATION_COMPONENT_EDITOR,
         UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "sql nosql data query"),
