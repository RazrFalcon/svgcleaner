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

pub use self::preproc::prepare_transforms;
pub use self::gradients::apply_transform_to_gradients;
pub use self::shapes::apply_transform_to_shapes;

mod preproc;
mod gradients;
mod shapes;

pub mod utils {
    use svgdom::{
        Attributes,
        AttributeValue,
        Node,
    };
    use svgdom::types::{
        Length,
        Transform,
    };

    use task::short::{EId, AId, Unit};

    // TODO: remove
    pub fn has_valid_transform(node: &Node) -> bool {
        if !node.has_attribute(AId::Transform) {
            return true;
        }

        let ts = get_ts(node);
        is_valid_transform(&ts)
    }

    pub fn is_valid_transform(ts: &Transform) -> bool {
        // If transform has non-proportional scale - we should skip it,
        // because it can be applied only to a raster.
        if ts.has_scale() && !ts.has_proportional_scale() {
            return false;
        }

        // TODO: is rotate supported?

        // If transform has skew part - we should skip it,
        // because it can be applied only to a raster.
        if ts.has_skew() {
            return false;
        }

        true
    }

    // Element shouldn't have any linked elements, because they also must be transformed.
    // TODO: process 'fill', 'stroke' and 'filter' linked elements only if they
    //       used only by this element.
    pub fn is_valid_attrs(node: &Node) -> bool {
        for aid in &[AId::Fill, AId::Stroke, AId::Filter, AId::Mask, AId::ClipPath] {
            if !is_valid_attr(node, *aid) {
                return false;
            }
        }

        true
    }

    // Checks that first occurred attribute value is not a FuncLink.
    fn is_valid_attr(node: &Node, aid: AId) -> bool {
        for parent in node.parents_with_self() {
            let attrs = parent.attributes();

            match attrs.get_value(aid) {
                Some(&AttributeValue::FuncLink(_)) => return false,
                Some(_) => return true,
                _ => {}
            }
        }

        true
    }

    // We can process only coordinates without units.
    pub fn is_valid_coords(node: &Node) -> bool {
        match node.tag_id().unwrap() {
            EId::Rect => _is_valid_coords(node, &[AId::X, AId::Y]),
            EId::Circle |
            EId::Ellipse => _is_valid_coords(node, &[AId::Cx, AId::Cy]),
            EId::Line |
            EId::LinearGradient => _is_valid_coords(node, &[AId::X1, AId::Y1, AId::X2, AId::Y2]),
            EId::RadialGradient => _is_valid_coords(node, &[AId::Cx, AId::Cy, AId::Fx, AId::Fy, AId::R]),
            _ => false,
        }
    }

    fn _is_valid_coords(node: &Node, attr_ids: &[AId]) -> bool {
        let attrs = node.attributes();

        fn is_valid_coord(attrs: &Attributes, aid: AId) -> bool {
            if let Some(&AttributeValue::Length(v)) = attrs.get_value(aid) {
                v.unit == Unit::None
            } else {
                true
            }
        }

        for id in attr_ids {
            if !is_valid_coord(&attrs, *id) {
                return false;
            }
        }

        true
    }

    pub fn transform_coords(attrs: &mut Attributes, aid_x: AId, aid_y: AId, ts: &Transform) {
        let x = get_value!(attrs, Length, aid_x, Length::zero());
        let y = get_value!(attrs, Length, aid_y, Length::zero());

        debug_assert_eq!(x.unit, Unit::None);
        debug_assert_eq!(y.unit, Unit::None);

        let (nx, ny) = ts.apply(x.num, y.num);
        attrs.insert_from(aid_x, (nx, Unit::None));
        attrs.insert_from(aid_y, (ny, Unit::None));
    }

    pub fn scale_coord(attrs: &mut Attributes, aid: AId, scale_factor: &f64) {
        if let Some(&mut AttributeValue::Length(ref mut len)) = attrs.get_value_mut(aid) {
            len.num *= *scale_factor;
        }
    }

    // TODO: remove
    pub fn get_ts(node: &Node) -> Transform {
        match node.attributes().get_value(AId::Transform).cloned() {
            Some(AttributeValue::Transform(ts)) => ts,
            _ => unreachable!("attribute must be resolved"),
        }
    }
}
