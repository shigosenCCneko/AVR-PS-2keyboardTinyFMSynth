#include "fmtone_def.h"
#include "FMTONE.h"
#include "display.h"

#include "changeParameter.h"
#include <avr/pgmspace.h>
#include <EEPROM.h>

extern FmOperator fm_operator[ ];

uint8_t conv_time(uint8_t);
int cut_limit(int,int);

extern uint8_t default_tone[MAX_DEFAULT_TONE][17];


int cut_limit(int i,int max_val){
  if(i < 0) return 0;
  if(i > max_val) return max_val;
  return i;
}


void change_atk(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 1 + 8 * opno;
  int i = default_tone[prg][adr];
  i  =  cut_limit(i+ val,15);
//  i += val;
//  if (i < 1) i = 1;
//  if (i > 15) i = 15;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_atk(adr, i);
  }
}


void change_decy(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 2 + 8 * opno;
  int i = default_tone[prg][adr];
  i  =  cut_limit(i+ val,15);
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_decy(adr, i);
  }
}

void change_sul(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 3 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,15);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 15) i = 15;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_sul(adr, i);
  }
}


void change_sus(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 4 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,15);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 15) i = 15;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_sus(adr, i);
  }
}




void change_rel(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 5 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,15);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 15) i = 15;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_rel(adr, i);
  }
}


void change_tl(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 6 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,31);
//  i -= val;
//  if (i < 0) i = 0;
//  if (i > 31) i = 31;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_tl(adr, 31 - i);
  }
}

void change_mul(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 7 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,15);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 15) i = 15;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.change_mul(adr, i);
  }
}

void change_wave(uint8_t prg, uint8_t opno, int val) {

  uint8_t  adr = 8 + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,7);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 7) i = 7;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 + opno;
    FMTONE.set_wave(adr, i);
  }
}

void change_fb(uint8_t prg, uint8_t opno, int val) {
  uint8_t  adr =  + 8 * opno;
  int i = default_tone[prg][adr];
    i  =  cut_limit(i+ val,7);
//  i += val;
//  if (i < 0) i = 0;
//  if (i > 7) i = 7;
  default_tone[prg][adr] = i;
  for (int j = 0; j < MAX_MIDITONE; j++) {
    adr = j * 2 ;
    FMTONE.change_fb(adr & 0xfe , i & 0x07);
  }
}


void save_eeprom(){
  char c;
  int adr = 0;
  for(int i = 0; i < MAX_DEFAULT_TONE;i++){
    for(int j = 0; j < 17;j++){
     c = default_tone[i][j];
     EEPROM.write(adr++,c);
  }
}
disp_saved();
}

void load_eeprom(){
  char c;
  int adr = 0;
   for(int i = 0; i < MAX_DEFAULT_TONE;i++){
    for(int j = 0; j < 17;j++){
     c = EEPROM.read(adr++);
     default_tone[i][j] = c;
     
  }
} 
disp_loaded();
}
  








