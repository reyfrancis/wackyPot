/* Includes */
#include "lcd_core.h"

/* For the random function */
#ifdef  __CC_ARM
#define random() rand()
#endif

#define Delay(t)  HAL_Delay(t)
#define GetTime() HAL_GetTick()

/* Create 16-bits color from RGB888 or BGR888 */
#define RGB888TORGB565(r, g, b) ((r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3)
#define RGB888TOBGR565(r, g, b) (r >> 3 | (g & 0xFC) << 3 | (b & 0xF8) << 8)

/* LCD Reverse */
#if     LCD_REVERSE16 == 0
#define RD(a)         a
#endif

/* 16-bit data byte change */
#if     LCD_REVERSE16 ==       1
#define RD(a)         __REVSH(a)
#endif

/* Coordinates correction macro */
#define RX(a) (uint16_t)(320-a)
#define RY(a) (uint16_t)(240-a)

/* Middle button coordinates */
const uint16_t  mid_btn_coor[] = MID_BTN_BR;
const uint16_t sens_coor[] = {BAT_SENS_BR, MOIST_SENS_BR, WAT_SENS_BR};

const uint8_t onof_arr[][4] = {"ON", "OFF"};


uint8_t bat_onof_val = 0;
uint8_t slp_onof_val = 0;
uint8_t alrm_val[]= {0, 0, 0, 0, 0, 0, 0, 0, 0}; 

/* Struct Initialization */
Menu gui  = {gui_menu_coor,  gui_menu_data};
Menu bat  = {bat_menu_coor,  bat_menu_data }; //&bat_onof_val
Menu alrm = {alrm_menu_coor, alrm_menu_data}; //&alrm_val
Menu slp  = {slp_menu_coor,  slp_menu_data}; //&slp_onof_val
Menu mid = {mid_btn_coor, mid_btn_data};
Menu sens = {sens_coor, sens_data};



/*-----------------------------     START OF Function Definition       -------------------------------*/

void LCDCore_ClearScreen(void)
{
  BSP_LCD_Clear(LCD_COLOR_WHITE);   /* Clear the screen with white background */
}

void LCDCore_Init(void)
{

/* Doesn't need to be initialized outside of the function since the values 
are constant as far as the compiler is concerned */
uint16_t bat_tlbr[]  = BAT_TLBR;   /* This will be the basis of all the other boxes */
/* The first two numbers is the top left coordinates and  the last two is the bottom right, (Xi, Yi) */
uint16_t gui_ctrpt[] = GUI_CTRPT; /* The coordinates of the center */
uint16_t box_dim[]   = BOX_DIM;     /* Dimensions of the box */

/* Array Initialization */
uint16_t bat_ctrpt[2];
uint16_t slp_br[2];
uint16_t alrm_br[2];
uint16_t clndr_br[2]; /* These arrays are need to be initiliazed separately since the compiler won't 
accept array arithmetic as constants. */

/* Array Definitions */
bat_ctrpt[0]           = (bat_tlbr[0] + bat_tlbr[2])/2; bat_ctrpt[1] = (bat_tlbr[1] + bat_tlbr[3])/2;
const uint16_t box_c2c = bat_ctrpt[1] - gui_ctrpt[1];
slp_br[0]              = (gui_ctrpt[0] - box_c2c) + (box_dim[1])/2; slp_br[1] = gui_ctrpt[1] + (box_dim[0])/2;
alrm_br[0]             = bat_tlbr[2];  alrm_br[1] = bat_tlbr[3] - 2 * box_c2c;
clndr_br[0]            = slp_br[0] + 2 * box_c2c; clndr_br[1] = slp_br[1];  /* Array Definitions */


/* Layer 1 Data Arrays */
uint8_t* gui_menu_src_data[] = {

  &gui_bat_0,   &gui_bat_1,
  &gui_slp_0,   &gui_slp_1,
  &gui_alrm_0,  &gui_alrm_1,
  &gui_clndr_0, &gui_clndr_1
};

uint8_t* bat_menu_src_data[] = {

  &bat_bat_0,  &bat_bat_1,
  &bat_onof_0, &bat_onof_1
};

uint8_t* slp_menu_src_data[] = {

  &slp_set_0,  &slp_set_1,
  &slp_slp_0,  &slp_slp_1, 
  &slp_onof_0, &slp_onof_1

};  

uint8_t* alrm_menu_src_data[]  = {

  &alrm_alrm_0,  &alrm_alrm_1,
  &alrm_box_0,   &alrm_box_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_ellip_0, &alrm_ellip_1,
  &alrm_box_0,   &alrm_box_1

};  /* Layer 1 Data Arrays */

/* Middle Button Data */
uint8_t* mid_btn_data_src[]  = {
  &mid_hand_0,   &mid_hand_1,
  &mid_clock_0,  &mid_clock_1
};

/* Sensor Data */
uint8_t* sens_data_src[]  = {
  &bat_sens,   &moist_sens,
  &wat_sets
};

/* Coordinate Array for Alarm Menu */
uint16_t alrm_ctrpt_coor[]  = {

  67  + ALRM_BOX_DIM_X/2,     101 +   ALRM_BOX_DIM_Y/2,
  68  + ALRM_ELLIPSE_DIM_X/2, 143 +   ALRM_ELLIPSE_DIM_Y/2,
  86  + ALRM_ELLIPSE_DIM_X/2, 180 +   ALRM_ELLIPSE_DIM_Y/2,
  119 + ALRM_ELLIPSE_DIM_X/2, 206 +   ALRM_ELLIPSE_DIM_Y/2,
  160 + ALRM_ELLIPSE_DIM_X/2, 215 +   ALRM_ELLIPSE_DIM_Y/2,
  201 + ALRM_ELLIPSE_DIM_X/2, 206 +   ALRM_ELLIPSE_DIM_Y/2,
  233 + ALRM_ELLIPSE_DIM_X/2, 180 +   ALRM_ELLIPSE_DIM_Y/2,
  252 + ALRM_ELLIPSE_DIM_X/2, 143 +   ALRM_ELLIPSE_DIM_Y/2,
  253 + ALRM_BOX_DIM_X/2,     101 +   ALRM_BOX_DIM_Y/2
 };

uint16_t alrm_menu_src_coor[20];

alrm_menu_src_coor[0] = alrm_br[0];
alrm_menu_src_coor[1] = alrm_br[1];

for (uint8_t i = 0; i < 18; i++) {
  alrm_menu_src_coor[2+i] = alrm_ctrpt_coor[i];
} /* Coordinate Array for Alarm Menu */

/* Coordinates memory copy */
memcpy(&gui_menu_coor[0], &bat_tlbr[2], 2*sizeof(uint16_t));
memcpy(&gui_menu_coor[2], &slp_br[0],   2*sizeof(uint16_t));
memcpy(&gui_menu_coor[4], &alrm_br[0],  2*sizeof(uint16_t));
memcpy(&gui_menu_coor[6], &clndr_br[0], 2*sizeof(uint16_t));

memcpy(&bat_menu_coor[0], &bat_tlbr[2], 2*sizeof(uint16_t));
memcpy(&bat_menu_coor[2], &clndr_br[0], 2*sizeof(uint16_t));

memcpy(&slp_menu_coor[0], &alrm_br[0],  2*sizeof(uint16_t));
memcpy(&slp_menu_coor[2], &slp_br[0],   2*sizeof(uint16_t));
memcpy(&slp_menu_coor[4], &bat_tlbr[2], 2*sizeof(uint16_t));  /* The parameters of memcpy are pointers! */

memcpy(alrm_menu_coor, alrm_menu_src_coor, 20*sizeof(uint16_t));  /* Do not use sizeof(alrm_menu_src_coor) 
its returning a wrong value which is 40. */

/* Data memory copy */
memcpy(gui_menu_data,  gui_menu_src_data,  8*sizeof(uint32_t));
memcpy(bat_menu_data,  bat_menu_src_data,  4*sizeof(uint32_t));
memcpy(slp_menu_data,  slp_menu_src_data,  6*sizeof(uint32_t));
memcpy(alrm_menu_data, alrm_menu_src_data, 20*sizeof(uint32_t));

memcpy(mid_btn_data,   mid_btn_data_src,   4*sizeof(uint32_t)); /* The address is a 32-bit pointer */

memcpy(sens_data, sens_data_src, 4*sizeof(uint32_t));

  /* Do not change */
  #ifdef  __GNUC__
  _impure_ptr->_r48->_rand_next = 0;
  #endif

  BSP_LCD_Init();
  Delay(100);   /* Necessary Delay */
  LCDCore_ClearScreen();

// /* Write GUI Default */
//   for (uint8_t i = 0; i < 4; i++) {
//     BSP_LCD_DrawBitmap(RX((&gui)->c[0+2*i]), RY((&gui)->c[1+2*i]), (&gui)->d[0+2*i]);
//   }
//   BSP_LCD_DrawBitmap(RX((&mid)->c[0]), RY((&mid)->c[1]), (&mid)->d[0]);
//   for (uint8_t i = 0; i < 3; i++) {
//     BSP_LCD_DrawBitmap(RX((&sens)->c[0+2*i]), RY((&sens)->c[1+2*i]), (&sens)->d[i]);
//   }/* Write GUI Default */


    BSP_LCD_DrawBitmap(0, 0, &clndr_blnk);




}


// void LCDCore_UpdateScreen(void) 
// {


//   while(1) 
//   {
//  	BSP_LCD_DrawBitmap(0, 0, (uint8_t *)&gui_default);
// 	BSP_LCD_DrawBitmap(0, 0, (uint8_t *)&slp_slp_0);
//  	Delay(100);

//   BSP_LCD_SetTextColor(RD(LCD_COLOR_BLACK));
//   BSP_LCD_FillCircle(10, 10, 8);
//   BSP_LCD_SetTextColor(RD(LCD_COLOR_RED));
//   BSP_LCD_FillCircle(310, 230, 8);

//   BSP_LCD_SetTextColor(RD(LCD_COLOR_RED));
//   BSP_LCD_FillCircle(RX(15), RY(15), 8);

//   BSP_LCD_SetTextColor(RD(LCD_COLOR_BLACK));
//   BSP_LCD_FillCircle(RX(*(gui_menu[0+4])), RY(*(gui_menu[1+4])), 8);
//   Delay(1000);

//  BSP_LCD_SetTextColor(RD(LCD_COLOR_BLACK));
//  BSP_LCD_FillCircle(RX(*(gui_menu[0])), RY(*(gui_menu[1])), 8);
//  Delay(1000);

//   BSP_LCD_SetTextColor(RD(LCD_COLOR_RED));
//  BSP_LCD_FillCircle(RX(*(gui_menu[0+4])), RY(*(gui_menu[1+4])), 8);
//  Delay(1000);


// BSP_LCD_DrawBitmap(RX(gui_menu[0+12]), RY(gui_menu[1+12]), (uint8_t *)gui_menu[3+12-1]);
//  BSP_LCD_DrawBitmap(RX((&gui)->c[0]), RY((&gui)->c[1]), (&gui)->d[1]);
//  Delay(100);

//  The 'arrow' operator is syntactic sugar. bar->member is the same as (*bar).member. 
//  One reason for the difference is maintainability.

//  BSP_LCD_DrawBitmap(RX((gui_menu[0])), RY((gui_menu[1])), (uint8_t *)gui_menu[3-1]);
//  BSP_LCD_DrawBitmap(RX((gui_menu[0+4])), RY((gui_menu[1+4])), (uint8_t *)gui_menu[3+4]);
//  Delay(100);

//  BSP_LCD_DrawBitmap(RX((gui_menu[0+4])), RY(*(gui_menu[1+4])), (uint8_t *)gui_menu[3+4-1]);
//  BSP_LCD_DrawBitmap(RX((gui_menu[0+8])), RY(*(gui_menu[1+8])), (uint8_t *)gui_menu[3+8]);
//  Delay(100);

//  BSP_LCD_DrawBitmap(RX((gui_menu[0+8])), RY((gui_menu[1+8])), (uint8_t *)gui_menu[3+8-1]);
//  BSP_LCD_DrawBitmap(RX((gui_menu[0+12])), RY(*(gui_menu[1+12])), (uint8_t *)gui_menu[3+12]);
//  Delay(100);


    // }

// }

// uint8_t lcd_states[] = {
//         0, 0, 0, 0	/* Battery Charge, Sleep, Calendar */
// };


// int sums[3];
// for( int i = 0; i < 3; i++ ) {
//   sums[i] = 0;
//   for( int j = 0; j < 3; j++ )
//     sums[i] += Big_array[i][j];
// }

// /* Pixel Test */
// uint32_t PixelTest(uint32_t n)          
// {
//     BSP_LCD_DrawBitmap(0, 0, (uint8_t *)&rombitmap);
// }


// void UpdateScreen(uint8_t prev_sel, uint8_t increment, uint8_t layer)
// {
//   uint8_t new_sel = (prev_sel + increment) % 4;

//   if (layer == 1 ) {

//     /* Write new selection */
//     BSP_LCD_DrawBitmap(RX((&gui)->c[0+2*new_sel]), RY((&gui)->c[1+2*new_sel]), \
//       (&gui)->d[1+2*new_sel];

//     /* Clear previous selection */
//     BSP_LCD_DrawBitmap(RX((&gui)->c[0+2*prev_sel]), RY((&gui)->c[1+2*prev_sel]), \
//       (&gui)->d[0+2*prev_sel];

//     if (click != 0) {

//       switch(i)
//       {

//         /* Default Load Screen for Layer 2 */
//         case '1':
//         LCDCore_ClearScreen();
        // BSP_LCD_DrawBitmap(RX((&bat)->c[0]),   RY((&bat)->c[1]),(&bat)->d[1]);
        // BSP_LCD_DrawBitmap(RX((&bat)->c[0+2]), RY((&bat)->c[1+2]),(&bat)->d[0+2]);
        // BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]);
//         break;

//         case '2':
        // LCDCore_ClearScreen();
        // BSP_LCD_DrawBitmap(RX((&slp)->c[0]),   RY((&slp)->c[1]),(&slp)->d[0]);
        // BSP_LCD_DrawBitmap(RX((&slp)->c[0+2]), RY((&slp)->c[1+2]),(&slp)->d[1+2]);
        // BSP_LCD_DrawBitmap(RX((&slp)->c[0+4]), RY((&slp)->c[1+4]),(&slp)->d[0+4]);
        // BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]);
        // BSP_LCD_DisplayStringAt(266, 133, &onof_arr[*((&slp)->v)])
//         break;

//         case '3':
        // LCDCore_ClearScreen();
        //   BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]),(&alrm)->d[1]);
        // for (uint8_t i = 0; i < 9; i++) {
        // BSP_LCD_DrawBitmap(RX((&alrm)->c[2+2*i]), RY((&alrm)->c[3+2*i]),(&alrm)->d[2+2*i]);
        // }
        // BSP_LCD_DrawBitmap(RX((&mid)->c[0]), RY((&mid)->c[1]),(&mid)->d[2]);
//         break;

//         case '3':
//         BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]),(&alrm)->d[1]);
//         for (uint8_t i = 0; i < 10; i++) {
//         BSP_LCD_DrawBitmap(RX((&alrm)->c[2+2*i]), RY((&alrm)->c[2+2*i]),(&alrm)->d[2+2*i]);
//         }
//         BSP_LCD_DrawBitmap(RX((&mid)->c[0]), RY((&mid)->c[1]),(&mid)->d[2]);
//         break;

// // operator doesn't match any case constant +, -, *, /
//         default:
//         // printf("Error! operator is not correct");
//       }
//     } 

// } else if (layer == 2) {
//   BSP_LCD_DrawBitmap(RX(*gui_menu[0+4*i][0+3*i]), RY(*gui_menu[0+4*i][1+3*i]), (uint8_t *)*gui_menu[0+4*i][3+3*i]);
//   BSP_LCD_DrawBitmap(RX(*gui_menu[0+4*i][0+3*current_i]), RY(*gui_menu[0+4*i][1+3*current_i]), \
//     (uint8_t *)*gui_menu[0+4*i][3+ (3*current_i) - 1]);
// }




// }
