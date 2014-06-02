/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
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

// TODO: maybe declare only often used elements and attributes

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
    const QString A_block_progression            = QL1S("block-progression");
    const QString A_clip_path                    = QL1S("clip-path");
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
    const QString A_kerning                      = QL1S("kerning");
    const QString A_letter_spacing               = QL1S("letter-spacing");
    const QString A_lighting_color               = QL1S("lighting-color");
    const QString A_marker_end                   = QL1S("marker-end");
    const QString A_marker_mid                   = QL1S("marker-mid");
    const QString A_marker                       = QL1S("marker");
    const QString A_marker_start                 = QL1S("marker-start");
    const QString A_mask                         = QL1S("mask");
    const QString A_offset                       = QL1S("offset");
    const QString A_opacity                      = QL1S("opacity");
    const QString A_overflow                     = QL1S("overflow");
    const QString A_pointer_events               = QL1S("pointer-events");
    const QString A_points                       = QL1S("points");
    const QString A_primitiveUnits               = QL1S("primitiveUnits");
    const QString A_r                            = QL1S("r");
    const QString A_rx                           = QL1S("rx");
    const QString A_ry                           = QL1S("ry");
    const QString A_shape_rendering              = QL1S("shape-rendering");
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
    const QString A_text_anchor                  = QL1S("text-anchor");
    const QString A_text_decoration              = QL1S("text-decoration");
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

const StringSet linkableStyleAttributes = StringSet()
    << A_clip_path << A_fill << A_mask << A_filter << A_stroke << A_marker_start
    << A_marker_mid << A_marker_end;

const StringSet digitList = StringSet()
    << A_x << A_y << A_x1 << A_y1 << A_x2 << A_y2 << A_width << A_height << A_r << A_rx << A_ry
    << A_fx << A_fy << A_cx << A_cy << A_dx << A_dy << A_offset;

const StringSet textElements = StringSet()
    << E_text << E_tspan << E_flowRoot << E_flowPara << E_flowSpan << E_textPath;

const StringSet textAttributes = StringSet()
    << A_font_style << A_font_variant << A_font_weight << A_font_weight << A_font_stretch
    << A_font_size << A_font_size_adjust << A_kerning << A_letter_spacing << A_word_spacing
    << A_text_decoration << A_writing_mode << A_glyph_orientation_vertical
    << A_glyph_orientation_horizontal << A_direction << A_text_anchor << A_dominant_baseline
    << A_alignment_baseline << A_baseline_shift;

const StringSet elementsUsingXLink = StringSet()
    << E_a << E_altGlyph << E_color_profile << E_cursor << E_feImage << E_filter << E_font_face_uri
    << E_glyphRef << E_image << E_linearGradient << E_mpath << E_pattern << E_radialGradient
    << E_script << E_textPath << E_use << E_animate << E_animateColor << E_animateMotion
    << E_animateTransform << E_set << E_tref;
}

namespace DefaultValue {
    const QString V_none = QL1S("none");
    const QString V_null = QL1S("0");
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
    const QChar percent = QL1C('%');
}

namespace CleanerAttr {
    const QString UsedElement = "used-element";
    const QString BoundingBox = "bbox";
    const QString BBoxTransform = "bbox-transform";
}

QVariantHash initDefaultStyleHash()
{
    static QVariantHash hash;
    if (!hash.isEmpty())
        return hash;

    using namespace Attribute;

    hash.insert(A_alignment_baseline, "auto");
    hash.insert(A_baseline_shift, "baseline");
    hash.insert(A_block_progression, "tb");
    hash.insert(A_clip, "auto");
    hash.insert(A_clip_path, "none");
    hash.insert(A_clip_rule, "nonzero");
    hash.insert(A_direction, "ltr");
    hash.insert(A_display, "inline");
    hash.insert(A_dominant_baseline, "auto");
    hash.insert(A_enable_background, "accumulate");
    hash.insert(A_fill_opacity, 1.0);
    hash.insert(A_fill_rule, "nonzero");
    hash.insert(A_filter, "none");
    hash.insert(A_flood_color, "black");
    hash.insert(A_font_size_adjust, "none");
    hash.insert(A_font_size, "medium");
    hash.insert(A_font_stretch, "normal");
    hash.insert(A_font_style, "normal");
    hash.insert(A_font_variant, "normal");
    hash.insert(A_font_weight, "normal");
    hash.insert(A_glyph_orientation_horizontal, "0deg");
    hash.insert(A_glyph_orientation_vertical, "auto");
    hash.insert(A_kerning, "auto");
    hash.insert(A_letter_spacing, "normal");
    hash.insert(A_marker_start, "none");
    hash.insert(A_marker_mid, "none");
    hash.insert(A_marker_end, "none");
    hash.insert(A_mask, "none");
    hash.insert(A_opacity, 1.0);
    // TODO: 'overflow' propery does not have default value
    hash.insert(A_overflow, "visible");
    hash.insert(A_pointer_events, "visiblePainted");
    hash.insert(A_stop_opacity, 1.0);
    hash.insert(A_stroke_dasharray, "none");
    hash.insert(A_stroke_dashoffset, 0);
    hash.insert(A_stroke_linecap, "butt");
    hash.insert(A_stroke_linejoin, "miter");
    hash.insert(A_stroke_miterlimit, 4.0);
    hash.insert(A_stroke, "none");
    hash.insert(A_stroke_opacity, 1.0);
    hash.insert(A_stroke_width, 1.0);
    hash.insert(A_text_anchor, "start");
    hash.insert(A_text_decoration, "none");
    hash.insert(A_visibility, "visible");
    hash.insert(A_word_spacing, "normal");
    hash.insert(A_writing_mode, "lr-tb");
    return hash;
}
