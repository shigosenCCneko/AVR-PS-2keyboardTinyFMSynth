/*
    AVR Tiny FM Synthsizer

       Keiji Katahira  2021/1/31
        ATmega328P 20MHz

    CC BY 4.0
    Licensed under a Creative Commons Attribution 4.0 International license:
    http://creativecommons.org/licenses/by/4.0/
*/



#include "FMTONE.h"
#include "display.h"
#include "changeParameter.h"
#include<avr/io.h>
#include <avr/pgmspace.h>


#include <SPI.h>



#define CLK_LO  pinMode(4,OUTPUT);
#define CLK_HI  pinMode(4,INPUT);
#define DATA_LO pinMode(0,OUTPUT);
#define DATA_HI pinMode(0,INPUT);
#define WAIT_CLK_LO while(digitalRead(4) == HIGH);
#define WAIT_CLK_HI while(digitalRead(4) == LOW);

#define DELAY 50

uint8_t prg_no = 0;
uint8_t chord_velo = 60;
uint8_t chord_offs = 0;
int8_t tone1_offs = 0;
int8_t tone2_offs = 0;

uint8_t selectedOp = 1;
enum {PLAY, EDIT, SAVE};
uint8_t mode = EDIT;


int km[0x90];

uint8_t tone_no[128];
volatile int read_point = 0;
volatile int write_point = 0;
volatile int rx_cnt = 0;
uint8_t rx_buff[64];

uint8_t send_data(uint8_t);
static const uint8_t sample_tone[17] = {0, 13, 4, 5, 2, 0, 10,  4, 2,  11, 9, 5, 3, 4, 3,  1, 0,};


const uint8_t chord[8][3] PROGMEM = {
  {48, 52, 55}, //C
  {48, 53, 57}, //F
  {50, 55, 59}, //G
  {50, 54, 57}, //D
  {50, 53, 57}, //Dm
  {48, 52, 57}, //Am
  {52, 55, 59},//Em
  {50, 55, 59},

};



//int main() {
//
//  //  CLKPR = 0x80; CLKPR = 0;
//  //
//  //  MCUSR &= ~(1<<WDRF); /* ｳｫｯﾁﾄﾞｯｸﾞ ﾘｾｯﾄ ﾌﾗｸﾞ(WDRF)解除 */
//  //  WDTCSR |= (1<<WDCE)|(1<<WDE); /* WDCEとWDEに論理1書き込み */
//  //  WDTCSR = 0x00; /* ｳｫｯﾁﾄﾞｯｸﾞ禁止 */
//
//  setup();
//  sei();
//  loop();
//}

void setup() {




  for (int i = 0 ; i < 0x90; i++) {
    km[i] = 0;
  }
  for (int i = 0 ; i < 128; i++) {
    tone_no[i] = 0;
  }
  km[0x12] = 59;
  km[0x1a] = 60;
  km[0x1b] = 61;
  km[0x22] = 62;
  km[0x23] = 63;
  km[0x21] = 64;
  km[0x2a] = 65;
  km[0x34] = 66;
  km[0x32] = 67;
  km[0x33] = 68;
  km[0x31] = 69;
  km[0x3b] = 70;
  km[0x3a] = 71;
  km[0x41] = 72;
  km[0x4b] = 73;
  km[0x49] = 74;
  km[0x4c] = 75;
  km[0x4a] = 76;
  km[0x51] = 77;
  km[0x5d] = 78;
  km[0x59] = 79;
  km[0x5a] = 80;
  km[0x6b] = 81;
  km[0x75] = 82;
  km[0x72] = 83;
  km[0x74] = 84;

  km[0x15] = -48;
  km[0x1e] = -49;
  km[0x1d] = -50;
  km[0x26] = -51;
  km[0x24] = -52 ;
  km[0x2d] = -53 ;
  km[0x2e] = -54 ;
  km[0x2c] = -55 ;
  km[0x36] = -56 ;
  km[0x35] = -57 ;
  km[0x3d] = -58 ;
  km[0x3c] = -59 ;
  km[0x43] = -60 ;
  km[0x46] = -61 ;
  km[0x44] = -62;
  km[0x45] = -63;
  km[0x4d] = -64;
  km[0x54] = -65;
  km[0x55] = -66;
  km[0x5b] = -67;


  km[0x76] = 128; //ESC  program change

  km[0x71] = 13;  //Delete
  km[0x69] = 14;  //End
  km[0x7a] = 15;  //PgDn

  km[0x70] = 16;  //Insert
  km[0x6c] = 17;  //Home
  km[0x7d] = 18;  //PgUp

  km[0x01] = 19;   //f9
  km[0x09] = 20;   //f10
  km[0x78] = 21;  //F11
  km[0x07] = 22;  //F12
  km[0x77] = 23;   //NumLock
  km[0x7e] = 24;   //ScrollLock

  km[0x05] = 1;
  km[0x06] = 2;
  km[0x04] = 3;
  km[0x0c] = 4;

  km[0x3] = 5;
  km[0xb] = 6;
  km[0x83] = 7;
  km[0xa] = 8;





  //UCSR0B = (1<<RXEN0) | (0<< UCSZ02) |(1 <<RXCIE0);
  //UCSR0C = (0<<UMSEL01)|(1<<UMSEL00) |(1<<UPM01)| (1<<UPM00)|(1<<UCSZ01)|(1<<UCSZ00);

  Serial.begin(9600, SERIAL_8O1);
  UCSR0C |= (1 << UMSEL00);   //同期通信モードへ変更

  pinMode(0, INPUT);
  pinMode(4, INPUT);

  digitalWrite(0, LOW);
  digitalWrite(4, LOW);






  FMTONE.setup_hardware();


  FMTONE.set_midistate(MIDI_POLY);


  FMTONE.midi_set_tone(sample_tone);
  //    FMTONE.midi_pg_chg(0);
  //CLK_HI;
  //DATA_LO;
  //
  //  send_data(0xff);


  delay(500);
  TIMSK1 = 0;
  InitDisplay();
  ClearDisplay();
  DisplayOn();

  DispForm();
  selectOp(selectedOp);
  TIMSK1 |= (1 << OCIE1A);    //Timer1 enable
}

void loop() {
  uint8_t c;
  int midi_no;
  int cnt = 0;
  uint8_t ex_code = 0;
  uint8_t ex_mode;
  prg_no = 0;
  sei();

  while (1) {

    c = get_usart();
    if (c == 0xE0) {
      ex_code = 1;
    } else {

      if (c != 0xF0) {

        midi_no = km[c];

        if (ex_code == 1) {
          ex_mode = 1;
        } else {
          ex_mode = 0;
        }
        ex_code = 0;




        if (midi_no == 128) {


          if ( (mode == EDIT) || (mode == PLAY)) {
            prg_no++;
            if (prg_no >= MAX_DEFAULT_TONE) {
              prg_no = 0;
            }
            FMTONE.midi_pg_chg(prg_no);

            TIMSK1 = 0;
            DispForm();
            drawEnvelope(0, WHITE);
            drawEnvelope(1, WHITE);
            drawOperatorWave(0);
            drawOperatorWave(1);
            //selectOp(selectedOp);
            TIMSK1 |= (1 << OCIE1A);    //Timer1 enable
          }

        } else {

          if (midi_no != 0) {

            if (tone_no[abs(midi_no)] == 0) {
              //------------------------------------------------------------------------------

              if (midi_no == 24) { //ScrollRock  ----------------------------------toggle Mode-------------------------
                mode++;
                if (mode > 2) mode = 0;


                if (mode == 0) {

                  TIMSK1 = 0;
                  ClearDisplay();
                  DispForm();
                  TIMSK1 |= (1 << OCIE1A);    //Timer1 enable
                }
                if (mode == 1) {

                  TIMSK1 = 0;
                  //DispForm();
                  selectOp(selectedOp);
                  TIMSK1 |= (1 << OCIE1A);    //Timer1 enable
                }
                if ( mode == 2) {

                  TIMSK1 = 0;
                  ClearDisplay();
                  disp_savemode();
                  TIMSK1 |= (1 << OCIE1A);    //Timer1 enable
                }


              } else {


                switch (mode) {
                  case EDIT:     // -------------------------------Edit Mode ----------------------------------------------------

                    switch (midi_no) {

                      case 1:
                        change_atk(prg_no, selectedOp, 1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 2:
                        change_atk(prg_no, selectedOp, -1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 3:
                        change_decy(prg_no, selectedOp, 1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 4:
                        change_decy(prg_no, selectedOp, -1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 5:
                        change_sul(prg_no, selectedOp, 1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 6:
                        change_sul(prg_no, selectedOp, -1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 7:
                        change_sus(prg_no, selectedOp, 1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 8:
                        change_sus(prg_no, selectedOp, -1);
                        drawEnvelope(selectedOp, WHITE);
                        break;

                      case 13://Del
                        if (ex_mode == 1) {
                          change_tl(prg_no, selectedOp, 1);
                          drawEnvelope(selectedOp, WHITE);
                        }
                        break;
                      case 14:  //End
                        if (ex_mode == 1) {
                          change_wave(prg_no, selectedOp, 1);
                          drawEnvelope(selectedOp, WHITE);
                          drawOperatorWave(selectedOp);
                        }
                        break;
                      case 15+128:  //PgDn
                        if (ex_mode == 1) {
                          change_fb(prg_no, selectedOp, 1);
                        }
                        break;
                      case 16:  //Insert
                        if (ex_mode == 1) {
                          change_tl(prg_no, selectedOp, -1);
                          drawEnvelope(selectedOp, WHITE);
                        }
                        break;
                      case 17:  //Home
                        if (ex_mode == 1) {
                          change_wave(prg_no, selectedOp, -1);
                          drawEnvelope(selectedOp, WHITE);
                        }
                        break;
                      case 18:  //PgUp

                        if (ex_mode == 1) {
                          change_fb(prg_no, selectedOp, -1);
                          drawEnvelope(selectedOp, WHITE);
                        }
                        break;
                      case 19:    //F9
                        change_rel(prg_no, selectedOp, 1);
                        break;
                      case 20:    //f10
                        change_rel(prg_no, selectedOp, -1);
                        drawEnvelope(selectedOp, WHITE);
                        break;
                      case 21:  //F11
                        change_mul(prg_no, selectedOp, -1);
                        drawMul(selectedOp);
                        break;
                      case 22:  //F12
                        change_mul(prg_no, selectedOp, 1);
                        drawMul(selectedOp);
                        break;
                      case 23:  //NumLock
                        selectedOp = (selectedOp + 1) & 0x01;
                        selectOp(selectedOp);
                        break;

                      default:
                        if (midi_no < 0) {
                          midi_no  = - midi_no;
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone1_offs, 120);
                        } else {
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone2_offs, 120);
                          break;
                        }
                    }
                    break; //EDIT break

                  case PLAY:      //--------------------------------------Play Mode ----------------------------------------------------------

                    switch (midi_no) {
                      case 1:
                      case 2:
                      case 3:
                      case 4:
                      case 5:
                      case 6:
                      case 7:
                      case 8:


                        tone_no[midi_no] = 1;
                        midi_no--;
                        chord_on(
                          pgm_read_byte(&(chord[midi_no][0])) + chord_offs,
                          pgm_read_byte(&(chord[midi_no][1])) + chord_offs,
                          pgm_read_byte(&(chord[midi_no][2])) + chord_offs, chord_velo);
                        break;
                      case 13://Del
                        chord_velo = 40;
                        // led_level(0);
                        break;
                      case 14:  //End
                        chord_velo = 60;
                        // led_level(1);
                        break;
                      case 15:  //PgDn
                        chord_velo = 80;
                        // led_level(2);
                        break;
                      case 16:  //Insert
                        chord_offs = -12;
                        break;
                      case 17:  //Home
                        chord_offs = 0;
                        break;
                      case 18:  //PgUp
                        chord_offs = 12;
                        break;
                      case 19:    //F9
                        tone1_offs = -12;
                        break;
                      case 20:    //f10
                        tone1_offs = 0;
                        break;
                      case 21:  //F11
                        tone2_offs = 0;
                        break;
                      case 22:  //F12
                        tone2_offs = 12;
                        break;
                      case 23:  //NumLock
                        break;




                      default:
                        if (midi_no < 0) {
                          midi_no  = - midi_no;
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone1_offs, 120);
                        } else {
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone2_offs, 120);
                          break;
                        }











                    }
                    break; //PLAY break;

                  case SAVE:   //----------------------------------Save Mode --------------------------------------------------------

                    switch (midi_no) {
                      case 1:
                        load_eeprom();
                        break;
                      case 2:
                        save_eeprom();
                        break;




                      default:
                        if (midi_no < 0) {
                          midi_no  = - midi_no;
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone1_offs, 120);
                        } else {
                          tone_no[midi_no] = 1;
                          FMTONE.midi_command(0x90, 0x90, midi_no + tone2_offs, 120);
                          break;
                        }



                    }
                    break; //SAVE break;

                  default:
                    break;



                }

                //-----------------------------------------------------------------------------------
              }
            }
          }
        }
      } else {                              //キーオフ
        c = get_usart();
        midi_no = km[c];
        if (midi_no != 0) {
          if (tone_no[abs(midi_no)] == 1) {


            switch (midi_no) {
              case 1:
              case 2:
              case 3:
              case 4:
              case 5:
              case 6:
              case 7:
              case 8:
                tone_no[midi_no] = 0;
                midi_no--;
                chord_off(
                  pgm_read_byte(&(chord[midi_no][0])) + chord_offs,
                  pgm_read_byte(&(chord[midi_no][1])) + chord_offs,
                  pgm_read_byte(&(chord[midi_no][2])) + chord_offs);
                break;


              default:
                if (midi_no < 0) {
                  midi_no  = -midi_no;
                  tone_no[midi_no] = 0;
                  FMTONE.midi_command(0x90, 0x90, midi_no + tone1_offs , 0);
                } else {
                  tone_no[midi_no] = 0;
                  FMTONE.midi_command(0x90, 0x90, midi_no + tone2_offs , 0);
                }
                break;
            }
          }
        }

      }
    }
  }

}

//uint8_t aget_usart() {
//  uint8_t c;
//
//  while (!(UCSR0A & (1 << RXC0)));
//  c = UDR0;
//  return c;
//}

uint8_t  get_usart() {
  uint8_t c;
  while (Serial.available() == 0)
    ;
  c = (0x00ff & Serial.read());
  return c;

}
//uint8_t cget_usart() {
//  uint8_t c;
//
//  while (rx_cnt == 0)
//    ;
//
//  cli();
//  rx_cnt--;
//  c = rx_buff[read_point];
//  read_point++;
//  if (read_point > 63) {
//    read_point = 0;
//  }
//  sei();
//  return c;
//}

//ISR(USART_RX_vect){
//  cli();
//  rx_buff[write_point] = UDR0;
//  rx_cnt++;
//  write_point++;
//  if(write_point > 63){
//    write_point = 0;
//
//  }
//  sei();
//
//}


void led_level(uint8_t level) {
  level &= 3;


  //  switch (level) {
  //    case 0:
  //      send_data(0xed);
  //      send_data(0);
  //      break;
  //    case 1:
  //      send_data(0xed);
  //      send_data(0x01);
  //      break;
  //    case 2:
  //      send_data(0xed);
  //      send_data(0x05);
  //      break;
  //    case 3:
  //      send_data(0xed);
  //      send_data(0x07);
  //      break;
  //
  //    default:
  //      send_data(0xed);
  //      send_data(0);
  //      break;
  //
  //  }

}




void chord_on(uint8_t fntone1, uint8_t fntone2, uint8_t fntone3, uint8_t vol) {

  FMTONE.midi_command(0x90, 0x90, fntone1 , vol);
  FMTONE.midi_command(0x90, 0x90, fntone2 , vol);
  FMTONE.midi_command(0x90, 0x90, fntone3 , vol);
}

void chord_off(uint8_t fntone1, uint8_t fntone2, uint8_t fntone3) {

  FMTONE.midi_command(0x90, 0x90, fntone1 , 0);
  FMTONE.midi_command(0x90, 0x90, fntone2 , 0);
  FMTONE.midi_command(0x90, 0x90, fntone3 , 0);
}



uint8_t send_data(uint8_t data) {
  int shift = 1;
  uint8_t parity = 0;
  uint8_t rcv;
  WAIT_CLK_HI;   //busy?

  delayMicroseconds(DELAY);


  TIMSK1 &= ~(1 << OCIE1A); //timer off
  UCSR0B &= ~(1 << RXEN0); //usart off
  UCSR0C &= ~(1 << UMSEL00);

  CLK_LO;                 //RTS
  delayMicroseconds(DELAY);

  DATA_LO; //start bit
  delayMicroseconds(DELAY);
  CLK_HI;

  WAIT_CLK_LO;          //キーボード発生のクロック待ち

  delayMicroseconds(DELAY / 2);




  while (shift != 0x100) {

    if (data & shift) {
      DATA_HI;  //bit = 1
      parity++;
    } else {
      DATA_LO  //bit = 0
    }
    WAIT_CLK_LO;
    WAIT_CLK_HI;
    delayMicroseconds(DELAY / 2);
    shift <<= 1;
  }
  //parity

  if (parity & 0x01) {
    DATA_LO;  //bit = 1
  } else {
    DATA_HI;  //bit = 0
  }
  WAIT_CLK_LO;

  WAIT_CLK_HI;

  delayMicroseconds(DELAY / 2);
  DATA_HI; //stop bit
  WAIT_CLK_LO;
  WAIT_CLK_HI;

  WAIT_CLK_LO;
  WAIT_CLK_HI;

  UCSR0B &= ~(1 << RXCIE0); // intrrupt off
  UCSR0B |= (1 << RXEN0); //usart on
  UCSR0C |= (1 << UMSEL00);
  while (!(UCSR0A & (1 << RXC0)));
  rcv = UDR0;
  UCSR0B |= (1 << RXCIE0); // intrrupt on

  TIMSK1 |= (1 << OCIE1A);
  return rcv;

}



