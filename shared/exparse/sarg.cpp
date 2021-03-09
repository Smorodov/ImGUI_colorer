SArg::SArg()
{
  type = SA_DOUBLE;
  arg.v_dbl = 0;
  empty = true;
};
SArg::~SArg()
{
};
SArg::SArg(SArg &a)
{
  type = a.type;
  arg  = a.arg;
  empty = a.empty;
};

SArg::SArg(__int64 var)
{
  type = SA_INT64;
  arg.v_int64 = var;
  empty = false;
};
SArg::SArg(bool var)
{
  type = SA_INT;
  arg.v_int = var;
  empty = false;
};
SArg::SArg(int var)
{
  type = SA_INT;
  arg.v_int = var;
  empty = false;
};
SArg::SArg(short var)
{
  type = SA_SHORT;
  arg.v_short = var;
  empty = false;
};
SArg::SArg(char var)
{
  type = SA_CHAR;
  arg.v_char = var;
  empty = false;
};
SArg::SArg(unsigned __int64 var)
{
  type = SA_UINT64;
  arg.v_uint64 = var;
  empty = false;
};
SArg::SArg(unsigned int var)
{
  type = SA_UINT;
  arg.v_uint = var;
  empty = false;
};
SArg::SArg(unsigned short var)
{
  type = SA_USHORT;
  arg.v_ushort = var;
  empty = false;
};
SArg::SArg(unsigned char var)
{
  type = SA_BYTE;
  arg.v_byte = var;
  empty = false;
};
SArg::SArg(double var)
{
  type = SA_DOUBLE;
  arg.v_dbl = var;
  empty = false;
};
SArg::SArg(float var)
{
  type = SA_FLOAT;
  arg.v_flt = var;
  empty = false;
};
SArg::SArg(dumpdata dd)
{
  empty = false;
  type = SA_DUMP;
  memmove(arg.v_dump,dd.dump,8);
};

//////////////////////////// operators /////////////////////////////////

SArg &SArg::operator=(SArg op)
{
  type = op.type;
  empty = op.empty;
  arg = op.arg;
  return *this;
};

SArg &SArg::operator=(double op)
{
  type = SA_DOUBLE;
  empty = false;
  arg.v_dbl = op;
  return *this;
};

SArg SArg::operator+(SArg op)
{
  if (op.type == SA_DOUBLE || op.type == SA_FLOAT)
    return (double)*this + (double)op;
  switch(type){
    case SA_INT64:  return arg.v_int64 + (__int64)op;
    case SA_INT:    return arg.v_int + (int)op;
    case SA_SHORT:  return short(arg.v_short + (short)op);
    case SA_CHAR:   return char(arg.v_char + (char)op);
    case SA_UINT64: return arg.v_uint64 + (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint + (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort + (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte + (unsigned char)op);
    case SA_DOUBLE: return arg.v_dbl + (double)op;
    case SA_FLOAT:  return arg.v_flt + (float)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator-(SArg op)
{
  if (op.type == SA_DOUBLE || op.type == SA_FLOAT)
    return (double)*this - (double)op;
  switch(type){
    case SA_INT64:  return arg.v_int64 - (__int64)op;
    case SA_INT:    return arg.v_int - (int)op;
    case SA_SHORT:  return short(arg.v_short - (short)op);
    case SA_CHAR:   return char(arg.v_char - (char)op);
    case SA_UINT64: return arg.v_uint64 - (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint - (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort - (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte - (unsigned char)op);
    case SA_DOUBLE: return arg.v_dbl - (double)op;
    case SA_FLOAT:  return arg.v_flt - (float)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator%(SArg op)
{
  if (!(__int64)op) throw ERR_ZERO;
  return (__int64)*this % (__int64)op;
};
SArg SArg::operator/(SArg op)
{
  if ((double)op == 0) throw ERR_ZERO;
  return (double)*this / (double)op;
};
SArg SArg::operator*(SArg op)
{
  if (op.type == SA_DOUBLE || op.type == SA_FLOAT)
    return (double)*this * (double)op;
  switch(type){
    case SA_INT64:  return arg.v_int64 * (__int64)op;
    case SA_INT:    return arg.v_int * (int)op;
    case SA_SHORT:  return short(arg.v_short * (short)op);
    case SA_CHAR:   return char(arg.v_char * (char)op);
    case SA_UINT64: return arg.v_uint64 * (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint * (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort * (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte * (unsigned char)op);
    case SA_DOUBLE: return arg.v_dbl * (double)op;
    case SA_FLOAT:  return arg.v_flt * (float)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator~()
{
  switch(type){
    case SA_INT64:  return ~arg.v_int64;
    case SA_INT:    return ~arg.v_int;
    case SA_SHORT:  return (short)~arg.v_short;
    case SA_CHAR:   return (char)~arg.v_char;
    case SA_UINT64: return ~arg.v_uint64;
    case SA_UINT:   return ~arg.v_uint;
    case SA_USHORT: return (unsigned short)~arg.v_ushort;
    case SA_BYTE:   return (unsigned char)~arg.v_byte;
    case SA_DOUBLE: return ~(int)arg.v_dbl;
    case SA_FLOAT:  return ~(int)arg.v_flt;
  };
  throw ERR_TYPE;
};
SArg SArg::operator|(SArg op)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 | (__int64)op;
    case SA_INT:    return arg.v_int | (int)op;
    case SA_SHORT:  return short(arg.v_short | (short)op);
    case SA_CHAR:   return char(arg.v_char | (char)op);
    case SA_UINT64: return arg.v_uint64 | (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint | (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort | (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte | (unsigned char)op);
    case SA_DOUBLE:
    case SA_FLOAT:
      return (int)*this | (int)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator^(SArg op)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 ^ (__int64)op;
    case SA_INT:    return arg.v_int ^ (int)op;
    case SA_SHORT:  return short(arg.v_short ^ (short)op);
    case SA_CHAR:   return char(arg.v_char ^ (char)op);
    case SA_UINT64: return arg.v_uint64 ^ (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint ^ (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort ^ (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte ^ (unsigned char)op);
    case SA_DOUBLE:
    case SA_FLOAT:
      return (int)*this ^ (int)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator&(SArg op)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 & (__int64)op;
    case SA_INT:    return arg.v_int & (int)op;
    case SA_SHORT:  return short(arg.v_short & (short)op);
    case SA_CHAR:   return char(arg.v_char & (char)op);
    case SA_UINT64: return arg.v_uint64 & (unsigned __int64)op;
    case SA_UINT:   return arg.v_uint & (unsigned int)op;
    case SA_USHORT: return (unsigned short)(arg.v_ushort & (unsigned short)op);
    case SA_BYTE:   return (unsigned char)(arg.v_byte & (unsigned char)op);
    case SA_DOUBLE:
    case SA_FLOAT:
      return (int)*this & (int)op;
  };
  throw ERR_TYPE;
};
SArg SArg::operator>>(int n)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 >> n;
    case SA_INT:    return arg.v_int >> n;
    case SA_SHORT:  return short(arg.v_short >> n);
    case SA_CHAR:   return char(arg.v_char >> n);
    case SA_UINT64: return arg.v_uint64 >> n;
    case SA_UINT:   return arg.v_uint >> n;
    case SA_USHORT: return (unsigned short)(arg.v_ushort >> n);
    case SA_BYTE:   return (unsigned char)(arg.v_byte >> n);
    case SA_DOUBLE:
    case SA_FLOAT:
      return (int)*this >> n;
  };
  throw ERR_TYPE;
};
SArg SArg::operator<<(int n)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 << n;
    case SA_INT:    return arg.v_int << n;
    case SA_SHORT:  return short(arg.v_short << n);
    case SA_CHAR:   return char(arg.v_char << n);
    case SA_UINT64: return arg.v_uint64 << n;
    case SA_UINT:   return arg.v_uint << n;
    case SA_USHORT: return (unsigned short)(arg.v_ushort << n);
    case SA_BYTE:   return (unsigned char)(arg.v_byte << n);
    case SA_DOUBLE:
    case SA_FLOAT:
      return (int)*this << n;
  };
  throw ERR_TYPE;
};

bool SArg::operator==(SArg op)
{
  return arg.v_int64 == op.arg.v_int64;
};
bool SArg::operator!=(SArg op)
{
  return arg.v_int64 != op.arg.v_int64;
};
bool SArg::operator>(SArg op)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 > (__int64)op;
    case SA_UINT64: return arg.v_uint64 > (unsigned __int64)op;
    default:
      return (double)*this > (double)op;
  };
  throw ERR_TYPE;
};
bool SArg::operator<(SArg op)
{
  switch(type){
    case SA_INT64:  return arg.v_int64 < (__int64)op;
    case SA_UINT64: return arg.v_uint64 < (unsigned __int64)op;
    default:
      return (double)*this < (double)op;
  };
  throw ERR_TYPE;
};

/////////////////////////// type cast //////////////////////////////

SArg::operator __int64()
{
  switch(type){
    case SA_INT64:  return arg.v_int64;
    case SA_INT:    return arg.v_int;
    case SA_SHORT:  return arg.v_short;
    case SA_CHAR:   return arg.v_char;
    case SA_UINT64: return arg.v_uint64;
    case SA_UINT:   return arg.v_uint;
    case SA_USHORT: return arg.v_ushort;
    case SA_BYTE:   return arg.v_byte;
    case SA_DOUBLE: return (__int64)arg.v_dbl;
    case SA_FLOAT:  return (__int64)arg.v_flt;
    case SA_DUMP:   return arg.v_int64;
  };
  throw ERR_ALL;
};
SArg::operator int()
{
  return (int)(__int64)*this;
};
SArg::operator short()
{
  return (short)(__int64)*this;
};
SArg::operator char()
{
  return (char)(__int64)*this;
};
SArg::operator unsigned __int64()
{
  switch(type){
    case SA_INT64:  return (unsigned __int64)arg.v_int64;
    case SA_INT:    return (unsigned int)arg.v_int;
    case SA_SHORT:  return (unsigned int)arg.v_short;
    case SA_CHAR:   return (unsigned int)arg.v_char;
    case SA_UINT64: return arg.v_uint64;
    case SA_UINT:   return arg.v_uint;
    case SA_USHORT: return arg.v_ushort;
    case SA_BYTE:   return arg.v_byte;
    case SA_DOUBLE: return (unsigned __int64)arg.v_dbl;
    case SA_FLOAT:  return (unsigned __int64)arg.v_flt;
    case SA_DUMP:   return arg.v_int64;
  };
  throw ERR_ALL;
};
SArg::operator unsigned int()
{
  return (unsigned int)(unsigned __int64)*this;
};
SArg::operator unsigned short()
{
  return (unsigned short)(unsigned __int64)*this;
};
SArg::operator unsigned char()
{
  return (unsigned char)(unsigned __int64)*this;
};
SArg::operator double()
{
  switch(type){
    case SA_UINT64:
    case SA_INT64:  return (double)arg.v_int64;
    case SA_INT:    return arg.v_int;
    case SA_SHORT:  return arg.v_short;
    case SA_CHAR:   return arg.v_char;
    case SA_UINT:   return arg.v_uint;
    case SA_USHORT: return arg.v_ushort;
    case SA_BYTE:   return arg.v_byte;
    case SA_DOUBLE: return arg.v_dbl;
    case SA_FLOAT:  return arg.v_flt;
    case SA_DUMP:   return arg.v_dbl;
  };
  throw ERR_ALL;
};
SArg::operator float()
{
  return (float)(double)*this;
};
SArg::operator dumpdata()
{
  dumpdata d;
  d.dump = arg.v_dump;
  return d;
};
SArg::operator bool()
{
  return (int)*this != 0;
};

/////////////////////////// services //////////////////////////////


bool SArg::isempty()
{
  return empty;
};
int SArg::varsize(){
  switch(type){
    case SA_UINT64:
    case SA_INT64:
      return sizeof(__int64);
    case SA_INT:
    case SA_UINT:
      return sizeof(int);
    case SA_SHORT:
    case SA_USHORT:
      return sizeof(short);
    case SA_CHAR:
    case SA_BYTE:
      return sizeof(char);
    case SA_DOUBLE:
      return sizeof(double);
    case SA_FLOAT:
      return sizeof(float);
    case SA_DUMP:
      return 8;
  };
  return 4;
};
EArgType SArg::gettype()
{
  return type;
};

bool SArg::setvalue(const char *str, int s, int e)
{
int      tp;
__int64  num64;
unsigned __int64 r;
int      i, j, pt, exps, expe;
bool     exp = false, esign = true;
double   dbl_num, dbl_lev, flt;

enum{ TP_HEX, TP_OCT, TP_BIN, TP_DEC, TP_DBL, TP_CHAR };

  type = SA_DOUBLE;
  empty = false;
  if (e == s){
    arg.v_dbl = 0;
    empty = true;
    return true;
  };
  tp = TP_DEC;
  str+=s;
  e -=s;
  s = 0;
  do{
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
      s += 2;
      tp = TP_HEX;
      break;
    };
    if (str[e-1] == 'h' || str[e-1] == 'H'){
      e--;
      tp = TP_HEX;
      break;
    };
    if (str[0] == '$' || str[0] == '#'){
      s++;
      tp = TP_HEX;
      break;
    };
    if (str[e-1] == 'o' || str[e-1] == 'O'){
      e--;
      tp = TP_OCT;
      break;
    };
    if (str[e-1] == 'b' || str[e-1] == 'B'){
      e--;
      tp = TP_BIN;
      break;
    };
    if (str[0] == '0'){
      tp = TP_OCT;
      break;
    };
    if (str[e-1] == 'd' || str[e-1] == 'D'){
      e--;
      tp = TP_DEC;
      break;
    };
    if (str[0] == '\'' && str[e-1] == '\''){
      e--;
      s++;
      tp = TP_CHAR;
      break;
    };
    break;
  }while(1);
  if (tp == TP_DEC || tp == TP_OCT)
    for(i = s;i < e;i++){
      if (str[i] == '.') tp = TP_DBL;
      if (str[i] == 'e' || str[i] =='E') tp = TP_DBL;
    };

  switch(tp){
    case TP_HEX:
      num64 = 0;
      i = e-1;
      r = 0;
      while(i >= s){
        j = str[i];
        if (((j < 0x30) || (j > 0x39)) && (((j | 0x20) < 'a') || ((j | 0x20) > 'f')))
          throw ERR_EXPR;
        if (r >= 16)  throw ERR_FLOW;
        if (j > 0x40) j -=7;
        num64 = num64 + (__int64(j & 0xF) << r*4);
        r++;
        i--;
      };
      if (num64 >> 32){
        arg.v_int64 = num64;
        type = (num64&(1i64 << 63))?SA_UINT64:SA_INT64;
        break;
      };
      arg.v_int = (int)num64;
      type = (num64&(1i64 << 31))?SA_UINT:SA_INT;
      break;
    case TP_OCT:
      num64 = 0;
      i = e-1;
      r = 0;
      while(i >= s){
        j = str[i];
        if ((j < 0x30) || (j > 0x37)) throw ERR_EXPR;
        if (r >= 23)  throw ERR_FLOW;
        num64 = num64 + (__int64(j & 0x7) << r*3);
        r++;
        i--;
      };
      if (num64 >> 32){
        arg.v_int64 = num64;
        type = (num64&(1i64 << 63))?SA_UINT64:SA_INT64;
        break;
      };
      arg.v_int = (int)num64;
      type = (num64&(1i64 << 31))?SA_UINT:SA_INT;
      break;
    case TP_BIN:
      num64 = 0;
      i = e-1;
      r = 0;
      while(i >= s){
        j = str[i];
        if ((j < 0x30) || (j > 0x31)) throw ERR_EXPR;
        if (r >= 64)  throw ERR_FLOW;
        num64 = num64 + (__int64(j & 0x1) << r);
        r++;
        i--;
      };
      if (num64 >> 32){
        arg.v_int64 = num64;
        type = (num64&(1i64 << 63))?SA_UINT64:SA_INT64;
        break;
      };
      arg.v_int = (int)num64;
      type = (num64&(1i64 << 31))?SA_UINT:SA_INT;
      break;
    case TP_DEC:  // decimal
      dbl_num = 0;
      i = e-1;
      dbl_lev = 1;
      while(i >= s){
        j = str[i];
        if ((j < 0x30) || (j > 0x39)) throw ERR_EXPR;
//        if (e-i >= 21) throw ERR_FLOW;
        dbl_num = dbl_num + dbl_lev*(j & 0xF);
        dbl_lev *=10;
        i--;
      };
      /*
      if (num64 >> 32){
        arg.v_int64 = num64;
        type = (num64&(1i64 << 63))?SA_UINT64:SA_INT64;
        break;
      };
      */
      arg.v_dbl = dbl_num;
//      type = (num64&(1i64 << 31))?SA_UINT:SA_INT;
      type = SA_DOUBLE;
      break;
    case TP_DBL:  // double
      // exponent
      for(i = s; i < e; i++)
        if (str[i] == 'e' || str[i] =='E'){
          exp = true;
          exps = i+1;
          if (str[i+1] == '+' || str[i+1] == '-'){
            exps++;
            if (str[i+1] == '-') esign = false;
          };
          expe = e;
          e = i;
        };
      // float point
      pt = e;
      for(i = s; i < e; i++)
        if (str[i] == '.'){
          pt = i;
          break;
        };
      // before point
      dbl_num = 0;
      dbl_lev = 1;
      i = pt - 1;
      while(i >= s){
        j = str[i];
        if((j < 0x30)||(j > 0x39)) throw ERR_EXPR;
        dbl_num += double(j & 15)*dbl_lev;
        dbl_lev *= 10;
        i--;
      };
      // after point
      dbl_lev = 0.1;
      i = pt+1;
      while(i < e){
        j = str[i];
        if (j < 0x30 || j > 0x39) throw ERR_EXPR;
        dbl_num += double(j & 15)*dbl_lev;
        dbl_lev /= 10;
        i++;
      };
      // exponent
      dbl_lev = 1;
      if (exp){
        flt = 0;
        i = expe-1;
        while(i >= exps){
          j = str[i];
          if(j < 0x30 || j > 0x39) throw ERR_EXPR;
          flt += double(j & 15)*dbl_lev;
          dbl_lev *= 10;
          i--;
        };
        if (esign) dbl_num = dbl_num*pow(10,flt);
        if (!esign) dbl_num = dbl_num/pow(10,flt);
      };
      arg.v_dbl = dbl_num;
      break;
    case TP_CHAR:  // char
      pt = 0;
      i = s;
      while(e-i){
        pt += str[i]<<(e-i-1)*8;
        i++;
      };
      switch(e-s){
        case 0:
        case 1:
          type = SA_BYTE;
          arg.v_byte=pt;
          break;
        case 2:
          type = SA_USHORT;
          arg.v_ushort=pt;
          break;
        case 3:
        case 4:
          type = SA_UINT;
          arg.v_uint=pt;
          break;
        default:
          return false;
      };
      break;
  };
  return true;
};
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
