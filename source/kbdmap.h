/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_KBDMAP_H)
#define INCLUDE_KBDMAP_H

enum { KBDMAP_ROWS = 0x11, KBDMAP_COLS = 0x10 };

/// The entries in a keyboard map file are 10 bigendian 32-bit integers that unpack to this.
struct kbdmap_entry {
	uint32_t cmd,p[16];
};

enum {
	KBDMAP_ACTION_MASK	= 0xFF000000,
	KBDMAP_ACTION_UCS24	= 0x01000000,
	KBDMAP_ACTION_SYSTEM	= 0x02000000,
	KBDMAP_ACTION_MODIFIER	= 0x03000000,
	KBDMAP_ACTION_SCREEN	= 0x0A000000,
	KBDMAP_ACTION_CONSUMER	= 0x0C000000,
	KBDMAP_ACTION_EXTENDED	= 0x0E000000,
	KBDMAP_ACTION_FUNCTION	= 0x0F000000,
	KBDMAP_ACTION_FUNCTION1	= 0x1F000000,
};

enum {
	KBDMAP_MODIFIER_1ST_LEVEL2	=  0U,
	KBDMAP_MODIFIER_1ST_LEVEL3	=  1U,
	KBDMAP_MODIFIER_1ST_GROUP2	=  2U,
	KBDMAP_MODIFIER_1ST_CONTROL	=  3U,
	KBDMAP_MODIFIER_1ST_SUPER	=  4U,
	KBDMAP_MODIFIER_1ST_ALT		=  5U,
	KBDMAP_MODIFIER_1ST_META	=  6U,
	KBDMAP_MODIFIER_2ND_LEVEL2	=  8U,
	KBDMAP_MODIFIER_2ND_LEVEL3	=  9U,
	KBDMAP_MODIFIER_2ND_GROUP2	= 10U,
	KBDMAP_MODIFIER_2ND_CONTROL	= 11U,
	KBDMAP_MODIFIER_2ND_SUPER	= 12U,
	KBDMAP_MODIFIER_2ND_ALT		= 13U,
	KBDMAP_MODIFIER_2ND_META	= 14U,
	KBDMAP_MODIFIER_LEVEL2		= 16U,
	KBDMAP_MODIFIER_LEVEL3		= 17U,
	KBDMAP_MODIFIER_CAPS		= 18U,
	KBDMAP_MODIFIER_NUM		= 19U,
	KBDMAP_MODIFIER_SCROLL		= 20U,
};

enum {
	KBDMAP_MODIFIER_CMD_MOMENTARY	= 0x01,	///< a momentary contact version of the modifier
	KBDMAP_MODIFIER_CMD_LATCH	= 0x02,	///< a latching version of the modifier
	KBDMAP_MODIFIER_CMD_LOCK	= 0x03,	///< a locking version of the modifier
};

#endif
