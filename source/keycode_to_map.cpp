/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if defined(__LINUX__) || defined(__linux__)
#include <linux/input.h>
#else
#include <sys/kbio.h>
#endif
#include "kbdmap_utils.h"

/// The operating system keycode maps to a row+column in the current keyboard map, which contains an action for that row+column.
uint16_t
keycode_to_keymap_index (
	uint16_t k
) {
#if defined(__LINUX__) || defined(__linux__)
	switch (k) {
		default:		break;
		case KEY_ESC:		return 0x0001;
		case KEY_1:		return 0x0002;
		case KEY_2:		return 0x0003;
		case KEY_3:		return 0x0004;
		case KEY_4:		return 0x0005;
		case KEY_5:		return 0x0006;
		case KEY_6:		return 0x0007;
		case KEY_7:		return 0x0008;
		case KEY_8:		return 0x0009;
		case KEY_9:		return 0x000A;
		case KEY_0:		return 0x000B;
		case KEY_MINUS:		return 0x000C;
		case KEY_EQUAL:		return 0x000D;
		case KEY_BACKSPACE:	return 0x000E;
		case KEY_TAB:		return 0x0100;
		case KEY_Q:		return 0x0101;
		case KEY_W:		return 0x0102;
		case KEY_E:		return 0x0103;
		case KEY_R:		return 0x0104;
		case KEY_T:		return 0x0105;
		case KEY_Y:		return 0x0106;
		case KEY_U:		return 0x0107;
		case KEY_I:		return 0x0108;
		case KEY_O:		return 0x0109;
		case KEY_P:		return 0x010A;
		case KEY_LEFTBRACE:	return 0x010B;
		case KEY_RIGHTBRACE:	return 0x010C;
		case KEY_ENTER:		return 0x010D;
		case KEY_A:		return 0x0201;
		case KEY_S:		return 0x0202;
		case KEY_D:		return 0x0203;
		case KEY_F:		return 0x0204;
		case KEY_G:		return 0x0205;
		case KEY_H:		return 0x0206;
		case KEY_J:		return 0x0207;
		case KEY_K:		return 0x0208;
		case KEY_L:		return 0x0209;
		case KEY_SEMICOLON:	return 0x020A;
		case KEY_APOSTROPHE:	return 0x020B;
		case KEY_GRAVE:		return 0x020C;
		case KEY_LINEFEED:	return 0x020F;
		case KEY_BACKSLASH:	return 0x0301;
		case KEY_Z:		return 0x0302;
		case KEY_X:		return 0x0303;
		case KEY_C:		return 0x0304;
		case KEY_V:		return 0x0305;
		case KEY_B:		return 0x0306;
		case KEY_N:		return 0x0307;
		case KEY_M:		return 0x0308;
		case KEY_COMMA:		return 0x0309;
		case KEY_DOT:		return 0x030A;
		case KEY_SLASH:		return 0x030B;
		case KEY_102ND:		return 0x030E;
		case KEY_YEN:		return 0x030F;
		case KEY_LEFTSHIFT:	return 0x0400;
		case KEY_RIGHTSHIFT:	return 0x0401;
		case KEY_RIGHTALT:	return 0x0402;
		case KEY_LEFTCTRL:	return 0x0404;
		case KEY_RIGHTCTRL:	return 0x0405;
		case KEY_LEFTMETA:	return 0x0406;
		case KEY_RIGHTMETA:	return 0x0407;
		case KEY_LEFTALT:	return 0x0408;
		case KEY_CAPSLOCK:	return 0x040C;
		case KEY_SCROLLLOCK:	return 0x040D;
		case KEY_NUMLOCK:	return 0x040E;
		case KEY_RO:		return 0x0500;
		case KEY_KATAKANAHIRAGANA:	return 0x0502;
		case KEY_ZENKAKUHANKAKU:return 0x0502;
		case KEY_HIRAGANA:	return 0x0503;
		case KEY_KATAKANA:	return 0x0504;
		case KEY_HENKAN:	return 0x0505;
		case KEY_MUHENKAN:	return 0x0506;
		case KEY_HANGEUL:	return 0x0508;
		case KEY_HANJA:		return 0x0509;
		case KEY_COMPOSE:	return 0x050E;
		case KEY_SPACE:		return 0x050F;
		case KEY_F1:		return 0x0900;
		case KEY_F2:		return 0x0901;
		case KEY_F3:		return 0x0902;
		case KEY_F4:		return 0x0903;
		case KEY_F5:		return 0x0904;
		case KEY_F6:		return 0x0905;
		case KEY_F7:		return 0x0906;
		case KEY_F8:		return 0x0907;
		case KEY_F9:		return 0x0908;
		case KEY_F10:		return 0x0909;
		case KEY_F11:		return 0x090A;
		case KEY_F12:		return 0x090B;
		case KEY_F13:		return 0x090C;
		case KEY_F14:		return 0x090D;
		case KEY_F15:		return 0x090E;
		case KEY_F16:		return 0x090F;
		case KEY_F17:		return 0x0A00;
		case KEY_F18:		return 0x0A01;
		case KEY_F19:		return 0x0A02;
		case KEY_F20:		return 0x0A03;
		case KEY_F21:		return 0x0A04;
		case KEY_F22:		return 0x0A05;
		case KEY_F23:		return 0x0A06;
		case KEY_F24:		return 0x0A07;
		case KEY_HOME:		return 0x0600;
		case KEY_UP:		return 0x0601;
		case KEY_PAGEUP:	return 0x0602;
		case KEY_LEFT:		return 0x0603;
		case KEY_RIGHT:		return 0x0604;
		case KEY_END:		return 0x0605;
		case KEY_DOWN:		return 0x0606;
		case KEY_PAGEDOWN:	return 0x0607;
		case KEY_INSERT:	return 0x0608;
		case KEY_DELETE:	return 0x0609;
		case KEY_KPASTERISK:	return 0x0700;
		case KEY_KP7:		return 0x0701;
		case KEY_KP8:		return 0x0702;
		case KEY_KP9:		return 0x0703;
		case KEY_KPMINUS:	return 0x0704;
		case KEY_KP4:		return 0x0705;
		case KEY_KP5:		return 0x0706;
		case KEY_KP6:		return 0x0707;
		case KEY_KPPLUS:	return 0x0708;
		case KEY_KP1:		return 0x0709;
		case KEY_KP2:		return 0x070A;
		case KEY_KP3:		return 0x070B;
		case KEY_KP0:		return 0x070C;
		case KEY_KPDOT:		return 0x070D;
		case KEY_KPENTER:	return 0x070E;
		case KEY_KPSLASH:	return 0x070F;
		case KEY_KPJPCOMMA:	return 0x0800;
		case KEY_KPCOMMA:	return 0x0801;
		case KEY_KPEQUAL:	return 0x0802;
		case KEY_KPPLUSMINUS:	return 0x0803;
		case KEY_KPLEFTPAREN:	return 0x0804;
		case KEY_KPRIGHTPAREN:	return 0x0805;
		case KEY_PAUSE:		return 0x0D00;
		case KEY_SYSRQ:		return 0x0D02;
		case KEY_STOP:		return 0x0E01;
		case KEY_AGAIN:		return 0x0E02;
		case KEY_PROPS:		return 0x0E03;
		case KEY_UNDO:		return 0x0E04;
		case KEY_REDO:		return 0x0E05;
		case KEY_COPY:		return 0x0E06;
		case KEY_OPEN:		return 0x0E07;
		case KEY_PASTE:		return 0x0E08;
		case KEY_FIND:		return 0x0E09;
		case KEY_CUT:		return 0x0E0A;
		case KEY_HELP:		return 0x0E0B;
		case KEY_MUTE:		return 0x0E0C;
		case KEY_VOLUMEDOWN:	return 0x0E0D;
		case KEY_VOLUMEUP:	return 0x0E0E;
		case KEY_CALC:		return 0x0F00;
		case KEY_FILE:		return 0x0F01;
		case KEY_WWW:		return 0x0F02;
		case KEY_HOMEPAGE:	return 0x0F03;
		case KEY_REFRESH:	return 0x0F04;
		case KEY_MAIL:		return 0x0F05;
		case KEY_BOOKMARKS:	return 0x0F06;
		case KEY_COMPUTER:	return 0x0F07;
		case KEY_BACK:		return 0x0F08;
		case KEY_FORWARD:	return 0x0F09;
		case KEY_SCREENLOCK:	return 0x0F0A;
		case KEY_MSDOS:		return 0x0F0B;
		case KEY_NEXTSONG:	return 0x0F0C;
		case KEY_PREVIOUSSONG:	return 0x0F0D;
		case KEY_PLAYPAUSE:	return 0x0F0E;
		case KEY_STOPCD:	return 0x0F0F;
		case KEY_RECORD:	return 0x1000;
		case KEY_REWIND:	return 0x1001;
		case KEY_FASTFORWARD:	return 0x1002;
		case KEY_EJECTCD:	return 0x1003;
		case KEY_NEW:		return 0x1004;
		case KEY_EXIT:		return 0x1005;
		case KEY_POWER:		break;
		case KEY_SLEEP:		break;
		case KEY_WAKEUP:	break;
	}
#else
	switch (k) {
		default:	break;
		case 0x01:	return 0x0001;
		case 0x02:	return 0x0002;
		case 0x03:	return 0x0003;
		case 0x04:	return 0x0004;
		case 0x05:	return 0x0005;
		case 0x06:	return 0x0006;
		case 0x07:	return 0x0007;
		case 0x08:	return 0x0008;
		case 0x09:	return 0x0009;
		case 0x0A:	return 0x000A;
		case 0x0B:	return 0x000B;
		case 0x0C:	return 0x000C;
		case 0x0D:	return 0x000D;
		case 0x0E:	return 0x000E;
		case 0x0F:	return 0x0100;
		case 0x10:	return 0x0101;
		case 0x11:	return 0x0102;
		case 0x12:	return 0x0103;
		case 0x13:	return 0x0104;
		case 0x14:	return 0x0105;
		case 0x15:	return 0x0106;
		case 0x16:	return 0x0107;
		case 0x17:	return 0x0108;
		case 0x18:	return 0x0109;
		case 0x19:	return 0x010A;
		case 0x1a:	return 0x010B;
		case 0x1b:	return 0x010C;
		case 0x1c:	return 0x010D;
		case 0x1d:	return 0x0404;
		case 0x1e:	return 0x0201;
		case 0x1f:	return 0x0202;
		case 0x20:	return 0x0203;
		case 0x21:	return 0x0204;
		case 0x22:	return 0x0205;
		case 0x23:	return 0x0206;
		case 0x24:	return 0x0207;
		case 0x25:	return 0x0208;
		case 0x26:	return 0x0209;
		case 0x27:	return 0x020A;
		case 0x28:	return 0x020B;
		case 0x29:	return 0x020C;
		case 0x2a:	return 0x0400;
		case 0x2b:	return 0x0301;
		case 0x2c:	return 0x0302;
		case 0x2d:	return 0x0303;
		case 0x2e:	return 0x0304;
		case 0x2f:	return 0x0305;
		case 0x30:	return 0x0306;
		case 0x31:	return 0x0307;
		case 0x32:	return 0x0308;
		case 0x33:	return 0x0309;
		case 0x34:	return 0x030A;
		case 0x35:	return 0x030B;
		case 0x36:	return 0x0401;
		case 0x37:	return 0x0700;
		case 0x38:	return 0x0408;
		case 0x39:	return 0x050F;
		case 0x3a:	return 0x040C;
		case 0x3b:	return 0x0900;
		case 0x3c:	return 0x0901;
		case 0x3d:	return 0x0902;
		case 0x3e:	return 0x0903;
		case 0x3f:	return 0x0904;
		case 0x40:	return 0x0905;
		case 0x41:	return 0x0906;
		case 0x42:	return 0x0907;
		case 0x43:	return 0x0908;
		case 0x44:	return 0x0909;
		case 0x45:	return 0x040E;
		case 0x46:	return 0x040D;
		case 0x47:	return 0x0701;
		case 0x48:	return 0x0702;
		case 0x49:	return 0x0703;
		case 0x4a:	return 0x0704;
		case 0x4b:	return 0x0705;
		case 0x4c:	return 0x0706;
		case 0x4d:	return 0x0707;
		case 0x4e:	return 0x0708;
		case 0x4f:	return 0x0709;
		case 0x50:	return 0x070A;
		case 0x51:	return 0x070B;
		case 0x52:	return 0x070C;
		case 0x53:	return 0x070D;
		case 0x54:	return 0x0D02;
		case 0x56:	return 0x030E;
		case 0x57:	return 0x090A;
		case 0x58:	return 0x090B;
		case 0x59:	return 0x070E;
		case 0x5A:	return 0x0405;
		case 0x5B:	return 0x070F;
		case 0x5C:	return 0x0D01;
		case 0x5D:	return 0x0402;
		case 0x5E:	return 0x0600;
		case 0x5F:	return 0x0601;
		case 0x60:	return 0x0602;
		case 0x61:	return 0x0603;
		case 0x62:	return 0x0604;
		case 0x63:	return 0x0605;
		case 0x64:	return 0x0606;
		case 0x65:	return 0x0607;
		case 0x66:	return 0x0608;
		case 0x67:	return 0x0609;
		case 0x68:	return 0x0D00;
		case 0x69:	return 0x0406;
		case 0x6A:	return 0x0407;
		case 0x6B:	return 0x0E00;
		case 0x6C:	return 0x0D04;
		case 0x6D:	/*power*/ break;
		case 0x6E:	/*sleep*/ break;
		case 0x6F:	/*wake*/ break;
		case 0x70:	return 0x0501;
		case 0x73:	return 0x0500;
		case 0x76:	return 0x0502;
		case 0x77:	return 0x0503;
		case 0x78:	return 0x0504;
		case 0x79:	return 0x0505;
		case 0x7B:	return 0x0506;
		case 0x7D:	return 0x030F;
		case 0x7E:	return 0x0800;
	}
#endif
	return 0xFFFF;
}
