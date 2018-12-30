#include <SPI.h>
#include <Ethernet.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 0, 100); // IP address, may need to change depending on network
IPAddress ip(192,168,1,104);
EthernetServer server(80);  // create a server at port 80

String HTTP_req;          // stores the HTTP request
boolean led_status = false;   // state of LED, off by default

int led = 9;
int brightness = 0;
int fadeAmount = 51;

boolean isLedTurnOn(EthernetClient cl) {
    if(!led_status) {
        cl.println("<script>");
        cl.println("alert(\"Please turn on the light first!\") \
           ");
        cl.println("</script>");
        return false;
    }
    else {return true;}
}

int increaseBrightness(int brightness, int fadeAmount) {
    return  brightness += fadeAmount;
}

int decreaseBrightness(int brightness, int fadeAmount) {
    return brightness -= fadeAmount;
}

void turnLedOn(int led = 9) {
    led_status = true;
    brightness = 255;
    analogWrite(led,brightness);
    digitalWrite(led, HIGH);
}

void turnLedOff(int led = 9) {
    led_status = false;
    brightness = 0;
    digitalWrite(led, LOW);
    analogWrite(led,brightness);
}

void ledModeProcess(char* mode) {
    if("sleep" == mode) {
      if(!led_status) {turnLedOn(led);}
      brightness = 51;
    }

    else if ("chill" == mode) {
      if(!led_status) {turnLedOn(led);}
      brightness = 102;
    }

    else if("focus" == mode) {
      if(!led_status) {turnLedOn(led);}
      brightness = 153;
    }
}

void LedProcess(EthernetClient cl)
{
    Serial.println("Current brightness:");
    Serial.println(brightness);
    //Turn on
    if(HTTP_req.indexOf("GET /?status=on")>=0) {
        turnLedOn(led);
    }
    //Turn off
    if(HTTP_req.indexOf("GET /?status=off")>=0) {
        turnLedOff(led);
    }
    //Increase brightness
    if(HTTP_req.indexOf("GET /?dimmer=increase")>=0) {
        if(255 == brightness) {
            cl.println("<script>");
            cl.println("alert(\"Reached the highest brightness!\") ");
            cl.println("</script>");
        }
        else {
            if(isLedTurnOn(cl))
            {
                increaseBrightness(brightness, fadeAmount);
            }
        }
        analogWrite(led,brightness);
    }
    //Decrease brightness
    if(HTTP_req.indexOf("GET /?dimmer=decrease")>=0) {

        if(brightness == 0 && led_status) {
            cl.println("<script>");
            cl.println("alert(\"Reached the lowest brightness!\") ");
            cl.println("</script>");
        }
        else {

            if(isLedTurnOn(cl))
            {
                decreaseBrightness(brightness, fadeAmount);
            }
        }
        analogWrite(led,brightness);
    }
}

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(led, OUTPUT);       // LED on pin 2
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
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
                    client.println("<title>Smarty Ho</title>");
                    client.println("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.2.1/css/bootstrap.min.css'>");
                    client.println("<link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.6.3/css/all.css' integrity='sha384-UHRtZLI+pbxtHCWp1t77Bi1L4ZtiqrqD80Kn4Z8NTSRyMA2Fd33n5dQ8lWUE00s/' crossorigin='anonymous'>");
                    client.println("<link rel='stylesheet' href='main.css'>");
                    client.println("<style>");
                    client.println("</style>");
                    client.println("<body>");
                    client.println("<main role='main' class='container-fluid'>");
                    client.println("<header class='site-header'>");
                    client.println("</header>");
                    client.println("<div class='content-area fw'>");
                    client.println("<ul class='nav nav-pills'>");
                    client.println("<li class='active'>");
                    client.println("<a href='#light' data-toggle='tab'>Light Control</a>");
                    client.println("</li>");
                    client.println("<li>");
                    client.println("<a href='#fire' data-toggle='tab'>Fire Alarm</a>");
                    client.println("</li>");
                    client.println("<li>");
                    client.println("<a href='#rain' data-toggle='tab'>Rain Detection</a>");
                    client.println("</li>");
                    client.println("<li>");
                    client.println("<a href='#plant' data-toggle='tab'>Plant Watering</a>");
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
                    client.println("<p class='lead'>Your light is <b class='text-success'>ON</b></p>");
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
                    client.println("<div class='col-md-4 p-4'");
                    client.println("<h4>Light Brightness</h4>");
                    client.println("<p class='lead'>Current brightness: <b class='text-info'>255</b></p>");
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
                    client.println("<h4>Light Color</h4>");
                    client.println("<p class='lead'>Current color: <b class='text-danger'>RED</b></p>");
                    client.println("<hr class='my-4'>");
                    client.println("<p>Change light color to fill the mood.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='btn-group'>");
                    client.println("<button class='btn btn-outline-danger btn-lg' href='#' role='button' name='color' value='red'>RED</button>");
                    client.println("<button class='btn btn-outline-success btn-lg' href='#' role='button' name='color' value='green'>GREEN</button>");
                    client.println("<button class='btn btn-outline-primary btn-lg' href='#' role='button' name='color' value='blue'>BLUE</button>");
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
                    client.println("<p class='lead'>Current Gas Percentage: <b class='text-success' data-toggle='tooltip' data-placement='right' data-html='true' title='<span>You are safe.</span>' data-animation='true'>5%</b></p>");
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

                    client.println("<div id='rain' class='tab-pane fade'>");
                    client.println("<div class='main-content__body'>");
                    client.println("<div id='rain-carousel' class='carousel slide' data-ride='carousel' data-interval='false'>");
                    client.println("<div class='carousel-inner'>");
                    client.println("<div class='carousel-item active'>");
                    client.println("<div class='jumbotron row'>");
                    client.println("<div class='col-md-12 col-12'>");
                    client.println("<h1 class='display-4'>Rain Prediction Control</h1>");
                    client.println("<div>");
                    client.println("<div class='col-md-3 col-3 border-right'>");
                    client.println("<h4>Air Hudmidity</h4>");
                    client.println("<p class='lead'>Current air hudmidity: <b class='text-success' data-toggle='tooltip' data-placement='right' data-html='true' title='<span>You are safe.</span>'data-animation='true'>5%</b></p>");
                    client.println("</div>");
                    client.println("<div class='col-md-5 col-5 border-right'>");
                    client.println("<h4>Controls</h4>");
                    client.println("<p>Adjust maximum air hudmidity to hide the clothes.</p>");
                    client.println("<form method='GET'>");
                    client.println("<div class='form-group'>");
                    client.println("<div class='input-group input-group-lg mb-3'>");
                    client.println("<input type='text' class='form-control' placeholder='Maximum Air Humidity Percentage' aria-label='Maximum Air Humidity Percentage' aria-describedby='button-rain' name='humid'>");
                    client.println("<div class='input-group-append'>");
                    client.println("<button type='submit' class='btn btn-outline-secondary' id='button-rain'>Set</button>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("<div class='col-md-3 col-3 '>");
                    client.println("<h4>Emergency</h4>");
                    client.println("<p>One hit to hide clothes from the rain.</p>");
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
                    client.println("<li data-target='#rain-carousel' data-slide-to='0' class='active'>Controls</li>");
                    client.println("<li data-target='#rain-carousel' data-slide-to='1'>Advices</li>");
                    client.println("</ol>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");

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

                    Serial.println("Connected");
                    //Serial.print(HTTP_req);
                    // Finished with request, empty string
                    HTTP_req = "";
                    break;
                }
                //Every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // a text character was received from client
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

