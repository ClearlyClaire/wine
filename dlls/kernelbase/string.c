/*
 * Copyright 2019 Nikolay Sivov for CodeWeavers
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
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winnls.h"
#include "shlwapi.h"

#include "wine/debug.h"
#include "wine/exception.h"
#include "wine/unicode.h"

WINE_DEFAULT_DEBUG_CHANNEL(string);

static WORD get_char_type(WCHAR ch)
{
    WORD type = 0;
    GetStringTypeW(CT_CTYPE1, &ch, 1, &type);
    return type;
}

DWORD WINAPI StrCmpCA(const char *str, const char *cmp)
{
    return lstrcmpA(str, cmp);
}

DWORD WINAPI StrCmpCW(const WCHAR *str, const WCHAR *cmp)
{
    return lstrcmpW(str, cmp);
}

DWORD WINAPI StrCmpICA(const char *str, const char *cmp)
{
    return lstrcmpiA(str, cmp);
}

DWORD WINAPI StrCmpICW(const WCHAR *str, const WCHAR *cmp)
{
    return lstrcmpiW(str, cmp);
}

DWORD WINAPI StrCmpNICA(const char *str, const char *cmp, DWORD len)
{
    return StrCmpNIA(str, cmp, len);
}

DWORD WINAPI StrCmpNICW(const WCHAR *str, const WCHAR *cmp, DWORD len)
{
    return StrCmpNIW(str, cmp, len);
}

BOOL WINAPI IsCharBlankW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_BLANK);
}

BOOL WINAPI IsCharCntrlW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_CNTRL);
}

BOOL WINAPI IsCharDigitW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_DIGIT);
}

BOOL WINAPI IsCharPunctW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_PUNCT);
}

BOOL WINAPI IsCharSpaceA(CHAR c)
{
    WORD type;
    return GetStringTypeA(GetSystemDefaultLCID(), CT_CTYPE1, &c, 1, &type) && (type & C1_SPACE);
}

BOOL WINAPI IsCharSpaceW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_SPACE);
}

BOOL WINAPI IsCharXDigitW(WCHAR wc)
{
    return !!(get_char_type(wc) & C1_XDIGIT);
}

BOOL WINAPI IsCharAlphaA(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharAlphaW(wch);
}

BOOL WINAPI IsCharAlphaW(WCHAR ch)
{
    return !!(get_char_type(ch) & C1_ALPHA);
}

BOOL WINAPI IsCharLowerA(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharLowerW(wch);
}

BOOL WINAPI IsCharLowerW(WCHAR ch)
{
    return !!(get_char_type(ch) & C1_LOWER);
}

BOOL WINAPI IsCharAlphaNumericA(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharAlphaNumericW(wch);
}

BOOL WINAPI IsCharAlphaNumericW(WCHAR ch)
{
    return !!(get_char_type(ch) & (C1_ALPHA | C1_DIGIT));
}

BOOL WINAPI IsCharUpperA(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharUpperW(wch);
}

BOOL WINAPI IsCharUpperW(WCHAR ch)
{
    return !!(get_char_type(ch) & C1_UPPER);
}

WCHAR * WINAPI StrChrW(const WCHAR *str, WCHAR ch)
{
    TRACE("%s, %#x\n", wine_dbgstr_w(str), ch);

    if (!str)
        return NULL;

    return strchrW(str, ch);
}

WCHAR * WINAPI StrChrIW(const WCHAR *str, WCHAR ch)
{
    TRACE("%s, %#x\n", wine_dbgstr_w(str), ch);

    if (!str)
        return NULL;

    ch = toupperW(ch);
    while (*str)
    {
        if (toupperW(*str) == ch)
            return (WCHAR *)str;
        str++;
    }
    str = NULL;

    return (WCHAR *)str;
}

WCHAR * WINAPI StrChrNW(const WCHAR *str, WCHAR ch, UINT max_len)
{
    TRACE("%s, %#x, %u\n", wine_dbgstr_wn(str, max_len), ch, max_len);

    if (!str)
        return NULL;

    while (*str && max_len-- > 0)
    {
        if (*str == ch)
            return (WCHAR *)str;
        str++;
    }

    return NULL;
}

char * WINAPI StrDupA(const char *str)
{
    unsigned int len;
    char *ret;

    TRACE("%s\n", wine_dbgstr_a(str));

    len = str ? strlen(str) + 1 : 1;
    ret = LocalAlloc(LMEM_FIXED, len);

    if (ret)
    {
        if (str)
            memcpy(ret, str, len);
        else
            *ret = '\0';
    }

    return ret;
}

WCHAR * WINAPI StrDupW(const WCHAR *str)
{
    unsigned int len;
    WCHAR *ret;

    TRACE("%s\n", wine_dbgstr_w(str));

    len = (str ? strlenW(str) + 1 : 1) * sizeof(WCHAR);
    ret = LocalAlloc(LMEM_FIXED, len);

    if (ret)
    {
        if (str)
            memcpy(ret, str, len);
        else
            *ret = '\0';
    }

    return ret;
}

BOOL WINAPI ChrCmpIW(WCHAR ch1, WCHAR ch2)
{
    return CompareStringW(GetThreadLocale(), NORM_IGNORECASE, &ch1, 1, &ch2, 1) - CSTR_EQUAL;
}

WCHAR * WINAPI StrStrW(const WCHAR *str, const WCHAR *search)
{
    TRACE("%s, %s\n", wine_dbgstr_w(str), wine_dbgstr_w(search));

    if (!str || !search || !*search)
        return NULL;

    return strstrW(str, search);
}

WCHAR * WINAPI StrStrNW(const WCHAR *str, const WCHAR *search, UINT max_len)
{
    unsigned int i, len;

    TRACE("%s, %s, %u\n", wine_dbgstr_w(str), wine_dbgstr_w(search), max_len);

    if (!str || !search || !*search || !max_len)
        return NULL;

    len = strlenW(search);

    for (i = max_len; *str && (i > 0); i--, str++)
    {
        if (!strncmpW(str, search, len))
            return (WCHAR *)str;
    }

    return NULL;
}

int WINAPI StrCmpNIA(const char *str, const char *cmp, int len)
{
    TRACE("%s, %s, %i\n", wine_dbgstr_a(str), wine_dbgstr_a(cmp), len);
    return CompareStringA(GetThreadLocale(), NORM_IGNORECASE, str, len, cmp, len) - CSTR_EQUAL;
}

WCHAR * WINAPI StrStrNIW(const WCHAR *str, const WCHAR *search, UINT max_len)
{
    unsigned int i, len;

    TRACE("%s, %s, %u\n", wine_dbgstr_w(str), wine_dbgstr_w(search), max_len);

    if (!str || !search || !*search || !max_len)
        return NULL;

    len = strlenW(search);

    for (i = max_len; *str && (i > 0); i--, str++)
    {
        if (!strncmpiW(str, search, len))
            return (WCHAR *)str;
    }

    return NULL;
}

int WINAPI StrCmpNW(const WCHAR *str, const WCHAR *comp, int len)
{
    TRACE("%s, %s, %i\n", wine_dbgstr_w(str), wine_dbgstr_w(comp), len);
    return CompareStringW(GetThreadLocale(), 0, str, len, comp, len) - CSTR_EQUAL;
}

int WINAPI StrCmpNIW(const WCHAR *str, const WCHAR *comp, int len)
{
    TRACE("%s, %s, %i\n", wine_dbgstr_w(str), wine_dbgstr_w(comp), len);
    return CompareStringW(GetThreadLocale(), NORM_IGNORECASE, str, len, comp, len) - CSTR_EQUAL;
}

int WINAPI StrCmpW(const WCHAR *str, const WCHAR *comp)
{
    TRACE("%s, %s\n", wine_dbgstr_w(str), wine_dbgstr_w(comp));
    return CompareStringW(GetThreadLocale(), 0, str, -1, comp, -1) - CSTR_EQUAL;
}

int WINAPI StrCmpIW(const WCHAR *str, const WCHAR *comp)
{
    TRACE("%s, %s\n", wine_dbgstr_w(str), wine_dbgstr_w(comp));
    return CompareStringW(GetThreadLocale(), NORM_IGNORECASE, str, -1, comp, -1) - CSTR_EQUAL;
}

WCHAR * WINAPI StrCpyNW(WCHAR *dst, const WCHAR *src, int count)
{
    const WCHAR *s = src;
    WCHAR *d = dst;

    TRACE("%p, %s, %i\n", dst, wine_dbgstr_w(src), count);

    if (s)
    {
        while ((count > 1) && *s)
        {
            count--;
            *d++ = *s++;
        }
    }
    if (count) *d = 0;

    return dst;
}

WCHAR * WINAPI StrStrIW(const WCHAR *str, const WCHAR *search)
{
    unsigned int len;
    const WCHAR *end;

    TRACE("%s, %s\n", wine_dbgstr_w(str), wine_dbgstr_w(search));

    if (!str || !search || !*search)
        return NULL;

    len = strlenW(search);
    end = str + strlenW(str);

    while (str + len <= end)
    {
        if (!StrCmpNIW(str, search, len))
            return (WCHAR *)str;
        str++;
    }

    return NULL;
}

int WINAPI StrSpnW(const WCHAR *str, const WCHAR *match)
{
    if (!str || !match) return 0;
    return strspnW(str, match);
}

WCHAR * WINAPI StrRChrW(const WCHAR *str, const WCHAR *end, WORD ch)
{
    WCHAR *ret = NULL;

    if (!str) return NULL;
    if (!end) end = str + strlenW(str);
    while (str < end)
    {
        if (*str == ch) ret = (WCHAR *)str;
        str++;
    }
    return ret;
}

WCHAR * WINAPI StrRChrIW(const WCHAR *str, const WCHAR *end, WORD ch)
{
    WCHAR *ret = NULL;

    if (!str) return NULL;
    if (!end) end = str + strlenW(str);
    while (str < end)
    {
        if (!ChrCmpIW(*str, ch)) ret = (WCHAR *)str;
        str++;
    }
    return ret;
}

WCHAR * WINAPI StrPBrkW(const WCHAR *str, const WCHAR *match)
{
    if (!str || !match) return NULL;
    return strpbrkW(str, match);
}

BOOL WINAPI StrTrimW(WCHAR *str, const WCHAR *trim)
{
    unsigned int len;
    WCHAR *ptr = str;
    BOOL ret = FALSE;

    TRACE("%s, %s\n", wine_dbgstr_w(str), wine_dbgstr_w(trim));

    if (!str || !*str)
        return FALSE;

    while (*ptr && StrChrW(trim, *ptr))
        ptr++;

    len = strlenW(ptr);

    if (ptr != str)
    {
        memmove(str, ptr, (len + 1) * sizeof(WCHAR));
        ret = TRUE;
    }

    if (len > 0)
    {
        ptr = str + len;
        while (StrChrW(trim, ptr[-1]))
            ptr--; /* Skip trailing matches */

        if (ptr != str + len)
        {
            *ptr = '\0';
            ret = TRUE;
        }
    }

    return ret;
}

BOOL WINAPI StrToInt64ExW(const WCHAR *str, DWORD flags, LONGLONG *ret)
{
    BOOL negative = FALSE;
    LONGLONG value = 0;

    TRACE("%s, %#x, %p\n", wine_dbgstr_w(str), flags, ret);

    if (!str || !ret)
        return FALSE;

    if (flags > STIF_SUPPORT_HEX)
        WARN("Unknown flags %#x.\n", flags);

    /* Skip leading space, '+', '-' */
    while (isspaceW(*str))
        str++;

    if (*str == '-')
    {
        negative = TRUE;
        str++;
    }
    else if (*str == '+')
        str++;

    if (flags & STIF_SUPPORT_HEX && *str == '0' && tolowerW(str[1]) == 'x')
    {
        /* Read hex number */
        str += 2;

        if (!isxdigitW(*str))
            return FALSE;

        while (isxdigitW(*str))
        {
            value *= 16;
            if (isdigitW(*str))
                value += (*str - '0');
            else
                value += 10 + (tolowerW(*str) - 'a');
            str++;
        }

        *ret = value;
        return TRUE;
    }

    /* Read decimal number */
    if (!isdigitW(*str))
        return FALSE;

    while (isdigitW(*str))
    {
        value *= 10;
        value += (*str - '0');
        str++;
    }

    *ret = negative ? -value : value;
    return TRUE;
}

BOOL WINAPI StrToIntExW(const WCHAR *str, DWORD flags, INT *ret)
{
    LONGLONG value;
    BOOL res;

    TRACE("%s, %#x, %p\n", wine_dbgstr_w(str), flags, ret);

    res = StrToInt64ExW(str, flags, &value);
    if (res) *ret = value;
    return res;
}

int WINAPI StrToIntW(const WCHAR *str)
{
    int value = 0;

    TRACE("%s\n", wine_dbgstr_w(str));

    if (!str)
        return 0;

    if (*str == '-' || isdigitW(*str))
        StrToIntExW(str, 0, &value);
    return value;
}

char * WINAPI StrCpyNXA(char *dst, const char *src, int len)
{
    TRACE("%p, %s, %i\n", dst, wine_dbgstr_a(src), len);

    if (dst && src && len > 0)
    {
        while ((len-- > 1) && *src)
            *dst++ = *src++;
        if (len >= 0)
            *dst = '\0';
    }

    return dst;
}

WCHAR * WINAPI StrCpyNXW(WCHAR *dst, const WCHAR *src, int len)
{
    TRACE("%p, %s, %i\n", dst, wine_dbgstr_w(src), len);

    if (dst && src && len > 0)
    {
        while ((len-- > 1) && *src)
            *dst++ = *src++;
        if (len >= 0)
            *dst = '\0';
    }

    return dst;
}

LPSTR WINAPI CharLowerA(char *str)
{
    if (IS_INTRESOURCE(str))
    {
        char ch = LOWORD(str);
        CharLowerBuffA( &ch, 1 );
        return (LPSTR)(UINT_PTR)(BYTE)ch;
    }

    __TRY
    {
        CharLowerBuffA( str, strlen(str) );
    }
    __EXCEPT_PAGE_FAULT
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return NULL;
    }
    __ENDTRY
    return str;
}

DWORD WINAPI CharLowerBuffA(char *str, DWORD len)
{
    DWORD lenW;
    WCHAR buffer[32];
    WCHAR *strW = buffer;

    if (!str) return 0; /* YES */

    lenW = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
    if (lenW > ARRAY_SIZE(buffer))
    {
        strW = HeapAlloc(GetProcessHeap(), 0, lenW * sizeof(WCHAR));
        if (!strW) return 0;
    }
    MultiByteToWideChar(CP_ACP, 0, str, len, strW, lenW);
    CharLowerBuffW(strW, lenW);
    len = WideCharToMultiByte(CP_ACP, 0, strW, lenW, str, len, NULL, NULL);
    if (strW != buffer) HeapFree(GetProcessHeap(), 0, strW);
    return len;
}

DWORD WINAPI CharLowerBuffW(WCHAR *str, DWORD len)
{
    if (!str) return 0; /* YES */
    return LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, str, len, str, len);
}

LPWSTR WINAPI CharLowerW(WCHAR *str)
{
    if (!IS_INTRESOURCE(str))
    {
        CharLowerBuffW(str, lstrlenW(str));
        return str;
    }
    else
    {
        WCHAR ch = LOWORD(str);
        CharLowerBuffW(&ch, 1);
        return (LPWSTR)(UINT_PTR)ch;
    }
}

LPSTR WINAPI CharNextA(const char *ptr)
{
    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByte( ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}

LPSTR WINAPI CharNextExA(WORD codepage, const char *ptr, DWORD flags)
{
    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByteEx( codepage, ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}

LPWSTR WINAPI CharNextW(const WCHAR *x)
{
    if (*x) x++;

    return (WCHAR *)x;
}

LPSTR WINAPI CharPrevA(const char *start, const char *ptr)
{
    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextA(start);
        if (next >= ptr) break;
        start = next;
    }
    return (LPSTR)start;
}

LPSTR WINAPI CharPrevExA(WORD codepage, const char *start, const char *ptr, DWORD flags)
{
    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextExA(codepage, start, flags);
        if (next >= ptr) break;
        start = next;
    }
    return (LPSTR)start;
}

LPWSTR WINAPI CharPrevW(const WCHAR *start, const WCHAR *x)
{
    if (x > start) return (LPWSTR)(x - 1);
    else return (LPWSTR)x;
}

LPSTR WINAPI CharUpperA(LPSTR str)
{
    if (IS_INTRESOURCE(str))
    {
        char ch = LOWORD(str);
        CharUpperBuffA(&ch, 1);
        return (LPSTR)(UINT_PTR)(BYTE)ch;
    }

    __TRY
    {
        CharUpperBuffA(str, strlen(str));
    }
    __EXCEPT_PAGE_FAULT
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    __ENDTRY
    return str;
}

DWORD WINAPI CharUpperBuffA(LPSTR str, DWORD len)
{
    DWORD lenW;
    WCHAR buffer[32];
    WCHAR *strW = buffer;

    if (!str) return 0; /* YES */

    lenW = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
    if (lenW > ARRAY_SIZE(buffer))
    {
        strW = HeapAlloc(GetProcessHeap(), 0, lenW * sizeof(WCHAR));
        if (!strW) return 0;
    }
    MultiByteToWideChar(CP_ACP, 0, str, len, strW, lenW);
    CharUpperBuffW(strW, lenW);
    len = WideCharToMultiByte(CP_ACP, 0, strW, lenW, str, len, NULL, NULL);
    if (strW != buffer) HeapFree(GetProcessHeap(), 0, strW);
    return len;
}

DWORD WINAPI CharUpperBuffW(WCHAR *str, DWORD len)
{
    if (!str) return 0; /* YES */
    return LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, str, len, str, len);
}

LPWSTR WINAPI CharUpperW(WCHAR *str)
{
    if (!IS_INTRESOURCE(str))
    {
        CharUpperBuffW(str, lstrlenW(str));
        return str;
    }
    else
    {
        WCHAR ch = LOWORD(str);
        CharUpperBuffW(&ch, 1);
        return (LPWSTR)(UINT_PTR)ch;
    }
}
