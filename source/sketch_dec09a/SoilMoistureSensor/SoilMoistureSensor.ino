const int maxSoilMoisture = 520;
const int minSoilMoisture = 420;
void setup() 
{
  Serial.begin(9600);//Mở cổng Serial ở mức 9600
}
 
void loop() 
{
  int value = analogRead(A2);     // Ta sẽ đọc giá trị hiệu điện thế của cảm biến
  int percent = map(value, minSoilMoisture, maxSoilMoisture, 100, 0);
  if(percent <= 0) {
    percent = 0;
  }
  else if(percent>=100) {
    percent = 100;
  }
  delay(1000);
  Serial.println(value);
  Serial.print(percent);
  Serial.println('%');
  Serial.println();
}
