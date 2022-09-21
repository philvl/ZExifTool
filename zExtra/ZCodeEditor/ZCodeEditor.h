#ifndef Z_CODE_EDITOR_H
#define Z_CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QPainter>

class ZCodeEditor : public QPlainTextEdit {
    Q_OBJECT
// METHODS
public:
    explicit ZCodeEditor(QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

// VARIABLES
private:
    QWidget *_lineNumberArea;
};

#endif // Z_CODE_EDITOR_H
