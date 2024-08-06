#include "stm32f10x.h"   // Device header 
#include <string.h>

#define max_kitu  100
char chuoi[max_kitu];
int demdodai = 0; 

void delay(){
    int i, j;
    for(i=0; i<100; i++){
        for(j=0; j<10000; j++);
    }
}
char nhan_1kitu(){
    while(!(USART1->SR & USART_SR_RXNE));
    char mot_kitu = USART1->DR;
    return mot_kitu;
}
void nhan_1chuoi(){
	char kytu = nhan_1kitu();
	chuoi[demdodai++] = kytu;
}

void gui_1kitu(char data){
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = data;
}

void gui_1chuoi(char *a){
    while(*a != 0){
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = *a++;
    }
}

int main(){
    int k;
    RCC->APB2ENR = (1<<2 | 1<<4 | 1<<14); // CLOCK UART AND GPIOA,C
    GPIOA->CRH &= 0xFFFFF00F; //OUTPUTGPIOA9 10;
    GPIOA->CRH |= 0x000004F0;
    //OUTPUT GPIOC13
    GPIOC->CRH &= 0xFF0FFFFF;
    GPIOC->CRH |= 0x00300000;
    //SET BUADRATE UART 9600;
    USART1->BRR = 0x1D4C;
    //UART TX=1 RX = 1; UART = 1;
    USART1->CR1 |= (1<<2 | 1<<3 | 1<<13);
		GPIOC->ODR |= 0x2000;
    while(1){
			nhan_1chuoi();
			if(strcmp(chuoi, "ledon")==0){
				gui_1chuoi(chuoi);
				for(k=0;k<5;k++){
				GPIOC->ODR |= 0x2000;
				delay();
				GPIOC->ODR &= ~0x2000;
			}
        memset(chuoi, 0, demdodai);
        demdodai = 0;
		}
			else if(strcmp(chuoi, "ledoff")==0){
				gui_1chuoi(chuoi);
				GPIOC->ODR &= ~0x2000;
        memset(chuoi, 0, demdodai);
        demdodai = 0;
			}
//			char temp = nhan_1kitu();
//			if(temp == 'a'){
//			gui_1chuoi("chucmung");
//			delay();
//			}
    }
}