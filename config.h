#ifndef __CONFIG_H__
#define __CONFIG_H__

#define	BAUD		19200

#ifndef __DEMO__
#define DISPLAYDRIVER	"ILI9340.h"
#define	DISPLAY		ILI9340

#define HAS_DRAWHLINE
#define	HAS_DRAWVLINE
#else
#define	DISPLAYDRIVER	"demo.h"
#define	DISPLAY		DEMO
#endif

#endif
