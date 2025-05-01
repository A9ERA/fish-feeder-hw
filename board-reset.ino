/**
 * www.arduinona.com
 * เขียนโค้ดสั่งรีเซ็ต (reset) บน ESP8266 และ ESP32 
 */

 int cnt = 10;           //ประกาศตัวแปรเป็น global 

 void setup() {
   Serial.begin(115200);
   delay(100);
   Serial.println("Program Started");
 }
 
 void loop() {
   
   Serial.println(cnt);   // แสดงค่าตัวเลขแต่ละรอบ
 
   // เมื่อรันครบ 10 รอบ (10วินาที) ให้สั่ง reset
   if (cnt == 0) {
     Serial.println("Reset..");
     ESP.restart();       // คำสั่งรีเซ็ต ESP
   }
 
   cnt--;                 // ลดค่าตัวแปร cnt ไปรอบละ 1 หน่วย 
   delay(1000);           // รอ 1 วินาที
 }