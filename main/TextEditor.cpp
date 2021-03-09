#include <algorithm>
#include <chrono>
#include <string>
#define NOMINMAX
#include "TextEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h" // for imGui::GetCurrentWindow()
// ----------------------------------------------
//
// ----------------------------------------------
void insert_chars(String*& line,wchar* chars, size_t chars_len, size_t aPos)
{
    if (line == NULL)
    {        
        line = new DString("");
    }
    if (line != NULL)
    {
        const wchar* s = line->getWChars();
        size_t len = line->length() + chars_len;
        wchar* s_dst = new wchar[len + 1];

        memcpy(s_dst, s, sizeof(wchar) * aPos);
        memcpy(s_dst + aPos, chars, sizeof(wchar) * chars_len);
        memcpy(s_dst + aPos + chars_len, s + aPos, sizeof(wchar) * (line->length() - aPos));
        s_dst[len] = 0;
        String* str = new DString(s_dst);
        delete line;
        line = str;
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void overwrite_chars(String*& line, wchar* chars, size_t chars_len, size_t aPos)
{
    const wchar* s = line->getWChars();
    size_t len = (chars_len + aPos) > line->length() ? chars_len + aPos : line->length();
    wchar* s_dst = new wchar[len + 1];

    memcpy(s_dst, s, sizeof(wchar) * line->length());
    memcpy(s_dst + aPos, chars, sizeof(wchar) * chars_len);

    s_dst[len] = 0;
    String* str = new DString(s_dst);
    delete line;
    line = str;
}
// ----------------------------------------------
//
// ----------------------------------------------
void append_char(String*& line, wchar c)
{ 
    insert_chars(line, &c, 1, line->length());
}
// ----------------------------------------------
//
// ----------------------------------------------
void prepend_char(String*& line, wchar c)
{
    insert_chars(line, &c, 1, 0);
}
// ----------------------------------------------
//
// ----------------------------------------------
void erase_chars(String*& line, size_t aStart, size_t aEnd)
{
    if (line!=NULL)
    {
      //  if (!withLineEnd)
      //  {
      //      // last not erased
      //      if (aEnd > line->length() - 1)
      //      {
      //          aEnd = line->length() - 1;
      //      }
      //  }

        if (line->length() >= aEnd)
        {
            const wchar* s = line->getWChars();
            size_t len = line->length() - (aEnd - aStart);
            wchar* s_dst = new wchar[len+1];
            memcpy(s_dst, s, sizeof(wchar) * aStart);
            memcpy(s_dst + aStart, s + aEnd, sizeof(wchar) * (line->length() - aEnd));
            s_dst[len] = 0;
            String* str = new DString(s_dst);
            delete line;
            line = str;
        }
    }
}

// ----------------------------------------------
//
// ----------------------------------------------
template<class InputIt1, class InputIt2, class BinaryPredicate>
bool equals(InputIt1 first1, InputIt1 last1,
    InputIt2 first2, InputIt2 last2, BinaryPredicate p)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (!p(*first1, *first2))
            return false;
    }
    return first1 == last1 && first2 == last2;
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::TextEditor()
    : mLineSpacing(1.0f)
    , mUndoIndex(0)
    , mTabSize(4)
    , mOverwrite(false)
    , mReadOnly(false)
    , mWithinRender(false)
    , mScrollToCursor(false)
    , mScrollToTop(false)
    , mTextChanged(false)
    , mTextStart(20.0f)
    , mLeftMargin(10)
    , mColorRangeMin(0)
    , mColorRangeMax(0)
    , mSelectionMode(SelectionMode::Normal)
    , mLastClick(-1.0f)
{
    mLines.push_back(Line());
    textLinesStore.lines.addElement(new DString(""));

    // Source file text lines store.    
    pf_ptr=new ParserFactory(&SString("./data/catalog.xml"));
    // Base editor to make primary parse    
    baseEditor_ptr = new BaseEditor(pf_ptr, &textLinesStore);
    textLinesStore_ptr = &textLinesStore;
    // HRD RegionMapper linking    
    baseEditor_ptr->setRegionMapper(&DString("rgb"), &DString("black"));
    for (int i = 0; pf_ptr->getHRCParser()->enumerateFileTypes(i); ++i)
    {
        std::cout << pf_ptr->getHRCParser()->enumerateFileTypes(i)->getName()->getChars() << std::endl;
    }
    FileType* type = pf_ptr->getHRCParser()->getFileType(&SString("cpp"));
    baseEditor_ptr->setFileType(type);
    // Initial line count notify
    baseEditor_ptr->lineCountEvent(textLinesStore.getLineCount());

    for (int i = 0; i < colors.size(); ++i)
    {
        colors[i].clear();
    }
    colors.clear();
    for (int i = 0; i < styles.size(); ++i)
    {
        styles[i].clear();
    }
    styles.clear();

    const StyledRegion* rd = StyledRegion::cast(baseEditor_ptr->rd_def_Text);
    int topline, leftpos;
    leftpos = topline = 0;
    int n_lines = textLinesStore.getLineCount();
    int n_cols = 0;
    int lline = n_lines;
    if (topline + lline > textLinesStore.getLineCount()) lline = textLinesStore.getLineCount() - topline;
    baseEditor_ptr->visibleTextEvent(topline, lline);

    for (int i = topline; i < topline + n_lines; i++)
    {
        std::vector<WORD> colors_line;
        std::vector<WORD> styles_line;
        int Y = i - topline;
        int li;
        if (i >= textLinesStore_ptr->getLineCount()) continue;
        n_cols = textLinesStore_ptr->getLine(i)->length();
        colors_line.resize(n_cols);
        styles_line.resize(n_cols);
        for (LineRegion* l1 = baseEditor_ptr->getLineRegions(i); l1 != null; l1 = l1->next)
        {
            if (l1->special || l1->rdef == null) continue;
            int end = l1->end;
            if (end == -1) end = n_cols;
            int X = l1->start - leftpos;
            int len = end - l1->start;
            if (X < 0)
            {
                len += X;
                X = 0;
            };
            if (len < 0 || X >= n_cols) continue;
            if (len + X > n_cols) len = n_cols - X;
            WORD color = (WORD)(l1->styled()->fore);
            WORD style = (WORD)(l1->styled()->style);

            std::fill(colors_line.begin() + X, colors_line.begin() + X + len, color);
            std::fill(styles_line.begin() + X, styles_line.begin() + X + len, style);
        }
        colors.push_back(colors_line);
        styles.push_back(styles_line);
    }

}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::~TextEditor()
{
    for (int i = 0; i < colors.size(); ++i)
    {
        colors[i].clear();
    }
    colors.clear();
    for (int i = 0; i < styles.size(); ++i)
    {
        styles[i].clear();
    }
    styles.clear();

    delete baseEditor_ptr;
    delete pf_ptr;

    for (int i = 0; i < textLinesStore.lines.size(); ++i)
    {
        delete textLinesStore.lines.elementAt(i);
    }
    textLinesStore.lines.clear();    
}
// ----------------------------------------------
//
// ----------------------------------------------
int TextEditor::AppendBuffer(CText& aBuffer,CText::Char chr, int aIndex)
{
    if (chr != _T('\t'))
    {
        aBuffer.push_back(chr);
        return aIndex + 1;
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetText(const Coordinates& aStart, const Coordinates& aEnd) const
{
    CText result;

    int prevLineNo = aStart.mLine;
    for (auto it = aStart; it <= aEnd; Advance(it))
    {
        if (prevLineNo != it.mLine && it.mLine < (int)mLines.size())
        {
            result.push_back(_T('\n'));
        }

        if (it == aEnd)
        {
            break;
        }

        prevLineNo = it.mLine;
        const auto& line = mLines[it.mLine];
        if (!line.empty() && it.mColumn < (int)line.size())
        {
            result.push_back(line[it.mColumn].mChar);
        }
    }
    return result;
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Coordinates TextEditor::GetActualCursorCoordinates() const
{
    return SanitizeCoordinates(mState.mCursorPosition);
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Coordinates TextEditor::SanitizeCoordinates(const Coordinates& aValue) const
{
    auto line = aValue.mLine;
    auto column = aValue.mColumn;

    if (line >= (int)mLines.size())
    {
        if (mLines.empty())
        {
            line = 0;
            column = 0;
        }
        else
        {
            line = (int)mLines.size() - 1;
            column = (int)mLines[line].size();
        }
    }
    else
    {
        column = mLines.empty() ? 0 : std::min((int)mLines[line].size(), aValue.mColumn);
    }
    return Coordinates(line, column);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Advance(Coordinates& aCoordinates) const
{
    if (aCoordinates.mLine < (int)mLines.size())
    {
        auto& line = mLines[aCoordinates.mLine];
        if (aCoordinates.mColumn + 1 < (int)line.size())
        {
            ++aCoordinates.mColumn;
        }
        else
        {
            ++aCoordinates.mLine;
            aCoordinates.mColumn = 0;
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::DeleteRange(const Coordinates& aStart, const Coordinates& aEnd)
{
    assert(aEnd >= aStart);
    assert(!mReadOnly);

    if (aEnd == aStart)
    {
        return;
    }

    if (aStart.mLine == aEnd.mLine)
    {
        auto& line = mLines[aStart.mLine];
        auto& line_s = textLinesStore.lines.elementAt(aStart.mLine);

        if (aEnd.mColumn >= (int)line.size())
        {
            line.erase(line.begin() + aStart.mColumn, line.end());           
            erase_chars(line_s, aStart.mColumn, line_s->length());
        }
        else
        {
            line.erase(line.begin() + aStart.mColumn, line.begin() + aEnd.mColumn);
            std::wstring src((wchar_t*)line_s->getWChars());
            erase_chars(line_s, aStart.mColumn, aEnd.mColumn);
            std::wstring res((wchar_t*)line_s->getWChars());
        }
    }
    else
    {
        auto& firstLine = mLines[aStart.mLine];
        auto& firstline_s = textLinesStore.lines.elementAt(aStart.mLine);
        auto& lastLine = mLines[aEnd.mLine];
        auto& lastline_s = textLinesStore.lines.elementAt(aEnd.mLine);

        firstLine.erase(firstLine.begin() + aStart.mColumn, firstLine.end());
        erase_chars(firstline_s, aStart.mColumn, firstline_s->length());
        lastLine.erase(lastLine.begin(), lastLine.begin() + aEnd.mColumn);
        erase_chars(lastline_s, 0, aEnd.mColumn);
        
        if (aStart.mLine < aEnd.mLine)
        {
            firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());
            insert_chars(firstline_s, (wchar*)lastline_s->getWChars(), lastline_s->length(), firstline_s->length());
        }
        if (aStart.mLine < aEnd.mLine)
        {
            RemoveLine(aStart.mLine + 1, aEnd.mLine + 1);
            for (int l = aStart.mLine; l < aEnd.mLine; ++l)
            {                
                std::wstring str((wchar_t*)textLinesStore.lines.elementAt(aStart.mLine)->getWChars());
                std::wcout << "removing " << str << std::endl;
                delete textLinesStore.lines.elementAt(aStart.mLine);
                textLinesStore.lines.removeElementAt(aStart.mLine);
            }            
        }
    }
    mTextChanged = true;
}
// ----------------------------------------------
//
// ----------------------------------------------
int TextEditor::InsertTextAt(Coordinates& /* inout */ aWhere, const CText::Char* aValue)
{
    assert(!mReadOnly);

    int totalLines = 0;
    auto chr = *aValue;
    while (chr != _T('\0'))
    {
        assert(!mLines.empty());
        if (chr == _T('\r'))
        {
            // skip
        }
        else if (chr == _T('\n'))
        {
            if (aWhere.mColumn < (int)mLines[aWhere.mLine].size())
            {
                auto& newLine = InsertLine(aWhere.mLine + 1);
                auto& line = mLines[aWhere.mLine];
                
                textLinesStore.lines.insertElementAt(new DString(""), aWhere.mLine + 1);
                auto& line_s = textLinesStore.lines.elementAt(aWhere.mLine);
                

                newLine.insert(newLine.begin(), line.begin() + aWhere.mColumn, line.end());
                line.erase(line.begin() + aWhere.mColumn, line.end());
                erase_chars(line_s, aWhere.mColumn, line_s->length()-1);
            }
            else
            {
                InsertLine(aWhere.mLine + 1);
                textLinesStore.lines.insertElementAt(new DString(""), aWhere.mLine + 1);
            }
            ++aWhere.mLine;
            aWhere.mColumn = 0;
            ++totalLines;
        }
        else
        {
            auto& line = mLines[aWhere.mLine];
            auto& line_s = textLinesStore.lines.elementAt(aWhere.mLine);
            line.insert(line.begin() + aWhere.mColumn, Glyph(chr, ImVec4(255, 0, 255, 255)));
            insert_chars(line_s, (wchar*)aValue, 1, aWhere.mColumn);
            ++aWhere.mColumn;
        }
        chr = *(++aValue);
        mTextChanged = true;
    }
    return totalLines;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::AddUndo(UndoRecord& aValue)
{
    assert(!mReadOnly);
    mUndoBuffer.resize(mUndoIndex + 1);
    mUndoBuffer.back() = aValue;
    ++mUndoIndex;
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Coordinates TextEditor::ScreenPosToCoordinates(const ImVec2& aPosition) const
{
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 local(aPosition.x - origin.x, aPosition.y - origin.y);
    int lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));
    // ------------------------------------------
    // Compute columnCoord according to text size
    // ------------------------------------------ 
    int columnCoord = 0;
    float columnWidth = 0.0f;
    CText cumulatedString = _T("");
    float cumulatedStringWidth[2] = { 0.0f, 0.0f }; //( [0] is the lastest, [1] is the previous. I use that trick to check where cursor is exactly (important for tabs)

    if (lineNo >= 0 && lineNo < (int)mLines.size())
    {
        auto& line = mLines.at(lineNo);
        // First we find the hovered column coord.
        while (mTextStart + cumulatedStringWidth[0] < local.x &&
            (size_t)columnCoord < line.size())
        {
            cumulatedStringWidth[1] = cumulatedStringWidth[0];
            cumulatedString += line[columnCoord].mChar;
            std::string b = cumulatedString.toSingle().str();
            cumulatedStringWidth[0] = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, b.c_str(), nullptr, nullptr).x;
            columnWidth = (cumulatedStringWidth[0] - cumulatedStringWidth[1]);
            columnCoord++;
        }
        // Then we reduce by 1 column coord if cursor is on the left side of the hovered column.
        if (mTextStart + cumulatedStringWidth[0] - columnWidth / 2.0f > local.x)
        {
            columnCoord = std::max(0, columnCoord - 1);
        }
    }
    return SanitizeCoordinates(Coordinates(lineNo, columnCoord));
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Coordinates TextEditor::FindWordStart(const Coordinates& aFrom) const
{
    Coordinates at = aFrom;
    if (at.mLine >= (int)mLines.size())
    {
        return at;
    }
    auto& line = mLines[at.mLine];

    if (at.mColumn >= (int)line.size())
    {
        return at;
    }

    if (iswspace(line[at.mColumn].mChar))
    {
        return at;
    }
    //auto cstart = (PaletteIndex)line[at.mColumn].mColorIndex;
    while (at.mColumn > 0)
    {
        if (iswspace(line[at.mColumn].mChar))
        {
            ++at.mColumn;
            break;
        }
        --at.mColumn;
    }
    return at;
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Coordinates TextEditor::FindWordEnd(const Coordinates& aFrom) const
{
    Coordinates at = aFrom;
    if (at.mLine >= (int)mLines.size())
    {
        return at;
    }
    auto& line = mLines[at.mLine];

    if (at.mColumn >= (int)line.size())
    {
        return at;
    }

    if (iswspace(line[at.mColumn].mChar))
    {
        return at;
    }

    while (at.mColumn < (int)line.size())
    {
        if (iswspace(line[at.mColumn].mChar))
        {
            break;
        }
        ++at.mColumn;
    }
    return at;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::RemoveLine(int aStart, int aEnd)
{
    assert(!mReadOnly);
    assert(aEnd >= aStart);
    assert(mLines.size() > (size_t)(aEnd - aStart));
    mLines.erase(mLines.begin() + aStart, mLines.begin() + aEnd);
    assert(!mLines.empty());
    mTextChanged = true;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::RemoveLine(int aIndex)
{
    assert(!mReadOnly);
    assert(mLines.size() > 1);
    mLines.erase(mLines.begin() + aIndex);
    assert(!mLines.empty());
    mTextChanged = true;
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::Line& TextEditor::InsertLine(int aIndex)
{
    assert(!mReadOnly);
    auto& result = *mLines.insert(mLines.begin() + aIndex, Line());
    return result;
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetWordUnderCursor() const
{
    auto c = GetCursorPosition();
    return GetWordAt(c);
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetWordAt(const Coordinates& aCoords) const
{
    auto start = FindWordStart(aCoords);
    auto end = FindWordEnd(aCoords);
    CText r;
    for (auto it = start; it < end; Advance(it))
    {
        r.push_back(mLines[it.mLine][it.mColumn].mChar);
    }
    return r;
}
// ----------------------------------------------
//
// ----------------------------------------------
ImU32 TextEditor::GetGlyphColor(const Glyph& aGlyph) const
{
    return ImGui::ColorConvertFloat4ToU32(aGlyph.mColor);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::HandleKeyboardInputs()
{
    ImGuiIO& io = ImGui::GetIO();
    auto shift = io.KeyShift;
    auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
    auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

    if (ImGui::IsWindowFocused())
    {
        if (ImGui::IsWindowHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        }

        io.WantCaptureKeyboard = true;
        io.WantTextInput = true;

        if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
            Undo();
        else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
            Undo();
        else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
            Redo();
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
            MoveUp(1, shift);
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
            MoveDown(1, shift);
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
            MoveLeft(1, shift, ctrl);
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
            MoveRight(1, shift, ctrl);
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
            MoveUp(GetPageSize() - 4, shift);
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
            MoveDown(GetPageSize() - 4, shift);
        else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
            MoveTop(shift);
        else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
            MoveBottom(shift);
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
            MoveHome(shift);
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
            MoveEnd(shift);
        else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
            Delete();
        else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
            BackSpace();
        else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
            mOverwrite ^= true;
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
            Copy();
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
            Copy();
        else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
            Paste();
        else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
            Paste();
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
            Cut();
        else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
            Cut();
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
            SelectAll();
        else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
            EnterCharacter(_T('\n'), false);
        else if (!IsReadOnly() && !ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
            EnterCharacter(_T('\t'), shift);
        else if (!IsReadOnly() && !ctrl && !alt)
        {
            for (int i = 0; i < io.InputQueueCharacters.Size; i++)
            {
                auto c = io.InputQueueCharacters[i];
                if (c != 0)
                {
                    if (iswprint(c) || iswspace(c))
                    {
                        EnterCharacter(c, shift);
                    }
                }
                //std::wcout << (Char)c << std::endl;
            }
            io.InputQueueCharacters.resize(0);
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::HandleMouseInputs()
{
    ImGuiIO& io = ImGui::GetIO();
    auto shift = io.KeyShift;
    auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
    auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

    if (ImGui::IsWindowHovered())
    {
        if (!shift && !alt)
        {
            auto click = ImGui::IsMouseClicked(0);
            auto doubleClick = ImGui::IsMouseDoubleClicked(0);
            auto t = ImGui::GetTime();
            auto tripleClick = click && !doubleClick && (mLastClick != -1.0f && (t - mLastClick) < io.MouseDoubleClickTime);

            // ------------------------------
            // Left mouse button triple click
            // -------------------------------

            if (tripleClick)
            {
                if (!ctrl)
                {
                    mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
                    mSelectionMode = SelectionMode::Line;
                    SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
                }

                mLastClick = -1.0f;
            }

            // -------------------------------
            // Left mouse button double click
            // --------------------------------

            else if (doubleClick)
            {
                if (!ctrl)
                {
                    mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
                    if (mSelectionMode == SelectionMode::Line)
                        mSelectionMode = SelectionMode::Normal;
                    else
                        mSelectionMode = SelectionMode::Word;
                    SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
                }

                mLastClick = (float)ImGui::GetTime();
            }

            // -----------------------
            // Left mouse button click
            // ------------------------
            else if (click)
            {
                mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
                if (ctrl)
                    mSelectionMode = SelectionMode::Word;
                else
                    mSelectionMode = SelectionMode::Normal;
                SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);

                mLastClick = (float)ImGui::GetTime();
            }
            // Mouse left button dragging (=> update selection)
            else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
            {
                io.WantCaptureMouse = true;
                mState.mCursorPosition = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos()));
                SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
            }
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Render()
{
    std::string b;
    /* Compute mCharAdvance regarding to scaled font size (Ctrl + mouse wheel)*/
    //ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    
    ImGui::PushFont(regular_font);
    
    const float fontSize = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)"#", nullptr, nullptr).x;
    mCharAdvance = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing() * mLineSpacing);


    static CText buffer;
    assert(buffer.isEmpty());

    auto contentSize = ImGui::GetWindowContentRegionMax();
    auto drawList = ImGui::GetWindowDrawList();
    float longest(mTextStart);

    if (mScrollToTop)
    {
        mScrollToTop = false;
        ImGui::SetScrollY(0.f);
    }

    ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
    auto scrollX = ImGui::GetScrollX();
    auto scrollY = ImGui::GetScrollY();
    
    int lineNo = (int)floor(scrollY / mCharAdvance.y);
    lineStart = lineNo;
    auto globalLineMax = (int)mLines.size();
    lineMax = std::max(0, std::min((int)mLines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));

    colorize();

    // Deduce mTextStart by evaluating mLines size (global lineMax) plus two spaces as text width
    CText::Char buf[16];
    _snwprintf(buf, 16, _T(" %d "), globalLineMax);
    b = CText(buf).toSingle().str();
    mTextStart = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)b.c_str(), nullptr, nullptr).x + mLeftMargin;

    if (!mLines.empty())
    {
        auto fontScale = 1;// ImGui::GetFontSize() / font->FontSize;
        float spaceSize = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)" ", nullptr, nullptr).x;
           
        while (lineNo <= lineMax)
        {
            ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * mCharAdvance.y);
            ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

            auto& line = mLines[lineNo];
            longest = std::max(mTextStart + TextDistanceToLineStart(Coordinates(lineNo, (int)line.size())), longest);
            auto columnNo = 0;
            Coordinates lineStartCoord(lineNo, 0);
            Coordinates lineEndCoord(lineNo, (int)line.size());

            // Draw selection for the current line
            float sstart = -1.0f;
            float ssend = -1.0f;

            assert(mState.mSelectionStart <= mState.mSelectionEnd);
            if (mState.mSelectionStart <= lineEndCoord)
                sstart = mState.mSelectionStart > lineStartCoord ? TextDistanceToLineStart(mState.mSelectionStart) : 0.0f;
            if (mState.mSelectionEnd > lineStartCoord)
                ssend = TextDistanceToLineStart(mState.mSelectionEnd < lineEndCoord ? mState.mSelectionEnd : lineEndCoord);

            if (mState.mSelectionEnd.mLine > lineNo)
                ssend += mCharAdvance.x;

            if (sstart != -1 && ssend != -1 && sstart < ssend)
            {
                ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
                ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
                drawList->AddRectFilled(vstart, vend, ImGui::ColorConvertFloat4ToU32(ImVec4(0.2, 0.2, 0.2, 1))); // фон выделения
            }

            // Draw breakpoints
            auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);
            // Highlight the current line (where the cursor is)
            if (mState.mCursorPosition.mLine == lineNo)
            {
                auto focused = ImGui::IsWindowFocused();

                if (!HasSelection())
                {
                    auto end = ImVec2(start.x + contentSize.x + scrollX, start.y + mCharAdvance.y);
                    drawList->AddRectFilled(start, end, focused ? ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0.1, 0.1, 1)) : ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0.1, 0.1, 1)));
                    drawList->AddRect(start, end, ImGui::ColorConvertFloat4ToU32(ImVec4(0.5, 0.5, 0.5, 1)), 1.0f);
                }

                float cx = TextDistanceToLineStart(mState.mCursorPosition);

                // курсор 
                if (focused)
                {
                    static auto timeStart = std::chrono::system_clock::now();
                    auto timeEnd = std::chrono::system_clock::now();
                    auto diff = timeEnd - timeStart;
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
                    if (elapsed > 400)
                    {
                        ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
                        ImVec2 cend(textScreenPos.x + cx + (mOverwrite ? mCharAdvance.x : 1.0f), lineStartScreenPos.y + mCharAdvance.y);
                        drawList->AddRectFilled(cstart, cend, ImGui::ColorConvertFloat4ToU32(ImVec4(0.2, 1.0, 0.2, 1)));
                        if (elapsed > 800)
                        {
                            timeStart = timeEnd;
                        }
                    }
                }
            }

            // Render colorized text
            auto prevColor = line.empty() ? ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)) : GetGlyphColor(line[0]);
            int prevStyle = line.empty() ? 0 : line[0].mBold;

            ImVec2 bufferOffset;
            //int f = 0;
            int Style = 0;
            for (auto& glyph : line)
            {
                Style = glyph.mBold;

                auto color = GetGlyphColor(glyph); // foreground color
                //glyph.mBold = f%2;
                //f++;

                if ((color != prevColor || Style!=prevStyle || glyph.mChar == _T('\t')) && !buffer.isEmpty())
                {
                    const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
                    b = buffer.toSingle().str();
                    
                    if (prevStyle==1)
                    {
                        ImGuiStyle& style = ImGui::GetStyle();
                        ImGui::PushFont(bold_font);                        
                    }
                    drawList->AddText(newOffset, prevColor, (char*)b.c_str());
                    if (prevStyle==1)
                    {
                        ImGui::PopFont();
                    }
                    auto textSize = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)b.c_str(), nullptr, nullptr);
                    bufferOffset.x += textSize.x;
                    buffer.clear();
                }
                prevColor = color;
                prevStyle = Style;

                if (glyph.mChar == _T('\t'))
                {
                    bufferOffset.x = (1.0f * fontScale + std::floor((1.0f + bufferOffset.x)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
                }
                else
                {
                    AppendBuffer(buffer, glyph.mChar, 0);
                }
                ++columnNo;
                              
            }

            if (!buffer.isEmpty())
            {
                const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
                b = buffer.toSingle().str();
                if (prevStyle==1)
                {
                    ImGuiStyle& style = ImGui::GetStyle();
                    ImGui::PushFont(bold_font);                    
                }
                drawList->AddText(newOffset, prevColor, (char*)b.c_str());
                if (prevStyle == 1)
                {                    
                    ImGui::PopFont();
                }
                buffer.clear();
            }
            
            // draw line numbers
            ImU32 Col = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1));
            _snwprintf(buf, 16, _T(" %d  "), lineNo + 1);
            b = CText(buf).toSingle().str();
            auto lineNoWidth = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)b.c_str(), nullptr, nullptr).x;
            
            ImGui::PushFont(regular_font);
            drawList->AddText(ImVec2(lineStartScreenPos.x + mTextStart+ 10 - lineNoWidth, lineStartScreenPos.y), Col, (char*)b.c_str());
            ImGui::PopFont();

            lineNo++;

        }

        // Draw a tooltip on known identifiers/preprocessor symbols
        if (ImGui::IsMousePosValid())
        {
            auto id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos()));
            if (!id.isEmpty())
            {
                b = id.toSingle().str();
                //wstrtostr(id);
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(b.c_str());
                ImGui::EndTooltip();
            }
        }
    }
    ImGui::PopFont();
    ImGui::Dummy(ImVec2((longest + 2), mLines.size() * mCharAdvance.y));
    if (mScrollToCursor)
    {
        EnsureCursorVisible();
        ImGui::SetWindowFocus();
        mScrollToCursor = false;
    }
    
}
// -----------------------------
//
// -----------------------------

void TextEditor::colorize()
{
    if (baseEditor_ptr != NULL)
    {
        for (int i = 0; i < colors.size(); ++i)
        {
            colors[i].clear();
        }
        colors.clear();

        for (int i = 0; i < styles.size(); ++i)
        {
            styles[i].clear();
        }
        styles.clear();

        baseEditor_ptr->lineCountEvent(GetTotalLines());
        baseEditor_ptr->modifyLineEvent(GetCursorPosition().mLine);
        baseEditor_ptr->visibleTextEvent(lineStart, lineMax - lineStart);

        const StyledRegion* rd = StyledRegion::cast(baseEditor_ptr->rd_def_Text);

        int topline = lineStart, leftpos;
        leftpos = 0;
        int n_lines = lineMax - lineStart + 1;//textLinesStore_ptr->getLineCount();
        int n_cols = 0;
        int lline = n_lines;
        if (topline + lline > textLinesStore_ptr->getLineCount()) lline = textLinesStore_ptr->getLineCount() - topline;
        baseEditor_ptr->visibleTextEvent(topline, lline);

        for (int i = topline; i < topline + n_lines; i++)
        {
            std::vector<WORD> colors_line;
            std::vector<WORD> styles_line;
            int Y = i - topline;
            int li;
            if (i >= textLinesStore_ptr->getLineCount()) continue;
            n_cols = textLinesStore_ptr->getLine(i)->length();
            colors_line.resize(n_cols);
            styles_line.resize(n_cols);
            for (LineRegion* l1 = baseEditor_ptr->getLineRegions(i); l1 != null; l1 = l1->next)
            {
                if (l1->special || l1->rdef == null) continue;
                int end = l1->end;
                if (end == -1) end = n_cols;
                int X = l1->start - leftpos;
                int len = end - l1->start;
                if (X < 0)
                {
                    len += X;
                    X = 0;
                };
                if (len < 0 || X >= n_cols) continue;
                if (len + X > n_cols) len = n_cols - X;
                WORD color = (WORD)(l1->styled()->fore);
                WORD style = (WORD)(l1->styled()->style);
                std::fill(colors_line.begin() + X, colors_line.begin() + X + len, color);
                std::fill(styles_line.begin() + X, styles_line.begin() + X + len, style);
            }
            colors.push_back(colors_line);
            styles.push_back(styles_line);
        }

        for (int i = topline; i < std::min((int)mLines.size(), topline + n_lines); ++i)
        {
            for (int j = 0; j < mLines[i].size(); ++j)
            {
                if (i - topline < colors.size())
                {
                    if (j < colors[i - topline].size())
                    {
                        WORD c = colors[i - topline][j];
                        float b = float(c & 0x000000FF) / 255.0;
                        float g = float((c & 0x0000FF00) >> 8) / 255.0;
                        float r = float((c & 0x00FF0000) >> 16) / 255.0;
                        WORD s = styles[i - topline][j];
                        mLines[i][j].mColor = ImVec4(1.0f - r, 1.0f - g, 1.0f - b, 1.0f);
                        if (s & StyledRegion::RD_BOLD)
                        {
                            mLines[i][j].mBold = true;
                            //lines[i][j].mColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
                        }
                        else
                            mLines[i][j].mBold = false;
                    }
                }
            }
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
int colorIndexFromName(std::string col_name)
{
    int ind = -1;
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        char* name = (char*)ImGui::GetStyleColorName(i);
        if (name == col_name)
        {
            ind = i;
            break;
        }
    }
    return ind;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Render(const CText::Char* aTitle, const ImVec2& aSize, bool aBorder)
{
    ImGuiStyle& style = ImGui::GetStyle();
    mWithinRender = true;
    mTextChanged = false;
    mCursorPositionChanged = false;
    ImVec4 bg_col = style.Colors[colorIndexFromName("ChildBg")];
    ImGui::PushStyleColor(3, bg_col); // background
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::BeginChild((char*)aTitle, aSize, aBorder, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove);
    ImGui::PushAllowKeyboardFocus(true);

    HandleKeyboardInputs();
    HandleMouseInputs();
    Render();

    ImGui::PopAllowKeyboardFocus();
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    mWithinRender = false;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetText(const CText& aText)
{
    mLines.clear();
    for (int i = 0; i < colors.size(); ++i)
    {
        colors[i].clear();
    }
    colors.clear();
    for (int i = 0; i < styles.size(); ++i)
    {
        styles[i].clear();
    }
    styles.clear();

    for (int i = 0; i < textLinesStore.lines.size(); ++i)
    {
        delete textLinesStore.lines.elementAt(i);
    }
    textLinesStore.lines.clear();

    mLines.emplace_back(Line());
    textLinesStore.lines.addElement(new DString(""));

    for (int i=0;i<aText.length();++i)
    {
        auto chr = aText[i];
        if (chr == _T('\r'))
        {
            // ignore the carriage return character
        }
        else if (chr == _T('\n'))
        {
            mLines.emplace_back(Line());
            textLinesStore.lines.addElement(new DString(""));
        }
        else
        {
            mLines.back().emplace_back(Glyph(chr, ImVec4(255, 0, 255, 255)));            
            append_char(textLinesStore.lines.lastElement(), chr);
            //textLinesStore.lines.addElement(new SString(chr));
        }
    }
    mTextChanged = true;
    mScrollToTop = true;
    mUndoBuffer.clear();
    mUndoIndex = 0;

    lineStart = 0;
    lineMax = mLines.size();

    colorize();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetTextLines(const std::vector<CText>& aLines)
{
    mLines.clear();
    for (int i = 0; i < colors.size(); ++i)
    {
        colors[i].clear();
    }
    colors.clear();
    for (int i = 0; i < styles.size(); ++i)
    {
        styles[i].clear();
    }
    styles.clear();
    for (int i = 0; i < textLinesStore.lines.size(); ++i)
    {
        delete textLinesStore.lines.elementAt(i);
    }
    textLinesStore.lines.clear();

    if (aLines.empty())
    {
        mLines.emplace_back(Line());
        textLinesStore.lines.addElement(new DString(""));
    }
    else
    {
        mLines.resize(aLines.size());

        for (size_t i = 0; i < aLines.size(); ++i)
        {
            const CText& aLine = aLines[i];

            mLines[i].reserve(aLine.length());
            for (size_t j = 0; j < aLine.length(); ++j)
            {
                mLines[i].emplace_back(Glyph(aLine[j], ImVec4(1, 1, 1, 1)));
            }
            textLinesStore.lines.addElement(new DString( (wchar*)aLine.str() ));
        }
    }
    lineStart = 0;
    lineMax = mLines.size();

    mTextChanged = true;
    mScrollToTop = true;
    mUndoBuffer.clear();
    mUndoIndex = 0;
    colorize();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::EnterCharacter(CText::Char aChar, bool aShift)
{
    assert(!mReadOnly);

    UndoRecord u;

    u.mBefore = mState;

    if (HasSelection())
    {
        if (aChar == _T('\t'))
        {
            auto start = mState.mSelectionStart;
            auto end = mState.mSelectionEnd;

            if (start > end)
            {
                std::swap(start, end);
            }
            start.mColumn = 0;
            //			end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
            if (end.mColumn == 0 && end.mLine > 0)
            {
                --end.mLine;
                end.mColumn = (int)mLines[end.mLine].size();
            }

            u.mRemovedStart = start;
            u.mRemovedEnd = end;
            u.mRemoved = GetText(start, end);

            bool modified = false;

            for (int i = start.mLine; i <= end.mLine; i++)
            {
                auto& line = mLines[i];
                auto& line_s = textLinesStore.lines.elementAt(i);

                if (aShift)
                {
                    if (line.empty() == false)
                    {
                        if (line.front().mChar == _T('\t'))
                        {
                            line.erase(line.begin());
                            erase_chars(line_s, 0, 1);

                            if (i == end.mLine && end.mColumn > 0)
                            {
                                end.mColumn--;
                            }
                            modified = true;
                        }
                    }
                    else
                    {
                        for (int j = 0; j < mTabSize && line.empty() == false && line.front().mChar == _T(' '); j++)
                        {
                            line.erase(line.begin());
                            erase_chars(line_s, 0, 1);
                            if (i == end.mLine && end.mColumn > 0)
                            {
                                end.mColumn--;
                            }
                            modified = true;
                        }
                    }
                }
                else
                {
                    line.insert(line.begin(), Glyph(_T('\t'), ImVec4(255, 0, 0, 0)));
                    prepend_char(line_s, L'\t');
                    if (i == end.mLine)
                    {
                        ++end.mColumn;
                    }
                    modified = true;
                }
            }

            if (modified)
            {
                u.mAddedStart = start;
                u.mAddedEnd = end;
                u.mAdded = GetText(start, end);
                mTextChanged = true;
                AddUndo(u);
                EnsureCursorVisible();
            }
            return;
        }
        else
        {
            u.mRemoved = GetSelectedText();
            u.mRemovedStart = mState.mSelectionStart;
            u.mRemovedEnd = mState.mSelectionEnd;
            DeleteSelection();
        }
    }

    auto coord = GetActualCursorCoordinates();
    u.mAddedStart = coord;

    assert(!mLines.empty());

    if (aChar == _T('\n'))
    {
        InsertLine(coord.mLine + 1);
        textLinesStore.lines.insertElementAt(new DString(""),coord.mLine + 1);

        auto& line = mLines[coord.mLine];
        auto& line_s = textLinesStore.lines.elementAt(coord.mLine);

        auto& newLine = mLines[coord.mLine + 1];
        auto& newLine_s = textLinesStore.lines.elementAt(coord.mLine+1);

        const size_t whitespaceSize = newLine.size();
        newLine.insert(newLine.end(), line.begin() + coord.mColumn, line.end());
        
        insert_chars(newLine_s, (wchar*)line_s->getWChars() + coord.mColumn, line_s->length() - coord.mColumn, newLine_s->length());
        
        line.erase(line.begin() + coord.mColumn, line.begin() + line.size());
        
        erase_chars(line_s, coord.mColumn, line_s->length());
        
        SetCursorPosition(Coordinates(coord.mLine + 1, (int)whitespaceSize));
    }
    else
    {
        auto& line = mLines[coord.mLine];
        auto& line_s = textLinesStore.lines.elementAt(coord.mLine);

        if (mOverwrite && (int)line.size() > coord.mColumn)
        {
            line[coord.mColumn] = Glyph(aChar, ImVec4(255, 0, 255, 255));
            overwrite_chars(line_s, (wchar*)&aChar, 1, coord.mColumn);
        }
        else
        {
            line.insert(line.begin() + coord.mColumn, Glyph(aChar, ImVec4(1, 1, 1, 1)));
            insert_chars(line_s, (wchar*)&aChar, 1, coord.mColumn);
        }
        SetCursorPosition(Coordinates(coord.mLine, coord.mColumn + 1));
    }

    colorize();

    mTextChanged = true;
    u.mAdded = aChar;
    u.mAddedEnd = GetActualCursorCoordinates();
    u.mAfter = mState;
    AddUndo(u);   
    EnsureCursorVisible();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetReadOnly(bool aValue)
{
    mReadOnly = aValue;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetCursorPosition(const Coordinates& aPosition)
{
    if (mState.mCursorPosition != aPosition)
    {
        mState.mCursorPosition = aPosition;
        mCursorPositionChanged = true;
        EnsureCursorVisible();
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetSelectionStart(const Coordinates& aPosition)
{
    mState.mSelectionStart = SanitizeCoordinates(aPosition);
    if (mState.mSelectionStart > mState.mSelectionEnd)
        std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetSelectionEnd(const Coordinates& aPosition)
{
    mState.mSelectionEnd = SanitizeCoordinates(aPosition);
    if (mState.mSelectionStart > mState.mSelectionEnd)
        std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SetSelection(const Coordinates& aStart, const Coordinates& aEnd, SelectionMode aMode)
{
    auto oldSelStart = mState.mSelectionStart;
    auto oldSelEnd = mState.mSelectionEnd;

    mState.mSelectionStart = SanitizeCoordinates(aStart);
    mState.mSelectionEnd = SanitizeCoordinates(aEnd);
    if (aStart > aEnd)
    {
        std::swap(mState.mSelectionStart, mState.mSelectionEnd);
    }
    switch (aMode)
    {
    case TextEditor::SelectionMode::Normal:
        break;
    case TextEditor::SelectionMode::Word:
    {
        mState.mSelectionStart = FindWordStart(mState.mSelectionStart);
        mState.mSelectionEnd = FindWordEnd(mState.mSelectionEnd);
        break;
    }
    case TextEditor::SelectionMode::Line:
    {
        const auto lineNo = mState.mSelectionEnd.mLine;
        const auto lineSize = (size_t)lineNo < mLines.size() ? mLines[lineNo].size() : 0;
        mState.mSelectionStart = Coordinates(mState.mSelectionStart.mLine, 0);
        mState.mSelectionEnd = Coordinates(lineNo, (int)lineSize);
        break;
    }
    default:
        break;
    }

    if (mState.mSelectionStart != oldSelStart ||
        mState.mSelectionEnd != oldSelEnd)
    {
        mCursorPositionChanged = true;
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::InsertText(const CText& aValue)
{
    InsertText(aValue.str());
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::InsertText(const CText::Char* aValue)
{
    if (aValue == nullptr)
    {
        return;
    }
    auto pos = GetActualCursorCoordinates();
    auto start = std::min(pos, mState.mSelectionStart);
    int totalLines = pos.mLine - start.mLine;

    totalLines += InsertTextAt(pos, aValue);

    SetSelection(pos, pos);
    SetCursorPosition(pos);
    colorize();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::DeleteSelection()
{
    assert(mState.mSelectionEnd >= mState.mSelectionStart);

    if (mState.mSelectionEnd == mState.mSelectionStart)
        return;

    DeleteRange(mState.mSelectionStart, mState.mSelectionEnd);

    SetSelection(mState.mSelectionStart, mState.mSelectionStart);
    SetCursorPosition(mState.mSelectionStart);
    colorize();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveUp(int aAmount, bool aSelect)
{
    auto oldPos = mState.mCursorPosition;
    mState.mCursorPosition.mLine = std::max(0, mState.mCursorPosition.mLine - aAmount);
    if (oldPos != mState.mCursorPosition)
    {
        if (aSelect)
        {
            if (oldPos == mInteractiveStart)
                mInteractiveStart = mState.mCursorPosition;
            else if (oldPos == mInteractiveEnd)
                mInteractiveEnd = mState.mCursorPosition;
            else
            {
                mInteractiveStart = mState.mCursorPosition;
                mInteractiveEnd = oldPos;
            }
        }
        else
            mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
        SetSelection(mInteractiveStart, mInteractiveEnd);

        EnsureCursorVisible();
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveDown(int aAmount, bool aSelect)
{
    assert(mState.mCursorPosition.mColumn >= 0);
    auto oldPos = mState.mCursorPosition;
    mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + aAmount));

    if (mState.mCursorPosition != oldPos)
    {
        if (aSelect)
        {
            if (oldPos == mInteractiveEnd)
                mInteractiveEnd = mState.mCursorPosition;
            else if (oldPos == mInteractiveStart)
                mInteractiveStart = mState.mCursorPosition;
            else
            {
                mInteractiveStart = oldPos;
                mInteractiveEnd = mState.mCursorPosition;
            }
        }
        else
            mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
        SetSelection(mInteractiveStart, mInteractiveEnd);

        EnsureCursorVisible();
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveLeft(int aAmount, bool aSelect, bool aWordMode)
{
    if (mLines.empty())
        return;

    auto oldPos = mState.mCursorPosition;
    mState.mCursorPosition = GetActualCursorCoordinates();

    while (aAmount-- > 0)
    {
        if (mState.mCursorPosition.mColumn == 0)
        {
            if (mState.mCursorPosition.mLine > 0)
            {
                --mState.mCursorPosition.mLine;
                mState.mCursorPosition.mColumn = (int)mLines[mState.mCursorPosition.mLine].size();
            }
        }
        else
        {
            mState.mCursorPosition.mColumn = std::max(0, mState.mCursorPosition.mColumn - 1);
            if (aWordMode)
                mState.mCursorPosition = FindWordStart(mState.mCursorPosition);
        }
    }

    assert(mState.mCursorPosition.mColumn >= 0);
    if (aSelect)
    {
        if (oldPos == mInteractiveStart)
            mInteractiveStart = mState.mCursorPosition;
        else if (oldPos == mInteractiveEnd)
            mInteractiveEnd = mState.mCursorPosition;
        else
        {
            mInteractiveStart = mState.mCursorPosition;
            mInteractiveEnd = oldPos;
        }
    }
    else
        mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
    SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);

    EnsureCursorVisible();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveRight(int aAmount, bool aSelect, bool aWordMode)
{
    auto oldPos = mState.mCursorPosition;

    if (mLines.empty())
        return;

    while (aAmount-- > 0)
    {
        auto& line = mLines[mState.mCursorPosition.mLine];
        if (mState.mCursorPosition.mColumn >= (int)line.size())
        {
            if (mState.mCursorPosition.mLine < (int)mLines.size() - 1)
            {
                mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + 1));
                mState.mCursorPosition.mColumn = 0;
            }
        }
        else
        {
            mState.mCursorPosition.mColumn = std::max(0, std::min((int)line.size(), mState.mCursorPosition.mColumn + 1));
            if (aWordMode)
                mState.mCursorPosition = FindWordEnd(mState.mCursorPosition);
        }
    }

    if (aSelect)
    {
        if (oldPos == mInteractiveEnd)
            mInteractiveEnd = SanitizeCoordinates(mState.mCursorPosition);
        else if (oldPos == mInteractiveStart)
            mInteractiveStart = mState.mCursorPosition;
        else
        {
            mInteractiveStart = oldPos;
            mInteractiveEnd = mState.mCursorPosition;
        }
    }
    else
        mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
    SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);

    EnsureCursorVisible();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveTop(bool aSelect)
{
    auto oldPos = mState.mCursorPosition;
    SetCursorPosition(Coordinates(0, 0));

    if (mState.mCursorPosition != oldPos)
    {
        if (aSelect)
        {
            mInteractiveEnd = oldPos;
            mInteractiveStart = mState.mCursorPosition;
        }
        else
            mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
        SetSelection(mInteractiveStart, mInteractiveEnd);
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::TextEditor::MoveBottom(bool aSelect)
{
    auto oldPos = GetCursorPosition();
    auto newPos = Coordinates((int)mLines.size() - 1, 0);
    SetCursorPosition(newPos);
    if (aSelect)
    {
        mInteractiveStart = oldPos;
        mInteractiveEnd = newPos;
    }
    else
        mInteractiveStart = mInteractiveEnd = newPos;
    SetSelection(mInteractiveStart, mInteractiveEnd);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveHome(bool aSelect)
{
    auto oldPos = mState.mCursorPosition;
    SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, 0));

    if (mState.mCursorPosition != oldPos)
    {
        if (aSelect)
        {
            if (oldPos == mInteractiveStart)
                mInteractiveStart = mState.mCursorPosition;
            else if (oldPos == mInteractiveEnd)
                mInteractiveEnd = mState.mCursorPosition;
            else
            {
                mInteractiveStart = mState.mCursorPosition;
                mInteractiveEnd = oldPos;
            }
        }
        else
            mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
        SetSelection(mInteractiveStart, mInteractiveEnd);
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::MoveEnd(bool aSelect)
{
    auto oldPos = mState.mCursorPosition;
    SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, (int)mLines[oldPos.mLine].size()));

    if (mState.mCursorPosition != oldPos)
    {
        if (aSelect)
        {
            if (oldPos == mInteractiveEnd)
                mInteractiveEnd = mState.mCursorPosition;
            else if (oldPos == mInteractiveStart)
                mInteractiveStart = mState.mCursorPosition;
            else
            {
                mInteractiveStart = oldPos;
                mInteractiveEnd = mState.mCursorPosition;
            }
        }
        else
            mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
        SetSelection(mInteractiveStart, mInteractiveEnd);
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Delete()
{
    assert(!mReadOnly);

    if (mLines.empty())
        return;

    UndoRecord u;
    u.mBefore = mState;

    if (HasSelection())
    {
        u.mRemoved = GetSelectedText();
        u.mRemovedStart = mState.mSelectionStart;
        u.mRemovedEnd = mState.mSelectionEnd;

        DeleteSelection();
    }
    else
    {
        auto pos = GetActualCursorCoordinates();
        SetCursorPosition(pos);
        auto& line = mLines[pos.mLine];

        if (pos.mColumn == (int)line.size())
        {
            if (pos.mLine == (int)mLines.size() - 1)
                return;

            u.mRemoved = _T('\n');
            u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
            Advance(u.mRemovedEnd);

            auto& nextLine = mLines[pos.mLine + 1];
            line.insert(line.end(), nextLine.begin(), nextLine.end());
            RemoveLine(pos.mLine + 1);

            auto& line_s = textLinesStore.lines.elementAt(pos.mLine);
            auto& nextLine_s = textLinesStore.lines.elementAt(pos.mLine + 1);            
            insert_chars(line_s, (wchar*)nextLine_s->getWChars(), nextLine_s->length(), line_s->length());
            delete textLinesStore.lines.elementAt(pos.mLine + 1);
            textLinesStore.lines.removeElementAt(pos.mLine + 1);


        }
        else
        {
            u.mRemoved = line[pos.mColumn].mChar;
            u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
            u.mRemovedEnd.mColumn++;

            line.erase(line.begin() + pos.mColumn);
            auto& line_s = textLinesStore.lines.elementAt(pos.mLine);
            erase_chars(line_s, pos.mColumn, pos.mColumn+1);
        }

        mTextChanged = true;

        colorize();
    }

    u.mAfter = mState;
    AddUndo(u);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::BackSpace()
{
    assert(!mReadOnly);

    if (mLines.empty())
        return;

    UndoRecord u;
    u.mBefore = mState;

    if (HasSelection())
    {
        u.mRemoved = GetSelectedText();
        u.mRemovedStart = mState.mSelectionStart;
        u.mRemovedEnd = mState.mSelectionEnd;

        DeleteSelection();
    }
    else
    {
        auto pos = GetActualCursorCoordinates();
        SetCursorPosition(pos);

        if (mState.mCursorPosition.mColumn == 0)
        {
            if (mState.mCursorPosition.mLine == 0)
                return;

            u.mRemoved = _T('\n');
            u.mRemovedStart = u.mRemovedEnd = Coordinates(pos.mLine - 1, (int)mLines[pos.mLine - 1].size());
            Advance(u.mRemovedEnd);

            auto& line = mLines[mState.mCursorPosition.mLine];
            auto& prevLine = mLines[mState.mCursorPosition.mLine - 1];
            auto prevSize = (int)prevLine.size();
            prevLine.insert(prevLine.end(), line.begin(), line.end());

            auto& line_s = textLinesStore.lines.elementAt(mState.mCursorPosition.mLine) ;
            auto& prevLine_s = textLinesStore.lines.elementAt(mState.mCursorPosition.mLine-1);
            auto prevSize_s = (int)prevLine_s->length();
            insert_chars(prevLine_s, (wchar*)line_s->getWChars(), line_s->length(), prevLine_s->length());
            delete textLinesStore.lines.elementAt(mState.mCursorPosition.mLine);
            textLinesStore.lines.removeElementAt(mState.mCursorPosition.mLine);

            RemoveLine(mState.mCursorPosition.mLine);
            --mState.mCursorPosition.mLine;
            mState.mCursorPosition.mColumn = prevSize;
        }
        else
        {
            auto& line = mLines[mState.mCursorPosition.mLine];

            u.mRemoved = line[pos.mColumn - 1].mChar;
            u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
            --u.mRemovedStart.mColumn;

            --mState.mCursorPosition.mColumn;
            if (mState.mCursorPosition.mColumn < (int)line.size())
            {
                line.erase(line.begin() + mState.mCursorPosition.mColumn);
                auto& line_s = textLinesStore.lines.elementAt(pos.mLine);
                erase_chars(line_s, mState.mCursorPosition.mColumn, mState.mCursorPosition.mColumn + 1);
            }
        }

        mTextChanged = true;

        EnsureCursorVisible();
        colorize();
    }

    u.mAfter = mState;
    AddUndo(u);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SelectWordUnderCursor()
{
    auto c = GetCursorPosition();
    SetSelection(FindWordStart(c), FindWordEnd(c));
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::SelectAll()
{
    SetSelection(Coordinates(0, 0), Coordinates((int)mLines.size(), 0));
}
// ----------------------------------------------
//
// ----------------------------------------------
bool TextEditor::HasSelection() const
{
    return mState.mSelectionEnd > mState.mSelectionStart;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Copy()
{
    if (HasSelection())
    {
        ImGui::SetClipboardText((char*)GetSelectedText().toSingle().str());
    }
    else
    {
        if (!mLines.empty())
        {
            CText str;
            auto& line = mLines[GetActualCursorCoordinates().mLine];
            for (auto& g : line)
            {
                str.push_back(g.mChar);
            }
            ImGui::SetClipboardText((char*)str.toSingle().str());
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Cut()
{
    if (IsReadOnly())
    {
        Copy();
    }
    else
    {
        if (HasSelection())
        {
            UndoRecord u;
            u.mBefore = mState;
            u.mRemoved = GetSelectedText();
            u.mRemovedStart = mState.mSelectionStart;
            u.mRemovedEnd = mState.mSelectionEnd;

            Copy();
            DeleteSelection();

            u.mAfter = mState;
            AddUndo(u);
        }
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Paste()
{
    auto clipText = ImGui::GetClipboardText();
    if (clipText != nullptr && strlen(clipText) > 0)
    {
        UndoRecord u;
        u.mBefore = mState;

        if (HasSelection())
        {
            u.mRemoved = GetSelectedText();
            u.mRemovedStart = mState.mSelectionStart;
            u.mRemovedEnd = mState.mSelectionEnd;
            DeleteSelection();
        }

        u.mAdded.fromSingle(clipText);
        u.mAddedStart = GetActualCursorCoordinates();

        InsertText(u.mAdded);

        u.mAddedEnd = GetActualCursorCoordinates();
        u.mAfter = mState;
        AddUndo(u);
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
bool TextEditor::CanUndo() const
{
    return mUndoIndex > 0;
}
// ----------------------------------------------
//
// ----------------------------------------------
bool TextEditor::CanRedo() const
{
    return mUndoIndex < (int)mUndoBuffer.size();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Undo(int aSteps)
{
    while (CanUndo() && aSteps-- > 0)
        mUndoBuffer[--mUndoIndex].Undo(this);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::Redo(int aSteps)
{
    while (CanRedo() && aSteps-- > 0)
        mUndoBuffer[mUndoIndex++].Redo(this);
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetText() const
{
    return GetText(Coordinates(), Coordinates((int)mLines.size(), 0));
}
// ----------------------------------------------
//
// ----------------------------------------------
std::vector<CText> TextEditor::GetTextLines() const
{
    std::vector<CText> result;

    result.reserve(mLines.size());

    for (auto& line : mLines)
    {
        CText text;

        text.resize(line.size());

        for (size_t i = 0; i < line.size(); ++i)
            text[i] = line[i].mChar;

        result.emplace_back(std::move(text));
    }

    return result;
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetSelectedText() const
{
    return GetText(mState.mSelectionStart, mState.mSelectionEnd);
}
// ----------------------------------------------
//
// ----------------------------------------------
CText TextEditor::GetCurrentLineText()const
{
    auto lineLength = (int)mLines[mState.mCursorPosition.mLine].size();
    return GetText(Coordinates(mState.mCursorPosition.mLine, 0), Coordinates(mState.mCursorPosition.mLine, lineLength));
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::ProcessInputs()
{
}
// ----------------------------------------------
//
// ----------------------------------------------
/*
void TextEditor::Colorize(int aFromLine, int aLines)
{
    int toLine = aLines == -1 ? (int)mLines.size() : std::min((int)mLines.size(), aFromLine + aLines);
    mColorRangeMin = std::min(mColorRangeMin, aFromLine);
    mColorRangeMax = std::max(mColorRangeMax, toLine);
    mColorRangeMin = std::max(0, mColorRangeMin);
    mColorRangeMax = std::max(mColorRangeMin, mColorRangeMax);
}
*/
/*
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::ColorizeRange(int aFromLine, int aToLine)
{
    if (mLines.empty())
        return;
    for (auto line : mLines)
    {
        line[0].mColor = ImVec4(255, 255, 0, 255);
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::ColorizeInternal()
{

}
*/
// ----------------------------------------------
//
// ----------------------------------------------
float TextEditor::TextDistanceToLineStart(const Coordinates& aFrom) const
{
    auto& line = mLines[aFrom.mLine];
    float distance = 0.0f;
    auto fontScale = 1.0f;// ImGui::GetFontSize() / ImGui::GetFont()->FontSize;
    float spaceSize = regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)" ", nullptr, nullptr).x;
    
    CText tempCString;
    for (size_t it = 0u; it < line.size() && it < (unsigned)aFrom.mColumn; ++it)
    {
        if (line[it].mChar == _T('\t'))
        {
            distance = (1.0f * fontScale + std::floor((1.0f + distance)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
        }
        else
        {            
            tempCString.append(line[it].mChar);
        }
    }
    tempCString.append(_T('\0'));
    std::string bstr;
    bstr = tempCString.toSingle().str();
    //wstrtostr(tempCString);
    distance += regular_font->CalcTextSizeA(regular_font->FontSize, FLT_MAX, -1.0f, (char*)bstr.c_str(), nullptr, nullptr).x;

    return distance;
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::EnsureCursorVisible()
{
    if (!mWithinRender)
    {
        mScrollToCursor = true;
        return;
    }
    float scrollX = ImGui::GetScrollX();
    float scrollY = ImGui::GetScrollY();
    auto height = ImGui::GetWindowHeight();
    auto width = ImGui::GetWindowWidth();
    auto top = 1 + (int)ceil(scrollY / mCharAdvance.y);
    auto bottom = (int)ceil((scrollY + height) / mCharAdvance.y);
    auto left = (int)ceil(scrollX / mCharAdvance.x);
    auto right = (int)ceil((scrollX + width) / mCharAdvance.x);
    auto pos = GetActualCursorCoordinates();
    auto len = TextDistanceToLineStart(pos);
    if (pos.mLine < top)
    {
        ImGui::SetScrollY(std::max(0.0f, (pos.mLine - 1) * mCharAdvance.y));
    }
    if (pos.mLine > bottom - 4)
    {
        ImGui::SetScrollY(std::max(0.0f, (pos.mLine + 4) * mCharAdvance.y - height));
    }
    if (len + mTextStart < left + 4)
    {
        ImGui::SetScrollX(std::max(0.0f, len + mTextStart - 4));
    }
    if (len + mTextStart > right - 4)
    {
        ImGui::SetScrollX(std::max(0.0f, len + mTextStart + 4 - width));
    }
}
// ----------------------------------------------
//
// ----------------------------------------------
int TextEditor::GetPageSize() const
{
    auto height = ImGui::GetWindowHeight() - 20.0f;
    return (int)floor(height / mCharAdvance.y);
}
// ----------------------------------------------
//
// ----------------------------------------------
TextEditor::UndoRecord::UndoRecord(
    const CText& aAdded,
    const TextEditor::Coordinates aAddedStart,
    const TextEditor::Coordinates aAddedEnd,
    const CText& aRemoved,
    const TextEditor::Coordinates aRemovedStart,
    const TextEditor::Coordinates aRemovedEnd,
    TextEditor::EditorState& aBefore,
    TextEditor::EditorState& aAfter)
    : mAdded(aAdded)
    , mAddedStart(aAddedStart)
    , mAddedEnd(aAddedEnd)
    , mRemoved(aRemoved)
    , mRemovedStart(aRemovedStart)
    , mRemovedEnd(aRemovedEnd)
    , mBefore(aBefore)
    , mAfter(aAfter)
{
    assert(mAddedStart <= mAddedEnd);
    assert(mRemovedStart <= mRemovedEnd);
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::UndoRecord::Undo(TextEditor* aEditor)
{
    if (!mAdded.isEmpty())
    {
        aEditor->DeleteRange(mAddedStart, mAddedEnd);
        aEditor->colorize();
    }
    if (!mRemoved.isEmpty())
    {
        auto start = mRemovedStart;
        aEditor->InsertTextAt(start, mRemoved.str());
        aEditor->colorize();
    }
    aEditor->mState = mBefore;
    aEditor->EnsureCursorVisible();
}
// ----------------------------------------------
//
// ----------------------------------------------
void TextEditor::UndoRecord::Redo(TextEditor* aEditor)
{
    if (!mRemoved.isEmpty())
    {
        aEditor->DeleteRange(mRemovedStart, mRemovedEnd);
        aEditor->colorize();
    }
    if (!mAdded.isEmpty())
    {
        auto start = mAddedStart;
        aEditor->InsertTextAt(start, mAdded.str());
        aEditor->colorize();
    }
    aEditor->mState = mAfter;
    aEditor->EnsureCursorVisible();
}
