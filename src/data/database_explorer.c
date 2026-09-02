/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/database_explorer.c
 *
 * PURPOSE:
 *   Implement the database explorer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - Database Explorer orchestration implementation
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/database_explorer.h"
#include <string.h>
/*
 * Provide the add active connection operation used by this module and its client
 * applications.
 */
static UmiStatus add_active_connection(UmiDatabaseExplorer *e){UmiDatabaseConnectionProfile p={0};UmiStatus s=umi_database_copy_text(p.id,sizeof(p.id),"active-data-server");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(p.display_name,sizeof(p.display_name),"Active Data Server");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(p.endpoint,sizeof(p.endpoint),umi_data_server_path(e->server));p.provider=umi_data_server_backend(e->server)==UMI_DATA_BACKEND_SQLITE?UMI_DATABASE_PROVIDER_SQLITE:UMI_DATABASE_PROVIDER_MEMORY;p.timeout_ms=5000U;p.state=UMI_DATABASE_READY;return s==UMI_STATUS_OK?umi_database_connection_registry_upsert(&e->connections,&p):s;}
/* Provide the add kv schema operation used by this module and its client applications. */
static UmiStatus add_kv_schema(UmiDatabaseExplorer *e){UmiDatabaseTableInfo t={0};UmiDatabaseColumnInfo c={0};UmiStatus s=umi_database_copy_text(t.schema_name,sizeof(t.schema_name),"main");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(t.name,sizeof(t.name),"umicom_kv");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(c.name,sizeof(c.name),"key");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(c.type_name,sizeof(c.type_name),"text");c.primary_key=1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_table_add_column(&t,&c);(void)memset(&c,0,sizeof(c));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(c.name,sizeof(c.name),"value");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(c.type_name,sizeof(c.type_name),"text");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)s=umi_database_table_add_column(&t,&c);t.estimated_rows=umi_data_server_count(e->server);return s==UMI_STATUS_OK?umi_database_schema_catalog_upsert(&e->schema,&t):s;}
/*
 * Initialise database explorer from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_database_explorer_init(UmiDatabaseExplorer *e,UmiDataServer *server){UmiDatabaseQueryPolicy p;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||server==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(e,0,sizeof(*e));e->server=server;s=umi_database_connection_registry_init(&e->connections);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=add_active_connection(e);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_schema_catalog_init(&e->schema);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=add_kv_schema(e);p=umi_database_query_policy_default();/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_query_session_init(&e->queries,server,&p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_explorer_transaction_init(&e->transaction,server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_migration_catalog_init(&e->migrations);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_backup_catalog_init(&e->backups);e->revision=1U;return s;}
/*
 * Provide the database explorer snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_explorer_snapshot(const UmiDatabaseExplorer *e,UmiDatabaseExplorerSnapshot *out){UmiDataServerSnapshot s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));st=umi_data_server_snapshot(e->server,&s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;st=umi_database_copy_text(out->backend,sizeof(out->backend),s.backend_name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_database_copy_text(out->path,sizeof(out->path),s.path);out->records=s.record_count;out->connections=e->connections.count;out->tables=e->schema.table_count;out->query_history=e->queries.history.count;out->pending_migrations=umi_database_migration_catalog_pending(&e->migrations);out->transfers=e->transfer_count;out->backups=e->backups.count;out->transaction_active=umi_data_server_in_transaction(e->server);out->revision=e->revision;return st;}
/*
 * Provide the database explorer add transfer operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_explorer_add_transfer(UmiDatabaseExplorer *e,const UmiDatabaseTransferPlan *p){char m[256];UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||p==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_database_transfer_plan_validate(p,m,sizeof(m));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->transfer_count>=UMI_DATABASE_MAX_TRANSFERS)return UMI_STATUS_CAPACITY_EXCEEDED;e->transfers[e->transfer_count++]=*p;e->revision+=1U;return UMI_STATUS_OK;}
