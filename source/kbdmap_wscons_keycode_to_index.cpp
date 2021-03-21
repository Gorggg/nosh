/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if defined(__OpenBSD__) || defined(__NetBSD__)

#include "kbdmap.h"
#include "kbdmap_utils.h"
#if defined(__OpenBSD__)
#include <dev/wscons/wskbdraw.h>
#endif

// Many WSCons keyboard devices will send raw keyboard events as XT scancodes, others will not
// The WSCons keyboard types that support raw output but do not send XT scancodes have an additional table here. 

uint16_t
wscons_xt_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x01:	return KBDMAP_INDEX_ESC;
		case 0x02:	return KBDMAP_INDEX_1;
		case 0x03:	return KBDMAP_INDEX_2;
		case 0x04:	return KBDMAP_INDEX_3;
		case 0x05:	return KBDMAP_INDEX_4;
		case 0x06:	return KBDMAP_INDEX_5;
		case 0x07:	return KBDMAP_INDEX_6;
		case 0x08:	return KBDMAP_INDEX_7;
		case 0x09:	return KBDMAP_INDEX_8;
		case 0x0a:	return KBDMAP_INDEX_9;
		case 0x0B:	return KBDMAP_INDEX_0;
		case 0x0C:	return KBDMAP_INDEX_MINUS;
		case 0x0D:	return KBDMAP_INDEX_EQUALS;
		case 0x0E:	return KBDMAP_INDEX_BACKSPACE;
		case 0x0F:	return KBDMAP_INDEX_TAB;
		case 0x10:	return KBDMAP_INDEX_Q;
		case 0x11:	return KBDMAP_INDEX_W;
		case 0x12:	return KBDMAP_INDEX_E;
		case 0x13:	return KBDMAP_INDEX_R;
		case 0x14:	return KBDMAP_INDEX_T;
		case 0x15:	return KBDMAP_INDEX_Y;
		case 0x16:	return KBDMAP_INDEX_U;
		case 0x17:	return KBDMAP_INDEX_I;
		case 0x18:	return KBDMAP_INDEX_O;
		case 0x19:	return KBDMAP_INDEX_P;
		case 0x1a:	return KBDMAP_INDEX_LEFTBRACE;
		case 0x1b:	return KBDMAP_INDEX_RIGHTBRACE;
		case 0x1c:	return KBDMAP_INDEX_RETURN;
		case 0x1d:	return KBDMAP_INDEX_CONTROL1;
		case 0x1e:	return KBDMAP_INDEX_A;
		case 0x1f:	return KBDMAP_INDEX_S;
		case 0x20:	return KBDMAP_INDEX_D;
		case 0x21:	return KBDMAP_INDEX_F;
		case 0x22:	return KBDMAP_INDEX_G;
		case 0x23:	return KBDMAP_INDEX_H;
		case 0x24:	return KBDMAP_INDEX_J;
		case 0x25:	return KBDMAP_INDEX_K;
		case 0x26:	return KBDMAP_INDEX_L;
		case 0x27:	return KBDMAP_INDEX_SEMICOLON;
		case 0x28:	return KBDMAP_INDEX_APOSTROPHE;
		case 0x29:	return KBDMAP_INDEX_GRAVE;
		case 0x2a:	return KBDMAP_INDEX_SHIFT1;
		case 0x2b:	return KBDMAP_INDEX_EUROPE1;
		case 0x2c:	return KBDMAP_INDEX_Z;
		case 0x2d:	return KBDMAP_INDEX_X;
		case 0x2e:	return KBDMAP_INDEX_C;
		case 0x2f:	return KBDMAP_INDEX_V;
		case 0x30:	return KBDMAP_INDEX_B;
		case 0x31:	return KBDMAP_INDEX_N;
		case 0x32:	return KBDMAP_INDEX_M;
		case 0x33:	return KBDMAP_INDEX_COMMA;
		case 0x34:	return KBDMAP_INDEX_DOT;
		case 0x35:	return KBDMAP_INDEX_SLASH1;
		case 0x36:	return KBDMAP_INDEX_SHIFT2;
		case 0x37:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0x38:	return KBDMAP_INDEX_ALT;
		case 0x39:	return KBDMAP_INDEX_SPACE;
		case 0x3a:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x3b:	return KBDMAP_INDEX_F1;
		case 0x3c:	return KBDMAP_INDEX_F2;
		case 0x3d:	return KBDMAP_INDEX_F3;
		case 0x3e:	return KBDMAP_INDEX_F4;
		case 0x3f:	return KBDMAP_INDEX_F5;
		case 0x40:	return KBDMAP_INDEX_F6;
		case 0x41:	return KBDMAP_INDEX_F7;
		case 0x42:	return KBDMAP_INDEX_F8;
		case 0x43:	return KBDMAP_INDEX_F9;
		case 0x44:	return KBDMAP_INDEX_F10;
		case 0x45:	return KBDMAP_INDEX_NUMLOCK;
		case 0x46:	return KBDMAP_INDEX_SCROLLLOCK;
		case 0x47:	return KBDMAP_INDEX_KP_7;
		case 0x48:	return KBDMAP_INDEX_KP_8;
		case 0x49:	return KBDMAP_INDEX_KP_9;
		case 0x4a:	return KBDMAP_INDEX_KP_MINUS;
		case 0x4b:	return KBDMAP_INDEX_KP_4;
		case 0x4c:	return KBDMAP_INDEX_KP_5;
		case 0x4d:	return KBDMAP_INDEX_KP_6;
		case 0x4e:	return KBDMAP_INDEX_KP_PLUS;
		case 0x4f:	return KBDMAP_INDEX_KP_1;
		case 0x50:	return KBDMAP_INDEX_KP_2;
		case 0x51:	return KBDMAP_INDEX_KP_3;
		case 0x52:	return KBDMAP_INDEX_KP_0;
		case 0x53:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x56:	return KBDMAP_INDEX_EUROPE2;
		case 0x57:	return KBDMAP_INDEX_F11;
		case 0x58:	return KBDMAP_INDEX_F12;
		case 0x70:	return KBDMAP_INDEX_KATAKANA_HIRAGANA;
		case 0x73:	return KBDMAP_INDEX_SLASH2;
		case 0x79:	return KBDMAP_INDEX_HENKAN;
		case 0x7b:	return KBDMAP_INDEX_MUHENKAN;
		case 0x7d:	return KBDMAP_INDEX_YEN;
		case 0x7f:	return KBDMAP_INDEX_PAUSE;
		case 0x9c:	return KBDMAP_INDEX_KP_ENTER;
		case 0x9d:	return KBDMAP_INDEX_CONTROL2;
		case 0xa0:	return KBDMAP_INDEX_MUTE;
		case 0xaa:	return KBDMAP_INDEX_PRINT_SCREEN;
		case 0xae:	return KBDMAP_INDEX_VOLUME_DOWN;
		case 0xaf:	return KBDMAP_INDEX_VOLUME_UP;
		case 0xb5:	return KBDMAP_INDEX_KP_SLASH;
		case 0xb7:	return KBDMAP_INDEX_PRINT_SCREEN;
		case 0xb8:	return KBDMAP_INDEX_OPTION;
		case 0xc7:	return KBDMAP_INDEX_HOME;
		case 0xc8:	return KBDMAP_INDEX_UP_ARROW;
		case 0xc9:	return KBDMAP_INDEX_PAGE_UP;
		case 0xcb:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0xcd:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0xcf:	return KBDMAP_INDEX_END;
		case 0xd0:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0xd1:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0xd2:	return KBDMAP_INDEX_INSERT;
		case 0xd3:	return KBDMAP_INDEX_DELETE;
		case 0xdb:	return KBDMAP_INDEX_SUPER1;
		case 0xdc:	return KBDMAP_INDEX_SUPER2;
		case 0xdd:	return KBDMAP_INDEX_MENU;
	}
	return 0xFFFF;
}

#if defined(__OpenBSD__)
uint16_t
rawkey_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:			break;
		case RAWKEY_Escape:		return KBDMAP_INDEX_ESC;
		case RAWKEY_1:			return KBDMAP_INDEX_1;
		case RAWKEY_2:			return KBDMAP_INDEX_2;
		case RAWKEY_3:			return KBDMAP_INDEX_3;
		case RAWKEY_4:			return KBDMAP_INDEX_4;
		case RAWKEY_5:			return KBDMAP_INDEX_5;
		case RAWKEY_6:			return KBDMAP_INDEX_6;
		case RAWKEY_7:			return KBDMAP_INDEX_7;
		case RAWKEY_8:			return KBDMAP_INDEX_8;
		case RAWKEY_9:			return KBDMAP_INDEX_9;
		case RAWKEY_0:			return KBDMAP_INDEX_0;
		case RAWKEY_minus:		return KBDMAP_INDEX_MINUS;
		case RAWKEY_equal:		return KBDMAP_INDEX_EQUALS;
		case RAWKEY_Tab:		return KBDMAP_INDEX_TAB;
		case RAWKEY_Q:			return KBDMAP_INDEX_Q;
		case RAWKEY_W:			return KBDMAP_INDEX_W;
		case RAWKEY_E:			return KBDMAP_INDEX_E;
		case RAWKEY_R:			return KBDMAP_INDEX_R;
		case RAWKEY_T:			return KBDMAP_INDEX_T;
		case RAWKEY_Y:			return KBDMAP_INDEX_Y;
		case RAWKEY_U:			return KBDMAP_INDEX_U;
		case RAWKEY_I:			return KBDMAP_INDEX_I;
		case RAWKEY_O:			return KBDMAP_INDEX_O;
		case RAWKEY_P:			return KBDMAP_INDEX_P;
		case RAWKEY_bracketleft:	return KBDMAP_INDEX_LEFTBRACE;
		case RAWKEY_bracketright:	return KBDMAP_INDEX_RIGHTBRACE;
		case RAWKEY_Return:		return KBDMAP_INDEX_RETURN;
		case RAWKEY_Control_L:		return KBDMAP_INDEX_CONTROL1;
		case RAWKEY_a:			return KBDMAP_INDEX_A;
		case RAWKEY_s:			return KBDMAP_INDEX_S;
		case RAWKEY_d:			return KBDMAP_INDEX_D;
		case RAWKEY_f:			return KBDMAP_INDEX_F;
		case RAWKEY_g:			return KBDMAP_INDEX_G;
		case RAWKEY_h:			return KBDMAP_INDEX_H;
		case RAWKEY_j:			return KBDMAP_INDEX_J;
		case RAWKEY_k:			return KBDMAP_INDEX_K;
		case RAWKEY_l:			return KBDMAP_INDEX_L;
		case RAWKEY_semicolon:		return KBDMAP_INDEX_SEMICOLON;
		case RAWKEY_apostrophe:		return KBDMAP_INDEX_APOSTROPHE;
		case RAWKEY_grave:		return KBDMAP_INDEX_GRAVE;
		case RAWKEY_Shift_L:		return KBDMAP_INDEX_SHIFT1;
		case RAWKEY_backslash:		return KBDMAP_INDEX_EUROPE1;
		case RAWKEY_z:			return KBDMAP_INDEX_Z;
		case RAWKEY_x:			return KBDMAP_INDEX_X;
		case RAWKEY_c:			return KBDMAP_INDEX_C;
		case RAWKEY_v:			return KBDMAP_INDEX_V;
		case RAWKEY_b:			return KBDMAP_INDEX_B;
		case RAWKEY_n:			return KBDMAP_INDEX_N;
		case RAWKEY_m:			return KBDMAP_INDEX_M;
		case RAWKEY_comma:		return KBDMAP_INDEX_COMMA;
		case RAWKEY_period:		return KBDMAP_INDEX_DOT;
		case RAWKEY_slash:		return KBDMAP_INDEX_SLASH1;
		case RAWKEY_Shift_R:		return KBDMAP_INDEX_SHIFT2;
		case RAWKEY_KP_Multiply:	return KBDMAP_INDEX_KP_ASTERISK;
		case RAWKEY_Alt_L:		return KBDMAP_INDEX_ALT;
		case RAWKEY_space:		return KBDMAP_INDEX_SPACE;
		case RAWKEY_Caps_Lock:		return KBDMAP_INDEX_CAPSLOCK;
		case RAWKEY_f1:			return KBDMAP_INDEX_F1;
		case RAWKEY_f2:			return KBDMAP_INDEX_F2;
		case RAWKEY_f3:			return KBDMAP_INDEX_F3;
		case RAWKEY_f4:			return KBDMAP_INDEX_F4;
		case RAWKEY_f5:			return KBDMAP_INDEX_F5;
		case RAWKEY_f6:			return KBDMAP_INDEX_F6;
		case RAWKEY_f7:			return KBDMAP_INDEX_F7;
		case RAWKEY_f8:			return KBDMAP_INDEX_F8;
		case RAWKEY_f9:			return KBDMAP_INDEX_F9;
		case RAWKEY_f10:		return KBDMAP_INDEX_F10;
		case RAWKEY_Num_Lock:		return KBDMAP_INDEX_NUMLOCK;
		case RAWKEY_Hold_Screen:	return KBDMAP_INDEX_SCROLLLOCK;
		case RAWKEY_KP_Home:		return KBDMAP_INDEX_KP_7;
		case RAWKEY_KP_Up:		return KBDMAP_INDEX_KP_8;
		case RAWKEY_KP_Prior:		return KBDMAP_INDEX_KP_9;
		case RAWKEY_KP_Subtract:	return KBDMAP_INDEX_KP_MINUS;
		case RAWKEY_KP_Left:		return KBDMAP_INDEX_KP_4;
		case RAWKEY_KP_Begin:		return KBDMAP_INDEX_KP_5;
		case RAWKEY_KP_Right:		return KBDMAP_INDEX_KP_6;
		case RAWKEY_KP_Add:		return KBDMAP_INDEX_KP_PLUS;
		case RAWKEY_KP_End:		return KBDMAP_INDEX_KP_1;
		case RAWKEY_KP_Down:		return KBDMAP_INDEX_KP_2;
		case RAWKEY_KP_Next:		return KBDMAP_INDEX_KP_3;
		case RAWKEY_KP_Insert:		return KBDMAP_INDEX_KP_0;
		case RAWKEY_KP_Delete:		return KBDMAP_INDEX_KP_DECIMAL;
		case RAWKEY_less:		return KBDMAP_INDEX_EUROPE2;
		case RAWKEY_f11:		return KBDMAP_INDEX_F11;
		case RAWKEY_f12:		return KBDMAP_INDEX_F12;
		case RAWKEY_Print_Screen:	return KBDMAP_INDEX_PRINT_SCREEN;
		case RAWKEY_Pause:		return KBDMAP_INDEX_PAUSE;
		case RAWKEY_Meta_L:		return KBDMAP_INDEX_SUPER1;
		case RAWKEY_Meta_R:		return KBDMAP_INDEX_SUPER2;
		case RAWKEY_KP_Equal:		return KBDMAP_INDEX_KP_EQUALS;
		case RAWKEY_KP_Enter:		return KBDMAP_INDEX_KP_ENTER;
		case RAWKEY_Control_R:		return KBDMAP_INDEX_CONTROL1;
		case RAWKEY_KP_Divide:		return KBDMAP_INDEX_KP_SLASH;
		case RAWKEY_Alt_R:		return KBDMAP_INDEX_OPTION;
		case RAWKEY_Home:		return KBDMAP_INDEX_HOME;
		case RAWKEY_Up:			return KBDMAP_INDEX_UP_ARROW;
		case RAWKEY_Prior:		return KBDMAP_INDEX_PAGE_UP;
		case RAWKEY_Left:		return KBDMAP_INDEX_LEFT_ARROW;
		case RAWKEY_Right:		return KBDMAP_INDEX_RIGHT_ARROW;
		case RAWKEY_End:		return KBDMAP_INDEX_END;
		case RAWKEY_Down:		return KBDMAP_INDEX_DOWN_ARROW;
		case RAWKEY_Next:		return KBDMAP_INDEX_PAGE_DOWN;
		case RAWKEY_Insert:		return KBDMAP_INDEX_INSERT;
		case RAWKEY_Delete:		return KBDMAP_INDEX_DELETE;
		case RAWKEY_Begin:		return 0xFFF;	// No corresponding KBDMAP entry
		case RAWKEY_Menu:		return KBDMAP_INDEX_MENU;
		case RAWKEY_Compose:		return KBDMAP_INDEX_COMPOSE;
		case RAWKEY_BackSpace:		return KBDMAP_INDEX_BACKSPACE;
		case RAWKEY_SysReq:		return KBDMAP_INDEX_ATTENTION;
		case RAWKEY_Power:		return KBDMAP_INDEX_POWER;
		case RAWKEY_AudioMute:		return KBDMAP_INDEX_MUTE;
		case RAWKEY_AudioLower:		return KBDMAP_INDEX_VOLUME_DOWN;
		case RAWKEY_AudioRaise:		return KBDMAP_INDEX_VOLUME_UP;
		case RAWKEY_Help:		return KBDMAP_INDEX_HELP;
		case RAWKEY_L1:			return KBDMAP_INDEX_STOP;
		case RAWKEY_L2:			return KBDMAP_INDEX_AGAIN;
		case RAWKEY_L3:			return KBDMAP_INDEX_PROPERTIES;
		case RAWKEY_L4:			return KBDMAP_INDEX_UNDO;
		case RAWKEY_L5:			return KBDMAP_INDEX_REDO;	// Front
		case RAWKEY_L6:			return KBDMAP_INDEX_COPY;
		case RAWKEY_L7:			return KBDMAP_INDEX_OPEN;
		case RAWKEY_L8:			return KBDMAP_INDEX_PASTE;
		case RAWKEY_L9:			return KBDMAP_INDEX_FIND;
		case RAWKEY_L10:		return KBDMAP_INDEX_CUT;
	}
	return 0xFFFF;
}
#endif

// Data lifted from IRIX 3 sources, file gl2/gl2/kgl/keyboard.c
uint16_t
wscons_sgi_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x02:	return KBDMAP_INDEX_CONTROL1;
		case 0x03:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x04:	return KBDMAP_INDEX_SHIFT1;
		case 0x05:	return KBDMAP_INDEX_SHIFT2;
		case 0x06:	return KBDMAP_INDEX_ESC;
		case 0x07:	return KBDMAP_INDEX_1;
		case 0x08:	return KBDMAP_INDEX_TAB;
		case 0x09:	return KBDMAP_INDEX_Q;
		case 0x0a:	return KBDMAP_INDEX_A;
		case 0x0b:	return KBDMAP_INDEX_S;
		case 0x0d:	return KBDMAP_INDEX_2;
		case 0x0e:	return KBDMAP_INDEX_3;
		case 0x0f:	return KBDMAP_INDEX_W;
		case 0x10:	return KBDMAP_INDEX_E;
		case 0x11:	return KBDMAP_INDEX_D;
		case 0x12:	return KBDMAP_INDEX_F;
		case 0x13:	return KBDMAP_INDEX_Z;
		case 0x14:	return KBDMAP_INDEX_X;
		case 0x15:	return KBDMAP_INDEX_4;
		case 0x16:	return KBDMAP_INDEX_5;
		case 0x17:	return KBDMAP_INDEX_R;
		case 0x18:	return KBDMAP_INDEX_T;
		case 0x19:	return KBDMAP_INDEX_G;
		case 0x1a:	return KBDMAP_INDEX_H;
		case 0x1b:	return KBDMAP_INDEX_C;
		case 0x1c:	return KBDMAP_INDEX_V;
		case 0x1d:	return KBDMAP_INDEX_6;
		case 0x1e:	return KBDMAP_INDEX_7;
		case 0x1f:	return KBDMAP_INDEX_Y;
		case 0x20:	return KBDMAP_INDEX_U;
		case 0x21:	return KBDMAP_INDEX_J;
		case 0x22:	return KBDMAP_INDEX_K;
		case 0x23:	return KBDMAP_INDEX_B;
		case 0x24:	return KBDMAP_INDEX_N;
		case 0x25:	return KBDMAP_INDEX_8;
		case 0x26:	return KBDMAP_INDEX_9;
		case 0x27:	return KBDMAP_INDEX_I;
		case 0x28:	return KBDMAP_INDEX_O;
		case 0x29:	return KBDMAP_INDEX_L;
		case 0x2a:	return KBDMAP_INDEX_SEMICOLON;
		case 0x2b:	return KBDMAP_INDEX_M;
		case 0x2c:	return KBDMAP_INDEX_COMMA;
		case 0x2d:	return KBDMAP_INDEX_0;
		case 0x2e:	return KBDMAP_INDEX_MINUS;
		case 0x2f:	return KBDMAP_INDEX_P;
		case 0x30:	return KBDMAP_INDEX_LEFTBRACE;
		case 0x31:	return KBDMAP_INDEX_APOSTROPHE;
		case 0x32:	return KBDMAP_INDEX_RETURN;
		case 0x33:	return KBDMAP_INDEX_DOT;
		case 0x34:	return KBDMAP_INDEX_SLASH1;
		case 0x35:	return KBDMAP_INDEX_EQUALS;
		case 0x36:	return KBDMAP_INDEX_GRAVE;
		case 0x37:	return KBDMAP_INDEX_RIGHTBRACE;
		case 0x38:	return KBDMAP_INDEX_EUROPE1;
		case 0x39:	return KBDMAP_INDEX_KP_1;
		case 0x3a:	return KBDMAP_INDEX_KP_0;
		case 0x3c:	return KBDMAP_INDEX_BACKSPACE;
		case 0x3d:	return KBDMAP_INDEX_DELETE;
		case 0x3e:	return KBDMAP_INDEX_KP_4;
		case 0x3f:	return KBDMAP_INDEX_KP_2;
		case 0x40:	return KBDMAP_INDEX_KP_3;
		case 0x41:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x42:	return KBDMAP_INDEX_KP_7;
		case 0x43:	return KBDMAP_INDEX_KP_8;
		case 0x44:	return KBDMAP_INDEX_KP_5;
		case 0x45:	return KBDMAP_INDEX_KP_6;
		case 0x48:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0x49:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0x4a:	return KBDMAP_INDEX_KP_9;
		case 0x4b:	return KBDMAP_INDEX_KP_MINUS;
		case 0x4f:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0x50:	return KBDMAP_INDEX_UP_ARROW;
		case 0x51:	return KBDMAP_INDEX_KP_ENTER;
		case 0x52:	return KBDMAP_INDEX_SPACE;
		case 0x53:	return KBDMAP_INDEX_ALT;
		case 0x54:	return KBDMAP_INDEX_OPTION;
		case 0x55:	return KBDMAP_INDEX_CONTROL2;
		case 0x56:	return KBDMAP_INDEX_F1;
		case 0x57:	return KBDMAP_INDEX_F2;
		case 0x58:	return KBDMAP_INDEX_F3;
		case 0x59:	return KBDMAP_INDEX_F4;
		case 0x5a:	return KBDMAP_INDEX_F5;
		case 0x5b:	return KBDMAP_INDEX_F6;
		case 0x5c:	return KBDMAP_INDEX_F7;
		case 0x5d:	return KBDMAP_INDEX_F8;
		case 0x5e:	return KBDMAP_INDEX_F9;
		case 0x5f:	return KBDMAP_INDEX_F10;
		case 0x60:	return KBDMAP_INDEX_F11;
		case 0x61:	return KBDMAP_INDEX_F12;
		case 0x62:	return KBDMAP_INDEX_PRINT_SCREEN;
		case 0x63:	return KBDMAP_INDEX_SCROLLLOCK;
		case 0x64:	return KBDMAP_INDEX_PAUSE;
		case 0x65:	return KBDMAP_INDEX_INSERT;
		case 0x66:	return KBDMAP_INDEX_HOME;
		case 0x67:	return KBDMAP_INDEX_PAGE_UP;
		case 0x68:	return KBDMAP_INDEX_END;
		case 0x69:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0x6a:	return KBDMAP_INDEX_NUMLOCK;
		case 0x6b:	return KBDMAP_INDEX_KP_SLASH;
		case 0x6c:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0x6d:	return KBDMAP_INDEX_KP_PLUS;
		case 0x6e:	return KBDMAP_INDEX_EUROPE2;
	}
	return 0xFFFF;
}

#if defined(__NetBSD__)
// Data from xf86-input-keyboard Xorg driver from NetBSD
uint16_t
wscons_adb_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x00:	return KBDMAP_INDEX_A;
		case 0x01:	return KBDMAP_INDEX_S;
		case 0x02:	return KBDMAP_INDEX_D;
		case 0x03:	return KBDMAP_INDEX_F;
		case 0x04:	return KBDMAP_INDEX_H;
		case 0x05:	return KBDMAP_INDEX_G;
		case 0x06:	return KBDMAP_INDEX_Z;
		case 0x07:	return KBDMAP_INDEX_X;
		case 0x08:	return KBDMAP_INDEX_C;
		case 0x09:	return KBDMAP_INDEX_V;
		case 0x0a:	return KBDMAP_INDEX_GRAVE;
		case 0x0b:	return KBDMAP_INDEX_B;
		case 0x0c:	return KBDMAP_INDEX_Q;
		case 0x0d:	return KBDMAP_INDEX_W;
		case 0x0e:	return KBDMAP_INDEX_E;
		case 0x0f:	return KBDMAP_INDEX_R;
		case 0x10:	return KBDMAP_INDEX_Y;
		case 0x11:	return KBDMAP_INDEX_T;
		case 0x12:	return KBDMAP_INDEX_1;
		case 0x13:	return KBDMAP_INDEX_2;
		case 0x14:	return KBDMAP_INDEX_3;
		case 0x15:	return KBDMAP_INDEX_4;
		case 0x16:	return KBDMAP_INDEX_6;
		case 0x17:	return KBDMAP_INDEX_5;
		case 0x18:	return KBDMAP_INDEX_EQUALS;
		case 0x19:	return KBDMAP_INDEX_9;
		case 0x1a:	return KBDMAP_INDEX_7;
		case 0x1b:	return KBDMAP_INDEX_MINUS;
		case 0x1c:	return KBDMAP_INDEX_8;
		case 0x1d:	return KBDMAP_INDEX_0;
		case 0x1e:	return KBDMAP_INDEX_RIGHTBRACE;
		case 0x1f:	return KBDMAP_INDEX_O;
		case 0x20:	return KBDMAP_INDEX_U;
		case 0x21:	return KBDMAP_INDEX_LEFTBRACE;
		case 0x22:	return KBDMAP_INDEX_I;
		case 0x23:	return KBDMAP_INDEX_P;
		case 0x24:	return KBDMAP_INDEX_RETURN;
		case 0x25:	return KBDMAP_INDEX_L;
		case 0x26:	return KBDMAP_INDEX_J;
		case 0x27:	return KBDMAP_INDEX_APOSTROPHE;
		case 0x28:	return KBDMAP_INDEX_K;
		case 0x29:	return KBDMAP_INDEX_SEMICOLON;
		case 0x2a:	return KBDMAP_INDEX_EUROPE1;
		case 0x2b:	return KBDMAP_INDEX_COMMA;
		case 0x2c:	return KBDMAP_INDEX_SLASH1;
		case 0x2d:	return KBDMAP_INDEX_N;
		case 0x2e:	return KBDMAP_INDEX_M;
		case 0x2f:	return KBDMAP_INDEX_DOT;
		case 0x30:	return KBDMAP_INDEX_TAB;
		case 0x31:	return KBDMAP_INDEX_SPACE;
		case 0x32:	return KBDMAP_INDEX_EUROPE2;
		case 0x33:	return KBDMAP_INDEX_BACKSPACE;
		case 0x34:	return KBDMAP_INDEX_OPTION;
		case 0x35:	return KBDMAP_INDEX_ESC;
		case 0x36:	return KBDMAP_INDEX_CONTROL1;
		case 0x37:	return KBDMAP_INDEX_SUPER1;
		case 0x38:	return KBDMAP_INDEX_SHIFT1;
		case 0x39:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x3a:	return KBDMAP_INDEX_ALT;
		case 0x3b:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0x3c:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0x3d:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0x3e:	return KBDMAP_INDEX_UP_ARROW;
		case 0x3f:	return 0xFFFF;	// Fn key
		case 0x41:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x43:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0x45:	return KBDMAP_INDEX_KP_PLUS;
		case 0x47:	return KBDMAP_INDEX_NUMLOCK;
		case 0x4b:	return KBDMAP_INDEX_KP_SLASH;
		case 0x51:	return KBDMAP_INDEX_EQUALS;
		case 0x52:	return KBDMAP_INDEX_KP_0;
		case 0x53:	return KBDMAP_INDEX_KP_1;
		case 0x54:	return KBDMAP_INDEX_KP_2;
		case 0x55:	return KBDMAP_INDEX_KP_3;
		case 0x56:	return KBDMAP_INDEX_KP_4;
		case 0x57:	return KBDMAP_INDEX_KP_5;
		case 0x58:	return KBDMAP_INDEX_KP_6;
		case 0x59:	return KBDMAP_INDEX_KP_7;
		case 0x5b:	return KBDMAP_INDEX_KP_8;
		case 0x5c:	return KBDMAP_INDEX_KP_9;
		case 0x5f:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x60:	return KBDMAP_INDEX_F5;
		case 0x61:	return KBDMAP_INDEX_F6;
		case 0x62:	return KBDMAP_INDEX_F7;
		case 0x63:	return KBDMAP_INDEX_F3;
		case 0x64:	return KBDMAP_INDEX_F8;
		case 0x65:	return KBDMAP_INDEX_F9;
		case 0x67:	return KBDMAP_INDEX_F11;
		case 0x69:	return KBDMAP_INDEX_PRINT_SCREEN;
		case 0x6a:	return KBDMAP_INDEX_KP_ENTER;
		case 0x6b:	return KBDMAP_INDEX_SCROLLLOCK;
		case 0x6d:	return KBDMAP_INDEX_F10;
		case 0x6f:	return KBDMAP_INDEX_F12;
		case 0x71:	return KBDMAP_INDEX_PAUSE;
		case 0x72:	return KBDMAP_INDEX_INSERT;
		case 0x73:	return KBDMAP_INDEX_HOME;
		case 0x74:	return KBDMAP_INDEX_PAGE_UP;
		case 0x75:	return KBDMAP_INDEX_DELETE;
		case 0x76:	return KBDMAP_INDEX_F4;
		case 0x77:	return KBDMAP_INDEX_END;
		case 0x78:	return KBDMAP_INDEX_F2;
		case 0x79:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0x7a:	return KBDMAP_INDEX_F1;
		case 0x7f:	return KBDMAP_INDEX_POWER;
	}
	return 0xFFFF;
}
#endif

#if defined(__NetBSD__)
// Data from xf86-input-keyboard Xorg driver from NetBSD
uint16_t
wscons_sun_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x00:	return KBDMAP_INDEX_HELP;
		case 0x01:	return KBDMAP_INDEX_STOP;
		case 0x02:	return KBDMAP_INDEX_VOLUME_DOWN;
		case 0x03:	return KBDMAP_INDEX_AGAIN;
		case 0x04:	return KBDMAP_INDEX_VOLUME_UP;
		case 0x05:	return KBDMAP_INDEX_F1;
		case 0x06:	return KBDMAP_INDEX_F2;
		case 0x07:	return KBDMAP_INDEX_F10;
		case 0x08:	return KBDMAP_INDEX_F3;
		case 0x09:	return KBDMAP_INDEX_F11;
		case 0x0a:	return KBDMAP_INDEX_F4;
		case 0x0b:	return KBDMAP_INDEX_F12;
		case 0x0c:	return KBDMAP_INDEX_F5;
		case 0x0d:	return KBDMAP_INDEX_OPTION;
		case 0x0e:	return KBDMAP_INDEX_F6;
		case 0x10:	return KBDMAP_INDEX_F7;
		case 0x11:	return KBDMAP_INDEX_F8;
		case 0x12:	return KBDMAP_INDEX_F9;
		case 0x13:	return KBDMAP_INDEX_ALT;
		case 0x14:	return KBDMAP_INDEX_UP_ARROW;
		case 0x15:	return KBDMAP_INDEX_PAUSE;
		case 0x16:	return KBDMAP_INDEX_PRINT_SCREEN;
		case 0x17:	return KBDMAP_INDEX_SCROLLLOCK;
		case 0x18:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0x19:	return KBDMAP_INDEX_PROPERTIES;
		case 0x1a:	return KBDMAP_INDEX_UNDO;
		case 0x1b:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0x1c:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0x1d:	return KBDMAP_INDEX_ESC;
		case 0x1e:	return KBDMAP_INDEX_1;
		case 0x1f:	return KBDMAP_INDEX_2;
		case 0x20:	return KBDMAP_INDEX_3;
		case 0x21:	return KBDMAP_INDEX_4;
		case 0x22:	return KBDMAP_INDEX_5;
		case 0x23:	return KBDMAP_INDEX_6;
		case 0x24:	return KBDMAP_INDEX_7;
		case 0x25:	return KBDMAP_INDEX_8;
		case 0x26:	return KBDMAP_INDEX_9;
		case 0x27:	return KBDMAP_INDEX_0;
		case 0x28:	return KBDMAP_INDEX_MINUS;
		case 0x29:	return KBDMAP_INDEX_EQUALS;
		case 0x2a:	return KBDMAP_INDEX_GRAVE;
		case 0x2b:	return KBDMAP_INDEX_BACKSPACE;
		case 0x2c:	return KBDMAP_INDEX_INSERT;
		case 0x2d:	return KBDMAP_INDEX_MUTE;
		case 0x2e:	return KBDMAP_INDEX_KP_SLASH;
		case 0x2f:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0x30:	return KBDMAP_INDEX_POWER;
		case 0x31:	return KBDMAP_INDEX_REDO;
		case 0x32:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x33:	return KBDMAP_INDEX_COPY;
		case 0x34:	return KBDMAP_INDEX_HOME;
		case 0x35:	return KBDMAP_INDEX_TAB;
		case 0x36:	return KBDMAP_INDEX_Q;
		case 0x37:	return KBDMAP_INDEX_W;
		case 0x38:	return KBDMAP_INDEX_E;
		case 0x39:	return KBDMAP_INDEX_R;
		case 0x3a:	return KBDMAP_INDEX_T;
		case 0x3b:	return KBDMAP_INDEX_Y;
		case 0x3c:	return KBDMAP_INDEX_U;
		case 0x3d:	return KBDMAP_INDEX_I;
		case 0x3e:	return KBDMAP_INDEX_O;
		case 0x3f:	return KBDMAP_INDEX_P;
		case 0x40:	return KBDMAP_INDEX_LEFTBRACE;
		case 0x41:	return KBDMAP_INDEX_RIGHTBRACE;
		case 0x42:	return KBDMAP_INDEX_DELETE;
		case 0x43:	return KBDMAP_INDEX_MENU;
		case 0x44:	return KBDMAP_INDEX_KP_7;
		case 0x45:	return KBDMAP_INDEX_KP_8;
		case 0x46:	return KBDMAP_INDEX_KP_9;
		case 0x47:	return KBDMAP_INDEX_KP_MINUS;
		case 0x48:	return KBDMAP_INDEX_OPEN;
		case 0x49:	return KBDMAP_INDEX_PASTE;
		case 0x4a:	return KBDMAP_INDEX_END;
		case 0x4c:	return KBDMAP_INDEX_CONTROL1;
		case 0x4d:	return KBDMAP_INDEX_A;
		case 0x4e:	return KBDMAP_INDEX_S;
		case 0x4f:	return KBDMAP_INDEX_D;
		case 0x50:	return KBDMAP_INDEX_F;
		case 0x51:	return KBDMAP_INDEX_G;
		case 0x52:	return KBDMAP_INDEX_H;
		case 0x53:	return KBDMAP_INDEX_J;
		case 0x54:	return KBDMAP_INDEX_K;
		case 0x55:	return KBDMAP_INDEX_L;
		case 0x56:	return KBDMAP_INDEX_SEMICOLON;
		case 0x57:	return KBDMAP_INDEX_APOSTROPHE;
		case 0x58:	return KBDMAP_INDEX_EUROPE1;
		case 0x59:	return KBDMAP_INDEX_RETURN;
		case 0x5b:	return KBDMAP_INDEX_KP_ENTER;
		case 0x5c:	return KBDMAP_INDEX_KP_5;
		case 0x5f:	return KBDMAP_INDEX_FIND;
		case 0x60:	return KBDMAP_INDEX_PAGE_UP;
		case 0x61:	return KBDMAP_INDEX_CUT;
		case 0x62:	return KBDMAP_INDEX_NUMLOCK;
		case 0x63:	return KBDMAP_INDEX_SHIFT1;
		case 0x64:	return KBDMAP_INDEX_Z;
		case 0x65:	return KBDMAP_INDEX_X;
		case 0x66:	return KBDMAP_INDEX_C;
		case 0x67:	return KBDMAP_INDEX_V;
		case 0x68:	return KBDMAP_INDEX_B;
		case 0x69:	return KBDMAP_INDEX_N;
		case 0x6a:	return KBDMAP_INDEX_M;
		case 0x6b:	return KBDMAP_INDEX_COMMA;
		case 0x6c:	return KBDMAP_INDEX_DOT;
		case 0x6d:	return KBDMAP_INDEX_SLASH1;
		case 0x6e:	return KBDMAP_INDEX_SHIFT2;
		case 0x70:	return KBDMAP_INDEX_KP_1;
		case 0x71:	return KBDMAP_INDEX_KP_2;
		case 0x72:	return KBDMAP_INDEX_KP_3;
		case 0x76:	return KBDMAP_INDEX_HELP;
		case 0x77:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x78:	return KBDMAP_INDEX_SUPER1;
		case 0x79:	return KBDMAP_INDEX_SPACE;
		case 0x7a:	return KBDMAP_INDEX_SUPER2;
		case 0x7b:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0x7c:	return KBDMAP_INDEX_EUROPE2;
		case 0x7d:	return KBDMAP_INDEX_KP_PLUS;
	}
	return 0xFFFF;
}
#endif

#if defined(__NetBSD__)
// Data from NetBSD kernel tables and EWS-4800 key layouts
uint16_t
wscons_ews_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x00:	return KBDMAP_INDEX_0;
		case 0x01:	return KBDMAP_INDEX_1;
		case 0x02:	return KBDMAP_INDEX_2;
		case 0x03:	return KBDMAP_INDEX_3;
		case 0x04:	return KBDMAP_INDEX_4;
		case 0x05:	return KBDMAP_INDEX_5;
		case 0x06:	return KBDMAP_INDEX_6;
		case 0x07:	return KBDMAP_INDEX_7;
		case 0x08:	return KBDMAP_INDEX_8;
		case 0x09:	return KBDMAP_INDEX_9;
		case 0x0a:	return KBDMAP_INDEX_MINUS;
		case 0x0b:	return KBDMAP_INDEX_GRAVE;
		case 0x0c:	return KBDMAP_INDEX_YEN;
		case 0x0d:	return KBDMAP_INDEX_APOSTROPHE;
		case 0x0e:	return KBDMAP_INDEX_DOT;
		case 0x0f:	return KBDMAP_INDEX_SLASH1;
		case 0x10:	return KBDMAP_INDEX_LEFTBRACE;
		case 0x11:	return KBDMAP_INDEX_A;
		case 0x12:	return KBDMAP_INDEX_B;
		case 0x13:	return KBDMAP_INDEX_C;
		case 0x14:	return KBDMAP_INDEX_D;
		case 0x15:	return KBDMAP_INDEX_E;
		case 0x16:	return KBDMAP_INDEX_F;
		case 0x17:	return KBDMAP_INDEX_G;
		case 0x18:	return KBDMAP_INDEX_H;
		case 0x19:	return KBDMAP_INDEX_I;
		case 0x1a:	return KBDMAP_INDEX_J;
		case 0x1b:	return KBDMAP_INDEX_K;
		case 0x1c:	return KBDMAP_INDEX_L;
		case 0x1d:	return KBDMAP_INDEX_M;
		case 0x1e:	return KBDMAP_INDEX_N;
		case 0x1f:	return KBDMAP_INDEX_O;
		case 0x20:	return KBDMAP_INDEX_P;
		case 0x21:	return KBDMAP_INDEX_Q;
		case 0x22:	return KBDMAP_INDEX_R;
		case 0x23:	return KBDMAP_INDEX_S;
		case 0x24:	return KBDMAP_INDEX_T;
		case 0x25:	return KBDMAP_INDEX_U;
		case 0x26:	return KBDMAP_INDEX_V;
		case 0x27:	return KBDMAP_INDEX_W;
		case 0x28:	return KBDMAP_INDEX_X;
		case 0x29:	return KBDMAP_INDEX_Y;
		case 0x2a:	return KBDMAP_INDEX_Z;
		case 0x2b:	return KBDMAP_INDEX_RIGHTBRACE;
		case 0x2c:	return KBDMAP_INDEX_COMMA;
		case 0x2d:	return KBDMAP_INDEX_EUROPE1;
		case 0x2e:	return KBDMAP_INDEX_SEMICOLON;
		case 0x2f:	return KBDMAP_INDEX_SLASH2;
		case 0x30:	return KBDMAP_INDEX_KP_0;
		case 0x31:	return KBDMAP_INDEX_KP_1;
		case 0x32:	return KBDMAP_INDEX_KP_2;
		case 0x33:	return KBDMAP_INDEX_KP_3;
		case 0x34:	return KBDMAP_INDEX_KP_4;
		case 0x35:	return KBDMAP_INDEX_KP_5;
		case 0x36:	return KBDMAP_INDEX_KP_6;
		case 0x37:	return KBDMAP_INDEX_KP_7;
		case 0x38:	return KBDMAP_INDEX_KP_8;
		case 0x39:	return KBDMAP_INDEX_KP_9;
		case 0x3a:	return KBDMAP_INDEX_SPACE;
		case 0x3b:	return KBDMAP_INDEX_KP_JPCOMMA;
		case 0x3e:	return KBDMAP_INDEX_KATAKANA_HIRAGANA;
		case 0x40:	return KBDMAP_INDEX_RETURN;
		case 0x41:	return KBDMAP_INDEX_RETURN;
		case 0x42:	return KBDMAP_INDEX_PAGE_UP;
		case 0x43:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0x44:	return KBDMAP_INDEX_KP_ENTER;
		case 0x46:	return KBDMAP_INDEX_KP_MINUS;
		case 0x47:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x48:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0x49:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0x50:	return KBDMAP_INDEX_BACKSPACE;
		case 0x52:	return KBDMAP_INDEX_CLEAR;
		case 0x53:	return KBDMAP_INDEX_DELETE;
		case 0x54:	return KBDMAP_INDEX_INSERT;
		case 0x56:	return KBDMAP_INDEX_F1;
		case 0x57:	return KBDMAP_INDEX_F2;
		case 0x58:	return KBDMAP_INDEX_F3;
		case 0x59:	return KBDMAP_INDEX_F4;
		case 0x5a:	return KBDMAP_INDEX_F5;
		case 0x5b:	return KBDMAP_INDEX_F6;
		case 0x5c:	return KBDMAP_INDEX_F7;
		case 0x5d:	return KBDMAP_INDEX_F8;
		case 0x5e:	return KBDMAP_INDEX_F9;
		case 0x5f:	return KBDMAP_INDEX_F10;
		case 0x60:	return KBDMAP_INDEX_F11;
		case 0x61:	return KBDMAP_INDEX_F12;
		case 0x62:	return KBDMAP_INDEX_F13;
		case 0x63:	return KBDMAP_INDEX_F14;
		case 0x64:	return KBDMAP_INDEX_F15;
		case 0x67:	return KBDMAP_INDEX_MUHENKAN;
		case 0x68:	return KBDMAP_INDEX_HENKAN;
		case 0x6a:	return KBDMAP_INDEX_UP_ARROW;
		case 0x6b:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0x6d:	return KBDMAP_INDEX_SUPER1;
		case 0x70:	return KBDMAP_INDEX_KP_PLUS;
		case 0x71:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0x72:	return KBDMAP_INDEX_SLASH1;
		case 0x73:	return KBDMAP_INDEX_ESC;
		case 0x74:	return KBDMAP_INDEX_HELP;
		case 0x75:	return KBDMAP_INDEX_KP_EQUALS;
		case 0x76:	return KBDMAP_INDEX_TAB;
		case 0x78:	return KBDMAP_INDEX_CONTROL1;
		case 0x79:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x7b:	return KBDMAP_INDEX_SHIFT1;
		case 0x7c:	return KBDMAP_INDEX_SHIFT2;
		case 0x7e:	return KBDMAP_INDEX_ALT;
		case 0x7f:	return 0xFFFF;	// FNC
	}
	return 0xFFFF;
}
#endif

#endif
