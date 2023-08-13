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

#include <QtWidgets/QFileDialog>

#include "wmainwindow.h"
#include "ui_wmainwindow.h"

#include "questionsmodel.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui(new Ui::WMainWindow)
{
  ui->setupUi(this);

  // Initialization //////////////////////////////////////////////////////////

  _questionsModel = new QuestionsModel(ui->questionsView);
  ui->questionsView->setModel(_questionsModel);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->questionsView, &QListView::activated,
          _questionsModel, &QuestionsModel::activate);
  connect(_questionsModel, &QuestionsModel::uncovered,
          this, &WMainWindow::uncover);

  connect(ui->openAction, &QAction::triggered, this, &WMainWindow::open);
  connect(ui->quitAction, &QAction::triggered, this, &WMainWindow::close);
}

WMainWindow::~WMainWindow()
{
  delete ui;
}

////// public slots //////////////////////////////////////////////////////////

void WMainWindow::open()
{
  const QString filename = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("Quiz (*.xml)"));
  if( filename.isEmpty() ) {
    return;
  }
  const Quiz q = Quiz::read(filename);
  if( q.isEmpty() ) {
    return;
  }
  setupQuiz(q);
}

void WMainWindow::uncover(const QChar& c)
{
  for(int i = 0; i < qMin<int>(_quiz.solution.size(), _uncovered.size()); i++) {
    if( _quiz.solution[i] == c ) {
      _uncovered[i] = c;
    }
  }
  ui->solutionEdit->setText(_uncovered);
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::setupQuiz(const Quiz& quiz)
{
  _quiz = quiz;

  _questionsModel->setQuestions(_quiz);

  _uncovered.fill(QChar::fromLatin1('_'), _quiz.solution.size());
  ui->solutionEdit->setText(_uncovered);
  QFont f = ui->solutionEdit->font();
  f.setBold(true);
  f.setLetterSpacing(QFont::AbsoluteSpacing, 16.0);
  f.setPointSize(_quiz.fontSize);
  ui->solutionEdit->setFont(f);
}
