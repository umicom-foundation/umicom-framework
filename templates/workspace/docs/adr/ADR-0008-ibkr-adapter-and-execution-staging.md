# ADR-0008: IBKR adapter and staged execution

Status: Accepted

Interactive Brokers is integrated behind a broker-neutral stable C contract. Vendor C++ classes remain inside an optional private adapter and the official SDK is not redistributed. Delivery proceeds from read-only connection and market data to paper execution, reconciliation and independent risk. Live trading remains disabled until explicit operational gates are satisfied.
