#include "U8glib.h"
#include <Wire.h>
U8GLIB_ST7920_128X64_4X u8g(13, 11, 10, 9);

char days_of_week[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char months_of_year[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "aug", "Sep", "Oct", "Nov", "Dec"};
char * station;
char * artist;
char * title;
enum text_dir { LEFT, RIGHT};
int station_x_pos = 0;
int artist_x_pos = 0;
int title_x_pos = 0;
short station_dir = LEFT;
short artist_dir = LEFT;
short title_dir = LEFT;

typedef struct DATETIME {
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t second;    
} Date_Time;

Date_Time dt;

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
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_idle(){
    u8g.setFont(u8g_font_unifont);

    u8g.drawCircle(64, 24, 24);
    float hours = dt.hour + (dt.min/60.0);
    float mins = dt.min;
    float hoursDeg = mapfloat(hours, 0, 12, -1.57, 4.712);
    float minsDeg = mapfloat(mins, 0, 60, -1.57, 4.712);
    int hourHandX = 64 + (int)floor(15.0 * cos(hoursDeg));
    int hourHandY = 24 + (int)floor(15.0 * sin(hoursDeg));
    int minsHandX = 64 + (int)floor(19.0 * cos(minsDeg));
    int minsHandY = 24 + (int)floor(19.0 * sin(minsDeg));
    
    u8g.drawLine(64,24,hourHandX, hourHandY);
    u8g.drawLine(64,24,minsHandX, minsHandY);

    //u8g.setFont(u8g_font_osr35);
    char hh[3];
    char MM[3];
    bool pm = false;
    int hour = dt.hour;
    if(dt.hour > 11){
      pm = true;
      if(dt.hour > 12){
        hour = dt.hour - 12;
      }
    }
    sprintf(hh, "%02d", hour);
    sprintf(MM, "%02d", dt.min);
    u8g.drawStr(12, 30, hh);
    u8g.drawStr(128-12-16, 30, MM);
    if(pm == true){
      u8g.drawStr(128-12-16, 48, "PM");
    } else {
      u8g.drawStr(128-12-16, 48, "AM");
    }

    char dom[3];
    char year_str[5];
    sprintf(dom, "%02d", dt.day_of_month);
    sprintf(year_str, "%d", dt.year);
    u8g.drawStr(4, 64, days_of_week[dt.day_of_week-1]);
    u8g.drawStr(36, 64, dom);
    u8g.drawStr(60, 64, months_of_year[dt.month-1]);
    u8g.drawStr(92, 64, year_str);
}

void update_station(){
  int station_len = strlen(station);
  if(station_len < 16){
    station_x_pos = ((16-station_len) / 2) * 8;
  }
  else if (station_len == 16){
    station_x_pos = 0;
  } else {
    if(station_x_pos == 0){
        station_x_pos = -1;        
    } else if(station_x_pos < 0){
      if(station_dir == LEFT){
        station_x_pos -= 1;  
      } else {
        station_x_pos += 1;
      }
    }
    if(station_x_pos < (station_len * 8) - 128){
      station_x_pos += 1;
      station_dir = RIGHT;
    }
  }
}

void update_artist(){
  int artist_len = strlen(artist);
  if(artist_len < 16){
    artist_x_pos = ((16-artist_len) / 2) * 8;
  }
  else if (artist_len == 16){
    artist_x_pos = 0;
  } else {
    if(artist_x_pos == 0){
        artist_x_pos = -1;        
    } else if(artist_x_pos < 0){
      if(artist_dir == LEFT){
        artist_x_pos -= 1;  
      } else {
        artist_x_pos += 1;
      }
    }
    if(artist_x_pos < (artist_len * 8) - 128){
      artist_x_pos += 1;
      artist_dir = RIGHT;
    }
  }
}

void update_title(){
  int title_len = strlen(title);
  if(title_len < 16){
    title_x_pos = ((16-title_len) / 2) * 8;
  }
  else if (title_len == 16){
    title_x_pos = 0;
  } else {
    Serial.println(title_x_pos);
    if(title_x_pos == 0){
        title_dir = LEFT;
        title_x_pos = -1;        
    } else if(title_x_pos < 0){
      if(title_dir == LEFT){
        title_x_pos -= 1;  
      } else {
        title_x_pos += 1;
      }
    }
    if(title_x_pos < -((title_len * 8) - 128)){
      Serial.println("Going back again");
      title_x_pos += 1;
      title_dir = RIGHT;
    }
  }
}

void draw_playing(){
  u8g.setFont(u8g_font_unifont);
  update_station();
  update_artist();
  update_title();  
  u8g.drawStr(station_x_pos, 12, station);
  u8g.drawStr(artist_x_pos, 26, artist);
  u8g.drawStr(title_x_pos, 40, title);
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

  setTime("4-17-01-2019-19-44-33");
  station = (char *) malloc(sizeof(char) * 9);
  artist = (char *) malloc(sizeof(char) * 9);
  title = (char *) malloc(sizeof(char) * 34);
  strcpy(station, "Triple J");  
  strcpy(artist, "deadmau5");
  strcpy(title, "Ghosts 'n' Stuff (feat Rob Swire)");
}

void loop() {
  // put your main code here, to run repeatedly:
  u8g.firstPage();  
  do {
   draw_playing();
  } while( u8g.nextPage() );
  delay(200);

}

void setTime(char * timeStr){
  //d-dd-mm-yyyy-hh-mm-ss
  char dom[2];
  char dd[3];
  char mm[3];
  char yyyy[5];
  char hh[3];
  char MM[3];
  char ss[3];
  memcpy(dom, timeStr, 1);
  memcpy(dd, timeStr+2, 2);
  memcpy(mm, timeStr+5, 2);
  memcpy(yyyy, timeStr+8, 4);
  memcpy(hh, timeStr+13, 2);
  memcpy(MM, timeStr+16, 2);
  memcpy(ss, timeStr+19, 2);
  dom[1] = '\0';
  dd[2] = '\0';
  mm[2] = '\0';
  yyyy[4] = '\0';
  hh[2] = '\0';
  MM[2] = '\0';
  ss[2] = '\0';
  dt.day_of_week = atoi(dom);
  dt.day_of_month = atoi(dd);
  dt.month = atoi(mm);
  dt.year = atoi(yyyy);
  dt.hour = atoi(hh);
  dt.min = atoi(MM);
  dt.second = atoi(ss);
  Serial.print("day of week ");
  Serial.println(dt.day_of_week);
  Serial.print("day of month ");
  Serial.println(dt.day_of_month);
  Serial.print("month ");
  Serial.println(dt.month);
  Serial.print("year ");
  Serial.println(dt.year);
  Serial.print("hour ");
  Serial.println(dt.hour);
  Serial.print("min ");
  Serial.println(dt.min);
  Serial.print("second ");
  Serial.println(dt.second);
  
}

void handleI2CString(char *i2c_str){
  char cmd[5];
  char time_str[22];
  memcpy(cmd, i2c_str, 4);
  cmd[4] = '\0';
  if(strcmp(cmd, "clki") == 0){
    memcpy(time_str, i2c_str+5, 21);
    time_str[21] = '\0';
    //Serial.println(time_str);
    setTime(time_str);
    u8g.firstPage();  
    do {
      draw_idle();
    } while( u8g.nextPage() );
  } else if(strcmp(cmd, "clkp") == 0){
    memcpy(time_str, i2c_str+5, 21);
    time_str[21] = '\0';
    //Serial.println(time_str);
    setTime(time_str);
  }
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
  handleI2CString(i2c_str);
}
