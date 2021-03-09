#include<sgml/sgml.h>
#include<exparse/sarg.h>
#include<float.h>

enum EERROR { ERR_OK = 0, ERR_ALL, ERR_EXPR, ERR_TYPE,
              ERR_PARAM, ERR_BRACK, ERR_ZERO, ERR_FLOW };

enum ETOSTR { EC_DEC, EC_DECPT, EC_DECEXP, EC_HEX, EC_OCT, EC_BIN };

typedef class  CParseNum *PParseNum;
typedef struct SBlock *PBlock;
typedef struct SSyntax *PSyntax;
typedef struct SVars *PVars;
typedef struct SArg *PArg;

extern int convertToString(SArg res, ETOSTR type, char* str, int len);

class CParseNum
{
  PSyntax Consts;
  PSyntax Ops;
  PSyntax Functs;
  PVars   Vars;
  bool FillSet(PSgmlEl set);
  bool AddLexem(PSyntax &syntax, char *name, char *val);
//  bool SetVar(char *name, SArg val);

  EERROR ParseError;
  EERROR Error;
  PBlock Base;
  PBlock FillTree(PBlock &base,const char *str,int s,int e);
  PBlock GetParam(PBlock par, int no);
  void SubstFuncts(PBlock newblock, PBlock par);
  void SubstVars(PBlock newblock, PBlock left, PBlock right);
  SArg MakeOp(int no, SArg left, SArg right);
  SArg MakeF(int no, PBlock arg);
  bool CountIt(PBlock base);
  SArg lowParse(PBlock base);
  int  delspaces(char *str);

public:
  CParseNum();
  ~CParseNum();
  bool LoadDefSymbols();
  bool LoadSymbols(char *data, int len);
  bool SetVar(char *name, SArg value);

  EERROR Compile(const char* str);
  SArg Parse();
  SArg Parse(const char* str);
  EERROR GetError();
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
