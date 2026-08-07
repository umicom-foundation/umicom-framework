# ADR-0005: Independent applications and composable suites

Status: Accepted

Each Umicom application is independently buildable and launchable and may bundle a compatible Framework source/runtime. Compatible application folders contain a versioned manifest and may be imported into a suite. The suite selects one compatible shared Framework build while applications remain separate executables. Separate processes are preferred for critical Studio, Trader and TMS deployments.
