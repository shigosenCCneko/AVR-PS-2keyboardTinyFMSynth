//#include "memtone.h"
#include "fmtone_def.h"
#include <avr/io.h>

#define WAVE_TBL_SIZE  64
#define TONE_DATLEN    17


#define MAX_DEFAULT_TONE    6


enum midistat {MIDI_POLY, MIDI_MONO};
enum wavetype {SIN, CLIP_SIN, TRI, SAW, RECT, ABS_SIN, RND, USER};
const uint8_t envelope_cnt[16]  = {0, 1, 2, 3, 4, 5, 7, 10, 13, 20, 29, 43, 64, 86, 128, 255};
                                         //endless,255,127,85,63,51,36,25,19,12,8,5,3,2,1,0
class FmTone
{

  protected:


  private:
    /* MIDI ポリフォニック発音割り振り用　*/
    uint8_t queue_cnt = 0;
    uint8_t voice_queue[MAX_MIDITONE];
    uint8_t voice_queue_top = 0;
    uint8_t voice_queue_tail = 0;

    uint8_t ch_midino[MAX_MIDITONE];
    uint8_t active_voice_num = 0;

  public:


    FmTone();
    setup_hardware();
    Initialize();
    midi_command(uint8_t com, char dat1, char dat2, char dat3);
    midi_setwave(wavetype wave);

    set_midistate(midistat);
    midistat get_midistate();

    set_wave(uint8_t, wavetype );
    set_envelope(uint8_t ch, uint8_t atk, uint8_t decy, uint8_t sul, uint8_t sus, uint8_t rel, uint8_t tl, uint8_t mul);

    set_tone(uint8_t ch , uint8_t *);
    midi_set_tone(uint8_t *);
    midi_pg_chg(uint8_t);

    change_atk(uint8_t ch, uint8_t atk);

    change_decy(uint8_t ch, uint8_t decy);
    change_sul(uint8_t ch, uint8_t sul);

    change_sus(uint8_t ch, uint8_t sus);
    change_rel(uint8_t ch, uint8_t rel);
    change_tl(uint8_t ch, uint8_t tl);
    change_mul(uint8_t ch, uint8_t mul);
    change_fb(uint8_t ch, uint8_t fb);
    copy_tone();
   uint8_t  conv_count(uint8_t );
  private:

    midi_note_off(uint8_t);
    midistat midi_state = MIDI_POLY;
  public:


};

extern FmTone FMTONE;

struct FmOperator {
    uint8_t fb;
    uint8_t fb_val;

    uint8_t atk;
    uint8_t decy;
    uint8_t sul;
    uint8_t sus;
    uint8_t rel;

    uint8_t  tl;
    uint8_t mul;
    char * wave_tbl;

    uint16_t sin_pos;
    uint16_t sin_step;

    uint8_t env_state;
    uint8_t env_cnt;
    uint8_t env_step;
    uint8_t level;
};

