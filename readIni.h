#ifndef READINI_H
#define READINI_H
#define mTCHAR char

extern "C"
{
    typedef int (*INI_CALLBACK)(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData);
    int ini_browse(INI_CALLBACK Callback, void *UserData, const mTCHAR *Filename);
    int ini_gets(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *DefValue, mTCHAR *Buffer, int BufferSize, const mTCHAR *Filename);
}

#endif
