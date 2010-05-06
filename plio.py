#! /usr/bin/env python

from ctypes import *
libplio = CDLL("lib/libplio")

"""
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
"""



tf = open("test-data", "w")
tfd = tf.fileno()

string = "12345678901234567890123456789012"
print "Writting str=", string, "returned", libplio.putStr(tfd, string)
number = 9001
print "Writting int=", number, "returned", libplio.putInt32(tfd, number)
tf.close()

tf = open("test-data")
tfd = tf.fileno()

s = c_char_p(" "*100)
n = c_int(0)

print "Reading a str returned", libplio.getStr(tfd, s, 100)
print "The string was ", s.value
print "Reading an int returned", libplio.getInt32(tfd, byref(n))
print "The int was", n.value
