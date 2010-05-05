/* 12nov09abu
 * PicoLisp I/O functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plio.h"

typedef int bool;

// I/O Tokens
enum {NIX, BEG, DOT, END};
enum {NUMBER, INTERN, TRANSIENT, EXTERN};

#define PUT(c) putchar(c)
#define GET()  getchar()

static int putBytes(int t, char *p, int len) {
   int i;

   if (len == 0) {
      if (PUT(NIX) < 0)
         return -1;
   }
   else if (len < 63) {
      if (PUT(len*4 | t) < 0)
         return -1;
      do
         if (PUT(*p++) < 0)
            return -1;
      while (--len > 0);
   }
   else {
      if (PUT(63*4 | t) < 0)
         return -1;
      for (i = 0; i < 63; ++i)
         if (PUT(*p++) < 0)
            return -1;
      len -= 63;
      while (len >= 255) {
         if (PUT(255) < 0)
            return -1;
         for (i = 0; i < 255; ++i)
            if (PUT(*p++) < 0)
               return -1;
         len -= 255;
      }
      if (PUT(len) < 0)
         return -1;
      while (--len >= 0)
         if (PUT(*p++) < 0)
            return -1;
   }
   return 0;
}

static int getBytes(int c, char **dst, int len) {
   int cnt;
   bool more;
   char *p = *dst;

   cnt = c / 4;
   more = cnt == 63;

   for (;;) {
      if ((c = GET()) < 0)
         return -1;
      if (--len < 0)
         return -3;
      *p++ = c;
      if (--cnt == 0) {
         if (!more || (cnt = GET()) == 0) {
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
int putInt32(int32_t n) {
   n = n >= 0? n * 2 : -n * 2 + 1;
   if ((n & 0xFFFFFF00) == 0) {
      if (PUT(1*4)<0 || PUT(n)<0)
         return -1;
   }
   else if ((n & 0xFFFF0000) == 0) {
      if (PUT(2*4)<0 || PUT(n)<0 || PUT(n>>8)<0)
         return -1;
   }
   else if ((n & 0xFF000000) == 0) {
      if (PUT(3*4)<0 || PUT(n)<0 || PUT(n>>8)<0 || PUT(n>>16)<0)
         return -1;
   }
   else {
      if (PUT(4*4)<0 || PUT(n)<0 || PUT(n>>8)<0 || PUT(n>>16)<0 || PUT(n>>24)<0)
         return -1;
   }
   return 0;
}

int putInt64(int64_t n) {
   if (n > 0x7FFFFFFF || n < -0x7FFFFFFF) {
      int i;
      char buf[sizeof(int64_t)];

      n = n >= 0? n * 2 : -n * 2 + 1;
      for (i = 0; i < (int)sizeof(int64_t); ++i)
         buf[i] = (char)(n & 0xFF),  n >>= 8;
      return putBytes(NUMBER, buf, sizeof(int64_t));
   }
   return putInt32((int32_t)n);

}

int putBig(char *buf, int len) {
   return putBytes(NUMBER, buf, len);
}

int putSym(char *s) {
   return putBytes(INTERN, s, strlen(s));
}

int putStr(char *s) {
   return putBytes(TRANSIENT, s, strlen(s));
}


/*** Read Functions ***/
int getInt32(int32_t *dst) {
   int c, cnt, i;
   int32_t n;

   if ((c = GET()) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   if ((cnt = c / 4 - 1) >= 4)
      return -3;
   i = 0;
   if ((n = GET()) < 0)
      return -1;
   while (--cnt >= 0) {
      if ((c = GET()) < 0)
         return -1;
      n |= (int32_t)c << (i += 8);
   }
   i = n & 1;
   n = (int32_t)((u_int32_t)n >> 1);
   *dst = i? -n : n;
   return 0;
}

int getInt64(int64_t *dst) {
   int c, cnt, i;
   int64_t n;

   if ((c = GET()) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   if ((cnt = c / 4 - 1) >= 8)
      return -3;
   i = 0;
   if ((n = GET()) < 0)
      return -1;
   while (--cnt >= 0) {
      if ((c = GET()) < 0)
         return -1;
      n |= (int64_t)c << (i += 8);
   }
   i = n & 1;
   n = (int64_t)((u_int64_t)n >> 1);
   *dst = i? -n : n;
   return 0;
}

int getBig(char *dst, int siz) {
   int c;

   if ((c = GET()) < 0)
      return -1;
   if (c <= END  ||  (c & 3) != NUMBER)
      return -2;
   return getBytes(c, &dst, siz);
}

int getSym(char *dst, int siz) {
   int c, cnt;

   if ((c = GET()) < 0)
      return -1;
   if (c == NIX) {
      *dst = '\0';
      return 0;
   }
   if (c <= END  ||  (c & 3) != INTERN)
      return -2;
   if ((cnt = getBytes(c, &dst, siz-1)) >= 0)
      *dst = '\0';
   return cnt;
}

int getStr(char *dst, int siz) {
   int c, cnt;

   if ((c = GET()) < 0)
      return -1;
   if (c == NIX) {
      *dst = '\0';
      return 0;
   }
   if (c <= END  ||  (c & 3) != TRANSIENT)
      return -2;
   if ((cnt = getBytes(c, &dst, siz-1)) >= 0)
      *dst = '\0';
   return cnt;
}
