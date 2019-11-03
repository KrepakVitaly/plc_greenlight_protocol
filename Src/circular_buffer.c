/**
  ******************************************************************************
  *
  * @file    circular_buffer.c 
  * @author  Vitaliy KREPAK
  * @version V1.0.1
  * @date    30.10.2019
  * @brief   Realisation of circular buffer of uint8_t.
  *          This buffer was implemented for
  *          parcing the UART byte flow and finding packets
  *
  ******************************************************************************
  */
  
#include "circular_buffer.h"
  

uint8_t CircularBuffer_Init(CircularBuffer_Typedef* buf)
{
  buf->size = 0;
  buf->head = 0;
  buf->tail = 0;
  buf->max_size = CIRC_BUFFER_LEN;
  for(uint16_t i = 0; i < CIRC_BUFFER_LEN; i++)
    buf->data[i] = 0;
  return 0;
}

/// Free a circular buffer, head == tail
uint8_t CircularBuffer_Clear(CircularBuffer_Typedef* buf)
{
  buf->size = 0;
  buf->head = 0;
  buf->tail = 0;
  for(uint16_t i = 0; i < CIRC_BUFFER_LEN; i++)
    buf->data[i] = 0;
  return 0;
}

/// Put continues to add data if the buffer is full
/// Old data is overwritten
uint8_t CircularBuffer_Put_OW(CircularBuffer_Typedef* buf, uint8_t val)
{
  if (CircularBuffer_IsFull(buf))
  {
    if (buf->head == CIRC_BUFFER_LEN - 1)
      buf->head = 0;
    else
      buf->head++;
  }
  else
  {
    buf->size++;
  }
  
  buf->data[buf->tail] = val;
  
  //if tail is in the end of array
  if (buf->tail == CIRC_BUFFER_LEN - 1)
    buf->tail = 0;
  else
    buf->tail++;
  

  return CIRC_BUFFER_OK;
}

/// Put Version 2 rejects new data if the buffer is full
/// Returns 0 on success, -1 if buffer is full
//uint8_t CircularBuffer_Put(CircularBuffer_Typedef* buf, uint8_t val)
//{
//  if (CircularBuffer_IsFull(buf))
//    return CIRC_BUFFER_ERR;
//  else
//    buf->size++;
//  
//  buf->data[buf->tail] = val;
//  
//  //if tail is in the end of array
//  if (buf->tail == CIRC_BUFFER_LEN - 1)
//    buf->tail = 0;
//  else
//    buf->tail++;
//  
//  return CIRC_BUFFER_OK;
//}


uint8_t CircularBuffer_RemoveLastValue(CircularBuffer_Typedef* buf)
{
  if (CircularBuffer_IsEmpty(buf))
    return CIRC_BUFFER_ERR;
  else
  {
    buf->size--;
  
    //if tail is in the end of array
    if (buf->tail == 0)
      buf->tail = CIRC_BUFFER_LEN - 1;
    else
      buf->tail--;

    buf->data[buf->tail] = 0;
    
    return CIRC_BUFFER_OK;
  }
}

uint8_t CircularBuffer_RemoveFirstValue(CircularBuffer_Typedef* buf)
{
  
  if (CircularBuffer_IsEmpty(buf))
    return CIRC_BUFFER_ERR;
  else
  {
    buf->size--;
    buf->data[buf->head] = 0;
  
    //if tail is in the end of array
    if (buf->head == CIRC_BUFFER_LEN - 1)
      buf->head = 0;
    else
      buf->head++;

    return CIRC_BUFFER_OK;
  }
}

uint8_t CircularBuffer_RemoveFirstNValues(CircularBuffer_Typedef* buf, uint16_t num)
{
  if (num > buf->size)
    return CIRC_BUFFER_ERR;
  
  for(uint16_t i = 0; i < num; i++)
  {
    if(CircularBuffer_RemoveFirstValue(buf) != CIRC_BUFFER_ERR)
      continue;
    else
      return CIRC_BUFFER_ERR;
  }
  return CIRC_BUFFER_OK;
}

uint8_t CircularBuffer_RemoveLastNValues(CircularBuffer_Typedef* buf, uint16_t num)
{
  if (num > buf->size)
    return CIRC_BUFFER_ERR;
  
  for(uint16_t i = 0; i < num; i++)
  {
    if(CircularBuffer_RemoveLastValue(buf) != CIRC_BUFFER_ERR)
      continue;
    else
      return CIRC_BUFFER_ERR;
  }
  return CIRC_BUFFER_OK;
}

uint16_t CircularBuffer_GetLength(CircularBuffer_Typedef* buf)
{
  return buf->size;
}

//uint8_t CircularBuffer_GetLastValue(CircularBuffer_Typedef* buf, uint8_t * result)
//{
//  if(CircularBuffer_IsEmpty(buf))
//    return CIRC_BUFFER_ERR;
//  else
//  {
//    if (buf->head != 0)
//      *result = buf->data[buf->head-1];
//    else
//      *result = buf->data[buf->max_size-1];
//    return CIRC_BUFFER_OK;
//  }
//}

uint8_t CircularBuffer_GetRandValue(CircularBuffer_Typedef* buf, uint16_t idx)
{
  if (idx >= buf->size)
    return 0;
  
  if (buf->head + idx < buf->max_size)
    return buf->data[buf->head + idx];
  else
    return buf->data[(buf->head + idx) - buf->max_size];
}

//uint8_t CircularBuffer_GetRandValue(CircularBuffer_Typedef* buf, uint8_t * result, uint16_t idx)
//{
//  if (idx >= buf->size)
//    return CIRC_BUFFER_ERR;
//  
//  if (buf->head + idx < buf->max_size)
//  {
//    *result = buf->data[buf->head + idx];
//  }
//  else
//  {
//    *result = buf->data[buf->max_size - (buf->head + idx)];
//  }
//
//  return CIRC_BUFFER_OK;
//}

uint16_t CircularBuffer_GetLastNValues(CircularBuffer_Typedef* buf, uint8_t * result, uint16_t num)
{
  if (num > buf->size)
    return CIRC_BUFFER_ERR;
  
  for(uint16_t i = 1; i <= num; i++)
  {
    if (buf->tail >= i)
    {
      result[num-i] = buf->data[buf->tail - i];
    }
    else
    {
      result[num-i] = buf->data[buf->max_size - (buf->tail + i)];
    }
  }
  return CIRC_BUFFER_OK;
}

Bool CircularBuffer_IsEmpty(CircularBuffer_Typedef* buf)
{
  if (buf->head == buf->tail && buf->size == 0)
    return True;
  else
    return False;
}

Bool CircularBuffer_IsFull(CircularBuffer_Typedef* buf)
{
  //if circular buffer is not full
  if (buf->size < buf->max_size - 1)
  {
    return False;
  }
  else //if circular buffer is full
  {
    return True;
  }
}

uint16_t dist (uint16_t head, uint16_t tail, uint16_t module)
{
  //1. normal mode T > H and |T - H| < MAX_SIZE, dist = T - H
  if (tail > head)
    return tail - head;
  //2. reverse mode T < H and |T - H| < MAX_SIZE, dist = MODULE - (H - T)
  else if (tail < head)
    return module - (head - tail); 
  
  //3. neigbor mode T > H and |E - H| = 1, size = T - H = 1
  //4. neigbor reverse mode H > T and |T - S| = 1, size = MODULE - H + T
  //5. edge reverse mode T > H and |T - H| = 1, size = H + 1 = 1
  //6. max mode T > H and |T - H| = MAX_SIZE
  return 0;
}

