/*
 * Intel Indeo 5 Video Decoder
 * Copyright 2023 Shaun Ren for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <stdarg.h>
#include <stdlib.h>
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "commdlg.h"
#include "vfw.h"
#include "ir50_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ir50_32);

static HINSTANCE IR50_32_hModule;

#define IV50_MAGIC mmioFOURCC('I','V','5','0')


static LRESULT
IV50_Open( const ICINFO *icinfo )
{
    FIXME("DRV_OPEN %p\n", icinfo);
    return 0;
}

static LRESULT
IV50_DecompressQuery( LPBITMAPINFO in, LPBITMAPINFO out )
{
    FIXME("ICM_DECOMPRESS_QUERY %p %p\n", in, out);
    return ICERR_UNSUPPORTED;
}

static LRESULT
IV50_DecompressGetFormat( LPBITMAPINFO in, LPBITMAPINFO out )
{
    FIXME("ICM_DECOMPRESS_GETFORMAT %p %p\n", in, out);
    return ICERR_UNSUPPORTED;
}

static LRESULT IV50_DecompressBegin( IMFTransform *decoder, LPBITMAPINFO in, LPBITMAPINFO out )
{
    FIXME("ICM_DECOMPRESS_BEGIN %p %p %p\n", decoder, in, out);
    return ICERR_UNSUPPORTED;
}

static LRESULT IV50_Decompress( IMFTransform *decoder, ICDECOMPRESS *icd, DWORD size )
{
    FIXME("ICM_DECOMPRESS %p %p %lu\n", decoder, icd, size);
    return ICERR_UNSUPPORTED;
}

static LRESULT IV50_GetInfo( ICINFO *icinfo, DWORD dwSize )
{
    TRACE("ICM_GETINFO %p %lu\n", icinfo, dwSize);

    if ( !icinfo ) return sizeof(ICINFO);
    if ( dwSize < sizeof(ICINFO) ) return 0;

    icinfo->dwSize       = sizeof(ICINFO);
    icinfo->fccType      = ICTYPE_VIDEO;
    icinfo->fccHandler   = IV50_MAGIC;
    icinfo->dwFlags      = 0;
    icinfo->dwVersion    = ICVERSION;
    icinfo->dwVersionICM = ICVERSION;

    LoadStringW( IR50_32_hModule, IDS_NAME, icinfo->szName, ARRAY_SIZE(icinfo->szName) );
    LoadStringW( IR50_32_hModule, IDS_DESCRIPTION, icinfo->szDescription, ARRAY_SIZE(icinfo->szDescription) );
    /* msvfw32 will fill icinfo->szDriver for us */

    return sizeof(ICINFO);
}

/***********************************************************************
 *              DriverProc (IR50_32.@)
 */
LRESULT WINAPI IV50_DriverProc( DWORD_PTR dwDriverId, HDRVR hdrvr, UINT msg,
                                LPARAM lParam1, LPARAM lParam2 )
{
    IMFTransform *decoder = (IMFTransform *) dwDriverId;
    LRESULT r = ICERR_UNSUPPORTED;

    TRACE("%Id %p %04x %08Ix %08Ix\n", dwDriverId, hdrvr, msg, lParam1, lParam2);

    switch( msg )
    {
    case DRV_LOAD:
        TRACE("DRV_LOAD\n");
        r = 1;
        break;

    case DRV_OPEN:
        r = IV50_Open((ICINFO *)lParam2);
        break;

    case DRV_CLOSE:
        FIXME("DRV_CLOSE\n");
        break;

    case DRV_ENABLE:
    case DRV_DISABLE:
    case DRV_FREE:
        break;

    case ICM_GETINFO:
        r = IV50_GetInfo( (ICINFO *) lParam1, (DWORD) lParam2 );
        break;

    case ICM_DECOMPRESS_QUERY:
        r = IV50_DecompressQuery( (LPBITMAPINFO) lParam1, (LPBITMAPINFO) lParam2 );
        break;

    case ICM_DECOMPRESS_GET_FORMAT:
        r = IV50_DecompressGetFormat( (LPBITMAPINFO) lParam1, (LPBITMAPINFO) lParam2 );
        break;

    case ICM_DECOMPRESS_GET_PALETTE:
        FIXME("ICM_DECOMPRESS_GET_PALETTE\n");
        break;

    case ICM_DECOMPRESS:
        r = IV50_Decompress( decoder, (ICDECOMPRESS *) lParam1, (DWORD) lParam2 );
        break;

    case ICM_DECOMPRESS_BEGIN:
        r = IV50_DecompressBegin( decoder, (LPBITMAPINFO) lParam1, (LPBITMAPINFO) lParam2 );
        break;

    case ICM_DECOMPRESS_END:
        r = ICERR_UNSUPPORTED;
        break;

    case ICM_DECOMPRESSEX_QUERY:
        FIXME("ICM_DECOMPRESSEX_QUERY\n");
        break;

    case ICM_DECOMPRESSEX:
        FIXME("ICM_DECOMPRESSEX\n");
        break;

    case ICM_COMPRESS_QUERY:
        r = ICERR_BADFORMAT;
        /* fall through */
    case ICM_COMPRESS_GET_FORMAT:
    case ICM_COMPRESS_END:
    case ICM_COMPRESS:
        FIXME("compression not implemented\n");
        break;

    case ICM_CONFIGURE:
        break;

    default:
        FIXME("Unknown message: %04x %Id %Id\n", msg, lParam1, lParam2);
    }

    return r;
}

/***********************************************************************
 *              DllMain
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    TRACE("(%p,%lu,%p)\n", hModule, dwReason, lpReserved);

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        IR50_32_hModule = hModule;
        break;
    }
    return TRUE;
}
