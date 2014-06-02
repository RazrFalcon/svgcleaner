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

#ifndef ENUMS_H
#define ENUMS_H

#include <QStringList>
#include <QSet>
#include <QVariantHash>

#define QL1S(x) QLatin1String(x)
#define QL1C(x) QLatin1Char(x)

typedef QSet<QString> StringSet;

extern const QString UrlPrefix;

namespace Element {
    extern const QString E_a;
    extern const QString E_altGlyph;
    extern const QString E_altGlyphDef;
    extern const QString E_altGlyphItem;
    extern const QString E_animate;
    extern const QString E_animateColor;
    extern const QString E_animateMotion;
    extern const QString E_animateTransform;
    extern const QString E_circle;
    extern const QString E_clipPath;
    extern const QString E_color_profile;
    extern const QString E_cursor;
    extern const QString E_defs;
    extern const QString E_desc;
    extern const QString E_ellipse;
    extern const QString E_feBlend;
    extern const QString E_feColorMatrix;
    extern const QString E_feComponentTransfer;
    extern const QString E_feComposite;
    extern const QString E_feConvolveMatrix;
    extern const QString E_feDiffuseLighting;
    extern const QString E_feDisplacementMap;
    extern const QString E_feDistantLight;
    extern const QString E_feFlood;
    extern const QString E_feGaussianBlur;
    extern const QString E_feImage;
    extern const QString E_feMerge;
    extern const QString E_feMergeNode;
    extern const QString E_feMorphology;
    extern const QString E_feOffset;
    extern const QString E_fePointLight;
    extern const QString E_feSpecularLighting;
    extern const QString E_feSpotLight;
    extern const QString E_feTile;
    extern const QString E_feTurbulence;
    extern const QString E_filter;
    extern const QString E_font;
    extern const QString E_font_face;
    extern const QString E_font_face_format;
    extern const QString E_font_face_name;
    extern const QString E_font_face_src;
    extern const QString E_font_face_uri;
    extern const QString E_foreignObject;
    extern const QString E_g;
    extern const QString E_glyph;
    extern const QString E_glyphRef;
    extern const QString E_hkern;
    extern const QString E_image;
    extern const QString E_line;
    extern const QString E_linearGradient;
    extern const QString E_marker;
    extern const QString E_mask;
    extern const QString E_metadata;
    extern const QString E_missing_glyph;
    extern const QString E_mpath;
    extern const QString E_path;
    extern const QString E_pattern;
    extern const QString E_polygon;
    extern const QString E_polyline;
    extern const QString E_radialGradient;
    extern const QString E_rect;
    extern const QString E_script;
    extern const QString E_set;
    extern const QString E_stop;
    extern const QString E_style;
    extern const QString E_svg;
    extern const QString E_switch;
    extern const QString E_symbol;
    extern const QString E_text;
    extern const QString E_textPath;
    extern const QString E_title;
    extern const QString E_tref;
    extern const QString E_flowRoot;
    extern const QString E_flowRegion;
    extern const QString E_flowPara;
    extern const QString E_flowSpan;
    extern const QString E_tspan;
    extern const QString E_use;
    extern const QString E_view;
    extern const QString E_vkern;
}

namespace Attribute {
    extern const QString A_alignment_baseline;
    extern const QString A_baseFrequency;
    extern const QString A_baseline_shift;
    extern const QString A_block_progression;
    extern const QString A_clip;
    extern const QString A_clip_path;
    extern const QString A_clip_rule;
    extern const QString A_color;
    extern const QString A_color_interpolation;
    extern const QString A_color_interpolation_filters;
    extern const QString A_color_profile;
    extern const QString A_color_rendering;
    extern const QString A_cursor;
    extern const QString A_cx;
    extern const QString A_cy;
    extern const QString A_d;
    extern const QString A_direction;
    extern const QString A_display;
    extern const QString A_dominant_baseline;
    extern const QString A_dx;
    extern const QString A_dy;
    extern const QString A_enable_background;
    extern const QString A_externalResourcesRequired;
    extern const QString A_fill;
    extern const QString A_fill_opacity;
    extern const QString A_fill_rule;
    extern const QString A_filter;
    extern const QString A_flood_color;
    extern const QString A_flood_opacity;
    extern const QString A_font;
    extern const QString A_font_family;
    extern const QString A_font_size;
    extern const QString A_font_size_adjust;
    extern const QString A_font_stretch;
    extern const QString A_font_style;
    extern const QString A_font_variant;
    extern const QString A_font_weight;
    extern const QString A_fx;
    extern const QString A_fy;
    extern const QString A_glyph_orientation_horizontal;
    extern const QString A_glyph_orientation_vertical;
    extern const QString A_gradientTransform;
    extern const QString A_gradientUnits;
    extern const QString A_height;
    extern const QString A_id;
    extern const QString A_image_rendering;
    extern const QString A_kerning;
    extern const QString A_letter_spacing;
    extern const QString A_lighting_color;
    extern const QString A_marker;
    extern const QString A_marker_end;
    extern const QString A_marker_mid;
    extern const QString A_marker_start;
    extern const QString A_mask;
    extern const QString A_offset;
    extern const QString A_opacity;
    extern const QString A_opacity;
    extern const QString A_overflow;
    extern const QString A_pointer_events;
    extern const QString A_points;
    extern const QString A_primitiveUnits;
    extern const QString A_r;
    extern const QString A_rx;
    extern const QString A_ry;
    extern const QString A_shape_rendering;
    extern const QString A_spreadMethod;
    extern const QString A_stdDeviation;
    extern const QString A_stop_color;
    extern const QString A_stop_opacity;
    extern const QString A_stroke;
    extern const QString A_stroke_dasharray;
    extern const QString A_stroke_dashoffset;
    extern const QString A_stroke_linecap;
    extern const QString A_stroke_linejoin;
    extern const QString A_stroke_miterlimit;
    extern const QString A_stroke_miterlimit;
    extern const QString A_stroke_opacity;
    extern const QString A_stroke_width;
    extern const QString A_style;
    extern const QString A_text;
    extern const QString A_text_anchor;
    extern const QString A_text_decoration;
    extern const QString A_text_rendering;
    extern const QString A_transform;
    extern const QString A_unicode_bidi;
    extern const QString A_viewBox;
    extern const QString A_visibility;
    extern const QString A_width;
    extern const QString A_word_spacing;
    extern const QString A_writing_mode;
    extern const QString A_x;
    extern const QString A_x1;
    extern const QString A_x2;
    extern const QString A_xlink_href;
    extern const QString A_y;
    extern const QString A_y1;
    extern const QString A_y2;
}

namespace DefaultValue {
    extern const QString V_none;
    extern const QString V_null;
}

namespace LengthType {
    extern const QString em;
    extern const QString ex;
    extern const QString px;
    extern const QString in;
    extern const QString cm;
    extern const QString mm;
    extern const QString pt;
    extern const QString pc;
    extern const QChar percent;
}

namespace Properties {
    extern const StringSet presentationAttributes;
    extern const StringSet linkableStyleAttributes;
    extern const StringSet digitList;
    extern const StringSet textElements;
    extern const StringSet textAttributes;
    extern const StringSet elementsUsingXLink;
}

namespace CleanerAttr {
    extern const QString UsedElement;
    extern const QString BoundingBox;
    extern const QString BBoxTransform;
}

QVariantHash initDefaultStyleHash();

#endif // ENUMS_H
