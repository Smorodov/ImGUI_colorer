#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<math.h>

#include<exparse/exparse.h>
#include<exparse/blockdef.cpp>
#include<exparse/sarg.cpp>
#include<exparse/syntax.cpp>
#include<exparse/tools.cpp>

#define _pi 3.14159265358979
#define EPSILON 1e-300

enum{
// Operators
  op_comma,
  op_lor,   op_land,  op_or,  op_xor, op_and, op_neq,  op_eq,
  op_gt,    op_lt,    op_shr, op_shl, op_ror, op_rol,
  op_minus, op_plus,  op_mod, op_div, op_mul, op_pow,
  op_not,   op_lnot,  op_factor,
  op_num,
};
char _symop[op_num][0x10] = {
  ",", "_lor",  "_land",  "_or",  "_xor", "_and", "_neq",  "_eq",
  "_gt",   "_lt",    "_shr", "_shl", "_ror", "_rol",
  "_minus","_plus",  "_mod", "_div", "_mul", "_pow",
  "_not",  "_lnot",  "_factor"
};

enum{
// functions
  f_frac,  f_floor, f_ceil,  f_sin,  f_tan, f_arctan, f_ln, f_rnd,
  f_iif,   f_sum,   f_avr,   f_integral, f_derived,
// types
  f_string, f_int64, f_uint64, f_int,   f_short, f_char,
  f_uint, f_ushort, f_byte, f_double, f_float, f_dump,
  f_num,
};
char _symf[f_num][0x10] = {
  "_frac", "_floor", "_ceil",
  "_sin", "_tan", "_arctan", "_ln", "_rnd",
  "_iif", "Sum", "Avr", "Integral", "Derived",
  "_string", "_int64", "_uint64", "_int", "_short", "_char",
  "_uint", "_ushort", "_byte", "_double", "_float", "_dump"
};



CParseNum::CParseNum()
{
  Consts = 0;
  Vars   = 0;
  Ops    = 0;
  Functs = 0;

  Base = 0;
  srand((unsigned)time(0));
};
CParseNum::~CParseNum()
{
  if (Base)   delete Base;
  if (Vars)   delete Vars;
  if (Consts) delete Consts;
  if (Ops)    delete Ops;
  if (Functs) delete Functs;
};

PBlock CParseNum::FillTree(PBlock &base,const char *str,int s,int e)
{
PSyntax op,lop;
PBlock newblock;
int i, j, l, brnum;
int curop=op_num-1, oppos=0, opsize=0;
int ecurop=0x1000, eoppos=0, eopsize=0;

  for(i = s; i < e; i++){
    if (str[i] == ')') throw ERR_BRACK;
    if (str[i] == '('){
      brnum = 0;
      while(i < e){
        if (str[i] == '(') brnum++;
        if (str[i] == ')') brnum--;
        if (!brnum) break;
        i++;
      };
      if (brnum) throw ERR_BRACK;
      continue;
    };
    op = Ops;
    j = 0;
    while(op && j <= ecurop){
      l = strlen(op->name);
      if(l <= e-s && !strnicmp(op->name, str+i, l)){
        ecurop = j;
        eoppos = i;
        eopsize = l;
        lop = op;
        i += l-1;  // Bad Thing?
      };
      j++;
      op = op->next;
    };
    for(j = 0; j <= curop; j++){
      l = strlen(_symop[j]);
      if (l <= e-s && !strnicmp(_symop[j], str+i, l)){
        curop = j;
        oppos = i;
        opsize = l;
      };
    };
  };

  // operator
  if (opsize){
    base->type = BL_OP;
    base->mean.OpNo = curop;
    base->counted = false;
    base->left  = new SBlock;
    base->right = new SBlock;
    FillTree(base->left, str, s, oppos);
    FillTree(base->right, str, oppos+opsize, e);
    CountIt(base);
    return base;
  };
  // extended operator
  if (eopsize){
    base->left  = new SBlock;
    base->right = new SBlock;
    FillTree(base->left, str, s, eoppos);
    FillTree(base->right, str, eoppos+eopsize, e);

    newblock = new SBlock;
    FillTree(newblock, lop->mean, 0, strlen(lop->mean));
    SubstVars(newblock, base->left, base->right);
    base->delref();  // with counter in destructor
    base = newblock;
    CountIt(base);
    return base;
  };
  // ( )
  if (str[s] == '(' && str[e-1] == ')')
    return FillTree(base, str, s+1, e-1);

  // function
  for(j = 0; j < f_num; j++){
    l = strlen(_symf[j]);
    if (l <= e-s && !strnicmp(_symf[j], str+s, l) && str[s+l]=='('){
      base->type = BL_FUNCT;
      base->mean.FunctNo = j;
      base->counted = false;
      base->left  = new SBlock;
      FillTree(base->left, str, s+l, e);
      CountIt(base);
      return base;
    };
  };
  // ext function
  op = Functs;
  while(op){
    l = strlen(op->name);
    if(l <= e-s && !strnicmp(op->name, str+s, l) && str[s+l]=='('){
      base->left  = new SBlock;
      FillTree(base->left, str, s+l, e);

      newblock = new SBlock;
      FillTree(newblock, op->mean, 0, strlen(op->mean));
      SubstFuncts(newblock, base->left);
      base->delref();
      base = newblock;
      CountIt(base);
      return base;
    };
    op = op->next;
  };
  // const
  if (e-s == 3 && !strnicmp("op",str+s,2) &&
      str[s+2] >= 0x30 && str[s+2] <= 0x39){
    base->type = EOpType(BL_VAR + 1 + str[s+2] - 0x30);
    base->counted = false;
    return base;
  };
  // user const
  op = Consts;
  while(op){
    l = strlen(op->name);
    if(l == e-s && !strnicmp(op->name, str+s, l)){
      newblock = new SBlock;
      FillTree(newblock,op->mean,0,strlen(op->mean));
      base->ArgValue = lowParse(newblock);
      delete newblock;
      base->type = BL_ARG;
      base->counted = true;
      return base;
    };
    op = op->next;
  };
  // variables
  op = Vars;
  while(op){
    l = strlen(op->name);
    if(l == e-s && !strnicmp(op->name, str+s, l)){
      base->type = BL_VAR;
      base->mean.VarValue = &PVars(op)->value;
      base->counted = false;
      return base;
    };
    op = op->next;
  };
  // simple numbers
  if (base->ArgValue.setvalue(str, s, e)){
    base->type = BL_ARG;
    base->counted = true;
    return base;
  };
  // fault
  throw ERR_EXPR;
  return 0;
};

PBlock CParseNum::GetParam(PBlock par, int no)
{
  if ((par->type != BL_OP || par->mean.OpNo != op_comma) && !no)
    return par;
  while(no-- && par)
    par = par->left;
  if (par->type == BL_OP && par->mean.OpNo == op_comma)
    return par->right;
  return par;
};

void CParseNum::SubstFuncts(PBlock newblock, PBlock par)
{
int pos;
  if (newblock->left)
    if (newblock->left->type > BL_VAR){
      pos = newblock->left->type - BL_VAR - 1;
      newblock->left->delref();
      newblock->left = GetParam(par,pos);
      newblock->left->addref();
    }else SubstFuncts(newblock->left, par);
  if (newblock->right)
    if (newblock->right->type > BL_VAR){
      pos = newblock->right->type - BL_VAR - 1;
      newblock->right->delref();
      newblock->right = GetParam(par,pos);
      newblock->right->addref();
    }else SubstFuncts(newblock->right, par);
};

void CParseNum::SubstVars(PBlock newblock, PBlock left, PBlock right)
{
  if (newblock->left)
    switch(newblock->left->type){
      case BL_VAR+1:
        newblock->left->delref();
        newblock->left = left;
        left->addref();
        break;
      case BL_VAR+2:
        newblock->left->delref();
        newblock->left = right;
        right->addref();
        break;
      default:
        SubstVars(newblock->left, left, right);
        CountIt(newblock->left);
        break;
    };
  if (newblock->right)
    switch(newblock->right->type){
      case BL_VAR+1:
        newblock->right->delref();
        newblock->right = left;
        left->addref();
        break;
      case BL_VAR+2:
        newblock->right->delref();
        newblock->right = right;
        right->addref();
        break;
      default:
        SubstVars(newblock->right, left, right);
        CountIt(newblock->right);
        break;
    };
};

SArg CParseNum::MakeOp(int no, SArg left, SArg right)
{
double num;
int fi;

  switch(no){
    case op_comma:
      return right;
    case op_lor:
      return (bool)left || (bool)right;
    case op_land:
      return (bool)left && (bool)right;
    case op_or:
      return left | right;
    case op_xor:
      return left ^ right;
    case op_and:
      return left & right;
    case op_neq:
      return left != right;
    case op_eq:
      return left == right;
    case op_gt:
      return left > right;
    case op_lt:
      return left < right;
    case op_shr:
      return left >> right;
    case op_shl:
      return left << right;
    case op_ror:
      // fuck!
      return  (left>>int(right)%(left.varsize()*8)) + (left<<(left.varsize()*8-int(right)%(left.varsize()*8)));
    case op_rol:
      // shit happens...
      return  (left<<int(right)%(left.varsize()*8)) + (left>>(left.varsize()*8-int(right)%(left.varsize()*8))%(left.varsize()*8));
    case op_minus:
      return left-right;
    case op_plus:
      return left+right;
    case op_mod:
      return left%right;
    case op_div:
      return left/right;
    case op_mul:
      return left*right;
    case op_pow:
      num = pow(left,right);
      return num;
    case op_not:
      if (!left.isempty())
        throw ERR_PARAM;
      return ~right;
    case op_lnot:
      if (!left.isempty())
        throw ERR_PARAM;
      return !(bool)right;
    case op_factor:
      if (!right.isempty() || (int)left < 0)
        throw ERR_PARAM;
      if ((int)left > 170)
        throw ERR_FLOW;
      num = 1;
      for(fi = 1;fi <= (int)left;fi++) num *= fi;
      return num;
  };
  throw ERR_ALL;
  return 0;
};

SArg CParseNum::MakeF(int no, PBlock arg)
{
int i;
double par1;
PBlock blpar;
SArg fp1, fp2, fp3;
SArg param = lowParse(arg);

  switch(no){
    case f_frac:
      return fabs(modf(param,&par1));
    case f_floor:
      return floor(param);
    case f_ceil:
      return ceil(param);
    case f_sin:
      return sin(param);
    case f_tan:
      if(fabs(fmod((double)param+_pi/2,_pi)) < EPSILON) throw ERR_PARAM;
      return tan(param);
    case f_arctan:
      return atan(param);
    case f_ln:
      if((double)param < EPSILON) throw ERR_PARAM;
      return log(param);
    case f_rnd:
      return double(rand())/RAND_MAX;
    case f_int64:
      return (__int64)param;
    case f_uint64:
      return (unsigned __int64)param;
    case f_int:
      return (int)param;
    case f_short:
      return (short)param;
    case f_char:
      return (char)param;
    case f_uint:
      return (unsigned int)param;
    case f_ushort:
      return (unsigned short)param;
    case f_byte:
      return (unsigned char)param;
    case f_double:
      return (double)param;
    case f_float:
      return (float)param;
    case f_dump:
      return (dumpdata)param;

    case f_iif:
      if (!arg->left || !arg->left->left)
        throw ERR_PARAM;
      fp1 = lowParse(arg);
      fp2 = lowParse(arg->left);
      fp3 = lowParse(arg->left->left);
      return fp3?fp2:fp1;
    case f_sum:
      blpar = arg;
      fp1 = 0;
      while(blpar){
        fp1 = fp1 + lowParse(blpar);
        blpar = blpar->left;
      };
      return fp1;
    case f_avr:
      blpar = arg;
      fp1 = 0;
      i = 0;
      while(blpar){
        fp1 = fp1 + lowParse(blpar);
        blpar = blpar->left;
        i++;
      };
      return (double)fp1/i;
/*
    case f_integral:
      return;
    case f_derived:
      return;
*/
  };
  throw ERR_ALL;
  return 0;
};

bool CParseNum::CountIt(PBlock base)
{
  if (base->left && !CountIt(base->left)) return false;
  if (base->right && !CountIt(base->right)) return false;
  if (base->type >= BL_VAR || (base->type == BL_OP && !base->mean.OpNo))
    return false;

  base->ArgValue = lowParse(base);
  base->type = BL_ARG;
  if (base->left)  base->left->delref();
  if (base->right) base->right->delref();
  base->left  = 0;
  base->right = 0;
  base->counted = true;
  return true;
};
SArg CParseNum::lowParse(PBlock base)
{
  switch(base->type){
    case BL_OP:
      return MakeOp(base->mean.OpNo, lowParse(base->left), lowParse(base->right));
    case BL_FUNCT:
      return MakeF(base->mean.FunctNo, base->left);
    case BL_ARG:
      return base->ArgValue;
    case BL_VAR:
      return *base->mean.VarValue;
  };
  return 0;
};
int CParseNum::delspaces(char *str)
{
int i, len = strlen(str);
  for(i = 0;i < len;i++)
    if (str[i] == ' '){
      strcpy(&str[i],&str[i+1]);
      len--;
      i--;
    };
  return len;
};

EERROR CParseNum::GetError()
{
  if (ParseError) return ParseError;
  return Error;
};

EERROR CParseNum::Compile(const char* str)
{
int len;
char *newstr=0;

  len = strlen(str);
  newstr = new char[len+1];
  strcpy(newstr,str);
  len = delspaces(newstr);
  if (Base) delete Base;
  Base = new SBlock;
  ParseError = ERR_OK;
  Error = ERR_OK;

  try{
    FillTree(Base,newstr,0,len);
  }catch(EERROR exc){
    ParseError = exc;
  }catch(...){
    ParseError = ERR_ALL;
  };
  delete newstr;
  return ParseError;
};

SArg CParseNum::Parse()
{
  if (ParseError) return 0;
  Error = ERR_OK;
  try{
    return lowParse(Base);
  }catch(EERROR exc){
    Error = exc;
  }catch(...){
    Error = ERR_ALL;
  };
  return 0;
};
SArg CParseNum::Parse(const char* str)
{
  Compile(str);
  return Parse();
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
