#include "U8glib.h"
#include <Wire.h>
U8GLIB_ST7920_128X64_4X u8g(13, 11, 10, 9);

void hello_world(){
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 11, "Hello");
  u8g.drawStr(0, 25, "World");
}

void draw_str(char * str){
  // TODO: safety check string
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 11, str);
}

void setup() {
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);


   u8g.firstPage();  
  do {
     hello_world();
  } while( u8g.nextPage() );

}

void loop() {
  // put your main code here, to run repeatedly:

}

void receiveEvent(int howMany) {
  Serial.print("Receiving ");
  Serial.println(howMany);
  char * i2c_str = (char *) malloc(sizeof(char) * howMany);
  int i = 0;
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    //Serial.print(c);         // print the character
    Serial.print(i);
    Serial.print(": ");
    Serial.println(c);
    if(c != '\0'){
      i2c_str[i] = c;
      i++;
    }
    
  }
  i2c_str[i] = '\0';
  int x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer
  Serial.println(i2c_str);
   u8g.firstPage();  
  do {
     draw_str(i2c_str);
  } while( u8g.nextPage() );
  
}
