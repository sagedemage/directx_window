//#pragma once

/* Big - Endian */
#ifdef _XBOX
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt'
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define foruccDPDS 'dpds'
#endif

/* Little-Endian */
#ifndef _XBOX
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT 'tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define foruccDPDS 'sdpd' 
#endif