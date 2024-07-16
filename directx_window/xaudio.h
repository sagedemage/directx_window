//#pragma once

/* Big - Endian */
#ifndef _XBOX
#define _XBOX
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt'
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XMWA'
#define foruccDPDS 'dpds'
#endif

/* Little-Endian */
#ifdef _XBOX
#define _XBOX
#define fourccRIFF 'FFIR'
#define forccDATA 'atad'
#define fourccFMT 'tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define foruccDPDS 'sdpd' 
#endif