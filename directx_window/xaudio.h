//#pragma once

/* Big - Endian */
#ifdef _XBOX
#define fourccRIFF 'RIFF' // 1380533830
#define fourccDATA 'data' // 1684108385
#define fourccFMT 'fmt' // 6712692
#define fourccWAVE 'WAVE' // 1463899717
#define fourccXWMA 'XWMA' // 1482116417
#define foruccDPDS 'dpds' // 1685087347
#endif

/* Little-Endian */
#ifndef _XBOX
#define fourccRIFF 'FFIR' // 1179011410
#define fourccDATA 'atad' // 1635017060
#define fourccFMT 'tmf' // 7630182
#define fourccWAVE 'EVAW' // 1163280727
#define fourccXWMA 'AMWX' // 1095587672
#define foruccDPDS 'sdpd' // 1935962212
#endif