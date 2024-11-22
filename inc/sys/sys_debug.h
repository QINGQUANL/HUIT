#ifndef __DEBUG_H__
#define __DEBUG_H__

//*****************************************************************************
//
// Prototype for the function that is called when an invalid argument is passed
// to an API.  This is only used when doing a DEBUG build.
//
//*****************************************************************************
//extern void __error__(char *pcFilename, unsigned long ulLine);



/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(unsigned char * file, unsigned long line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT *//* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


//*****************************************************************************
//
// The ASSERT macro, which does the actual assertion checking.  Typically, this
// will be for procedure arguments.
//
//*****************************************************************************
#ifdef USE_FULL_ASSERT
#define ASSERT(expr)                                       \
                         if(!(expr))                        \
                         {   								\
                             assert_failed(__FILE__, __LINE__);  \
                         }                                  \

#else
#define ASSERT(expr)			((void)0)
#endif


#endif // __DEBUG_H__
