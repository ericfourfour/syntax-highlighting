/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "format.h"
#include "theme.h"
#include "xml_p.h"

#include <QColor>
#include <QDebug>
#include <QXmlStreamReader>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {
class FormatPrivate
{
public:
    FormatPrivate();
    QString name;
    QColor color;
    QColor backgroundColor;
    TextStyle defaultStyle;
    bool italic;
    bool bold;
    bool underline;
    bool strikeout;
    bool spellCheck;

    // flags indicating wheter the itemData contains this info
    bool hasItalic;
    bool hasBold;
    bool hasUnderline;
    bool hasStrikeout;
    bool hasColor;
    bool hasSelColor;
    bool hasBgColor;
};
}

static TextStyle stringToDefaultFormat(const QStringRef &str)
{
#define D(type) if (str == QLatin1String("ds" #type)) return type;
    D(Normal)
    D(Keyword)
    D(Function)
    D(Variable)
    D(ControlFlow)
    D(Operator)
    D(BuiltIn)
    D(Extension)
    D(Preprocessor)
    D(Attribute)
    D(Char)
    D(SpecialChar)
    D(String)
    D(VerbatimString)
    D(SpecialString)
    D(Import)
    D(DataType)
    D(DecVal)
    D(BaseN)
    D(Float)
    D(Constant)
    D(Comment)
    D(Documentation)
    D(Annotation)
    D(CommentVar)
    D(RegionMarker)
    D(Information)
    D(Warning)
    D(Alert)
    D(Error)
    D(Others)
#undef D
    return Normal;
}

FormatPrivate::FormatPrivate()
    : defaultStyle(Normal)
    , italic(false)
    , bold(false)
    , underline(false)
    , strikeout(false)
    , spellCheck(true)
    , hasItalic(false)
    , hasBold(false)
    , hasUnderline(false)
    , hasStrikeout(false)
    , hasColor(false)
    , hasSelColor(false)
    , hasBgColor(false)
{
}

Format::Format() : d(new FormatPrivate)
{
}

Format::~Format()
{
}

bool Format::isValid() const
{
    return !d->name.isEmpty();
}

QString Format::name() const
{
    return d->name;
}

bool Format::isNormal(const Theme &theme) const
{
    return !hasColor(theme) && !d->hasSelColor && !hasBackgroundColor(theme)
        && !isBold(theme) && !isItalic(theme) && !isUnderline(theme) && !isStrikeThrough(theme);
}

bool Format::hasColor(const Theme &theme) const
{
    return d->hasColor || (theme.textColor(d->defaultStyle) & 0xff000000);
}

QColor Format::color(const Theme &theme) const
{
    return d->hasColor ? d->color : QColor(theme.textColor(d->defaultStyle));
}

bool Format::hasBackgroundColor(const Theme &theme) const
{
    return d->hasBgColor || (theme.backgroundColor(d->defaultStyle) & 0xff000000);
}

QColor Format::backgroundColor(const Theme &theme) const
{
    return d->hasBgColor ? d->backgroundColor
                         : QColor(theme.backgroundColor(d->defaultStyle));
}

bool Format::isBold(const Theme &theme) const
{
    return d->hasBold ? d->bold : theme.isBold(d->defaultStyle);
}

bool Format::isItalic(const Theme &theme) const
{
    return d->hasItalic ? d->italic : theme.isItalic(d->defaultStyle);
}

bool Format::isUnderline(const Theme &theme) const
{
    return d->hasUnderline ? d->underline : theme.isUnderline(d->defaultStyle);
}

bool Format::isStrikeThrough(const Theme &theme) const
{
    return d->hasStrikeout ? d->strikeout : theme.isStrikeThrough(d->defaultStyle);
}

bool Format::spellCheck() const
{
    return d->spellCheck;
}

void Format::load(QXmlStreamReader& reader)
{
    d->name = reader.attributes().value(QStringLiteral("name")).toString();
    d->defaultStyle = stringToDefaultFormat(reader.attributes().value(QStringLiteral("defStyleNum")));

    QStringRef ref = reader.attributes().value(QStringLiteral("color"));
    if (!ref.isEmpty()) {
        d->hasColor = true;
        d->color = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("backgroundColor"));
    if (!ref.isEmpty()) {
        d->hasBgColor = true;
        d->backgroundColor = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("italic"));
    if (!ref.isEmpty()) {
        d->hasItalic = true;
        d->italic = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("bold"));
    if (!ref.isEmpty()) {
        d->hasBold = true;
        d->bold = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("underline"));
    if (!ref.isEmpty()) {
        d->hasUnderline = true;
        d->underline = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("strikeOut"));
    if (!ref.isEmpty()) {
        d->hasStrikeout = true;
        d->strikeout = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("spellChecking"));
    if (!ref.isEmpty()) {
        d->spellCheck = Xml::attrToBool(ref);
    }
}
