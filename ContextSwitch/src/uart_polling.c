/**
 * @brief: uart_polling.c, polling UART to send and receive data
 * @author: Yiqing Huang
 * @date: 2013/01/06
 * NOTE: the code only handles UART0 for now. 
 */

#include <LPC17xx.h>
#include "uart_polling.h"

/**
 * @brief: read a char from the n_uart, blocking read
 */
     
int uart_get_char(int n_uart)
{
  LPC_UART_TypeDef *pUart;

  if (n_uart == 0) {
    pUart = (LPC_UART_TypeDef *) LPC_UART0;
  } else if (n_uart == 1) {
    pUart = (LPC_UART_TypeDef *) LPC_UART1;
  } else {
    return -1;  /* UART1,2,3 not supported yet */
  }

  /* polling the LSR RDR (Receiver Data Ready) bit to wait it is not empty */
  while (!(pUart->LSR & LSR_RDR)); 
  return (pUart->RBR); 
}

/**
 * @brief: write a char c to the n_uart
 */
  
int uart_put_char(int n_uart, unsigned char c)
{
  LPC_UART_TypeDef *pUart;

  if (n_uart == 0) {
    pUart = (LPC_UART_TypeDef *)LPC_UART0;
  } else if (n_uart == 1) {
    pUart = (LPC_UART_TypeDef *)LPC_UART1;
  } else {
    return 1;  // UART 1,2,3 not supported
  }

  /* polling LSR THRE bit to wait it is empty */
  while (!(pUart->LSR & LSR_THRE)); 
  return (pUart->THR = c);  /* write c to the THR */
}

/**
 * @brief write a string to UART
 */
int uart_put_string(int n_uart, unsigned char *s)
{
  if (n_uart >1 ) return -1;    /* only uart0 is supported for now      */
  while (*s !=0) {              /* loop through each char in the string */
    uart_put_char(n_uart, *s++);/* print the char, then ptr increments  */
  }
  return 0;
}
