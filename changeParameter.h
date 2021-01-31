void change_atk(uint8_t prg,uint8_t opno,int val);
void change_decy(uint8_t prg,uint8_t opno,int val);
void change_sul(uint8_t prg,uint8_t opno,int val);


void change_sus(uint8_t prg,uint8_t opno,int val);
void change_rel(uint8_t prg,uint8_t opno,int val);
void change_tl(uint8_t prg,uint8_t opno,int val);
void change_mul(uint8_t prg,uint8_t opno,int val);
void change_wave(uint8_t prg,uint8_t opno,int val);

void change_fb(uint8_t, uint8_t ch,int fb);

const uint8_t envelope_time[16]  = { 255, 180, 150, 100, 70, 50, 40, 26, 20, 14, 10, 8, 6, 4, 2, 0};
  

void save_eeprom(void);
void load_eeprom(void);
