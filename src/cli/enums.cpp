/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2015 Evgeniy Reizner
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

#include "enums.h"

const QString UrlPrefix = QL1S("url(");

namespace Element {
    const QString E_a                   = QL1S("a");
    const QString E_altGlyph            = QL1S("altGlyph");
    const QString E_altGlyphDef         = QL1S("altGlyphDef");
    const QString E_altGlyphItem        = QL1S("altGlyphItem");
    const QString E_animate             = QL1S("animate");
    const QString E_animateColor        = QL1S("animateColor");
    const QString E_animateMotion       = QL1S("animateMotion");
    const QString E_animateTransform    = QL1S("animateTransform");
    const QString E_circle              = QL1S("circle");
    const QString E_clipPath            = QL1S("clipPath");
    const QString E_color_profile       = QL1S("color-profile");
    const QString E_cursor              = QL1S("cursor");
    const QString E_defs                = QL1S("defs");
    const QString E_desc                = QL1S("desc");
    const QString E_ellipse             = QL1S("ellipse");
    const QString E_feBlend             = QL1S("feBlend");
    const QString E_feColorMatrix       = QL1S("feColorMatrix");
    const QString E_feComponentTransfer = QL1S("feComponentTransfer");
    const QString E_feComposite         = QL1S("feComposite");
    const QString E_feConvolveMatrix    = QL1S("feConvolveMatrix");
    const QString E_feDiffuseLighting   = QL1S("feDiffuseLighting");
    const QString E_feDisplacementMap   = QL1S("feDisplacementMap");
    const QString E_feDistantLight      = QL1S("feDistantLight");
    const QString E_feFlood             = QL1S("feFlood");
    const QString E_feGaussianBlur      = QL1S("feGaussianBlur");
    const QString E_feImage             = QL1S("feImage");
    const QString E_feMerge             = QL1S("feMerge");
    const QString E_feMergeNode         = QL1S("feMergeNode");
    const QString E_feMorphology        = QL1S("feMorphology");
    const QString E_feOffset            = QL1S("feOffset");
    const QString E_fePointLight        = QL1S("fePointLight");
    const QString E_feSpecularLighting  = QL1S("feSpecularLighting");
    const QString E_feSpotLight         = QL1S("feSpotLight");
    const QString E_feTile              = QL1S("feTile");
    const QString E_feTurbulence        = QL1S("feTurbulence");
    const QString E_filter              = QL1S("filter");
    const QString E_font                = QL1S("font");
    const QString E_font_face           = QL1S("font-face");
    const QString E_font_face_format    = QL1S("font-face-format");
    const QString E_font_face_name      = QL1S("font-face-name");
    const QString E_font_face_src       = QL1S("font-face-src");
    const QString E_font_face_uri       = QL1S("font-face-uri");
    const QString E_foreignObject       = QL1S("foreignObject");
    const QString E_g                   = QL1S("g");
    const QString E_glyph               = QL1S("glyph");
    const QString E_glyphRef            = QL1S("glyphRef");
    const QString E_hkern               = QL1S("hkern");
    const QString E_image               = QL1S("image");
    const QString E_line                = QL1S("line");
    const QString E_linearGradient      = QL1S("linearGradient");
    const QString E_marker              = QL1S("marker");
    const QString E_mask                = QL1S("mask");
    const QString E_metadata            = QL1S("metadata");
    const QString E_missing_glyph       = QL1S("missing-glyph");
    const QString E_mpath               = QL1S("mpath");
    const QString E_path                = QL1S("path");
    const QString E_pattern             = QL1S("pattern");
    const QString E_polygon             = QL1S("polygon");
    const QString E_polyline            = QL1S("polyline");
    const QString E_radialGradient      = QL1S("radialGradient");
    const QString E_rect                = QL1S("rect");
    const QString E_script              = QL1S("script");
    const QString E_set                 = QL1S("set");
    const QString E_stop                = QL1S("stop");
    const QString E_style               = QL1S("style");
    const QString E_svg                 = QL1S("svg");
    const QString E_switch              = QL1S("switch");
    const QString E_symbol              = QL1S("symbol");
    const QString E_text                = QL1S("text");
    const QString E_textPath            = QL1S("textPath");
    const QString E_title               = QL1S("title");
    const QString E_tref                = QL1S("tref");
    const QString E_flowRoot            = QL1S("flowRoot");
    const QString E_flowRegion          = QL1S("flowRegion");
    const QString E_flowPara            = QL1S("flowPara");
    const QString E_flowSpan            = QL1S("flowSpan");
    const QString E_tspan               = QL1S("tspan");
    const QString E_use                 = QL1S("use");
    const QString E_view                = QL1S("view");
    const QString E_vkern               = QL1S("vkern");
}

namespace Attribute {
    const QString A_alignment_baseline           = QL1S("alignment-baseline");
    const QString A_baseFrequency                = QL1S("baseFrequency");
    const QString A_baseline_shift               = QL1S("baseline-shift");
    const QString A_bbox                         = QL1S("bbox");
    const QString A_block_progression            = QL1S("block-progression");
    const QString A_class_                       = QL1S("class");
    const QString A_clip_path                    = QL1S("clip-path");
    const QString A_clipPathUnits                = QL1S("clipPathUnits");
    const QString A_clip                         = QL1S("clip");
    const QString A_clip_rule                    = QL1S("clip-rule");
    const QString A_color_interpolation_filters  = QL1S("color-interpolation-filters");
    const QString A_color_interpolation          = QL1S("color-interpolation");
    const QString A_color_profile                = QL1S("color-profile");
    const QString A_color                        = QL1S("color");
    const QString A_color_rendering              = QL1S("color-rendering");
    const QString A_cursor                       = QL1S("cursor");
    const QString A_cx                           = QL1S("cx");
    const QString A_cy                           = QL1S("cy");
    const QString A_direction                    = QL1S("direction");
    const QString A_display                      = QL1S("display");
    const QString A_dominant_baseline            = QL1S("dominant-baseline");
    const QString A_d                            = QL1S("d");
    const QString A_dx                           = QL1S("dx");
    const QString A_dy                           = QL1S("dy");
    const QString A_enable_background            = QL1S("enable-background");
    const QString A_externalResourcesRequired    = QL1S("externalResourcesRequired");
    const QString A_fill_opacity                 = QL1S("fill-opacity");
    const QString A_fill                         = QL1S("fill");
    const QString A_fill_rule                    = QL1S("fill-rule");
    const QString A_filter                       = QL1S("filter");
    const QString A_filterRes                    = QL1S("filterRes");
    const QString A_filterUnits                  = QL1S("filterUnits");
    const QString A_flood_color                  = QL1S("flood-color");
    const QString A_flood_opacity                = QL1S("flood-opacity");
    const QString A_font_family                  = QL1S("font-family");
    const QString A_font                         = QL1S("font");
    const QString A_font_size_adjust             = QL1S("font-size-adjust");
    const QString A_font_size                    = QL1S("font-size");
    const QString A_font_stretch                 = QL1S("font-stretch");
    const QString A_font_style                   = QL1S("font-style");
    const QString A_font_variant                 = QL1S("font-variant");
    const QString A_font_weight                  = QL1S("font-weight");
    const QString A_fx                           = QL1S("fx");
    const QString A_fy                           = QL1S("fy");
    const QString A_glyph_orientation_horizontal = QL1S("glyph-orientation-horizontal");
    const QString A_glyph_orientation_vertical   = QL1S("glyph-orientation-vertical");
    const QString A_gradientTransform            = QL1S("gradientTransform");
    const QString A_gradientUnits                = QL1S("gradientUnits");
    const QString A_height                       = QL1S("height");
    const QString A_id                           = QL1S("id");
    const QString A_image_rendering              = QL1S("image-rendering");
    const QString A_k1                           = QL1S("k1");
    const QString A_k2                           = QL1S("k2");
    const QString A_k3                           = QL1S("k3");
    const QString A_kerning                      = QL1S("kerning");
    const QString A_k                            = QL1S("k");
    const QString A_lengthAdjust                 = QL1S("lengthAdjust");
    const QString A_letter_spacing               = QL1S("letter-spacing");
    const QString A_lighting_color               = QL1S("lighting-color");
    const QString A_line_height                  = QL1S("line-height");
    const QString A_marker_end                   = QL1S("marker-end");
    const QString A_marker_mid                   = QL1S("marker-mid");
    const QString A_marker                       = QL1S("marker");
    const QString A_marker_start                 = QL1S("marker-start");
    const QString A_mask                         = QL1S("mask");
    const QString A_offset                       = QL1S("offset");
    const QString A_opacity                      = QL1S("opacity");
    const QString A_overflow                     = QL1S("overflow");
    const QString A_patternTransform             = QL1S("patternTransform");
    const QString A_pointer_events               = QL1S("pointer-events");
    const QString A_points                       = QL1S("points");
    const QString A_primitiveUnits               = QL1S("primitiveUnits");
    const QString A_rotate                       = QL1S("rotate");
    const QString A_r                            = QL1S("r");
    const QString A_rx                           = QL1S("rx");
    const QString A_ry                           = QL1S("ry");
    const QString A_shape_rendering              = QL1S("shape-rendering");
    const QString A_specularConstant             = QL1S("specularConstant");
    const QString A_spreadMethod                 = QL1S("spreadMethod");
    const QString A_stdDeviation                 = QL1S("stdDeviation");
    const QString A_stop_color                   = QL1S("stop-color");
    const QString A_stop_opacity                 = QL1S("stop-opacity");
    const QString A_stroke_dasharray             = QL1S("stroke-dasharray");
    const QString A_stroke_dashoffset            = QL1S("stroke-dashoffset");
    const QString A_stroke_linecap               = QL1S("stroke-linecap");
    const QString A_stroke_linejoin              = QL1S("stroke-linejoin");
    const QString A_stroke_miterlimit            = QL1S("stroke-miterlimit");
    const QString A_stroke_opacity               = QL1S("stroke-opacity");
    const QString A_stroke                       = QL1S("stroke");
    const QString A_stroke_width                 = QL1S("stroke-width");
    const QString A_style                        = QL1S("style");
    const QString A_text_align                   = QL1S("text-align");
    const QString A_text_anchor                  = QL1S("text-anchor");
    const QString A_text_decoration              = QL1S("text-decoration");
    const QString A_textLength                   = QL1S("textLength");
    const QString A_text                         = QL1S("text");
    const QString A_text_rendering               = QL1S("text-rendering");
    const QString A_transform                    = QL1S("transform");
    const QString A_unicode_bidi                 = QL1S("unicode-bidi");
    const QString A_viewBox                      = QL1S("viewBox");
    const QString A_visibility                   = QL1S("visibility");
    const QString A_width                        = QL1S("width");
    const QString A_word_spacing                 = QL1S("word-spacing");
    const QString A_writing_mode                 = QL1S("writing-mode");
    const QString A_x1                           = QL1S("x1");
    const QString A_x2                           = QL1S("x2");
    const QString A_xlink_href                   = QL1S("xlink:href");
    const QString A_x                            = QL1S("x");
    const QString A_y1                           = QL1S("y1");
    const QString A_y2                           = QL1S("y2");
    const QString A_y                            = QL1S("y");
}

namespace AttrId {

#define makeAttrHash(zzz) \
    const uint zzz = qHash(Attribute::A_##zzz)

    makeAttrHash(alignment_baseline);
    makeAttrHash(baseFrequency);
    makeAttrHash(baseline_shift);
    makeAttrHash(bbox);
    makeAttrHash(block_progression);
    makeAttrHash(class_);
    makeAttrHash(clip);
    makeAttrHash(clip_path);
    makeAttrHash(clipPathUnits);
    makeAttrHash(clip_rule);
    makeAttrHash(color);
    makeAttrHash(color_interpolation);
    makeAttrHash(color_interpolation_filters);
    makeAttrHash(color_profile);
    makeAttrHash(color_rendering);
    makeAttrHash(cursor);
    makeAttrHash(cx);
    makeAttrHash(cy);
    makeAttrHash(d);
    makeAttrHash(direction);
    makeAttrHash(display);
    makeAttrHash(dominant_baseline);
    makeAttrHash(dx);
    makeAttrHash(dy);
    makeAttrHash(enable_background);
    makeAttrHash(externalResourcesRequired);
    makeAttrHash(fill);
    makeAttrHash(fill_opacity);
    makeAttrHash(fill_rule);
    makeAttrHash(filter);
    makeAttrHash(filterRes);
    makeAttrHash(filterUnits);
    makeAttrHash(flood_color);
    makeAttrHash(flood_opacity);
    makeAttrHash(font);
    makeAttrHash(font_family);
    makeAttrHash(font_size);
    makeAttrHash(font_size_adjust);
    makeAttrHash(font_stretch);
    makeAttrHash(font_style);
    makeAttrHash(font_variant);
    makeAttrHash(font_weight);
    makeAttrHash(fx);
    makeAttrHash(fy);
    makeAttrHash(glyph_orientation_horizontal);
    makeAttrHash(glyph_orientation_vertical);
    makeAttrHash(gradientTransform);
    makeAttrHash(gradientUnits);
    makeAttrHash(height);
    makeAttrHash(id);
    makeAttrHash(image_rendering);
    makeAttrHash(k);
    makeAttrHash(k1);
    makeAttrHash(k2);
    makeAttrHash(k3);
    makeAttrHash(kerning);
    makeAttrHash(lengthAdjust);
    makeAttrHash(letter_spacing);
    makeAttrHash(lighting_color);
    makeAttrHash(line_height);
    makeAttrHash(marker);
    makeAttrHash(marker_end);
    makeAttrHash(marker_mid);
    makeAttrHash(marker_start);
    makeAttrHash(mask);
    makeAttrHash(offset);
    makeAttrHash(opacity);
    makeAttrHash(overflow);
    makeAttrHash(patternTransform);
    makeAttrHash(pointer_events);
    makeAttrHash(points);
    makeAttrHash(primitiveUnits);
    makeAttrHash(r);
    makeAttrHash(rotate);
    makeAttrHash(rx);
    makeAttrHash(ry);
    makeAttrHash(shape_rendering);
    makeAttrHash(specularConstant);
    makeAttrHash(spreadMethod);
    makeAttrHash(stdDeviation);
    makeAttrHash(stop_color);
    makeAttrHash(stop_opacity);
    makeAttrHash(stroke);
    makeAttrHash(stroke_dasharray);
    makeAttrHash(stroke_dashoffset);
    makeAttrHash(stroke_linecap);
    makeAttrHash(stroke_linejoin);
    makeAttrHash(stroke_miterlimit);
    makeAttrHash(stroke_opacity);
    makeAttrHash(stroke_width);
    makeAttrHash(style);
    makeAttrHash(text);
    makeAttrHash(text_align);
    makeAttrHash(text_anchor);
    makeAttrHash(text_decoration);
    makeAttrHash(textLength);
    makeAttrHash(text_rendering);
    makeAttrHash(transform);
    makeAttrHash(unicode_bidi);
    makeAttrHash(viewBox);
    makeAttrHash(visibility);
    makeAttrHash(width);
    makeAttrHash(word_spacing);
    makeAttrHash(writing_mode);
    makeAttrHash(x);
    makeAttrHash(x1);
    makeAttrHash(x2);
    makeAttrHash(xlink_href);
    makeAttrHash(y);
    makeAttrHash(y1);
    makeAttrHash(y2);
}

namespace Properties
{
using namespace Element;
using namespace Attribute;

const StringSet presentationAttributes = StringSet()
    << A_alignment_baseline << A_baseline_shift << A_clip_path << A_clip_rule << A_clip
    << A_color_interpolation_filters << A_color_interpolation << A_color_profile
    << A_color_rendering << A_color << A_cursor << A_direction << A_display << A_dominant_baseline
    << A_enable_background << A_fill_opacity << A_fill_rule << A_fill << A_filter << A_flood_color
    << A_flood_opacity << A_font_family << A_font_size_adjust << A_font_size << A_font_stretch
    << A_font_style << A_font_variant << A_font_weight << A_glyph_orientation_horizontal
    << A_glyph_orientation_vertical << A_image_rendering << A_kerning << A_letter_spacing
    << A_lighting_color << A_marker_end << A_marker_mid << A_marker_start << A_mask << A_opacity
    << A_overflow << A_pointer_events << A_shape_rendering << A_stop_color << A_stop_opacity
    << A_stroke_dasharray << A_stroke_dashoffset << A_stroke_linecap << A_stroke_linejoin
    << A_stroke_miterlimit << A_stroke_opacity << A_stroke_width << A_stroke << A_text_anchor
    << A_text_decoration << A_text_rendering << A_unicode_bidi << A_visibility << A_word_spacing
    << A_writing_mode;

const QList<uint> presentationAttributesIds = QList<uint>()
    << AttrId::alignment_baseline << AttrId::baseline_shift << AttrId::clip_path
    << AttrId::clip_rule << AttrId::clip << AttrId::color_interpolation_filters
    << AttrId::color_interpolation << AttrId::color_profile << AttrId::color_rendering
    << AttrId::color << AttrId::cursor << AttrId::direction << AttrId::display
    << AttrId::dominant_baseline << AttrId::enable_background << AttrId::fill_opacity
    << AttrId::fill_rule << AttrId::fill << AttrId::filter << AttrId::flood_color
    << AttrId::flood_opacity << AttrId::font_family << AttrId::font_size_adjust << AttrId::font_size
    << AttrId::font_stretch << AttrId::font_style << AttrId::font_variant << AttrId::font_weight
    << AttrId::glyph_orientation_horizontal << AttrId::glyph_orientation_vertical
    << AttrId::image_rendering << AttrId::kerning << AttrId::letter_spacing
    << AttrId::lighting_color << AttrId::marker_end << AttrId::marker_mid << AttrId::marker_start
    << AttrId::mask << AttrId::opacity << AttrId::overflow << AttrId::pointer_events
    << AttrId::shape_rendering << AttrId::stop_color << AttrId::stop_opacity
    << AttrId::stroke_dasharray << AttrId::stroke_dashoffset << AttrId::stroke_linecap
    << AttrId::stroke_linejoin << AttrId::stroke_miterlimit << AttrId::stroke_opacity
    << AttrId::stroke_width << AttrId::stroke << AttrId::text_anchor << AttrId::text_decoration
    << AttrId::text_rendering << AttrId::unicode_bidi << AttrId::visibility << AttrId::word_spacing
    << AttrId::writing_mode;

const QList<uint> linkableStyleAttributesIds = QList<uint>()
    << AttrId::clip_path << AttrId::fill << AttrId::mask << AttrId::filter << AttrId::stroke
    << AttrId::marker_start << AttrId::marker_mid << AttrId::marker_end;

const QList<uint> digitListIds = QList<uint>()
    << AttrId::x << AttrId::y << AttrId::x1 << AttrId::y1 << AttrId::x2 << AttrId::y2
    << AttrId::width << AttrId::height << AttrId::r << AttrId::rx << AttrId::ry << AttrId::fx
    << AttrId::fy << AttrId::cx << AttrId::cy << AttrId::dx << AttrId::dy << AttrId::offset;

const QList<uint> textAttributesIds = QList<uint>()
    << AttrId::font_style << AttrId::font_variant << AttrId::font_weight << AttrId::font_weight
    << AttrId::font_stretch << AttrId::font_size << AttrId::font_family << AttrId::font_size_adjust
    << AttrId::kerning << AttrId::letter_spacing << AttrId::word_spacing << AttrId::text_decoration
    << AttrId::writing_mode << AttrId::glyph_orientation_vertical
    << AttrId::glyph_orientation_horizontal << AttrId::direction << AttrId::text_anchor
    << AttrId::dominant_baseline << AttrId::alignment_baseline << AttrId::baseline_shift;

const StringSet textElements = StringSet()
    << E_text << E_tspan << E_flowRoot << E_flowPara << E_flowSpan << E_textPath;

const StringSet elementsUsingXLink = StringSet()
    << E_a << E_altGlyph << E_color_profile << E_cursor << E_feImage << E_filter << E_font_face_uri
    << E_glyphRef << E_image << E_linearGradient << E_mpath << E_pattern << E_radialGradient
    << E_script << E_textPath << E_use << E_animate << E_animateColor << E_animateMotion
    << E_animateTransform << E_set << E_tref;

const StringSet defsList = StringSet()
    << E_altGlyphDef << E_clipPath << E_cursor << E_filter << E_linearGradient
    << E_marker << E_mask << E_pattern << E_radialGradient /* << E_symbol*/;
}

namespace DefaultValue {
    const QString V_none = QL1S("none");
    const QString V_zero = QL1S("0");
}

namespace LengthType {
    const QString em = QL1S("em");
    const QString ex = QL1S("ex");
    const QString px = QL1S("px");
    const QString in = QL1S("in");
    const QString cm = QL1S("cm");
    const QString mm = QL1S("mm");
    const QString pt = QL1S("pt");
    const QString pc = QL1S("pc");
    const QChar Percent = QL1C('%');
}

QHash<uint, QVariant> initDefaultStyleHash()
{
    static QHash<uint,QVariant> hash;
    if (!hash.isEmpty())
        return hash;

    using namespace Attribute;

    hash.insert(AttrId::alignment_baseline,           QL1S("auto"));
    hash.insert(AttrId::baseline_shift,               QL1S("baseline"));
    hash.insert(AttrId::block_progression,            QL1S("tb"));
    hash.insert(AttrId::clip,                         QL1S("auto"));
    hash.insert(AttrId::clip_path,                    QL1S("none"));
    hash.insert(AttrId::clip_rule,                    QL1S("nonzero"));
    hash.insert(AttrId::direction,                    QL1S("ltr"));
    hash.insert(AttrId::display,                      QL1S("inline"));
    hash.insert(AttrId::dominant_baseline,            QL1S("auto"));
    hash.insert(AttrId::enable_background,            QL1S("accumulate"));
    hash.insert(AttrId::fill_opacity,                 1.0);
    hash.insert(AttrId::fill_rule,                    QL1S("nonzero"));
    hash.insert(AttrId::filter,                       QL1S("none"));
    hash.insert(AttrId::flood_color,                  QL1S("black"));
    hash.insert(AttrId::font_size_adjust,             QL1S("none"));
    hash.insert(AttrId::font_size,                    QL1S("medium"));
    hash.insert(AttrId::font_stretch,                 QL1S("normal"));
    hash.insert(AttrId::font_style,                   QL1S("normal"));
    hash.insert(AttrId::font_variant,                 QL1S("normal"));
    hash.insert(AttrId::font_weight,                  QL1S("normal"));
    hash.insert(AttrId::glyph_orientation_horizontal, QL1S("0deg"));
    hash.insert(AttrId::glyph_orientation_vertical,   QL1S("auto"));
    hash.insert(AttrId::kerning,                      QL1S("auto"));
    hash.insert(AttrId::letter_spacing,               QL1S("normal"));
    hash.insert(AttrId::marker_start,                 QL1S("none"));
    hash.insert(AttrId::marker_mid,                   QL1S("none"));
    hash.insert(AttrId::marker_end,                   QL1S("none"));
    hash.insert(AttrId::mask,                         QL1S("none"));
    hash.insert(AttrId::opacity,                      1.0);
    // TODO: 'overflow' propery does not have default value
    hash.insert(AttrId::overflow,                     QL1S("visible"));
    hash.insert(AttrId::pointer_events,               QL1S("visiblePainted"));
    hash.insert(AttrId::stop_opacity,                 1.0);
    hash.insert(AttrId::stroke_dasharray,             QL1S("none"));
    hash.insert(AttrId::stroke_dashoffset,            0);
    hash.insert(AttrId::stroke_linecap,               QL1S("butt"));
    hash.insert(AttrId::stroke_linejoin,              QL1S("miter"));
    hash.insert(AttrId::stroke_miterlimit,            4.0);
    hash.insert(AttrId::stroke,                       QL1S("none"));
    hash.insert(AttrId::stroke_opacity,               1.0);
    hash.insert(AttrId::stroke_width,                 1.0);
    hash.insert(AttrId::text_anchor,                  QL1S("start"));
    hash.insert(AttrId::text_decoration,              QL1S("none"));
    hash.insert(AttrId::visibility,                   QL1S("visible"));
    hash.insert(AttrId::word_spacing,                 QL1S("normal"));
    hash.insert(AttrId::writing_mode,                 QL1S("lr-tb"));

    return hash;
}

const QHash<QString,uint> allAttributesHash()
{
    static QHash<QString,uint> hash;
    if (!hash.isEmpty())
        return hash;

    using namespace Attribute;
    using namespace AttrId;

    #define insertToHash(zzz) \
        hash.insert(A_##zzz, AttrId::zzz)

    insertToHash(alignment_baseline);
    insertToHash(baseFrequency);
    insertToHash(baseline_shift);
    insertToHash(bbox);
    insertToHash(block_progression);
    insertToHash(class_);
    insertToHash(clip);
    insertToHash(clip_path);
    insertToHash(clipPathUnits);
    insertToHash(clip_rule);
    insertToHash(color);
    insertToHash(color_interpolation);
    insertToHash(color_interpolation_filters);
    insertToHash(color_profile);
    insertToHash(color_rendering);
    insertToHash(cursor);
    insertToHash(cx);
    insertToHash(cy);
    insertToHash(d);
    insertToHash(direction);
    insertToHash(display);
    insertToHash(dominant_baseline);
    insertToHash(dx);
    insertToHash(dy);
    insertToHash(enable_background);
    insertToHash(externalResourcesRequired);
    insertToHash(fill);
    insertToHash(fill_opacity);
    insertToHash(fill_rule);
    insertToHash(filter);
    insertToHash(filterRes);
    insertToHash(filterUnits);
    insertToHash(flood_color);
    insertToHash(flood_opacity);
    insertToHash(font);
    insertToHash(font_family);
    insertToHash(font_size);
    insertToHash(font_size_adjust);
    insertToHash(font_stretch);
    insertToHash(font_style);
    insertToHash(font_variant);
    insertToHash(font_weight);
    insertToHash(fx);
    insertToHash(fy);
    insertToHash(glyph_orientation_horizontal);
    insertToHash(glyph_orientation_vertical);
    insertToHash(gradientTransform);
    insertToHash(gradientUnits);
    insertToHash(height);
    insertToHash(id);
    insertToHash(image_rendering);
    insertToHash(k);
    insertToHash(k1);
    insertToHash(k2);
    insertToHash(k3);
    insertToHash(kerning);
    insertToHash(lengthAdjust);
    insertToHash(letter_spacing);
    insertToHash(lighting_color);
    insertToHash(line_height);
    insertToHash(marker);
    insertToHash(marker_end);
    insertToHash(marker_mid);
    insertToHash(marker_start);
    insertToHash(mask);
    insertToHash(offset);
    insertToHash(opacity);
    insertToHash(overflow);
    insertToHash(patternTransform);
    insertToHash(pointer_events);
    insertToHash(points);
    insertToHash(primitiveUnits);
    insertToHash(r);
    insertToHash(rotate);
    insertToHash(rx);
    insertToHash(ry);
    insertToHash(shape_rendering);
    insertToHash(specularConstant);
    insertToHash(spreadMethod);
    insertToHash(stdDeviation);
    insertToHash(stop_color);
    insertToHash(stop_opacity);
    insertToHash(stroke);
    insertToHash(stroke_dasharray);
    insertToHash(stroke_dashoffset);
    insertToHash(stroke_linecap);
    insertToHash(stroke_linejoin);
    insertToHash(stroke_miterlimit);
    insertToHash(stroke_opacity);
    insertToHash(stroke_width);
    insertToHash(style);
    insertToHash(text);
    insertToHash(text_align);
    insertToHash(text_anchor);
    insertToHash(text_decoration);
    insertToHash(textLength);
    insertToHash(text_rendering);
    insertToHash(transform);
    insertToHash(unicode_bidi);
    insertToHash(viewBox);
    insertToHash(visibility);
    insertToHash(width);
    insertToHash(word_spacing);
    insertToHash(writing_mode);
    insertToHash(x);
    insertToHash(x1);
    insertToHash(x2);
    insertToHash(xlink_href);
    insertToHash(y);
    insertToHash(y1);
    insertToHash(y2);

    return hash;
}

const QHash<uint,QString>& allAttributesHashInv()
{
    static QHash<uint,QString> hash;
    if (hash.isEmpty()) {
        QHash<QString,uint> h2 = allAttributesHash();
        QHash<QString,uint>::const_iterator it2 = h2.constBegin();
        for (; it2 != h2.constEnd(); ++it2)
            hash.insert(it2.value(), it2.key());
    }
    return hash;
}

QString attrIdToStr(uint id)
{
    return allAttributesHashInv().value(id);
}

uint attrStrToId(const QString &name)
{
    uint h = qHash(name);
    if (!allAttributesHashInv().contains(h))
        return 0;
    return h;
}

bool isDefaultAttribute(const QString &name)
{
    return allAttributesHash().contains(name);
}

bool isDefaultAttribute(uint id)
{
    return allAttributesHashInv().contains(id);
}

uint hash(const QChar *p, int n)
{
    uint h = 0;

    while (n--) {
        h = (h << 4) + (*p++).unicode();
        h ^= (h & 0xf0000000) >> 23;
        h &= 0x0fffffff;
    }
    return h;
}

uint qHash(const QString &key)
{
    return hash(key.unicode(), key.size());
}
