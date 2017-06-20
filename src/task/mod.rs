/****************************************************************************
**
** svgcleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2017 Evgeniy Reizner
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

pub use self::conv_shapes::convert_shapes_to_paths;
pub use self::final_fixes::{
    remove_empty_defs,
    fix_xmlns_attribute,
};
pub use self::fix_attrs::fix_invalid_attributes;
pub use self::group_by_style::group_by_style;
pub use self::group_defs::group_defs;
pub use self::join_style_attrs::join_style_attributes;
pub use self::merge_gradients::merge_gradients;
pub use self::preclean_checks::preclean_checks;
pub use self::rm_default_attrs::remove_default_attributes;
pub use self::rm_dupl_defs::{
    remove_dupl_linear_gradients,
    remove_dupl_radial_gradients,
    remove_dupl_fe_gaussian_blur,
};
pub use self::regroup_gradient_stops::regroup_gradient_stops;
pub use self::resolve_use::resolve_use;
pub use self::rm_elems::remove_element;
pub use self::rm_gradient_attrs::remove_gradient_attributes;
pub use self::rm_invalid_stops::remove_invalid_stops;
pub use self::rm_invisible_elements::remove_invisible_elements;
pub use self::rm_needless_attrs::remove_needless_attributes;
pub use self::rm_text_attrs::remove_text_attributes;
pub use self::rm_unref_ids::remove_unreferenced_ids;
pub use self::rm_unused_coords::remove_unused_coordinates;
pub use self::rm_unused_defs::remove_unused_defs;
pub use self::rm_version::remove_version;
pub use self::trim_ids::trim_ids;
pub use self::ungroup_defs::ungroup_defs;
pub use self::ungroup_groups::ungroup_groups;
pub use self::round_numbers::round_numbers;

#[macro_use] mod macros;
mod conv_shapes;
mod final_fixes;
mod fix_attrs;
mod group_by_style;
mod group_defs;
mod join_style_attrs;
mod merge_gradients;
mod preclean_checks;
mod regroup_gradient_stops;
mod resolve_use;
mod rm_default_attrs;
mod rm_dupl_defs;
mod rm_elems;
mod rm_gradient_attrs;
mod rm_invalid_stops;
mod rm_invisible_elements;
mod rm_needless_attrs;
mod rm_text_attrs;
mod rm_unref_ids;
mod rm_unused_coords;
mod rm_unused_defs;
mod rm_version;
mod trim_ids;
mod ungroup_defs;
mod ungroup_groups;
mod round_numbers;
pub mod apply_transforms;
pub mod paths;

// Shorthand names for modules.
mod short {
    pub use svgdom::types::LengthUnit as Unit;
    pub use svgdom::ElementId as EId;
    pub use svgdom::AttributeId as AId;
}

pub mod utils {
    use svgdom::{Document, Node, AttributeValue, Error as SvgDomError};
    use svgdom::types::FuzzyEq;
    use svgdom::postproc;
    use task::short::AId;

    pub fn resolve_gradient_attributes(doc: &Document) -> Result<(), SvgDomError> {
        postproc::resolve_linear_gradient_attributes(doc);
        postproc::resolve_radial_gradient_attributes(doc);
        postproc::resolve_stop_attributes(doc)?;
        Ok(())
    }

    pub fn recalc_stroke(node: &Node, scale_factor: f64) {
        recalc_stroke_num(node, AId::StrokeWidth, scale_factor);
        recalc_stroke_dasharray(node, scale_factor);
        recalc_stroke_num(node, AId::StrokeDashoffset, scale_factor);
    }

    fn recalc_stroke_num(node: &Node, aid: AId, scale_factor: f64) {
        // Resolve current value.
        let value = if let Some(attr) = node.attributes().get(aid).cloned() {
            // Defined in the current node.
            attr.value
        } else {
            if let Some(n) = node.parents().find(|n| n.has_attribute(aid)) {
                // Defined in the parent node.
                n.attributes().get_value(aid).cloned().unwrap()
            } else {
                // Default value.
                AttributeValue::default_value(aid).unwrap()
            }
        };

        let mut len = *value.as_length().unwrap();
        if len.num.fuzzy_ne(&0.0) {
            len.num *= scale_factor;
            node.set_attribute((aid, len));
        }
    }

    fn recalc_stroke_dasharray(node: &Node, scale_factor: f64) {
        let aid = AId::StrokeDasharray;

        // Resolve current 'stroke-dasharray'.
        let value = if let Some(attr) = node.attributes().get(aid).cloned() {
            // Defined in the current node.
            Some(attr.value)
        } else {
            if let Some(n) = node.parents().find(|n| n.has_attribute(aid)) {
                // Defined in the parent node.
                Some(n.attributes().get_value(aid).cloned().unwrap())
            } else {
                None
            }
        };

        if let Some(value) = value {
            if let AttributeValue::LengthList(mut list) = value {
                for n in &mut list {
                    n.num *= scale_factor
                }
                node.set_attribute((aid, list));
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::short::EId;
    use svgdom::{Document, WriteToString};
    use task::utils;

    macro_rules! test {
        ($name:ident, $in_text:expr, $out_text:expr) => (
            #[test]
            fn $name() {
                let doc = Document::from_str($in_text).unwrap();
                let node = doc.descendants().find(|n| n.is_tag_name(EId::Path)).unwrap();
                utils::recalc_stroke(&node, 2.0);
                assert_eq_text!(doc.to_string_with_opt(&write_opt_for_tests!()), $out_text);
            }
        )
    }

    // From default.
    test!(recalc_stroke_1,
"<svg>
    <path/>
</svg>",
"<svg>
    <path stroke-width='2'/>
</svg>
");

    // From current.
    test!(recalc_stroke_2,
"<svg>
    <path stroke-width='2'/>
</svg>",
"<svg>
    <path stroke-width='4'/>
</svg>
");

    // From parent.
    test!(recalc_stroke_3,
"<svg stroke-width='2'>
    <path />
</svg>",
"<svg stroke-width='2'>
    <path stroke-width='4'/>
</svg>
");

    test!(recalc_stroke_4,
"<svg>
    <path stroke-dasharray='5 2 5 5 2 5'/>
</svg>",
"<svg>
    <path stroke-dasharray='10 4 10 10 4 10' stroke-width='2'/>
</svg>
");

    test!(recalc_stroke_5,
"<svg>
    <path stroke-dashoffset='2'/>
</svg>",
"<svg>
    <path stroke-dashoffset='4' stroke-width='2'/>
</svg>
");

    test!(recalc_stroke_6,
"<svg>
    <path stroke-dasharray='5 2 5 5 2 5' stroke-dashoffset='2'/>
</svg>",
"<svg>
    <path stroke-dasharray='10 4 10 10 4 10' stroke-dashoffset='4' stroke-width='2'/>
</svg>
");

}
