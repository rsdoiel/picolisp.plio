/* 05may10jir
 * PicoLisp I/O functionality
 */

/*** Print Functions ***/
// Return -1 if error
int putInt32(int fd, int32_t n);
int putInt64(int fd, int64_t n);
int putBig(int fd, char *buf, int len);
int putSym(int fd, char *s);
int putStr(int fd, char *s);


/*** Read Functions ***/
// Return >= 0 if Ok
// -1 for unexpected EOF
// -2 for protocol errors
// -3 for buffer overruns
int getInt32(int fd, int32_t *dst);         // Return 0
int getInt64(int fd, int64_t *dst);         // Return 0
int getBig(int fd, char *dst, int siz);     // Return number of bytes
int getSym(int fd, char *dst, int siz);     // Return name length
int getStr(int fd, char *dst, int siz);     // Return string length
