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

  setupFont(ui->questionBrowser);
  ui->questionBrowser->setText(_question.question);
  ui->questionBrowser->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

////// private slots /////////////////////////////////////////////////////////

void WQuestion::showAnswer()
{
  setupFont(ui->answerBrowser);
  ui->answerBrowser->setText(_question.answer);
  ui->answerBrowser->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

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

void WQuestion::setupFont(QWidget *w) const
{
  QFont f = w->font();
  f.setBold(true);
  f.setPointSize(_fontSize);
  w->setFont(f);
}
