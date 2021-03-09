#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <iostream>
#include "imgui.h"

#include <CText.h>
#include <functional>
#include <algorithm>
// for syntax highlighting
#include<common/Vector.h>
#include <colorer/viewer/TextLinesStore.h>
#include <colorer/editor/BaseEditor.h>
typedef unsigned short      WORD;

void insert_chars(String*& line, wchar* chars, size_t chars_len, size_t aPos);
void append_char(String*& line, wchar c);
void prepend_char(String*& line, wchar c);
void erase_chars(String*& line, size_t aStart, size_t aEnd);
void overwrite_chars(String*& line, wchar* chars, size_t chars_len, size_t aPos);

class TextEditor
{
public:
	// Шрифт содержимого редактора
	ImFont* regular_font;
	ImFont* bold_font;
	// Режим выделения
	enum class SelectionMode
	{
		Normal,
		Word,
		Line
	};
	// Координаты текстового курсора
	struct Coordinates
	{
		// Строка и столбец
		int mLine, mColumn;
		// Инициализируем нулями
		Coordinates() : mLine(0), mColumn(0) {}
		// Инициализируем значениями
		Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn)
		{
			assert(aLine >= 0);
			assert(aColumn >= 0);
		}
		// Нкправильные координаты
		static Coordinates Invalid() 
		{
			static Coordinates invalid(-1, -1);
			return invalid;
		}
		// Равенство
		bool operator ==(const Coordinates& o) const
		{
			return mLine == o.mLine && mColumn == o.mColumn;
		}
		// Неравенство
		bool operator !=(const Coordinates& o) const
		{
			return mLine != o.mLine || mColumn != o.mColumn;
		}
		// Меньше
		bool operator <(const Coordinates& o) const
		{
			if (mLine != o.mLine)
			{
				return mLine < o.mLine;
			}
			return mColumn < o.mColumn;
		}
		// больше
		bool operator >(const Coordinates& o) const
		{
			if (mLine != o.mLine)
			{
				return mLine > o.mLine;
			}
			return mColumn > o.mColumn;
		}
		// меньше или равно
		bool operator <=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
			{
				return mLine < o.mLine;
			}
			return mColumn <= o.mColumn;
		}
		// больше или равно
		bool operator >=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
			{
				return mLine > o.mLine;
			}
			return mColumn >= o.mColumn;
		}
	};
	
	struct Glyph
	{
		CText::Char mChar;
		ImVec4 mColor;
		bool mBold;
		Glyph(ImWchar aChar, ImVec4 aColor, bool aBold=false) : mChar(aChar), mColor(aColor),mBold(aBold) {}
	};

	typedef std::vector<Glyph> Line;
	typedef std::vector<Line> Lines;
	TextLinesStore textLinesStore;
	std::vector < std::vector<WORD> > colors;
	std::vector < std::vector<WORD> > styles;
	TextLinesStore* textLinesStore_ptr;
	BaseEditor* baseEditor_ptr;
	ParserFactory* pf_ptr;
	void colorize(void);
	//std::function<void(TextEditor*,Lines&)> Colorize;
	Lines mLines;

	TextEditor();
	~TextEditor();

	void Render(const CText::Char* aTitle, const ImVec2& aSize = ImVec2(), bool aBorder = false);
	void SetText(const CText& aText);
	void SetTextLines(const std::vector<CText>& aLines);
	CText GetText() const;
	std::vector<CText> GetTextLines() const;
	CText GetSelectedText() const;
	CText GetCurrentLineText()const;
	
	int GetTotalLines() const { return (int)mLines.size(); }
	bool IsOverwrite() const { return mOverwrite; }

	void SetReadOnly(bool aValue);
	bool IsReadOnly() const { return mReadOnly; }
	bool IsTextChanged() const { return mTextChanged; }
	bool IsCursorPositionChanged() const { return mCursorPositionChanged; }

	Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }
	void SetCursorPosition(const Coordinates& aPosition);

	void InsertText(const CText& aValue);
	void InsertText(const CText::Char* aValue);

	void MoveUp(int aAmount = 1, bool aSelect = false);
	void MoveDown(int aAmount = 1, bool aSelect = false);
	void MoveLeft(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveRight(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveTop(bool aSelect = false);
	void MoveBottom(bool aSelect = false);
	void MoveHome(bool aSelect = false);
	void MoveEnd(bool aSelect = false);

	void SetSelectionStart(const Coordinates& aPosition);
	void SetSelectionEnd(const Coordinates& aPosition);
	void SetSelection(const Coordinates& aStart, const Coordinates& aEnd, SelectionMode aMode = SelectionMode::Normal);
	void SelectWordUnderCursor();
	void SelectAll();
	bool HasSelection() const;

	void Copy();
	void Cut();
	void Paste();
	void Delete();

	bool CanUndo() const;
	bool CanRedo() const;
	void Undo(int aSteps = 1);
	void Redo(int aSteps = 1);

	int lineStart = 0;
	int lineMax = 0;
private:

	struct EditorState
	{
		Coordinates mSelectionStart;
		Coordinates mSelectionEnd;
		Coordinates mCursorPosition;
	};

	class UndoRecord
	{
	public:
		UndoRecord() {}
		~UndoRecord() {}

		UndoRecord(
			const CText& aAdded,
			const TextEditor::Coordinates aAddedStart, 
			const TextEditor::Coordinates aAddedEnd, 
			
			const CText& aRemoved, 
			const TextEditor::Coordinates aRemovedStart,
			const TextEditor::Coordinates aRemovedEnd,
			
			TextEditor::EditorState& aBefore, 
			TextEditor::EditorState& aAfter);

		void Undo(TextEditor* aEditor);
		void Redo(TextEditor* aEditor);

		CText mAdded;
		Coordinates mAddedStart;
		Coordinates mAddedEnd;

		CText mRemoved;
		Coordinates mRemovedStart;
		Coordinates mRemovedEnd;

		EditorState mBefore;
		EditorState mAfter;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

	void ProcessInputs();
	float TextDistanceToLineStart(const Coordinates& aFrom) const;
	void EnsureCursorVisible();
	int GetPageSize() const;
	int AppendBuffer(CText& aBuffer, CText::Char chr, int aIndex);
	CText GetText(const Coordinates& aStart, const Coordinates& aEnd) const;
	Coordinates GetActualCursorCoordinates() const;
	Coordinates SanitizeCoordinates(const Coordinates& aValue) const;
	void Advance(Coordinates& aCoordinates) const;
	void DeleteRange(const Coordinates& aStart, const Coordinates& aEnd);
	int InsertTextAt(Coordinates& aWhere, const CText::Char* aValue);
	void AddUndo(UndoRecord& aValue);
	Coordinates ScreenPosToCoordinates(const ImVec2& aPosition) const;
	Coordinates FindWordStart(const Coordinates& aFrom) const;
	Coordinates FindWordEnd(const Coordinates& aFrom) const;
	//bool IsOnWordBoundary(const Coordinates& aAt) const;
	void RemoveLine(int aStart, int aEnd);
	void RemoveLine(int aIndex);
	Line& InsertLine(int aIndex);
	void EnterCharacter(CText::Char aChar, bool aShift);
	void BackSpace();
	void DeleteSelection();
	CText GetWordUnderCursor() const;
	CText GetWordAt(const Coordinates& aCoords) const;
	ImU32 GetGlyphColor(const Glyph& aGlyph) const;

	void HandleKeyboardInputs();
	void HandleMouseInputs();
	void Render();

	float mLineSpacing;
	
	EditorState mState;
	UndoBuffer mUndoBuffer;
	int mUndoIndex;
	
	int mTabSize;
	bool mOverwrite;
	bool mReadOnly;
	bool mWithinRender;
	bool mScrollToCursor;
	bool mScrollToTop;
	bool mTextChanged;
	float  mTextStart;                   // position (in pixels) where a code line starts relative to the left of the TextEditor.
	int  mLeftMargin;
	bool mCursorPositionChanged;
	int mColorRangeMin, mColorRangeMax;
	SelectionMode mSelectionMode;
	ImVec2 mCharAdvance;
	Coordinates mInteractiveStart, mInteractiveEnd;
	

	float mLastClick;
};
