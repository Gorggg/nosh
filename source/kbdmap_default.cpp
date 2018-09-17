/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "kbdmap.h"
#include "kbdmap_entries.h"
#include "kbdmap_default.h"
#include "InputMessage.h"

/* Base keyboard ************************************************************
// **************************************************************************
*/

static 
const KeyboardMap
English_international_keyboard =
{
	// 00: ISO 9995 "E" row
	{
		SIMPLE(NOOP(0xFEFF)),
		SHIFTABLE(UCSA('1'),UCSA('!'),UCSA(L'¡'),UCSA(L'¹'),UCSA(0x00B9),UCSA(0x00A1),UCSA(0x02B9),NOOP(9995)),
		SHIFTABLE_CONTROL(UCSA('2'),UCSA('@'),UCSA(L'²'),UCSA(L'⅔'),UCSA('@'-0x40),UCSA(0x00B2),UCSA(0x00A4),UCSA(0x02BA),NOOP(9995)),
		SHIFTABLE(UCSA('3'),UCSA('#'),UCSA(L'³'),UCSA(L'⅜'),UCSA(0x00B3),UCSA(0x00A3),UCSA(0x02BF),NOOP(9995)),
		SHIFTABLE(UCSA('4'),UCSA('$'),UCSA(L'¤'),UCSA(L'£'),UCSA(0x00BC),UCSA(0x20AC),UCSA(0x02BE),NOOP(9995)),
		SHIFTABLE(UCSA('5'),UCSA('%'),UCSA(L'€'),UCSA(L'‰'),UCSA(0x00BD),UCSA(0x2191),UCSA(0x02C1),NOOP(9995)),
		SHIFTABLE_CONTROL(UCSA('6'),UCSA('^'),UCSA(L'¼'),UCSA(L'⅙'),UCSA('^'-0x40),UCSA(0x00BE),UCSA(0x2193),UCSA(0x02C0),NOOP(9995)),
		SHIFTABLE(UCSA('7'),UCSA('&'),UCSA(L'½'),UCSA(L'⅞'),UCSA(0x215B),UCSA(0x2190),UCSA(0x007B),NOOP(9995)),
		SHIFTABLE(UCSA('8'),UCSA('*'),UCSA(L'¾'),UCSA(L'⅛'),UCSA(0x215C),UCSA(0x2192),UCSA(0x007D),NOOP(9995)),
		SHIFTABLE(UCSA('9'),UCSA('('),UCSA(L'‘'),UCSA(L'⅟'),UCSA(0x215D),UCSA(0x00B1),UCSA(0x005B),NOOP(9995)),
		SHIFTABLE(UCSA('0'),UCSA(')'),UCSA(L'’'),UCSA(L'⁄'),UCSA(0x215E),UCSA(0x2122),UCSA(0x005D),NOOP(9995)),
		SHIFTABLE_CONTROL(UCSA('-'),UCSA('_'),UCSA(0x2013),UCSA(0x2014),UCSA('_'-0x40),UCSA(0x005C),UCSA(0x00BF),UCSA(0x02BB),NOOP(9995)),
		SHIFTABLE(UCSA('='),UCSA('+'),UCSA(L'×'),UCSA(L'÷'),UCSA(0x0327),UCSA(0x0328),UCSA(0x00AC),NOOP(9995)),
/* 109 */	SIMPLE(EXTE(EXTENDED_KEY_IM_TOGGLE)),
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_BACKSPACE)),
	},
	// 01: ISO 9995 "D" row
	{
		SHIFTABLE(UCSA(0x09),EXTE(EXTENDED_KEY_BACKTAB),UCSA(0x09),EXTE(EXTENDED_KEY_BACKTAB),UCSA(0x09),EXTE(EXTENDED_KEY_BACKTAB),UCSA(0x09),EXTE(EXTENDED_KEY_BACKTAB)),
		CAPSABLE_CONTROL(UCSA('q'),UCSA('Q'),UCSA(L'ä'),UCSA(L'Ä'),UCSA('q'-0x60),UCSA(0x0242),UCSA(0x0241),UCSA(0x030D),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('w'),UCSA('W'),UCSA(L'å'),UCSA(L'Å'),UCSA('w'-0x60),UCSA(0x02B7),UCSA(0x2126),UCSA(0x0307),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('e'),UCSA('E'),UCSA(L'é'),UCSA(L'É'),UCSA('e'-0x60),UCSA(0x0153),UCSA(0x0152),UCSA(0x0306),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('r'),UCSA('R'),UCSA(L'™'),UCSA(L'®'),UCSA('r'-0x60),UCSA(0x00B6),UCSA(0x00AE),UCSA(0x0302),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('t'),UCSA('T'),UCSA(L'þ'),UCSA(L'Þ'),UCSA('t'-0x60),UCSA(0xA78C),UCSA(0xA78B),UCSA(0x0308),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('y'),UCSA('Y'),UCSA(L'ü'),UCSA(L'Ü'),UCSA('y'-0x60),UCSA(0x027C),UCSA(L'¥'),UCSA(0x0311),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('u'),UCSA('U'),UCSA(L'ú'),UCSA(L'Ú'),UCSA('u'-0x60),UCSA(0x0223),UCSA(0x0222),UCSA(0x030C),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('i'),UCSA('I'),UCSA(L'í'),UCSA(L'Í'),UCSA('i'-0x60),UCSA(0x0131),UCSA(0x214D),UCSA(0x0313),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('o'),UCSA('O'),UCSA(L'ó'),UCSA(L'Ó'),UCSA('o'-0x60),UCSA(0x00F8),UCSA(0x00D8),UCSA(0x031B),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('p'),UCSA('P'),UCSA(L'ö'),UCSA(L'Ö'),UCSA('p'-0x60),UCSA(0x00FE),UCSA(0x00DE),UCSA(0x0309),NOOP(9995)),
		SHIFTABLE_CONTROL(UCSA('['),UCSA('{'),UCSA(L'«'),UCSA(L'“'),UCSA('['-0x40),UCSA(0x017F),UCSA(0x030A),UCSA(0x0300),NOOP(9995)),
		SHIFTABLE_CONTROL(UCSA(']'),UCSA('}'),UCSA(L'»'),UCSA(L'”'),UCSA(']'-0x40),UCSA(0x0303),UCSA(0x0304),UCSA(0x0040),NOOP(9995)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_RETURN_OR_ENTER)),
	},
	// 02: ISO 9995 "C" row
	{
		SIMPLE(NOOP(0)),
		CAPSABLE_CONTROL(UCSA('a'),UCSA('A'),UCSA(L'á'),UCSA(L'Á'),UCSA('a'-0x60),UCSA(0x00E6),UCSA(0x00C6),UCSA(0x0329),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('s'),UCSA('S'),UCSA(L'ß'),UCSA(L'§'),UCSA('s'-0x60),UCSA(0x00DF),UCSA(0x00A7),UCSA(0x0323),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('d'),UCSA('D'),UCSA(L'ð'),UCSA(L'Ð'),UCSA('d'-0x60),UCSA(0x00F0),UCSA(0x00D0),UCSA(0x032E),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('f'),UCSA('F'),NOOP(0),NOOP(0),UCSA('f'-0x60),UCSA(0x0294),UCSA(0x00AA),UCSA(0x032D),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('g'),UCSA('G'),NOOP(0),NOOP(0),UCSA('g'-0x60),UCSA(0x014B),UCSA(0x014A),UCSA(0x0331),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('h'),UCSA('H'),UCSA(0x2020),UCSA(0x2021),UCSA('h'-0x60),UCSA(0x0272),UCSA(0x019D),UCSA(0x0332),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('j'),UCSA('J'),NOOP(0),NOOP(0),UCSA('j'-0x60),UCSA(0x0133),UCSA(0x0132),UCSA(0x0325),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('k'),UCSA('K'),UCSA(L'œ'),UCSA(L'Œ'),UCSA('k'-0x60),UCSA(0x0138),UCSA(0x0325),UCSA(0x0335),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('l'),UCSA('L'),UCSA(L'ø'),UCSA(L'Ø'),UCSA('l'-0x60),UCSA(0x0142),UCSA(0x0141),UCSA(0x0338),NOOP(9995)),
		SHIFTABLE(UCSA(';'),UCSA(':'),UCSA(L'¶'),UCSA(L'°'),UCSA(0x0301),UCSA(0x030B),UCSA(0x00B0),NOOP(9995)),
		SHIFTABLE(UCSA('\''),UCSA('\"'),UCSA(L'´'),UCSA(L'¨'),UCSA(0x019B),UCSA(0x1E9E),UCSA(0x2032),NOOP(9995)),
/* grave */	SHIFTABLE(UCSA('`'),UCSA('~'),UCSA(L'±'),UCSA(L'¥'),UCSA(0x204A),UCSA(0x00AD),UCSA('|'),NOOP(9995)),
/* = D13 */	SHIFTABLE_CONTROL(UCSA('\\'),UCSA('|'),UCSA(L'¬'),UCSA(L'¦'),UCSA('\\'-0x40),UCSA(0x0259),UCSA(0x018F),UCSA(0x2033),NOOP(9995)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 03: ISO 9995 "B" row
	{
		SIMPLE(NOOP(0)),
/* 105+107 */	SHIFTABLE_CONTROL(UCSA('\\'),UCSA('|'),UCSA(L'¬'),UCSA(L'¦'),UCSA('\\'-0x40),UCSA(0x0149),UCSA(L'¦'),UCSA(0x266A),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('z'),UCSA('Z'),UCSA(L'æ'),UCSA(L'Æ'),UCSA('z'-0x60),UCSA(0x0292),UCSA(0x01B7),UCSA(0x00AB),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('x'),UCSA('X'),NOOP(0),NOOP(0),UCSA('x'-0x60),UCSA(0x201E),UCSA(0x201A),UCSA(0x00BB),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('c'),UCSA('C'),UCSA(L'¢'),UCSA(L'©'),UCSA('c'-0x60),UCSA(0x00A2),UCSA(0x00A9),UCSA(0x2015),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('v'),UCSA('V'),UCSA(L'ł'),UCSA(L'Ł'),UCSA('v'-0x60),UCSA(0x201C),UCSA(0x2018),UCSA(0x2039),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('b'),UCSA('B'),NOOP(0),NOOP(0),UCSA('b'-0x60),UCSA(0x201D),UCSA(0x2019),UCSA(0x203A),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('n'),UCSA('N'),UCSA(L'ñ'),UCSA(L'Ñ'),UCSA('n'-0x60),UCSA(0x019E),UCSA(0x0220),UCSA(0x2013),NOOP(9995)),
		CAPSABLE_CONTROL(UCSA('m'),UCSA('M'),UCSA(L'µ'),UCSA(L'Μ'),UCSA('m'-0x60),UCSA(0x00B5),UCSA(0x00BA),UCSA(0x2014),NOOP(9995)),
		SHIFTABLE(UCSA(','),UCSA('<'),UCSA(L'ç'),UCSA(L'Ç'),UCSA(0x2026),UCSA(0x00D7),UCSA(0x0024),NOOP(9995)),
		SHIFTABLE(UCSA('.'),UCSA('>'),UCSA(L'·'),UCSA(L'…'),UCSA(0x00B7),UCSA(0x00F7),UCSA(0x0023),NOOP(9995)),
		SHIFTABLE(UCSA('/'),UCSA('?'),UCSA(L'¿'),UCSA(L'‽'),UCSA(0x0140),UCSA(0x013F),UCSA(0x2011),NOOP(9995)),
/* 107+109 */	SHIFTABLE(UCSA(0x20A8),UCSA(L'₩'),UCSA(0x2031),UCSA(0x203B),NOOP(9995),NOOP(9995),NOOP(9995),NOOP(9995)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 04: Modifier row
	{
		SIMPLE(MMNT(KBDMAP_MODIFIER_1ST_LEVEL2)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_2ND_LEVEL2)),
		SHIFTABLE(MMNT(KBDMAP_MODIFIER_1ST_LEVEL3),LOCK(KBDMAP_MODIFIER_1ST_GROUP2),MMNT(KBDMAP_MODIFIER_1ST_LEVEL3),LOCK(KBDMAP_MODIFIER_1ST_GROUP2),MMNT(KBDMAP_MODIFIER_1ST_LEVEL3),LOCK(KBDMAP_MODIFIER_1ST_GROUP2),MMNT(KBDMAP_MODIFIER_1ST_LEVEL3),LOCK(KBDMAP_MODIFIER_1ST_GROUP2)),
		SIMPLE(NOOP(0)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_1ST_CONTROL)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_2ND_CONTROL)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_1ST_SUPER)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_2ND_SUPER)),
		SIMPLE(MMNT(KBDMAP_MODIFIER_1ST_ALT)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SHIFTABLE(LOCK(KBDMAP_MODIFIER_CAPS),LOCK(KBDMAP_MODIFIER_LEVEL2),LOCK(KBDMAP_MODIFIER_CAPS),LOCK(KBDMAP_MODIFIER_LEVEL2),LOCK(KBDMAP_MODIFIER_CAPS),LOCK(KBDMAP_MODIFIER_LEVEL2),LOCK(KBDMAP_MODIFIER_CAPS),LOCK(KBDMAP_MODIFIER_LEVEL2)),
		SIMPLE(LOCK(KBDMAP_MODIFIER_SCROLL)),
		SIMPLE(LOCK(KBDMAP_MODIFIER_NUM)),
		SIMPLE(NOOP(0)),
 	},
	// 05: ISO 9995 "A" row
	{
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_KATAKANA_HIRAGANA)),
		SIMPLE(EXTE(EXTENDED_KEY_ZENKAKU_HANKAKU)),
		SIMPLE(EXTE(EXTENDED_KEY_HIRAGANA)),
		SIMPLE(EXTE(EXTENDED_KEY_KATAKANA)),
		SIMPLE(EXTE(EXTENDED_KEY_HENKAN)),
		SIMPLE(EXTE(EXTENDED_KEY_MUHENKAN)),
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_HAN_YEONG)),
		SIMPLE(EXTE(EXTENDED_KEY_HANJA)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_ALTERNATE_ERASE)),
		SIMPLE(NOOP(0)),
		SHIFTABLE(UCSA(' '),UCSA(' '),UCSA(' '),UCSA(' '),UCSA(0x202F),UCSA(0x200C),UCSA(0x00A0),NOOP(9995)),
	},
	// 06: Cursor/editing "row"
	{
		SIMPLE(EXTE(EXTENDED_KEY_HOME)),
		SIMPLE(EXTE(EXTENDED_KEY_UP_ARROW)),
		SIMPLE(EXTE(EXTENDED_KEY_PAGE_UP)),
		SIMPLE(EXTE(EXTENDED_KEY_LEFT_ARROW)),
		SIMPLE(EXTE(EXTENDED_KEY_RIGHT_ARROW)),
		SIMPLE(EXTE(EXTENDED_KEY_END)),
		SIMPLE(EXTE(EXTENDED_KEY_DOWN_ARROW)),
		SIMPLE(EXTE(EXTENDED_KEY_PAGE_DOWN)),
		SIMPLE(EXTE(EXTENDED_KEY_INSERT)),
		SIMPLE(EXTE(EXTENDED_KEY_DELETE)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 07: Calculator keypad "row" 1
	{
		// The use of EXTN() here masks the Level 2 shift and NumLock that we are stripping out here with the keyboard map.
		NUMABLE(EXTN(EXTENDED_KEY_PAD_ASTERISK),	UCSA('*')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_HOME),		UCSA('7')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_UP),		UCSA('8')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_PAGE_UP),		UCSA('9')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_MINUS),		UCSA('-')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_LEFT),		UCSA('4')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_CENTRE),		UCSA('5')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_RIGHT),		UCSA('6')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_PLUS),		UCSA('+')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_END),		UCSA('1')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_DOWN),		UCSA('2')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_PAGE_DOWN),	UCSA('3')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_INSERT),		UCSA('0')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_DELETE),		UCSA('.')),
		NUMABLE_CONTROL(EXTN(EXTENDED_KEY_PAD_ENTER),	UCSA(0x0D),UCSA(0x0A)),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_SLASH),		UCSA('/')),
	},
	// 08: Calculator keypad "row" 2
	{
		// The use of EXTN() here masks the Level 2 shift and NumLock that we are stripping out here with the keyboard map.
		NUMABLE(EXTN(EXTENDED_KEY_PAD_COMMA),		UCSA(',')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_COMMA),		UCSA(',')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_EQUALS),		UCSA('=')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_EQUALS),		UCSA('=')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_SIGN),		UCSA(0xB1)),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_OPEN_BRACKET),	UCSA('(')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_CLOSE_BRACKET),	UCSA(')')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_OPEN_BRACE),	UCSA('{')),
		NUMABLE(EXTN(EXTENDED_KEY_PAD_CLOSE_BRACE),	UCSA('}')),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 09: Function row 1
	{
		SIMPLE(UCSA(0x1b)),
#if defined(__LINUX__) || defined(__linux__)
		FUNKABLE2(1,EXTENDED_KEY_PAD_F1),
		FUNKABLE2(2,EXTENDED_KEY_PAD_F2),
		FUNKABLE2(3,EXTENDED_KEY_PAD_F3),
		FUNKABLE2(4,EXTENDED_KEY_PAD_F4),
		FUNKABLE2(5,EXTENDED_KEY_PAD_F5),
		FUNKABLE1(6),
		FUNKABLE1(7),
		FUNKABLE1(8),
		FUNKABLE1(9),
		FUNKABLE1(10),
		FUNKABLE1(11),
		FUNKABLE1(12),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
#else
		FUNCABLE2(1,EXTENDED_KEY_PAD_F1),
		FUNCABLE2(2,EXTENDED_KEY_PAD_F2),
		FUNCABLE2(3,EXTENDED_KEY_PAD_F3),
		FUNCABLE2(4,EXTENDED_KEY_PAD_F4),
		FUNCABLE2(5,EXTENDED_KEY_PAD_F5),
		FUNCABLE1(6),
		FUNCABLE1(7),
		FUNCABLE1(8),
		FUNCABLE1(9),
		FUNCABLE1(10),
		FUNCABLE1(11),
		FUNCABLE1(12),
		FUNCABLE1(13),
		FUNCABLE1(14),
		FUNCABLE1(15),
#endif
	},
	// 0A: Function row 2
	{
#if defined(__LINUX__) || defined(__linux__)
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
#else
		FUNCABLE1(16),
		FUNCABLE1(17),
		FUNCABLE1(18),
		FUNCABLE1(19),
		FUNCABLE1(20),
		FUNCABLE1(21),
		FUNCABLE1(22),
		FUNCABLE1(23),
		FUNCABLE1(24),
		FUNCABLE1(25),
		FUNCABLE1(26),
		FUNCABLE1(27),
		FUNCABLE1(28),
		FUNCABLE1(29),
		FUNCABLE1(30),
		FUNCABLE1(31),
#endif
	},
	// 0B: Function row 3
	{
#if defined(__LINUX__) || defined(__linux__)
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
#else
		FUNCABLE1(32),
		FUNCABLE1(33),
		FUNCABLE1(34),
		FUNCABLE1(35),
		FUNCABLE1(36),
		FUNCABLE1(37),
		FUNCABLE1(38),
		FUNCABLE1(39),
		FUNCABLE1(40),
		FUNCABLE1(41),
		FUNCABLE1(42),
		FUNCABLE1(43),
		FUNCABLE1(44),
		FUNCABLE1(45),
		FUNCABLE1(46),
		FUNCABLE1(47),
#endif
	},
	// 0C: Function row 4
	{
#if defined(__LINUX__) || defined(__linux__)
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
#else
		FUNCABLE1(48),
		FUNCABLE1(49),
		FUNCABLE1(50),
		FUNCABLE1(51),
		FUNCABLE1(52),
		FUNCABLE1(53),
		FUNCABLE1(54),
		FUNCABLE1(55),
		FUNCABLE1(56),
		FUNCABLE1(57),
		FUNCABLE1(58),
		FUNCABLE1(59),
		FUNCABLE1(60),
		FUNCABLE1(61),
		FUNCABLE1(62),
		FUNCABLE1(63),
#endif
	},
	// 0D: System Commands keypad "row"
	{
		SIMPLE(NOOP(0)),
		SIMPLE(SYST(SYSTEM_KEY_POWER)),
		SIMPLE(SYST(SYSTEM_KEY_SLEEP)),
		SIMPLE(SYST(SYSTEM_KEY_WAKE)),
		SIMPLE(SYST(SYSTEM_KEY_DEBUG)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 0E: Application Commands keypad "row" 1
	{
		SIMPLE(EXTE(EXTENDED_KEY_PAUSE)),
		SHIFTABLE(CONS(CONSUMER_KEY_NEXT_TASK),CONS(CONSUMER_KEY_PREVIOUS_TASK),NOOP(0),NOOP(0),CONS(CONSUMER_KEY_NEXT_TASK),CONS(CONSUMER_KEY_PREVIOUS_TASK),NOOP(0),NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_ATTENTION)),
		SIMPLE(EXTE(EXTENDED_KEY_APPLICATION)),
		SIMPLE(EXTE(EXTENDED_KEY_BREAK)),
		SIMPLE(EXTE(EXTENDED_KEY_PRINT_SCREEN)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_MUTE)),
		SIMPLE(EXTE(EXTENDED_KEY_VOLUME_DOWN)),
		SIMPLE(EXTE(EXTENDED_KEY_VOLUME_UP)),
	},
	// 0F: Application Commands keypad "row" 2
	{
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_EXECUTE)),
		SIMPLE(EXTE(EXTENDED_KEY_HELP)),
		SIMPLE(EXTE(EXTENDED_KEY_MENU)),
		SIMPLE(EXTE(EXTENDED_KEY_SELECT)),
		SIMPLE(EXTE(EXTENDED_KEY_CANCEL)),
		SIMPLE(EXTE(EXTENDED_KEY_CLEAR)),
		SIMPLE(EXTE(EXTENDED_KEY_PRIOR)),
		SIMPLE(EXTE(EXTENDED_KEY_RETURN)),
		SIMPLE(EXTE(EXTENDED_KEY_SEPARATOR)),
		SIMPLE(EXTE(EXTENDED_KEY_OUT)),
		SIMPLE(EXTE(EXTENDED_KEY_OPER)),
		SIMPLE(EXTE(EXTENDED_KEY_CLEAR_OR_AGAIN)),
		SIMPLE(EXTE(EXTENDED_KEY_EX_SEL)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 10: Application Commands keypad "row" 3
	{
		SIMPLE(NOOP(0)),
		SIMPLE(EXTE(EXTENDED_KEY_STOP)),
		SIMPLE(EXTE(EXTENDED_KEY_AGAIN)),
		SIMPLE(EXTE(EXTENDED_KEY_PROPERTIES)),
		SIMPLE(EXTE(EXTENDED_KEY_UNDO)),
		SIMPLE(EXTE(EXTENDED_KEY_REDO)),
		SIMPLE(EXTE(EXTENDED_KEY_COPY)),
		SIMPLE(EXTE(EXTENDED_KEY_OPEN)),
		SIMPLE(EXTE(EXTENDED_KEY_PASTE)),
		SIMPLE(EXTE(EXTENDED_KEY_FIND)),
		SIMPLE(EXTE(EXTENDED_KEY_CUT)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
	// 11: Consumer keypad "row" 1
	{
		SIMPLE(CONS(CONSUMER_KEY_CALCULATOR)),
		SIMPLE(CONS(CONSUMER_KEY_FILE_MANAGER)),
		SIMPLE(CONS(CONSUMER_KEY_WWW)),
		SIMPLE(CONS(CONSUMER_KEY_HOME)),
		SIMPLE(CONS(CONSUMER_KEY_REFRESH)),
		SIMPLE(CONS(CONSUMER_KEY_MAIL)),
		SIMPLE(CONS(CONSUMER_KEY_BOOKMARKS)),
		SIMPLE(CONS(CONSUMER_KEY_COMPUTER)),
		SIMPLE(CONS(CONSUMER_KEY_BACK)),
		SIMPLE(CONS(CONSUMER_KEY_FORWARD)),
		SIMPLE(CONS(CONSUMER_KEY_LOCK)),
		SIMPLE(CONS(CONSUMER_KEY_CLI)),
		SIMPLE(CONS(CONSUMER_KEY_NEXT_TRACK)),
		SIMPLE(CONS(CONSUMER_KEY_PREV_TRACK)),
		SIMPLE(CONS(CONSUMER_KEY_PLAY_PAUSE)),
		SIMPLE(CONS(CONSUMER_KEY_STOP_PLAYING)),
	},
	// 12: Consumer keypad "row" 2
	{
		SIMPLE(CONS(CONSUMER_KEY_RECORD)),
		SIMPLE(CONS(CONSUMER_KEY_REWIND)),
		SIMPLE(CONS(CONSUMER_KEY_FAST_FORWARD)),
		SIMPLE(CONS(CONSUMER_KEY_EJECT)),
		SIMPLE(CONS(CONSUMER_KEY_NEW)),
		SIMPLE(CONS(CONSUMER_KEY_EXIT)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
		SIMPLE(NOOP(0)),
	},
};

/* Map utility functions for setting defaults *******************************
// **************************************************************************
*/

void 
overlay_group2_latch (
	KeyboardMap & map
) {
	kbdmap_entry & e(map[KBDMAP_INDEX_OPTION >> 8][KBDMAP_INDEX_OPTION & 0xFF]);
	e.p[1] = e.p[5] = e.p[9] = e.p[13] = LTCH(KBDMAP_MODIFIER_1ST_GROUP2);
}

void 
set_default (
	KeyboardMap & map
) {
	for (unsigned i(0U); i < KBDMAP_ROWS; ++i) {
		for (unsigned j(0U); j < KBDMAP_COLS; ++j)
			map[i][j] = English_international_keyboard[i][j];
	}
}

void 
wipe (
	KeyboardMap & map
) {
	for (unsigned i(0U); i < KBDMAP_ROWS; ++i) {
		for (unsigned j(0U); j < KBDMAP_COLS; ++j) {
			kbdmap_entry & e(map[i][j]);
			e.cmd = '\0';
			for (unsigned k(0U); k < sizeof e.p/sizeof *e.p; ++k)
				e.p[k] = NOOP(0);
		}
	}
}
