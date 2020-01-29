// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2018 Evgeniy Reizner
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

use svgdom;

error_chain! {
    types {
        Error, ErrorKind, ResultExt, Result;
    }

    links {
        Dom(svgdom::Error, svgdom::ErrorKind) #[doc = "svgdom errors"];
    }

    errors {
        UnresolvedAttribute(attr_name: String) {
            display("failed to resolve attribute '{}'", attr_name)
        }

        MissingAttribute(tag_name: String, attr_name: String) {
            display("the attribute '{}' is missing in the '{}' element", attr_name, tag_name)
        }

        ScriptingIsNotSupported {
            display("scripting is not supported")
        }

        AnimationIsNotSupported {
            display("animation is not supported")
        }

        ConditionalProcessingIsNotSupported {
            display("conditional processing attributes is not supported")
        }

        ExternalHrefIsNotSupported(ref_data: String) {
            display("the 'xlink:href' attribute is referencing an external object '{}', \
                     which is not supported", ref_data)
        }
    }
}
