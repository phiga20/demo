#include<stdio.h>
#include <REGX51.H>
#include "I2C.h"
unsigned char address_slave;
sbit SCL=P0^6;
sbit SDA=P0^7;
void start_i2c(){
	SDA = 1;
	SCL = 1;
	SDA = 0;
	SCL = 0;
}
void stop_i2c(){
	SCL = 0;
	SDA = 0;
	SCL = 1;
	SDA = 1;
}
void ack(){
	SCL = 0;
	SDA = 1;
	SCL = 1;
	while(SDA);
}
void add_slave(unsigned char address){
	address_slave = address;
}
void delay_1ms(int time){
    while(time > 0){
        TMOD = 0x01;
        TH0 = 0xFC;
        TL0 = 0x66;
        TR0 = 1;
        while(TF0 == 0){
        }
        TF0 = 0;
        TR0 = 0;
        time--;
    }
}
void write_i2c(unsigned char dat){
	int i;
	for(i=0;i<8;i++){
		SCL = 0;
		SDA = (dat & (0x80 >> i))?1:0;
		SCL = 1;
	}
}
void send_cmd(unsigned char cmd){
	unsigned char b_h = cmd & 0xF0;
	unsigned char b_l = (cmd << 4) & 0xF0;
	start_i2c();
	write_i2c(address_slave);
	ack();
	write_i2c(b_h|0x0C);
	ack();
	delay_1ms(1);
	write_i2c(b_h|0x08);
	ack();
	delay_1ms(1);
	write_i2c(b_l|0x0C);
	ack();
	delay_1ms(1);
	write_i2c(b_l|0x08);
	ack();
	delay_1ms(1);
	stop_i2c();
}
void send_data(unsigned char dat){
	unsigned char b_h = dat&0xF0;
	unsigned char b_l = (dat<<4) & 0xF0;
	start_i2c();
	write_i2c(address_slave);
	ack();
	write_i2c(b_h|0x0D);
	ack();
	delay_1ms(1);
	write_i2c(b_h|0x09);
	ack();
	delay_1ms(1);
	write_i2c(b_l|0x0D);
	ack();
	delay_1ms(1);
	write_i2c(b_l|0x09);
	ack();
	delay_1ms(1);
	stop_i2c();
}
void send_string(unsigned char *p){
	while(*p != '\0'){
		send_data(*p++);
	}
}
void lcd_col_row(int col, int row){
	int p_address;
	if(row==0){
		p_address = 0x80 + col;
	}
	else{
		p_address = 0xC0 + col;
	}
	send_cmd(p_address);
}
void lcd_init()
{
	send_cmd(0x02);
	send_cmd(0x28);
	send_cmd(0x0C);
	send_cmd(0x06);
	send_cmd(0x01);
}
void send_number(int number){
	if(number <= 9){
		send_data(number+48);
	}
	if(number > 9 && number <100 ){
		send_data(number/10+48);
		send_data(number%10+48);
	}
	if(number >= 100 && number < 1000){
		send_data(number/100+48);
		send_data(number/10%10+48);
		send_data(number%10+48);
	}
		if(number >= 1000 && number < 10000){
		send_data(number/1000+48);
		send_data(number/100%10+48);
		send_data(number/10%10+48);
		send_data(number%10+48);
 }
}