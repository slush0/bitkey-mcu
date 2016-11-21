/*
 * This file is part of the TREZOR project.
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

#ifndef __PROTECT_H__
#define __PROTECT_H__

#include <stdbool.h>
#include "storage.h"
#include "types.pb.h"
#include "ccid.h"

bool protectButton(ButtonRequestType type, bool confirm_only);
bool ccidProtectButton(bool confirm_only, const CCID_HEADER *header);
bool protectPin(bool use_cached);
uint32_t *ccidPinWait(const CCID_HEADER *header);
bool protectChangePin(void);
bool protectPassphrase(void);

extern bool protectAbortedByInitialize;

static inline bool protectUnlockedPin(bool use_cached) {
	return !storage_hasPin() || (use_cached && session_isPinCached());
}

static inline bool protectUnlockedPassphrase(void) {
	return !storage.has_passphrase_protection || !storage.passphrase_protection || session_isPassphraseCached();
}

static inline bool protectUnlocked(bool use_cached) {
	return protectUnlockedPin(use_cached) && protectUnlockedPassphrase();
}

#endif
