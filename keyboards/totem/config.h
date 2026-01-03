// Copyright 2022 GEIST @geigeigeist
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define MATRIX_ROWS 8
#define MATRIX_COLS 5

// Half-duplex serial for single-wire TRRS connection
#define USE_SERIAL
#define SOFT_SERIAL_PIN GP0
#define SERIAL_USE_MULTI_TRANSACTION
#define SERIAL_PIO_USE_PIO1

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U
