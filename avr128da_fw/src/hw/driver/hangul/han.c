/*
 * han.c
 *
 *  Created on: 2020. 12. 27.
 *      Author: baram
 */



#include "han.h"
#include "font_eng.h"
#include "font_han.h"


void hanEngFontLoad(char *HanCode, han_font_t *FontPtr);
void hanUniFontLoad(char *HanCode, han_font_t *FontPtr);





uint16_t hanFontLoad(char *HanCode, han_font_t *FontPtr )
{
  // 버퍼 초기화
  memset(FontPtr->FontBuffer, 0x00, 32);


  FontPtr->Code_Type = PHAN_NULL_CODE;
  // 한글코드인지 감별
  //
  if( !HanCode[0] || HanCode[0] == 0x0A )   // 문자열 마지막
  {
    FontPtr->Code_Type = PHAN_END_CODE;
    FontPtr->Size_Char = 1;
    return PHAN_END_CODE;
  }
  else if( HanCode[0] & 0x80 )              // 한글 코드인경우
  {
    uint32_t utf8_code = 0;
    uint8_t han_buf[3];

    han_buf[0] = HanCode[0];
    han_buf[1] = HanCode[1];
    han_buf[2] = HanCode[2];


    utf8_code = ((uint32_t)han_buf[0]<<16) | ((uint32_t)han_buf[1]<<8) | ((uint32_t)han_buf[2]<<0);

    if (utf8_code >= 0xEAB080 && utf8_code <= 0xED9FB0)
    {
      FontPtr->Code_Type = PHAN_HANGUL_CODE;
      FontPtr->Size_Char = 3;
      hanUniFontLoad(HanCode, FontPtr);
    }
    else
    {
      FontPtr->Code_Type = PHAN_HANGUL_CODE;
      FontPtr->Size_Char = 2;      
    }
    return PHAN_HANGUL_CODE;
  }
  else                                      // 영문 코드
  {
    FontPtr->Code_Type = PHAN_ENG_CODE;
    FontPtr->Size_Char = 1;
    hanEngFontLoad(HanCode, FontPtr);
    return PHAN_ENG_CODE;
  }


  return FontPtr->Code_Type;

}

void hanUniFontLoad(char *HanCode, han_font_t *FontPtr)
{
  uint16_t i;
  uint16_t utf16;

  //static declaration 은 속도를 높이기 위한것임.
  static uint16_t uChosung, uJoongsung, uJongsung, uChoType, uJooType,uJonType;


  utf16 = (uint16_t)(HanCode[0] & 0x0f) << 12 | (uint16_t)(HanCode[1] & 0x3f) << 6 | (uint16_t)(HanCode[2] & 0x3f);


  //seperate phoneme code
  utf16 -= 0xac00;
  uJongsung  = utf16 % 28;
  utf16 /= 28;
  uJoongsung = utf16 % 21;
  uChosung   = utf16 / 21;


  //make font index
  uChosung   = pgm_read_byte_far(&UniChoIdxTbl[uChosung]);    //Chosung index
  uJoongsung = pgm_read_byte_far(&UniJooIdxTbl[uJoongsung]);  //Joongsung index
  uJongsung  = pgm_read_byte_far(&UniJonIdxTbl[uJongsung]);   //Jongsung index


  //decide a character type (몇번째 벌을 사용할지 결정)
  uChoType = uJongsung ? pgm_read_byte_far(&ChoTypeCaseJongYes[uJoongsung]):pgm_read_byte_far(&ChoTypeCaseJongNo[uJoongsung]);
    //'ㄱ'(1) 이나 'ㅋ'(16) 인경우는
  uJooType = ((uChosung == 0 || uChosung == 1 ||uChosung == 16 ) ? 0: 1) + (uJongsung ? 2: 0);
  uJonType = pgm_read_byte_far(&JonType[uJoongsung]);

  for(i = 0; i<32; i++)
  {
    FontPtr->FontBuffer[i]  = (uint8_t)pgm_read_byte_far(&K_font[uChoType*20+uChosung][i]);
    FontPtr->FontBuffer[i] |= (uint8_t)pgm_read_byte_far(&K_font[160 + uJooType*22+uJoongsung][i]);
  }

  //combine Jongsung
  if(uJongsung)
  {
    for(i = 0; i < 32; i++)   FontPtr->FontBuffer[i] |= pgm_read_byte_far(&K_font[248 + uJonType*28+uJongsung][i]);
  }
}

void hanEngFontLoad(char *HanCode, han_font_t *FontPtr)
{
  uint16_t i;
  uint8_t EngCode;

  EngCode = (uint8_t)*HanCode;
  EngCode -= 0x20;  // FONT는 스페이스 부터 시작한다.

  for (i = 0 ; i < 16 ; i++)
  {
    FontPtr->FontBuffer[i] = pgm_read_byte_far(&wEngFon[EngCode][i]);     
  }
}


