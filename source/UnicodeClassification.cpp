/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <algorithm>
#include <stdint.h>
#include "UnicodeClassification.h"

namespace {

struct ClosedRange {
	uint32_t first, last;
	bool operator < (const ClosedRange &) const;
	bool Contains (uint32_t character) const { return first <= character && character <= last; }
};

struct ClosedRangeWithRank {
	uint32_t first, last;
	unsigned rank;
	bool operator < (const ClosedRangeWithRank &) const;
	bool Contains (uint32_t character) const { return first <= character && character <= last; }
};

inline
bool
ClosedRange::operator < (
	const ClosedRange & b
) const {
	return last < b.first;
}

inline
bool
ClosedRangeWithRank::operator < (
	const ClosedRangeWithRank & b
) const {
	return last < b.first;
}

static inline
bool
Contains (
	const ClosedRange * begin,
	const ClosedRange * end,
	uint32_t character
) {
	const ClosedRange one = { character, character };
	const ClosedRange * p(std::lower_bound(begin, end, one));
	if (p >= end) return false;
	return p->Contains(character);
}

static inline
unsigned int
Rank (
	const ClosedRangeWithRank * begin,
	const ClosedRangeWithRank * end,
	uint32_t character,
	unsigned int d
) {
	const ClosedRangeWithRank one = { character, character, -1U };
	const ClosedRangeWithRank * p(std::lower_bound(begin, end, one));
	if (p >= end || !p->Contains(character)) return d;
	return p->rank;
}

// These ranges are derived from the Unicode Data Table published by the Unicode Consortium.
static const 
ClosedRange
Mn[] = {
	{ 0x00000300, 0x0000036F }, { 0x00000483, 0x00000487 }, { 0x00000591, 0x000005BD }, { 0x000005BF, 0x000005BF },
	{ 0x000005C1, 0x000005C2 }, { 0x000005C4, 0x000005C5 }, { 0x000005C7, 0x000005C7 }, { 0x00000610, 0x0000061A },
	{ 0x0000064B, 0x0000065F }, { 0x00000670, 0x00000670 }, { 0x000006D6, 0x000006DC }, { 0x000006DF, 0x000006E4 },
	{ 0x000006E7, 0x000006E8 }, { 0x000006EA, 0x000006ED }, { 0x00000711, 0x00000711 }, { 0x00000730, 0x0000074A },
	{ 0x000007A6, 0x000007B0 }, { 0x000007EB, 0x000007F3 }, { 0x00000816, 0x00000819 }, { 0x0000081B, 0x00000823 },
	{ 0x00000825, 0x00000827 }, { 0x00000829, 0x0000082D }, { 0x00000859, 0x0000085B }, { 0x000008E4, 0x00000902 },
	{ 0x0000093A, 0x0000093A }, { 0x0000093C, 0x0000093C }, { 0x00000941, 0x00000948 }, { 0x0000094D, 0x0000094D },
	{ 0x00000951, 0x00000957 }, { 0x00000962, 0x00000963 }, { 0x00000981, 0x00000981 }, { 0x000009BC, 0x000009BC },
	{ 0x000009C1, 0x000009C4 }, { 0x000009CD, 0x000009CD }, { 0x000009E2, 0x000009E3 }, { 0x00000A01, 0x00000A02 },
	{ 0x00000A3C, 0x00000A3C }, { 0x00000A41, 0x00000A42 }, { 0x00000A47, 0x00000A48 }, { 0x00000A4B, 0x00000A4D },
	{ 0x00000A51, 0x00000A51 }, { 0x00000A70, 0x00000A71 }, { 0x00000A75, 0x00000A75 }, { 0x00000A81, 0x00000A82 },
	{ 0x00000ABC, 0x00000ABC }, { 0x00000AC1, 0x00000AC5 }, { 0x00000AC7, 0x00000AC8 }, { 0x00000ACD, 0x00000ACD },
	{ 0x00000AE2, 0x00000AE3 }, { 0x00000B01, 0x00000B01 }, { 0x00000B3C, 0x00000B3C }, { 0x00000B3F, 0x00000B3F },
	{ 0x00000B41, 0x00000B44 }, { 0x00000B4D, 0x00000B4D }, { 0x00000B56, 0x00000B56 }, { 0x00000B62, 0x00000B63 },
	{ 0x00000B82, 0x00000B82 }, { 0x00000BC0, 0x00000BC0 }, { 0x00000BCD, 0x00000BCD }, { 0x00000C00, 0x00000C00 },
	{ 0x00000C3E, 0x00000C40 }, { 0x00000C46, 0x00000C48 }, { 0x00000C4A, 0x00000C4D }, { 0x00000C55, 0x00000C56 },
	{ 0x00000C62, 0x00000C63 }, { 0x00000C81, 0x00000C81 }, { 0x00000CBC, 0x00000CBC }, { 0x00000CBF, 0x00000CBF },
	{ 0x00000CC6, 0x00000CC6 }, { 0x00000CCC, 0x00000CCD }, { 0x00000CE2, 0x00000CE3 }, { 0x00000D01, 0x00000D01 },
	{ 0x00000D41, 0x00000D44 }, { 0x00000D4D, 0x00000D4D }, { 0x00000D62, 0x00000D63 }, { 0x00000DCA, 0x00000DCA },
	{ 0x00000DD2, 0x00000DD4 }, { 0x00000DD6, 0x00000DD6 }, { 0x00000E31, 0x00000E31 }, { 0x00000E34, 0x00000E3A },
	{ 0x00000E47, 0x00000E4E }, { 0x00000EB1, 0x00000EB1 }, { 0x00000EB4, 0x00000EB9 }, { 0x00000EBB, 0x00000EBC },
	{ 0x00000EC8, 0x00000ECD }, { 0x00000F18, 0x00000F19 }, { 0x00000F35, 0x00000F35 }, { 0x00000F37, 0x00000F37 },
	{ 0x00000F39, 0x00000F39 }, { 0x00000F71, 0x00000F7E }, { 0x00000F80, 0x00000F84 }, { 0x00000F86, 0x00000F87 },
	{ 0x00000F8D, 0x00000F97 }, { 0x00000F99, 0x00000FBC }, { 0x00000FC6, 0x00000FC6 }, { 0x0000102D, 0x00001030 },
	{ 0x00001032, 0x00001037 }, { 0x00001039, 0x0000103A }, { 0x0000103D, 0x0000103E }, { 0x00001058, 0x00001059 },
	{ 0x0000105E, 0x00001060 }, { 0x00001071, 0x00001074 }, { 0x00001082, 0x00001082 }, { 0x00001085, 0x00001086 },
	{ 0x0000108D, 0x0000108D }, { 0x0000109D, 0x0000109D }, { 0x0000135D, 0x0000135F }, { 0x00001712, 0x00001714 },
	{ 0x00001732, 0x00001734 }, { 0x00001752, 0x00001753 }, { 0x00001772, 0x00001773 }, { 0x000017B4, 0x000017B5 },
	{ 0x000017B7, 0x000017BD }, { 0x000017C6, 0x000017C6 }, { 0x000017C9, 0x000017D3 }, { 0x000017DD, 0x000017DD },
	{ 0x0000180B, 0x0000180D }, { 0x000018A9, 0x000018A9 }, { 0x00001920, 0x00001922 }, { 0x00001927, 0x00001928 },
	{ 0x00001932, 0x00001932 }, { 0x00001939, 0x0000193B }, { 0x00001A17, 0x00001A18 }, { 0x00001A1B, 0x00001A1B },
	{ 0x00001A56, 0x00001A56 }, { 0x00001A58, 0x00001A5E }, { 0x00001A60, 0x00001A60 }, { 0x00001A62, 0x00001A62 },
	{ 0x00001A65, 0x00001A6C }, { 0x00001A73, 0x00001A7C }, { 0x00001A7F, 0x00001A7F }, { 0x00001AB0, 0x00001ABD },
	{ 0x00001B00, 0x00001B03 }, { 0x00001B34, 0x00001B34 }, { 0x00001B36, 0x00001B3A }, { 0x00001B3C, 0x00001B3C },
	{ 0x00001B42, 0x00001B42 }, { 0x00001B6B, 0x00001B73 }, { 0x00001B80, 0x00001B81 }, { 0x00001BA2, 0x00001BA5 },
	{ 0x00001BA8, 0x00001BA9 }, { 0x00001BAB, 0x00001BAD }, { 0x00001BE6, 0x00001BE6 }, { 0x00001BE8, 0x00001BE9 },
	{ 0x00001BED, 0x00001BED }, { 0x00001BEF, 0x00001BF1 }, { 0x00001C2C, 0x00001C33 }, { 0x00001C36, 0x00001C37 },
	{ 0x00001CD0, 0x00001CD2 }, { 0x00001CD4, 0x00001CE0 }, { 0x00001CE2, 0x00001CE8 }, { 0x00001CED, 0x00001CED },
	{ 0x00001CF4, 0x00001CF4 }, { 0x00001CF8, 0x00001CF9 }, { 0x00001DC0, 0x00001DF5 }, { 0x00001DFC, 0x00001DFF },
	{ 0x000020D0, 0x000020DC }, { 0x000020E1, 0x000020E1 }, { 0x000020E5, 0x000020F0 }, { 0x00002CEF, 0x00002CF1 },
	{ 0x00002D7F, 0x00002D7F }, { 0x00002DE0, 0x00002DFF }, { 0x0000302A, 0x0000302D }, { 0x00003099, 0x0000309A },
	{ 0x0000A66F, 0x0000A66F }, { 0x0000A674, 0x0000A67D }, { 0x0000A69F, 0x0000A69F }, { 0x0000A6F0, 0x0000A6F1 },
	{ 0x0000A802, 0x0000A802 }, { 0x0000A806, 0x0000A806 }, { 0x0000A80B, 0x0000A80B }, { 0x0000A825, 0x0000A826 },
	{ 0x0000A8C4, 0x0000A8C4 }, { 0x0000A8E0, 0x0000A8F1 }, { 0x0000A926, 0x0000A92D }, { 0x0000A947, 0x0000A951 },
	{ 0x0000A980, 0x0000A982 }, { 0x0000A9B3, 0x0000A9B3 }, { 0x0000A9B6, 0x0000A9B9 }, { 0x0000A9BC, 0x0000A9BC },
	{ 0x0000A9E5, 0x0000A9E5 }, { 0x0000AA29, 0x0000AA2E }, { 0x0000AA31, 0x0000AA32 }, { 0x0000AA35, 0x0000AA36 },
	{ 0x0000AA43, 0x0000AA43 }, { 0x0000AA4C, 0x0000AA4C }, { 0x0000AA7C, 0x0000AA7C }, { 0x0000AAB0, 0x0000AAB0 },
	{ 0x0000AAB2, 0x0000AAB4 }, { 0x0000AAB7, 0x0000AAB8 }, { 0x0000AABE, 0x0000AABF }, { 0x0000AAC1, 0x0000AAC1 },
	{ 0x0000AAEC, 0x0000AAED }, { 0x0000AAF6, 0x0000AAF6 }, { 0x0000ABE5, 0x0000ABE5 }, { 0x0000ABE8, 0x0000ABE8 },
	{ 0x0000ABED, 0x0000ABED }, { 0x0000FB1E, 0x0000FB1E }, { 0x0000FE00, 0x0000FE0F }, { 0x0000FE20, 0x0000FE2D },
	{ 0x000101FD, 0x000101FD }, { 0x000102E0, 0x000102E0 }, { 0x00010376, 0x0001037A }, { 0x00010A01, 0x00010A03 },
	{ 0x00010A05, 0x00010A06 }, { 0x00010A0C, 0x00010A0F }, { 0x00010A38, 0x00010A3A }, { 0x00010A3F, 0x00010A3F },
	{ 0x00010AE5, 0x00010AE6 }, { 0x00011001, 0x00011001 }, { 0x00011038, 0x00011046 }, { 0x0001107F, 0x00011081 },
	{ 0x000110B3, 0x000110B6 }, { 0x000110B9, 0x000110BA }, { 0x00011100, 0x00011102 }, { 0x00011127, 0x0001112B },
	{ 0x0001112D, 0x00011134 }, { 0x00011173, 0x00011173 }, { 0x00011180, 0x00011181 }, { 0x000111B6, 0x000111BE },
	{ 0x0001122F, 0x00011231 }, { 0x00011234, 0x00011234 }, { 0x00011236, 0x00011237 }, { 0x000112DF, 0x000112DF },
	{ 0x000112E3, 0x000112EA }, { 0x00011301, 0x00011301 }, { 0x0001133C, 0x0001133C }, { 0x00011340, 0x00011340 },
	{ 0x00011366, 0x0001136C }, { 0x00011370, 0x00011374 }, { 0x000114B3, 0x000114B8 }, { 0x000114BA, 0x000114BA },
	{ 0x000114BF, 0x000114C0 }, { 0x000114C2, 0x000114C3 }, { 0x000115B2, 0x000115B5 }, { 0x000115BC, 0x000115BD },
	{ 0x000115BF, 0x000115C0 }, { 0x00011633, 0x0001163A }, { 0x0001163D, 0x0001163D }, { 0x0001163F, 0x00011640 },
	{ 0x000116AB, 0x000116AB }, { 0x000116AD, 0x000116AD }, { 0x000116B0, 0x000116B5 }, { 0x000116B7, 0x000116B7 },
	{ 0x00016AF0, 0x00016AF4 }, { 0x00016B30, 0x00016B36 }, { 0x00016F8F, 0x00016F92 }, { 0x0001BC9D, 0x0001BC9E },
	{ 0x0001D167, 0x0001D169 }, { 0x0001D17B, 0x0001D182 }, { 0x0001D185, 0x0001D18B }, { 0x0001D1AA, 0x0001D1AD },
	{ 0x0001D242, 0x0001D244 }, { 0x0001E8D0, 0x0001E8D6 }, { 0x000E0100, 0x000E01EF }, 
},
Me[] = {
	{ 0x00000488, 0x00000489 }, { 0x00001ABE, 0x00001ABE }, { 0x000020DD, 0x000020E4 }, { 0x0000A670, 0x0000A672 },
},
Cf[] = {
	{ 0x000000AD, 0x000000AD }, { 0x00000600, 0x00000605 }, { 0x0000061C, 0x0000061C }, { 0x000006DD, 0x000006DD },
	{ 0x0000070F, 0x0000070F }, { 0x0000180E, 0x0000180E }, { 0x0000200B, 0x0000200F }, { 0x0000202A, 0x0000202E }, 
	{ 0x00002060, 0x00002064 }, { 0x00002066, 0x0000206F }, { 0x0000FEFF, 0x0000FEFF }, { 0x0000FFF9, 0x0000FFFB },
	{ 0x000110BD, 0x000110BD }, { 0x0001BCA0, 0x0001BCA3 }, { 0x0001D173, 0x0001D17A }, { 0x000E0001, 0x000E007F }
};
static const ClosedRange * const Mn_end(Mn + sizeof Mn/sizeof *Mn);
static const ClosedRange * const Me_end(Me + sizeof Me/sizeof *Me);
static const ClosedRange * const Cf_end(Cf + sizeof Cf/sizeof *Cf);
static const 
ClosedRangeWithRank
CC[] = {
	{ 0x00000300, 0x00000314,  230 },
	{ 0x00000315, 0x00000315,  232 },
	{ 0x00000316, 0x00000319,  220 },
	{ 0x0000031a, 0x0000031a,  232 },
	{ 0x0000031b, 0x0000031b,  216 },
	{ 0x0000031c, 0x00000320,  220 },
	{ 0x00000321, 0x00000322,  202 },
	{ 0x00000323, 0x00000326,  220 },
	{ 0x00000327, 0x00000328,  202 },
	{ 0x00000329, 0x00000333,  220 },
	{ 0x00000334, 0x00000338,    1 },
	{ 0x00000339, 0x0000033c,  220 },
	{ 0x0000033d, 0x00000344,  230 },
	{ 0x00000345, 0x00000345,  240 },
	{ 0x00000346, 0x00000346,  230 },
	{ 0x00000347, 0x00000349,  220 },
	{ 0x0000034a, 0x0000034c,  230 },
	{ 0x0000034d, 0x0000034e,  220 },
	{ 0x00000350, 0x00000352,  230 },
	{ 0x00000353, 0x00000356,  220 },
	{ 0x00000357, 0x00000357,  230 },
	{ 0x00000358, 0x00000358,  232 },
	{ 0x00000359, 0x0000035a,  220 },
	{ 0x0000035b, 0x0000035b,  230 },
	{ 0x0000035c, 0x0000035c,  233 },
	{ 0x0000035d, 0x0000035e,  234 },
	{ 0x0000035f, 0x0000035f,  233 },
	{ 0x00000360, 0x00000361,  234 },
	{ 0x00000362, 0x00000362,  233 },
	{ 0x00000363, 0x0000036f,  230 },
	{ 0x00000483, 0x00000487,  230 },
	{ 0x00000591, 0x00000591,  220 },
	{ 0x00000592, 0x00000595,  230 },
	{ 0x00000596, 0x00000596,  220 },
	{ 0x00000597, 0x00000599,  230 },
	{ 0x0000059a, 0x0000059a,  222 },
	{ 0x0000059b, 0x0000059b,  220 },
	{ 0x0000059c, 0x000005a1,  230 },
	{ 0x000005a2, 0x000005a7,  220 },
	{ 0x000005a8, 0x000005a9,  230 },
	{ 0x000005aa, 0x000005aa,  220 },
	{ 0x000005ab, 0x000005ac,  230 },
	{ 0x000005ad, 0x000005ad,  222 },
	{ 0x000005ae, 0x000005ae,  228 },
	{ 0x000005af, 0x000005af,  230 },
	{ 0x000005b0, 0x000005b0,   10 },
	{ 0x000005b1, 0x000005b1,   11 },
	{ 0x000005b2, 0x000005b2,   12 },
	{ 0x000005b3, 0x000005b3,   13 },
	{ 0x000005b4, 0x000005b4,   14 },
	{ 0x000005b5, 0x000005b5,   15 },
	{ 0x000005b6, 0x000005b6,   16 },
	{ 0x000005b7, 0x000005b7,   17 },
	{ 0x000005b8, 0x000005b8,   18 },
	{ 0x000005b9, 0x000005ba,   19 },
	{ 0x000005bb, 0x000005bb,   20 },
	{ 0x000005bc, 0x000005bc,   21 },
	{ 0x000005bd, 0x000005bd,   22 },
	{ 0x000005bf, 0x000005bf,   23 },
	{ 0x000005c1, 0x000005c1,   24 },
	{ 0x000005c2, 0x000005c2,   25 },
	{ 0x000005c4, 0x000005c4,  230 },
	{ 0x000005c5, 0x000005c5,  220 },
	{ 0x000005c7, 0x000005c7,   18 },
	{ 0x00000610, 0x00000617,  230 },
	{ 0x00000618, 0x00000618,   30 },
	{ 0x00000619, 0x00000619,   31 },
	{ 0x0000061a, 0x0000061a,   32 },
	{ 0x0000064b, 0x0000064b,   27 },
	{ 0x0000064c, 0x0000064c,   28 },
	{ 0x0000064d, 0x0000064d,   29 },
	{ 0x0000064e, 0x0000064e,   30 },
	{ 0x0000064f, 0x0000064f,   31 },
	{ 0x00000650, 0x00000650,   32 },
	{ 0x00000651, 0x00000651,   33 },
	{ 0x00000652, 0x00000652,   34 },
	{ 0x00000653, 0x00000654,  230 },
	{ 0x00000655, 0x00000656,  220 },
	{ 0x00000657, 0x0000065b,  230 },
	{ 0x0000065c, 0x0000065c,  220 },
	{ 0x0000065d, 0x0000065e,  230 },
	{ 0x0000065f, 0x0000065f,  220 },
	{ 0x00000670, 0x00000670,   35 },
	{ 0x000006d6, 0x000006dc,  230 },
	{ 0x000006df, 0x000006e2,  230 },
	{ 0x000006e3, 0x000006e3,  220 },
	{ 0x000006e4, 0x000006e4,  230 },
	{ 0x000006e7, 0x000006e8,  230 },
	{ 0x000006ea, 0x000006ea,  220 },
	{ 0x000006eb, 0x000006ec,  230 },
	{ 0x000006ed, 0x000006ed,  220 },
	{ 0x00000711, 0x00000711,   36 },
	{ 0x00000730, 0x00000730,  230 },
	{ 0x00000731, 0x00000731,  220 },
	{ 0x00000732, 0x00000733,  230 },
	{ 0x00000734, 0x00000734,  220 },
	{ 0x00000735, 0x00000736,  230 },
	{ 0x00000737, 0x00000739,  220 },
	{ 0x0000073a, 0x0000073a,  230 },
	{ 0x0000073b, 0x0000073c,  220 },
	{ 0x0000073d, 0x0000073d,  230 },
	{ 0x0000073e, 0x0000073e,  220 },
	{ 0x0000073f, 0x00000741,  230 },
	{ 0x00000742, 0x00000742,  220 },
	{ 0x00000743, 0x00000743,  230 },
	{ 0x00000744, 0x00000744,  220 },
	{ 0x00000745, 0x00000745,  230 },
	{ 0x00000746, 0x00000746,  220 },
	{ 0x00000747, 0x00000747,  230 },
	{ 0x00000748, 0x00000748,  220 },
	{ 0x00000749, 0x0000074a,  230 },
	{ 0x000007eb, 0x000007f1,  230 },
	{ 0x000007f2, 0x000007f2,  220 },
	{ 0x000007f3, 0x000007f3,  230 },
	{ 0x00000816, 0x00000819,  230 },
	{ 0x0000081b, 0x00000823,  230 },
	{ 0x00000825, 0x00000827,  230 },
	{ 0x00000829, 0x0000082d,  230 },
	{ 0x00000859, 0x0000085b,  220 },
	{ 0x000008e4, 0x000008e5,  230 },
	{ 0x000008e6, 0x000008e6,  220 },
	{ 0x000008e7, 0x000008e8,  230 },
	{ 0x000008e9, 0x000008e9,  220 },
	{ 0x000008ea, 0x000008ec,  230 },
	{ 0x000008ed, 0x000008ef,  220 },
	{ 0x000008f0, 0x000008f0,   27 },
	{ 0x000008f1, 0x000008f1,   28 },
	{ 0x000008f2, 0x000008f2,   29 },
	{ 0x000008f3, 0x000008f5,  230 },
	{ 0x000008f6, 0x000008f6,  220 },
	{ 0x000008f7, 0x000008f8,  230 },
	{ 0x000008f9, 0x000008fa,  220 },
	{ 0x000008fb, 0x000008ff,  230 },
	{ 0x0000093c, 0x0000093c,    7 },
	{ 0x0000094d, 0x0000094d,    9 },
	{ 0x00000951, 0x00000951,  230 },
	{ 0x00000952, 0x00000952,  220 },
	{ 0x00000953, 0x00000954,  230 },
	{ 0x000009bc, 0x000009bc,    7 },
	{ 0x000009cd, 0x000009cd,    9 },
	{ 0x00000a3c, 0x00000a3c,    7 },
	{ 0x00000a4d, 0x00000a4d,    9 },
	{ 0x00000abc, 0x00000abc,    7 },
	{ 0x00000acd, 0x00000acd,    9 },
	{ 0x00000b3c, 0x00000b3c,    7 },
	{ 0x00000b4d, 0x00000b4d,    9 },
	{ 0x00000bcd, 0x00000bcd,    9 },
	{ 0x00000c4d, 0x00000c4d,    9 },
	{ 0x00000c55, 0x00000c55,   84 },
	{ 0x00000c56, 0x00000c56,   91 },
	{ 0x00000cbc, 0x00000cbc,    7 },
	{ 0x00000ccd, 0x00000ccd,    9 },
	{ 0x00000d4d, 0x00000d4d,    9 },
	{ 0x00000dca, 0x00000dca,    9 },
	{ 0x00000e38, 0x00000e39,  103 },
	{ 0x00000e3a, 0x00000e3a,    9 },
	{ 0x00000e48, 0x00000e4b,  107 },
	{ 0x00000eb8, 0x00000eb9,  118 },
	{ 0x00000ec8, 0x00000ecb,  122 },
	{ 0x00000f18, 0x00000f19,  220 },
	{ 0x00000f35, 0x00000f35,  220 },
	{ 0x00000f37, 0x00000f37,  220 },
	{ 0x00000f39, 0x00000f39,  216 },
	{ 0x00000f71, 0x00000f71,  129 },
	{ 0x00000f72, 0x00000f72,  130 },
	{ 0x00000f74, 0x00000f74,  132 },
	{ 0x00000f7a, 0x00000f7d,  130 },
	{ 0x00000f80, 0x00000f80,  130 },
	{ 0x00000f82, 0x00000f83,  230 },
	{ 0x00000f84, 0x00000f84,    9 },
	{ 0x00000f86, 0x00000f87,  230 },
	{ 0x00000fc6, 0x00000fc6,  220 },
	{ 0x00001037, 0x00001037,    7 },
	{ 0x00001039, 0x0000103a,    9 },
	{ 0x0000108d, 0x0000108d,  220 },
	{ 0x0000135d, 0x0000135f,  230 },
	{ 0x00001714, 0x00001714,    9 },
	{ 0x00001734, 0x00001734,    9 },
	{ 0x000017d2, 0x000017d2,    9 },
	{ 0x000017dd, 0x000017dd,  230 },
	{ 0x000018a9, 0x000018a9,  228 },
	{ 0x00001939, 0x00001939,  222 },
	{ 0x0000193a, 0x0000193a,  230 },
	{ 0x0000193b, 0x0000193b,  220 },
	{ 0x00001a17, 0x00001a17,  230 },
	{ 0x00001a18, 0x00001a18,  220 },
	{ 0x00001a60, 0x00001a60,    9 },
	{ 0x00001a75, 0x00001a7c,  230 },
	{ 0x00001a7f, 0x00001a7f,  220 },
	{ 0x00001ab0, 0x00001ab4,  230 },
	{ 0x00001ab5, 0x00001aba,  220 },
	{ 0x00001abb, 0x00001abc,  230 },
	{ 0x00001abd, 0x00001abd,  220 },
	{ 0x00001b34, 0x00001b34,    7 },
	{ 0x00001b44, 0x00001b44,    9 },
	{ 0x00001b6b, 0x00001b6b,  230 },
	{ 0x00001b6c, 0x00001b6c,  220 },
	{ 0x00001b6d, 0x00001b73,  230 },
	{ 0x00001baa, 0x00001bab,    9 },
	{ 0x00001be6, 0x00001be6,    7 },
	{ 0x00001bf2, 0x00001bf3,    9 },
	{ 0x00001c37, 0x00001c37,    7 },
	{ 0x00001cd0, 0x00001cd2,  230 },
	{ 0x00001cd4, 0x00001cd4,    1 },
	{ 0x00001cd5, 0x00001cd9,  220 },
	{ 0x00001cda, 0x00001cdb,  230 },
	{ 0x00001cdc, 0x00001cdf,  220 },
	{ 0x00001ce0, 0x00001ce0,  230 },
	{ 0x00001ce2, 0x00001ce8,    1 },
	{ 0x00001ced, 0x00001ced,  220 },
	{ 0x00001cf4, 0x00001cf4,  230 },
	{ 0x00001cf8, 0x00001cf9,  230 },
	{ 0x00001dc0, 0x00001dc1,  230 },
	{ 0x00001dc2, 0x00001dc2,  220 },
	{ 0x00001dc3, 0x00001dc9,  230 },
	{ 0x00001dca, 0x00001dca,  220 },
	{ 0x00001dcb, 0x00001dcc,  230 },
	{ 0x00001dcd, 0x00001dcd,  234 },
	{ 0x00001dce, 0x00001dce,  214 },
	{ 0x00001dcf, 0x00001dcf,  220 },
	{ 0x00001dd0, 0x00001dd0,  202 },
	{ 0x00001dd1, 0x00001df5,  230 },
	{ 0x00001dfc, 0x00001dfc,  233 },
	{ 0x00001dfd, 0x00001dfd,  220 },
	{ 0x00001dfe, 0x00001dfe,  230 },
	{ 0x00001dff, 0x00001dff,  220 },
	{ 0x000020d0, 0x000020d1,  230 },
	{ 0x000020d2, 0x000020d3,    1 },
	{ 0x000020d4, 0x000020d7,  230 },
	{ 0x000020d8, 0x000020da,    1 },
	{ 0x000020db, 0x000020dc,  230 },
	{ 0x000020e1, 0x000020e1,  230 },
	{ 0x000020e5, 0x000020e6,    1 },
	{ 0x000020e7, 0x000020e7,  230 },
	{ 0x000020e8, 0x000020e8,  220 },
	{ 0x000020e9, 0x000020e9,  230 },
	{ 0x000020ea, 0x000020eb,    1 },
	{ 0x000020ec, 0x000020ef,  220 },
	{ 0x000020f0, 0x000020f0,  230 },
	{ 0x00002cef, 0x00002cf1,  230 },
	{ 0x00002d7f, 0x00002d7f,    9 },
	{ 0x00002de0, 0x00002dff,  230 },
	{ 0x0000302a, 0x0000302a,  218 },
	{ 0x0000302b, 0x0000302b,  228 },
	{ 0x0000302c, 0x0000302c,  232 },
	{ 0x0000302d, 0x0000302d,  222 },
	{ 0x0000302e, 0x0000302f,  224 },
	{ 0x00003099, 0x0000309a,    8 },
	{ 0x0000a66f, 0x0000a66f,  230 },
	{ 0x0000a674, 0x0000a67d,  230 },
	{ 0x0000a69f, 0x0000a69f,  230 },
	{ 0x0000a6f0, 0x0000a6f1,  230 },
	{ 0x0000a806, 0x0000a806,    9 },
	{ 0x0000a8c4, 0x0000a8c4,    9 },
	{ 0x0000a8e0, 0x0000a8f1,  230 },
	{ 0x0000a92b, 0x0000a92d,  220 },
	{ 0x0000a953, 0x0000a953,    9 },
	{ 0x0000a9b3, 0x0000a9b3,    7 },
	{ 0x0000a9c0, 0x0000a9c0,    9 },
	{ 0x0000aab0, 0x0000aab0,  230 },
	{ 0x0000aab2, 0x0000aab3,  230 },
	{ 0x0000aab4, 0x0000aab4,  220 },
	{ 0x0000aab7, 0x0000aab8,  230 },
	{ 0x0000aabe, 0x0000aabf,  230 },
	{ 0x0000aac1, 0x0000aac1,  230 },
	{ 0x0000aaf6, 0x0000aaf6,    9 },
	{ 0x0000abed, 0x0000abed,    9 },
	{ 0x0000fb1e, 0x0000fb1e,   26 },
	{ 0x0000fe20, 0x0000fe26,  230 },
	{ 0x0000fe27, 0x0000fe2d,  220 },
	{ 0x000101fd, 0x000101fd,  220 },
	{ 0x000102e0, 0x000102e0,  220 },
	{ 0x00010376, 0x0001037a,  230 },
	{ 0x00010a0d, 0x00010a0d,  220 },
	{ 0x00010a0f, 0x00010a0f,  230 },
	{ 0x00010a38, 0x00010a38,  230 },
	{ 0x00010a39, 0x00010a39,    1 },
	{ 0x00010a3a, 0x00010a3a,  220 },
	{ 0x00010a3f, 0x00010a3f,    9 },
	{ 0x00010ae5, 0x00010ae5,  230 },
	{ 0x00010ae6, 0x00010ae6,  220 },
	{ 0x00011046, 0x00011046,    9 },
	{ 0x0001107f, 0x0001107f,    9 },
	{ 0x000110b9, 0x000110b9,    9 },
	{ 0x000110ba, 0x000110ba,    7 },
	{ 0x00011100, 0x00011102,  230 },
	{ 0x00011133, 0x00011134,    9 },
	{ 0x00011173, 0x00011173,    7 },
	{ 0x000111c0, 0x000111c0,    9 },
	{ 0x00011235, 0x00011235,    9 },
	{ 0x00011236, 0x00011236,    7 },
	{ 0x000112e9, 0x000112e9,    7 },
	{ 0x000112ea, 0x000112ea,    9 },
	{ 0x0001133c, 0x0001133c,    7 },
	{ 0x0001134d, 0x0001134d,    9 },
	{ 0x00011366, 0x0001136c,  230 },
	{ 0x00011370, 0x00011374,  230 },
	{ 0x000114c2, 0x000114c2,    9 },
	{ 0x000114c3, 0x000114c3,    7 },
	{ 0x000115bf, 0x000115bf,    9 },
	{ 0x000115c0, 0x000115c0,    7 },
	{ 0x0001163f, 0x0001163f,    9 },
	{ 0x000116b6, 0x000116b6,    9 },
	{ 0x000116b7, 0x000116b7,    7 },
	{ 0x00016af0, 0x00016af4,    1 },
	{ 0x00016b30, 0x00016b36,  230 },
	{ 0x0001bc9e, 0x0001bc9e,    1 },
	{ 0x0001d165, 0x0001d166,  216 },
	{ 0x0001d167, 0x0001d169,    1 },
	{ 0x0001d16d, 0x0001d16d,  226 },
	{ 0x0001d16e, 0x0001d172,  216 },
	{ 0x0001d17b, 0x0001d182,  220 },
	{ 0x0001d185, 0x0001d189,  230 },
	{ 0x0001d18a, 0x0001d18b,  220 },
	{ 0x0001d1aa, 0x0001d1ad,  230 },
	{ 0x0001d242, 0x0001d244,  230 },
	{ 0x0001e8d0, 0x0001e8d6,  220 },
};
static const ClosedRangeWithRank * const CC_end(CC + sizeof CC/sizeof *CC);
 
}

namespace UnicodeCategorization {

bool 
IsMarkNonSpacing(uint32_t character)
{
	return Contains(Mn, Mn_end, character);
}

bool 
IsMarkEnclosing(uint32_t character)
{
	return Contains(Me, Me_end, character);
}

bool 
IsOtherFormat(uint32_t character)
{
	return Contains(Cf, Cf_end, character);
}

unsigned int 
CombiningClass(uint32_t character)
{
	return Rank(CC, CC_end, character, 0U);
}

}
