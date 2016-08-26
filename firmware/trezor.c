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

#include "trezor.h"
#include "oled.h"
#include "bitmaps.h"
#include "util.h"
#include "usb.h"
#include "setup.h"
#include "storage.h"
#include "layout.h"
#include "layout2.h"
#include "rng.h"

#include "buttons.h"

uint32_t __stack_chk_guard;

void __attribute__((noreturn)) __stack_chk_fail(void)
{
	layoutDialog(&bmp_icon_error, NULL, NULL, NULL, "Stack smashing", "detected.", NULL, "Please unplug", "the device.", NULL);
	for (;;) {} // loop forever
}

void lock_device(void)
{
	static bool lockButtonDown = false;

	buttonUpdate();
	if (button.YesDown >= 100000) {
		/* Button held down for long enough, wait for button release */
		lockButtonDown = true;
	} else if (lockButtonDown && button.YesUp) {
		/* Button released */
		lockButtonDown = false;

		layoutDialog(&bmp_icon_question, "Cancel", "Lock Device", NULL, "Do you really want to", "lock your TREZOR?", NULL, NULL, NULL, NULL);

        usbTiny(1);
		do {
			usbDelay(3300);
			buttonUpdate();
		} while (!button.YesUp && !button.NoUp);
        usbTiny(0);

		if (button.YesUp) {
			/* Lock the device */
			session_clear(true);
			layoutScreensaver();
		} else {
			layoutHome();
		}
	} else {
		/* Button possibly released during usbPoll() */
		lockButtonDown = false;
	}
}

int main(void)
{
	__stack_chk_guard = random32();
#ifndef APPVER
	setup();
	oledInit();
#else
	setupApp();
#endif
#if DEBUG_LINK
	oledSetDebug(1);
	storage_reset(); // wipe storage if debug link
	storage_reset_uuid();
	storage_commit();
	storage_clearPinArea(); // reset PIN failures if debug link
#endif

	oledDrawBitmap(40, 0, &bmp_logo64);
	oledRefresh();

	storage_init();
	layoutHome();
	usbInit();
	for (;;) {
		usbPoll();
        lock_device();
	}

	return 0;
}
