/* Includes */
#include "lcd_core.h"
#include "../Encoder/encoder.h"

/* For the random function */
#ifdef  __CC_ARM
#define random() rand()
#endif /* For the random function */

#define Delay(t)  HAL_Delay(t)
#define GetTime() HAL_GetTick()

/* Create 16-bits color from RGB888 or BGR888 */
#define RGB888TORGB565(r, g, b) ((r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3)
#define RGB888TOBGR565(r, g, b) (r >> 3 | (g & 0xFC) << 3 | (b & 0xF8) << 8)

/* LCD Reverse */
#if     LCD_REVERSE16 == 0
#define RD(a)         a
#endif  /* LCD Reverse */

/* 16-bit data byte change */
#if     LCD_REVERSE16 ==       1
#define RD(a)         __REVSH(a)  /* 16-bit data byte change */
#endif

/* Coordinates correction macro */
#define RX(a) a
#define RY(a) (uint16_t)(240-a) /* Coordinates correction macro */

/* Middle button coordinates */
const uint16_t  mid_btn_coor[] = MID_BTN_BR;
const uint16_t sens_coor[] = {BAT_SENS_BR, MOIST_SENS_BR, WAT_SENS_BR}; /* Middle button coordinates */

uint8_t bat_onof_val = 0;
uint8_t slp_onof_val = 0;
uint8_t alrm_val[]= {0, 0, 0, 0, 0, 0, 0, 0, 0}; 
uint8_t clndr_val = 0;

/* Struct Initialization */
Menu gui  = {gui_menu_coor,  gui_menu_data};
Menu bat  = {bat_menu_coor,  bat_menu_data, &bat_onof_val }; 
Menu slp  = {slp_menu_coor,  slp_menu_data}; //&slp_onof_val 
Menu alrm = {alrm_menu_coor, alrm_menu_data}; //&alrm_val 
//Menu clndr = {clndr_menu_coor, clndr_menu_data}; //&clndr_val
Menu mid =  {mid_btn_coor, mid_btn_data};
Menu sens = {sens_coor, sens_data}; /* Struct Initialization */

//Encoder_valTypeDef encoder_val; /* Shared struct with lcd_core.c */

/*-----------------------------     START OF Function Definition       -------------------------------*/

void LCDCore_ClearScreen(void)
{
  BSP_LCD_Clear(LCD_COLOR_WHITE);   /* Clear the screen with white background */
}


void LCDCore_Init(void)
{
  /* Generate the global constant arrays */
  LCDCore_writeCoordinatesArray();
  LCDCore_writeDataArray();
  // LCDCore_writeStringArray();

  /* Do not change */
  #ifdef  __GNUC__
  _impure_ptr->_r48->_rand_next = 0;
  #endif

  BSP_LCD_Init();
  Delay(100);   /* Necessary Delay */
  LCDCore_ClearScreen();

  // /* Write GUI Default */
  // for (uint8_t i = 0; i < 4; i++) {
  //   BSP_LCD_DrawBitmap(RX((&gui)->c[0+2*i]), RY((&gui)->c[1+2*i]), (&gui)->d[0+2*i]);
  // }
  // BSP_LCD_DrawBitmap(RX((&mid)->c[0]), RY((&mid)->c[1]), (&mid)->d[0]);
  // for (uint8_t i = 0; i < 3; i++) {
  //   BSP_LCD_DrawBitmap(RX((&sens)->c[0+2*i]), RY((&sens)->c[1+2*i]), (&sens)->d[i]);
  // } /* End of: Write GUI Default */

 // Delay(3000);

  /*------------------------------- Code Below is only for Testing  -------------------------------*/
 // BSP_LCD_DrawBitmap(RX((&bat)->c[0]),   RY((&bat)->c[1]),(&bat)->d[1]); /* bat_bat -> new_sel = 0 */
 // BSP_LCD_DrawBitmap(RX((&bat)->c[0+2]), RY((&bat)->c[1+2]),(&bat)->d[0+2]); /* bat_onof -> new_sel = 1 */
 // BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]); /* Draw middle button */


(&bat)->v = 0;
(&slp)->v = 0;
// LCDCore_NavigateBat();
LCDCore_NavigateSlp();
// linearMinutes();
// drawRoundedRect(200, 200, LCDCore_GREY, 6, 30, 30); 
// BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
// BSP_LCD_FillCircle(RX(60), RY(60),  2);  
} /* End of: LCDCore_Init(void) */ 


void LCDCore_writeCoordinatesArray(void) 
{

/* Doesn't need to be initialized outside of the function since the values 
  are constant as far as the compiler is concerned */
  uint16_t bat_tlbr[]  = BAT_TLBR;   /* This will be the basis of all the other boxes */
  /* The first two numbers is the top left coordinates and  the last two is the bottom right, (Xi, Yi) */
  uint16_t gui_ctrpt[] = GUI_CTRPT; /* The coordinates of the center */
  uint16_t box_dim[]   = BOX_DIM;     /* Dimensions of the box */

  /* Array Initialization */
  uint16_t bat_ctrpt[2];
  uint16_t slp_bl[2];
  uint16_t alrm_bl[2];
  uint16_t clndr_bl[2]; /* These arrays are need to be initiliazed separately since the compiler won't 
  accept array arithmetic as constants. */

  /* Array Definitions */
  bat_ctrpt[0]           = (bat_tlbr[0] + bat_tlbr[2])/2; bat_ctrpt[1] = (bat_tlbr[1] + bat_tlbr[3])/2;
  const uint16_t box_c2c = bat_ctrpt[1] - gui_ctrpt[1];
  slp_bl[0]              = (gui_ctrpt[0] - box_c2c) - (box_dim[1])/2; slp_bl[1] = gui_ctrpt[1] + (box_dim[0])/2;
  alrm_bl[0]             = bat_tlbr[0];  alrm_bl[1] = bat_tlbr[3] - 2 * box_c2c;
  clndr_bl[0]            = slp_bl[0] + 2 * box_c2c; clndr_bl[1] = slp_bl[1];  
  bat_tlbr[2] = bat_tlbr[2] - box_dim[0]; /* Since the coordinates should be all bottom left, we update lastly the 
  battery coordinate array. We change the 3rd element to the left X. */
  /* End of Array Definitions */


  /*-------------------------------Coordinate Arrays-------------------------------*/
  /*  Alarm Menu Coordinates */
  uint16_t alrm_ctrpt_coor[]  = {

    67  - ALRM_BOX_DIM_X/2,     101 +   ALRM_BOX_DIM_Y/2,   /*  The initial coordinates are the centerpoints of each */
    68  - ALRM_ELLIPSE_DIM_X/2, 143 +   ALRM_ELLIPSE_DIM_Y/2, /*  We add and subtract accordingly to get the bottom left */
    86  - ALRM_ELLIPSE_DIM_X/2, 180 +   ALRM_ELLIPSE_DIM_Y/2, /*  coordinates. */
    119 - ALRM_ELLIPSE_DIM_X/2, 206 +   ALRM_ELLIPSE_DIM_Y/2,
    160 - ALRM_ELLIPSE_DIM_X/2, 215 +   ALRM_ELLIPSE_DIM_Y/2,
    201 - ALRM_ELLIPSE_DIM_X/2, 206 +   ALRM_ELLIPSE_DIM_Y/2,
    233 - ALRM_ELLIPSE_DIM_X/2, 180 +   ALRM_ELLIPSE_DIM_Y/2,
    252 - ALRM_ELLIPSE_DIM_X/2, 143 +   ALRM_ELLIPSE_DIM_Y/2,
    253 - ALRM_BOX_DIM_X/2,     101 +   ALRM_BOX_DIM_Y/2

   }; 

  uint16_t alrm_menu_src_coor[20];  /*  Initialize the array */
  alrm_menu_src_coor[0] = alrm_bl[0];
  alrm_menu_src_coor[1] = alrm_bl[1];

  for (uint8_t i = 0; i < 18; i++) {
    alrm_menu_src_coor[2+i] = alrm_ctrpt_coor[i];
  } /*  Alarm Menu Coordinates */

   /*  Calendar Menu Coordinates */
  uint16_t clndr_menu_src_coor[]  = {
    111, 47,
    111+37, 47,
    244, 47
   }; /*  Calendar Menu Coordinates */


  /*-------------------------------Coordinate Arrays-------------------------------*/

  /* Coordinates memory copy */
  memcpy(&gui_menu_coor[0], &bat_tlbr[2], 2*sizeof(uint16_t));
  memcpy(&gui_menu_coor[2], &slp_bl[0],   2*sizeof(uint16_t));
  memcpy(&gui_menu_coor[4], &alrm_bl[0],  2*sizeof(uint16_t));
  memcpy(&gui_menu_coor[6], &clndr_bl[0], 2*sizeof(uint16_t));
  memcpy(&bat_menu_coor[0], &bat_tlbr[2], 2*sizeof(uint16_t));
  memcpy(&bat_menu_coor[2], &clndr_bl[0], 2*sizeof(uint16_t));
  memcpy(&slp_menu_coor[0], &alrm_bl[0],  2*sizeof(uint16_t));
  memcpy(&slp_menu_coor[2], &slp_bl[0],   2*sizeof(uint16_t));
  memcpy(&slp_menu_coor[4], &bat_tlbr[2], 2*sizeof(uint16_t));  /* The parameters of memcpy are pointers! */

  memcpy(alrm_menu_coor,  alrm_menu_src_coor,  20*sizeof(uint16_t)); /* Do not use sizeof(alrm_menu_src_coor) 
    its returning a wrong value which is 40. */
  memcpy(clndr_menu_coor, clndr_menu_src_coor, 6*sizeof(uint16_t));  /* Coordinates memory copy */

}


void LCDCore_writeDataArray(void) 
{

 /*-------------------------------Data Arrays-------------------------------*/

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

    &alrm_alrm_0,  &alrm_alrm_1
    // &alrm_box_0,   &alrm_box_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_ellip_0, &alrm_ellip_1,
    // &alrm_box_0,   &alrm_box_1

    };  

  // uint8_t* clndr_menu_src_data[]  = {

  //   // &year_0, &year_1,
  //   // &year_0, &year_1,
  //   // &jan_0, &jan_1,
  //   // &feb_0, &feb_1,
  //   // &mar_0, &mar_1,
  //   // &apr_0, &apr_1,
  //   // &may_0, &may_1,
  //   // &jun_0, &jun_1,
  //   // &jul_0, &jul_1,
  //   // &aug_0, &aug_1,
  //   // &sep_0, &sep_1,
  //   // &oct_0, &oct_1,
  //   // &nov_0, &nov_1,
  //   // &dec_0, &dec_1

    // };  /* Layer 1 Data Arrays */

  /* Middle Button Data */
  uint8_t* mid_btn_data_src[]  = {

    &mid_hand_0,   &mid_hand_1
    // &mid_clock_0,  &mid_clock_1

  };  /* Middle Button Data */

  /* Sensor Data */
  uint8_t* sens_data_src[]  = {

    &bat_sens,   &moist_sens,
    &wat_sets

  };  /* Sensor Data */

  /*-------------------------------Data Arrays-------------------------------*/

  /* Data memory copy */
  memcpy(gui_menu_data,   gui_menu_src_data,   8*sizeof(uint32_t));
  memcpy(bat_menu_data,   bat_menu_src_data,   4*sizeof(uint32_t));
  memcpy(slp_menu_data,   slp_menu_src_data,   6*sizeof(uint32_t));
  memcpy(alrm_menu_data,  alrm_menu_src_data,  2*sizeof(uint32_t));
  memcpy(mid_btn_data,    mid_btn_data_src,    2*sizeof(uint32_t)); /* The address is a 32-bit pointer */
  memcpy(sens_data,       sens_data_src,       3*sizeof(uint32_t)); /* Data memory copy */
  }

void LCDCore_NavigateBat(void)
{

 /* Write Battery Menu Defaults */
 LCDCore_ClearScreen();
 BSP_LCD_DrawBitmap(RX((&bat)->c[0]),   RY((&bat)->c[1]),(&bat)->d[1]); /* bat_bat -> new_sel = 0 */
 BSP_LCD_DrawBitmap(RX((&bat)->c[0+2]), RY((&bat)->c[1+2]),(&bat)->d[0+2]); /* bat_onof -> new_sel = 1 */
 BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]); /* Draw middle button */
 strONOF(BAT_STR_X, BAT_STR_Y, (&bat)->v); /* Write the ONOF string depending on the stored value */

 uint8_t new_sel = 0, prev_sel = 0, exit_flag = 0, click_init, click;
 int8_t increment = 0;

click_init = Enc_getClick();  /* Read the initial click value */

while(!exit_flag) {  /* !0 is always 1 and also !(any non zero value) is 0 */

 increment = Enc_GetIncrement();  /* Read the increment */
 click = Enc_getClick();  /* Read the click values */

 /* Check if the increment is in the right direction. If not, no changes shall be made. */
 if ((prev_sel == 0) && increment >= MIN_INCREMENT || (prev_sel == 1) && increment <= -MIN_INCREMENT)
 {

  if (increment <= -MIN_INCREMENT) { /* If the increment is too negative make it negative 1. */
    increment = -1;
  } else if (increment >= MIN_INCREMENT) {
    increment = 1; /* If the increment is too positive make it positive 1. */
  } /* Keep in mind we won't have a negative increment if the previous selection is at zero. Similarly, 
  no positive increment if the previous selection is 1 and also no chanmge if the increment is below 
  the minimum threshold value. */
     
    new_sel = prev_sel + increment;

   /* Write new selection */
   BSP_LCD_DrawBitmap(RX((&bat)->c[0+2*new_sel]), RY((&bat)->c[1+2*new_sel]), \
   (&bat)->d[1+2*new_sel]);

   /* Clear previous selection */
   BSP_LCD_DrawBitmap(RX((&bat)->c[0+2*prev_sel]), RY((&bat)->c[1+2*prev_sel]), \
   (&bat)->d[0+2*prev_sel]);

   if (new_sel == 1) { /* Since ONOF tile is black when new_sel == 1, we update the string to black
   else the string should be updated as blue */
     strONOF(BAT_STR_X, BAT_STR_Y, 2 + (&bat)->v);
   } else {
     strONOF(BAT_STR_X, BAT_STR_Y, (&bat)->v);
     } /* End of: if (new_sel == 1) */
     } /* End of: if ((prev_sel + increment) == 0 ||  (prev_sel + increment) == 1) */

 if (click != click_init) { /* If the initial click has changed */
   switch(new_sel)
   {
   case 1: /* ONOF is selected */
     (&bat)->v = !((&bat)->v); /* !0 is always 1 and also !(any non zero value) is 0 */
     click_init = click; /* Update the initial click value */
     strONOF(BAT_STR_X, BAT_STR_Y, 2 + (&bat)->v);
     break; /* The break will stop the switch into evaluating the next expressions */

   case 0: /* Battery Battery is selected */
     exit_flag = !exit_flag;
     break;
   } /* End of: switch-case */

   } /* End of: if (click != 0) */

  prev_sel = new_sel; /* Update the previous selection */
  Delay(200); /* Necessary delay */

 } /* End of: while(!exit_flag) */

} /* End of: LCDCore_NavigateBat(void) */



void LCDCore_NavigateSlp(void)
{
 /* Write Sleep Menu Defaults */
 LCDCore_ClearScreen();
 BSP_LCD_DrawBitmap(RX((&slp)->c[0]),   RY((&slp)->c[1]),(&slp)->d[0]); /* slp_set -> new_sel = 0 */
 BSP_LCD_DrawBitmap(RX((&slp)->c[0+2]), RY((&slp)->c[1+2]),(&slp)->d[1+2]); /* slp_slp -> new_sel = 1 */
 BSP_LCD_DrawBitmap(RX((&slp)->c[0+4]), RY((&slp)->c[1+4]),(&slp)->d[0+4]); /* slp_onof -> new_sel = 2 */
 BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]); /* Draw middle button */

 strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v); /* Write the ONOF string depending on the stored value */
 strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 0); /* Write the SET string on blue background */

 uint8_t click_init, click;
 uint8_t new_sel = 1;   /* This value needs to be initialized as 1 since if there is no increment, we are effectively
 changing previous selection to 0 if new selection = 0.  */
 uint8_t exit_flag = 0;
 uint8_t prev_sel = 1;
 int8_t increment = 0;

 click_init = Enc_getClick();  /* Read the initial click value */

 while(!exit_flag) {  /* !0 is always 1 and also !(any non zero value) is 0 */

   increment = Enc_GetIncrement();  /* Read the increment. If no new increment then it will return zero. */
   click = Enc_getClick();  /* Read the click values */

    /* Check if the increment is in the right direction. If not, no changes shall be made. */
   if ( ((prev_sel == 0) && increment >= MIN_INCREMENT) || ((prev_sel == 1) && ABS_VAL(increment) >= MIN_INCREMENT) \
    || ((prev_sel == 2) && increment <= -MIN_INCREMENT) )
      {
        if (increment > 0) {
          increment = 1;
        } else if (increment < 0) {
          increment = -1;
        }

     new_sel = prev_sel + increment;

     /* Clear previous selection> This needs to be implemented first before writing the new selection to avoid
     unnecessary overlap */ 
     BSP_LCD_DrawBitmap(RX((&slp)->c[0+2*prev_sel]), RY((&slp)->c[1+2*prev_sel]), \
     (&slp)->d[0+2*prev_sel]);

     /* Write new selection */
     BSP_LCD_DrawBitmap(RX((&slp)->c[0+2*new_sel]), RY((&slp)->c[1+2*new_sel]), \
     (&slp)->d[1+2*new_sel]);

     /* -------- Check ONOF String -------- */

     if (new_sel == 2) { /* Since ONOF tile is black when new_sel == 2, we update the string to black
     else the string should be updated as blue */

      strONOF(SLP_STR_X, SLP_STR_Y, 2 + (&slp)->v);

     } else {

      strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v);

     } /* -------- End of: Check ONOF String -------- */


     /* -------- Check SET String -------- */

     if (new_sel == 0) { /* Since SET tile is black when new_sel == 0, we update the string to black
     else the string should be updated as blue */
    
      strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 1);

     } else {

      strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 0);

     } /* -------- End of: Check SET String -------- */

   } /* End of: if ((prev_sel + increment) == 0 ||  (prev_sel + increment) == 1) */

   if (click != click_init) {

     switch(new_sel)
     {
     case 2: /* ONOF is selected */
       (&slp)->v = !((&slp)->v);
       click_init = click; /* Update the initial click value */
       strONOF(SLP_STR_X, SLP_STR_Y, 2 + (&slp)->v);
       break;

     case 1: /* Sleep Sleep is selected */
       exit_flag = !exit_flag;
       break;

     case 0: /* SET is selected */
       linearMinutes();

       LCDCore_ClearScreen();  /* Clear the screen */

       /* Load all the previous menu back */
       BSP_LCD_DrawBitmap(RX((&slp)->c[0+4]), RY((&slp)->c[1+4]),(&slp)->d[0+4]); /* slp_onof -> new_sel = 2 */
       BSP_LCD_DrawBitmap(RX((&slp)->c[0+2]), RY((&slp)->c[1+2]),(&slp)->d[0+2]); /* slp_slp -> new_sel = 1 */
       BSP_LCD_DrawBitmap(RX((&slp)->c[0]),   RY((&slp)->c[1]),(&slp)->d[1]); /* slp_set -> new_sel = 0 */
       BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]),(&mid)->d[0]); /* Draw middle button */

       strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v); /* Write the ONOF string depending on the stored value */
       strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 1); /* Write the SET string on blue background */
       break;
     } /* End of: switch-case */

   } /* End of: if (click != 0) */

    prev_sel = new_sel; /* Update the previous selection */
    Delay(200); /* Necessary delay */

 } /* End of: while(!exit_flag) */

} /* End of: LCDCore_NavigateSlp(void) */


// void LCDCore_NavigateAlrm(void)
// {

// /* Write Alarm Menu Defaults */
// LCDCore_ClearScreen();
// BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]),(&alrm)->d[1]); /*alarm_alarm -> new_sel = 0 */
//                                                                         /*alarm_alarm_box1 -> new_sel = 1 */
//                                                                         /*alarm_alarm_ellipse -> new_sel = 2- 8 */
//                                                                         /*alarm_alarm_box2 -> new_sel = 9 */

// for (uint8_t i = 0; i < 7; i++) { /* Draw the ellipses */
//   drawDaysEllipse((&alrm)->c[4+2*i], (&alrm)->c[5+2*i], LCDCore_Grey);
// }

// BSP_LCD_DrawBitmap(RX((&mid)->c[0]), RY((&mid)->c[1]),(&mid)->d[2]);  Draw middle button 


// /* Update the the ellipses */
// for (uint8_t j = 0; j < 8; j++) {
//  if ((&slp)->v[j] == 1) {
//    BSP_LCD_DisplayChar( RX((&alrm)->c[2+2*j]), RY((&mid)->c[3+2*j]), 88);  /* Draw an X */
//  }
// }

// // int array[10], array_size;
// // ...
// // array_size = sizeof(array)/sizeof(int);

// // BSP_LCD_DisplayChar( RX((&alrm)->c[2+2*i] ),RY((&mid)->c[1]), 88)  /* Draw an X */

// //     &alrm_alrm_0,  &alrm_alrm_1,
// //     &alrm_box_0,   &alrm_box_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_ellip_0, &alrm_ellip_1,
// //     &alrm_box_0,   &alrm_box_1
// }



void strONOF(uint8_t x_pos, uint8_t y_pos, uint8_t val) 
{
  char str_on[] = "ON";
  char str_of[] = "OF";

  if (val > 1) {
    val = val - 2;

    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCDCore_BLUE);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

    if (val == ON) {
      BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_on, LEFT_MODE);
    }
    else if (val == OFF)
    {
      BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_of, LEFT_MODE);
    }

  } /* End of:  if (val > 1) */
  else {

    BSP_LCD_SetBackColor(LCDCore_BLUE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

    if (val == ON) {
      BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_on, LEFT_MODE);
    }
    else if (val == OFF)
    {
      BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_of, LEFT_MODE);
    }
  } /* End of:  if (val > 1), else */

} /* End of: void batONOF(uint8_t val) */


void strSET(uint8_t x_pos, uint8_t y_pos, uint8_t val) 
{

  char str_set[] = "SET";

  if (val == 1) {

    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCDCore_BLUE);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  } /* End of:  if (val == 1) */
  else if (val == 0) {

    BSP_LCD_SetBackColor(LCDCore_BLUE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  } /* End of:  else if (val == 0) */

  BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_set, LEFT_MODE);

} /* End of: strSET(uint8_t x_pos, uint8_t y_pos, uint8_t val)  */

void drawDaysEllipse(uint8_t x_pos, uint8_t y_pos, uint16_t color) 
{

  BSP_LCD_SetTextColor(color);
  BSP_LCD_DrawEllipse(RX(x_pos), RY(y_pos), 26, 17);

}

void drawRoundedRect(uint8_t x_pos, uint8_t y_pos, uint16_t color, uint8_t radius, uint8_t height, uint8_t width) 
{

  x_pos = x_pos - width;

  BSP_LCD_SetTextColor(color);
  BSP_LCD_FillRect(RX(x_pos + radius), RY(y_pos), width - 2*radius, height);  /* Draw the initial rectangle */
  BSP_LCD_FillRect(RX(x_pos), RY(y_pos + radius), width, height - 2 * radius);  /* Draw the initial rectangle */

  BSP_LCD_FillCircle(RX(x_pos + radius), RY(y_pos + radius),  radius);   /* Draw the corner circles */
  BSP_LCD_FillCircle(RX(x_pos + radius), RY(y_pos  + height - radius),  radius);
  BSP_LCD_FillCircle(RX(x_pos + width - radius), RY(y_pos + radius),  radius);
  BSP_LCD_FillCircle(RX(x_pos + width - radius), RY(y_pos  + height - radius),  radius);

}

void linearMinutes(void)
{
  LCDCore_ClearScreen();
  char str_minutes[] = "MINUTES: ";
  char str_num[3];

  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  BSP_LCD_DisplayStringAt(RX(90), RY(90), (uint8_t *)&str_minutes, LEFT_MODE);

  BSP_LCD_SetTextColor(LCDCore_BLUE);
  for (uint16_t i = 0; i < ((260-80)/2); i++) {
    BSP_LCD_FillCircle(RX(260-i), RY(150), 9); /* forward 260, 259, 258, 257, ... 60 */
  }                                            /* reverse 60, 61, 62, 63, ... 260 */


  uint8_t click_init, click;
  uint8_t exit_flag = 0;
  int8_t increment = 0;
  uint16_t circle_init = 260 - ((260-80)/2); /* From the default initialization, 260-100  */
  /* This variables needs to be 16-bit since the maximum value is 260 to prevent overflow. */

  click_init = Enc_getClick(); 

  while(!exit_flag) {  

  increment = Enc_GetIncrement();  
  click = Enc_getClick();  

    if ((circle_init >= linearMinutesMIN) && (circle_init <= linearMinutesMAX)) { 
    /* Check if the values are still 1 step away from the MIN and MAX values  */

      if ((increment > 0) && (circle_init != linearMinutesMAX)) {  /* reverse 160, 161, 162,... 260 */
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillCircle(RX(circle_init++), RY(150), 9);
        BSP_LCD_SetTextColor(LCDCore_BLUE);
        BSP_LCD_FillCircle(RX(circle_init), RY(150), 9);

      } else if ((increment < 0) && (circle_init != linearMinutesMIN)) { /* forward */
        BSP_LCD_SetTextColor(LCDCore_BLUE);
        BSP_LCD_FillCircle(RX(circle_init--), RY(150), 9);
        BSP_LCD_FillCircle(RX(circle_init), RY(150), 9);
      }
      uint16_t minutes = 260 - circle_init;
      sprintf(str_num, "%d", minutes);
      
      if (minutes < 10) {

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130+2*19), RY(120), (uint8_t *)&str_num, LEFT_MODE);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillRect(RX(130+1*19), RY(120), 2*17, 17);

      } else if (minutes < 100) {

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130+1*19), RY(120), (uint8_t *)&str_num, LEFT_MODE); 
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillRect(RX(130+2*19), RY(120), 1*17, 17);  

      } else {
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130), RY(120), (uint8_t *)&str_num, LEFT_MODE); 
      }

    } /* End of: if ((circle_init >= linearMinutesMIN) && (circle_init <= linearMinutesMAX)) */

    if (click != click_init) {
      exit_flag = !exit_flag;

    } /* End of: if (click != click_init) */

  } /* End of:   while(!exit_flag) */

} /* End of: linearMinutes(void) */