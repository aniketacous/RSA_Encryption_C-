

#include <iostream>
#include <cmath>
#include <string>
#include <math.h> 
#include <stdlib.h>
#include <cstddef>
#include <vector>
#include <random>
#include <time.h>
#include <cstdint>
#include <fstream>
#include "codec.h"
#include <cstdlib>
 
using namespace std;

// GCD 
int64_t gcd(int64_t a, int64_t b)

{
    int64_t t;
    while(b!=0)
    {
        t=b;
        b=a%b;
        a=t;

    }
    return a;
}
  void usage()
  { 
    cout << " This might work" << endl;
  }
     int64_t extEuclid(int64_t a, int64_t b, int64_t &lastX, int64_t &lastY) {
     int64_t q,r,x=0, y=1, tmp;
     lastX=1, lastY=0;
     while(b!=0)
     {  
     q=a/b;
     r= a%b;
     tmp =x; 
     x=lastX-q*x;
     lastX = tmp;
     tmp=y;
     y=lastY-q*y;
     lastY= tmp;
     a=b;
     b=r;
     }

      return a;
   }
  
//prime number
bool isPrime(int64_t n)
  {
  if (n < 2)
    return false;
  if (n == 2)
    return true;
  if (n % 2 == 0)
    return false;

  for(int64_t i = 3; i*i<=n; i+=2)
    {
      if(n%i==0)
        {
	  return false;
        }
    }

  return true;
}
// Key Generation
void KeyGen(void)
{
  srand(time(NULL));
  int64_t p,q,e,f,x,d,g,n;  //declration of variables
   
  do {                       // findind random prime number p
  p = rand() % (65536-4096) + 4096;
     }
  while(!isPrime(p));
   
      
     
  do {                      //finding random prime number q

  q=rand() % (65536-4096) + 4096;
     }
  while(!isPrime(q));
   
   f=(p-1)*(q-1);       //computing f
   n=p*q;              // computing n
  do {
    e= rand()%(65536-4096) + 4096; 
  } while (gcd(e,f) != 1);
    
  g = extEuclid(e,f,d,x); 
  if (g != 1) 
   { 
    cout << "Error!" << endl;
    return;
   }
   if(d<0)
    d += f;

cout << p << ' ' << q << ' ' << f << endl;

 cout<< " KeyGen Functions are: " << endl;
 cout << "Public Key" <<"   "<<n<<"   " <<e<< endl;
 cout << "Private Key"<<"  "<<n<<"   " <<d<< endl;
 
  
}
 int64_t modExp(int64_t b,int64_t e,int64_t m)
{
  int64_t ans=1;
   while(e!=0)
    {
      if(e%2==1)
      ans=(ans*b)%m;
      b=(b*b)%m;
      e=e/2;
    }
    return ans;
} 

  // output n, e as the public key
Codec64 codec;
void encrypt(char *infn, char *outfn, int64_t n, int64_t e)
{

char C1, C2, C3;
int64_t  M, m;
ifstream inFile;
ofstream outFile;

inFile.open(infn);
if (!inFile) {
  cout << "Error: cannot open input file" << endl;
  return;
}
outFile.open(outfn);
if (!outFile) {
  inFile.close();
  cout << "Error: cannot open output file" << endl;
  return;
}

codec.beginEncode();

while(true)
   {
    inFile.get(C1);
   if(!inFile)
    break;
   inFile.get(C2);
    if(!inFile)
     C2=0;
   inFile.get(C3);
     if(!inFile)
     C3=0;

     m=C1+256*C2+65536*C3;
     M=modExp(m,e,n);
    codec.put64(M);
	
    while(codec.getEncodedChar(C1))
     outFile <<C1;
     }
     codec.endEncode();
      while(codec.getEncodedChar(C1))
     outFile <<C1;
outFile << endl;
outFile.close();
}


  // output n, d as the private key
  void decrypt(char *infn, char *outfn, int64_t n, int64_t d)
{
  char C1, ch;
  uint64_t M, m;
  ifstream inFile;
  ofstream outFile;
  inFile.open(infn);
  if (!inFile) {
     cout << "Error: cannot open input file" << endl;
     return;
   }
  outFile.open(outfn);
  if (!outFile) {
    inFile.close();
    cout << "Error: cannot open output file" << endl;
    return;
  }
  codec.beginDecode();
     while(true)
       {
	 //inFile.get(C1);
	 inFile >> C1;
//cout << C1;
	 if (!inFile)
	   break;
	 codec.putEncodedChar(C1);

	 if (codec.get64(M))
	   {
	     m=modExp(M,d,n);
	     ch = m%256;
	     m= m/256;
	     if(ch!=0)
	       outFile << ch;
	     ch = m%256;
	     m=m/256;
	     if(ch!=0)
	       outFile << ch;
	     ch = m%256;
	     m=m/256;
	     if(ch!=0)
	       outFile << ch;
	   }
       }
     codec.endDecode();
     inFile.close();
     outFile.close();
}
int main(int argc, char *argv[])
{
     int64_t n,e,d;

     if (argc < 2) {
       usage();
       return 1;
     } 
     if (argv[1][0]!='-') {
	     usage();
	     return 1;
     }
     if(argv[1][1]=='k')
       {
	 if(argc !=2)
	   { 
	     usage();
	     return 1;
	   }
	 KeyGen();
       }
     else if(argv[1][1]=='e')
       {
	 if (argc!=6)
	   {
	     usage();
	     return 1;
	   }
	 n= atoll(argv[2]);
	 e= atoll(argv[3]);
	 encrypt(argv[4], argv[5], n, e);
       }
     else if(argv[1][1]== 'd')
       {
	 if (argc!=6)
	   {
	     usage();
	     return 1;
	   }
	 n= atoll(argv[2]);
	 d= atoll(argv[3]);
	 decrypt(argv[4], argv[5], n, d);
       }
     else
       {
	 usage();
	 return 1;
       }
     return 0;
}
