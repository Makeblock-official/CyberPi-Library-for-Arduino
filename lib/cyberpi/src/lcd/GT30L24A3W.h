
#ifndef _GT30L24A3W_H_
#define _GT30L24A3W_H_


typedef enum
{
  FONT_SIZE_5_7 = 1,
  FONT_SIZE_7_8,
  FONT_SIZE_6_12,
  FONT_SIZE_12_12,
  FONT_SIZE_8_16,
  FONT_SIZE_12_24_A,
  FONT_SIZE_12_24_P,
  FONT_SIZE_16_32,
  FONT_SIZE_16_16,
  FONT_SIZE_24_24,
  FONT_SIZE_32_32,
}GT3024_FONT_SIZE;

#define ASCII_5X7              1
#define ASCII_7X8              2
#define ASCII_6X12             3
#define ASCII_12_A 	           4
#define ASCII_8X16             5
#define ASCII_12X24_A          6
#define ASCII_12X24_P          7
#define ASCII_16X32            8
#define ASCII_16_A             9
#define ASCII_24_B            10
#define ASCII_32_B            11

//code1 page ���� 
#define CP_437            0
#define CP_737            1
#define CP_775            2
#define CP_850            3
#define CP_852            4
#define CP_855            5
#define CP_857            6
#define CP_858            7
#define CP_860            8
#define CP_862            9
#define CP_863            10
#define CP_864            11
#define CP_865            12
#define CP_866            13
#define CP_1251           14
#define CP_1252           15
#define CP_1253           16
#define CP_1254           17
#define CP_1255           18
#define CP_1256           19
#define CP_1257           20
#define CP_928            21
#define CP_HEBREW         22
#define CP_KATAKANA       23

#define      USA           0x2B825EUL
#define 	 FRANCE        0x2B8276UL
#define 	 GERMANY       0x2B828EUL
#define 	 UK            0x2B82A6UL
#define 	 DENMARK       0x2B82BEUL
#define 	 SWEDEN        0x2B82D6UL
#define 	 LTALY         0x2B82EEUL
#define 	 SPAIN         0x2B8306UL
#define 	 JAPAN         0x2B831EUL
#define 	 NORWAY        0x2B8336UL
#define 	 DENMARK_II    0x2B834EUL 

extern unsigned char r_dat_bat(unsigned long  ReadAddr,unsigned int  NumByteToRead,unsigned char* pBuffer);

unsigned char  ASCII_GetData(unsigned char asc,unsigned long ascii_kind,unsigned char *DZ_Data);
void gt_12_GetData (unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
void  gt_16_GetData (unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
unsigned long GBK_24_GetData (unsigned char c1, unsigned char c2,unsigned char *DZ_Data) ;
unsigned long Shift_Jis_8X16_GetData(unsigned int code,unsigned char *DZ_Data);
unsigned long Unicode_Shift_Jis_GetData(unsigned int code,unsigned char *DZ_Data);
unsigned long JIS0208_16X16_GetData(unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
unsigned long JIS0208_24X24_GetData(unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
unsigned long KSC5601_F_16_GetData(unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
unsigned long KSC5601_F_24_GetData(unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
unsigned long LATIN_8X16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long GREECE_8X16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long CYRILLIC_8X16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long HEBREW_8X16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long THAILAND_8X16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long LATIN_16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long GREECE_16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long CYRILLIC_16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long ALB_16_GetData(unsigned int unicode_alb,unsigned char *DZ_Data);
unsigned long Indic_16_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long LATIN_12X24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long GREECE_12X24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long CYRILLIC_12X24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long HEBREW_12X24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long THAILAND_16X24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long ALB_24_GetData(unsigned int UNIcode_alb,unsigned char *DZ_Data);
unsigned long Khmer_24_GetData(unsigned int Fontcode,unsigned char *DZ_Data);
unsigned long ICCS_Addr(unsigned char Icode,unsigned long BaseAddr);
unsigned long Katakana_GetData(unsigned char code,unsigned char *DZ_Data);
unsigned long CP_Patch_8X16_GetData(unsigned int code,unsigned char *DZ_Data);
unsigned long CP_Patch_16X16_GetData(unsigned int code,unsigned char *DZ_Data);
unsigned long CP_P_KATAKANA_16X16_GetData(unsigned int code,unsigned char *DZ_Data);
unsigned long cp_patch_12x24(unsigned int code,unsigned char *DZ_Data);
unsigned long ISO8859_8X16_GetData(unsigned int Fontcode,unsigned char Num,unsigned char *DZ_Data);
unsigned long U2J_GetData(unsigned int Unicode,unsigned char *DZ_Data);
unsigned long Shift_JIS_TO_JIS0208(unsigned int code16);
unsigned long U2K_GetData_16X16(unsigned int Unicode,unsigned char *DZ_Data);
unsigned long U2K_GetData_24X24(unsigned int Unicode,unsigned char *DZ_Data);
unsigned long U2G_GetData_12X12(unsigned int  unicode,unsigned char *DZ_Data);
unsigned long U2G_GetData_16X16(unsigned int  unicode,unsigned char *DZ_Data);
unsigned long U2G_GetData_24X24(unsigned int  unicode,unsigned char *DZ_Data);
unsigned long BIG5_GBK( unsigned char h,unsigned char l);
unsigned long  U2G(unsigned int  unicode);

unsigned long U2K(unsigned int Unicode);
unsigned long U2J(unsigned int Unicode);

void CP_12X24_GetData(unsigned int code,unsigned char cpsel,unsigned char *DZ_data);
void INTERNATIONAL_CHARACTER_12X24(unsigned char Icode,unsigned long INTERNATIONAL_CHARACTER,unsigned char  *DZ_Data);



#endif
