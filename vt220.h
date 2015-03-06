#ifndef __VT220_H__
#define __VT220_H__

#define	KAM		2	// Keyboard Action Mode
#define	IRM		4	// Insert/Replace Mode
#define	SRM		12	// Send/Receive Mode
#define	LNM		20	// Line Feed/New Line Mode
#define	DECTCEM		25	// Text Cursor Enable Mode
#define	DECCKM		1	// Cursor Key Mode
#define	DECANM		2	// ANSI/VT52 Mode
#define	DECCOLM		3	// Column Mode
#define	DECSCLM		4	// Scrolling Mode
#define	DECSCNM		5	// Screen Mode
#define	DECOM		6	// Origin Mode
#define	DECAWM		7	// Auto Wrap Mode
#define	DECARM		8	// Auto Repeat
#define	DECPFF		18	// Print Form Feed Mode
#define	DECPEX		19	// Print Extent Mode
#define	DECCKPAM	'='	// Application Keypad Mode
#define	DECKPNM		'>'	// Numeric Keypad Mode
#define	DECNRCM		42	// Character Set Mode
#define	CUU		'A'	// Cursor Up
#define	CUD		'B'	// Cursor Down
#define	CUF		'C'	// Cursor Forward
#define	CUB		'D'	// Cursor Backward
#define	CUP		'H'	// Cursor Position
#define	HVP		'f'	// Horizontal And Vertical Position
#define	DECSC		7	// Save Cursor
#define	DECRC		8	// Restore Cursor
#define	DECSWL		5	// Single-Width Line
#define	DECDWL		6	// Double-Width Line
#define	DECSTBM		'r'	// Set Top and Bottom Margins

#define	DISPLAY_BOLD		0
#define	DISPLAY_UNDERSCORED	1
#define	DISPLAY_BLINKING	2
#define	DISPLAY_REVERSE		3

#define	MODE_7BIT	0
#define	MODE_VT52COMPAT	1

#endif
