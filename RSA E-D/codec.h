#ifndef _CODEC_H
#define _CODEC_H

const unsigned int CODEC_BUF_SIZE = 16;

class Codec64 {
 public:
  Codec64(void);
  ~Codec64(void) { }

  bool getEncodedChar(char &ch);
  bool putEncodedChar(char ch);

  void put8(unsigned char n);

  void put16(unsigned short int n);

  void put32(unsigned int n);

  void put64(unsigned long long int n);

  bool get8(unsigned char &n);

  bool get16(unsigned short int &n);

  bool get32(unsigned int &n);

  bool get64(uint64_t &n);

  void beginEncode(void);
  void beginDecode(void);
  void endEncode(void);
  void endDecode(void);

 private:
  unsigned char
    inBuffer[CODEC_BUF_SIZE],
    outBuffer[CODEC_BUF_SIZE];
  unsigned int
    inHead,inTail,inCount,
    outHead,outTail,outCount;
  bool
    isEncoding,
    isDecoding;
};

#endif