/************************************************************************************//**
* \file         port/LINUX/tbxport.c
* \brief        Port specifics source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX global header             */
#include <pthread.h>                             /* Posix thread utilities             */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Flag to determine if the critical section object was already initialized. */
static volatile uint8_t criticalSectionInitialized = TBX_FALSE;

/** \brief Flag to keep track of the interrupt disabled status. */
static volatile uint8_t interruptsDisabled = TBX_FALSE;

/** \brief Critical section object. */
static volatile pthread_mutex_t mtxCritSect;


/************************************************************************************//**
** \brief     Stores the current state of the CPU status register and then disables the
**            generation of global interrupts. The status register contains information
**            about the interrupts being disable/enabled before they get disabled. This
**            is needed to later on restore the state.
** \return    The current value of the CPU status register.
**
****************************************************************************************/
tTbxPortCpuSR TbxPortInterruptsDisable(void)
{
  tTbxPortCpuSR result;

  /* Initialize the result. Note that this value is don't care for this port, as under
   * Linux, the already available PThread Mutex API is used.
   */
  result = 0;
  
  /* Make sure the critical section object is initialized. */
  if (criticalSectionInitialized == TBX_FALSE)
  {
    /* Initialize the critical section object. */
    (void)pthread_mutex_init((pthread_mutex_t *)&mtxCritSect, NULL);
    /* Set initialized flag. */
    criticalSectionInitialized = TBX_TRUE;
  }

  /* Enter the critical section, if not already entered. */
  if (interruptsDisabled == TBX_FALSE)
  {
    interruptsDisabled = TBX_TRUE;
    (void)pthread_mutex_lock((pthread_mutex_t *)&mtxCritSect);
  }
  
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxPortInterruptsDisable ***/


/************************************************************************************//**
** \brief     Restores the interrupts enabled/disabled state to the state it was when
**            function TbxPortInterruptsDisable() was previously called. It does this
**            by writing the value of the CPU status register that was returned by
**            TbxPortInterruptsDisable().
** \param     prev_cpu_sr The previous value of the CPU status register from right before
**            the interrupts where disabled. This value is returned by function
**            TbxPortInterruptsDisable().
**
****************************************************************************************/
void TbxPortInterruptsRestore(tTbxPortCpuSR prev_cpu_sr)
{
  /* The parameter is not used for this port, because under Linux, the already
   * available PThread Mutex API is used. 
   */
  TBX_UNUSED_ARG(prev_cpu_sr);

  /* Make sure the critical section object was initialized. */
  TBX_ASSERT(criticalSectionInitialized == TBX_TRUE);

  /* Leave the critical section, if is was entered. */
  if (interruptsDisabled == TBX_TRUE)
  {
    (void)pthread_mutex_unlock((pthread_mutex_t *)&mtxCritSect);
    interruptsDisabled = TBX_FALSE;
  }
} /*** end of TbxPortInterruptsRestore ***/


/*********************************** end of tbxport.c **********************************/
