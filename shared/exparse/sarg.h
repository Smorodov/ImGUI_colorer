enum EArgType {
  SA_INT64, SA_UINT64, SA_INT, SA_SHORT, SA_CHAR,
  SA_UINT, SA_USHORT, SA_BYTE, SA_DOUBLE, SA_FLOAT, SA_DUMP
};

struct dumpdata{
  int size;
  unsigned char *dump;
};

struct SArg{
private:
  EArgType type;
  bool empty;
  int  size;
  union{
    __int64 v_int64;
    int     v_int;
    short   v_short;
    char    v_char;
    unsigned __int64 v_uint64;
    unsigned int   v_uint;
    unsigned short v_ushort;
    unsigned char  v_byte;
    double  v_dbl;
    float   v_flt;
    unsigned char v_dump[8];
  }arg;
public:
  SArg();
  ~SArg();
  SArg(SArg &a);

  SArg(__int64 var);
  SArg(bool var);
  SArg(short var);
  SArg(char var);
  SArg(int var);
  SArg(unsigned __int64 var);
  SArg(unsigned int var);
  SArg(unsigned short var);
  SArg(unsigned char var);
  SArg(double var);
  SArg(float var);
  SArg(dumpdata dd);

  bool setvalue(const char *str, int s, int e);
  bool isempty();
  int  varsize();
  EArgType gettype();

  SArg &operator=(SArg op);
  SArg &operator=(double op);

  SArg operator+(SArg op);
  SArg operator-(SArg op);
  SArg operator%(SArg op);
  SArg operator/(SArg op);
  SArg operator*(SArg op);
  SArg operator~();
  SArg operator|(SArg op);
  SArg operator^(SArg op);
  SArg operator&(SArg op);
  SArg operator>>(int n);
  SArg operator<<(int n);
  bool operator==(SArg op);
  bool operator!=(SArg op);
  bool operator>(SArg op);
  bool operator<(SArg op);

  operator __int64();
  operator int();
  operator short();
  operator char();
  operator unsigned __int64();
  operator unsigned int();
  operator unsigned short();
  operator unsigned char();
  operator double();
  operator float();
  operator dumpdata();
  operator bool();
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
