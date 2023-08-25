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

#include <algorithm>
#include <iterator>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>

#include "data.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  QString adjustImagePath(const QString& imagePath, const QString& basePath)
  {
    if( QFileInfo::exists(imagePath) ) {
      return imagePath;
    }

    const QDir dir       = QFileInfo(basePath).absoluteDir();
    const QFileInfo info = QFileInfo(dir, imagePath);
    if( info.exists() ) {
      return info.absoluteFilePath();
    }

    return QString();
  }

  void appendText(QDomDocument& doc, QDomNode& parent,
                  const QString& tag, const QString& text)
  {
    QDomElement xml_elem = doc.createElement(tag);
    parent.appendChild(xml_elem);

    QDomText xml_text = doc.createTextNode(text);
    xml_elem.appendChild(xml_text);
  }

  bool probeBoolAttribute(const QDomElement& elem,
                          const QString& attr, const bool defValue = false)
  {
    if( elem.isNull() ) {
      return defValue;
    }

    const QString defValueStr = defValue
                                ? QStringLiteral("true")
                                : QStringLiteral("false");

    return elem.attribute(attr, defValueStr) == QStringLiteral("true");
  }

  int probeIntAttribute(const QDomElement& elem,
                        const QString& attr, const int defValue = 0, const int base = 10)
  {
    if( elem.isNull() ) {
      return defValue;
    }

    const QString defValueStr = QString::number(defValue, base);

    bool ok             = false;
    const int attrValue = elem.attribute(attr, defValueStr).toInt(&ok, base);
    return ok
           ? attrValue
           : defValue;
  }

  QString readText(const QDomNode& parent, const QString& tag)
  {
    const QDomElement xml_child = parent.firstChildElement(tag);
    return xml_child.isNull()
           ? QString()
           : xml_child.text();
  }

  void assignText(QString& lhs, const QDomNode& parent, const QString& tag)
  {
    const QString text = readText(parent, tag);
    if( !text.isEmpty() ) {
      lhs = text;
    }
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

Quiz::Quiz(const QString& _solution)
{
  solution = _solution.toUpper().simplified();

  if( solution.isEmpty() ) {
    return;
  }

  for( const QChar& c : solution ) {
    if( c.isLetter() && !letters.contains(c) ) {
      letters.push_back(c);
    }
  }
  std::sort(letters.begin(), letters.end());

  for( int i = 0; i < letters.size(); i++ ) {
    const int no = i + 1;
    Question q;
    q.answer   = QStringLiteral("Answer %1").arg(no);
    q.category = QStringLiteral("Category %1").arg(no);
    q.letter   = letters[i];
    q.question = QStringLiteral("Question %1").arg(no);
    questions.push_back(q);
  }

  reset();
}

bool Quiz::isEmpty() const
{
  return letters.isEmpty() || questions.isEmpty() || solution.isEmpty();
}

void Quiz::reset()
{
  constexpr QChar UNDERSCORE = QChar::fromLatin1('_');

  constexpr auto if_letter = [](const QChar& c) -> bool {
    return c.isLetter();
  };

  displayText = solution;
  std::replace_if(displayText.begin(), displayText.end(), if_letter, UNDERSCORE);
}

QString Quiz::solve(const QChar& c)
{
  if( !c.isLetter() ) {
    return displayText;
  }

  const int len = std::min<int>(displayText.size(), solution.size());
  for( int i = 0; i < len; i++ ) {
    if( solution[i] == c.toUpper() ) {
      displayText[i] = solution[i];
    }
  }

  return displayText;
}

void Quiz::write(const QString& filename) const
{
  QDomDocument doc;

  QDomElement xml_root = doc.createElement(QStringLiteral("quiz"));
  doc.appendChild(xml_root);

  priv::appendText(doc, xml_root, QStringLiteral("solution"), solution);

  for( const Question& q : questions ) {
    QDomElement xml_question = doc.createElement(QStringLiteral("question"));
    xml_root.appendChild(xml_question);

    priv::appendText(doc, xml_question, QStringLiteral("answer"), q.answer);
    priv::appendText(doc, xml_question, QStringLiteral("category"), q.category);
    priv::appendText(doc, xml_question, QStringLiteral("question"), q.question);
  }

  QFile file(filename);
  if( !file.open(QIODevice::WriteOnly) ) {
    return;
  }

  QTextStream stream(&file);
  stream << doc.toString();
  stream.flush();
}

Quiz Quiz::read(const QString& filename)
{
  Quiz result;

  QFile file(filename);
  if( !file.open(QIODevice::ReadOnly) ) {
    return Quiz();
  }

  QDomDocument doc;
  if( !doc.setContent(&file) ) {
    return Quiz();
  }

  const QDomElement xml_quiz = doc.firstChildElement(QStringLiteral("quiz"));
  if( xml_quiz.isNull() ) {
    return Quiz();
  }

  result = Quiz(priv::readText(xml_quiz, QStringLiteral("solution")));
  if( result.isEmpty() ) {
    return Quiz();
  }

  result.fontSize = priv::probeIntAttribute(xml_quiz, QStringLiteral("font_size"), DEFAULT_FONTSIZE);

  auto it = result.questions.begin();
  for( QDomElement xml_question = xml_quiz.firstChildElement(QStringLiteral("question"));
       !xml_question.isNull() && it != result.questions.end();
       xml_question = xml_question.nextSiblingElement(QStringLiteral("question")), ++it ) {
    priv::assignText(it->answer, xml_question, QStringLiteral("answer"));
    priv::assignText(it->category, xml_question, QStringLiteral("category"));
    priv::assignText(it->question, xml_question, QStringLiteral("question"));

    it->images.clear();
    for( QDomElement xml_image = xml_question.firstChildElement(QStringLiteral("image"));
         !xml_image.isNull();
         xml_image = xml_image.nextSiblingElement(QStringLiteral("image")) ) {
      Image image;

      image.path    = priv::adjustImagePath(xml_image.text(), filename);
      image.bgColor = xml_image.attribute(QStringLiteral("bg"));
      image.flipH   = priv::probeBoolAttribute(xml_image, QStringLiteral("flip_h"));
      image.flipV   = priv::probeBoolAttribute(xml_image, QStringLiteral("flip_v"));
      image.rotate  = priv::probeIntAttribute(xml_image, QStringLiteral("rotate"));

      if( image.exists() ) {
        it->images.push_back(image);
      }
    } // For Each Image

  } // For Each Question

  return result;
}
