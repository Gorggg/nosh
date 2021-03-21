/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_KBDMAP_UTILS_H)
#define INCLUDE_KBDMAP_UTILS_H

#include <stdint.h>

extern
uint16_t
bsd_keycode_to_keymap_index (
	const uint16_t k
) ;
#if !defined(__LINUX__) && !defined(__linux__)
extern
uint16_t
usb_ident_to_keymap_index (
	const uint32_t ident
) ;
#else
extern
uint16_t
linux_evdev_keycode_to_keymap_index (
	const uint16_t k
) ;
#endif
#if defined(__OpenBSD__) || defined(__NetBSD__)
extern
uint16_t
wscons_xt_keycode_to_keymap_index (
	const uint16_t k
) ;
extern
uint16_t
wscons_sgi_keycode_to_keymap_index (
	const uint16_t k
) ;
#if defined(__OpenBSD__)
extern
uint16_t
rawkey_keycode_to_keymap_index (
	const uint16_t k
) ;
#elif defined(__NetBSD__)
extern
uint16_t
wscons_adb_keycode_to_keymap_index (
	const uint16_t k
) ;
extern
uint16_t
wscons_sun_keycode_to_keymap_index (
	const uint16_t k
) ;
extern
uint16_t
wscons_ews_keycode_to_keymap_index (
	const uint16_t k
) ;
#endif
#endif

#endif
