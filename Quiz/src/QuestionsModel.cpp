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

#include <QtGui/QFont>
#include <QtWidgets/QApplication>

#include "questionsmodel.h"

#include "util.h"
#include "wimageviewer.h"
#include "wquestion.h"

////// public ////////////////////////////////////////////////////////////////

QuestionsModel::QuestionsModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

QuestionsModel::~QuestionsModel()
{
}

QVariant QuestionsModel::data(const QModelIndex& index, int role) const
{
  if(        role == Qt::FontRole ) {
    QFont font = QApplication::font();
    font.setBold(true);
    font.setPointSize(_fontSize);
    return font;

  } else if( role == Qt::TextAlignmentRole ) {
    return int(Qt::AlignHCenter | Qt::AlignVCenter);

  } else if( role == Qt::DisplayRole ) {
    const auto it = std::next(_questions.begin(), index.row());
    return it->category;

  }

  return QVariant();
}

Qt::ItemFlags QuestionsModel::flags(const QModelIndex& /*index*/) const
{
  return Qt::ItemIsEnabled;
}

int QuestionsModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _questions.size();
}

void QuestionsModel::setQuestions(const Quiz& quiz)
{
  beginResetModel();
  _fontSize  = quiz.fontSize;
  _questions = quiz.questions;
  endResetModel();
}

////// public slots //////////////////////////////////////////////////////////

void QuestionsModel::activate(const QModelIndex& index)
{
  if( !index.isValid()  ||  index.row() < 0  ||  index.row() >= _questions.size() ) {
    return;
  }

  auto it = std::next(_questions.begin(), index.row());

  WQuestion d(dynamic_cast<QWidget*>(parent()));
  d.setQuestion(_fontSize, *it);
  d.resize(800, 600);

  if( d.exec() != QDialog::Accepted ) {
    return;
  }

  emit uncovered(it->letter);

  QImage image;
  if( !it->image.isEmpty()  &&  image.load(it->image) ) {
    if(        it->imageRotate != 0 ) {
      image = util::rotated(image, it->imageRotate);
    } else if( it->imageFlipH  ||  it->imageFlipV ) {
      image = image.mirrored(it->imageFlipH, it->imageFlipV);
    }
    WImageViewer *viewer = new WImageViewer(image);
    viewer->showMaximized();
  }

  beginResetModel();
  _questions.erase(it);
  endResetModel();
}
