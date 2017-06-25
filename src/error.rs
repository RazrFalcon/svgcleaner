// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2017 Evgeniy Reizner
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

use std::fmt;

use svgdom;

#[derive(PartialEq)]
pub enum Error {
    UnresolvedAttribute(String), // attribute name
    MissingAttribute(String, String), // tag name, attribute name
    ScriptingIsNotSupported,
    AnimationIsNotSupported,
    ConditionalProcessingIsNotSupported,
    ExternalHrefIsNotSupported(String), // ref data
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Error::UnresolvedAttribute(ref name) =>
                write!(f, "Failed to resolved attribute '{}'", name),
            Error::MissingAttribute(ref tag_name, ref attr_name) =>
                write!(f, "The attribute '{}' is missing in the '{}' element", attr_name, tag_name),
            Error::ScriptingIsNotSupported =>
                write!(f, "Scripting is not supported"),
            Error::AnimationIsNotSupported =>
                write!(f, "Animation is not supported"),
            Error::ConditionalProcessingIsNotSupported =>
                write!(f, "Conditional processing attributes is not supported"),
            Error::ExternalHrefIsNotSupported(ref s) =>
                write!(f, "The 'xlink:href' attribute is referencing an external object '{}', \
                           which is not supported", s),
        }
    }
}

impl From<svgdom::Error> for Error {
    fn from(value: svgdom::Error) -> Error {
        match value {
            svgdom::Error::MissingAttribute(tag_name, attr_name) =>
                    Error::MissingAttribute(tag_name, attr_name),
            _ => unreachable!(),
        }
    }
}
