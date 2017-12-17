/*
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UTIL_H_
#define __UTIL_H_

#include <stdint.h>

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>

// Statement expressions make these macros side-effect safe
#define MIN(_a, _b) ({ __auto_type a = (_a); __auto_type b = (_b); a < b ? a : b; })
#define MAX(_a, _b) ({ __auto_type a = (_a); __auto_type b = (_b); a > b ? a : b; })

void delay(uint32_t wait);

// converts uint32 to hexa (8 digits)
void uint32hex(uint32_t num, char *str);

// converts data to hexa
void data2hex(const void *data, uint32_t len, char *str);

// read protobuf integer and advance pointer
uint32_t readprotobufint(uint8_t **ptr);

// halt execution (or do an endless loop)
void __attribute__((noreturn)) system_halt(void);
// reset system
void __attribute__((noreturn)) system_reset(void);

// defined in memory.ld
extern uint8_t _ram_start[], _ram_end[];

// defined in startup.s
extern void memset_reg(void *start, void *stop, uint32_t val);

static inline void __attribute__((noreturn)) load_vector_table(const vector_table_t *vector_table)
{
	// Relocate vector table
	SCB_VTOR = (uint32_t)vector_table;

	// Set stack pointer
	__asm__ volatile("msr msp, %0" :: "r" (vector_table->initial_sp_value));

	// Jump to address
	vector_table->reset();

	// Prevent compiler from generating stack protector code (which causes CPU fault because the stack is moved)
	for (;;);
}

#endif
