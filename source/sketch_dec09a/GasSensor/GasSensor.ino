const int MAXGAS = 300;
void setup() 
{
  Serial.begin(9600);//Mở cổng Serial ở mức 9600
}
 
void loop() 
{
  int value = analogRead(A1);     // Ta sẽ đọc giá trị hiệu điện thế của cảm biến
  if(value>=MAXGAS) {
      Serial.println("GAS ALERT");
  }
  delay(1000);
 Serial.println(value);

}
