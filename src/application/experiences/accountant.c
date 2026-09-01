/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/accountant.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Accountant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/accountant.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Accountant offers bookkeeping, payroll and controller starting layouts.
 * These recipes arrange shared finance and people panels; users may still
 * dock, float, group, hide and save panels through Framework customisation. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "dashboard", "Business Home",
      "Cash, receivables, payables and profit", "umicom.accounting", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "ledger", "General Ledger",
      "Accounts, journals and balances", "umicom.accounting", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "banking", "Bank Reconciliation",
      "Feeds, matching, rules and reconciliation", "umicom.banking", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "sales", "Sales",
      "Quotes, invoices, receipts and customers", "umicom.accounting", "finance.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "purchases", "Purchases",
      "Bills, payments and suppliers", "umicom.accounting", "finance.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "documents", "Documents",
      "Receipts, invoices and evidence", "umicom.audit", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "employees", "Employees",
      "Employee records and HR data", "umicom.hr", "people.purple", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "payroll", "Payroll",
      "Gross-to-net pay run and postings", "umicom.payroll", "people.purple", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "timesheets", "Timesheets",
      "Time and attendance", "umicom.hr", "people.purple", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "leave", "Leave",
      "Holiday and absence", "umicom.hr", "people.purple", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "expenses", "Expenses",
      "Employee expenses and approvals", "umicom.accounting", "people.purple", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "reports", "Reports",
      "P&L, Balance Sheet, Cash Flow and Trial Balance", "umicom.accounting", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "close", "Period Close",
      "Month-end checklist and period lock", "umicom.accounting", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "reconciliation", "Reconciliation",
      "Bank/subledger/GL reconciliation", "umicom.reconciliation", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "audit", "Audit",
      "Evidence, approvals and history", "umicom.audit", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_BOOKKEEPING[] = {"dashboard", "ledger", "banking", "sales", "purchases", "documents"};
static const char *const LAYOUT_PAYROLL[] = {"employees", "payroll", "timesheets", "leave", "expenses"};
static const char *const LAYOUT_CONTROLLER[] = {"reports", "close", "reconciliation", "ledger", "audit"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "bookkeeping", "Bookkeeping",
      "Business home, ledger and banking", LAYOUT_BOOKKEEPING, COUNT_OF(LAYOUT_BOOKKEEPING), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "payroll", "Payroll & HR",
      "Employees, pay run, time and leave", LAYOUT_PAYROLL, COUNT_OF(LAYOUT_PAYROLL), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "controller", "Financial Controller",
      "Reports, budgets and close", LAYOUT_CONTROLLER, COUNT_OF(LAYOUT_CONTROLLER), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "accountant.ledger", "General ledger",
      "Chart, journals and financial statements", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.sales-purchases", "Sales/purchases",
      "Receivables, payables and documents", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.bank", "Connected banking",
      "Bank feeds/import/matching/reconciliation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.banking" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.payroll", "Payroll",
      "Pay runs, statutory adapters and journals", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.payroll" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.hr", "HR",
      "Employees, leave, time and expenses", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.hr" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.reporting", "Financial reporting",
      "P&L, BS, cash flow, budgets and drill-down", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.close", "Period close",
      "Approvals, blockers, lock and audit", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "accountant.ui", "SME application clients",
      "Owner/bookkeeper/payroll/controller workspaces", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.accountant",
    "Umicom Accountant",
    "bookkeeping",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_accountant(void)
{
    return &DEFINITION;
}
