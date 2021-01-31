void DispForm(void);
void selectOp(uint8_t);
void drawEnvelope(uint8_t,int);
void drawOperatorWave(uint8_t);

void SetColor(int);
void ClearDisplay (void); 
void MoveTo (int x, int y);
void PlotPoint (int x, int y);
void DrawTo (int x, int y);
void FillRect (int w, int h);
void PlotChar (char c);
void PlotText(char *);
void selectOp(uint8_t);

void drawMul(uint8_t);

void disp_savemode(void);
void disp_saved(void);
void disp_loaded(void);

#define SUSDIV 124

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    (0x0013 <<11) | 0x0013 | (0x0013 <<6)
