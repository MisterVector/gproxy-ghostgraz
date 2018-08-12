#include "ClickableLineEdit.h"

ClickableLineEdit::ClickableLineEdit (QWidget *parent) : QLineEdit (parent) { }

ClickableLineEdit::~ClickableLineEdit () { }

void ClickableLineEdit::mousePressEvent (QMouseEvent *event)
{
    emit clicked();
}

void ClickableLineEdit::focusInEvent (QFocusEvent *event)
{
    clearFocus();
}