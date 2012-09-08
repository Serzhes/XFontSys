﻿/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: All operations with the FreeType library contains
/*			 only in this module
/****************************************************************************/

#include "FT_Lib.h"
#include "public/common.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_SIZES_H

// converts points to pixels
inline float fix26d6ToPixel(const int p)
{
	//const int div = 1 << 6;
	//return float(p) / div;
	return (float)(p >> 6) + ( (p & 0x20) >> 5 );
}

namespace ftLib
{
	FT_Library  g_FT2Lib = nullptr;

	FTFace::FTFace() : m_Face(nullptr)
	{	}

	FTFace::~FTFace(void)
	{
		if (m_Face)
			FT_Done_Face(m_Face);
	}

	bool FTFace::CreateFace(const char* fontFile, int index)
	{
		if (!g_FT2Lib || !fontFile)
			return false;

		if (FT_New_Face(g_FT2Lib, fontFile, index, &m_Face))
		{
			m_Face = nullptr;
			return false;
		}

		return true;
	}

	void FTFace::SetPixelSize(int fontSize, int horz_res, int vert_res)
	{
		if (FT_Set_Char_Size(m_Face, fontSize << 6, fontSize << 6, horz_res, vert_res))
			return;
	}

	bool FTFace::Glyph(const int wch)
	{
		if (FT_Load_Char(m_Face, wch, FT_LOAD_RENDER))
		{
			return false;
		}

		return true;
	}

	int FTFace::GlyphAdvanceX(const int wch_prev, const int wch_next)
	{
		if ((FT_UInt)wch_next && FT_HAS_KERNING(m_Face))
		{
			FT_Vector vKern;
			FT_Get_Kerning(m_Face, (FT_UInt)wch_prev, (FT_UInt)wch_next, FT_KERNING_DEFAULT, &vKern);
			return (GlyphAdvanceX() + fix26d6ToPixel(vKern.x));
		}
		else
			return GlyphAdvanceX();
	}

	int FTFace::GlyphAdvanceX() const
	{
		return fix26d6ToPixel(m_Face->glyph->advance.x);
	}

	int FTFace::GlyphAdvanceY(const int wch_prev, const int wch_next)
	{
		if (wch_next && FT_HAS_KERNING(m_Face))
		{
			FT_Vector vKern;
			FT_Get_Kerning(m_Face, wch_prev, wch_next, FT_KERNING_DEFAULT, &vKern);
			return (GlyphAdvanceY() + fix26d6ToPixel(vKern.y));
		}
		else
			return GlyphAdvanceY();
	}

	int FTFace::GlyphAdvanceY() const
	{
		return fix26d6ToPixel(m_Face->glyph->advance.y);
	}

	int FTFace::GlyphWidth(void) const
	{
		return fix26d6ToPixel(m_Face->glyph->metrics.width);
	}

	int FTFace::GlyphHeight() const
	{
		return fix26d6ToPixel(m_Face->glyph->metrics.height);
	}

	int FTFace::GlyphBearingX(void) const
	{
		return fix26d6ToPixel(m_Face->glyph->metrics.horiBearingX);
	}

	int FTFace::GlyphBearingY(void) const
	{
		return fix26d6ToPixel(m_Face->glyph->metrics.horiBearingY);
	}

	int FTFace::GlyphAscender() const
	{
		return fix26d6ToPixel(m_Face->size->metrics.ascender);
	}

	int FTFace::GlyphDescender() const
	{
		return fix26d6ToPixel(m_Face->size->metrics.descender);
	}

	int FTFace::GlyphBitmapWidth(void) const
	{
		return m_Face->glyph->bitmap.width;
	}

	int FTFace::GlyphBitmapHeight(void) const
	{
		return m_Face->glyph->bitmap.rows;
	}

	int FTFace::GlyphBitmapLeft(void) const
	{
		return m_Face->glyph->bitmap_left;
	}

	int FTFace::GlyphBitmapTop(void) const
	{
		return m_Face->glyph->bitmap_top;
	}

	unsigned char const* FTFace::GlyphBitmap( void ) const
	{
		return m_Face->glyph->bitmap.buffer;
	}

	int FTFace::GlyphBitmapPith( void ) const
	{
		return m_Face->glyph->bitmap.pitch;
	}

	bool InitFT2Lib( void )
	{
		if ( FT_Init_FreeType( &g_FT2Lib ) )
			return false;

		return true;
	}

	void DoneFT2Lib( void )
	{
		if (g_FT2Lib) {
			FT_Done_FreeType(g_FT2Lib);
			g_FT2Lib = nullptr;
		}
	}
};