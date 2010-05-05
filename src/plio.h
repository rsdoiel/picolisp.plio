/* 12nov09abu
 * PicoLisp I/O functionality
 */

/*** Print Functions ***/
// Return -1 if error
int putInt32(int32_t n);
int putInt64(int64_t n);
int putBig(char *buf, int len);
int putSym(char *s);
int putStr(char *s);


/*** Read Functions ***/
// Return >= 0 if Ok
// -1 for unexpected EOF
// -2 for protocol errors
// -3 for buffer overruns
int getInt32(int32_t *dst);         // Return 0
int getInt64(int64_t *dst);         // Return 0
int getBig(char *dst, int siz);     // Return number of bytes
int getSym(char *dst, int siz);     // Return name length
int getStr(char *dst, int siz);     // Return string length
