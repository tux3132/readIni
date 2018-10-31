/*
 *  These routines are in part based on minIni by CompuPhase
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "readIni.h"

#define INI_FILETYPE                    FILE*
#define INI_BUFFERSIZE                  512
#define INI_FILEPOS                     long int
#define ini_openread(filename,file)     ((*(file) = fopen((filename),"rb")) != NULL)
#define ini_close(file)                 (fclose(*(file)) == 0)
#define ini_read(buffer,size,file)      (fgets((buffer),(size),*(file)) != NULL)
#define ini_tell(file,pos)              (*(pos) = ftell(*(file)))
#define __T(s)    s
#define assert(e)
#define TCHAR     char
#define _tcschr   strchr
#define _tcslen   strlen
#define _tcsrchr  strrchr
#define _tcsnicmp strnicmp
#define strnicmp  strncasecmp

enum quote_option
{
    QUOTE_NONE,
    QUOTE_ENQUOTE,
    QUOTE_DEQUOTE,
};

static TCHAR *skipleading(const TCHAR *str)
{
    assert(str != NULL);
    while ('\0' < *str && *str <= ' ')
        str++;
    return (TCHAR *)str;
}

static TCHAR *skiptrailing(const TCHAR *str, const TCHAR *base)
{
    assert(str != NULL);
    assert(base != NULL);
    while (str > base && '\0' < *(str-1) && *(str-1) <= ' ')
        str--;
    return (TCHAR *)str;
}

static TCHAR *striptrailing(TCHAR *str)
{
    TCHAR *ptr = skiptrailing(_tcschr(str, '\0'), str);
    assert(ptr != NULL);
    *ptr = '\0';
    return str;
}

static TCHAR *ini_strncpy(TCHAR *dest, const TCHAR *source, size_t maxlen, enum quote_option option)
{
    size_t d, s;

    assert(maxlen>0);
    assert(source != NULL && dest != NULL);
    assert((dest < source || (dest == source && option != QUOTE_ENQUOTE)) || dest > source + strlen(source));
    if (option == QUOTE_ENQUOTE && maxlen < 3)
        option = QUOTE_NONE;  /* cannot store two quotes and a terminating zero in less than 3 characters */

    switch (option)
    {
    case QUOTE_NONE:
        for (d = 0; d < maxlen - 1 && source[d] != '\0'; d++)
            dest[d] = source[d];
        assert(d < maxlen);
        dest[d] = '\0';
        break;
    case QUOTE_ENQUOTE:
        d = 0;
        dest[d++] = '"';
        for (s = 0; source[s] != '\0' && d < maxlen - 2; s++, d++)
        {
            if (source[s] == '"')
            {
                if (d >= maxlen - 3)
                    break;  /* no space to store the escape character plus the one that follows it */
                dest[d++] = '\\';
            } /* if */
            dest[d] = source[s];
        } /* for */
        dest[d++] = '"';
        dest[d] = '\0';
        break;
    case QUOTE_DEQUOTE:
        for (d = s = 0; source[s] != '\0' && d < maxlen - 1; s++, d++)
        {
            if ((source[s] == '"' || source[s] == '\\') && source[s + 1] == '"')
                s++;
            dest[d] = source[s];
        } /* for */
        dest[d] = '\0';
        break;
    default:
        assert(0);
    } /* switch */

    return dest;
}

static TCHAR *cleanstring(TCHAR *string, enum quote_option *quotes)
{
    int isstring;
    TCHAR *ep;

    assert(string != NULL);
    assert(quotes != NULL);

    /* Remove a trailing comment */
    isstring = 0;
    for (ep = string; *ep != '\0' && ((*ep != ';' && *ep != '#') || isstring); ep++)
    {
        if (*ep == '"')
        {
            if (*(ep + 1) == '"')
                ep++;                 /* skip "" (both quotes) */
            else
                isstring = !isstring; /* single quote, toggle isstring */
        }
        else if (*ep == '\\' && *(ep + 1) == '"')
        {
            ep++;                   /* skip \" (both quotes */
        } /* if */
    } /* for */
    assert(ep != NULL && (*ep == '\0' || *ep == ';' || *ep == '#'));
    *ep = '\0';                 /* terminate at a comment */
    striptrailing(string);
    /* Remove double quotes surrounding a value */
    *quotes = QUOTE_NONE;
    if (*string == '"' && (ep = _tcschr(string, '\0')) != NULL && *(ep - 1) == '"')
    {
        string++;
        *--ep = '\0';
        *quotes = QUOTE_DEQUOTE;  /* this is a string, so remove escaped characters */
    } /* if */
    return string;
}

static int getkeystring(INI_FILETYPE *fp, const TCHAR *Section, const TCHAR *Key,
                        int idxSection, int idxKey, TCHAR *Buffer, int BufferSize,
                        INI_FILEPOS *mark)
{
  TCHAR *sp, *ep;
  int len, idx;
  enum quote_option quotes;
  TCHAR LocalBuffer[INI_BUFFERSIZE];

  assert(fp != NULL);
  /* Move through file 1 line at a time until a section is matched or EOF. If
   * parameter Section is NULL, only look at keys above the first section. If
   * idxSection is positive, copy the relevant section name.
   */
  len = (Section != NULL) ? (int)_tcslen(Section) : 0;
  if (len > 0 || idxSection >= 0) {
    assert(idxSection >= 0 || Section != NULL);
    idx = -1;
    do {
      if (!ini_read(LocalBuffer, INI_BUFFERSIZE, fp))
        return 0;
      sp = skipleading(LocalBuffer);
      ep = _tcsrchr(sp, ']');
    } while (*sp != '[' || ep == NULL ||
             (((int)(ep-sp-1) != len || Section == NULL || _tcsnicmp(sp+1,Section,len) != 0) && ++idx != idxSection));
    if (idxSection >= 0) {
      if (idx == idxSection) {
        assert(ep != NULL);
        assert(*ep == ']');
        *ep = '\0';
        ini_strncpy(Buffer, sp + 1, BufferSize, QUOTE_NONE);
        return 1;
      } /* if */
      return 0; /* no more section found */
    } /* if */
  } /* if */

  /* Now that the section has been found, find the entry.
   * Stop searching upon leaving the section's area.
   */
  assert(Key != NULL || idxKey >= 0);
  len = (Key != NULL) ? (int)_tcslen(Key) : 0;
  idx = -1;
  do {
    if (mark != NULL)
      ini_tell(fp, mark);   /* optionally keep the mark to the start of the line */
    if (!ini_read(LocalBuffer,INI_BUFFERSIZE,fp) || *(sp = skipleading(LocalBuffer)) == '[')
      return 0;
    sp = skipleading(LocalBuffer);
    ep = _tcschr(sp, '=');  /* Parse out the equal sign */
    if (ep == NULL)
      ep = _tcschr(sp, ':');
  } while (*sp == ';' || *sp == '#' || ep == NULL
           || ((len == 0 || (int)(skiptrailing(ep,sp)-sp) != len || _tcsnicmp(sp,Key,len) != 0) && ++idx != idxKey));
  if (idxKey >= 0) {
    if (idx == idxKey) {
      assert(ep != NULL);
      assert(*ep == '=' || *ep == ':');
      *ep = '\0';
      striptrailing(sp);
      ini_strncpy(Buffer, sp, BufferSize, QUOTE_NONE);
      return 1;
    } /* if */
    return 0;   /* no more key found (in this section) */
  } /* if */

  /* Copy up to BufferSize chars to buffer */
  assert(ep != NULL);
  assert(*ep == '=' || *ep == ':');
  sp = skipleading(ep + 1);
  sp = cleanstring(sp, &quotes);  /* Remove a trailing comment */
  ini_strncpy(Buffer, sp, BufferSize, quotes);
  return 1;
}



/** ini_browse()
 * \param Callback    a pointer to a function that will be called for every
 *                    setting in the INI file.
 * \param UserData    arbitrary data, which the function passes on the
 *                    \c Callback function
 * \param Filename    the name and full path of the .ini file to read from
 *
 * \return            1 on success, 0 on failure (INI file not found)
 *
 * \note              The \c Callback function must return 1 to continue
 *                    browsing through the INI file, or 0 to stop. Even when the
 *                    callback stops the browsing, this function will return 1
 *                    (for success).
 */
int  ini_browse(INI_CALLBACK Callback, void *UserData, const TCHAR *Filename)
{
    TCHAR LocalBuffer[INI_BUFFERSIZE];
    int lenSec, lenKey;
    enum quote_option quotes;
    INI_FILETYPE fp;

    if (Callback == NULL)
        return 0;
    if (!ini_openread(Filename, &fp))
        return 0;

    LocalBuffer[0] = '\0';   /* copy an empty section in the buffer */
    lenSec = (int)_tcslen(LocalBuffer) + 1;
    for ( ;; )
    {
        TCHAR *sp, *ep;
        if (!ini_read(LocalBuffer + lenSec, INI_BUFFERSIZE - lenSec, &fp))
            break;
        sp = skipleading(LocalBuffer + lenSec);
        /* ignore empty strings and comments */
        if (*sp == '\0' || *sp == ';' || *sp == '#')
            continue;
        /* see whether we reached a new section */
        ep = _tcsrchr(sp, ']');
        if (*sp == '[' && ep != NULL)
        {
            *ep = '\0';
            ini_strncpy(LocalBuffer, sp + 1, INI_BUFFERSIZE, QUOTE_NONE);
            lenSec = (int)_tcslen(LocalBuffer) + 1;
            continue;
        } /* if */
        /* not a new section, test for a key/value pair */
        ep = _tcschr(sp, '=');    /* test for the equal sign or colon */
        if (ep == NULL)
            ep = _tcschr(sp, ':');
        if (ep == NULL)
            continue;               /* invalid line, ignore */
        *ep++ = '\0';             /* split the key from the value */
        striptrailing(sp);
        ini_strncpy(LocalBuffer + lenSec, sp, INI_BUFFERSIZE - lenSec, QUOTE_NONE);
        lenKey = (int)_tcslen(LocalBuffer + lenSec) + 1;
        /* clean up the value */
        sp = skipleading(ep);
        sp = cleanstring(sp, &quotes);  /* Remove a trailing comment */
        ini_strncpy(LocalBuffer + lenSec + lenKey, sp, INI_BUFFERSIZE - lenSec - lenKey, quotes);
        /* call the callback */
        if (!Callback(LocalBuffer, LocalBuffer + lenSec, LocalBuffer + lenSec + lenKey, UserData))
            break;
    } /* for */

    (void)ini_close(&fp);
    return 1;
}


/** ini_gets()
 * \param Section     the name of the section to search for
 * \param Key         the name of the entry to find the value of
 * \param DefValue    default string in the event of a failed read
 * \param Buffer      a pointer to the buffer to copy into
 * \param BufferSize  the maximum number of characters to copy
 * \param Filename    the name and full path of the .ini file to read from
 *
 * \return            the number of characters copied into the supplied buffer
 */
int ini_gets(const TCHAR *Section, const TCHAR *Key, const TCHAR *DefValue,
             TCHAR *Buffer, int BufferSize, const TCHAR *Filename)
{
  INI_FILETYPE fp;
  int ok = 0;

  if (Buffer == NULL || BufferSize <= 0 || Key == NULL)
    return 0;
  if (ini_openread(Filename, &fp)) {
    ok = getkeystring(&fp, Section, Key, -1, -1, Buffer, BufferSize, NULL);
    (void)ini_close(&fp);
  } /* if */
  if (!ok)
    ini_strncpy(Buffer, (DefValue != NULL) ? DefValue : __T(""), BufferSize, QUOTE_NONE);
  return (int)_tcslen(Buffer);
}

