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

#include <iterator>

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include "wimageviewer.h"

////// public ////////////////////////////////////////////////////////////////

WImageViewer::WImageViewer(const Images& images, QWidget *parent, Qt::WindowFlags f)
  : QWidget{parent, f}
  , _images{images}
{
  if( parentWidget() == nullptr ) {
    setAttribute(Qt::WA_DeleteOnClose, true);
  }
  setAttribute(Qt::WA_OpaquePaintEvent, true);

  _pos = _images.cbegin();
  updateImage();
}

WImageViewer::~WImageViewer()
{
}

////// protected /////////////////////////////////////////////////////////////

void WImageViewer::keyPressEvent(QKeyEvent *event)
{
  if( event->key() == Qt::Key_Escape ) {
    if( parentWidget() == nullptr ) {
      close();
    }
  } else if( event->key() == Qt::Key_Backspace ) {
    if( !isEmpty() && !isBegin() ) {
      _pos = std::prev(_pos);
      updateImage();
    }
  } else if( event->key() == Qt::Key_Space ) {
    if( !isEmpty() && std::next(_pos) != _images.cend() ) {
      _pos = std::next(_pos);
      updateImage();
    }
  }
}

void WImageViewer::paintEvent(QPaintEvent * /*event*/)
{
  QPainter painter(this);
  painter.fillRect(0, 0, width(), height(), _bgColor);

  if( _image.isNull() ) {
    return;
  }

  const QImage image = _image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  const int offx     = (width() - image.width()) / 2;
  const int offy     = (height() - image.height()) / 2;

  painter.drawImage(offx, offy, image);
}

////// private ///////////////////////////////////////////////////////////////

bool WImageViewer::isBegin() const
{
  return _pos == _images.cbegin();
}

bool WImageViewer::isEmpty() const
{
  return _images.empty();
}

void WImageViewer::updateImage()
{
  if( !isEmpty() ) {
    QString title;
    title += QStringLiteral("Image");
    if( _images.size() > 1 ) {
      const auto i = std::distance(_images.cbegin(), _pos);

      title += QStringLiteral(" %1 of %2").arg(i + 1).arg(_images.size());
    }
    title += QStringLiteral(" - [%1]").arg(_pos->fileName());
    setWindowTitle(title);

    _bgColor.setNamedColor(_pos->bgColor);
    if( !_bgColor.isValid() ) {
      _bgColor = Qt::black;
    }

    _image = _pos->load();
  } else {
    setWindowTitle(QStringLiteral("No Image"));

    _bgColor = Qt::black;
  }
  update();
}
