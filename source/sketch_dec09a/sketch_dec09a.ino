#include <SPI.h>
#include <Ethernet.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 100); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String HTTP_req;          // stores the HTTP request
boolean led_status = false;   // state of LED, off by default

int led = 9;
int brightness = 0;
int fadeAmount = 51;

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
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Smarty Ho</title>");
                    //bootstrap 4.0
                    client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
                    client.println("</head>");
                    //main style
                    client.println("<style>");

                    client.println("</style>");  
                    client.println("<body class=\"bg-light\">");
                    client.println("<main role=\"main\" class=\"container\">");
                    client.println("<div class=\"control\">");
                        
                        //Process the state of LED
                        LedProcess(client);
                        client.println("<div class=\"card\">");
                            client.println("<div class=\"card-header\">Light Control</div>");
                            client.println("<div class=\"card-body\">");
                                client.println("<div class=\"card-title\"><h4>Light Switch</h4></div>");
                                client.println("<p class=\"card-text\">Turn on or turn off the light.</p>");
                                client.println("<p class=\"card-text\"> \
                                     Current status:");
                                client.println(led_status ? "<span class=\"text-success\">ON</span>" : "<span class=\"text-danger\">OFF</span>");
                                client.println("<\p>");
                                client.println("<form method=\"get\">");
                                  client.println("<p> <input type=\"submit\" name=\"status\" value=\"on\" class=\"btn btn-success\">");
                                  client.println("<input type=\"submit\" name=\"status\" value=\"off\" class=\"btn btn-danger\"></p>");
                                client.println("</form>");
                            client.println("</div>");
                            client.println("<hr/>");
                            client.println("<div class=\"card-body\">");
                                  client.println("<div class=\"card-title\"><h4>Light Dimmer</h4></div>");
                                  client.println("<p class=\"card-text\">Increase or decrease light brightness.</p>");
                                  client.println("<p class=\"card-text\"> \
                                     Current brightness:");
                                  client.println("<span class=\"text-info\">");
                                  client.println(brightness);
                                  client.println("</span>");
                                  client.println("<\p>");
                                  client.println("<form method=\"get\">");
                                      client.println("<p> <input type=\"submit\" name=\"dimmer\" value=\"increase\" class=\"btn btn-light\">");
                                      client.println("<input type=\"submit\" name=\"dimmer\" value=\"decrease\" class=\"btn btn-dark\"></p>");
                                  client.println("</form>");
                           client.println("</div>");//bod
                   
                        client.println("</div>");                        
                    client.println("</div>"); //light control

                    client.println("</main>"); //container
                    client.println("</body>");
                    client.println("</html>");
                    Serial.print(HTTP_req);
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

void LedProcess(EthernetClient cl)
{
     int led = 9;
    Serial.println("Current brightness:");
    Serial.println(brightness);
    //Turn on
    if(HTTP_req.indexOf("GET /?status=on")>=0) {
       led_status = true;
       brightness = 255;
       analogWrite(led,brightness);
       digitalWrite(led, HIGH);
    }
    //Turn off
    if(HTTP_req.indexOf("GET /?status=off")>=0) {
       led_status = false;
       brightness = 0;
       digitalWrite(led, LOW);
       analogWrite(led,brightness);
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
           brightness = brightness + fadeAmount;
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
           brightness = brightness - fadeAmount;
         }
       }
       analogWrite(led,brightness);
    }
}

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
