
BOOL SetClipData(void *Data, int Size)
{
HANDLE hGlob;
char *ptr;

  hGlob = GlobalAlloc(GHND|GMEM_DDESHARE, Size + 1);
  if (!hGlob) return FALSE;
  ptr = (char*)GlobalLock(hGlob);
  if (!ptr) return FALSE;
  MoveMemory(ptr,Data,Size+1);
  ptr[Size] = 0;
  GlobalUnlock(hGlob);
  if (OpenClipboard(NULL)){
    EmptyClipboard();
    SetClipboardData(CF_OEMTEXT,hGlob);
    CloseClipboard();
    return TRUE;
  };
  return FALSE;
};

BOOL GetClipData(void *Data, int Size)
{
HANDLE hGlob;
char *ptr;
  if (OpenClipboard(NULL)){
    hGlob = GetClipboardData(CF_OEMTEXT);
    ptr = (char*)GlobalLock(hGlob);
    if (!ptr) return FALSE;
    for(int i = 0;ptr[i] && i < Size;i++)
      ((char*)Data)[i] = ptr[i];
    ((char*)Data)[i] = 0;
    GlobalUnlock(hGlob);
    CloseClipboard();
    return TRUE;
  };
  return FALSE;
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
