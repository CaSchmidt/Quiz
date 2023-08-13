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

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include "wimageviewer.h"

////// public ////////////////////////////////////////////////////////////////

WImageViewer::WImageViewer(const QImage& image, QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , _image(image)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setWindowTitle(QStringLiteral("Image"));
}

WImageViewer::~WImageViewer()
{
}

////// protected /////////////////////////////////////////////////////////////

void WImageViewer::keyPressEvent(QKeyEvent *event)
{
  if( event->key() == Qt::Key_Escape ) {
    close();
  }
}

void WImageViewer::paintEvent(QPaintEvent * /*event*/)
{
  const QImage image = _image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  const int     offx = (width()  - image.width() )/2;
  const int     offy = (height() - image.height())/2;

  QPainter painter(this);
  painter.fillRect(0, 0, width(), height(), Qt::black);
  painter.drawImage(offx, offy, image);
}
