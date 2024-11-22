#ifndef _PREPROC_COMMON_EXPORT_H_
#define _PREPROC_COMMON_EXPORT_H_

#define SLTK_CMDBASE_PP_COMMON  0x1000
#define SLTK_CMDBASE_PP_AENC    0xa000
#define SLTK_CMDBASE_PP_AFC     0xa100
#define SLTK_CMDBASE_PP_NHS     0xa200
#define SLTK_CMDBASE_PP_FS      0xa300
#define SLTK_CMDBASE_PP_PLC     0xa400
#define SLTK_CMDBASE_PP_SMMW    0xa500
#define SLTK_CMDBASE_PP_ADMFD   0xa600
#define SLTK_CMDBASE_PP_ADMAENC 0xa700
#define SLTK_CMDBASE_PP_ADMOMINI    0xa800
#define SLTK_CMDBASE_PP_NSLITE 0xa900
#define SLTK_CMDBASE_PP_AENCBWE 0xaa00
#define SLTK_CMDBASE_PP_ADMOMNIAENC 0xab00
#define SLTK_CMDBASE_PP_HE  0xac00
#define SLTK_CMDBASE_PP_HPEMAFC 0xad00
enum {
    /* preproc common command */
    SLTK_CMD_PP_COMMON_RESET = SLTK_CMDBASE_PP_COMMON,
    SLTK_CMD_PP_COMMON_SETFUNC,
    SLTK_CMD_PP_COMMON_SETRTPRM,
    SLTK_CMD_PP_COMMON_FLUSHRTPRM,

    /* aenc specificial command */
    SLTK_CMD_PP_AENC_GETIR = SLTK_CMDBASE_PP_AENC,
    SLTK_CMD_PP_AENC_SETIR,
    SLTK_CMD_PP_AENC_SETRTPRM,
    SLTK_CMD_PP_AENC_SETRXAGCRTPRM,
    SLTK_CMD_PP_AENC_SETTXAGCRTPRM,
    SLTK_CMD_PP_AENC_RXGAIN,
    SLTK_CMD_PP_AENC_TXGAIN,
    SLTK_CMD_PP_AENC_SETRXEQ,
    SLTK_CMD_PP_AENC_SETTXEQ,
    SLTK_CMD_PP_AENC_SETMASK,
    SLTK_CMD_PP_AENC_SETASYNCWORK,
    SLTK_CMD_PP_AENC_SETASYNCWORK2,

    /* afc specificial command */
    SLTK_CMD_PP_AFC_GETIR = SLTK_CMDBASE_PP_AFC,
    SLTK_CMD_PP_AFC_SETIR,


    /* SMMW specificial command */
    SLTK_CMD_PP_SMMW_SETRTPRM = SLTK_CMDBASE_PP_SMMW,
    SLTK_CMD_PP_SMMW_SETRXAGCRTPRM,
    SLTK_CMD_PP_SMMW_SETTXAGCRTPRM,

    /* ADMFD specificial command */
    SLTK_CMD_PP_ADMFD_SETRTPRM = SLTK_CMDBASE_PP_ADMFD,
    SLTK_CMD_PP_ADMFD_SETAGCRTPRM,
    SLTK_CMD_PP_ADMFD_SETEQRTPRM,

    /* ADMAENC specificial command */
    SLTK_CMD_PP_ADMAENC_SETRTPRM = SLTK_CMDBASE_PP_ADMAENC,
    SLTK_CMD_PP_ADMAENC_SETRXAGCRTPRM,
    SLTK_CMD_PP_ADMAENC_SETTXAGCRTPRM,
    SLTK_CMD_PP_ADMAENC_GETFIR,
    SLTK_CMD_PP_ADMAENC_GETRIR,
    SLTK_CMD_PP_ADMAENC_SETFIR,
    SLTK_CMD_PP_ADMAENC_SETRIR,
    SLTK_CMD_PP_ADMAENC_SETTXEQ,
    SLTK_CMD_PP_ADMAENC_SETRXEQ,
    SLTK_CMD_PP_ADMAENC_SETADMMASK,
    SLTK_CMD_PP_ADMAENC_SETAENCMASK,
    SLTK_CMD_PP_ADMAENC_SETASYNCWORK,
    SLTK_CMD_PP_ADMAENC_SETASYNCWORK2,

    /* ADMOMINI specificial command */
    SLTK_CMD_PP_ADMOMINI_SETRTPRM = SLTK_CMDBASE_PP_ADMOMINI,
    SLTK_CMD_PP_ADMOMINI_SETAGCRTPRM,
    SLTK_CMD_PP_ADMOMINI_SETEQRTPRM,

    SLTK_CMD_PP_NSLITE_SETRTPRM = SLTK_CMDBASE_PP_NSLITE,
    SLTK_CMD_PP_NSLITE_SETRXAGCRTPRM,
    SLTK_CMD_PP_NSLITE_SETTXAGCRTPRM,
    SLTK_CMD_PP_NSLITE_SETANS,

    SLTK_CMD_PP_AENCBWE_SETRTPRM = SLTK_CMDBASE_PP_AENCBWE,
    SLTK_CMD_PP_AENCBWE_SETRXAGCRTPRM,
    SLTK_CMD_PP_AENCBWE_SETTXAGCRTPRM,
    SLTK_CMD_PP_AENCBWE_GETFIR,
    SLTK_CMD_PP_AENCBWE_GETRIR,
    SLTK_CMD_PP_AENCBWE_SETFIR,
    SLTK_CMD_PP_AENCBWE_SETRIR,
    SLTK_CMD_PP_AENCBWE_SETTXEQ,
    SLTK_CMD_PP_AENCBWE_SETRXEQ,

    SLTK_CMD_PP_ADMOMNIAENC_SETRTPRM = SLTK_CMDBASE_PP_ADMOMNIAENC,
    SLTK_CMD_PP_ADMOMNIAENC_SETRXAGCRTPRM,
    SLTK_CMD_PP_ADMOMNIAENC_SETTXAGCRTPRM,
    SLTK_CMD_PP_ADMOMNIAENC_GETFIR,
    SLTK_CMD_PP_ADMOMNIAENC_GETRIR,
    SLTK_CMD_PP_ADMOMNIAENC_SETFIR,
    SLTK_CMD_PP_ADMOMNIAENC_SETRIR,
    SLTK_CMD_PP_ADMOMNIAENC_SETTXEQ,
    SLTK_CMD_PP_ADMOMNIAENC_SETRXEQ,
    SLTK_CMD_PP_ADMOMNIAENC_SETADMMASK,
    SLTK_CMD_PP_ADMOMNIAENC_SETAENCMASK,

    SLTK_CMD_PP_HE_SETRTPRM = SLTK_CMDBASE_PP_HE,
    SLTK_CMD_PP_HE_SETDRC,
    SLTK_CMD_PP_HE_SETEQBASE,
    SLTK_CMD_PP_HE_SETEQ,
    SLTK_CMD_PP_HE_SETAPPPRM,
    SLTK_CMD_PP_HE_SETAFCPRM,
    SLTK_CMD_PP_HE_GETAFCIR,
    SLTK_CMD_PP_HE_SETNHSPRM,

    SLTK_CMD_PP_HPEMAFC_SETRTPRM = SLTK_CMDBASE_PP_HPEMAFC,
    SLTK_CMD_PP_HPEMAFC_GETIR,
    SLTK_CMD_PP_HPEMAFC_SETIR,
    SLTK_CMD_PP_HPEMAFC_SETFBSIMPRM,
};

#endif
