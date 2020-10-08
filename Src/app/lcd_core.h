#ifndef LCD_CORE_H
#define LCD_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "lcd.h"
#include "bmp.h"
#include "stm32_adafruit_lcd.h" /* BSP_LCD Function Definitions */

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
#define alrm_box_0    ALRM_BOX_0
#define alrm_box_1    ALRM_BOX_1
#define alrm_ellip_0  ALRM_ELLIP_0
#define alrm_ellip_1  ALRM_ELLIP_1

#define clndr_clndr_0 CLNDR_CLNDR_0
#define clndr_clndr_1 CLNDR_CLNDR_1
// #define date_0        DATE_0  @Wrong COLOR!
#define date_1        DATE_1

#define bat_sens      BAT_SENS
#define moist_sens    MOIST_SENS
#define wat_sets      WAT_SENS

#define mid_clock_0   MID_CLOCK_0
#define mid_clock_1   MID_CLOCK_1
#define mid_hand_0    MID_HAND_0
#define mid_hand_1    MID_HAND_1 /* Images Macros */

#define bat_off_0_c BAT_OFF_0_C
#define bat_off_1_c BAT_OFF_1_C
#define bat_on_0_c  BAT_ON_0_C
#define bat_on_1_c  BAT_ON_1_C

#define set_0_c     SET_0_C
#define set_1_c     SET_1_C
#define slp_off_0_c SLP_OFF_0_C
#define slp_off_1_c SLP_OFF_1_C
#define slp_on_0_c  SLP_ON_0_C
#define slp_on_1_c  SLP_ON_1_C

/* LCD    Dimensions in Pixels */
#define WIDTH  320
#define HEIGHT 240

/* LCD Initialization Macros */
#define BAT_TLBR           {84,      167,      236, 230}    /* Coodinates of top left and top right battery box */
#define GUI_CTRPT          {WIDTH/2, HEIGHT/2}
#define BOX_DIM            {152,     63}

#define ALRM_ELLIPSE_DIM_X 28
#define ALRM_ELLIPSE_DIM_Y 20
#define ALRM_BOX_DIM_X     42
#define ALRM_BOX_DIM_Y     46

#define MID_BTN_BR         {201,     161}

#define BAT_SENS_BR        301,     25
#define MOIST_SENS_BR      44,     232
#define WAT_SENS_BR        311,     234

/* Extern Images */
extern const BITMAPSTRUCT gui_default;

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

extern const BITMAPSTRUCT alrm_ellip_0;
extern const BITMAPSTRUCT alrm_ellip_1;
extern const BITMAPSTRUCT alrm_box_0;
extern const BITMAPSTRUCT alrm_box_1; 

extern const BITMAPSTRUCT bat_sens; 
extern const BITMAPSTRUCT moist_sens; 
extern const BITMAPSTRUCT wat_sets; 

extern const BITMAPSTRUCT mid_clock_0; 
extern const BITMAPSTRUCT mid_clock_1; 
extern const BITMAPSTRUCT mid_hand_0; 
extern const BITMAPSTRUCT mid_hand_1; 

extern const BITMAPSTRUCT clndr_blnk; 
extern const BITMAPSTRUCT clndr_clndr_0; 
extern const BITMAPSTRUCT clndr_clndr_1; 
extern const BITMAPSTRUCT date_0; 
extern const BITMAPSTRUCT date_1; /* Extern Images */

/* Global Arrays */
const uint16_t  gui_menu_coor[8];   /* We can't join the regular  data type to pointer data type on the */
const uint16_t  bat_menu_coor[4];   /* same array. Hence, we create two separate arrays */
const uint16_t  slp_menu_coor[6];
const uint16_t  alrm_menu_coor[20];
const uint16_t  clndr_menu_coor[12];


const uint8_t* gui_menu_data[8];
const uint8_t* bat_menu_data[4];
const uint8_t* alrm_menu_data[20];
const uint8_t* slp_menu_data[6];
const uint8_t* clndr_menu_data[12]; 

const uint8_t* mid_btn_data[4]; 
const uint8_t* sens_data[4]; 

/* Global Arrays */

/* Struct Definition */
typedef struct { 
    uint16_t* c;
    uint8_t** d; /* These should be double pointer since we are pointing to a pointer variable. 
                    Basically when you are pointing to an array, you are pointing to the arrays
                    data type. That is why we use regular pointer to point to an int array since 
                    we treat the int array as an int variable. Similarly, we treat an int pointer 
                    array as int pointer. Hence, a double pointer will point to an in pointer. */
    // uint8_t* v; /* c = coordinates, d = image data array, v = stored value  */
} Menu; 


/* Global Function Prototypes */
void LCDCore_Init(void);
void LCDCore_UpdateScreen(void);

#endif /*LCD_CORE_H */
