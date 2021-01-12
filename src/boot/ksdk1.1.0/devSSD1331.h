/*
 *	See https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino for the Arduino driver.
 */

typedef enum
{
	kSSD1331ColororderRGB		= 1,
	kSSD1331DelaysHWFILL		= 3,
	kSSD1331DelaysHWLINE		= 1,
} SSD1331Constants;

typedef enum
{
	kSSD1331CommandDRAWLINE		= 0x21,
	kSSD1331CommandDRAWRECT		= 0x22,
	kSSD1331CommandCLEAR		= 0x25,
	kSSD1331CommandFILL		= 0x26,
	kSSD1331CommandSETCOLUMN	= 0x15,
	kSSD1331CommandSETROW		= 0x75,
	kSSD1331CommandCONTRASTA	= 0x81,
	kSSD1331CommandCONTRASTB	= 0x82,
	kSSD1331CommandCONTRASTC	= 0x83,
	kSSD1331CommandMASTERCURRENT	= 0x87,
	kSSD1331CommandSETREMAP		= 0xA0,
	kSSD1331CommandSTARTLINE	= 0xA1,
	kSSD1331CommandDISPLAYOFFSET	= 0xA2,
	kSSD1331CommandNORMALDISPLAY	= 0xA4,
	kSSD1331CommandDISPLAYALLON	= 0xA5,
	kSSD1331CommandDISPLAYALLOFF	= 0xA6,
	kSSD1331CommandINVERTDISPLAY	= 0xA7,
	kSSD1331CommandSETMULTIPLEX	= 0xA8,
	kSSD1331CommandSETMASTER	= 0xAD,
	kSSD1331CommandDISPLAYOFF	= 0xAE,
	kSSD1331CommandDISPLAYON	= 0xAF,
	kSSD1331CommandPOWERMODE	= 0xB0,
	kSSD1331CommandPRECHARGE	= 0xB1,
	kSSD1331CommandCLOCKDIV		= 0xB3,
	kSSD1331CommandPRECHARGEA	= 0x8A,
	kSSD1331CommandPRECHARGEB	= 0x8B,
	kSSD1331CommandPRECHARGEC	= 0x8C,
	kSSD1331CommandPRECHARGELEVEL	= 0xBB,
	kSSD1331CommandVCOMH		= 0xBE,
} SSD1331Commands;

int	devSSD1331init(void);
void clear(void);
void writeChar (int a);
void writeText(uint8_t x_coord, uint8_t y_coord, char *s, int len);
int writeCommandBuf(uint8_t* commandByteBuf, uint8_t size);

// Screen Settings
#define width   96-1        // Max X axial direction in screen
#define height  64-1        // Max Y axial direction in screen
#define Set_Column_Address  0x15
#define Set_Row_Address     0x75
#define contrastA           0x81
#define contrastB           0x82
#define contrastC           0x83
#define display_on          0xAF
#define display_off         0xAE

// Internal Font size settings
#define NORMAL  0
#define WIDE    1
#define HIGH    2
#define WH      3
#define WHx36   4
#define X_width 6 
#define Y_height 9 

// GAC hardware acceleration commands
#define GAC_DRAW_LINE           0x21    // Draw Line
#define GAC_DRAW_RECTANGLE      0x22    // Rectangle
#define GAC_COPY_AREA           0x23    // Copy Area
#define GAC_DIM_WINDOW          0x24    // Dim Window
#define GAC_CLEAR_WINDOW        0x25    // Clear Window
#define GAC_FILL_ENABLE_DISABLE 0x26    // Enable Fill
#define SCROLL_SETUP            0x27    // Setup scroll
#define SCROLL_STOP             0x2E    // Scroll Stop
#define SCROLL_START            0x2F    // Scroll Start
 
    void pixel(uint8_t x,uint8_t y,uint16_t Color);
    void PutChar(uint8_t x,uint8_t y,int a);
    void FontSizeConvert(int *lpx,int *lpy);
	void background(uint16_t color);
	void foreground(uint16_t color);
	void fillrect(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t colorline,uint16_t colorfill);
	void Fill_Screen(uint16_t color);
	uint16_t toRGB(uint16_t R,uint16_t G,uint16_t B);
	void line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t color);
    uint16_t Char_Color;    // text color
    uint16_t BGround_Color; // background color

    // window location
    uint8_t _x1;
    uint8_t _x2;
    uint8_t _y1;
    uint8_t _y2;
    uint8_t char_x;
    uint8_t char_y;
    uint8_t chr_size;
    uint8_t cwidth;       // character's width
    uint8_t cvert;        // character's height
