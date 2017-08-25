# gt20l16s1y
## Display Chinese with U8glib and GT20L16S1Y
### For Arduino
## Usage：
1.Copy the codes in the GT20L16S1Y.c and paste them into your code.
2.Ensure your U8g object is named "u8g".
3.Then call displayStr(int x, int y, int n, unsigned char *c).
##### Like this:
  ```
  unsigned char name0[20] PROGMEM = {0x4D, 0x5A, 0x59, 0x37, 0x20, 0x5A, 0x65, 0x72, 0x6F, 0x57, 0x61, 0x74, 0x63, 0x68, 0x76, 0x31};
  displayStr(0, 0, 16, name0);  
  
  x and y:You know.
  n:How many characters you want to display. A Chinese character is 2 characters.
  ```
