/****************************************************************************
** Copyright (c) 2023, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "wquestion.h"
#include "ui_wquestion.h"

////// Private ///////////////////////////////////////////////////////////////

namespace impl {

  void setupDocument(QTextDocument *doc, const bool font_bold, const int font_size)
  {
    doc->clear();

    // (1) Font //////////////////////////////////////////////////////////////

    QFont font = doc->defaultFont();
    font.setBold(font_bold);
    if( font_size > 0 ) {
      font.setPointSize(font_size);
    }
    doc->setDefaultFont(font);

    // (2) Layout ////////////////////////////////////////////////////////////

    QTextOption opt = doc->defaultTextOption();
    opt.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(opt);
  }

} // namespace impl

////// public ////////////////////////////////////////////////////////////////

WQuestion::WQuestion(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WQuestion)
{
  ui->setupUi(this);

  // Setup UI ////////////////////////////////////////////////////////////////

  enableOk(false);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->answerButton, &QPushButton::clicked, this, &WQuestion::showAnswer);
}

WQuestion::~WQuestion()
{
}

void WQuestion::setQuestion(const int fontSize, const Question& q)
{
  enableOk(false);
  ui->answerBrowser->clear();
  ui->questionBrowser->clear();

  _fontSize = fontSize;
  _question = q;

  impl::setupDocument(ui->questionBrowser->document(), true, _fontSize);
  ui->questionBrowser->setHtml(_question.question);
}

////// private slots /////////////////////////////////////////////////////////

void WQuestion::showAnswer()
{
  impl::setupDocument(ui->answerBrowser->document(), true, _fontSize);
  ui->answerBrowser->setHtml(_question.answer);

  enableOk(true);
}

////// private ///////////////////////////////////////////////////////////////

void WQuestion::enableOk(const bool enable)
{
  QPushButton *b = ui->buttonBox->button(QDialogButtonBox::Ok);
  if( b != nullptr ) {
    b->setEnabled(enable);
  }
}
