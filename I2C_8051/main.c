#include <REGX51.H>
#include <stdio.h>
#include "I2C.h"
unsigned char slave1 = 0x7E;

int main(){
	int so = 2024;
	char chuoi[] = "Hello TTS";
	add_slave(slave1);
	lcd_init();
	lcd_col_row(3,0);
	send_string(&chuoi);
	lcd_col_row(5,1);
	delay_1ms(2000);
	send_number(2024);
	while(1){
	}
}
