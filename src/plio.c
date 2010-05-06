/* 05may10jir
 * PicoLisp I/O functionality
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "plio.h"

typedef int bool;

// I/O Tokens
enum {NIX, BEG, DOT, END};
enum {NUMBER, INTERN, TRANSIENT, EXTERN};

//#define PUT(fd, c) fdutc(c, fd)
//#define GET(fd)  fgetc(fd)

inline ssize_t PUT(int fd, char c) {
  return write(fd, &c, 1);
}

inline ssize_t GET(int fd) {
  unsigned char b; int r;
  r = read(fd, &b, 1);
  if (r < 0)
    return r;
  return b;
}

static int putBytes(int fd, int t, char *p, int len) {
   int i;

   if (len == 0) {
      if (PUT(fd, NIX) < 0)
         return -1;
   }
   else if (len < 63) {
      if (PUT(fd, len*4 | t) < 0)
         return -1;
      do
         if (PUT(fd, *p++) < 0)
            return -1;
      while (--len > 0);
   }
   else {
      if (PUT(fd, 63*4 | t) < 0)
         return -1;
      for (i = 0; i < 63; ++i)
         if (PUT(fd, *p++) < 0)
            return -1;
      len -= 63;
      while (len >= 255) {
         if (PUT(fd, 255) < 0)
            return -1;
         for (i = 0; i < 255; ++i)
            if (PUT(fd, *p++) < 0)
               return -1;
         len -= 255;
      }
      if (PUT(fd, len) < 0)
         return -1;
      while (--len >= 0)
         if (PUT(fd, *p++) < 0)
            return -1;
   }
   return 0;
}

static int getBytes(int fd, int c, char **dst, int len) {
   int cnt;
   bool more;
   char *p = *dst;

   cnt = c / 4;
   more = cnt == 63;

   for (;;) {
      if ((c = GET(fd)) < 0)
         return -1;
      if (--len < 0)
         return -3;
      *p++ = c;
      if (--cnt == 0) {
         if (!more || (cnt = GET(fd)) == 0) {
            cnt = p - *dst;
            *dst = p;
            return cnt;
         }
         if (cnt < 0)
            return -1;
         more = cnt == 255;
      }
   }
}


/*** Print Functions ***/
int putInt32(int fd, int32_t n) {
   n = n >= 0? n * 2 : -n * 2 + 1;
   if ((n & 0xFFFFFF00) == 0) {
      if (PUT(fd, 1*4)<0 || PUT(fd, n)<0)
         return -1;
   }
   else if ((n & 0xFFFF0000) == 0) {
      if (PUT(fd, 2*4)<0 || PUT(fd, n)<0 || PUT(fd, n>>8)<0)
         return -1;
   }
   else if ((n & 0xFF000000) == 0) {
      if (PUT(fd, 3*4)<0 || PUT(fd, n)<0 || PUT(fd, n>>8)<0 || PUT(fd, n>>16)<0)
         return -1;
   }
   else {
      if (PUT(fd, 4*4)<0 || PUT(fd, n)<0 || PUT(fd, n>>8)<0 || PUT(fd, n>>16)<0 || PUT(fd, n>>24)<0)
         return -1;
   }
   return 0;
}

int putInt64(int fd, int64_t n) {
   if (n > 0x7FFFFFFF || n < -0x7FFFFFFF) {
      int i;
      char buf[sizeof(int64_t)];

      n = n >= 0? n * 2 : -n * 2 + 1;
      for (i = 0; i < (int)sizeof(int64_t); ++i)
         buf[i] = (char)(n & 0xFF),  n >>= 8;
      return putBytes(fd, NUMBER, buf, sizeof(int64_t));
   }
   return putInt32(fd, (int32_t)n);

}

int putBig(int fd, char *buf, int len) {
   return putBytes(fd, NUMBER, buf, len);
}

int putSym(int fd, char *s) {
   return putBytes(fd, INTERN, s, strlen(s));
}

int putStr(int fd, char *s) {
   return putBytes(fd, TRANSIENT, s, strlen(s));
}


/*** Read Functions ***/
int getInt32(int fd, int32_t *dst) {
   int c, cnt, i;
   int32_t n;

   if ((c = GET(fd)) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   if ((cnt = c / 4 - 1) >= 4)
      return -3;
   i = 0;
   if ((n = GET(fd)) < 0)
      return -1;
   while (--cnt >= 0) {
      if ((c = GET(fd)) < 0)
         return -1;
      n |= (int32_t)c << (i += 8);
   }
   i = n & 1;
   n = (int32_t)((u_int32_t)n >> 1);
   *dst = i? -n : n;
   return 0;
}

int getInt64(int fd, int64_t *dst) {
   int c, cnt, i;
   int64_t n;

   if ((c = GET(fd)) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   if ((cnt = c / 4 - 1) >= 8)
      return -3;
   i = 0;
   if ((n = GET(fd)) < 0)
      return -1;
   while (--cnt >= 0) {
      if ((c = GET(fd)) < 0)
         return -1;
      n |= (int64_t)c << (i += 8);
   }
   i = n & 1;
   n = (int64_t)((u_int64_t)n >> 1);
   *dst = i? -n : n;
   return 0;
}

int getBig(int fd, char *dst, int siz) {
   int c;

   if ((c = GET(fd)) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   return getBytes(fd, c, &dst, siz);
}

int getSym(int fd, char *dst, int siz) {
   int c, cnt;

   if ((c = GET(fd)) < 0)
      return -1;
   if (c == NIX) {
      *dst = '\0';
      return 0;
   }
   if (c <= END  ||  (c & 3) != INTERN)
      return -2;
   if ((cnt = getBytes(fd, c, &dst, siz-1)) >= 0)
      *dst = '\0';
   return cnt;
}

int getStr(int fd, char *dst, int siz) {
   int c, cnt;

   if ((c = GET(fd)) < 0)
      return -1;
   if (c == NIX) {
      *dst = '\0';
      return 0;
   }
   if (c <= END  ||  (c & 3) != TRANSIENT)
      return -2;
   if ((cnt = getBytes(fd, c, &dst, siz-1)) >= 0)
      *dst = '\0';
   return cnt;
}
