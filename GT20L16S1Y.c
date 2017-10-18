#include <SPI.h>

#define CS 10
#define SO 11
#define SI 12
#define CL 13

long getAddr(uint16_t str)
{
  byte hb = (byte)((str & 0xff00) >> 8);
  byte lb = (byte)(str & 0x00ff);
  Serial.print("HB ");
  Serial.print(hb, HEX);
  Serial.print(" LB ");
  Serial.println(lb, HEX);
  long Address;
  if (hb > 0xA0 && lb > 0xA0)//is GB2312
  {
    long BaseAdd = 0;
    if (hb == 0xA9 && lb >= 0xA1)
    {
      Address = (282 + (lb - 0xA1 ));//8位机在此出现了溢出，所以分三步
      Address = Address * 32;
      Address += BaseAdd;
    }
    else if (hb >= 0xA1 && hb <= 0xA3 && lb >= 0xA1)
    {
      Address = ((hb - 0xA1) * 94 + (lb - 0xA1));
      Address = Address * 32;
      Address += BaseAdd;
    }
    else if (hb >= 0xB0 && hb <= 0xF7 && lb >= 0xA1)
    {
      Address = ((hb - 0xB0) * 94 + (lb - 0xA1) + 846);
      Address = Address * 32;
      Address += BaseAdd;
    }
  } else {//is ASCII
    long BaseAdd = 0x03b7c0;
    if (lb >= 0x20 && lb <= 0x7E)
    {
      Address = (lb - 0x20 ) * 16 + BaseAdd;
    }
  }
  return Address;
}

byte revBit(byte input)
{
  byte output = 0;
  for (int i = 0;  i < 8;  i++)
  {
    if (input & (1 << i))
      output |= 1 << (7 - i);
  }
  return output;
}

void fetchBitmap16(long address, byte *p)
{
  byte hb = (byte)((address & 0x00ff0000) >> 16);
  byte mb = (byte)((address & 0x0000ff00) >> 8);
  byte lb = (byte)(address & 0x000000ff);
  digitalWrite(CS, LOW);//选通字库
  SPI.transfer(0x0b);//移入命令字
  SPI.transfer(hb);//移入地址次高位
  SPI.transfer(mb);//移入地址次低位
  SPI.transfer(lb);//移入地址最低位
  SPI.transfer(0x8d);//移入dummy byte
  for (int i = 0; i < 16; i++)
  {
    byte buf = SPI.transfer(0x00); //读出16bytes
    p[i] = revBit(buf);
  }
  digitalWrite(CS, HIGH);//反选通字库
}

void fetchBitmap32(long address, byte *p)
{
  byte hb = (byte)((address & 0x00ff0000) >> 16);
  byte mb = (byte)((address & 0x0000ff00) >> 8);
  byte lb = (byte)(address & 0x000000ff);
  digitalWrite(CS, LOW);//选通字库
  SPI.transfer(0x0b);//移入命令字
  SPI.transfer(hb);//移入地址次高位
  SPI.transfer(mb);//移入地址次低位
  SPI.transfer(lb);//移入地址最低位
  SPI.transfer(0x8d);//移入dummy byte
  for (int i = 0; i < 32; i++)
  {
    byte buf = SPI.transfer(0x00); //读出32bytes
    p[i] = revBit(buf);
  }
  digitalWrite(CS, HIGH);//反选通字库
}

void drawV8P(xint x, int y, byte b)
{
  if ((b & 0x80) == 0x80)
    u8g.drawPixel(x, y);
  if ((b & 0x40) == 0x40)
    u8g.drawPixel(x, y + 1);
  if ((b & 0x20) == 0x20)
    u8g.drawPixel(x, y + 2);
  if ((b & 0x10) == 0x10)
    u8g.drawPixel(x, y + 3);
  if ((b & 0x08) == 0x08)
    u8g.drawPixel(x, y + 4);
  if ((b & 0x04) == 0x04)
    u8g.drawPixel(x, y + 5);
  if ((b & 0x02) == 0x02)
    u8g.drawPixel(x, y + 6);
  if ((b & 0x01) == 0x01)
    u8g.drawPixel(x, y + 7);
}

void draw8x16(int x, int y, byte *b)
{
  int i;
  for (i = 0; i < 8; i++)
  {
    drawV8P(x + i, y, b[i]);
  }
  for (i = 0; i < 8; i++)
  {
    drawV8P(x + i, y + 8, b[i + 8]);
  }
}

void draw16x16(int x, int y, byte *b)
{
  int i;
  for (i = 0; i < 16; i++)
  {
    drawV8P(x + i, y, b[i]);
  }
  for (i = 0; i < 16; i++)
  {
    drawV8P(x + i, y + 8, b[i + 16]);
  }
}

void displayStr(int x, int y, int len, unsigned char *c)
{
  int i;
  for (i = 0; i < len; i++)
  {
    if (c[i] > 0xA0) //is GB2312
    {
      int code = (int)(c[i] * 256) + (int)c[i + 1];
      byte bmp[32];
      fetchBitmap32(getAddr(code), bmp);
      draw16x16(x, y, bmp);
      x += 16;
      i++;
    } else { //is ASCII
      int code = (int)c[i];
      byte bmp[16];
      fetchBitmap16(getAddr(code), bmp);
      draw8x16(x, y, bmp);
      x += 8;
    }
  }
}

void displayStrC(int row, int len, unsigned char *c)
{
	int x=64-((len*8)/2);
	displayStr(x,row,len,c);
}