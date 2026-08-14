// Umicom polyglot Zig unit | Sammy Hegab | Umicom Foundation | MIT
export fn umicom_clamp(value: c_int) c_int { return if (value < 0) 0 else value; }
