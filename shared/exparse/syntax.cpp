char calcset[] =
"<calc version=\"3.0\">"
"   <use set=\"DefaultConsts\"/>"
"   <use set=\"CStyleOperators\"/>"
"   <use set=\"DefaultFunct\"/>"

"   <set name=\"DefaultConsts\">"
"     <const syntax=\"pi\" mean=\"3.1415926535897932385\"/>"
"     <const syntax=\"exp\" mean=\"2.71828182845904524\"/>"
"     <const syntax=\"light\" mean=\"2.998e8\"/>"
"     <const syntax=\"grav\" mean=\"9.807\"/>"
"   </set>"

"   <set name=\"CStyleOperators\">"
"     <op syntax=\"||\"   mean=\"op0 _lor op1\"/>"
"     <op syntax=\"&amp;&amp;\"   mean=\"op0 _land op1\"/>"
"     <op syntax=\"|\"    mean=\"op0 _or op1\"/>"
"     <op syntax=\"&amp;\"    mean=\"op0 _and op1\"/>"
"     <op syntax=\"^\"    mean=\"op0 _xor op1\"/>"
"     <op syntax=\">>\"   mean=\"op0 _shr op1\"/>"
"     <op syntax=\"&lt;&lt;\"   mean=\"op0 _shl op1\"/>"
"     <op syntax=\"ror\"  mean=\"op0 _ror op1\"/>"
"     <op syntax=\"rol\"  mean=\"op0 _rol op1\"/>"
"     <op syntax=\">=\"   mean=\"(op0 _gt op1) _lor (op0 _eq op1)\"/>"
"     <op syntax=\"&lt;=\"   mean=\"(op0 _lt op1) _lor (op0 _eq op1)\"/>"
"     <op syntax=\">\"    mean=\"op0 _gt op1\"/>"
"     <op syntax=\"&lt;\"    mean=\"op0 _lt op1\"/>"
"     <op syntax=\"==\"   mean=\"op0 _eq op1\"/>"
"     <op syntax=\"!=\"   mean=\"op0 _neq op1\"/>"
"     <op syntax=\"+\"    mean=\"op0 _plus op1\"/>"
"     <op syntax=\"-\"    mean=\"op0 _minus op1\"/>"
"     <op syntax=\"*\"    mean=\"op0 _mul op1\"/>"
"     <op syntax=\"/\"    mean=\"op0 _div op1\"/>"
"     <op syntax=\"\\\"    mean=\"_floor(op0 _div op1)\"/>"
"     <op syntax=\"%\"    mean=\"op0 _mod op1\"/>"
"     <op syntax=\"pow\"  mean=\"op0 _pow op1\"/>"
"     <op syntax=\"!\"    mean=\"_lnot op1\"/>"
"     <op syntax=\"~\"    mean=\"_not op1\"/>"
"     <op syntax=\"fct\"  mean=\"op0 _factor\"/>"
"     <op syntax=\"Kb\"   mean=\"op0*1024\"/>"
"     <op syntax=\"Mb\"   mean=\"op0*1024*1024\"/>"
"     <op syntax=\"Gb\"   mean=\"_int64(op0)*1024*1024*1024\"/>"
"     <op syntax=\"Tb\"   mean=\"_int64(op0)*1024*1024*1024*1024\"/>"
"     <op syntax=\"e+\"   mean=\"op0 * (10 _pow _int(op1))\"/>"
"     <op syntax=\"e-\"   mean=\"op0 * (10 _pow _int(-op1))\"/>"
"   </set>"

"   <set name=\"DefaultFunct\">"
"     <func syntax=\"frac\"   mean=\"_frac(op0)\"/>"
"     <func syntax=\"floor\"  mean=\"_floor(op0)\"/>"
"     <func syntax=\"ceil\"   mean=\"_ceil(op0)\"/>"
"     <func syntax=\"round\"  mean=\"_iif( frac(op0) >= 0.5, _iif(op0>0, ceil(op0), floor(op0)), _iif(op0>0, floor(op0), ceil(op0)) )\"/>"
"     <func syntax=\"sign\"   mean=\"_iif( op0 > 0, 1, _iif(op0 == 0, 0, -1) )\"/>"
"     <func syntax=\"sqrt\"   mean=\"op0 _pow 0.5\"/>"
"     <func syntax=\"sqr\"    mean=\"op0 * op0\"/>"
"     <func syntax=\"abs\"    mean=\"sqrt(op0 * op0)\"/>"
"     <func syntax=\"ln\"     mean=\"_ln(op0)\"/>"
"     <func syntax=\"lg\"     mean=\"_ln(op0) / _ln(10)\"/>"
"     <func syntax=\"log\"    mean=\"_ln(op0) / _ln(op1)\"/>"
"     <func syntax=\"exp\"    mean=\"2.71828182845904524 _pow op0\"/>"
"     <func syntax=\"rnd\"    mean=\"_rnd()\"/>"

"     <func syntax=\"sin\"    mean=\"_sin(op0)\"/>"
"     <func syntax=\"cos\"    mean=\"_sin(1.5707963267948966 - op0)\"/>"
"     <func syntax=\"tg\"     mean=\"_tan(op0)\"/>"
"     <func syntax=\"ctg\"    mean=\"1 / _tan(op0)\"/>"
"     <func syntax=\"sec\"    mean=\"1/cos(op0)\"/>"
"     <func syntax=\"cosec\"  mean=\"1/sin(op0)\"/>"
"     <func syntax=\"arctg\"  mean=\"_arctan(op0)\"/>"
"     <func syntax=\"arcctg\" mean=\"1.5707963267948966 - _arctan(op0)\"/>"
"     <func syntax=\"arcsin\" mean=\"_arctan(op0 / sqrt((1-op0*op0)) )\"/>"
"     <func syntax=\"arccos\" mean=\"_iif(op0 &lt; 0,  3.1415926535897932385 - abs(_arctan(sqrt(1-op0*op0)/op0)), abs(_arctan(sqrt(1-op0*op0)/op0)) )\"/>"
"     <func syntax=\"sh\"     mean=\"(2.71828182845904524 _pow op0  _minus  2.71828182845904524  _pow (-op0))/2\"/>"
"     <func syntax=\"ch\"     mean=\"(2.71828182845904524 _pow op0  _plus   2.71828182845904524  _pow (-op0))/2\"/>"
"     <func syntax=\"th\"     mean=\"sh(op0) / ch(op0)\"/>"
"     <func syntax=\"cth\"    mean=\"ch(op0) / sh(op0)\"/>"
"     <func syntax=\"arsh\"   mean=\"_ln(op0 + sqrt(1 _plus op0 _pow 2))\"/>"
"     <func syntax=\"arch\"   mean=\"_ln(op0 + sqrt(op0 _pow 2 _minus 1))\"/>"
"     <func syntax=\"arth\"   mean=\"0.5 * _ln((1+op0)/(1-op0))\"/>"
"     <func syntax=\"arcth\"  mean=\"0.5 * _ln((1+op0)/(op0-1))\"/>"
"     <func syntax=\"iif\"    mean=\"_iif(op2, op1, op0)\"/>"
"     <func syntax=\"deg\"    mean=\"(op0 * 180) / 3.1415926535897932385\"/>"
"     <func syntax=\"rad\"    mean=\"(op0*3.1415926535897932385) / 180\"/>"
"     <func syntax=\"cels\"   mean=\"(op0 - 32)*5/9\"/>"
"     <func syntax=\"fahr\"   mean=\"32 + op0*9/5\"/>"
"     <func syntax=\"kelv\"   mean=\"275 + op0\"/>"
"     <func syntax=\"gsin\"   mean=\"sin(rad(op0))\"/>"
"     <func syntax=\"gcos\"   mean=\"cos(rad(op0))\"/>"
"     <func syntax=\"CNK\"   mean=\"(op1 _factor ) _div ((op0 _factor) _mul ((op1 _minus op0)_factor))\"/>"
"   </set>"
"   </calc>";




struct SSyntax{
  char *name;
  char *mean;
  SSyntax *next;
  SSyntax();
  ~SSyntax();
};
struct SVars:SSyntax
{
  SArg value;
  SVars();
  ~SVars();
};
SSyntax::SSyntax()
{
  next = 0;
  name = mean = 0;
};
SSyntax::~SSyntax()
{
  if (next) delete next;
  if (name) delete[] name;
  if (mean) delete[] mean;
};
SVars::SVars()
{ };
SVars::~SVars()
{ };

bool CParseNum::LoadDefSymbols()
{
  return LoadSymbols(calcset, strlen(calcset));
};

bool CParseNum::LoadSymbols(char *data, int len)
{
char *par1;
PSgmlEl BaseRc,Base,El,Set;

  if (Consts)   delete Consts;
  if (Ops)    delete Ops;
  if (Functs) delete Functs;
  Consts = Ops = Functs = 0;

  BaseRc = new CSgmlEl;
  BaseRc->parse(data, len);

  Base = BaseRc;
  while(Base = Base->next())
    if (!Base || Base->getname() && !stricmp(Base->getname(),"Calc")) break;
  if (!Base){
    delete BaseRc;
    return false;
  };
  El = Base->child();
  while(El){
    if ( El->getname() && !stricmp(El->getname(),"Use")){
      par1 = El->GetChrParam("Set");
      Set = Base->child();
      while(Set){
        if (Set->getname() && !stricmp(Set->getname(),"Set") &&
        !stricmp(par1,Set->GetChrParam("Name"))){
          FillSet(Set);
          break;
        };
        Set = Set->next();
      };
    };
    El = El->next();
  };
  delete BaseRc;
  return true;
};

bool CParseNum::FillSet(PSgmlEl set)
{
PSgmlEl Ch = set->child();
  while(Ch){
    if (Ch->getname() && !stricmp(Ch->getname(), "Const"))
       AddLexem(Consts, Ch->GetChrParam("Syntax"), Ch->GetChrParam("Mean"));
    if (Ch->getname() && !stricmp(Ch->getname(), "Op"))
       AddLexem(Ops, Ch->GetChrParam("Syntax"), Ch->GetChrParam("Mean"));
    if (Ch->getname() && !stricmp(Ch->getname(), "Func"))
       AddLexem(Functs, Ch->GetChrParam("Syntax"), Ch->GetChrParam("Mean"));
    Ch = Ch->next();
  };
  return true;
};

bool CParseNum::AddLexem(PSyntax &syntax, char *name, char *val)
{
PSyntax snt,nxt;
SArg r;
  snt = syntax;
  while(snt){
    if (!snt->next) break;
    snt = snt->next;
  };
  nxt = new SSyntax;
  nxt->name = new char[strlen(name)+1];
  nxt->mean = new char[strlen(val)+1];
  strcpy(nxt->name,name);
  strcpy(nxt->mean,val);
  delspaces(nxt->mean);
  if (!syntax) syntax = nxt;
  else snt->next = nxt;
  return true;
};

bool CParseNum::SetVar(char *name, SArg value)
{
PVars snt, newv;
bool fnd = false;
  snt = Vars;
  while(snt){
    if (!stricmp(name,snt->name)){
      fnd = true;
      break;
    };
    if (!snt->next) break;
    snt = (PVars)snt->next;
  };
  if (!fnd){
    newv = new SVars();
    newv->name = new char[strlen(name)+1];
    newv->value = value;
    strcpy(newv->name,name);
    if (!Vars) Vars = newv;
    else snt->next = newv;
    return true;
  };
  snt->value = value;
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
