/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  //x,y全为1时，&才为1，异或运算^是x,y相同为0，不同为1
 //或者说:~((x&y)|(~x&~y))
  return ~(~x&~y)&~(x&y);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void){
	//最小二进制补码:10000000000000000000000000000000
  return 1<<31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    return !(~((x+1)^x))+!!(~x)+(~0);//-1需要特判
 // return !!(~x) & !((~x) ^ (x + 1));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  //odd位为1则为1，否则为0,用1010，也就是0xA
//因为只需要关注奇数位，所以采用异或找到相同取0就行
  int k=(0xAA<<24)+(0xAA<<16)+(0xAA<<8)+(0xAA);
  return !((x&k)^k);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  //取相反数，即取非＋1，就是求补码
  return ~x+1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  return (!((x>>4)^3))&((((x>>3)&1)&!(x>>1&1)&!(x>>2&1))|!((x>>3)&1));
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  //如果x为0则为0，如果x为非0则为1
  int a=!!x;
  int fan=~a+1;//取相反数,x为1,fan为0xffffffff or x为0,fan为0x00000000
//以fan为位均为1
  return (fan&y)|(~fan&z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  //x==y好判断,x<y <=> x-y<0 <=> x+(~y)+1<0 <=> 判断最高位即符号位是否为1
//  return !(x^y)|((x+(~y)+1)>>31&1); 出错因为这题x，y运算还需要考虑溢出情况
  int flagx=x>>31&1;
  int flagy=y>>31&1;
//如果符号相同则x-y不存在溢出，如果符号不同则可能存在溢出，所以这里需要判断符号
  int flag=!!(flagx^flagy);//相同为0，不同为1
  return  (!(x^y))|((!flag)&((x+(~y)+1)>>31&1))|(flag&flagx);//不同存在溢出而无法计算进行ret归类的情况是，x<0,y>0
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  x=conditional(x>>31&1,~x,x);//x?~x:x 正数为最左边1＋1（符号位0）负数为最左边0+1（符号位1） 全部转成正数则都为最左边1+1
    //(~!(x>>16)+1) 高16位有1则为0，无则为-1
    int bits16=~(~!(x>>16)+1)&16;//16 0
    x>>=bits16;
    int bits8=~(~!(x>>8)+1)&8;//8 0
    x>>=bits8;
    int bits4=~(~!(x>>4)+1)&4;//4 0
    x>>=bits4;
    int bits2=~(~!(x>>2)+1)&2;//2 0
    x>>=bits2;
    int bits1=~(~!(x>>1)+1)&1;//1 0
    x>>=bits1;
    int bits0=x;
    int bits=bits16+bits8+bits4+bits2+bits1+bits0+1;
    return bits;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
	 unsigned s=uf>>31;
        unsigned expr=uf>>23&0xFF;
        unsigned frac=uf&0x7FFFFF;
       //(-1)^s * (M) * (2^E)
//E=expr-bias=expr-127
        //0
        if(expr==0&&frac==0)
                return uf;
        //NaN or inf
        if(expr==0xFF)
                return uf;
        //非规格化
        if(expr==0){
                frac<<=1;
                return (s<<31)|frac;
        }
        //规格化
        //e+1,E=e-bias+1-->E++-->f*2
        expr+=1;
        return (s<<31)|(expr<<23)|frac;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned s=uf>>31;
        unsigned expr=uf>>23&0xFF;
        unsigned frac=uf&0x7FFFFF;
//规格化：s 非0xx非1xx f
//非规格化: s 0xx f
//inf: s 1xx 0xx
//NaN: s 1xx 非0xx
        //0
         if(expr==0&&frac==0)
                return 0;
        //inf or NaN
        if(expr==0xFF)
                return 1<<31;
        //非规格化 M=0.xxx<1
        if(expr==0){
                return 0;
        }
        //规格化 M=1.xxx>1
        int E=expr-127;
        frac=frac|(1<<23);//转成1.f
        if(E<0)
                return 0;
        else if(E>31)//溢出
                return 1<<31;
        if(E<=23)
                frac>>=(23-E);//右移截断低位,小数
        else frac<<=(E-23);//左移扩展，保证23位
        if(s) frac=~frac+1;
        return frac;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
     int expr=x+127;
   if(x>=128) return 0xFF<<23;//inf,指数位全1
   if(x>=-126) return expr<<23;//规格，指数位移动相应位置
   if(x>=-149) return 1<<(x+149);//非规格[-149,-126)，2^-(x+149)
   return 0;
}
