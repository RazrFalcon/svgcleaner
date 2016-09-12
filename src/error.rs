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
    UnresolvedAttribute, // TODO: which one
    ScriptingIsNotSupported,
    AnimationIsNotSupported,
    ConditionalProcessingIsNotSupported,
    BiggerFile,
}

impl fmt::Debug for CleanerError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            CleanerError::UnresolvedAttribute =>
                write!(f, "Unresolved attribute"),
            CleanerError::ScriptingIsNotSupported =>
                write!(f, "Scripting is not supported"),
            CleanerError::AnimationIsNotSupported =>
                write!(f, "Animation is not supported"),
            CleanerError::BiggerFile =>
                write!(f, "Cleaned file is bigger than original"),
            CleanerError::ConditionalProcessingIsNotSupported =>
                write!(f, "Conditional processing attributes is not supported"),
        }
    }
}
