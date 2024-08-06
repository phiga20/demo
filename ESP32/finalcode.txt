#include <string.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi.h>

#define lcd_add 0x27
#define lcd_cot 16
#define lcd_hang 2
#define RX2 16
#define TX2 17

// Thông tin mạng WiFi
const char* ssid = "taikhoanlagi";
const char* mk = "lamgicopass";

// Thông tin MySQL Server
IPAddress server_addr(192, 168, 0, 107);  // Địa chỉ IP của máy tính chạy MySQL Server
char user[] = "root";                     // MySQL username
char password[] = "";                     // MySQL password (trống)

WiFiClient client;
MySQL_Connection conn(&client);
LiquidCrystal_I2C lcd(lcd_add, lcd_cot, lcd_hang);
HardwareSerial GM65(2);
String receivedString;
float total_price = 0.0;  // Biến để lưu trữ tổng giá tiền
int mode = 1; // Biến lưu trữ chế độ hiện tại

void setup() {
  Serial.begin(9600);
  GM65.begin(9600, SERIAL_8N1, 16, 17);
  while (!Serial);

  // Kết nối WiFi
  WiFi.begin(ssid, mk);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Kết nối MySQL
  Serial.print("Connecting to MySQL Server at ");
  Serial.println(server_addr);
  if (conn.connect(server_addr, 3306, user, password)) {
    Serial.println("Connected to MySQL Server");
    
    // Chọn cơ sở dữ liệu
    MySQL_Cursor cur_mem = MySQL_Cursor(&conn);
    cur_mem.execute("USE sanpham"); // Thay "sanpham" bằng tên cơ sở dữ liệu của bạn
    
  } else {
    Serial.println("Connection to MySQL failed.");
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MA:");
  delay(2000);
}

void loop() {
  // Kiểm tra ký tự từ terminal
  if (Serial.available()) {
    char command = Serial.read();
    switch (command) {
      case '1':
        mode = 1;
        break;
      case '2':
        mode = 2;
        break;
      case '3':
        // Reset tổng tiền và chuyển về mode 2
        total_price = 0.0;
        mode = 2;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reset thanh cong");
        Serial.println("Reset thanh cong");
        delay(1000); // Hiển thị thông báo reset trong 1 giây
        break;
      default:
        Serial.println("Invalid command");
        break;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(mode);
    delay(1000); // Hiển thị chế độ hiện tại trong 1 giây
  }

  if (GM65.available()) {
    receivedString = GM65.readString();
    Serial.println(receivedString);
    receivedString.trim();

    if (mode == 1) {
      // Chức năng 1: Xem thông tin sản phẩm
      char query[128];
      sprintf(query, "SELECT name, price FROM products WHERE barcode='%s'", receivedString.c_str());
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(query);
      column_names *cols = cur_mem->get_columns();
      row_values *row = NULL;
      bool productFound = false;

      do {
        row = cur_mem->get_next_row();
        if (row != NULL) {
          productFound = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(receivedString);
          lcd.setCursor(0, 1);
          Serial.print(row->values[0]);  // In tên sản phẩm
          Serial.print(", ");
          Serial.println(row->values[1]); // In giá sản phẩm
          lcd.print(row->values[0]);
          lcd.print(", ");
          lcd.print(row->values[1]);
        }
      } while (row != NULL);

      if (!productFound) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(receivedString);
        lcd.setCursor(0, 1);
        Serial.println("Ma khong hop le");
        lcd.print("Ma khong hop le");
      }

      delete cur_mem;
    } else if (mode == 2) {
      // Chức năng 2: Tính tổng tiền của đơn hàng
      char query[128];
      sprintf(query, "SELECT name, price FROM products WHERE barcode='%s'", receivedString.c_str());
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(query);
      column_names *cols = cur_mem->get_columns();
      row_values *row = NULL;
      bool productFound = false;

      do {
        row = cur_mem->get_next_row();
        if (row != NULL) {
          productFound = true;
          total_price += atof(row->values[1]);

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Tong gia:");
          lcd.setCursor(0, 1);
          lcd.print(total_price);
          
          Serial.print("Tong gia: ");
          Serial.println(total_price);
        }
      } while (row != NULL);

      if (!productFound) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(receivedString);
        lcd.setCursor(0, 1);
        Serial.println("Ma khong hop le");
        lcd.print("Ma khong hop le");
      }

      delete cur_mem;
    }
  }
}
