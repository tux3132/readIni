/**
 *
 *  These routines are in part based on minIni by CompuPhase
 *
 */

#define ini_openread(filename,file)     ((*(file) = fopen((filename),"rb")) != NULL)
#define ini_close(file)                 (fclose(*(file)) == 0)
#define ini_read(buffer,size,file)      (fgets((buffer),(size),*(file)) != NULL)
#define ini_tell(file,pos)              (*(pos) = ftell(*(file)))
#define __T(s)    s
#define assert(e)

#include "readIni.h"

enum quote_option
{
    QUOTE_NONE,
    QUOTE_ENQUOTE,
    QUOTE_DEQUOTE,
};

static char *skipleading(const char *str)
{
    assert(str != NULL);
    while ('\0' < *str && *str <= ' ')
        str++;
    return (char *)str;
}

static char *skiptrailing(const char *str, const char *base)
{
    assert(str != NULL);
    assert(base != NULL);
    while (str > base && '\0' < *(str-1) && *(str-1) <= ' ')
        str--;
    return (char *)str;
}

static char *striptrailing(char *str)
{
    char *ptr = skiptrailing(strchr(str, '\0'), str);
    assert(ptr != NULL);
    *ptr = '\0';
    return str;
}

static char *ini_strncpy(char *dest, const char *source, size_t maxlen, enum quote_option option)
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

static char *cleanstring(char *string, enum quote_option *quotes)
{
    int isstring;
    char *ep;

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
    if (*string == '"' && (ep = strchr(string, '\0')) != NULL && *(ep - 1) == '"')
    {
        string++;
        *--ep = '\0';
        *quotes = QUOTE_DEQUOTE;  /* this is a string, so remove escaped characters */
    } /* if */
    return string;
}

static int getkeystring(FILE* *fp, const char *Section, const char *Key,
                        int idxSection, int idxKey, char *Buffer, int BufferSize,
                        long int *mark)
{
  char *sp, *ep;
  int len, idx;
  enum quote_option quotes;
  char LocalBuffer[512];

  assert(fp != NULL);
  /* Move through file 1 line at a time until a section is matched or EOF. If
   * parameter Section is NULL, only look at keys above the first section. If
   * idxSection is positive, copy the relevant section name.
   */
  len = (Section != NULL) ? (int)strlen(Section) : 0;
  if (len > 0 || idxSection >= 0) {
    assert(idxSection >= 0 || Section != NULL);
    idx = -1;
    do {
      if (!ini_read(LocalBuffer, 512, fp))
        return 0;
      sp = skipleading(LocalBuffer);
      ep = strrchr(sp, ']');
    } while (*sp != '[' || ep == NULL ||
             (((int)(ep-sp-1) != len || Section == NULL || strncasecmp(sp+1,Section,len) != 0) && ++idx != idxSection));
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
  len = (Key != NULL) ? (int)strlen(Key) : 0;
  idx = -1;
  do {
    if (mark != NULL)
      ini_tell(fp, mark);   /* optionally keep the mark to the start of the line */
    if (!ini_read(LocalBuffer,512,fp) || *(sp = skipleading(LocalBuffer)) == '[')
      return 0;
    sp = skipleading(LocalBuffer);
    ep = strchr(sp, '=');  /* Parse out the equal sign */
    if (ep == NULL)
      ep = strchr(sp, ':');
  } while (*sp == ';' || *sp == '#' || ep == NULL
           || ((len == 0 || (int)(skiptrailing(ep,sp)-sp) != len || strncasecmp(sp,Key,len) != 0) && ++idx != idxKey));
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
int ini_gets(const char *Section, const char *Key, const char *DefValue,
             char *Buffer, int BufferSize, const char *Filename)
{
  FILE* fp;
  int ok = 0;

  if (Buffer == NULL || BufferSize <= 0 || Key == NULL)
    return 0;
  if (ini_openread(Filename, &fp)) {
    ok = getkeystring(&fp, Section, Key, -1, -1, Buffer, BufferSize, NULL);
    (void)ini_close(&fp);
  } /* if */
  if (!ok)
    ini_strncpy(Buffer, (DefValue != NULL) ? DefValue : __T(""), BufferSize, QUOTE_NONE);
  return (int)strlen(Buffer);
}

