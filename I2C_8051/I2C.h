#ifndef __I2C__
#define __I2C__
void start_i2c();
void stop_i2c();
void ack();
void add_slave(unsigned char add);
void send_cmd(unsigned char cmd);
void send_data(unsigned char dat);
void write_i2c(unsigned char dat);
void send_string(unsigned char *p);
void lcd_init();
void lcd_col_row(int col, int row);
void send_number(int number);
void delay_1ms(int time);
#endif