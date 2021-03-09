
// EC_DEC, EC_DECPT, EC_DECEXP, EC_HEX, EC_OCT, EC_BIN
int convertToString(SArg val, ETOSTR type, char* str, int len)
{
int     i, j;
char    c;
unsigned __int64 num64;

//__asm { finit };
try{

  switch(type){
    case EC_DEC:
      return sprintf(str,"%.0f",(double)val);
    case EC_DECPT:
      i = sprintf(str,"%f",(double)val);
      for (j = i-1; j && str[j]=='0' && str[j]!='.';j--){
        str[j] = 0;
        i--;
      };
      if (str[j] == '.'){
        str[j] = 0;
        i--;
      };
      return i;
    case EC_DECEXP:
      return sprintf(str,"%17.19E",(double)val);
    case EC_HEX:
      num64 = (unsigned __int64)val;
      i = 60;
      j = 0;
      while(!(num64&(0xFi64<<i))){
        if (!i){
          str[0] = 0x30;
          str[1] = 0;
          return 1;
        };
        i-=4;
      };
      while(true){
        c = char( (num64&(0xFi64<<i)) >> i);
        str[j] = c > 9? c + 0x37 : (c + 0x30);
        j++;
        if (!i) break;
        i -= 4;
      };
      str[j] = 0;
      return j;
    case EC_BIN:
      num64 = (unsigned __int64)val;
      i = 63;
      j = 0;
      while(!(num64&(0x1i64<<i))){
        if (!i){
          str[0] = 0x30;
          str[1] = 0;
          return 1;
        };
        i--;
      };
      while(true){
        c = char( (num64&(0x1i64<<i)) >> i);
        str[j] = c ? 0x31 : 0x30;
        j++;
        if (!i) break;
        i--;
      };
      str[j] = 0;
      return j;
    case EC_OCT:
      num64 = (unsigned __int64)val;
      j = 0;
      i = 63;
      while(!(num64&(0x7i64<<i))){
        if (!i){
          str[0] = 0x30;
          str[1] = 0;
          return 1;
        };
        i-=3;
      };
      while(true){
        c = char( (num64&(0x7i64<<i)) >> i);
        if (c > 7) c = c%7;
        str[j] = c + 0x30;
        j++;
        if (!i) break;
        i -= 3;
      };
      str[j] = 0;
      return j;
  };
}catch(...){
  str[0] = 0;
  _fpreset();
  //
};
  return 0;
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
