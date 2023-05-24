/* Copyright (c) Kuba Szczodrzyński 2023-03-02. */

#pragma once

#include LT_VARIANT_H

// Choose the main UART output port
#ifndef LT_UART_DEFAULT_PORT
#if HAS_SERIAL2
#define LT_UART_DEFAULT_PORT 2
#elif HAS_SERIAL0
#define LT_UART_DEFAULT_PORT 0
#elif HAS_SERIAL1
#define LT_UART_DEFAULT_PORT 1
#else
#error "No serial port is available"
#endif
#endif

// clang-format off
// Auto-download-reboot detection pattern
// Family ID, big-endian
#define LT_UART_ADR_PATTERN 0x55, 0xAA, \
	(FAMILY >> 24) & 0xFF, \
	(FAMILY >> 16) & 0xFF, \
	(FAMILY >> 8) & 0xFF, \
	(FAMILY >> 0) & 0xFF
// clang-format on
