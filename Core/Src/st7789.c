#include "st7789.h"
#include <stdbool.h>
#include <string.h>
#include "sse_util.h"

#ifdef USE_DMA
uint16_t DMA_MIN_SIZE = 16;
/* If you're using DMA, then u need a "framebuffer" to store datas to be displayed.
 * If your MCU don't have enough RAM, please avoid using DMA(or set 5 to 1).
 * And if your MCU have enough RAM(even larger than full-frame size),
 * Then you can specify the framebuffer size to the full resolution below.
 */
uint8_t disp_buf[ST7789_HEIGHT * 2];
#endif

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short*)(addr))
#define pgm_read_dword(addr)(*(const unsigned long *)(addr))

const fontinfo fontdata [] = {
   { NULL, 0, 0, 0 },
   // GLCD font (Font 1) does not have all parameters
   { NULL, 0, 0, 0 },
   // Font16.h fora do codigo
   { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
   // Font 3 current unused
   { NULL, 0, 0, 0 },
   { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
   { NULL, 0, 0, 0 },
   { NULL, 0, 0, 0 },
   { NULL, 0, 0, 0 },
   { NULL, 0, 0, 0 }
};

/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */


static void ST7789_WriteCommand(uint8_t cmd)
{
	ST7789_Select();
	ST7789_DC_Clr();
	HAL_SPI_Transmit_IT(&ST7789_SPI_PORT, &cmd, sizeof(cmd));
	ST7789_UnSelect();
}

/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
static void ST7789_WriteData(uint8_t *buff, size_t buff_size)
{
	ST7789_Select();
	ST7789_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
		#ifdef USE_DMA
			if (DMA_MIN_SIZE <= buff_size)
			{
				HAL_SPI_Transmit_DMA(&ST7789_SPI_PORT, buff, chunk_size);
				while (ST7789_SPI_PORT.hdmatx->State != HAL_DMA_STATE_READY)
				{}
			}
			else
				HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		#else
			HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		#endif
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	ST7789_UnSelect();
}
/**
 * @brief Write data to ST7789 controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */
static void ST7789_WriteSmallData(uint8_t data)
{
	ST7789_Select();
	ST7789_DC_Set();
	HAL_SPI_Transmit_IT(&ST7789_SPI_PORT, &data, sizeof(data));
	ST7789_UnSelect();
}

static void ST7789_pushBlock(uint16_t color, uint32_t len)
{
	uint8_t data[len*2];

	for(uint8_t i = 0; i<len; i++) {
		data[i*2] = color >> 8;
		data[i*2+1] = color & 0xFF;
	}
	ST7789_WriteData(data,sizeof(data));
}
/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in st7789.h)
 * @return none
 */
void ST7789_SetRotation(uint8_t m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
		break;
	case 3:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	ST7789_Select();
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;

	/* Column Address set */
	ST7789_WriteCommand(ST7789_CASET);
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
	ST7789_UnSelect();
}

/**
 * @brief Initialize ST7789 controller
 * @param none
 * @return none
 */
void ST7789_Init(void)
{
	HAL_Delay(10);
	ST7789_RST_Clr();
	HAL_Delay(10);
	ST7789_RST_Set();
	HAL_Delay(20);

	ST7789_WriteCommand(ST7789_COLMOD);		//	Set color mode
	ST7789_WriteSmallData(ST7789_COLOR_MODE_16bit);
	ST7789_WriteCommand(0xB2);				//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		ST7789_WriteData(data, sizeof(data));
	}
	ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)

	/* Internal LCD Voltage generator settings */
	ST7789_WriteCommand(0XB7);				//	Gate Control
	ST7789_WriteSmallData(0x35);			//	Default value
	ST7789_WriteCommand(0xBB);				//	VCOM setting
	ST7789_WriteSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
	ST7789_WriteCommand(0xC0);				//	LCMCTRL
	ST7789_WriteSmallData (0x2C);			//	Default value
	ST7789_WriteCommand (0xC2);				//	VDV and VRH command Enable
	ST7789_WriteSmallData (0x01);			//	Default value
	ST7789_WriteCommand (0xC3);				//	VRH set
	ST7789_WriteSmallData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
	ST7789_WriteCommand (0xC4);				//	VDV set
	ST7789_WriteSmallData (0x20);			//	Default value
	ST7789_WriteCommand (0xC6);				//	Frame rate control in normal mode
	ST7789_WriteSmallData (0x0F);			//	Default value (60HZ)
	ST7789_WriteCommand (0xD0);				//	Power control
	ST7789_WriteSmallData (0xA4);			//	Default value
	ST7789_WriteSmallData (0xA1);			//	Default value
	/**************** Division line ****************/

	ST7789_WriteCommand(0xE0);
	{
		uint8_t data[] = {0xD0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0e, 0x12, 0x14, 0x17};
		ST7789_WriteData(data, sizeof(data));
	}

	ST7789_WriteCommand(0xE1);
	{
		uint8_t data[] = {0xD0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x31, 0x54, 0x47, 0x0e, 0x1c, 0x17, 0x1b, 0x1e};
		ST7789_WriteData(data, sizeof(data));
	}
	ST7789_WriteCommand (ST7789_INVON);		//	Inversion ON
	ST7789_WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
	ST7789_WriteCommand (ST7789_NORON);		//	Normal Display on
	ST7789_WriteCommand (ST7789_DISPON);	//	Main screen turned on

	HAL_Delay(50);
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill_Color(uint16_t color)
{
	uint16_t i;
	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	ST7789_Select();

	#ifdef USE_DMA
	for(i = 0; i < ST7789_HEIGHT; i++) {
		disp_buf[i*2] = color >> 8;
		disp_buf[i*2+1] = color & 0xFF;
	}
	for (i = 0; i < ST7789_WIDTH; i++) {
		ST7789_WriteData(disp_buf, sizeof(disp_buf));
	}
	#else
	uint16_t j;
	for (i = 0; i < ST7789_WIDTH; i++)
			for (j = 0; j < ST7789_HEIGHT; j++) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				ST7789_WriteData(data, sizeof(data));
			}
	#endif
	ST7789_UnSelect();
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= ST7789_WIDTH) ||
		 (y < 0) || (y >= ST7789_HEIGHT))	return;
	
	ST7789_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7789_Select();
	ST7789_WriteData(data, sizeof(data));
	ST7789_UnSelect();
}

/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
		 (yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
	ST7789_Select();
	uint16_t i, j;
	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	uint8_t data[] = {color >> 8, color & 0xFF};
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++) {
			ST7789_WriteData(data, sizeof(data));
		}
	ST7789_UnSelect();
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        uint16_t color) {
	uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789_DrawPixel(y0, x0, color);
        } else {
            ST7789_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ST7789_fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{

	  if ((x >= 320) || (y >= 270)) return;
	  if ((x + w) > 320) w = 320 - x;
	  if ((y + h) > 270) h = 270 - y;
	  if ((w < 1) || (h < 1)) return;
	  ST7789_Select();
	  ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	  ST7789_pushBlock(color, w * h);
	  ST7789_UnSelect();
}

/**
 * @brief Draw a Rectangle with single color
 * @param xi&yi -> 2 coordinates of 2 top points.
 * @param color -> color of the Rectangle line
 * @return none
 */
void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	ST7789_Select();
	ST7789_DrawLine(x1, y1, x2, y1, color);
	ST7789_DrawLine(x1, y1, x1, y2, color);
	ST7789_DrawLine(x1, y2, x2, y2, color);
	ST7789_DrawLine(x2, y1, x2, y2, color);
	ST7789_UnSelect();
}

/** 
 * @brief Write a char
 * @param  x&y -> cursor of the start point.
 * @param ch -> char to write
 * @param font -> fontstyle of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return  none
 */
uint16_t ST7789_WriteChar(uint32_t x, uint32_t y, char ch, uint8_t font, uint8_t size, uint32_t color, uint32_t bgcolor)
{
	uint32_t width  = 0;
	uint32_t height = 0;
	uint32_t flash_address = 0;
	ch -= 32;

	ST7789_Select();
	if(font == 2) {
	    flash_address = pgm_read_dword(&chrtbl_f16[(uint8_t)ch]);
	    width = pgm_read_byte(widtbl_f16 + ch);
	    height = chr_hgt_f16;
	}
	else {
		flash_address = pgm_read_dword( (const void*)(pgm_read_dword( &(fontdata[font].chartbl ) ) + ch*sizeof(void *)) );
		width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + ch );
		height= pgm_read_byte( &fontdata[font].height );
	}
    uint32_t w = width;
    uint32_t pY = y;
    uint8_t line = 0;
	bool clip = x < 0 || x + width  * size >= 320 || y < 0 || y + height * size >= 170;
	if(font == 2) {
		    uint32_t pX = 0;
		    w = w + 6; // Should be + 7 but we need to compensate for width increment
		    w = w / 8;

		    if (color == bgcolor || size != 1 || clip) {
		      for (int32_t i = 0; i < height; i++) {
		        if (color != bgcolor) ST7789_fillRect(x, pY, width * size, size, bgcolor);

		        for (int32_t k = 0; k < w; k++) {
		          line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
		          if (line) {
		            if (size == 1) {
		              pX = x + k * 8;
		              if (line & 0x80) ST7789_DrawPixel(pX, pY, color);
		              if (line & 0x40) ST7789_DrawPixel(pX + 1, pY, color);
		              if (line & 0x20) ST7789_DrawPixel(pX + 2, pY, color);
		              if (line & 0x10) ST7789_DrawPixel(pX + 3, pY, color);
		              if (line & 0x08) ST7789_DrawPixel(pX + 4, pY, color);
		              if (line & 0x04) ST7789_DrawPixel(pX + 5, pY, color);
		              if (line & 0x02) ST7789_DrawPixel(pX + 6, pY, color);
		              if (line & 0x01) ST7789_DrawPixel(pX + 7, pY, color);
		            }
		            else {
		              pX = x + k * 8 * size;
		              if (line & 0x80) ST7789_fillRect(pX, pY, size, size, color);
		              if (line & 0x40) ST7789_fillRect(pX + size, pY, size, size, color);
		              if (line & 0x20) ST7789_fillRect(pX + 2 * size, pY, size, size, color);
		              if (line & 0x10) ST7789_fillRect(pX + 3 * size, pY, size, size, color);
		              if (line & 0x08) ST7789_fillRect(pX + 4 * size, pY, size, size, color);
		              if (line & 0x04) ST7789_fillRect(pX + 5 * size, pY, size, size, color);
		              if (line & 0x02) ST7789_fillRect(pX + 6 * size, pY, size, size, color);
		              if (line & 0x01) ST7789_fillRect(pX + 7 * size, pY, size, size, color);
		            }
		          }
		        }
		        pY += size;
		      }
		    }
		    else { // Faster drawing of characters and background using block write

		      ST7789_SetAddressWindow(x, y, x + width - 1, y + height - 1);

		      uint8_t mask;
		      for (int32_t i = 0; i < height; i++) {
		        pX = width;
		        for (int32_t k = 0; k < w; k++) {
		          line = pgm_read_byte((uint8_t *) (flash_address + w * i + k) );
		          mask = 0x80;
		          while (mask && pX) {
		            if (line & mask) {
		            	uint8_t data[] = {color >> 8, color & 0xFF};
	            	    ST7789_WriteData(data, sizeof(data));
		            }
		            else {
		            	uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
		            	ST7789_WriteData(data, sizeof(data));
		            }
		            pX--;
		            mask = mask >> 1;
		          }
		        }
		        if (pX) {
		        	uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
		        	ST7789_WriteData(data, sizeof(data));
		        }
		      }

		    }
	}
	else {

		w *= height; // Now w is total number of pixels in the character
	    if (color == bgcolor && !clip) {

		      int32_t px = 0, py = pY; // To hold character block start and end column and row values
		      int32_t pc = 0; // Pixel count
		      uint8_t np = size * size; // Number of pixels in a drawn pixel

		      uint8_t tnp = 0; // Temporary copy of np for while loop
		      uint8_t ts = size - 1; // Temporary copy of textsize
		      // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
		      // w is total number of pixels to plot to fill character block
		      while (pc < w) {
		        line = pgm_read_byte((uint8_t *)flash_address);
		        flash_address++;
		        if (line & 0x80) {
		          line &= 0x7F;
		          line++;
		          if (ts) {
		            px = x + size * (pc % width); // Keep these px and py calculations outside the loop as they are slow
		            py = y + size * (pc / width);
		          }
		          else {
		            px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
		            py = y + pc / width;
		          }
		          while (line--) { // In this case the while(line--) is faster
		            pc++; // This is faster than putting pc+=line before while()?
		            ST7789_SetAddressWindow(px, py, px + ts, py + ts);

		            if (ts) {
		              tnp = np;
		              while (tnp--) {
		            	  uint8_t data[] = {color >> 8, color & 0xFF};
		            	  ST7789_WriteData(data, sizeof(data));
		              }
		            }
		            else {
		            	uint8_t data[] = {color >> 8, color & 0xFF};
		            	ST7789_WriteData(data, sizeof(data));
		            }
		            px += size;

		            if (px >= (x + width * size)) {
		              px = x;
		              py += size;
		            }
		          }
		        }
		        else {
		          line++;
		          pc += line;
		        }
		      }
		    }
		    else {
		      // Text colour != background and textsize = 1 and character is within viewport area
		      // so use faster drawing of characters and background using block write
		      if (color != bgcolor && size == 1 && !clip)
		      {
		    	ST7789_SetAddressWindow(x, y, x + width - 1, y + height - 1);

		        // Maximum font size is equivalent to 180x180 pixels in area
		        while (w > 0) {
		          line = pgm_read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
		          if (line & 0x80) {
		            line &= 0x7F;
		            line++; w -= line;
		            ST7789_pushBlock(color,line);
		          }
		          else {
		            line++; w -= line;
		            ST7789_pushBlock(bgcolor,line);
		          }
		        }
		      }
		      else {
		        uint32_t px = 0, py = 0;  // To hold character pixel coords
		        uint32_t tx = 0, ty = 0;  // To hold character TFT pixel coords
		        uint32_t pc = 0;          // Pixel count
		        uint32_t pl = 0;          // Pixel line length
		        uint16_t pcol = 0;       // Pixel color
		        bool     pf = true;      // Flag for plotting
		        while (pc < w) {
		          line = pgm_read_byte((uint8_t *)flash_address);
		          flash_address++;
		          if (line & 0x80) { pcol = color; line &= 0x7F; pf = true;}
		          else { pcol = bgcolor; if (color == bgcolor) pf = false;}
		          line++;
		          px = pc % width;
		          tx = x + size * px;
		          py = pc / width;
		          ty = y + size * py;

		          pl = 0;
		          pc += line;
		          while (line--) {
		            pl++;
		            if ((px+pl) >= width) {
		              if (pf) ST7789_fillRect(tx, ty, pl * size, size, pcol);
		              pl = 0;
		              px = 0;
		              tx = x;
		              py ++;
		              ty += size;
		            }
		          }
		          if (pl && pf) ST7789_fillRect(tx, ty, pl * size, size, pcol);
		        }
		      }
		    }
	}
	ST7789_UnSelect();
	return width * size;
}
/** 
 * @brief Write a string 
 * @param  x&y -> cursor of the start point.
 * @param str -> string to write
 * @param font -> fontstyle of the string
 * @param color -> color of the string
 * @param bgcolor -> background color of the string
 * @return  none
 */
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, uint8_t font, uint8_t size, uint16_t color, uint16_t bgcolor)
{
	uint16_t sumX = 0;

	while (*str) {
		sumX += ST7789_WriteChar(x+sumX, y, *str, font, size, color, bgcolor);
		str++;
	}
	ST7789_UnSelect();
}

/** 
 * @brief Draw a Filled circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle
 * @return  none
 */
void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	ST7789_Select();
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);
	ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}

/** 
 * @brief A Simple test function for ST7789
 * @param  none
 * @return  none
 */
void ST7789_Layout(void)
{
	ST7789_Fill_Color(BLACK);
	ST7789_WriteString(10, 150, "Equip.", 2, 1, ORANGE, BLACK);
	ST7789_WriteString(10, 120, "Codigo", 2, 1, ORANGE, BLACK);
	ST7789_WriteString(130, 120, "Valor", 2, 1, ORANGE, BLACK);
	ST7789_WriteString(60, 147, "31604211", 4, 1, WHITE, BLACK);
	ST7789_WriteString(10, 5, "C1: Totalizador Geral", 4, 1, WHITE, BLACK);
	ST7789_WriteString(10, 55, "003", 4, 2, WHITE, BLACK);
	ST7789_WriteString(130, 55, "8418.27", 4, 2, WHITE, BLACK);
	ST7789_WriteString(250, 115, "KWh", 4, 1, WHITE, BLACK);
	ST7789_DrawRectangle(0, 1, 318, 169, ORANGE);
	ST7789_DrawLine(0, 30, 318, 30, ORANGE);
	ST7789_DrawLine(120, 30, 120, 145, ORANGE);
	ST7789_DrawLine(0, 145, 318, 145, ORANGE);
	ST7789_DrawFilledCircle(298, 157, 8, GREEN);
}

void ST7789_Layout_2(void)
{
	char buff[8] = {};
#if (RADIO == 0)
	HexU8ToString(&buff[0],medidor.en_ativa[0],3);
	ST7789_WriteString(10, 5, "C4", 4, 1, WHITE, BLACK);
	ST7789_DrawLine(0, 30, 318, 30, ORANGE);
	ST7789_WriteString(10, 55, "103", 4, 2, WHITE, BLACK);
	ST7789_WriteString(130, 55, buff, 4, 2, WHITE, BLACK);
	ST7789_WriteString(250, 115, "KWh", 4, 1, WHITE, BLACK);
#else
	HexU8ToString(&buff[0],displayQuantities[105].value,4);
	ST7789_WriteString(10, 5, displayQuantities[105].desc, 4, 1, WHITE, BLACK);
	ST7789_DrawLine(0, 30, 318, 30, ORANGE);
	ST7789_WriteString(10, 55, displayQuantities[105].code, 4, 2, WHITE, BLACK);
	ST7789_WriteString(130, 55, buff, 4, 2, WHITE, BLACK);
	ST7789_WriteString(250, 115, displayQuantities[105].unit, 4, 1, WHITE, BLACK);
#endif
}

void ST7789_Layout_3(void)
{
	char buff[8] = {};
#if (RADIO == 0)
	HexU8ToString(&buff[0],medidor.en_ativa[0],3);
	ST7789_WriteString(10, 5, "C1", 4, 1, WHITE, BLACK);
	ST7789_DrawLine(0, 30, 318, 30, ORANGE);
	ST7789_WriteString(10, 55, "003", 4, 2, WHITE, BLACK);
	ST7789_WriteString(130, 55, buff, 4, 2, WHITE, BLACK);
	ST7789_WriteString(250, 115, "KWh", 4, 1, WHITE, BLACK);
#else
	HexU8ToString(&buff[0],displayQuantities[04].value,4);
	ST7789_WriteString(10, 5, displayQuantities[04].desc, 4, 1, WHITE, BLACK);
	ST7789_DrawLine(0, 30, 318, 30, ORANGE);
	ST7789_WriteString(10, 55, displayQuantities[04].code, 4, 2, WHITE, BLACK);
	ST7789_WriteString(130, 55, buff, 4, 2, WHITE, BLACK);
	ST7789_WriteString(250, 115, displayQuantities[04].unit, 4, 1, WHITE, BLACK);
#endif
}
