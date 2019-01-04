#include <SPI.h>
#include <Ethernet.h>
#include "pitch.h"
#include <Servo.h>

Servo myservo;
// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 0, 100); // IP address, may need to change depending on network
IPAddress ip(10,222,237,58);
EthernetServer server(80);  // create a server at port 80

String HTTP_req;    // stores the HTTP request
char* current_place = "Living Room";

boolean led_status = false;   // state of LED, off by default
boolean gas_status = false; //state of Gas Sensor

int led = 9;
const int Room =9;
const int Rooftop= 10;

int brightness = 255;
int fadeAmount = 51;
const int ON = 0; //In PIN9, Led LOW is HIGH   
const int OFF = 1;

int gasPercent = 0;

int maxSoilMoisture = 520;
int minSoilMoisture = 420;
int soilMoisture = 0;
int soilMoisturePercent = 0;

int pos = 0;
int currentRainHumidity;

/*Speaker declecration*/
int speakerPin = 13;
int melody[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
int noteDurations[] = {16, 8, 8, 4,4,4,4,4 };

/*LED's functions*/
boolean isLedTurnOn(EthernetClient cl) {
    if(!led_status) {
        cl.println("<script>");
        cl.println("alert('Please turn on the light first!')");
        cl.println("</script>");
        return false;
    }
    else {return true;}
}

void turnLedOn() {
    //abcln("Turning on....");
    brightness = 0;
    digitalWrite(led, ON);
    analogWrite(led,0);
    led_status = !digitalRead(led); //led_status = TRUE 
}

void turnLedOff() {
    //abcln("Turning off....");
    brightness = 255;
    digitalWrite(led, OFF);
    analogWrite(led,255);
    led_status = !digitalRead(led); //led_Status = FALSE
}

/*Process Led Mode associated with GET request*/
void ledModeProcess() {
    
    if(HTTP_req.indexOf("GET /?mode=sleep")>=0) {
      if(!led_status) {turnLedOn();}
      brightness = 51;
      
    }
    else if(HTTP_req.indexOf("GET /?mode=chill")>=0) {
      if(!led_status) {turnLedOn();}
      brightness = 102;
  
    }
    else if(HTTP_req.indexOf("GET /?mode=focus")>=0) {
      if(!led_status) {turnLedOn();}
      brightness = 153;
      
    }  
    analogWrite(led,brightness);
}

void ledPlaceProcess() {
  if(HTTP_req.indexOf("GET /?place=room")>=0)  {
    led = Room;
    current_place = "Living Room"; 
  }
  
  if(HTTP_req.indexOf("GET /?place=roof")>=0)  {
    led = Rooftop;
    current_place = "Bedroom";
  }
  
  led_status = !digitalRead(led);
  if(led_status) {brightness=0;}
  else {brightness=255;}
  analogWrite(led,brightness);
  //abcln(led);

}
void LedProcess(EthernetClient cl)
{  
    if(HTTP_req.indexOf("GET /?place")>=0) {
      ledPlaceProcess();
    }
    //Turn on

    if(HTTP_req.indexOf("GET /?status=on")>=0) {

        turnLedOn();
 
    }
    //Turn off
    if(HTTP_req.indexOf("GET /?status=off")>=0) {

        turnLedOff();
 
    }
    //Increase brightness
    if(HTTP_req.indexOf("GET /?dimmer=increase")>=0) {
        if(0 == brightness) {
            cl.println("<script>");
            cl.println("alert('Reached the highest brightness!') ");
            cl.println("</script>");
            return;
        }
        else {
            if(isLedTurnOn(cl))
            {
                 brightness -= fadeAmount;
                 analogWrite(led,brightness);
                 //abcln("checkpoint-increase");
            }
        }
    }
    //Decrease brightness
    if(HTTP_req.indexOf("GET /?dimmer=decrease")>=0) {

        if(255 == brightness) {
            //abc("Lowest");
            cl.println("<script>");
            cl.println("alert('Reached the lowest brightness!') ");
            cl.println("</script>");
            return;
        }
        else {
                brightness += fadeAmount;
                analogWrite(led,brightness);
                //abcln("checkpoint");
        }
        
    }
    if(HTTP_req.indexOf("GET /?mode")>=0) {
      ledModeProcess();
    }
}

/*GAS'functions*/
void gasSwitchProccess(EthernetClient cl) {
    if(HTTP_req.indexOf("GET /?gas_status=on")>=0) {
        gas_status = true;
    }
    if(HTTP_req.indexOf("GET /?gas_status=off")>=0) {
        gas_status = false;
    }
}

void gasAlert() {
   for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
    noTone(speakerPin);
  }
}

void gasProcess(EthernetClient cl,int maxGas) {
  gasSwitchProccess(cl);
  gasPercent = analogRead(A1);
  if(gasPercent >= maxGas) {
       gasAlert();
//       cl.println("<script>");
//       cl.println("alert(\"DANGER!DANGER!DANGER!\") ");
//       cl.println("</script>");
       Serial.println("ALERT!ALERT!");
  }
  delay(1000);
  
}

/*Soil Moisture's functions*/
void soilProcess(EthernetClient cl) {
  soilMoisture = analogRead(A2);
  int percentSoilMoisture = map(soilMoisture, minSoilMoisture, maxSoilMoisture, 100,0);
  if(percentSoilMoisture<=0) {
    percentSoilMoisture = 0;
    cl.println("<script>");
    //cl.println("alert('Start Watering Plants!Pew Pew!') ");
    cl.println("</script>");
    /*
      Do something here
    */
  }
  else if(percentSoilMoisture>=100) {
    percentSoilMoisture = 100;
  }
  
  soilMoisturePercent = percentSoilMoisture;
  //abc(soilMoisture);
  delay(1000);
}

/*Rain Dectection's functions*/
void rainProcess(EthernetClient cl) {
    int rainHumidity = analogRead(A3);
    int currentPosition = myservo.read();
    //Phoi do
    if(rainHumidity <= 550 && currentPosition > 5) {
      for(pos = 99; pos >= 1; pos -= 1)  
              {                                  
                myservo.write(pos);              
                delay(15);                       
              } 
    }
    //Cat do
    else if(rainHumidity > 550 && currentPosition < 95) {
        for(pos = 0; pos < 98; pos += 1)  // goes from 0 degrees to 90 degrees 
        {                                  // in steps of 1 degree 
          myservo.write(pos);              // tell servo to go to position in variable   'pos' 
          delay(15);                       // waits 15ms for the servo to reach the position 
        } //

    }
     Serial.println(rainHumidity);
     Serial.println(myservo.read());
    //Move the servo
    
   delay(1000);
  currentRainHumidity = rainHumidity;
}

void setup()
{
    /*Get board IP Adress*/
        delay(1000);
        system("ifconfig -a > /dev/ttyGS0");
    
    Ethernet.begin(mac,ip);  // initialize Ethernet device
    //Ethernet.begin(mac);
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(Room, OUTPUT);    //LED 9
    pinMode(Rooftop, OUTPUT); // LED 10
    digitalWrite(Room, OFF);
    digitalWrite(Rooftop, OFF);
    Serial.println("Server address:");
    Serial.println(Ethernet.localIP());
    //Servo
   myservo.attach(11); 
}

void loop()
{
    EthernetClient client = server.available();  // try to get client
    if (client) {  // got client?
        Serial.println("Got the client");
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            Serial.println("Connecting to the web page...");
            //soilProcess(client); 
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received 
                if (c == '\n' && currentLineIsBlank) { 
                    Serial.println("Current line is blank");
                    //abcln(255-brightness);
                     //abcln("Connected");
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page 
                    client.println("<!DOCTYPE html>");
                    client.println("<html lange='en'>");
                    client.println("<head>");
                    client.println("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
                    client.println("<meta http-equiv='X-UA-Compatible' content='ie=edge'>");
//                    client.println("<meta http-equiv='refresh' content='30'> ");
                    client.println("<title>Smarty Ho</title>");
                    client.println("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/css/bootstrap.min.css'>");
                    client.println("<link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.6.3/css/all.css' integrity='sha384-UHRtZLI+pbxtHCWp1t77Bi1L4ZtiqrqD80Kn4Z8NTSRyMA2Fd33n5dQ8lWUE00s/' crossorigin='anonymous'>");
                    client.println("<style>");
                    client.println(".carousel-indicators-numbers li {text-indent: 0;margin:0 2px; width: 30px; height: 30px;border: none; border-radius: 100%; line-height: 30px; color: #fff; background-color: #999; -webkit-transition: all 0.25s ease; transition: all 0.25s ease; ");
                    client.println(".carousel-indicators-numbers li.active,.carousel-indicators-numbers li:hover{margin: 0 2px; width: 30px; hegiht: 30px; background-color: #28a745;}");
                    client.println("body{padding: 10px;}");
                    client.println("</style>");
                    client.println("<body>");
                    client.println("<main role='main' class='container-fluid'>");
                    /*Start to process*/
                    LedProcess(client);
                    gasProcess(client,300);
                    soilProcess(client);
                    rainProcess(client);
                    client.println("<header class='site-header'>");
                    client.println("</header>");
                    client.println("<div class='content-area fw'>");
                    
                    client.println("<ul class='nav nav-pills'>");
                    client.println("<li class='nav-item active'>");
                    client.println("<a class='nav-link' href='#light' data-toggle='tab'>Light Control</a>");
                    client.println("</li>");
                    client.println("<li class='nav-item' >");
                    client.println("<a class='nav-link' href='#fire' data-toggle='tab'>Fire Alarm</a>");
                    client.println("</li>");
                    client.println("<li class='nav-item' >");
                    client.println("<a class='nav-link' href='#rain' data-toggle='tab'>Rain Detection</a>");
                    client.println("</li>");
                    client.println("<li class='nav-item' >");
                    client.println("<a class='nav-link' href='#plant' data-toggle='tab'>Plant Watering</a>");
                    client.println("</li>");
                    client.println("</ul>");
                    client.println("<div class='tab-content clearfix' data-interval='false'>");
                    client.println("<div id='light' class='main-content tab-pane fade show active'>");
                    client.println("<div class='main-content__body'>");
                    client.println("<div id='light-control-carousel' class='carousel slide' data-ride='carousel' data-interval='false'>");
                    client.println("<div class='carousel-inner'>");
                    client.println("<div class='row carousel-item active'>");
                    client.println("<div class='jumbotron row'>");
                    client.println("<div class='row ml-auto align-items-end pd-2 justify-content-center'>");
                    client.println("<div class='col-md-4 align-middle text-center'>");
                    client.println("<form method='GET'>");
                    client.println("<button type='submit' class='btn btn-outline-dark btn-md rounded-circle' role='button' name='mode' value='sleep' data-toggle='tooltip' data-placement='top' data-html='true' title='<span>Sleep mode</span>' data-animation='true'> <i class='fas fa-moon fa-xs'></i></button>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("<div class='col-md-4 align-middle text-center'>");
                    client.println("<form method='GET'>");
                    client.println("<button type='submit' class='btn btn-outline-warning btn-md rounded-circle' role='button' name='mode' value='chill' data-toggle='tooltip' data-placement='top' data-html='true' title='<span>Chilling mode</span>'>");
                    client.println("<i class='fas fa-music fa-xs'></i></button>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("<div class='col-md-4 align-middle text-center'>");
                    client.println("<form method='GET'>");
                    client.println("<button class='btn btn-outline-info btn-md rounded-circle' role='button' name='mode' value='focus' data-toggle='tooltip' data-placement='top' data-html='true' title='<span>Focus mode</span>' data-animation='true'>");
                    client.println("<i class='fas fa-book-reader fa-xs'></i></button>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");

                    client.println("<div class='col-md-12 col-12'>");
                    client.println("<h1 class='display-4'>Light Control</h1>");
                    client.println("</div>");
                    client.println("<div class=' col-md-4 p-4'>");
                    client.println("<h4>Light Switch</h4>");
                    client.println(led_status ? "<p class='lead'>Your light is <b class='text-success'>ON</b></p>" : "<p class='lead'>Your light is <b class='text-danger'>OFF</b></p>");
                    client.println("<hr class='my-4'>");
                    client.println("<p>Press the button to turn the light on or turn it off.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='btn-group'>");
                    client.println("<button type='submit' class='btn btn-outline-success btn-lg' name='status' value='on' role='button'>ON ");
                    client.println("</button>");
                    client.println("<button type='submit' class='btn btn-outline-success btn-lg' name='status' value='off' role='button'>OFF ");
                    client.println("</button>");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("<div class='col-md-4 p-4'>");
                    client.println("<h4>Light Brightness </h4>");
                    client.println("<p class='lead'>Current brightness: <b class='text-info'>");
                    client.println(255 - brightness);
                    client.println("</b></p>");
                    client.println("<hr class='my-4'>");
                    client.println("<p>Adjust brightness to fit with your circumstance.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='btn-group'>");
                    client.println("<button type='submit' class='btn btn-outline-primary btn-lg' name='dimmer' value='increase' role='button'>+</button>");
                    client.println("<button type='submit' class='btn btn-outline-primary btn-lg' name='dimmer' value='decrease' role='button'>-</button>");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("<div class='col-md-4 p-4'>");
                    client.println("<h4>Lights Location</h4>");
                    /*TO-DO: Get the current color*/
                    client.println("<p class='lead'>Current room: <b class='text-info'>");
                    client.println(current_place);
                    client.println("</b></p>");
                    client.println("<hr class='my-4'>");
                    client.println("<p>Choose which lights to control.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='btn-group'>");
                    client.println("<button class='btn btn-outline-danger btn-lg' role='button' name='place' value='room'>Living Room</button>");
                    client.println("<button class='btn btn-outline-success btn-lg' role='button' name='place' value='roof'>Bedroom</button>");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");

                    client.println("<div class='carousel-item'>");
                    client.println("<div class='jumbotron'>");
                    client.println("<h4>Advices</h4>");
                    client.println("<p>ABC</p>");
                    client.println("<p>ABC</p>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<ol class='carousel-indicators carousel-indicators-numbers'>");
                    client.println("<li data-target='#light-control-carousel' data-slide-to='0' class='active'>Controls</li>");
                    client.println("<li data-target='#light-control-carousel' data-slide-to='1'>Advices</li>");
                    client.println("</ol>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");

                    client.println("<div id='fire' class='main-content tab-pane fade'>");
                    client.println("<div class='main-content__body'>");
                    client.println("<div id='fire-alarm-carousel' class='carousel slide' data-ride='carousel' data-interval='false'>");
                    client.println("<div class='carousel-inner'>");
                    client.println("<div class='carousel-item active'>");
                    client.println("<div class='jumbotron row'>");
                    client.println("<div class='col-md-12 col-12'>");
                    client.println("<h1 class='display-4'>Fire Alarm Control</h1>");
                    client.println("</div>");
                    client.println("<div class='cold-md-3 col-3'>");
                    client.println("<h4>Gas Status</h4>");
                    client.println("<p class='lead'>Current Gas Percentage: <b class='text-success' data-toggle='tooltip' data-placement='right' data-html='true' title='<span>You are safe.</span>' data-animation='true'>");
                    client.println(gasPercent);
                    client.println("</b></p>");
                    client.println("</div>");
                    client.println("<div class='col-md-4 col-4 offset-md-2'>");
                    client.println("<p>Adjust maximum gas percentage to fire the alarm.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='form-group'>");
                    client.println("<div class='input-group input-group-lg mb-3'>");
                    client.println("<input type='text' class='form-control' placeholder='Maximum Gas Percentage' aria-label='Maximum Gas Percentage' aria-describedby='button-addon2' name='gas'>");
                    client.println("<div class='input-group-append'>");
                    client.println("<button type='submit' class='btn btn-outline-secondary' id='button-addon2'>Set</button>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<div class='carousel-item'>");
                    client.println("<div class='jumbotron'>");
                    client.println("<h4>Advices</h4>");
                    client.println("<p></p>");
                    client.println("<p></p>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<ol class='carousel-indicators carousel-indicators-numbers'>");
                    client.println("<li data-target='#fire-alarm-carousel' data-slide-to='0' class='active'>Controls</li>");
                    client.println("<li data-target='#fire-alarm-carousel' data-slide-to='1'>Advices</li>");
                    client.println("</ol>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");

                    client.println("<div id='plant' class='tab-pane fade'>");
                    client.println("<div class='main-content__body'>");
                    client.println("<div id='plant-carousel' class='carousel slide' data-ride='carousel' data-interval='false'>");
                    client.println("<div class='carousel-inner'>");
                    client.println("<div class='carousel-item active'>");
                    client.println("<div class='jumbotron row'>");
                    client.println("<div class='col-md-12 col-12'>");
                    client.println("<h1 class='display-4'>Plant Watering</h1>");
                    client.println("</div>");
                    client.println("<div class='col-md-3 col-3 border-right'>");
                    client.println("<h4>Soil Hudmidity</h4>");
                    client.println("<p class='lead'>Current soil hudmidity: <b class='text-success' data-toggle='tooltip' data-placement='right' data-html='true' title='<span>You are safe.</span>'data-animation='true'>");
                    client.println(soilMoisturePercent);
                    client.println("</b></p>");
                    client.println("</div>");
                    client.println("<div class='col-md-3 col-3 '>");
                    client.println("<h4>Emergency</h4>");
                    client.println("<p>One hit to water the plants.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='form-group'>");
                    client.println("<div class='input-group input-group-lg mb-3'>");
                    client.println("<button type='submit' class='btn btn-outline-danger btn-lg' name='takeit' value='true'>DO IT</button>");
                    client.println("</div");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<div class='carousel-item'>");
                    client.println("<div class='jumbotron'>");
                    client.println("<h4>Advices</h4>");
                    client.println("<p>ABC</p>");
                    client.println("<p>ABC</p>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<ol class='carousel-indicators carousel-indicators-numbers'>");
                    client.println("<li data-target='#plant-carousel' data-slide-to='0' class='active'>Controls</li>");
                    client.println("<li data-target='#plant-carousel' data-slide-to='1'>Advices</li>");
                    client.println("</ol>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    
                    client.println("<div id='rain' class='tab-pane fade'>");
                    client.println("<div class='main-content__body'>");
                    client.println("<div id='rain-carousel' class='carousel slide' data-ride='carousel' data-interval='false'>");
                    client.println("<div class='carousel-inner'>");
                    client.println("<div class='carousel-item active'>");
                    client.println("<div class='jumbotron row'>");
                    client.println("<div class='col-md-12 col-12'>");
                    client.println("<h1 class='display-4'>Rain Detection</h1>");
                    client.println("</div>");
                    client.println("<div class='col-md-3 col-3 border-right'>");
                    client.println("<h4>Air Hudmidity</h4>");
                    client.println("<p class='lead'>Current air hudmidity: <b class='text-success' data-toggle='tooltip' data-placement='right' data-html='true' title='<span>You are safe.</span>'data-animation='true'>");
                    client.println(currentRainHumidity);
                    client.println("</b></p>");
                    client.println("</div>");
                    client.println("<div class='col-md-3 col-3 '>");
                    client.println("<h4>Emergency</h4>");
                    client.println("<p>One hit to hide the clothes from the rain.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='form-group'>");
                    client.println("<div class='input-group input-group-lg mb-3'>");
                    client.println("<button type='submit' class='btn btn-outline-danger btn-lg' name='take_clothes' value='true'>DO IT</button>");
                    client.println("</div");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<div class='carousel-item'>");
                    client.println("<div class='jumbotron'>");
                    client.println("<h4>Advices</h4>");
                    client.println("<p>ABC</p>");
                    client.println("<p>ABC</p>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<ol class='carousel-indicators carousel-indicators-numbers'>");
                    client.println("<li data-target='#rain-carousel' data-slide-to='0' class='active'>Controls</li>");
                    client.println("<li data-target='#rain-carousel' data-slide-to='1'>Advices</li>");
                    client.println("</ol>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    
                    client.println("</div>"); //div.content-fuild

                    client.println("<footer class='site-footer'>");
                    client.println("</footer>");
                    client.println("<main>");
                    client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>");
                    client.println("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/js/bootstrap.min.js'></script>");
                    client.println("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/js/bootstrap.bundle.min.js'></script>");
                    client.println("<script type='text/javascript'>");
                    client.println("$(function () {");
                    client.println("$('[data-toggle=\\\"tooltip\\\"]').tooltip()");
                    client.println("})");
                    client.println("</script>");
                    client.println("</body>");
                    client.println("</html>");
                    ////abc(HTTP_req);
                    // Finished with request, empty string
                    
                    break;
                }
                //Every line of text received from the client ends with \r\n
                if (c == '\n') {
                    Serial.println("Current line is blank");
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                    HTTP_req = "";
                }
                else if (c == '\r') {
                    // a text character was received from client
                    Serial.println("C == '\r' ");
                }
                
                else {
                  currentLineIsBlank = false;
                }

            } // end if (client.available())
        } // end while (client.connected())
        //Give the web time to response
        delay(1);
        //Close the connection     
        client.stop();
    } // end if (client)
}


