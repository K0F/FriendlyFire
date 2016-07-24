/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Charles Tuttle, 2016
 */


#define BUTT_TIME  500

char mesg[10];
int char_i =0;
long ser_timeo; // incoming serial timeout

long proj_adv; //holds time pressing advance button on projector

long now_t, prev_t, delta_t; // timekeepers
void setup() {
  for(int i=2;i<9;i++){pinMode(i, OUTPUT);}
  Serial.begin(115200);

}

void loop() {
  now_t = millis();
  delta_t = now_t - prev_t;

  if(proj_adv > 0){ // check for projector button press time
    proj_adv += delta_t;
    if(proj_adv > BUTT_TIME){digitalWrite(5, LOW); proj_adv = 0;}
  }

  ser_timeo += delta_t;
  if (char_i > 0 && ser_timeo > 50){char_i = 0;} // reset serial indx
  if (Serial.available() > 0) {
    mesg[char_i] = Serial.read();
    char_i++;
    ser_timeo =0;
  }

  if(char_i == 8 ){
    Serial.println("8");
    for(int i=0;i<10;i++){Serial.print(mesg[i]);}
    Serial.println("");
    char_i =0;
    activ();
  }

  prev_t = now_t; //time marches on
}// end loop

void activ(){
  for(int i=2,y=1;i<9;i++,y++){
    if(y == 4){if(mesg[y] =='1'){digitalWrite(5,HIGH);proj_adv =1;}}
    else{
      if(mesg[y] == '1'){digitalWrite(i, HIGH);}
      else if(mesg[y] == '2'){digitalWrite(i, LOW);}
    }
  }
}
