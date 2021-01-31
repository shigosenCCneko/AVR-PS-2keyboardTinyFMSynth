#include <avr/io.h>
#include "display.h"
#include "FMTONE.h"
#include "changeParameter.h"
#include <avr/pgmspace.h>

int posx[2][8];
int posy[2][8];

const char op1[] PROGMEM = "OP1";
const char op2[] PROGMEM = "OP2";

const char save_text[] PROGMEM = "Save=F2";
const char load_text[] PROGMEM = "Load=F1";
const char mul_mes[] PROGMEM = "ML=";






extern FmOperator fm_operator[ ];


void drawOperatorWave(uint8_t no) {

  signed char  *rp = no == 0 ?  fm_operator[0].wave_tbl : fm_operator[1].wave_tbl ;
  int ofs = no == 0 ? 190 : 70;
  SetColor(BLACK);
  MoveTo(1, ofs - 29);
  FillRect(72, 49);

  SetColor(YELLOW);

  for (int i = 3; i < 60; i = i + 4) {
    PlotPoint(i, ofs);
  }
  SetColor(GREEN);
  for (uint8_t x = 0; x < 64; x++) {
    PlotPoint(x / 2 + 15, (rp[x] >> 1) / 2 + ofs);
  }


}

void DispForm() {


  SetColor(GREEN);
  drawOperatorWave(0);
  drawOperatorWave(1);

  SetColor(BLUE);
  MoveTo(80, 15);
  FillRect(158, 80);
  MoveTo(80, 135);
  FillRect(158, 80);
  drawEnvelope(0, WHITE);
  drawEnvelope(1, WHITE);
  drawMul(0);
  drawMul(1);

}



uint8_t conv_time(uint8_t val) {
  for (int i = 0; i < 16; i++) {
    if (val == envelope_cnt[i]) {
      return envelope_time[i];
    }
  }
  return 1;
}


void drawEnvelope(uint8_t no, int color) {
  int yofs = no == 0 ?    135 : 15;
  int xofs = 80;
  int xpos = 80;
  int x, y;
  int cnt = 1;
  const int xdiv = 1;
  int tlv;

  signed char  *rp = no == 0 ?  fm_operator[0].wave_tbl : fm_operator[1].wave_tbl ;
  FmOperator *op = no == 0 ? &(fm_operator[0]) : &(fm_operator[1]);

  //  SetColor(BLUE);
  //  MoveTo(80,yofs);
  //  FillRect(158,80);
  SetColor(BLUE);
  x = posx[no][1];
  y = posy[no][1];
  MoveTo(x, y);
  for (int i = 2; i < posx[no][0]; i++) {
    x = posx[no][i];
    y = posy[no][i];
    DrawTo(x, y);
    MoveTo(x, y);
  }




  SetColor(color);

  y = op->tl;
  tlv = y;
  /* Atack */
  x = conv_time( op->atk);
  y = y / 4 + yofs;
  x = x / xdiv + xpos;
  MoveTo(xpos, yofs);
  posx[no][cnt] = xpos;
  posy[no][cnt++] = yofs;

  posx[no][cnt] = x;
  posy[no][cnt++] = y;
  DrawTo(x, y);
  MoveTo(x, y);
  xpos = x;

  /* Decy */
  y = (op->sul) * tlv / SUSDIV;
  x = conv_time( op->decy);

  if (x != 255) {
    y = y / 4 + yofs;

    x = x / xdiv + xpos;
    posx[no][cnt] = x;
    posy[no][cnt++] = y;
    DrawTo(x, y);
    MoveTo(x, y);
    xpos = x;

    /* sus */
    y = (op->sul) * op->sul / SUSDIV;
    x = conv_time( op->sus);
    if (x != 255) {

      y = y / 4 + yofs;
      x = x / xdiv / 2 + xpos;
      posx[no][cnt] = x;
      posy[no][cnt++] = y;
      DrawTo(x, y);
      MoveTo(x, y);
      xpos = x;
      /* release */
      x = conv_time( op->rel);
      x = x / xdiv + xpos;
      y = yofs;
      posx[no][cnt] = x;
      posy[no][cnt++] = y;
      DrawTo(x, y);


    } else {
      //y = op->sul;
      y = (op->sul) * op->tl / SUSDIV;
      y = y / 4 + yofs;
      x = 50 +  xpos;
      posx[no][cnt] = x;
      posy[no][cnt++] = y;
      DrawTo(x, y);
      MoveTo(x, y);
      xpos = x;

      x = conv_time( op->rel);
      x = x / xdiv * 2 + xpos;

      y = yofs;
      posx[no][cnt] = x;
      posy[no][cnt++] = y;
      DrawTo(x, y);

    }



  } else {
    /* attack to release*/
    y = op->tl;
    y = y / 4 + yofs;
    x = 80 +  xpos;
    posx[no][cnt] = x;
    posy[no][cnt++] = y;
    DrawTo(x, y);
    MoveTo(x, y);
    xpos = x;

    x = conv_time( op->rel);
    x = x / xdiv * 4 + xpos;
    y = yofs;
    posx[no][cnt] = x;
    posy[no][cnt++] = y;
    DrawTo(x, y);

  }
  posx[no][0] = cnt;

}

void selectOp(uint8_t no) {
  int color1, color2;
  color1 = no == 0 ? WHITE : GRAY;
  color2 = no == 1 ? WHITE : GRAY;
  SetColor(color2);
  MoveTo(0, 0);
  DrawTo(239, 0);
  DrawTo(239, 119);
  MoveTo(239, 0);
  DrawTo(239, 119);
  MoveTo(0, 0);
  DrawTo(0, 119);


  SetColor(color1);
  MoveTo(0, 120);
  DrawTo(0, 239);
  DrawTo(239, 239);
  //MoveTo(239,239);
  DrawTo(239, 120);
  SetColor(WHITE);
  MoveTo(0, 120);
  DrawTo(239, 120);

  SetColor(color1);
  MoveTo(4, 222);
  PlotText(op1);
  SetColor(color2);
  MoveTo(4, 102);
  PlotText(op2);

}

void drawMul(uint8_t no){

  uint8_t mul = no == 0 ?  fm_operator[0].mul : fm_operator[1].mul;
  uint8_t yofs = no == 0? 130:10;
  char c;
  MoveTo(5,yofs);
  SetColor(GRAY);
  PlotText(mul_mes);
   
   if(mul >= 10){
      c ='1';
      mul -= 10;
    }else{
      c = ' ';
    }
    
    MoveTo(40,yofs);
    PlotChar(c);
   
   MoveTo(52,yofs);
    PlotChar('0'+mul);
  
  
}



//-----------------------------------------------------------------
void disp_savemode(){
  SetColor(WHITE);
  MoveTo(20,150);
  PlotText(save_text);
  MoveTo(20,100);
  PlotText(load_text);
    
}
const char saveEnd[] PROGMEM = "save";
const char loadEnd[] PROGMEM = "load";
void disp_saved(){
  SetColor(WHITE);
  MoveTo(20,60);
  PlotText(saveEnd);
}

void disp_loaded(){
  SetColor(WHITE);
  MoveTo(20,60);
  PlotText(loadEnd);
}

