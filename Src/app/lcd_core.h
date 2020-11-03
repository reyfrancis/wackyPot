/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "lcd.h"
#include "bmp.h"
#include "stm32_adafruit_lcd.h" /* BSP_LCD Function Definitions */

#ifdef __cplusplus
extern "C" {
#endif

/* Images Macros */
#define gui_bat_0     GUI_BAT_0
#define gui_bat_1     GUI_BAT_1
#define gui_slp_0     GUI_SLP_0
#define gui_slp_1     GUI_SLP_1
#define gui_alrm_0    GUI_ALRM_0
#define gui_alrm_1    GUI_ALRM_1
#define gui_clndr_0   GUI_CLNDR_0
#define gui_clndr_1   GUI_CLNDR_1

#define bat_bat_0     BAT_BAT_0
#define bat_bat_1     BAT_BAT_1
#define bat_onof_0    BAT_ONOF_0
#define bat_onof_1    BAT_ONOF_1

#define slp_slp_0     SLP_SLP_0
#define slp_slp_1     SLP_SLP_1
#define slp_onof_0    SLP_ONOF_0
#define slp_onof_1    SLP_ONOF_1
#define slp_set_0     SLP_SET_0
#define slp_set_1     SLP_SET_1

#define alrm_alrm_1   ALRM_ALRM_1
#define alrm_alrm_0   ALRM_ALRM_0

#define bat_sens      BAT_SENS
#define moist_sens    MOIST_SENS
#define wat_sets      WAT_SENS

#define mid_clock_0   MID_CLOCK_0
#define mid_clock_1   MID_CLOCK_1
#define mid_hand_0    MID_HAND_0
#define mid_hand_1    MID_HAND_1

/* LCD    Dimensions in Pixels */
#define WIDTH  320
#define HEIGHT 240

/* LCD Initialization Macros */

/* Default GUI */
#define BAT_TLBR           {84,          167,      236,     230}    /* Coodinates of top left and top right battery box */
#define GUI_CTRPT          {WIDTH/2,     HEIGHT/2}
#define BOX_DIM            {152,         63}       /* Default GUI */
#define BAT_SENS_BR        266,          25
#define MOIST_SENS_BR      12,           232
#define WAT_SENS_BR        272,          234
#define MID_BTN_BR         {118,         161}

/* ONOF values */
#define OFF                0
#define ON                 1

/* CLICK values */
#define CLICK              1

/* Font Widths */
#define FontPerpetuaTitlingMT17_WIDTH 19
#define FontCalibri23_WIDTH 21 

/* Battery Menu */
#define BAT_STR_X          (uint16_t)226
#define BAT_STR_Y          (uint16_t)144 /* Battery Menu */

/* Sleep Menu */
#define SLP_STR_X          (uint16_t)145
#define SLP_STR_Y          (uint16_t)213

#define SLP_SET_STR_X      (uint16_t)135
#define SLP_SET_STR_Y      (uint16_t)43 

#define MINUTESMAX          260
#define MINUTESMIN          80 /* Sleep Menu */

/* Alarm Menu */
#define ALRM_ELLIPSE_DIM_X 18
#define ALRM_ELLIPSE_DIM_Y 12
#define ALRM_ELLIPSE_CORRECTION_X 8
#define ALRM_BOX_DIM_X     42
#define ALRM_BOX_DIM_Y     30 
#define ALRM_ELLIPSE_STR_CORRECTION_X 10
#define ALRM_ELLIPSE_STR_CORRECTION_Y 6 
#define ALRM_BOX_STR_CORRECTION_Y 4 /* Alarm Menu */

/* Calendar Menu */
#define CLNDR_MAIN_WIDTH 28
#define CLNDR_MAIN_HEIGHT 24
#define CLNDR_MAIN_X       (uint16_t)60
#define CLNDR_MAIN_Y       (uint16_t)73
#define CLNDR_MAIN_X_CORRECTION 7
#define CLNDR_MAIN_Y_CORRECTION 4
#define CLNDR_MAIN_X_CORRECTION_2DIG 1

#define CLNDR_MONTH_WIDTH 80
#define CLNDR_MONTH_HEIGHT 40
#define CLNDR_MONTH_X       (uint16_t)60
#define CLNDR_MONTH_Y       (uint16_t)47
#define CLNDR_MONTH_X_CORRECTION 4
#define CLNDR_MONTH_Y_CORRECTION 6

#define CLNDR_YEAR_WIDTH 46
#define CLNDR_YEAR_HEIGHT 40
#define CLNDR_YEAR_X       (uint16_t)162
#define CLNDR_YEAR_Y       (uint16_t)47
#define CLNDR_YEAR_GAP 2
#define CLNDR_YEAR_X_CORRECTION 2
#define CLNDR_YEAR_Y_CORRECTION 7




// #define CLNDR_MAIN_WIDTH   (uint16_t)169
// #define CLNDR_MAIN_HEIGHT  (uint16_t)164

// #define SUNDAY_X           (uint16_t)95
// #define SUNDAY_Y           (uint16_t)67
// #define SUNDAY_OFFSET      (uint16_t)24

// #define YEAR_X           (uint16_t)111
// #define YEAR_Y           (uint16_t)47
// #define YEAR_OFFSET      (uint16_t)37

// #define MONTH_X           (uint16_t)244
// #define MONTH_Y           (uint16_t)47

// #define NUM_66_WIDTH      (uint16_t)12  /* Calendar Menu */

/* Colors Palette */
#define LCDCORE_GREY    LCD_COLOR(231, 230, 230)
#define LCDCORE_RED     LCD_COLOR(128, 0, 0)
#define LCDCORE_BLUE    LCD_COLOR(48, 192, 255)


/* Extern Images */
extern const BITMAPSTRUCT gui_bat_0;
extern const BITMAPSTRUCT gui_bat_1;
extern const BITMAPSTRUCT gui_slp_0;
extern const BITMAPSTRUCT gui_slp_1;
extern const BITMAPSTRUCT gui_alrm_0;
extern const BITMAPSTRUCT gui_alrm_1;
extern const BITMAPSTRUCT gui_clndr_0;
extern const BITMAPSTRUCT gui_clndr_1;

extern const BITMAPSTRUCT bat_bat_0;
extern const BITMAPSTRUCT bat_bat_1;
extern const BITMAPSTRUCT bat_onof_0;
extern const BITMAPSTRUCT bat_onof_1;
extern const BITMAPSTRUCT slp_slp_0;
extern const BITMAPSTRUCT slp_slp_1;
extern const BITMAPSTRUCT slp_onof_0;
extern const BITMAPSTRUCT slp_onof_1;
extern const BITMAPSTRUCT slp_set_0;
extern const BITMAPSTRUCT slp_set_1;
extern const BITMAPSTRUCT alrm_alrm_1;
extern const BITMAPSTRUCT alrm_alrm_0;


extern const BITMAPSTRUCT bat_sens;
extern const BITMAPSTRUCT moist_sens;
extern const BITMAPSTRUCT wat_sets;

extern const BITMAPSTRUCT mid_clock_0;
extern const BITMAPSTRUCT mid_clock_1;
extern const BITMAPSTRUCT mid_hand_0;
extern const BITMAPSTRUCT mid_hand_1;/* Extern Images */

/* Global Arrays */

/* Coordinate Arrays */
uint16_t gui_menu_coor[8]; /* We can't join the regular  data type to pointer data type on the */
uint16_t bat_menu_coor[4]; /* same array. Hence, we create two separate arrays */
uint16_t slp_menu_coor[6];
uint16_t alrm_menu_coor[20];
uint16_t clndr_menu_coor[6]; /* Coordinate Arrays */

/* Data Arrays */
uint8_t *gui_menu_data[8];
uint8_t *bat_menu_data[4];
uint8_t *slp_menu_data[6];
uint8_t *alrm_menu_data[2];
uint8_t *mid_btn_data[2];
uint8_t *sens_data[3]; /* Data Arrays */

/* Global Arrays */

/* Struct Definition */
typedef struct {
    uint16_t *c;
    uint8_t **d; /* These should be double pointer since we are pointing to a pointer variable.
     Basically when you are pointing to an array, you are pointing to the arrays
     data type. That is why we use regular pointer to point to an int array since
     we treat the int array as an int variable. Similarly, we treat an int pointer
     array as int pointer. Hence, a double pointer will point to an in pointer. */
    uint8_t *v; /* c = coordinates, d = image data array, v = stored value  */
} Menu;

/* Global Function Prototypes */
void LCDCore_Init(void);
void LCDCore_UpdateScreen(void);
//uint8_t LCDCore_getPixelState(uint8_t new_sel);

#ifdef __cplusplus
}
#endif
