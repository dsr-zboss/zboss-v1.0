/***********************************************************************************
  Filename:     util_buffer.c

  Description:  Ringbuffer implementation.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "util_buffer.h"
#include "hal_board.h"
#include "hal_int.h"
#include "hal_assert.h"

/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      bufInit
*
* @brief   Initialise a ringbuffer. The buffer must be allocated by the
*          application.
*
* @param   pBuf - pointer to the ringbuffer
*
* @return  none
*/
void bufInit(ringBuf_t *pBuf)
{
    pBuf->nBytes= 0;
    pBuf->iHead= 0;
    pBuf->iTail= 0;
}


/***********************************************************************************
* @fn      bufPut
*
* @brief   Add bytes to the buffer.
*
* @param   pBuf - pointer to the ringbuffer
*          pData - pointer to data to be appended to the buffer
*          nBytes - number of bytes
*
* @return  Number of bytes copied to the buffer
*/
uint8 bufPut(ringBuf_t *pBuf, const uint8 *pData, uint8 nBytes)
{
    uint8 i;
    
    if (pBuf->nBytes+nBytes < BUF_SIZE) {

        i= 0;
        while(i<nBytes) {
            pBuf->pData[pBuf->iTail]= pData[i];
            pBuf->iTail++;
            if (pBuf->iTail==BUF_SIZE)
                pBuf->iTail= 0;
            i++;
        }
        pBuf->nBytes+= i;
    } else {
        i= 0;
    }

    return i;
}


/***********************************************************************************
* @fn      bufGet
*
* @brief   Extract bytes from the buffer.
*
* @param   pBuf   - pointer to the ringbuffer
*          pData  - pointer to data to be extracted
*          nBytes - number of bytes
*
* @return  Bytes actually returned
*/
uint8 bufGet(ringBuf_t *pBuf, uint8 *pData, uint8 nBytes)
{
    uint8 i;

    i= 0;
    while(i<nBytes && i<pBuf->nBytes) {
        pData[i]= pBuf->pData[pBuf->iHead];
        pBuf->iHead++;
        if (pBuf->iHead==BUF_SIZE)
            pBuf->iHead= 0;
        i++;
    }
    pBuf->nBytes-= i;

    return i;
}


/***********************************************************************************
* @fn      bufNumBytes
*
* @brief   Return the byte count for the ring buffer.
*
* @param   pBuf- pointer to the buffer
*
* @return  Number of bytes present.
*/
uint8 bufNumBytes(ringBuf_t *pBuf)
{
    return pBuf->nBytes;
}


/***********************************************************************************
* @fn      bufIsFull
*
* @brief   Return 1 if the ring buffer is full.
*
* @param   pBuf- pointer to the buffer
*
* @return  1 if buffer is full, else 0.
*/
uint8 bufIsFull(ringBuf_t *pBuf)
{
    return (pBuf->nBytes == BUF_SIZE);
}

/***********************************************************************************
* @fn      bufHasEnoughSpace
*
* @brief   Return 1 if the ring buffer is full.
*
* @param   pBuf- pointer to the buffer
*
* @return  1 if buffer is full, else 0.
*/
uint8 bufHasEnoughSpace(ringBuf_t *pBuf, uint8 len)
{
    return (pBuf->nBytes + len < BUF_SIZE);
}

/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/

