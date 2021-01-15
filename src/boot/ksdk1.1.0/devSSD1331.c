// some parts were adapted from the mbed display driver by Paul Staron https://os.mbed.com/users/star297/code/ssd1331/

#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"

volatile uint8_t	inBuffer[10];
volatile uint8_t	payloadBytes[1];

#define countof(x) ( sizeof(x) / sizeof(x[0]) )

/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 13),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};

int writeCommand(uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low. Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	payloadBytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
					(uint8_t * restrict)&inBuffer[0],
					1		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

int writeCommandBuf(uint8_t* commandByteBuf, uint8_t size)
{
	spi_status_t status;

	/*
	 *	Drive /CS low. Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)commandByteBuf,
					(uint8_t * restrict)&inBuffer[0],
					size		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);
	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

int
devSSD1331init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	enableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);		// 0xA0
	writeCommand(0x72);				// RGB Color
	writeCommand(kSSD1331CommandSTARTLINE);		// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);				// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);		// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);		// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);		// 0xB1
	writeCommand(0x31);
	writeCommand(kSSD1331CommandCLOCKDIV);		// 0xB3
	writeCommand(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0x78);
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8C
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3A);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	writeCommand(0x06);
	writeCommand(kSSD1331CommandCONTRASTA);		// 0x81
	writeCommand(0x91);
	writeCommand(kSSD1331CommandCONTRASTB);		// 0x82
	writeCommand(0x50);
	writeCommand(kSSD1331CommandCONTRASTC);		// 0x83
	writeCommand(0x7D);
	writeCommand(kSSD1331CommandDISPLAYON);		// Turn on oled panel

	/*
	 *	To use fill commands, you will have to issue a command to the display to enable them. See the manual.
	 */
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x01);

    // Clear screen
    clear();

    // Font size
    chr_size = HIGH;
	/*
	 *	Any post-initialization drawing commands go here.
	 */
	//...

    // Write initial display template
    background(0);
	foreground(toRGB(255,255,255));
	writeText(0, 1, "TEMP", 5);
	writeText(42, 1, "-----~C", 8);
	writeText(0, 22, "RH", 3);
	writeText(54, 22, "--%", 3);
	writeText(0, 43, "MOIST", 6);
	writeText(51, 42, "-", 2); 
 
	return 0;
}

void writeChar (int a)
{
    clear();

	PutChar(2,2,a);
}

void writeText(uint8_t x_coord, uint8_t y_coord, char *s, int len)
{
    clear_text(x_coord, y_coord, len);
    for (int i=0; i<len; i++)
    {
        PutChar(x_coord, y_coord, *s);
        s++;
        x_coord += X_width;
    }
    
}

static const char font6x8[0x60][6] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00 } , /*SPC */
    { 0x00,0x00,0x5F,0x00,0x00,0x00 } , /* !  */
    { 0x04,0x03,0x04,0x03,0x00,0x00 } , /* "  */
    { 0x28,0x7E,0x14,0x3F,0x0A,0x00 } , /* #  */
    { 0x24,0x2A,0x7F,0x2A,0x12,0x00 } , /* $  */
    { 0x23,0x13,0x08,0x64,0x62,0x00 } , /* %  */
    { 0x30,0x4E,0x59,0x26,0x50,0x00 } , /* &  */
    { 0x00,0x00,0x02,0x01,0x00,0x00 } , /* '  */
    { 0x00,0x00,0x1C,0x22,0x41,0x00 } , /* (  */
    { 0x41,0x22,0x1C,0x00,0x00,0x00 } , /* )  */
    { 0x22,0x14,0x08,0x14,0x22,0x00 } , /* *  */
    { 0x08,0x08,0x3E,0x08,0x08,0x00 } , /* +  */
    { 0x50,0x30,0x00,0x00,0x00,0x00 } , /* ,  */
    { 0x08,0x08,0x08,0x08,0x08,0x00 } , /* -  */ // space
    { 0x60,0x60,0x00,0x00,0x00,0x00 } , /* .  */
    { 0x20,0x10,0x08,0x04,0x02,0x00 } , /* /  */
    { 0x3E,0x51,0x49,0x45,0x3E,0x00 } , /* 0  */
    { 0x00,0x42,0x7F,0x40,0x00,0x00 } , /* 1  */
    { 0x62,0x51,0x49,0x49,0x46,0x00 } , /* 2  */
    { 0x22,0x41,0x49,0x49,0x36,0x00 } , /* 3  */
    { 0x18,0x14,0x12,0x7F,0x10,0x00 } , /* 4  */
    { 0x2F,0x45,0x45,0x45,0x39,0x00 } , /* 5  */
    { 0x3E,0x49,0x49,0x49,0x32,0x00 } , /* 6  */
    { 0x01,0x61,0x19,0x05,0x03,0x00 } , /* 7  */
    { 0x36,0x49,0x49,0x49,0x36,0x00 } , /* 8  */
    { 0x26,0x49,0x49,0x49,0x3E,0x00 } , /* 9  */
    { 0x00,0x36,0x36,0x00,0x00,0x00 } , /* :  */
    { 0x00,0x56,0x36,0x00,0x00,0x00 } , /* ;  */
    { 0x00,0x08,0x14,0x22,0x41,0x00 } , /* <  */
    { 0x14,0x14,0x14,0x14,0x14,0x00 } , /* =  */
    { 0x41,0x22,0x14,0x08,0x00,0x00 } , /* >  */
    { 0x02,0x01,0x59,0x09,0x06,0x00 } , /* ?  */
    { 0x3E,0x41,0x5D,0x55,0x2E,0x00 } , /* @  */
    { 0x60,0x1C,0x13,0x1C,0x60,0x00 } , /* A  */
    { 0x7F,0x49,0x49,0x49,0x36,0x00 } , /* B  */
    { 0x3E,0x41,0x41,0x41,0x22,0x00 } , /* C  */
    { 0x7F,0x41,0x41,0x22,0x1C,0x00 } , /* D  */
    { 0x7F,0x49,0x49,0x49,0x41,0x00 } , /* E  */
    { 0x7F,0x09,0x09,0x09,0x01,0x00 } , /* F  */
    { 0x1C,0x22,0x41,0x49,0x3A,0x00 } , /* G  */
    { 0x7F,0x08,0x08,0x08,0x7F,0x00 } , /* H  */
    { 0x00,0x41,0x7F,0x41,0x00,0x00 } , /* I  */
    { 0x20,0x40,0x40,0x40,0x3F,0x00 } , /* J  */
    { 0x7F,0x08,0x14,0x22,0x41,0x00 } , /* K  */
    { 0x7F,0x40,0x40,0x40,0x00,0x00 } , /* L  */
    { 0x7F,0x04,0x18,0x04,0x7F,0x00 } , /* M  */
    { 0x7F,0x04,0x08,0x10,0x7F,0x00 } , /* N  */
    { 0x3E,0x41,0x41,0x41,0x3E,0x00 } , /* O  */
    { 0x7F,0x09,0x09,0x09,0x06,0x00 } , /* P  */
    { 0x3E,0x41,0x51,0x21,0x5E,0x00 } , /* Q  */
    { 0x7F,0x09,0x19,0x29,0x46,0x00 } , /* R  */
    { 0x26,0x49,0x49,0x49,0x32,0x00 } , /* S  */
    { 0x01,0x01,0x7F,0x01,0x01,0x00 } , /* T  */
    { 0x3F,0x40,0x40,0x40,0x3F,0x00 } , /* U  */
    { 0x03,0x1C,0x60,0x1C,0x03,0x00 } , /* V  */
    { 0x0F,0x70,0x0F,0x70,0x0F,0x00 } , /* W  */
    { 0x41,0x36,0x08,0x36,0x41,0x00 } , /* X  */
    { 0x01,0x06,0x78,0x02,0x01,0x00 } , /* Y  */
    { 0x61,0x51,0x49,0x45,0x43,0x00 } , /* Z  */
    { 0x00,0x00,0x7F,0x41,0x41,0x00 } , /* [  */
    { 0x15,0x16,0x7C,0x16,0x11,0x00 } , /* \  */
    { 0x41,0x41,0x7F,0x00,0x00,0x00 } , /* ]  */
    { 0x00,0x02,0x01,0x02,0x00,0x00 } , /* ^  */
    { 0x40,0x40,0x40,0x40,0x40,0x00 } , /* _  */
    { 0x00,0x01,0x02,0x00,0x00,0x00 } , /* `  */
    { 0x00,0x20,0x54,0x54,0x78,0x00 } , /* a  */
    { 0x00,0x7F,0x44,0x44,0x38,0x00 } , /* b  */
    { 0x00,0x38,0x44,0x44,0x28,0x00 } , /* c  */
    { 0x00,0x38,0x44,0x44,0x7F,0x00 } , /* d  */
    { 0x00,0x38,0x54,0x54,0x18,0x00 } , /* e  */
    { 0x00,0x04,0x3E,0x05,0x01,0x00 } , /* f  */
    { 0x00,0x08,0x54,0x54,0x3C,0x00 } , /* g  */
    { 0x00,0x7F,0x04,0x04,0x78,0x00 } , /* h  */
    { 0x00,0x00,0x7D,0x00,0x00,0x00 } , /* i  */
    { 0x00,0x40,0x40,0x3D,0x00,0x00 } , /* j  */
    { 0x00,0x7F,0x10,0x28,0x44,0x00 } , /* k  */
    { 0x00,0x01,0x7F,0x00,0x00,0x00 } , /* l  */
    { 0x7C,0x04,0x7C,0x04,0x78,0x00 } , /* m  */
    { 0x00,0x7C,0x04,0x04,0x78,0x00 } , /* n  */
    { 0x00,0x38,0x44,0x44,0x38,0x00 } , /* o  */
    { 0x00,0x7C,0x14,0x14,0x08,0x00 } , /* p  */
    { 0x00,0x08,0x14,0x14,0x7C,0x00 } , /* q  */
    { 0x00,0x7C,0x08,0x04,0x04,0x00 } , /* r  */
    { 0x00,0x48,0x54,0x54,0x24,0x00 } , /* s  */
    { 0x00,0x04,0x3E,0x44,0x40,0x00 } , /* t  */
    { 0x00,0x3C,0x40,0x40,0x7C,0x00 } , /* u  */
    { 0x00,0x7C,0x20,0x10,0x0C,0x00 } , /* v  */
    { 0x1C,0x60,0x1C,0x60,0x1C,0x00 } , /* w  */
    { 0x00,0x6C,0x10,0x10,0x6C,0x00 } , /* x  */
    { 0x00,0x4C,0x50,0x30,0x1C,0x00 } , /* y  */
    { 0x00,0x44,0x64,0x54,0x4C,0x00 } , /* z  */
    { 0x02,0x04,0x48,0x70,0x70,0x78 } , /* arrow down as {  */
    { 0x00,0x08,0x08,0x3E,0x1C,0x08 } , /* arrow forward as |  */
    { 0x20,0x10,0x09,0x07,0x07,0x0F } , /* arrow up as }  */
    { 0x00,0x02,0x05,0x02,0x00,0x00 } , /* deg masked as ~ */
    { 0x00,0x00,0x00,0x00,0x00,0x00 }    /*null*/
};

void PutChar (uint8_t column,uint8_t row, int value)
{
        if ((value == (int)'{') || (value == (int)'}') || (value == (int)'|')) 
            chr_size = WH;
        else chr_size = HIGH;

        char_x  = column;
        char_y = row;
        // internal font
        if(value == '\n') {
            char_x = 0;
            char_y = char_y + Y_height;
        }
        if ((value < 31) || (value > 127)) return;   // test char range
        if (char_x + X_width > width) {
            char_x = 0;
            char_y = char_y + Y_height;
            if (char_y >= height - Y_height) {
                char_y = 0;
            }
        }
        int i,j,w,lpx,lpy,k,l,xw;
        unsigned char Temp=0;
        j = 0; i = 0;
        w = X_width;
        FontSizeConvert(&lpx, &lpy);
        xw = X_width;
        
        for(i=0; i<xw; i++) {
            for ( l=0; l<lpx; l++) {
                Temp = font6x8[value-32][i];
                
                uint8_t line_length = 0;
                
                for(j=Y_height-1; j>=0; j--) {
                    
                    for (k=0; k<lpy; k++) {    
                        if( (Temp & 0x80)==0x80) {
                            line_length++;
                        }else if(line_length>0){
                            line(char_x+(i*lpx)+l, char_y+(((j+1)*lpy)-1)-k, char_x+(i*lpx)+l,  char_y+(((j+1)*lpy)-1)-k + line_length-1, Char_Color);
                            line_length = 0;
                        }
                    }
                    Temp = Temp << 1;
                }
            }
        }
        FontSizeConvert(&lpx, &lpy);
        char_x += (w*lpx);
}

void FontSizeConvert(int *lpx,int *lpy)
{
    switch( chr_size ) {
        case WIDE:
            *lpx=2;
            *lpy=1;
            break;
        case HIGH:
            *lpx=1;
            *lpy=2;
            break;
        case WH  :
            *lpx=2;
            *lpy=2;
            break;
        case WHx36  :
            *lpx=6;
            *lpy=6;
            break;
        case NORMAL:
        default:
            *lpx=1;
            *lpy=1;
            break;
    }
}

void pixel(uint8_t x,uint8_t y,uint16_t Color)
{
    unsigned char cmd[7]= {Set_Column_Address,0x00,0x00,Set_Row_Address,0x00,0x00};
    if ((x>width)||(y>height)) return ;
    cmd[1] = (unsigned char)x;
    cmd[2] = (unsigned char)x;
    cmd[4] = (unsigned char)y;
    cmd[5] = (unsigned char)y;
    
    //RegWriteM
    writeCommandBuf(&cmd,6); 
    
    //DataWrite_to
	uint8_t colorBuf [2] = {(unsigned char)((Color >> 8)), (unsigned char)(Color)};
    writeCommandBuf(&colorBuf, 2);
}

void foreground(uint16_t color)
{
    Char_Color = color;
}
void background(uint16_t color)
{
    BGround_Color = color;
}

void Fill_Screen(uint16_t color)
{
    BGround_Color = color;
    fillrect(0,0,width,height,color,color);
}

uint16_t toRGB(uint16_t R,uint16_t G,uint16_t B)
{  
    uint16_t c;
    c = R >> 3;
    c <<= 6;
    c |= G >> 2;
    c <<= 5;
    c |= B >> 3;
    return c;
}

void clear(void)
{
    /*
	 *	Clear Screen
	 */
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
}

void clear_text(uint8_t x0, uint8_t y0, uint8_t len)
{
    // clears specified text, works only for text in the same row

	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(x0);
	writeCommand(y0);

    if(chr_size == WH)
    {
        writeCommand(x0+X_width*len*2);
        writeCommand(y0+Y_height*2);
    }
	else // else assume chr_size HIGH
    {
        writeCommand(x0+X_width*len);  
        writeCommand(y0+Y_height*2);
    }

	
}

void fillrect(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t colorline,uint16_t colorfill)
{
    if  ( x1 > width ) x1 = width;
    if  ( y1 > height ) y1 = height;
    if  ( x2 > width ) x2 = width;
    if  ( y2 > height ) y2 = height;
 
    unsigned char cmd[11]= { 0 };
    cmd[0] = GAC_FILL_ENABLE_DISABLE;
    cmd[1] = 1;      // fill 1, empty 0
    writeCommandBuf(cmd, 2);
    cmd[0] = GAC_DRAW_RECTANGLE;
    cmd[1] = (unsigned char)x1;
    cmd[2] = (unsigned char)y1;
    cmd[3] = (unsigned char)x2;
    cmd[4] = (unsigned char)y2;
    cmd[5] = (unsigned char)((colorline>> 11) << 1);    // Outline Blue
    cmd[6] = (unsigned char)((colorline>> 5 ) & 0x3F);  // Outline Green
    cmd[7] = (unsigned char)((colorline<< 1 ) & 0x3F);  // Outline Red
    cmd[8] = (unsigned char)((colorfill>> 11) << 1);    // fill Blue
    cmd[9] = (unsigned char)((colorfill>> 5 ) & 0x3F);  // fill Green
    cmd[10]= (unsigned char)((colorfill<< 1 ) & 0x3F);  // fill Red
    writeCommandBuf(cmd, 11);
    OSA_TimeDelay(500);
}

void line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint16_t color)
{
    if  ( x1 > width ) x1 = width;
    if  ( y1 > height ) y1 = height;
    if  ( x2 > width ) x2 = width;
    if  ( y2 > height ) y2 = height;
 
    unsigned char cmd[11]= { 0 };
    cmd[0] = GAC_FILL_ENABLE_DISABLE;
    cmd[1] = 0;      // fill 0, empty 0
    writeCommandBuf(cmd, 2);
    cmd[0] = GAC_DRAW_LINE;
    cmd[1] = (unsigned char)x1;
    cmd[2] = (unsigned char)y1;
    cmd[3] = (unsigned char)x2;
    cmd[4] = (unsigned char)y2;
    cmd[5] = (unsigned char)(((color>>11)&0x1F)<<1);    // Blue
    cmd[6] = (unsigned char)((color>>5)&0x3F);          // Green
    cmd[7] = (unsigned char)((color&0x1F)<<1);          // Red
    writeCommandBuf(cmd, 8);
}