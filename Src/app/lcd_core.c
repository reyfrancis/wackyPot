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
uint8_t slp_val[] = {0, 0};
uint8_t alrm_val[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t clndr_val = 0;

/* Struct Initialization */
Menu gui  = {gui_menu_coor,  gui_menu_data};
Menu bat  = {bat_menu_coor,  bat_menu_data, &bat_onof_val};
Menu slp  = {slp_menu_coor,  slp_menu_data, &slp_val};
Menu alrm = {alrm_menu_coor, alrm_menu_data, &alrm_val}; //
Menu clndr = {NULL, NULL, &clndr_val};
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


  (&clndr)->v[0] = 3;
  (&clndr)->v[1] = 20;
  (&clndr)->v[2] = 20;

  LCDCore_NavigateClndr();
// linearMinutes();
// drawRoundedRect(200, 200, LCDCORE_GREY, 6, 30, 30);
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
  bat_ctrpt[0]           = (bat_tlbr[0] + bat_tlbr[2]) / 2; bat_ctrpt[1] = (bat_tlbr[1] + bat_tlbr[3]) / 2;
  const uint16_t box_c2c = bat_ctrpt[1] - gui_ctrpt[1];
  slp_bl[0]              = (gui_ctrpt[0] - box_c2c) - (box_dim[1]) / 2; slp_bl[1] = gui_ctrpt[1] + (box_dim[0]) / 2;
  alrm_bl[0]             = bat_tlbr[0];  alrm_bl[1] = bat_tlbr[3] - 2 * box_c2c;
  clndr_bl[0]            = slp_bl[0] + 2 * box_c2c; clndr_bl[1] = slp_bl[1];
  bat_tlbr[2] = bat_tlbr[2] - box_dim[0]; /* Since the coordinates should be all bottom left, we update lastly the
battery coordinate array. We change the 3rd element to the left X. */
  /* End of Array Definitions */


  /*-------------------------------Coordinate Arrays-------------------------------*/
  /*  Alarm Menu Coordinates */
  uint16_t alrm_ctrpt_coor[]  = {

    67  - ALRM_BOX_DIM_X / 2,     101 +   ALRM_BOX_DIM_Y / 2, /*  The initial coordinates are the centerpoints of each */
    68  - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 143 +   ALRM_ELLIPSE_DIM_Y / 2, /*  We add and subtract accordingly to get the bottom left */
    86  - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 180 +   ALRM_ELLIPSE_DIM_Y / 2, /*  coordinates. */
    119 - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 206 +   ALRM_ELLIPSE_DIM_Y / 2,
    160 - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 215 +   ALRM_ELLIPSE_DIM_Y / 2,
    201 - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 206 +   ALRM_ELLIPSE_DIM_Y / 2,
    233 - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 180 +   ALRM_ELLIPSE_DIM_Y / 2,
    252 - ALRM_ELLIPSE_CORRECTION_X - ALRM_ELLIPSE_DIM_X / 2, 143 +   ALRM_ELLIPSE_DIM_Y / 2,
    253 - ALRM_BOX_DIM_X / 2,     101 +   ALRM_BOX_DIM_Y / 2

  };

  uint16_t alrm_menu_src_coor[20];  /*  Initialize the array */
  alrm_menu_src_coor[0] = alrm_bl[0];
  alrm_menu_src_coor[1] = alrm_bl[1];

  for (uint8_t i = 0; i < 18; i++) {
    alrm_menu_src_coor[2 + i] = alrm_ctrpt_coor[i];
  } /*  Alarm Menu Coordinates */

  /*  Calendar Menu Coordinates */
  uint16_t clndr_menu_src_coor[]  = {
    111, 47,
    111 + 37, 47,
    244, 47
  }; /*  Calendar Menu Coordinates */


  /*-------------------------------Coordinate Arrays-------------------------------*/

  /* Coordinates memory copy */
  memcpy(&gui_menu_coor[0], &bat_tlbr[2], 2 * sizeof(uint16_t));
  memcpy(&gui_menu_coor[2], &slp_bl[0],   2 * sizeof(uint16_t));
  memcpy(&gui_menu_coor[4], &alrm_bl[0],  2 * sizeof(uint16_t));
  memcpy(&gui_menu_coor[6], &clndr_bl[0], 2 * sizeof(uint16_t));
  memcpy(&bat_menu_coor[0], &bat_tlbr[2], 2 * sizeof(uint16_t));
  memcpy(&bat_menu_coor[2], &clndr_bl[0], 2 * sizeof(uint16_t));
  memcpy(&slp_menu_coor[0], &alrm_bl[0],  2 * sizeof(uint16_t));
  memcpy(&slp_menu_coor[2], &slp_bl[0],   2 * sizeof(uint16_t));
  memcpy(&slp_menu_coor[4], &bat_tlbr[2], 2 * sizeof(uint16_t)); /* The parameters of memcpy are pointers! */

  memcpy(alrm_menu_coor,  alrm_menu_src_coor,  20 * sizeof(uint16_t)); /* Do not use sizeof(alrm_menu_src_coor)
its returning a wrong value which is 40. */
  memcpy(clndr_menu_coor, clndr_menu_src_coor, 6 * sizeof(uint16_t)); /* Coordinates memory copy */

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

  };

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
  memcpy(gui_menu_data,   gui_menu_src_data,   8 * sizeof(uint32_t));
  memcpy(bat_menu_data,   bat_menu_src_data,   4 * sizeof(uint32_t));
  memcpy(slp_menu_data,   slp_menu_src_data,   6 * sizeof(uint32_t));
  memcpy(alrm_menu_data,  alrm_menu_src_data,  2 * sizeof(uint32_t));
  memcpy(mid_btn_data,    mid_btn_data_src,    2 * sizeof(uint32_t)); /* The address is a 32-bit pointer */
  memcpy(sens_data,       sens_data_src,       3 * sizeof(uint32_t)); /* Data memory copy */
}

void LCDCore_NavigateBat(void)
{

  /* Write Battery Menu Defaults */
  LCDCore_ClearScreen();
  BSP_LCD_DrawBitmap(RX((&bat)->c[0]),   RY((&bat)->c[1]), (&bat)->d[1]); /* bat_bat -> new_sel = 0 */
  BSP_LCD_DrawBitmap(RX((&bat)->c[0 + 2]), RY((&bat)->c[1 + 2]), (&bat)->d[0 + 2]); /* bat_onof -> new_sel = 1 */
  BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]), (&mid)->d[0]); /* Draw middle button */
  strONOF(BAT_STR_X, BAT_STR_Y, (&bat)->v); /* Write the ONOF string depending on the stored value */

  /*
  Multiline Declaration:
  https://stackoverflow.com/questions/6838408/how-can-i-declare-and-define-multiple-variables-in-one-line-using-c/6838447
  */
  uint8_t new_sel = 0, prev_sel = 0, exit_flag = 0, click_init;
  int8_t increment = 0;

  click_init = Enc_getClick();  /* Read the initial click value */

  while (!exit_flag) { /* !0 is always 1 and also !(any non zero value) is 0 */

    increment = Enc_GetIncrement();  /* Read the increment */

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
      BSP_LCD_DrawBitmap(RX((&bat)->c[0 + 2 * new_sel]), RY((&bat)->c[1 + 2 * new_sel]), \
                         (&bat)->d[1 + 2 * new_sel]);

      /* Clear previous selection */
      BSP_LCD_DrawBitmap(RX((&bat)->c[0 + 2 * prev_sel]), RY((&bat)->c[1 + 2 * prev_sel]), \
                         (&bat)->d[0 + 2 * prev_sel]);

      if (new_sel == 1) {
        /* Since ONOF tile is black when new_sel == 1, we update the string to black
        else the string should be updated as blue */
        strONOF(BAT_STR_X, BAT_STR_Y, 2 + (&bat)->v);
      } else {
        strONOF(BAT_STR_X, BAT_STR_Y, (&bat)->v);
      } /* End of: if (new_sel == 1) */

    } /* End of: if ((prev_sel + increment) == 0 ||  (prev_sel + increment) == 1) */

    if (Enc_getClick() != click_init) { /* If the initial click has changed */
      switch (new_sel)
      {
      case 1: /* ONOF is selected */
        (&bat)->v = !((&bat)->v); /* !0 is always 1 and also !(any non zero value) is 0 */
        click_init = !click_init; /* Update the initial click value */
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
  BSP_LCD_DrawBitmap(RX((&slp)->c[0]),   RY((&slp)->c[1]), (&slp)->d[0]);           /* slp_set -> new_sel = 0 */
  BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 2]), RY((&slp)->c[1 + 2]), (&slp)->d[1 + 2]); /* slp_slp -> new_sel = 1 */
  BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 4]), RY((&slp)->c[1 + 4]), (&slp)->d[0 + 4]); /* slp_onof -> new_sel = 2 */
  BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]), (&mid)->d[0]); /* Draw middle button */

  strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v[0]); /* Write the ONOF string depending on the stored value */
  strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 0); /* Write the SET string on blue background */

  uint8_t click_init, exit_flag = 0, prev_sel = 1;
  uint8_t new_sel = 1; /* This value needs to be initialized as 1 since if there is no increment, we are effectively
  changing previous selection to 0 if new selection = 0.  */

  int8_t increment = 0;

  click_init = Enc_getClick();  /* Read the initial click value */

  while (!exit_flag) { /* !0 is always 1 and also !(any non zero value) is 0 */

    increment = Enc_GetIncrement();  /* Read the increment. If no new increment then it will return zero. */

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

      /* Clear previous selection. This needs to be implemented first before writing the new selection to avoid
      unnecessary overlap */
      BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 2 * prev_sel]), RY((&slp)->c[1 + 2 * prev_sel]), \
                         (&slp)->d[0 + 2 * prev_sel]);

      /* Write new selection */
      BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 2 * new_sel]), RY((&slp)->c[1 + 2 * new_sel]), \
                         (&slp)->d[1 + 2 * new_sel]);

      /* -------- Check ONOF String -------- */

      if (new_sel == 2) {
        /* Since ONOF tile is black when new_sel == 2, we update the string to black
        else the string should be updated as blue */

        strONOF(SLP_STR_X, SLP_STR_Y, 2 + (&slp)->v[0]);

      } else {

        strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v[0]);

      } /* -------- End of: Check ONOF String -------- */


      /* -------- Check SET String -------- */

      if (new_sel == 0) {
        /* Since SET tile is black when new_sel == 0, we update the string to black
        else the string should be updated as blue */

        strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 1);

      } else {

        strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 0);

      } /* -------- End of: Check SET String -------- */

    } /* End of: if ((prev_sel + increment) == 0 ||  (prev_sel + increment) == 1) */

    if (Enc_getClick() != click_init) {

      switch (new_sel)
      {
      case 2: /* ONOF is selected */
        (&slp)->v[0] = !((&slp)->v[0]);
        click_init = !click_init; /* Update the initial click value */
        strONOF(SLP_STR_X, SLP_STR_Y, 2 + (&slp)->v[0]);
        break;

      case 1: /* Sleep_Sleep is selected */
        exit_flag = !exit_flag;
        break;

      case 0: /* SET is selected */
        linearMinutes();

        /* We changed the value of Enc_getClick() inside the linearMinutes function. Therefore, after exiting
        linearMinutes, Enc_getClick() == click_init. Hence, there we don't update click_init anymore. */

        LCDCore_ClearScreen();  /* Clear the screen */

        /* Load all the previous menu back */
        BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 4]), RY((&slp)->c[1 + 4]), (&slp)->d[0 + 4]); /* slp_onof -> new_sel = 2 */
        BSP_LCD_DrawBitmap(RX((&slp)->c[0 + 2]), RY((&slp)->c[1 + 2]), (&slp)->d[0 + 2]); /* slp_slp -> new_sel = 1 */
        BSP_LCD_DrawBitmap(RX((&slp)->c[0]),   RY((&slp)->c[1]), (&slp)->d[1]);           /* slp_set -> new_sel = 0 */
        BSP_LCD_DrawBitmap(RX((&mid)->c[0]),   RY((&mid)->c[1]), (&mid)->d[0]); /* Draw middle button */

        strONOF(SLP_STR_X, SLP_STR_Y, (&slp)->v[0]); /* Write the ONOF string depending on the stored value */
        strSET(SLP_SET_STR_X, SLP_SET_STR_Y, 1); /* Write the SET string on blue background */
        break;
      } /* End of: switch-case */

    } /* End of: if (click != 0) */

    prev_sel = new_sel; /* Update the previous selection */
    Delay(200); /* Necessary delay */

  } /* End of: while(!exit_flag) */

} /* End of: LCDCore_NavigateSlp(void) */


void LCDCore_NavigateAlrm(void)
{

  /* -------- Write Alarm Defaults -------- */
  LCDCore_ClearScreen();
  BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]), (&alrm)->d[1]); /*alarm_alarm -> new_sel = 0 */
  /*alarm_alarm_box1 -> new_sel = 1 */
  /*alarm_alarm_ellipse -> new_sel = 2- 8 */
  /*alarm_alarm_box2 -> new_sel = 9 */

  drawRect((&alrm)->c[2], (&alrm)->c[3], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCDCORE_GREY); /* Draw the rectangles */
  drawRect((&alrm)->c[18], (&alrm)->c[19], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCDCORE_GREY);

  char* str_days[] = {"S", "M" , "T", "W", "T", "F", "S"};
  char str_num[3]; /* Need to add 1 more character for the null char */

  for (uint8_t i = 0; i < 7; i++) { /* Draw the ellipses */

    if ((&alrm)->v[1 + i] == 0) { /* Check if the Mon-Fri is not set */

      drawDaysEllipse((&alrm)->c[4 + 2 * i], (&alrm)->c[5 + 2 * i], ALRM_ELLIPSE_DIM_X, ALRM_ELLIPSE_DIM_Y, LCDCORE_GREY);
      BSP_LCD_SetBackColor(LCDCORE_GREY);
      BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    } else {

      drawDaysEllipse((&alrm)->c[4 + 2 * i], (&alrm)->c[5 + 2 * i], ALRM_ELLIPSE_DIM_X, ALRM_ELLIPSE_DIM_Y, LCDCORE_RED);
      BSP_LCD_SetBackColor(LCDCORE_RED);
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    }

    BSP_LCD_SetFont(&FontCalibri15);
    BSP_LCD_DisplayStringAt(RX((&alrm)->c[4 + 2 * i] + ALRM_ELLIPSE_STR_CORRECTION_X), \
                            RY((&alrm)->c[5 + 2 * i] - ALRM_ELLIPSE_STR_CORRECTION_Y), str_days[i], LEFT_MODE);

  } /* End of: for (uint8_t i = 0; i < 7; i++) */

  drawRect((&alrm)->c[2], (&alrm)->c[3], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCDCORE_GREY);  /* Color the rectangle black */
  drawRect((&alrm)->c[18], (&alrm)->c[19], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCDCORE_GREY);

  BSP_LCD_SetFont(&FontCalibri23);
  BSP_LCD_SetBackColor(LCDCORE_GREY);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  for (uint8_t i = 1; i <= 9; i += 8 ) {

    snprintf(str_num, 3, "%d", (&alrm)->v[i - 1]);

    if ((&alrm)->v[i - 1] < 10) {

      BSP_LCD_DisplayChar(RX((&alrm)->c[2 * i]), RY((&alrm)->c[2 * i + 1] + ALRM_BOX_STR_CORRECTION_Y), 48);
      BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * i] + FontCalibri23_WIDTH), RY((&alrm)->c[2 * i + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                              (uint8_t *)&str_num, LEFT_MODE);

    } else {

      BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * i]), RY((&alrm)->c[2 * i + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                              (uint8_t *)&str_num, LEFT_MODE);

    } /* End of: if ((&alrm)->v[i - 1] < 10) { */
  } /* End of: for ( i = 1; i <= 9; i += 8 ) { */
  /* -------- End of: Write Alarm Defaults -------- */

  uint8_t click_init, new_sel = 0, exit_flag = 0, prev_sel = 0;
  int8_t increment = 0;

  click_init = Enc_getClick();  /* Read the initial click value */

  while (!exit_flag) { /* !0 is always 1 and also !(any non zero value) is 0 */

    increment = Enc_GetIncrement();  /* Read the increment. If no new increment then it will return zero. */

    if (ABS_VAL(increment) >= MIN_INCREMENT)
    {

      if (increment <= -MIN_INCREMENT) { /* If the increment is too negative make it negative 1. */
        increment = -1;
      } else if (increment >= MIN_INCREMENT) {
        increment = 1; /* If the increment is too positive make it positive 1. */
      }

      /* Check for overflow. This line of code make it circular from 0-9. */
      if (prev_sel == 0 && increment == -1) {
        new_sel = 9;
      } else if (prev_sel == 9 && increment == 1) {
        new_sel = 0;
      } else {
        new_sel = prev_sel + increment;
      }

      /* -------- Update new selection -------- */
      if (new_sel == 1 || new_sel == 9)
      {

        drawRect((&alrm)->c[2 * new_sel], (&alrm)->c[2 * new_sel + 1], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCD_COLOR_BLACK);

        snprintf(str_num, 3, "%d", (&alrm)->v[new_sel - 1]);

        BSP_LCD_SetFont(&FontCalibri23);
        BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
        BSP_LCD_SetTextColor(LCDCORE_BLUE);

        if ((&alrm)->v[new_sel - 1] < 10) {

          BSP_LCD_DisplayChar(RX((&alrm)->c[2 * new_sel]), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), 48);
          BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * new_sel] + FontCalibri23_WIDTH), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                  (uint8_t *)&str_num, LEFT_MODE);

        } else {

          BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * new_sel]), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                  (uint8_t *)&str_num, LEFT_MODE);

        }

      } else if (new_sel >= 2 && new_sel <= 8) {

        drawDaysEllipse((&alrm)->c[2 * new_sel], (&alrm)->c[2 * new_sel + 1], ALRM_ELLIPSE_DIM_X, ALRM_ELLIPSE_DIM_Y, LCD_COLOR_BLACK);
        BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
        BSP_LCD_SetTextColor(LCDCORE_BLUE);
        BSP_LCD_SetFont(&FontCalibri15);
        BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * new_sel] + ALRM_ELLIPSE_STR_CORRECTION_X), \
                                RY((&alrm)->c[2 * new_sel + 1] - ALRM_ELLIPSE_STR_CORRECTION_Y), str_days[new_sel - 2], LEFT_MODE);
      } else if (new_sel == 0) {

        BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]), (&alrm)->d[1]);

      }
      /* -------- End of: Update new selection -------- */

      /* -------- Clear previous selection -------- */
      if (prev_sel == 1 || prev_sel == 9)
      {

        drawRect((&alrm)->c[2 * prev_sel], (&alrm)->c[2 * prev_sel + 1], ALRM_BOX_DIM_X, ALRM_BOX_DIM_Y, LCDCORE_GREY);

        snprintf(str_num, 3, "%d", (&alrm)->v[prev_sel - 1]);

        BSP_LCD_SetFont(&FontCalibri23);
        BSP_LCD_SetBackColor(LCDCORE_GREY);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

        if ((&alrm)->v[prev_sel - 1] < 10) {

          BSP_LCD_DisplayChar(RX((&alrm)->c[2 * prev_sel]), RY((&alrm)->c[2 * prev_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), 48);
          BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * prev_sel] + FontCalibri23_WIDTH), RY((&alrm)->c[2 * prev_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                  (uint8_t *)&str_num, LEFT_MODE);

        } else {

          BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * prev_sel]), RY((&alrm)->c[2 * prev_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                  (uint8_t *)&str_num, LEFT_MODE);

        }

      } else if (prev_sel >= 2 && prev_sel <= 8) {

        if ((&alrm)->v[prev_sel - 1] == 1) {

          drawDaysEllipse((&alrm)->c[2 * prev_sel], (&alrm)->c[2 * prev_sel + 1], ALRM_ELLIPSE_DIM_X, ALRM_ELLIPSE_DIM_Y, LCDCORE_RED);
          BSP_LCD_SetBackColor(LCDCORE_RED);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

        } else {

          drawDaysEllipse((&alrm)->c[2 * prev_sel], (&alrm)->c[2 * prev_sel + 1], ALRM_ELLIPSE_DIM_X, ALRM_ELLIPSE_DIM_Y, LCDCORE_GREY);
          BSP_LCD_SetBackColor(LCDCORE_GREY);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          BSP_LCD_SetFont(&FontCalibri15);
        }

        BSP_LCD_SetFont(&FontCalibri15);
        BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * prev_sel] + ALRM_ELLIPSE_STR_CORRECTION_X), \
                                RY((&alrm)->c[2 * prev_sel + 1] - ALRM_ELLIPSE_STR_CORRECTION_Y), str_days[prev_sel - 2], LEFT_MODE);
      } else if (prev_sel == 0) {

        BSP_LCD_DrawBitmap(RX((&alrm)->c[0]), RY((&alrm)->c[1]), (&alrm)->d[0]);

      }
      /* -------- End of: Clear previous selection -------- */

    } /* End of: if (ABS_VAL(increment) >= MIN_INCREMENT) */

    if (Enc_getClick() != click_init) {

      if (new_sel >= 2 && new_sel <= 8) {

        (&alrm)->v[new_sel - 1] = !((&alrm)->v[new_sel - 1]);
        click_init = !click_init; /* Update the initial click value */

      } else if (new_sel == 0) {

        exit_flag = !exit_flag; /* ALARM_ALARM is selected */

      } else if (new_sel == 1 || new_sel == 9) {

        /* Copy the  while (!exit_flag) implementation but need to use new set of variables */
        uint8_t click_init_box, click_box;
        uint8_t exit_flag_box = 0;
        uint8_t val_max; /* If new_sel == 1, then val_max is 24 (23 hours) else if new_sel ==9, then val_max
        should be from 0-59 (59 minutes). Therefore, the range is from 00:00 to 23:59. */
        uint8_t val = (&alrm)->v[new_sel - 1];
        char val_num[3];

        if (new_sel == 1) {
          val_max = 23;
        } else if (new_sel == 9 ) {
          val_max = 59;
        } /* End of : if (new_sel == 1) */

        click_init_box = Enc_getClick();  /* Read the initial click value */

        while (!exit_flag_box) {
          increment = Enc_GetIncrement();

          if ((increment > 0) && (val < val_max)) {
            /* Check if val is not the maximum value yet. Don't increment
            if it is already the max value */
            ++val;
          } else if ((increment < 0) && (val > 0)) {
            /* Check if the val is zero. Don't decrement if the val == 0
            to avoid overflow */
            --val;
          }

          snprintf(val_num, 3, "%d", val);

          BSP_LCD_SetFont(&FontCalibri23);
          BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
          BSP_LCD_SetTextColor(LCDCORE_BLUE);

          if (val < 10) {

            BSP_LCD_DisplayChar(RX((&alrm)->c[2 * new_sel]), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), 48);
            BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * new_sel] + FontCalibri23_WIDTH), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                    (uint8_t *)&val_num, LEFT_MODE);

          } else {

            BSP_LCD_DisplayStringAt(RX((&alrm)->c[2 * new_sel]), RY((&alrm)->c[2 * new_sel + 1] + ALRM_BOX_STR_CORRECTION_Y), \
                                    (uint8_t *)&val_num, LEFT_MODE);

          }

          if (Enc_getClick() != click_init_box) {

            (&alrm)->v[new_sel - 1] = val; /* Store the hours values in the Menu alrm struct */
            exit_flag_box = !exit_flag_box;
            Delay(200); /* Necessary delay */
          } /* End of: if (click != click_init) */

        } /* End of: while (!exit_flag_box)*/

        // click_init = click; /* Update the initial click value */

      } /* End of: else if (new_sel == 1 || new_sel == 9) */

    } /* End of: if (click != click_init) */

    prev_sel = new_sel; /* Update the previous selection */
    Delay(200); /* Necessary delay */

  } /* End of: while (!exit_flag) */

} /* LCDCore_NavigateAlrm */


void LCDCore_NavigateClndr(void)
{

  LCDCore_ClearScreen();

  char* str_days[] = {"S", "M" , "T", "W", "T", "F", "S"};
  char* str_months[] = {"JAN", "FEB" , "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  char str_num[3]; /* Need to add 1 more character for the null char */


  /* -------- Write Defaults -------- */
  for (uint8_t i = 0; i < 7; i++) { /* Draw the main calendar */
    for (uint8_t j = 0; j < 7; j++) {
      /* Draw the filled rectangles */
      drawRect(CLNDR_MAIN_X + i * CLNDR_MAIN_WIDTH, CLNDR_MAIN_Y + j * CLNDR_MAIN_HEIGHT, CLNDR_MAIN_WIDTH, CLNDR_MAIN_HEIGHT, LCDCORE_GREY);
    }
  }

  BSP_LCD_SetFont(&FontCalibri15);
  BSP_LCD_SetBackColor(LCDCORE_GREY);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  for (uint8_t i = 0; i < 7; i++) {
    for (uint8_t j = 0; j < 7; j++) {
      /* Draw the non-filled rectangles or the outline */
      drawRect_nonfill(CLNDR_MAIN_X + i * CLNDR_MAIN_WIDTH, CLNDR_MAIN_Y + j * CLNDR_MAIN_HEIGHT, CLNDR_MAIN_WIDTH, CLNDR_MAIN_HEIGHT, LCD_COLOR_BLACK);
    }
  } /* Draw the main calendar */

  for (uint8_t i = 0; i < 7; i++) { /* Write Sunday to Saturday */
    BSP_LCD_DisplayStringAt(RX(CLNDR_MAIN_X + i * CLNDR_MAIN_WIDTH + CLNDR_MAIN_X_CORRECTION), \
                            RY(CLNDR_MAIN_Y + CLNDR_MAIN_Y_CORRECTION), str_days[i], LEFT_MODE);
  }

  drawRect(CLNDR_MONTH_X, CLNDR_MONTH_Y, CLNDR_MONTH_WIDTH, CLNDR_MONTH_HEIGHT, LCDCORE_GREY);

  for (uint8_t i = 0; i < 2; i++) { /* Draw the year rectangles */

    drawRect(CLNDR_YEAR_X + (CLNDR_YEAR_WIDTH + CLNDR_YEAR_GAP)*i, \
             CLNDR_YEAR_Y, CLNDR_YEAR_WIDTH, CLNDR_YEAR_HEIGHT, LCDCORE_GREY);
  }


  BSP_LCD_SetBackColor(LCDCORE_GREY);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  BSP_LCD_SetFont(&FontCalibri27); /* Write the month from the stored values */
  /* The value of month should be:
  1- January
  2- February
  ...
  12- December */
  BSP_LCD_DisplayStringAt(RX(CLNDR_MONTH_X + CLNDR_MONTH_X_CORRECTION), \
                          RY(CLNDR_MONTH_Y + CLNDR_MONTH_Y_CORRECTION), str_months[(&clndr)->v[0] - 1], LEFT_MODE);

  BSP_LCD_SetFont(&FontCalibri23);

  for (uint8_t i = 0; i < 2; i++) { /* Write the year from the stored values */

    snprintf(str_num, 4, "%d", (&clndr)->v[i + 1]);
    /* The first two digits of the year is stored at (&clndr)-v[1] and the last two digits is stored at (&clndr)-v[2] */
    BSP_LCD_DisplayStringAt(RX( CLNDR_YEAR_X + (CLNDR_YEAR_WIDTH + CLNDR_YEAR_GAP)*i + CLNDR_YEAR_X_CORRECTION), \
                            RY(CLNDR_YEAR_Y + CLNDR_YEAR_Y_CORRECTION), (uint8_t *)&str_num, LEFT_MODE);
  }


  // generateCalendar((&clndr)->v[0], ((&clndr)->v[1]) * 100 + (&clndr)->v[2], \
  //                  first_wkday_month, month_days);  /* Generate the initial calendar based on the HAL_RTC */

  /* -------- End of: Write Defaults -------- */

  uint8_t first_wkday_month; 
  uint8_t month_days;

  generateCalendar((&clndr)->v[0], ((&clndr)->v[1]) * 100 + (&clndr)->v[2], \
                   &first_wkday_month, &month_days); /* Replace */

  uint8_t click_init, new_sel = 0, exit_flag = 0, prev_sel = 0;
  int8_t increment = 0;

  click_init = Enc_getClick();  /* Read the initial click value */

  while (!exit_flag) { /* !0 is always 1 and also !(any non zero value) is 0 */

    increment = Enc_GetIncrement();  /* Read the increment. If no new increment then it will return zero. */

    /* Check if the increment is in the right direction. If not, no changes shall be made. */
    if ( ((prev_sel == 0) && (increment >= MIN_INCREMENT)) || ((prev_sel == month_days + 2) && (increment <= -MIN_INCREMENT)) || \
   (prev_sel <= month_days + 2) || (prev_sel >= 0)  )
    {

      if (increment <= -MIN_INCREMENT) { /* If the increment is too negative make it negative 1. */
        increment = -1;
      } else if (increment >= MIN_INCREMENT) {
        increment = 1; /* If the increment is too positive make it positive 1. */
      }

      new_sel = prev_sel + increment;

      /* -------- Update new selection and Clear previous selection (Optimized code) -------- */
      uint8_t selection = new_sel;

      uint16_t font_color = LCDCORE_BLUE, font_bg = LCD_COLOR_BLACK, shape_color = LCD_COLOR_BLACK;

      for (uint8_t i = 0; i < 2; i++) {

        if (i == 1) {
          selection = prev_sel;
          font_color = LCD_COLOR_BLACK;
          font_bg = LCDCORE_GREY;
          shape_color = LCDCORE_GREY;
        }

        if (selection == 0) /* CLNDR_MONTH is selsected */
        {
          drawRect(CLNDR_MONTH_X, \
                   CLNDR_MONTH_Y, CLNDR_MONTH_WIDTH, CLNDR_MONTH_HEIGHT, shape_color);
          BSP_LCD_SetFont(&FontCalibri27);
          BSP_LCD_SetBackColor(font_bg);
          BSP_LCD_SetTextColor(font_color);

          BSP_LCD_DisplayStringAt(RX(CLNDR_MONTH_X + CLNDR_MONTH_X_CORRECTION), \
                                  RY(CLNDR_MONTH_Y + CLNDR_MONTH_Y_CORRECTION), str_months[(&clndr)->v[0] - 1], LEFT_MODE);
        } else if (selection == 1 || selection == 2) {

          drawRect(CLNDR_YEAR_X + (CLNDR_YEAR_WIDTH + CLNDR_YEAR_GAP) * (selection - 1), \
                   CLNDR_YEAR_Y, CLNDR_YEAR_WIDTH, CLNDR_YEAR_HEIGHT, shape_color);

          BSP_LCD_SetBackColor(font_bg);
          BSP_LCD_SetTextColor(font_color);

          snprintf(str_num, 4, "%d", (&clndr)->v[selection]);
          BSP_LCD_DisplayStringAt(RX( CLNDR_YEAR_X + (CLNDR_YEAR_WIDTH + CLNDR_YEAR_GAP) * (selection - 1) + CLNDR_YEAR_X_CORRECTION), \
                                  RY(CLNDR_YEAR_Y + CLNDR_YEAR_Y_CORRECTION), (uint8_t *)&str_num, LEFT_MODE);


        } else if ( selection <= month_days + 2 && selection >= 3) { /* If new selection is between 3 or month_days + 2. */
          /* For example, for a 31 day month, the count is:
          0 - CALENDAR_MONTH
          1 - CALENDAR_YEAR1 (First 2 digits of the year)
          2 - CALENDAR_YEAR2 (Last 2 digits of the year)
          3 - First day of the calendar
          ...
          33 - Last day of the calendar. This is calculated by  month_days + 2.
          34 - Return */

          drawRect(CLNDR_MAIN_X + ((first_wkday_month + selection - 3) % 7) * CLNDR_MAIN_WIDTH, \
                   CLNDR_MAIN_Y + ((first_wkday_month + selection - 3) / 7 + 1) * CLNDR_MAIN_HEIGHT,  \
                   CLNDR_MAIN_HEIGHT, shape_color);

          uint8_t str_correction;

          if ((selection - 2) < 10) {
            str_correction = CLNDR_MAIN_X_CORRECTION;
          } else {
            str_correction = CLNDR_MAIN_X_CORRECTION_2DIG;
          }

          BSP_LCD_SetBackColor(font_bg);
          BSP_LCD_SetTextColor(font_color);
          snprintf(str_num, 3, "%d", selection - 2);
          BSP_LCD_SetFont(&FontCalibri15);
          BSP_LCD_DisplayStringAt(RX(CLNDR_MAIN_X +  ((first_wkday_month + selection - 3) % 7) * CLNDR_MAIN_WIDTH + str_correction), \
                                  RY(CLNDR_MAIN_Y + CLNDR_MAIN_Y_CORRECTION - ((first_wkday_month + selection - 3) / 7 + 1) * CLNDR_MAIN_HEIGHT), \
                                  (uint8_t *)&str_num, LEFT_MODE);

        } else if (selection ==  month_days + 3) {

          drawRect(CLNDR_MAIN_X + 6 * CLNDR_MAIN_WIDTH, CLNDR_MAIN_Y + 6 * CLNDR_MAIN_HEIGHT, CLNDR_MAIN_HEIGHT, shape_color);

          BSP_LCD_SetBackColor(font_bg);
          BSP_LCD_SetTextColor(font_color);
          BSP_LCD_SetFont(&FontCalibri15);
          BSP_LCD_DisplayChar(RX(CLNDR_MAIN_X +  6 * CLNDR_MAIN_WIDTH + CLNDR_MAIN_X_CORRECTION), \
                              RY(CLNDR_MAIN_Y + CLNDR_MAIN_Y_CORRECTION - 6 * CLNDR_MAIN_HEIGHT), 82); /* The ASCII code for capital R is 82 */

        }
        /* -------- End of: Update new selection and Clear previous selection (Optimized code) -------- */

      } /* End of: for (uint8_t i; i < 2; i++) */

    } /* End of: if ((prev_sel == 0) && increment >= MIN_INCREMENT || (prev_sel == 1) && increment <= -MIN_INCREMENT) */
    prev_sel = new_sel;
  } /* End of: while (!exit_flag) */

} /* LCDCore_NavigateClndr(void) */


/*-------------------------------String functions-------------------------------*/

void strONOF(uint8_t x_pos, uint8_t y_pos, uint8_t val)
{
  char str_on[] = "ON";
  char str_of[] = "OF";

  if (val > 1) {
    val = val - 2;

    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCDCORE_BLUE);
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

    BSP_LCD_SetBackColor(LCDCORE_BLUE);
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
    BSP_LCD_SetTextColor(LCDCORE_BLUE);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  } /* End of:  if (val == 1) */
  else if (val == 0) {

    BSP_LCD_SetBackColor(LCDCORE_BLUE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  } /* End of:  else if (val == 0) */

  BSP_LCD_DisplayStringAt(RX(x_pos), RY(y_pos), (uint8_t *)&str_set, LEFT_MODE);

} /* End of: strSET(uint8_t x_pos, uint8_t y_pos, uint8_t val)  */

/*-------------------------------End of String functions-------------------------------*/


/*-------------------------------Shape functions-------------------------------*/

void drawDaysEllipse(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t color)
{

  BSP_LCD_SetTextColor(color);
  /* This is the correct implementation of the BSP_LCD library for making shapes such
  that the coordinates we input is still consistent with the bitmap images, that is, bottom left.  */
  BSP_LCD_FillEllipse(WIDTH - (x_pos + width), HEIGHT - y_pos, width, height );

}

void drawRect(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t color)
{

  BSP_LCD_SetTextColor(color);
  BSP_LCD_FillRect(WIDTH - (x_pos + width), HEIGHT - y_pos, width, height );

}


void drawRect_nonfill(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t color)
{

  BSP_LCD_SetTextColor(color);
  BSP_LCD_DrawRect(WIDTH - (x_pos + width), HEIGHT - y_pos, width, height );

}


/*-------------------------------End of Shape functions-------------------------------*/

void linearMinutes(void)
{
  LCDCore_ClearScreen();
  char str_minutes[] = "MINUTES: ";
  char str_num[4]; /* 3 digits + 1 null char */

  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&FontPerpetuaTitlingMT17);

  BSP_LCD_DisplayStringAt(RX(90), RY(90), (uint8_t *)&str_minutes, LEFT_MODE);

  BSP_LCD_SetTextColor(LCDCORE_BLUE);
  for (uint8_t i = 0; i < (&slp)->v[1]; i++) {
    BSP_LCD_FillCircle(RX(MINUTESMAX - i), RY(150), 9); /* forward 260, 259, 258, 257, ... 60 */
  }                                                     /* reverse 60, 61, 62, 63, ... 260 */

  uint8_t click_init, exit_flag = 0;
  int8_t increment = 0;
  uint16_t circle_init = MINUTESMAX - (&slp)->v[1]; /* From the default initialization, 260-100  */
  /* This variables needs to be 16-bit since the maximum value is 260 to prevent overflow. */

  click_init = Enc_getClick();

  while (!exit_flag) {

    increment = Enc_GetIncrement();

    if ((circle_init >= MINUTESMIN) && (circle_init <= MINUTESMAX)) {
      /* Check if the values are still 1 step away from the MIN and MAX values  */

      if ((increment > 0) && (circle_init != MINUTESMAX)) {  /* reverse 160, 161, 162,... 260 */
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillCircle(RX(circle_init++), RY(150), 9);
        BSP_LCD_SetTextColor(LCDCORE_BLUE);
        BSP_LCD_FillCircle(RX(circle_init), RY(150), 9);

      } else if ((increment < 0) && (circle_init != MINUTESMIN)) { /* forward */
        BSP_LCD_SetTextColor(LCDCORE_BLUE);
        BSP_LCD_FillCircle(RX(circle_init--), RY(150), 9);
        BSP_LCD_FillCircle(RX(circle_init), RY(150), 9);
      }

      if (circle_init == MINUTESMAX) { /* Check if it reached zero minute mark, then remove the remaining blue circles */
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_FillCircle(RX(circle_init), RY(150), 9);
      }

      uint16_t minutes = MINUTESMAX - circle_init;
      snprintf(str_num, 4, "%d", minutes);

      if (minutes < 10) {

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130 + 2 * FontPerpetuaTitlingMT17_WIDTH), RY(120), (uint8_t *)&str_num, LEFT_MODE);
        drawRect(130, 120, 2 * FontPerpetuaTitlingMT17_WIDTH, FontPerpetuaTitlingMT17_WIDTH, LCD_COLOR_WHITE);

      } else if (minutes < 100) {

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130 + 1 * FontPerpetuaTitlingMT17_WIDTH), RY(120), (uint8_t *)&str_num, LEFT_MODE);
        drawRect(130, 120, 1 * FontPerpetuaTitlingMT17_WIDTH, FontPerpetuaTitlingMT17_WIDTH, LCD_COLOR_WHITE);

      } else {

        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(RX(130), RY(120), (uint8_t *)&str_num, LEFT_MODE);
      }

    } /* End of: if ((circle_init >= MINUTESMIN) && (circle_init <= MINUTESMAX)) */

    if (Enc_getClick() != click_init) {
      uint16_t minutes = MINUTESMAX - circle_init;
      (&slp)->v[1] = (uint8_t)minutes; /* Store the minutes values in teh Meu slp struct */
      exit_flag = !exit_flag;

    } /* End of: if (click != click_init) */

  } /* End of:   while(!exit_flag) */

} /* End of: linearMinutes(void) */


void generateCalendar(uint8_t month, uint16_t year, uint8_t* wkday_ptr,  uint8_t* month_days_ptr) {

  uint8_t month_days[] = {31 , 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  char* str_months[] = {"JAN", "FEB" , "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  char str_num[3]; /* Need to add 1 more character for the null char */

  /* Get the first weekday of the year */
  uint32_t first_wkday_year, first_wkday_month;

  first_wkday_year = (((year - 1) * 365) + ((year - 1) / 4) - ((year - 1) / 100) + ((year) / 400) + 1) % 7;

  /* Check if leap year */
  if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
    month_days[1] = 29; /* Set February to 29 days */
  }

  /* Get the first weekday of the month */
  first_wkday_month = first_wkday_year;
  for (uint8_t i = 0; i < (month - 1); i++) {
    first_wkday_month = first_wkday_month + month_days[i];
  }
  first_wkday_month = first_wkday_month % 7;

  BSP_LCD_SetFont(&FontCalibri15);
  BSP_LCD_SetBackColor(LCDCORE_GREY);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  uint8_t days_in_a_week = (uint8_t)first_wkday_month;
  uint8_t i = 0, j = 1, str_correction;

  for (uint8_t day = 0; day < month_days[month - 1]; day++) {

    snprintf(str_num, 3, "%d", day + 1);

    if (day + 1 < 10) {
      str_correction = CLNDR_MAIN_X_CORRECTION;
    } else {
      str_correction = CLNDR_MAIN_X_CORRECTION_2DIG;
    }

    if (j == 1) {
      BSP_LCD_DisplayStringAt(RX(CLNDR_MAIN_X + (first_wkday_month + i)*CLNDR_MAIN_WIDTH + str_correction), \
                              RY(CLNDR_MAIN_Y + CLNDR_MAIN_Y_CORRECTION - CLNDR_MAIN_HEIGHT), (uint8_t *)&str_num, LEFT_MODE);

    } else {
      BSP_LCD_DisplayStringAt(RX(CLNDR_MAIN_X + i * CLNDR_MAIN_WIDTH + str_correction), \
                              RY(CLNDR_MAIN_Y + CLNDR_MAIN_Y_CORRECTION - j * CLNDR_MAIN_HEIGHT), (uint8_t *)&str_num, LEFT_MODE);
    }
    i++;
    if ((++days_in_a_week) % 7 == 0) {
      j++;
      i = 0;
    } /* End of: if (++days_in_a_week == 7) { */

  } /* End of: for (uint8_t day = 0; day < month_days[month - 1]; day++) */

  for (uint8_t i = 0; i < 7; i++) {
    for (uint8_t j = 0; j < 7; j++) {
      drawRect_nonfill(CLNDR_MAIN_X + i * CLNDR_MAIN_WIDTH, CLNDR_MAIN_Y + j * CLNDR_MAIN_HEIGHT, CLNDR_MAIN_WIDTH, CLNDR_MAIN_HEIGHT, LCD_COLOR_BLACK);
    }
  }

  /* Update the pointer arguments */
  *wkday_ptr = (uint8_t)first_wkday_month;
  *month_days_ptr = (uint8_t)month_days[month - 1];

}





