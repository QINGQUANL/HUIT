#ifndef   _RFDSS_H
#define   _RFDSS_H

#define __REG(x)           (*((volatile unsigned int  *)(x)))
#define EM_ADDR            0x40030000//32KB---[0x40030000~0x40038000)

#define  TX_GAIN_DIG_VALUE   -32

//#define   RX_GAIN_FORCE    //RX增益force为RX_GAIN_GEAR
#define   RX_GAIN_GEAR    26 // 0~26档

//#define   TX_GAIN_FORCE      //TX增益force为TX_GAIN_GEAR
#define   TX_GAIN_GEAR    18 // 0~25档

//#define  PLL_VALUE_FORCE     //force住PLL频率*&&*跑协议栈必须关闭此开关,且打开rfdss_init的1M中频功能*&&*
#define  PLL_VALUE        (2441-2400)  //频率force为X就是X-2401



//------------------------------------rf user define---------------------------------------
//#define    RF_CAL_MODE
//#define    TX_BT_IF_MODE_SEL

#define    XO_TRIM              0x4c   //read and write，110pf step
#define    FRE_OFFSET           0x7    //read only
#define    ADC_CMP_ICW          0x5    //read only
#define    PLL_MODE             0x0    //read only 0x0:hardware ctr,0x1 pll must cal,0x2 pll must table
//----------------------------------rf user define end-------------------------------------



/*---------------------RFDSS Register define---------------------*/
#define RFDSS_BASE			         0x40011000

#define RFDSS_CNTL_REG			     __REG(RFDSS_BASE + 0x00)
#define RFDSS_MEM_INTF           __REG(RFDSS_BASE + 0x04)
#define RFDSS_MEM_DATA           __REG(RFDSS_BASE + 0x08)
#define RFDSS_TXEM_CNTL          __REG(RFDSS_BASE + 0x0C)
#define RFDSS_RXEM_CNTL          __REG(RFDSS_BASE + 0x10)
#define RFDSS_RXEM_CURADDR       __REG(RFDSS_BASE + 0x14)
#define RFDSS_DCEST_CNTL0        __REG(RFDSS_BASE + 0x18)
#define RFDSS_DCEST_CNTL1        __REG(RFDSS_BASE + 0x1C)
#define RFDSS_ADC_RSTAUX_CNTL    __REG(RFDSS_BASE + 0x20)
#define RFDSS_TXLDO_CNTL         __REG(RFDSS_BASE + 0x24)
//#define RFDSS_RXLDO_CNTL         __REG(RFDSS_BASE + 0x28)
#define RFDSS_RFVCO_LDO_CNTL     __REG(RFDSS_BASE + 0x2C)

#define RFDSS_IREF_CNTL          __REG(RFDSS_BASE + 0x40)
#define RFDSS_ATEST_IQCAL_CNTL   __REG(RFDSS_BASE + 0x44)

#define RFDSS_RX_CNTL0           __REG(RFDSS_BASE + 0x60)
#define RFDSS_RX_CNTL1           __REG(RFDSS_BASE + 0x64)

#define RFDSS_RX_PARA            __REG(RFDSS_BASE + 0x80)
#define RFDSS_RXADC_CNTL         __REG(RFDSS_BASE + 0x84)
#define RFDSS_RXADC_CAL          __REG(RFDSS_BASE + 0x88)

#define RFDSS_TX_CNTL0           __REG(RFDSS_BASE + 0xA0)
#define RFDSS_TX_CNTL1           __REG(RFDSS_BASE + 0xA4)
#define RFDSS_TX_CNTL2           __REG(RFDSS_BASE + 0xA8)
#define RFDSS_TX_GAIN            __REG(RFDSS_BASE + 0xAC)

#define RFDSS_RFPLL_CNTL0        __REG(RFDSS_BASE + 0xB0)
#define RFDSS_RFPLL_CNTL1        __REG(RFDSS_BASE + 0xB4)
#define RFDSS_RFPLL_PARA0        __REG(RFDSS_BASE + 0xB8)
#define RFDSS_RFPLL_PARA1        __REG(RFDSS_BASE + 0xBC)
#define RFDSS_RFPLL_FREQ         __REG(RFDSS_BASE + 0xC0)
#define RFDSS_RFPLL_OUT0         __REG(RFDSS_BASE + 0xC4)
#define RFDSS_RFPLL_OUT1         __REG(RFDSS_BASE + 0xC8)
#define RFDSS_RFPLL_OUT2         __REG(RFDSS_BASE + 0xCC)

#define RFDSS_ADC_CAL_STATUE     __REG(RFDSS_BASE + 0xD0)
#define RFDSS_ADC_CAL_TH         __REG(RFDSS_BASE + 0xD4)
#define RFDSS_EM_ADDR_TH         __REG(RFDSS_BASE + 0xD8)
#define RFDSS_GAIN_SATUE         __REG(RFDSS_BASE + 0xDC)
#define RFDSS_PLL_SATUE          __REG(RFDSS_BASE + 0xE0)
//#define RFDSS_RF_VERSION         __REG(RFDSS_BASE + 0xE4)
#define RFDSS_PLL_FRE_OFFSET0    __REG(RFDSS_BASE + 0xE4)
#define RFDSS_PLL_FRE_OFFSET1    __REG(RFDSS_BASE + 0xE8)
#define RFDSS_TX_RF_COMP_CNTL0   __REG(RFDSS_BASE + 0xEC)
#define RFDSS_TX_RF_COMP_CNTL1   __REG(RFDSS_BASE + 0xF0)
#define RFDSS_TX_RF_COMP_CNTL2   __REG(RFDSS_BASE + 0xF4)
#define RFDSS_TX_RF_COMP_CNTL3   __REG(RFDSS_BASE + 0xF8)

enum rfd_mem_type {
	MEM_TXPPA_GAIN,
	MEM_TX_DC_Q_1M,
	MEM_TX_DC_I_1M,
	MEM_TX_DAC_GAIN,
	MEM_TX_DC_Q_2M,
	MEM_TX_DC_I_2M,
	MEM_TX_GAIN_DIG,
	MEM_TX_PA_I,
	MEM_TX_CQ_1M,
	MEM_TX_CE_1M,
	MEM_TX_CQ_2M,
	MEM_TX_CE_2M,
	MEM_RX_GAIN_LNA,
	MEM_RX_ABB_GAIN,
	MEM_RX_DC_Q,
	MEM_RX_DC_I,
	MEM_RX_LNA_G,
	MEM_RX_MIXER_BUFLP,
	MEM_RX_1M_ABB_LP0,
	MEM_RX_2M_ABB_LP0,
	MEM_RX_1M_ABB_LP,
	MEM_RX_2M_ABB_LP,
	MEM_RX_CQ,
	MEM_RX_CE,
	MEM_FREQ_INT,
	MEM_FREQ_FRAC,
	MEM_AMP_CALOUT_2G4,
	MEM_FRE_CALOUT_2G4,
	MEM_AMP_CALOUT_2G5,
	MEM_FRE_CALOUT_2G5,
};

/* RFDSS_CNTL_REG */
#define RFDSS_CNTL_RFADC_CLKPOL_192M   					(0x1U<<22)
#define RFDSS_CNTL_TMD_CLR_POL_96M   						(0x1U<<21)
#define RFDSS_CNTL_TMD_CLR       								(0x1U<<20)
#define RFDSS_CNTL_TMD_CHSEL    							  (0x1U<<18)
#define RFDSS_CNTL_TMODE                        (0x7U<<15)
#define RFDSS_CNTL_PH12M_SEL		                (0x1U<<14)
#define RFDSS_CNTL_RFADC_CLKPOL_96M             (0x1U<<13)
#define RFDSS_CNTL_RFDAC_CLKPOL                 (0x1U<<12)
#define RFDSS_CNTL_SEL_DITH			                (0x1U<<11)
#define RFDSS_CNTL_SEL_RMDC			                (0x1U<<10)
#define RFDSS_CNTL_TX_DEM_EN		                (0x1U<<9)
//#define RFDSS_CNTL_RFCLK_EN		                  (0x1U<<0)

/* RFDSS_MEM_INTF */
#define RFDSS_MEM_INTF_RX_2M_FREQ_OFFSET        (0x7U<<29)
#define RFDSS_MEM_INTF_RX_1M_FREQ_OFFSET        (0x7U<<26)
#define RFDSS_MEM_INTF_TX_2M_FREQ_OFFSET        (0x7U<<23)
#define RFDSS_MEM_INTF_TX_1M_FREQ_OFFSET        (0x7U<<20)
#define RFDSS_MEM_INTF_REQ			                (0x1U<<16)
#define RFDSS_MEM_INTF_WRNRD		                (0x1U<<11)
#define RFDSS_MEM_INTF_PROTOCOL_SEL             (0x1U<<9)
#define RFDSS_MEM_INTF_PROTOCOL_SET             (0x1U<<8)
#define RFDSS_MEM_INTF_ADDR			                (0x1FFU<<0)

/* RFDSS_MEM_DATA */
#define RFDSS_MEM_DATA_VAL								      (0xFFFFFFFFU<<0)

/* RFDSS_TXEM_CNTL */
#define RFDSS_TXEM_CNTL_START			              (0x1U<<31)
#define RFDSS_TXEM_CNTL_ADDR			              (0x7FFFU<<16)
#define RFDSS_TXEM_CNTL_MODE_SEL		            (0x1U<<15)
#define RFDSS_TXEM_CNTL_LEN			                (0x7FFFU<<0)

/* RFDSS_RXEM_CNTL */
#define RFDSS_RXEM_CNTL_START			              (0x1U<<31)
#define RFDSS_RXEM_CNTL_ADDR			              (0x7FFFU<<16)
#define RFDSS_RXEM_CNTL_MODE_SEL		            (0x1U<<15)
#define RFDSS_RXEM_CNTL_LEN			                (0x7FFFU<<0)

/* RFDSS_RXEM_CURADDR */
#define RFDSS_RXEM_CUR_ADDR			                (0x7FFFU<<0)

/* RFDSS_DCEST_CNTL0 */
#define RFDSS_DCEST_CNTL0_EN			              (0x1U<<31)
#define RFDSS_DCEST_CNTL0_ACC_WIN	              (0x3U<<24)
#define RFDSS_DCEST_CNTL0_ACC_NUM_I	            (0x7FFFFFU<<0)

/* RFDSS_DCEST_CNTL1 */
#define RFDSS_DCEST_CNTL1_ACC_NUM_Q	            (0x7FFFFFU<<0)

/* RFDSS_ADC_RSTAUX_CNTL */
#define RFDSS_ADC_RSTAUX_CNTL_ST		            (0x1U<<31)
#define RFDSS_ADC_RSTAUX_CNTL_FLAG              (0x1U<<30)
#define RFDSS_ADC_RSTAUX_CNTL_K		              (0x3U<<26)
#define RFDSS_ADC_RSTAUX_CNTL_DLY	              (0x3U<<24)
#define RFDSS_ADC_RSTAUX_SAT_RESET_EN	          (0x1U<<21)
#define RFDSS_ADC_RSTAUX_CNTL_MODE	            (0x1U<<20)
#define RFDSS_ADC_RSTAUX_CNTL_TH1	              (0x1FU<<15)
#define RFDSS_ADC_RSTAUX_CNTL_TH2	              (0x7FU<<8)
#define RFDSS_ADC_RSTAUX_RESTAR_TIME	          (0x3FU<<2)
#define RFDSS_ADC_RSTAUX_CNTL_SEL	              (0x1U<<1)
#define RFDSS_ADC_RSTAUX_CNTL_EN		            (0x1U<<0)

/* RFDSS_TXLDO_CNTL */
#define RFDSS_TXLDO_CNTL_ADC_RESET_TIME0			  (0xFU<<28)
#define RFDSS_TXLDO_CNTL_ADC_RESET_TIME1			  (0xFU<<24)

/* RFDSS_RXLDO_CNTL */

/* RFDSS_RFVCO_LDO_CNTL */
#define RFDSS_RFVCO_LDO_CNTL_TRIM					      (0xFU<<24)
#define RFDSS_RFVCO_LDO_CNTL_PUP_SEL		        (0x1U<<2)
#define RFDSS_RFVCO_LDO_CNTL_PUP			          (0x1U<<0)

/* RFDSS_IREF_CNTL */
#define RFDSS_IREF_CNTL_TRIM				            (0xFU<<16)
#define RFDSS_IREF_CNTL_PUP_DELAY		            (0x7FU<<8)
#define RFDSS_IREF_CNTL_PUP_CGM1		            (0x1U<<5)
#define RFDSS_IREF_CNTL_PUP_CGM_INV	            (0x1U<<2)
#define RFDSS_IREF_CNTL_TESTEN			            (0x1U<<1)

/* RFDSS_ATEST_IQCAL_CNTL */
#define RFDSS_ATEST_IQCAL_CNTL_TX_IQCAL_PUP			(0x1U<<9)
#define RFDSS_ATEST_IQCAL_CNTL_TX_IQCAL_BANDSEL	(0x1U<<8)
#define RFDSS_ATEST_IQCAL_CNTL_ATEST_PD_TESTIO	(0x1U<<3)
#define RFDSS_ATEST_IQCAL_CNTL_ATEST_LG_TX_DAC	(0x1U<<1)
#define RFDSS_ATEST_IQCAL_CNTL_ATEST_VTUNE_EN		(0x1U<<0)

/* RFDSS_RX_CNTL0 */
#define RFDSS_RX_CNTL0_2M_LPF_CCAL		          (0x1FU<<26)
#define RFDSS_RX_CNTL0_LNALB				            (0x3U<<24)
#define RFDSS_RX_CNTL0_BIAS_EN			            (0x1U<<21)
#define RFDSS_RX_CNTL0_5US_EN				            (0x1U<<20)
#define RFDSS_RX_CNTL0_LNABUFLB			            (0x3U<<16)
#define RFDSS_RX_CNTL0_1M_LPF_CCAL		          (0x1FU<<10)
#define RFDSS_RX_CNTL0_RX_ABB_WOBG              (0x1U<<9)
#define RFDSS_RX_CNTL0_EN_VDD0                  (0x1U<<8)
#define RFDSS_RX_CNTL0_TESTI_EN			            (0x1U<<7)
#define RFDSS_RX_CNTL0_TESTQ_EN			            (0x1U<<6)
#define RFDSS_RX_CNTL0_ABB_TESTI_EN		          (0x1U<<5)
#define RFDSS_RX_CNTL0_ABB_TESTQ_EN		          (0x1U<<4)
#define RFDSS_RX_CNTL0_TEST_SEL			            (0x1U<<3)
#define RFDSS_RX_CNTL0_DCOFFSET_B		            (0x7U<<0)

/* RFDSS_RX_CNTL1 */
#define RFDSS_RX_CNTL1_RX_BW_SEL				        (0x1U<<20)
#define RFDSS_RX_CNTL1_RXABB_PUP_SEL				    (0x1U<<19)
#define RFDSS_RX_CNTL1_RXRF_PUP_SEL				      (0x1U<<18)
#define RFDSS_RX_CNTL1_RX_BW				            (0x1U<<17)
#define RFDSS_RX_CNTL1_RX_LNA_LB_PUP				    (0x1U<<16)
#define RFDSS_RX_CNTL1_RX_BGAP_PUP_INV			    (0x1U<<15)
#define RFDSS_RX_CNTL1_RX_BGAP_PUP				      (0x1U<<7)
#define RFDSS_RX_CNTL1_RX_INABUFF_LB_PUP			  (0x1U<<6)
#define RFDSS_RX_CNTL1_RX_LO_LB_BUF_PUP			    (0x1U<<5)
#define RFDSS_RX_CNTL1_RX_ABBI_PUP				      (0x1U<<4)
#define RFDSS_RX_CNTL1_RX_ABBQ_PUP				      (0x1U<<3)
#define RFDSS_RX_CNTL1_RX_DC_PUP				        (0x1U<<2)
#define RFDSS_RX_CNTL1_RX_DACI_EN				        (0x1U<<1)
#define RFDSS_RX_CNTL1_RX_DACQ_EN				        (0x1U<<0)


/* RFDSS_RX_PARA */
#define RFDSS_RX_PARA_RXADC_PUP_DELAY           (0x7U<<28)
#define RFDSS_RX_PARA_RXRF_ABB_ADC_PUP_DELAY	  (0xFFU<<20)
#define RFDSS_RX_PARA_2M_ADC_SEL				        (0x1U<<19)
#define RFDSS_RX_PARA_1M_ADC_SEL				        (0x1U<<18)
#define RFDSS_RX_PARA_RX_GAINIDX_SEL				    (0x1U<<17)
#define RFDSS_RX_PARA_RX_GAINIDX_UPDATE			    (0x1U<<16)
#define RFDSS_RX_PARA_RX_GAINIDX_VAL			      (0x7FU<<8)
#define RFDSS_RX_PARA_RX_LNA_CW				          (0x7U<<0)


/* RFDSS_RXADC_CNTL */
#define RFDSS_RXADC_CNTL_R_192M					        (0xFU<<28)
#define RFDSS_RXADC_CNTL_VREF_FIL               (0x7U<<22)
#define RFDSS_RXADC_CNTL_VREF_FIL_EN            (0x1U<<20)
#define RFDSS_RXADC_CNTL_CAL_SEL					      (0x1U<<19)
#define RFDSS_RXADC_CNTL_SEL				           	(0x1U<<18)
#define RFDSS_RXADC_CNTL_COREI_PUP_192M			    (0x1U<<14)
#define RFDSS_RXADC_CNTL_COREQ_PUP_192M			    (0x1U<<13)
#define RFDSS_RXADC_CNTL_BIAS_PUP_192M			    (0x1U<<12)
#define RFDSS_RXADC_CNTL_CLKEN_192M				      (0x1U<<11)
#define RFDSS_RXADC_CNTL_RST_I_192M				      (0x1U<<10)
#define RFDSS_RXADC_CNTL_RST_Q_192M				      (0x1U<<9)
#define RFDSS_RXADC_CNTL_COREI_PUP			        (0x1U<<8)
#define RFDSS_RXADC_CNTL_COREQ_PUP			        (0x1U<<7)
#define RFDSS_RXADC_CNTL_BIAS_PUP			          (0x1U<<5)
#define RFDSS_RXADC_CNTL_CLKEN				          (0x1U<<4)
#define RFDSS_RXADC_CNTL_RST_I				          (0x1U<<3)
#define RFDSS_RXADC_CNTL_RST_Q				          (0x1U<<2)
#define RFDSS_RXADC_CNTL_CALEN				          (0x1U<<1)
#define RFDSS_RXADC_CNTL_CALSGN				          (0x1U<<0)


/* RFDSS_RXADC_CAL */
#define RFDSS_RXADC_CAL_R_96M					          (0xFU<<28)
#define RFDSS_RXADC_CAL_DEM_EN				          (0x1U<<26)
#define RFDSS_RXADC_CAL_DEM_SEL				          (0x1U<<25)
#define RFDSS_RXADC_CAL_L_VCM_192M					    (0x1U<<24)
#define RFDSS_RXADC_CAL_L_VCM_96M					      (0x1U<<23)
#define RFDSS_RXADC_CAL_ICW_192M				  	    (0x7U<<19)
#define RFDSS_RXADC_CAL_ICW_96M				  	      (0x7U<<16)
#define RFDSS_RXADC_CAL_CMP_ICW_192M   				  (0x7U<<11)
#define RFDSS_RXADC_CAL_CMP_ICW_96M				      (0x7U<<8)
#define RFDSS_RXADC_CAL_RCAL_192M					      (0xFU<<4)
#define RFDSS_RXADC_CAL_RCAL_96M					      (0xFU<<0)


/* RFDSS_TX_CNTL0 */
#define RFDSS_TX_CNTL0_PUP_SEL    				      (0x1U<<31)
#define RFDSS_TX_CNTL0_PAPOWER_RTUNE		        (0x7U<<28)
#define RFDSS_TX_CNTL0_BW_SEL         	     	  (0x1U<<27)
#define RFDSS_TX_CNTL0_BW         	     	      (0x1U<<25)
#define RFDSS_TX_CNTL0_PA_POWER_BIAS_EDR_PUP		(0x1U<<24)
#define RFDSS_TX_CNTL0_ABB_PUP					        (0x1U<<22)
#define RFDSS_TX_CNTL0_DACANA_PUP				        (0x1U<<21)
#define RFDSS_TX_CNTL0_DIG_RST					        (0x1U<<20)
#define RFDSS_TX_CNTL0_LO_LB_BUF_PUP				    (0x1U<<19)
#define RFDSS_TX_CNTL0_PA_POWER_BIAS_BR_PUP		  (0x1U<<18)
#define RFDSS_TX_CNTL0_PA_POWER_EN				      (0x1U<<17)
#define RFDSS_TX_CNTL0_RF_PUP						        (0x1U<<16)
#define RFDSS_TX_CNTL0_PUP_DELAY1				        (0xFFU<<8)
#define RFDSS_TX_CNTL0_PUP_DELAY2				        (0x7U<<0)

/* RFDSS_TX_CNTL1 */
#define RFDSS_TX_CNTL1_PAPOWER_CTUNE	          (0x7U<<29)
#define RFDSS_TX_CNTL1_LPF_CCAL			            (0x1FU<<24)
#define RFDSS_TX_CNTL1_DACANA_TEST		          (0x3U<<22)
#define RFDSS_TX_CNTL1_ABB_TEST			            (0x3U<<20)
#define RFDSS_TX_CNTL1_CIC_EN                   (0x1U<<18)
#define RFDSS_TX_CNTL1_PAPOWER_TRIM		          (0x3FU<<12)
#define RFDSS_TX_CNTL1_2M_ADC_SEL               (0x1U<<11)
#define RFDSS_TX_CNTL1_1M_ADC_SEL               (0x1U<<10)
#define RFDSS_TX_CNTL1_RF_CASC_CTRL		          (0x3U<<8)
#define RFDSS_TX_CNTL1_IREF_CTRL_MIX		        (0x7U<<4)
#define RFDSS_TX_CNTL1_DAC_PULSE_EN		          (0x1U<<3)
#define RFDSS_TX_CNTL1_DAC_DELAY_SEL		        (0x1U<<2)
#define RFDSS_TX_CNTL1_DAC_TEST_MODE	          (0x1U<<1)
#define RFDSS_TX_CNTL1_TEST_SEL			            (0x1U<<0)

/* RFDSS_TX_CNTL2 */
#define RFDSS_TX_CNTL2_TX_SH_2M                 (0x3U<<10)
#define RFDSS_TX_CNTL2_TX_SH_1M                 (0x3U<<8)
#define RFDSS_TX_CNTL2_TX_WBW_2M                (0x3U<<6)
#define RFDSS_TX_CNTL2_TX_WBW_1M                (0x3U<<4)
#define RFDSS_TX_CNTL2_TX_FILTER_BIAS           (0x3U<<0)

/* RFDSS_TX_GAIN */
#define RFDSS_TX_GAIN_TX_PA_DELAY               (0x7FU<<25)
#define RFDSS_TX_GAIN_TX_BUF_DELAY              (0x7FU<<18)
#define RFDSS_TX_GAIN_IQPHASE_CLIP24B_EN	      (0x1U<<16)
#define RFDSS_TX_GAIN_GAINIDX_SEL			          (0x1U<<9)
#define RFDSS_TX_GAIN_GAINIDX_UPDATE		        (0x1U<<8)
#define RFDSS_TX_GAIN_GAINIDX_VAL			          (0x1FU<<0)

/* RFDSS_RFPLL_CNTL0 */
#define RFDSS_RFPLL_CNTL0_TABLE_DELAY1		      (0x3FU<<25)
#define RFDSS_RFPLL_CNTL0_CNTL_SEL		          (0x1U<<24)
#define RFDSS_RFPLL_CNTL0_CAL_DELAY2		        (0x7FU<<16)
#define RFDSS_RFPLL_CNTL0_CAL_DELAY1		        (0x7FU<<8)
#define RFDSS_RFPLL_CNTL0_PUP_DELAY1		        (0x7FU<<0)

/* RFDSS_RFPLL_CNTL1 */
#define RFDSS_RFPLL_CNTL1_VCO_CALREQ_INV			  (0x1U<<16)
#define RFDSS_RFPLL_CNTL1_DIV2P_LB_PUP			    (0x1U<<15)
#define RFDSS_RFPLL_CNTL1_CALRSTN					      (0x1U<<14)
#define RFDSS_RFPLL_CNTL1_CP_PUP					      (0x1U<<13)
#define RFDSS_RFPLL_CNTL1_SDMASHCLK				      (0x1U<<12)
#define RFDSS_RFPLL_CNTL1_SDCLKEN					      (0x1U<<11)
#define RFDSS_RFPLL_CNTL1_SDRSTN					      (0x1U<<10)
#define RFDSS_RFPLL_CNTL1_PIP_LB_PUP				    (0x1U<<9)
#define RFDSS_RFPLL_CNTL1_PFD_PUP					      (0x1U<<8)
#define RFDSS_RFPLL_CNTL1_CLKCTRL_RSTN			    (0x1U<<7)
#define RFDSS_RFPLL_CNTL1_LB_PUP					      (0x1U<<6)
#define RFDSS_RFPLL_CNTL1_SYNCH_DIVRSTN			    (0x1U<<5)
#define RFDSS_RFPLL_CNTL1_DIVRSTN					      (0x1U<<4)
#define RFDSS_RFPLL_CNTL1_RSTN					        (0x1U<<3)
#define RFDSS_RFPLL_CNTL1_DIV2M_LB_PUP			    (0x1U<<2)
#define RFDSS_RFPLL_CNTL1_VREFBUFF_PUP			    (0x1U<<1)
#define RFDSS_RFPLL_CNTL1_VCO_CALREQ				    (0x1U<<0)

/* RFDSS_RFPLL_PARA0 */
#define RFDSS_RFPLL_PARA0_PLL_TABLE_MODE		    (0x1U<<29)
#define RFDSS_RFPLL_PARA0_PLL_CAL_MODE			    (0x1U<<29)
#define RFDSS_RFPLL_PARA0_PLL_CAL_SEL				    (0x1U<<28)
#define RFDSS_RFPLL_PARA0_PLL_FCOM_SEL			    (0xFU<<24)
#define RFDSS_RFPLL_PARA0_VCO_FRE_SEL			      (0x1U<<23)
#define RFDSS_RFPLL_PARA0_VCO_FREQCAL			      (0x7FU<<16)
#define RFDSS_RFPLL_PARA0_VCO_AMPCAL				    (0xFFU<<8)
#define RFDSS_RFPLL_PARA0_VCO_AMPCAL_SEL		    (0x1U<<7)
#define RFDSS_RFPLL_PARA0_PLL_SD_FILT_RSTN		  (0x1U<<6)
#define RFDSS_RFPLL_PARA0_PLL_SW					      (0x3U<<4)
#define RFDSS_RFPLL_PARA0_PLL_SD_MASH_RSTN		  (0x1U<<3)
#define RFDSS_RFPLL_PARA0_PLL_SD_OCW				    (0x3U<<1)
#define RFDSS_RFPLL_PARA0_PLL_SD_FILTER_SEL		  (0x1U<<0)

/* RFDSS_RFPLL_PARA1 */
#define RFDSS_RFPLL_PARA1_CP_ISEL			          (0x1U<<28)
#define RFDSS_RFPLL_PARA1_DCW				            (0x1U<<27)
#define RFDSS_RFPLL_PARA1_FREF_SEL		          (0x1U<<26)
#define RFDSS_RFPLL_PARA1_FCOMP_SEL		          (0x1U<<25)
#define RFDSS_RFPLL_PARA1_TEST_COMP		          (0x1U<<24)
#define RFDSS_RFPLL_PARA1_PIPE_LP		            (0x1U<<20)
#define RFDSS_RFPLL_PARA1_VREFBUF_LP			      (0x1U<<19)
#define RFDSS_RFPLL_PARA1_LP_CP			            (0x1U<<18)
#define RFDSS_RFPLL_PARA1_REFFREQ_SEL	          (0x3U<<16)
#define RFDSS_RFPLL_PARA1_SPARE_OUT		          (0x3U<<14)
#define RFDSS_RFPLL_PARA1_DOWN_SPLIT		        (0x3U<<12)
#define RFDSS_RFPLL_PARA1_CP_ICW			          (0x7U<<8)
#define RFDSS_RFPLL_PARA1_AMP_VCO		            (0x3U<<4)
#define RFDSS_RFPLL_PARA1_DIV2_EN			          (0x1U<<3)
#define RFDSS_RFPLL_PARA1_LPF_EN			          (0x3U<<0)

/* RFDSS_RFPLL_FREQ */
#define RFDSS_RFPLL_FREQ_FREQIDX_SEL		        (0x1U<<9)
#define RFDSS_RFPLL_FREQ_FREQIDX_UPDATE	        (0x1U<<8)
#define RFDSS_RFPLL_FREQ_FREQIDX_VAL		        (0x7FU<<0)

/* RFDSS_RFPLL_OUT0 */
#define RFDSS_RFPLL_OUT0_P_OUT			            (0xFFU<<16)
#define RFDSS_RFPLL_OUT0_M_OUT			            (0x7FU<<8)
#define RFDSS_RFPLL_OUT0_MSB_OUT			          (0x1U<<7)
#define RFDSS_RFPLL_OUT0_FREQCAL_OUT	 	        (0x7FU<<0)

/* RFDSS_RFPLL_OUT1 */
#define RFDSS_RFPLL_OUT1_P_OUT			            (0xFFFU<<16)
#define RFDSS_RFPLL_OUT1_M_OUT			            (0xFFFU<<0)

/* RFDSS_RFPLL_OUT2 */
#define RFDSS_RFPLL_OUT2_VCO                    (0xFFU<<12)
#define RFDSS_RFPLL_OUT2_OUT				            (0xFFFU<<0)

/* RFDSS_ADC_CAL_STATUE */
#define RFDSS_ADC_CAL_STATUE_RES_VAL            (0x7FFFFU<<0)

/* RFDSS_ADC_CAL_TH */
#define RFDSS_ADC_CAL_TH_RES_TH                 (0x3FFFFU<<0)

/* RFDSS_EM_ADDR_TH */
#define RFDSS_EM_ADDR_TH_BLE_FLAG               (0x1U<<31)
#define RFDSS_EM_ADDR_TH_BT_FLAG                (0x1U<<30)
#define RFDSS_EM_ADDR_TH_BLE_ADDR               (0xFFFFU<<0)

/* RFDSS_GAIN_SATUE */
#define RFDSS_GAIN_SATUE_RX_2M_ABB_LP           (0x3U<<27)
#define RFDSS_GAIN_SATUE_DAC_GAIN               (0x7U<<24)
#define RFDSS_GAIN_SATUE_TX_GAIN                (0xFFU<<16)
#define RFDSS_GAIN_SATUE_MXIER_BUFLP            (0x1U<<15)
#define RFDSS_GAIN_SATUE_RX_1M_ABB_LP           (0x3U<<13)
#define RFDSS_GAIN_SATUE_LNA_G                  (0xFU<<9)
#define RFDSS_GAIN_SATUE_LNA_GAIN               (0x3U<<7)
#define RFDSS_GAIN_SATUE_RX_2M_ABB_LP0          (0x1U<<6)
#define RFDSS_GAIN_SATUE_RX_1M_ABB_LP0          (0x1U<<5)
#define RFDSS_GAIN_SATUE_RX_GAIN                (0x1FU<<0)

/* RFDSS_PLL_SATUE */
#define RFDSS_PLL_SATUE_INT                     (0x1FFU<<21)
#define RFDSS_PLL_SATUE_FRAC                    (0x1FFFFFU<<0)

/*RFDSS_PLL_FRE_OFFSET0*/
#define RFDSS_PLL_FRE_INT_OFFSET0               (0x1FFU<<21)
#define RFDSS_PLL_FRE_FRAC_OFFSET0              (0x1FFFFFU<<0)

/*RFDSS_PLL_FRE_OFFSET1*/
#define RFDSS_PLL_FRE_FRAC_OFFSET1              (0x1FFFFFU<<0)
/*---------------------RFDSS Register define end---------------------*/




/*----------------------ccmu Register define------------------------*/
#define CCMU_BASE                  0X40080000
#define RF_192M_96EM_EN            __REG(CCMU_BASE + 0x30)
#define RF_MODULE_GATING_CFG       __REG(CCMU_BASE + 0x64)//BTSS clk_gating is bit2
#define RF_MODULE_RST_CFG          __REG(CCMU_BASE + 0x74)//BTSS rest is bit2
#define BTSS_MODULE_CLK_CFG        __REG(CCMU_BASE + 0xe8)//BTSS CLK enable bit0

/* RF_192M_96EM_EN */
#define CORE_PLL_DIV_CLK96M_EN           (0x1U<<5)
#define CORE_PLL_DIV_CLK192M_EN          (0x1U<<4)
#define CORE_PLL_DIV_CPLL_DIV            (0x7U<<0)

/* RF_MODULE_GAT_CFG */
#define MODULE_GAT_CFG_BTSS_CLK_EN			(0x1U<<2)

/* RF_MODULE_RST_CFG */
#define MODULE_RST_CFG_BTSS_RST			(0x1U<<2)

/*BTSS_MODULE_CLK_CFG*/
#define BTSS_MODULE_CLK_CFG_EN    (0X1U<<0)
/*---------------------ccmu Register define end---------------------*/


/*----------------------RTC Register define------------------------*/
#define RTC_REG_                 0x40085000
#define XO_CTL_REG             __REG(RTC_REG_+0x90)

/* XO_CTL_REG */
#define XO_CTL_REG_TRIM			   (0xFFU<<4)
#define XO_CTL_REG_ICTL			   (0x1FU<<12)

/*----------------------simulation Register define------------------------*/
#define MEM_DUMP_CTRL                0x40000104
/*---------------------simulation Register define end----------------------*/





/*--------------------Register read_write define----------------------*/
#define mctl_write_w(v, addr)	(*((volatile unsigned int  *)(addr)) = (unsigned int)(v))
#define mctl_read_w(addr)		(*((volatile unsigned int  *)(addr)))
#define iq_write(v, addr)	(*((volatile unsigned short  *)(addr)) = (short)(v))
#define iq_read(addr)		(*((volatile unsigned short  *)(addr)))
/*------------------Register read_write define end--------------------*/

extern short sin_cos_demod[256];
extern volatile short tx_dc_i_1m, tx_dc_q_1m, tx_ce_1m, tx_cq_1m, tx_dc_i_2m, tx_dc_q_2m, tx_ce_2m, tx_cq_2m,cal_mode,tx_if_mode ;
extern uint8_t bt_testmode_get(void);//1 dut mode,0 bt mode

/*-------------------------function define---------------------------*/
extern void rf_udelay(int val);
extern unsigned int  rfdss_test(void);
extern void btss_close(void);
extern unsigned int rfdss_fre_cal(void);
extern void rf_power_force(uint8_t enable, uint8_t power);
extern int rfd_read(enum rfd_mem_type type, unsigned int index);
extern void rfd_write(enum rfd_mem_type type, unsigned int index, int val);
extern void rx_para_set(unsigned int rx_gain_lna,unsigned int rx_abb_gain,unsigned int rx_lna_g);
extern unsigned int rfdss_mem_test(void);
extern unsigned int	rfdss_em_tx_rx_test(void);
extern unsigned int	rfdss_tx_rx_loopback_test(void);
extern unsigned int	rfdss_adc_cal_test(void);
extern unsigned int	rfdss_adc_capture_test(void);
extern unsigned int	rfdss_adc_rst_test(void);
extern unsigned int	rfdss_dcest_test(void);
extern unsigned int	rfdss_gain_test(void);
extern unsigned int	rfdss_interface_test(void);
extern unsigned int	rfdss_em_test(void);
extern unsigned int	rfdss_rfa_staue_test(void);
extern unsigned int	rfdss_inv_test(void);
extern unsigned int	rfdss_efuse_test(void);
extern unsigned int	rfdss_gain_sta_test(void);
extern unsigned int	modem_reg_test(void);
extern unsigned int	rfdss_iref_test(void);
extern unsigned int	rfdss_mux_ctr_signal_test(void);
extern void rfdss_mem_inil(void);
extern void tx_ldo_power_up(void);//1.1
extern void tx_ldo_lqb(void);//1.4
extern void rx_ldo_power_up(void);//1.2
extern void rx_ldo_lqb(void);//1.5
extern void rfvco_ldo_power_up(void);//1.3
extern void rfvco_ldo_lqb(void);//1.6
extern void iref_power_up(void);//2.1
extern void iref_test_en(void);//2.2
extern void atest_en(void);//2.3
extern void iqcal_power_up(void);//2.4
extern void rx_rf_power_up(void);//3.1
extern void rx_abb_power_up(void);//3.2
extern void rx_adc_power_up(char adc_sel);//4.1
extern void rx_adc_reset(char adc_sel);
extern void rx_adc_cal_power_up(char adc_sel);//4.2
extern void rx_adc_reset_release(char adc_sel);//4.3
extern void rx_open_no_adc(void);
extern void tx_power_up(void);//5.1
extern void rf_pll_power_up(void);//6.1
extern void rf_pll_cal(void);//6.2
extern void tx_open(void);
extern void rx_open(char adc_sel);
extern void RF_SET_FREQ(unsigned int freq);
extern void rfdss_init(void);
extern void rfd_mem_write(unsigned int addr, int data);
extern unsigned int rfd_mem_read(unsigned int addr);
extern void RF_SET_FRAC_INT(short pll_int, int pll_frac);
extern void rfdss_para_cal(void);
extern void rfdss_cal_init(void);
extern void Tone_Gen_10k(unsigned int fre_indix,unsigned int em_addr,char am_shift);
extern void rfdss_tone(unsigned int fre);
extern unsigned int	rfdss_adc_cal(char adc_sel);
extern float power_cal(unsigned int fft_size,unsigned int fft_pot,short *sin_table,short *cos_table);
extern unsigned int rfdss_ccal_cal(unsigned int pa,unsigned int mix,unsigned int rx_gain_lna,unsigned int rx_abb_gain,unsigned int rx_lna_g);
extern void tx_para_set(unsigned int pa,unsigned int mix,int dig,char dac_gain,int tx_if_fre,char if_shift,char BW_1M,char BW_2M);
extern void	rfdss_tx_iq_dc_cal_test(unsigned int pa,unsigned int mix)	;
extern int tx_dc_i_comp_cal(int dc_i_comp_min,int dc_i_comp_max,unsigned int dc_i_comp_step,unsigned int cal_loop_i,unsigned int fft_size,unsigned int fft_pot,short *sin_table,short *cos_table);
extern int tx_dc_q_comp_cal(int dc_q_comp_min,int dc_q_comp_max,unsigned int dc_q_comp_step,unsigned int cal_loop_q,unsigned int fft_size,unsigned int fft_pot,short *sin_table,short *cos_table);
extern int tx_ce_comp_cal(int ce_comp_min,int ce_comp_max,unsigned int ce_comp_step,unsigned int cal_loop_ce,unsigned int fft_size,unsigned int fft_pot,short *sin_table,short *cos_table);
extern int tx_cq_comp_cal(int ce_comp_min,int ce_comp_max,unsigned int ce_comp_step,unsigned int cal_loop_ce,unsigned int fft_size,unsigned int fft_pot,short *sin_table,short *cos_table);
extern short *freq_int_list_get(void);
extern int *freq_frac_list_get(void);

/*-----------------------function define end-------------------------*/

#endif  //_RFDSS_H

