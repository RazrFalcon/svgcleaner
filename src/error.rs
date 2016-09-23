/****************************************************************************
**
** svgcleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2016 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

use std::fmt;

#[derive(PartialEq)]
pub enum CleanerError {
    UnresolvedAttribute(String), // attribute name
    MissingAttribute(String, String), // tag name, attribute name
    ScriptingIsNotSupported,
    AnimationIsNotSupported,
    ConditionalProcessingIsNotSupported,
    ExternalHrefIsNotSupported(String), // ref data
    BiggerFile,
}

impl fmt::Debug for CleanerError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            CleanerError::UnresolvedAttribute(ref name) =>
                write!(f, "Failed to resolved attribute '{}'", name),
            CleanerError::MissingAttribute(ref tag_name, ref attr_name) =>
                write!(f, "The attribute '{}' is missing in the '{}' element", attr_name, tag_name),
            CleanerError::ScriptingIsNotSupported =>
                write!(f, "Scripting is not supported"),
            CleanerError::AnimationIsNotSupported =>
                write!(f, "Animation is not supported"),
            CleanerError::BiggerFile =>
                write!(f, "Cleaned file is bigger than original"),
            CleanerError::ConditionalProcessingIsNotSupported =>
                write!(f, "Conditional processing attributes is not supported"),
            CleanerError::ExternalHrefIsNotSupported(ref s) =>
                write!(f, "The 'xlink:href' attribute is referencing an external object '{}', \
                           which is not supported", s),
        }
    }
}
