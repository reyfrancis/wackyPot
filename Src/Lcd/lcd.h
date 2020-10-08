/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
   
#define  LCD_DRVTYPE_V1_1

/* A 16bites �rt�keket (szinkodok, bitmapok) forditott sorrendben legyenek-e t�rolva
 * - 0: ne legyen forditva t�rolva
 * - 1: forditva legyen t�rolva
 *   a forditott t�rol�st kiz�r�lag FSMC 8bit DMA miatt lehet �rdemes bekapcsolni, ugyanis amig a kijelz� a 16bites �rt�ket
 *   fels� b�jt - also b�jt sorrendben v�rja, az FSMC 8bit DMA csak also b�jt-fels� b�jt sorrendben tudja k�ldeni
 */
#define  LCD_REVERSE16   0

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup LCD
  * @{
  */
 
/** @defgroup LCD_Exported_Types
  * @{
  */

/** @defgroup LCD_Driver_structure  LCD Driver structure
  * @{
  */
typedef struct
{
  void     (*Init)(void);
  uint16_t (*ReadID)(void);
  void     (*DisplayOn)(void);
  void     (*DisplayOff)(void);
  void     (*SetCursor)(uint16_t, uint16_t);
  void     (*WritePixel)(uint16_t, uint16_t, uint16_t);
  uint16_t (*ReadPixel)(uint16_t, uint16_t);
  
   /* Optimized operation */
  void     (*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  
  uint16_t (*GetLcdPixelWidth)(void);
  uint16_t (*GetLcdPixelHeight)(void);
  void     (*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
  void     (*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
  #ifdef   LCD_DRVTYPE_V1_1
  void     (*FillRect)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*ReadRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
  #endif
}LCD_DrvTypeDef;    
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
