#ifndef _AUDIO_PARAMS_CONFIG_H_
#define _AUDIO_PARAMS_CONFIG_H_

/* AENC TX EQ 参数 */
#define CFG_AENC_TXEQPRM                                                 \
    0,                                                                   \
    {                                                                    \
        {PHONE_EQ_BIQUAD_LOWSHELVING, 450, -3, 0.707},                   \
        {PHONE_EQ_BIQUAD_HIGHSHELVING, 300, -3, 0.707},                  \
        {0,},                                                            \
        {0,},                                                            \
        {0,},                                                            \
    }

/* TX AGC 参数 */
#define CFG_AENC_TXAGCPRM                                                \
    .func_mask = /*PHONE_AGC_MASK_LIMITER |*/ PHONE_AGC_MASK_NOISEGATE,  \
    .lev_mode = 0,                                                       \
    .look_ahead = 0,                                                     \
    .lt_thd = -18,                                                       \
    .lt_ratio = 3,                                                       \
    .lt_kw = 20,                                                         \
    .lt_att = 1,                                                         \
    .lt_rel = 800,                                                       \
    .ng_thd = -45,                                                       \
    .ng_ratio = 5,                                                       \
    .ng_kw = 20,                                                         \
    .ng_hold = 25,                                                       \
    .ng_att = 50,                                                        \
    .ng_rel = 150,                                                       \
    .peak_att = 1,                                                       \
    .peak_rel = 30,                                                      \
    .makeup_gain = 0,

/* RX EQ 参数 */
#define CFG_AENC_RXEQPRM                                                 \
    0,                                                                   \
    {                                                                    \
        {PHONE_EQ_BIQUAD_LOWSHELVING, 450,  3, 0.707},                   \
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 900,  -0.5, 1.0},                    \
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 2650, 0.3, 2.0},                     \
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 3400,  -5,  2.7},                    \
        {PHONE_EQ_BIQUAD_LOWSHELVING, 200,  3,  0.707},                  \
    }

/* RX nslite */
#define CFG_AENC_RX_NSLITE_ENABLE    1
#define CFG_NSLITE_COFNIG_PARAMS                                                  \
    .R                  = 2,                                                      \
    .func_mask          = (/*NSLITE_MASK_ANS  |*/ NSLITE_MASK_GSM  | NSLITE_MASK_CNG),\
    .finit              = 1,                                                      \
    .pf_Xi_opt          = 15,                                                     \
    .pf_ph1_thd         = (float).99,                                             \
    .pf_alpha_npsd      = 1 - (float)0.8,                                         \
    .pf_alpha_PH1       = 1 - (float)0.9,                                         \
    .pf_alpha_Xis       = 1 - (float)0.99,                                        \
    .pf_alpha_Nmin_f    = (float)0.3,                                             \
    .pf_inc_Nmin_t      = (float)0.001,                                           \
    .pf_GHmin           = -15,                                                    \
    .pf_Gcn             = -10,                                                    \
    .Npsd_bias          = (float).9,                                              \
    .gain_smooth_thd    = (float)0.4,                                             \
    .gain_smooth_width  = (float)5,                                               \
    .SNR_s_Min          = (float)-20,                                             \
    .psd_Min            = -80,

/* RX AGC 参数 */
#define CFG_AENC_RXAGCPRM                                                \
    .func_mask = PHONE_AGC_MASK_LIMITER | PHONE_AGC_MASK_NOISEGATE,      \
    .lev_mode = 0,                                                       \
    .look_ahead = 0,                                                     \
    .lt_thd = -9,                                                        \
    .lt_ratio = 2,                                                       \
    .lt_kw = 15,                                                         \
    .lt_att = 1,                                                         \
    .lt_rel = 800,                                                       \
    .ng_thd = -50,                                                       \
    .ng_ratio = 5,                                                       \
    .ng_kw = 20,                                                         \
    .ng_hold = 25,                                                       \
    .ng_att = 50,                                                        \
    .ng_rel = 150,                                                       \
    .peak_att = 1,                                                       \
    .peak_rel = 30,                                                      \
    .makeup_gain = 0,

/* 基本通话降噪算法参数 */
#define CFG_AENC_CONFIG_PARAMS                                           \
    .func_mask = (/*PHONE_AEC | */PHONE_AES | PHONE_NS /*| PHONE_SMSNR*/ | \
                  PHONE_TXAGC | PHONE_RXAGC |                            \
                  PHONE_CNG | PHONE_GSMOOTH |                            \
                  PHONE_TXEQ | PHONE_RXEQ),                              \
    .rse_thd        =  0,           /* 0 ~ 10 */                         \
    .ref_thd        = 250,          /* 0 ~ 255 */                        \
    .alpha_slow     = (float)0.3,   /* 0 ~ 1 */                          \
    .alpha_fast     = (float)0.5,   /* 0 ~ 1 */                          \
    .pf_GHmin       = (float)-20,   /* -30 ~ 0 (dB) */                   \
    .res_overest    = (float)0,     /* -10 ~ 10 (dB) */                  \
    .rse_maxG       = (float)12,   /* -30 ~ 30 (dB) */                   \
    .dt_thd_norm    = (float)0.92,  /* 0 ~ 1 */                          \
    .pf_Xi_opt      = (float)9,     /* 0 ~ 20 */                         \
    .Npsd_bias      = (float).9,    /* 0 ~ 2 */                          \
    .pf_Gcn         = (float)-15,   /* -30 ~ 0 (dB) */                   \
    .avq_reftx      = (float)-27,                                        \
    .avq_refrx      = (float)-18,                                        \
    .avq_vadthd     = (float)-40,   /* -60 ~ 0 (dBFs) */                 \
    .avq_offset     = (float)9,     /* 0 ~ 10 (dB) */                    \
    .avq_Gmax       = (float)10,    /* 0 ~ 15 (dB) */

/* 其他通话降噪算法参数 */
#define CFG_AENC_PARAMS                                                  \
    .gain_smooth_thd    = (float)0.4,                                    \
    .gain_smooth_width  = (float)5,                                      \
    .SNR_s_Min          = (float)-20,                                    \
    .SNR_ns_Min         = (float)-60,                                    \
    .psd_Min            = (float)-100,                                   \
    .pf_alpha_Xis       = 1-(float)0.99,                                 \
    .pf_alpha_Xins      = 1-(float)0.95,                                 \
                                                                         \
    .alpha_fast_resG    = (float)0.5,                                    \
    .alpha_slow_resG    = (float)0.0,                                    \
    .dt_thd_limit       = (float)0.15,                                   \
    .dt_init_thd        = (float)0.5,                                    \
    .dt_steprise_decvar = (float)0.0145,                                 \
    .dt_stepfall_decvar = (float)8./30000,                               \
                                                                         \
    .finit              = (float)40,                                     \
    .pf_ph1_thd         = (float).995,                                   \
    .pf_alpha_npsd      = 1-(float)0.8,                                  \
    .pf_alpha_PH1       = 1-(float)0.9,                                  \
                                                                         \
    .pf_alpha_Nmin_f    = (float)0.1,                                    \
    .pf_inc_Nmin_t      = (float)0.001,                                  \
                                                                         \
    .pf_alpha_z         = 1-(float)0.95,                                 \
    .pf_decay           = (float)-8,                                     \
    .decay_lthd         = (float)-3.2,                                   \
    .alpha_decay        = (float)0.4,                                    \
    .decay_km           = (float)0.001,                                  \
    .rise_km            = (float)0.4,                                    \
    .thd_km             = (float)-40,                                    \
                                                                         \
    .avq_dec_step       = (float)5,                                      \
    .avq_sf_norm        = (float)-30,                                    \
    .avq_alpha_fref     = (float)0.03,                                   \
    .avq_alpha_sref     = (float)0.01,                                   \
    .avq_alpha_fn       = (float)0.15,                                   \
    .avq_alpha_sn       = (float)0.05,                                   \
    .dt_overest         = (float)0,                                      \
    .init_overest       = (float)18,

/* 基本双麦通话降噪算法参数 */
#define CFG_ADMAENC_CONFIG_PARAMS                                                                               \
    .func_mask = (/*PHONE_AEC |*/ PHONE_AES | PHONE_NS /*| PHONE_SMSNR*/ | PHONE_TXAGC | /*PHONE_RXAGC |*/ PHONE_TXEQ | PHONE_RXEQ    \
                  | PHONE_CNG | PHONE_GSMOOTH /*| PHONE_DMCAB*/ | PHONE_DMANS | PHONE_DMWNS | PHONE_DMADP),     \
    .rse_thd        =  0,           /* 0 ~ 10 */                                                                \
    .ref_thd        = 125,          /* 0 ~ 255 */                                                               \
    .alpha_slow     = (float)0.3,   /* 0 ~ 1 */                                                                 \
    .alpha_fast     = (float)0.5,   /* 0 ~ 1 */                                                                 \
    .pf_GHmin       = (float)-30,   /* -30 ~ 0 (dB) */                                                          \
    .res_overest    = (float)0,     /* -10 ~ 10 (dB) */                                                         \
    .rse_maxG       = (float)-6,    /* -30 ~ 30 (dB) */                                                         \
    .dt_thd_norm    = (float)0.92,  /* 0 ~ 1 */                                                                 \
    .pf_Xi_opt      = (float)9,     /* 0 ~ 20 */                                                                \
    .Npsd_bias      = (float).9,    /* 0 ~ 2 */                                                                 \
    .pf_Gcn         = (float)-25,   /* -30 ~ 0 (dB) */                                                          \
    .cab_thd        = (float)3,     /*calibration threshold */

#define CFG_ADMAENC_PARAMS                                               \
   /* initial ambient noise recursive estimation duration in frames */   \
    .finit          = 50,                                                \
    /* prms for calibration */                                           \
    /* step size for calibration filter */                               \
    .uw             =   (float)0.003,                                    \
    /* step size for adaptive beamforming filter */                      \
    .u              =   (float)0.003,                                    \
    /* vad thd in dB */                                                  \
    .vad_thd        =   (float)0,                                        \
    /* wind onset thd */                                                 \
    .wind_thd       =   (float)-0.5,                                     \
    /* smoothing prm for reference psd calculation */                    \
    .alpha_xpsd     =   (float)0.04,                                     \
    /* calibration low freq */                                           \
    .cflow          =   50,                                              \
    /* calibration high freq */                                          \
    .cfhigh         =   4000,                                            \
   /* wind noise detection freq region low */                            \
    .wflow          =   200,                                             \
    /* wind noise detection freq region high */                          \
    .wfhigh         =   1000,                                            \
    /* slow varied calibrated weight low pass cutoff freq */             \
    .lpfws          =   2,                                               \
    /* fast varied calibrated weight low pass cutoff freq */             \
    .lpfwf          =   20,                                              \
    /* prms for ANS */                                                   \
                                                                         \
    /* maximum of allowable SPP */                                       \
    .pf_ph1_thd     =   (float)0.995,                                    \
    /* ambient noise psd smoothing prm*/                                 \
    .pf_alpha_npsd  =   1 - (float)0.8,                                  \
    /* SPP smoothing prm */                                              \
    .pf_alpha_PH1   =   1 - (float)0.9,                                  \
    /* SNR smoothing prm */                                              \
    .pf_alpha_Xis   =   1 - (float)0.99,                                 \
                                                                         \
    /* minimum SNR for signal to ambient noise */                        \
    .SNR_s_Min      =   (float)-30,                                      \
    /* allowable mimum psd */                                            \
    .psd_Min        =   (float)-100,                                     \
                                                                         \
    /* Wind Noise prms */                                                \
    /* recursive wiener filter prm a */                                  \
    .Rwa            =   (float)1.0,                                      \
    /* recursive wiener filter prm c */                                  \
    .Rwc            =   (float)0.75,                                     \
                                                                         \
     /* decay time after onset false */                                  \
    .N_decay        =   10,                                              \
    /* hold time after onset true */                                     \
    .N_hold         =   2,                                               \
                                                                         \
    /* CNG module */                                                     \
    /* CNG smoothing prm for frequency domain */                         \
    .pf_alpha_Nmin_f    =   (float)0.7,                                  \
    /* CNG increment step */                                             \
    .pf_inc_Nmin_t      =   (float)0.001,                                \
                                                                         \
    /*gain smoothing module */                                           \
    /* up limit for snr to use gain smoothing */                         \
    .gain_smooth_thd    =   (float)0.1,                                  \
    /* gain smoothing width in frequency bin num */                      \
    .gain_smooth_width  =   5,                                           \
    .alpha_fast_resG    =   (float)0.5,                                  \
    .alpha_slow_resG    =   (float)0,                                    \
                                                                         \
    .dt_thd_limit       =   (float)0.15,                                 \
    .dt_init_thd        =   (float)0.5,                                  \
    .dt_steprise_decvar =   (float)0.0145,                               \
    .dt_stepfall_decvar =   (float)8./30000,                             \
    /* non-stationary SNR low bound */                                   \
    .SNR_ns_Min         =   (float)-60,                                  \
    .alpha_Xins         =   1-(float)0.95,                               \
    .dt_overest         =   (float)0,                                    \
    .init_overest       =   (float)0,                                    \
    .vadinc_time        =   10,                                          \
    .vaddec_time        =   100,                                         \
    .vadNmin            =   -80,                                         \
    .vadVmin            =   -50,

#define CFG_ANC_MODE_PARAMS \
{ \
    /* FF */    \
    {ANC_REAL_TO_COEFF(-9.061722679770e-01), ANC_REAL_TO_COEFF(+1.804346183103e+00), ANC_REAL_TO_COEFF(-8.981841331802e-01), ANC_REAL_TO_COEFF(+1.991053989389e+00), ANC_REAL_TO_COEFF(-9.910616362041e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.112091038091e+00), ANC_REAL_TO_COEFF(+1.086383910793e+00), ANC_REAL_TO_COEFF(-1.627163711934e+00), ANC_REAL_TO_COEFF(-7.265518377108e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-2.703979031624e-01), ANC_REAL_TO_COEFF(+5.763640930711e-01), ANC_REAL_TO_COEFF(+1.276477562866e+00), ANC_REAL_TO_COEFF(-7.518611624080e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-1.731037294037e+00), ANC_REAL_TO_COEFF(+8.497035397409e-01), ANC_REAL_TO_COEFF(+1.519882674982e+00), ANC_REAL_TO_COEFF(-7.889588610805e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+9.524185257325e-01), ANC_REAL_TO_COEFF(+5.327962311664e-01), ANC_REAL_TO_COEFF(-1.067508694334e+00), ANC_REAL_TO_COEFF(-8.175595661176e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.817027857305e+00), ANC_REAL_TO_COEFF(+1.099168393881e+00), ANC_REAL_TO_COEFF(-1.722358544457e+00), ANC_REAL_TO_COEFF(-8.574876078215e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+8.400063336350e-01), ANC_REAL_TO_COEFF(+7.581765188807e-01), ANC_REAL_TO_COEFF(-7.000533637983e-01), ANC_REAL_TO_COEFF(-8.851890812169e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.835325706412e-01), ANC_REAL_TO_COEFF(+1.098911326118e+00), ANC_REAL_TO_COEFF(-5.271526447050e-01), ANC_REAL_TO_COEFF(-9.200450954216e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.554624459905e+00), ANC_REAL_TO_COEFF(+8.707203686314e-01), ANC_REAL_TO_COEFF(-1.512648695706e+00), ANC_REAL_TO_COEFF(-9.252368342950e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-1.013784937982e+00), ANC_REAL_TO_COEFF(+8.588453792218e-01), ANC_REAL_TO_COEFF(+7.622966320731e-01), ANC_REAL_TO_COEFF(-9.365042148494e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-6.174319906417e-01), ANC_REAL_TO_COEFF(+8.640355900012e-01), ANC_REAL_TO_COEFF(+3.770823518817e-01), ANC_REAL_TO_COEFF(-9.390288198506e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-1.668131938257e+00), ANC_REAL_TO_COEFF(+7.815436087553e-01), ANC_REAL_TO_COEFF(+1.948806708742e+00), ANC_REAL_TO_COEFF(-9.514311772460e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.801492673228e+00), ANC_REAL_TO_COEFF(+9.053913390155e-01), ANC_REAL_TO_COEFF(-1.957860680353e+00), ANC_REAL_TO_COEFF(-9.734055176493e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-1.793152821430e+00), ANC_REAL_TO_COEFF(+8.936273575036e-01), ANC_REAL_TO_COEFF(+1.967402867297e+00), ANC_REAL_TO_COEFF(-9.748093092715e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(-1.214789380309e+00), ANC_REAL_TO_COEFF(+7.062222937105e-01), ANC_REAL_TO_COEFF(+1.575048226431e+00), ANC_REAL_TO_COEFF(-9.748735921087e-01)},\
    {ANC_REAL_TO_COEFF(+9.061722679770e-01), ANC_REAL_TO_COEFF(+1.572704374724e-01), ANC_REAL_TO_COEFF(+7.721596143829e-01), ANC_REAL_TO_COEFF(-1.436396396504e-01), ANC_REAL_TO_COEFF(-9.790125395216e-01)},\
                                                                                                              \
    /* FB */                                                                                                  \
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(-1.412850256965e+00), ANC_REAL_TO_COEFF(+6.836192878584e-01), ANC_REAL_TO_COEFF(-1.300230523378e+00), ANC_REAL_TO_COEFF(-4.035009636577e-01)},\
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(+1.061329909877e+00), ANC_REAL_TO_COEFF(+4.707798898248e-01), ANC_REAL_TO_COEFF(-8.534124075438e-01), ANC_REAL_TO_COEFF(-3.720565989197e-01)},\
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(+8.955369013101e-01), ANC_REAL_TO_COEFF(+1.086649318239e+00), ANC_REAL_TO_COEFF(-7.306885047394e-01), ANC_REAL_TO_COEFF(-8.594267602529e-01)},\
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(-5.583843985590e-02), ANC_REAL_TO_COEFF(+4.083142389806e-01), ANC_REAL_TO_COEFF(+1.879817500378e+00), ANC_REAL_TO_COEFF(-8.839212714607e-01)},\
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(-1.430811541006e+00), ANC_REAL_TO_COEFF(+7.188029679430e-01), ANC_REAL_TO_COEFF(+1.929388936518e+00), ANC_REAL_TO_COEFF(-9.558845439649e-01)},\
    {ANC_REAL_TO_COEFF(+7.292885445391e-01), ANC_REAL_TO_COEFF(-1.424143918210e+00), ANC_REAL_TO_COEFF(+7.030096489627e-01), ANC_REAL_TO_COEFF(+1.993182336001e+00), ANC_REAL_TO_COEFF(-9.931991934325e-01)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    \
    /* COMP */ \
    {ANC_REAL_TO_COEFF(-7.967722457930e-01), ANC_REAL_TO_COEFF(+2.073858583666e+00), ANC_REAL_TO_COEFF(-1.277353027819e+00), ANC_REAL_TO_COEFF(+1.979188630663e+00), ANC_REAL_TO_COEFF(-9.792482080766e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-2.936669405841e+00), ANC_REAL_TO_COEFF(+3.073866622825e+00), ANC_REAL_TO_COEFF(+1.439851816316e+00), ANC_REAL_TO_COEFF(-5.850629606463e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+4.029514353154e-01), ANC_REAL_TO_COEFF(+7.737375800819e-01), ANC_REAL_TO_COEFF(-6.649136691083e-01), ANC_REAL_TO_COEFF(-6.980737590120e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+1.477953384538e+00), ANC_REAL_TO_COEFF(+8.743207643085e-01), ANC_REAL_TO_COEFF(-1.725035075076e+00), ANC_REAL_TO_COEFF(-7.579626915925e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.475712714369e+00), ANC_REAL_TO_COEFF(+7.602327185490e-01), ANC_REAL_TO_COEFF(+1.811611198103e+00), ANC_REAL_TO_COEFF(-8.358102929408e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.190325907488e+00), ANC_REAL_TO_COEFF(+8.513521670916e-01), ANC_REAL_TO_COEFF(+1.379395417536e+00), ANC_REAL_TO_COEFF(-8.636575954363e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-4.826343047725e-01), ANC_REAL_TO_COEFF(+1.130721574440e+00), ANC_REAL_TO_COEFF(+5.127422263716e-01), ANC_REAL_TO_COEFF(-9.300302730799e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+1.250561668900e+00), ANC_REAL_TO_COEFF(+6.553088536174e-01), ANC_REAL_TO_COEFF(-1.571861367708e+00), ANC_REAL_TO_COEFF(-9.321989422298e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+1.584019384344e+00), ANC_REAL_TO_COEFF(+8.051421357002e-01), ANC_REAL_TO_COEFF(-1.855242970593e+00), ANC_REAL_TO_COEFF(-9.355645596426e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.573165620567e+00), ANC_REAL_TO_COEFF(+7.775660445308e-01), ANC_REAL_TO_COEFF(+1.914239625282e+00), ANC_REAL_TO_COEFF(-9.381290445172e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.523476533821e+00), ANC_REAL_TO_COEFF(+7.609077377418e-01), ANC_REAL_TO_COEFF(+1.881451391304e+00), ANC_REAL_TO_COEFF(-9.460013431483e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+1.186391131965e+00), ANC_REAL_TO_COEFF(+7.892466157568e-01), ANC_REAL_TO_COEFF(-1.320351970784e+00), ANC_REAL_TO_COEFF(-9.623272373447e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+7.116757104349e-01), ANC_REAL_TO_COEFF(+6.926242835044e-01), ANC_REAL_TO_COEFF(-9.404400470924e-01), ANC_REAL_TO_COEFF(-9.629239146839e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.355674873814e+00), ANC_REAL_TO_COEFF(+7.858131756132e-01), ANC_REAL_TO_COEFF(+1.704204098830e+00), ANC_REAL_TO_COEFF(-9.873718974703e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(+1.446823886526e-01), ANC_REAL_TO_COEFF(+8.370457516692e-01), ANC_REAL_TO_COEFF(-2.105063196745e-01), ANC_REAL_TO_COEFF(-9.917189769183e-01)},\
    {ANC_REAL_TO_COEFF(+7.967722457930e-01), ANC_REAL_TO_COEFF(-1.279310704740e+00), ANC_REAL_TO_COEFF(+7.938385205417e-01), ANC_REAL_TO_COEFF(+1.606649926364e+00), ANC_REAL_TO_COEFF(-9.975257073425e-01)},\
}

#define CFG_HT_MODE_PARAMS \
{ \
    /* FF */ \
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+3.018482695324e+00), ANC_REAL_TO_COEFF(+8.620711811533e-01), ANC_REAL_TO_COEFF(+1.672738182527e-01), ANC_REAL_TO_COEFF(+5.446958115632e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.947236009113e-01), ANC_REAL_TO_COEFF(-5.929973932959e-01), ANC_REAL_TO_COEFF(+1.103923670334e-01), ANC_REAL_TO_COEFF(+8.889795283618e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.155766850865e+00), ANC_REAL_TO_COEFF(+1.022859618074e+00), ANC_REAL_TO_COEFF(+5.157853862518e-01), ANC_REAL_TO_COEFF(-2.915398658001e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-5.705017901308e-01), ANC_REAL_TO_COEFF(+7.436836204086e-01), ANC_REAL_TO_COEFF(+9.962408256840e-01), ANC_REAL_TO_COEFF(-6.236604893031e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-3.204574727675e-01), ANC_REAL_TO_COEFF(+5.085942777295e-01), ANC_REAL_TO_COEFF(+4.639601100694e-01), ANC_REAL_TO_COEFF(-7.208702815073e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+6.900223723577e-02), ANC_REAL_TO_COEFF(+8.318340092600e-01), ANC_REAL_TO_COEFF(-2.621581932545e-02), ANC_REAL_TO_COEFF(-7.307447598955e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+4.915708893660e-01), ANC_REAL_TO_COEFF(+9.760300150618e-01), ANC_REAL_TO_COEFF(-6.937378536556e-01), ANC_REAL_TO_COEFF(-7.808604011374e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+8.797818092171e-01), ANC_REAL_TO_COEFF(+4.537870076726e-01), ANC_REAL_TO_COEFF(-1.660750054987e+00), ANC_REAL_TO_COEFF(-8.091211219174e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+1.645643418372e+00), ANC_REAL_TO_COEFF(+8.847644867044e-01), ANC_REAL_TO_COEFF(-1.749296319247e+00), ANC_REAL_TO_COEFF(-8.094020671343e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-2.631103013409e-01), ANC_REAL_TO_COEFF(+8.733092685015e-01), ANC_REAL_TO_COEFF(+1.607948102268e-01), ANC_REAL_TO_COEFF(-8.137368422174e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.544438661697e+00), ANC_REAL_TO_COEFF(+7.625649144818e-01), ANC_REAL_TO_COEFF(+1.909241391073e+00), ANC_REAL_TO_COEFF(-9.154073685698e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.543464750872e+00), ANC_REAL_TO_COEFF(+7.732995342493e-01), ANC_REAL_TO_COEFF(+1.926424218538e+00), ANC_REAL_TO_COEFF(-9.466618155634e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.562450544509e+00), ANC_REAL_TO_COEFF(+7.752676639778e-01), ANC_REAL_TO_COEFF(+1.963646998103e+00), ANC_REAL_TO_COEFF(-9.641404591251e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(-1.530561022776e+00), ANC_REAL_TO_COEFF(+7.758406970640e-01), ANC_REAL_TO_COEFF(+1.925753967753e+00), ANC_REAL_TO_COEFF(-9.654152083786e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+1.165753910212e+00), ANC_REAL_TO_COEFF(+1.127887612079e+00), ANC_REAL_TO_COEFF(-1.128728048662e+00), ANC_REAL_TO_COEFF(-9.781241303883e-01)},\
    {ANC_REAL_TO_COEFF(+7.872719730433e-01), ANC_REAL_TO_COEFF(+1.441489108702e+00), ANC_REAL_TO_COEFF(+8.495039920223e-01), ANC_REAL_TO_COEFF(-1.498413256161e+00), ANC_REAL_TO_COEFF(-9.812396193920e-01)},\
    \
    /* FB */ \
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(1),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    \
    /* COMP */  \
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
    {ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0),ANC_REAL_TO_COEFF(0)},\
}

//ANC模式默认的gain
#define DEFAULT_ANC_FF_GAIN             ((float)0.4955)
#define DEFAULT_ANC_FB_GAIN             ((float)-0.4955)
#define DEFAULT_ANC_COMP_GAIN           ((float)-2.0)
//通透模式默认的gain
#define DEFAULT_HT_FF_GAIN              ((float)0.4955)
#define DEFAULT_HT_FB_GAIN              ((float)0.0)
#define DEFAULT_HT_COMP_GAIN            ((float)0.0)

#define AUDIO_ANC_HT_FB_ENABLE          0 //通透是否开FB mic
#define AUDIO_ANC_AMIC_CIC_GAIN_CFG     2

//VAD参数
#define CFG_VAD_MIC_PGA_GAIN            4 //范围0~7: 0:0dB, 1:24dB, 2:27dB, ..., 7:42dB
#define CFG_VAD_LAMBDA_MIN            -60
#define CFG_VAD_VOICE_MIN             -45

/* 音乐音量映射曲线 */
#define BTMUSIC_VOL_MAP_CFG \
    {100, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:127 */\
    {90, -3 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:119 */\
    {85, -6 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:111 */\
    {80, -9 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:103 */\
    {75, -13 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:95 */\
    {65, -17 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:87 */\
    {60, -21 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:79 */\
    {55, -25 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:71 */\
    {50, -29 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:63 */\
    {40, -33 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:55 */\
    {35, -37.5 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},    /* bt_vol:47 */\
    {30, -42 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:39 */\
    {25, -47 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:31 */\
    {15, -52 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:23 */\
    {10, -59 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      /* bt_vol:15 */\
    {5, -66 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:7 */\
    {0, -73 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       /* bt_vol:0 */

/* 通话音量映射曲线 */
#define CALL_VOL_MAP_CFG \
    {100, 0 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:127 */\
    {90, -3 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:119 */\
    {85, -6 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:111 */\
    {80, -9 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:103 */\
    {75, -13 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:95 */\
    {65, -17 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:87 */\
    {60, -21 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:79 */\
    {55, -25 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:71 */\
    {50, -29 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:63 */\
    {40, -33 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:55 */\
    {35, -37.5 + AUDIO_CALL_MAX_VOLUME_DECIBELS},    /* bt_vol:47 */\
    {30, -42 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:39 */\
    {25, -47 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:31 */\
    {15, -52 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:23 */\
    {10, -59 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      /* bt_vol:15 */\
    {5, -66 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:7 */\
    {0, -73 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       /* bt_vol:0 */

#endif
