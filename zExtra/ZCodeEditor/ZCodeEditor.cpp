#include "ZCodeEditor.h"

// Qt Gui
#include <QTextBlock>

//---
// Internal LineNumberArea class declaration & implementation
//---
class LineNumberArea : public QWidget {
public:
    LineNumberArea(ZCodeEditor *editor) : QWidget(editor) {
        _codeEditor= editor;
    }

    QSize sizeHint() const override {
        return QSize(_codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        _codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    ZCodeEditor *_codeEditor;
};



//---
// ZCodeEditor
//---
ZCodeEditor::ZCodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    // Use the default system Fixed Font
    const QFont fixedFont= QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(fixedFont);

    // Create line number area
    _lineNumberArea= new LineNumberArea(this);
    updateLineNumberAreaWidth(0);

    connect(this, &ZCodeEditor::blockCountChanged, this, &ZCodeEditor::updateLineNumberAreaWidth);
    connect(this, &ZCodeEditor::updateRequest, this, &ZCodeEditor::updateLineNumberArea);
}

int ZCodeEditor::lineNumberAreaWidth() {
    int digits= qMax(2, QString::number(blockCount()).size());
    int space= 4 + fontMetrics().horizontalAdvance(QLatin1Char('0')) * digits + 4;
    return space;
}

void ZCodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ZCodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        _lineNumberArea->scroll(0, dy);
    else
        _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void ZCodeEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr= contentsRect();
    _lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ZCodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(_lineNumberArea);
    QPalette palette;
    painter.fillRect(event->rect(), palette.color(QPalette::Window));

    QTextBlock block= firstVisibleBlock();
    int blockNumber= block.blockNumber();
    int top= qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom= top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number= QString::number(blockNumber + 1);
            painter.setPen(palette.color(QPalette::Mid));
            painter.drawText(0, top, _lineNumberArea->width()-4, fontMetrics().height(), Qt::AlignRight, number);
        }

        block= block.next();
        top= bottom;
        bottom= top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
