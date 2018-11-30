#ifndef READINI_H
#define READINI_H

#define mTCHAR char

extern "C"
{
    typedef int (*INI_CALLBACK)(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData);

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
    int ini_browse(INI_CALLBACK Callback, void *UserData, const mTCHAR *Filename);

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
    int ini_gets(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *DefValue, mTCHAR *Buffer, int BufferSize, const mTCHAR *Filename);
}

#endif  /* READINI_H */
