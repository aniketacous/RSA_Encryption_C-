#include <cstdint>
#include "codec.h"

static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "0123456789+/";

Codec64::Codec64(void) {

  isEncoding = isDecoding = false;
  inHead = inTail = inCount = outHead = outTail = outCount = 0;
}

bool Codec64::getEncodedChar(char &ch) {

  if (outCount == 0)
    return false;

  ch = outBuffer[outHead];
  outHead = ++outHead % CODEC_BUF_SIZE;
  outCount--;

  return true;
}

bool Codec64::putEncodedChar(char ch) {
  char
    c[4];
  unsigned int
    tmp,
    i;

  if (!isDecoding)
    return false;

  inBuffer[inTail] = ch;
  inTail = ++inTail % CODEC_BUF_SIZE;
  inCount++;

  if (inCount == 4) {
    for (i=0;i<4;i++) {
      c[i] = inBuffer[inHead];
      inHead = ++inHead % CODEC_BUF_SIZE;
      inCount--;
    }
    for (i=0;c[0]!=charset[i];i++);
    tmp = i << 18;
    for (i=0;c[1]!=charset[i];i++);
    tmp += i << 12;

    outBuffer[outTail] = (tmp >> 16) & 0xff;
    outTail = ++outTail % CODEC_BUF_SIZE;
    outCount++;

    if (c[2] != '=') {
      for (i=0;c[2]!=charset[i];i++);
      tmp += i << 6;

      outBuffer[outTail] = (tmp >> 8) & 0xff;
      outTail = ++outTail % CODEC_BUF_SIZE;
      outCount++;

      if (c[3] != '=') {
        for (i=0;c[3]!=charset[i];i++);
        tmp += i;

        outBuffer[outTail] = tmp & 0xff;
        outTail = ++outTail % CODEC_BUF_SIZE;
        outCount++;

      }
    }
  }

  return true;
}

void Codec64::put8(unsigned char n) {
  unsigned int
    i,
    tmp;
  unsigned char
    ch;

  if (isEncoding) {
    inBuffer[inTail] = n;
    inTail = ++inTail % CODEC_BUF_SIZE;
    inCount++;

    if (inCount == 3) {
      tmp = 0;
      for (i=0;i<3;i++) {
        ch = inBuffer[inHead];
        inHead = ++inHead % CODEC_BUF_SIZE;
        inCount--;
        tmp = (tmp << 8) + ch;
      }
      for (i=0;i<4;i++) {
        ch = charset[(tmp >> (18-6*i)) & 0x3f];
        outBuffer[outTail] = ch;
        outTail = ++outTail % CODEC_BUF_SIZE;
        outCount++;
      }
    }
  }
}

void Codec64::put16(unsigned short int n) {

  put8((unsigned char)(n & 0xff));
  put8((unsigned char)(n >> 8));
}

void Codec64::put32(unsigned int n) {

  put16((unsigned short int)(n & 0xffff));
  put16((unsigned short)(n >> 16));
}

void Codec64::put64(unsigned long long int n) {

  put32((unsigned int)(n & 0xffffffff));
  put32((unsigned int)(n >> 32));
}

bool Codec64::get8(unsigned char &n) {

  if (outCount == 0)
    return false;

  n = outBuffer[outHead];
  outHead = ++outHead % CODEC_BUF_SIZE;
  outCount--;

  return true;
}

bool Codec64::get16(unsigned short int &n) {
  unsigned char
    high,low;

  if (outCount < 2)
    return false;

  get8(low);
  get8(high);

  n = low + ((short int)high << 8);

  return true;
}

bool Codec64::get32(unsigned int &n) {
  unsigned short int
    high,low;

  if (outCount < 4)
    return false;

  get16(low);
  get16(high);

  n = low + ((int)high << 16);

  return true;
}

bool Codec64::get64(uint64_t &n) {
  unsigned int
    high,low;

  if (outCount < 8)
    return false;

  get32(low);
  get32(high);

  n = low + ((unsigned long long int)high << 32);

  return true;
}

void Codec64::beginEncode(void) {
  inCount = inHead = inTail = outCount = outHead = outTail = 0;
  isEncoding = true;
  isDecoding = false;
}

void Codec64::beginDecode(void) {
  inCount = inHead = inTail = outCount = outHead = outTail = 0;
  isEncoding = false;
  isDecoding = true;
}

void Codec64::endEncode(void) {

  if (inCount == 2) {
    put8(0);
    outTail = (outTail + CODEC_BUF_SIZE - 1) % CODEC_BUF_SIZE;
    outBuffer[outTail] = '=';
  }

  if (inCount == 1) {
    put16(0);
    outTail = (outTail + CODEC_BUF_SIZE - 2) % CODEC_BUF_SIZE;
    outBuffer[outTail] = '=';
    outBuffer[(outTail+1)%CODEC_BUF_SIZE] = '=';
  }

  isEncoding = false;
}

void Codec64::endDecode(void) {

  isDecoding = false;
}
