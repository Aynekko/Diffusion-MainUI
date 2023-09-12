/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "Framework.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "Slider.h"
#include "CheckBox.h"
#include "SpinControl.h"
#include "StringArrayModel.h"

#define ART_BANNER	  	"gfx/shell/head_vidoptions"
#define ART_GAMMA		"gfx/shell/gamma"

#define LEGACY_VIEWSIZE 0

enum
{
	PRESET_LOW = 0,
	PRESET_MEDIUM,
	PRESET_HIGH,
	PRESET_MAXIMUM,
};

class CMenuVidOptions : public CMenuFramework
{
private:
	void _Init() override;
	void _VidInit() override;

public:
	CMenuVidOptions() : CMenuFramework( "CMenuVidOptions" ) { }
	void SaveAndPopMenu() override;
//	void GammaUpdate();
//	void GammaGet();
	void UpdateConfig();
	void GetConfig();
	void Show();
	void Hide();
//	void SetSettingsTo( int Quality );

	void WriteAnisotropy( void );

	int		outlineWidth;

	class CMenuVidPreview : public CMenuBitmap
	{
		void Draw() override;
	} testImage;

	CMenuPicButton done;
	CMenuSpinControl maxFPS;

	CMenuPicButton	SetToLow;
	CMenuPicButton	SetToMedium;
	CMenuPicButton	SetToHigh;
	CMenuPicButton	SetToMaximum;

#if LEGACY_VIEWSIZE
	CMenuSlider	screenSize;
#endif
	CMenuSlider	gammaIntensity;
	CMenuSlider	glareReduction;
	CMenuCheckBox	fastSky;
	CMenuCheckBox   vbo;
	CMenuCheckBox   bump;
	CMenuCheckBox	cl_muzzlelight;
	CMenuCheckBox	sv_fadecorpses;
	CMenuCheckBox	gl_sunshafts;
//	CMenuCheckBox	r_shadows;
	CMenuCheckBox	r_bloom;
	CMenuCheckBox	r_blur;
	CMenuSpinControl shadowQ;
	CMenuSpinControl mirrorQ;
	CMenuSpinControl ssaoQ;
	CMenuCheckBox	gl_msaa;
	CMenuCheckBox	gl_lensflare;
	CMenuCheckBox	gl_emboss;
	CMenuCheckBox	gl_bump;
	CMenuCheckBox	gl_specular;
	CMenuSpinControl gl_anisotropy;
	CMenuCheckBox gl_water_refraction;
	CMenuCheckBox gl_exposure;
	CMenuCheckBox gl_cubemaps;
	CMenuCheckBox gl_water_planar;

	HIMAGE		hTestImage;
};

void SetSettingsTo( int Quality )
{
	if( Quality == PRESET_LOW )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 2 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 0 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 0 );
		EngFuncs::CvarSetValue( "gl_msaa", 0 );
		EngFuncs::CvarSetValue( "r_bloom", 0 );
		EngFuncs::CvarSetValue( "r_blur", 0 );
		EngFuncs::CvarSetValue( "gl_lensflare", 0 );
		EngFuncs::CvarSetValue( "gl_emboss", 0 );
		EngFuncs::CvarSetValue( "gl_water_refraction", 0 );
		EngFuncs::CvarSetValue( "gl_exposure", 0 );
		EngFuncs::CvarSetValue( "gl_bump", 0 );
		EngFuncs::CvarSetValue( "gl_specular", 0 );
		EngFuncs::CvarSetValue( "gl_cubemaps", 0 );
		EngFuncs::CvarSetValue( "gl_water_planar", 0 );
		EngFuncs::CvarSetValue( "r_mirrorquality", 0 );
		EngFuncs::CvarSetValue( "r_shadowquality", 0 );
		EngFuncs::CvarSetValue( "gl_ssao", 0 );
	}
	else if( Quality == PRESET_MEDIUM )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 4 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 0 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 1 );
		EngFuncs::CvarSetValue( "gl_msaa", 1 );
		EngFuncs::CvarSetValue( "r_bloom", 0 );
		EngFuncs::CvarSetValue( "r_blur", 0 );
		EngFuncs::CvarSetValue( "gl_lensflare", 1 );
		EngFuncs::CvarSetValue( "gl_emboss", 1 );
		EngFuncs::CvarSetValue( "gl_water_refraction", 1 );
		EngFuncs::CvarSetValue( "gl_exposure", 0 );
		EngFuncs::CvarSetValue( "gl_bump", 0 );
		EngFuncs::CvarSetValue( "gl_specular", 1 );
		EngFuncs::CvarSetValue( "gl_cubemaps", 1 );
		EngFuncs::CvarSetValue( "gl_water_planar", 0 );
		EngFuncs::CvarSetValue( "r_mirrorquality", 2 );
		EngFuncs::CvarSetValue( "r_shadowquality", 1 );
		EngFuncs::CvarSetValue( "gl_ssao", 1 );
	}
	else if( Quality == PRESET_HIGH )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 8 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 1 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 1 );
		EngFuncs::CvarSetValue( "gl_msaa", 1 );
		EngFuncs::CvarSetValue( "r_bloom", 1 );
		EngFuncs::CvarSetValue( "r_blur", 1 );
		EngFuncs::CvarSetValue( "gl_lensflare", 1 );
		EngFuncs::CvarSetValue( "gl_emboss", 1 );
		EngFuncs::CvarSetValue( "gl_water_refraction", 1 );
		EngFuncs::CvarSetValue( "gl_exposure", 1 );
		EngFuncs::CvarSetValue( "gl_bump", 1 );
		EngFuncs::CvarSetValue( "gl_specular", 1 );
		EngFuncs::CvarSetValue( "gl_cubemaps", 1 );
		EngFuncs::CvarSetValue( "gl_water_planar", 0 );
		EngFuncs::CvarSetValue( "r_mirrorquality", 3 );
		EngFuncs::CvarSetValue( "r_shadowquality", 2 );
		EngFuncs::CvarSetValue( "gl_ssao", 2 );
	}
	else if( Quality == PRESET_MAXIMUM )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 16 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 1 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 1 );
		EngFuncs::CvarSetValue( "gl_msaa", 1 );
		EngFuncs::CvarSetValue( "r_bloom", 1 );
		EngFuncs::CvarSetValue( "r_blur", 1 );
		EngFuncs::CvarSetValue( "gl_lensflare", 1 );
		EngFuncs::CvarSetValue( "gl_emboss", 1 );
		EngFuncs::CvarSetValue( "gl_water_refraction", 1 );
		EngFuncs::CvarSetValue( "gl_exposure", 1 );
		EngFuncs::CvarSetValue( "gl_bump", 1);
		EngFuncs::CvarSetValue( "gl_specular", 1 );
		EngFuncs::CvarSetValue( "gl_cubemaps", 1 );
		EngFuncs::CvarSetValue( "gl_water_planar", 0 );
		EngFuncs::CvarSetValue( "r_mirrorquality", 4 );
		EngFuncs::CvarSetValue( "r_shadowquality", 3 );
		EngFuncs::CvarSetValue( "gl_ssao", 3 );
	}
}

void CMenuVidOptions::UpdateConfig( void )
{
	float val1 = RemapVal( gammaIntensity.GetCurrentValue(), 0.0, 1.0, 1.8, 3.0 );
	float val2 = RemapVal( glareReduction.GetCurrentValue(), 0.0, 1.0, 0.0, 3.0 );
	EngFuncs::CvarSetValue( "gamma", val1 );
	EngFuncs::CvarSetValue( "brightness", val2 );
	EngFuncs::ProcessImage( hTestImage, val1, val2 );
}

void CMenuVidOptions::GetConfig( void )
{
	float val1 = EngFuncs::GetCvarFloat( "gamma" );
	float val2 = EngFuncs::GetCvarFloat( "brightness" );

	gammaIntensity.SetCurrentValue( RemapVal( val1, 1.8f, 3.0f, 0.0f, 1.0f ) );
	glareReduction.SetCurrentValue( RemapVal( val2, 0.0f, 3.0f, 0.0f, 1.0f ) );
	EngFuncs::ProcessImage( hTestImage, val1, val2 );

	gammaIntensity.SetOriginalValue( val1 );
	glareReduction.SetOriginalValue( val2 );

	cl_muzzlelight.LinkCvar( "cl_muzzlelight" );
	sv_fadecorpses.LinkCvar( "sv_fadecorpses" );
	gl_sunshafts.LinkCvar( "gl_sunshafts" );
	//	r_shadows.LinkCvar( "r_shadows" );
	r_bloom.LinkCvar( "r_bloom" );
	r_blur.LinkCvar( "r_blur" );
	shadowQ.LinkCvar( "r_shadowquality", CMenuEditable::CVAR_VALUE );
	mirrorQ.LinkCvar( "r_mirrorquality", CMenuEditable::CVAR_VALUE );
	ssaoQ.LinkCvar( "gl_ssao", CMenuEditable::CVAR_VALUE );
	gl_msaa.LinkCvar( "gl_msaa" );
	gl_lensflare.LinkCvar( "gl_lensflare" );
	gl_emboss.LinkCvar( "gl_emboss" );
	gl_bump.LinkCvar( "gl_bump" );
	gl_specular.LinkCvar( "gl_specular" );
	gl_water_refraction.LinkCvar( "gl_water_refraction" );
	gl_exposure.LinkCvar( "gl_exposure" );
	gl_cubemaps.LinkCvar( "gl_cubemaps" );
	gl_water_planar.LinkCvar( "gl_water_planar" );

	// special case here...
	// this sucks. fix it!
//	gl_anisotropy.LinkCvar( "gl_anisotropy", CMenuEditable::CVAR_VALUE );
	int AnisotropyValue = (int)EngFuncs::GetCvarFloat("gl_anisotropy");
	switch( AnisotropyValue )
	{
	default:
	case 1: gl_anisotropy.SetCurrentValue( 0.0f ); break;
	case 2: gl_anisotropy.SetCurrentValue( 1.0f ); break;
	case 4: gl_anisotropy.SetCurrentValue( 2.0f ); break;
	case 8: gl_anisotropy.SetCurrentValue( 3.0f ); break;
	case 16: gl_anisotropy.SetCurrentValue( 4.0f ); break;
	}
}

void CMenuVidOptions::WriteAnisotropy(void)
{
	int AnisotropyValue = gl_anisotropy.GetCurrentValue();
	switch( AnisotropyValue )
	{
	default:
	case 0: EngFuncs::CvarSetValue( "gl_anisotropy", 1 ); break;
	case 1: EngFuncs::CvarSetValue( "gl_anisotropy", 2 ); break;
	case 2: EngFuncs::CvarSetValue( "gl_anisotropy", 4 ); break;
	case 3: EngFuncs::CvarSetValue( "gl_anisotropy", 8 ); break;
	case 4: EngFuncs::CvarSetValue( "gl_anisotropy", 16 ); break;
	}
}

void CMenuVidOptions::SaveAndPopMenu( void )
{
#if LEGACY_VIEWSIZE
	screenSize.WriteCvar();
#endif
	// gamma and brightness are already written
//	glareReduction.WriteCvar();

	
	maxFPS.WriteCvar();
	cl_muzzlelight.WriteCvar();
	sv_fadecorpses.WriteCvar();
	gl_sunshafts.WriteCvar();
	//	r_shadows.WriteCvar();
	r_bloom.WriteCvar();
	r_blur.WriteCvar();
	shadowQ.WriteCvar();
	mirrorQ.WriteCvar();
	ssaoQ.WriteCvar();
	gl_msaa.WriteCvar();
	gl_lensflare.WriteCvar();
	gl_emboss.WriteCvar();
	gl_bump.WriteCvar();
	gl_specular.WriteCvar();
	gl_water_refraction.WriteCvar();
	gl_exposure.WriteCvar();
	gl_cubemaps.WriteCvar();
	gl_water_planar.WriteCvar();

	// special case here...
//	gl_anisotropy.WriteCvar();
	WriteAnisotropy();

	CMenuFramework::SaveAndPopMenu();
}

/*
=================
CMenuVidOptions::Ownerdraw
=================
*/
void CMenuVidOptions::CMenuVidPreview::Draw( )
{
	int		color = 0xFFFF0000; // 255, 0, 0, 255
	int		viewport[4];
	int		viewsize, size, sb_lines;

#if LEGACY_VIEWSIZE
	viewsize = EngFuncs::GetCvarFloat( "viewsize" );
#else
	viewsize = 120;
#endif

	if( viewsize >= 120 )
		sb_lines = 0;	// no status bar at all
	else if( viewsize >= 110 )
		sb_lines = 24;	// no inventory
	else sb_lines = 48;

	size = Q_min( viewsize, 100 );

	viewport[2] = m_scSize.w * size / 100;
	viewport[3] = m_scSize.h * size / 100;

	if( viewport[3] > m_scSize.h - sb_lines )
		viewport[3] = m_scSize.h - sb_lines;
	if( viewport[3] > m_scSize.h )
		viewport[3] = m_scSize.h;

	viewport[2] &= ~7;
	viewport[3] &= ~1;

	viewport[0] = (m_scSize.w - viewport[2]) / 2;
	viewport[1] = (m_scSize.h - sb_lines - viewport[3]) / 2;

	UI_DrawPic( m_scPos.x + viewport[0], m_scPos.y + viewport[1], viewport[2], viewport[3], uiColorWhite, szPic );
	UI_DrawRectangleExt( m_scPos, m_scSize, color, ((CMenuVidOptions*)Parent())->outlineWidth );
}

/*
=================
CMenuVidOptions::Init
=================
*/
void CMenuVidOptions::_Init( void )
{
#ifdef PIC_KEEP_RGBDATA
	hTestImage = EngFuncs::PIC_Load( ART_GAMMA, PIC_KEEP_RGBDATA );
#else
	hTestImage = EngFuncs::PIC_Load( ART_GAMMA, PIC_KEEP_SOURCE | PIC_EXPAND_SOURCE );
#endif

	static const char *ShadowqualityStr[] =
	{
	//	L( "Low (256p)" ), L( "Medium (512p)" ), L( "High (1024p)" ), L( "Maximum (2048p)" ), L( "Extreme (4096p)" )
		L( "GameUI_Low256" ), L( "GameUI_Medium512" ), L( "GameUI_High1024" ), L( "GameUI_Maximum2048" )
	};

	static const char *MirrorqualityStr[] =
	{
		L( "GameUI_Off" ), L( "GameUI_Low" ), L( "GameUI_Medium" ), L( "GameUI_High" ), L( "GameUI_Maximum" )
	};

	static const char *SSAOqualityStr[] =
	{
		L( "GameUI_Off" ), L( "GameUI_Low" ), L( "GameUI_Medium" ), L( "GameUI_Maximum" )
	};

	static const char *AnisotropyStr[] =
	{
		L( "1x" ), L( "2x" ), L( "4x" ), L( "8x" ), L( "16x" )
	};

	banner.SetPicture( ART_BANNER );

	testImage.iFlags = QMF_INACTIVE;
	testImage.SetRect( 390, 225, 480, 450 );
	testImage.SetPicture( ART_GAMMA );

	gl_sunshafts.SetNameAndStatus( L( "GameUI_Sunshafts" ), L( "-" ) );
	gl_sunshafts.iFlags |= QMF_NOTIFY;
	gl_sunshafts.SetCoord( 372, MenuYOffset - 50 );
	gl_sunshafts.onChanged = CMenuEditable::WriteCvarCb;

	//	r_shadows.SetNameAndStatus( L( "Dynamic shadows" ), L( "-" ) );
	//	r_shadows.iFlags |= QMF_NOTIFY;
	//	r_shadows.SetCoord( 372, MenuYOffset + 0 );
	//	r_shadows.onChanged = CMenuEditable::WriteCvarCb;

	r_bloom.SetNameAndStatus( L( "GameUI_Bloom" ), L( "-" ) );
	r_bloom.iFlags |= QMF_NOTIFY;
	r_bloom.SetCoord( 672, MenuYOffset - 50 );
	r_bloom.onChanged = CMenuEditable::WriteCvarCb;

	r_blur.SetNameAndStatus( L( "GameUI_Blur" ), L( "-" ) );
	r_blur.iFlags |= QMF_NOTIFY;
	r_blur.SetCoord( 672, MenuYOffset + 0 );
	r_blur.onChanged = CMenuEditable::WriteCvarCb;

	cl_muzzlelight.SetNameAndStatus( L( "GameUI_LightFromGunshots" ), L( "-" ) );
	cl_muzzlelight.iFlags |= QMF_NOTIFY;
	cl_muzzlelight.SetCoord( 372, MenuYOffset + 0 );

	sv_fadecorpses.SetNameAndStatus( L( "GameUI_FadeCorpses" ), L( "-" ) );
	sv_fadecorpses.iFlags |= QMF_NOTIFY;
	sv_fadecorpses.SetCoord( 372, MenuYOffset + 50 );

	maxFPS.szName = L( "GameUI_FPSlimit" );
	maxFPS.szStatusText = L( "Cap your game frame rate" );
	maxFPS.Setup( 25, 200, 5 );
	maxFPS.LinkCvar( "fps_max", CMenuEditable::CVAR_VALUE );
	maxFPS.SetRect( 72, MenuYOffset + 200, 220, 32 );

	gl_msaa.SetNameAndStatus( L( "GameUI_MSAA" ), L( "-" ) );
	gl_msaa.iFlags |= QMF_NOTIFY;
	gl_msaa.SetCoord( 372, MenuYOffset + 100 );
	gl_msaa.onChanged = CMenuEditable::WriteCvarCb;

	gl_lensflare.SetNameAndStatus( L( "GameUI_LensFlare" ), L( "-" ) );
	gl_lensflare.iFlags |= QMF_NOTIFY;
	gl_lensflare.SetCoord( 672, MenuYOffset + 50 );
	gl_lensflare.onChanged = CMenuEditable::WriteCvarCb;

	gl_exposure.SetNameAndStatus( L( "GameUI_AutoExposure" ), L( "-" ) );
	gl_exposure.iFlags |= QMF_NOTIFY;
	gl_exposure.SetCoord( 972, MenuYOffset + 0 );
	gl_exposure.onChanged = CMenuEditable::WriteCvarCb;

	gl_cubemaps.SetNameAndStatus( L( "GameUI_Cubemaps" ), L( "-" ) );
	gl_cubemaps.iFlags |= QMF_NOTIFY;
	gl_cubemaps.SetCoord( 372, MenuYOffset + 300 );
	gl_cubemaps.onChanged = CMenuEditable::WriteCvarCb;

	gl_emboss.SetNameAndStatus( L( "GameUI_Emboss" ), L( "-" ) );
	gl_emboss.iFlags |= QMF_NOTIFY;
	gl_emboss.SetCoord( 672, MenuYOffset + 100 );
	gl_emboss.onChanged = CMenuEditable::WriteCvarCb;

	gl_bump.SetNameAndStatus( L( "GameUI_Bump" ), L( "-" ) );
	gl_bump.iFlags |= QMF_NOTIFY;
	gl_bump.SetCoord( 972, MenuYOffset + 50 );
	gl_bump.onChanged = CMenuEditable::WriteCvarCb;

	gl_specular.SetNameAndStatus( L( "GameUI_Specular" ), L( "-" ) );
	gl_specular.iFlags |= QMF_NOTIFY;
	gl_specular.SetCoord( 972, MenuYOffset + 100 );
	gl_specular.onChanged = CMenuEditable::WriteCvarCb;

	gl_water_refraction.SetNameAndStatus( L( "GameUI_WaterRefraction" ), L( "-" ) );
	gl_water_refraction.iFlags |= QMF_NOTIFY;
	gl_water_refraction.SetCoord( 972, MenuYOffset - 50 );
	gl_water_refraction.onChanged = CMenuEditable::WriteCvarCb;

	done.SetNameAndStatus( L( "GameUI_OK" ), L( "Go back to the Video Menu" ) );
	done.SetCoord( 72, MenuYOffset + 500 );
	done.SetPicture( PC_DONE );
	done.onReleased = VoidCb( &CMenuVidOptions::SaveAndPopMenu );

	SetToLow.SetNameAndStatus( L( "GameUI_SetToLow" ), L( "-" ) );
	SetToLow.SetCoord( 72, MenuYOffset - 50 );
	SetToLow.SetPicture( 0 );
	SET_EVENT( SetToLow.onPressed, SetSettingsTo( 0 ) );
	SetToLow.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToMedium.SetNameAndStatus( L( "GameUI_SetToMedium" ), L( "-" ) );
	SetToMedium.SetCoord( 72, MenuYOffset + 0 );
	//	SetToMedium.SetPicture( PC_DONE );
	SET_EVENT( SetToMedium.onPressed, SetSettingsTo( 1 ) );
	SetToMedium.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToHigh.SetNameAndStatus( L( "GameUI_SetToHigh" ), L( "-" ) );
	SetToHigh.SetCoord( 72, MenuYOffset + 50 );
	//	SetToHigh.SetPicture( PC_DONE );
	SET_EVENT( SetToHigh.onPressed, SetSettingsTo( 2 ) );
	SetToHigh.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToMaximum.SetNameAndStatus( L( "GameUI_SetToMaximum" ), L( "-" ) );
	SetToMaximum.SetCoord( 72, MenuYOffset + 100 );
	//	SetToMaximum.SetPicture( PC_DONE );
	SET_EVENT( SetToMaximum.onPressed, SetSettingsTo( 3 ) );
	SetToMaximum.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	int height = 280;

#if LEGACY_VIEWSIZE
	screenSize.SetNameAndStatus( L( "Screen size" ), L( "Set the screen size" ) );
	screenSize.SetCoord( 72, height );
	screenSize.Setup( 30, 120, 10 );
	screenSize.onChanged = CMenuEditable::WriteCvarCb;

	height += 60;
#endif

	gammaIntensity.SetNameAndStatus( L( "GameUI_Gamma" ), L( "Set gamma value" ) );
	gammaIntensity.SetCoord( 72, MenuYOffset + 400 );
	gammaIntensity.Setup( 0.0, 1.0, 0.025 );
	gammaIntensity.onChanged = VoidCb( &CMenuVidOptions::UpdateConfig );
	gammaIntensity.onCvarGet = VoidCb( &CMenuVidOptions::GetConfig );

	glareReduction.SetCoord( 72, MenuYOffset + 460 );
	glareReduction.SetNameAndStatus( L( "GameUI_Brightness" ), L( "Set brightness level" ) );
	glareReduction.Setup( 0, 1.0, 0.025 );
	glareReduction.onChanged = VoidCb( &CMenuVidOptions::UpdateConfig );
	glareReduction.onCvarGet = VoidCb( &CMenuVidOptions::GetConfig );

//	vbo.SetNameAndStatus( L( "Use VBO" ), L( "Use new world renderer. Faster, but rarely glitchy" ) );
//	vbo.SetCoord( 72, MenuYOffset + 565 );

	static CStringArrayModel mi( MirrorqualityStr, V_ARRAYSIZE( MirrorqualityStr ) );
	mirrorQ.SetNameAndStatus( L( "GameUI_MirrorQuality" ), L( "Mirror reflection quality" ) );
	mirrorQ.Setup( &mi );
	mirrorQ.onChanged = CMenuEditable::WriteCvarCb;
	mirrorQ.font = QM_SMALLFONT;
	mirrorQ.SetRect( 372, MenuYOffset + 200, 220, 32 );

	gl_water_planar.SetNameAndStatus( L( "GameUI_WaterPlanar" ), L( "-" ) );
	gl_water_planar.iFlags |= QMF_NOTIFY;
	gl_water_planar.SetCoord( 372, MenuYOffset + 240 );
	gl_water_planar.onChanged = CMenuEditable::WriteCvarCb;

	static CStringArrayModel ani( AnisotropyStr, V_ARRAYSIZE( AnisotropyStr ) );
	gl_anisotropy.SetNameAndStatus( L( "GameUI_Anisotropy" ), L( "Anisotropic filtering" ) );
	gl_anisotropy.Setup( &ani );
	gl_anisotropy.onChanged = VoidCb(&CMenuVidOptions::WriteAnisotropy);
	gl_anisotropy.font = QM_SMALLFONT;
	gl_anisotropy.SetRect( 672, MenuYOffset + 200, 220, 32 );

	static CStringArrayModel sh( ShadowqualityStr, V_ARRAYSIZE( ShadowqualityStr ) );
	shadowQ.SetNameAndStatus( L( "GameUI_ShadowQuality" ), L( "Dynamic shadow quality" ) );
	shadowQ.Setup( &sh );
	shadowQ.onChanged = CMenuEditable::WriteCvarCb;
	shadowQ.font = QM_SMALLFONT;
	shadowQ.SetRect( 972, MenuYOffset + 200, 300, 32 );

	static CStringArrayModel ssao( SSAOqualityStr, V_ARRAYSIZE( SSAOqualityStr ) );
	ssaoQ.SetNameAndStatus( L( "GameUI_SSAOQuality" ), L( "SSAO quality" ) );
	ssaoQ.Setup( &ssao );
	ssaoQ.onChanged = CMenuEditable::WriteCvarCb;
	ssaoQ.font = QM_SMALLFONT;
	ssaoQ.SetRect( 72, MenuYOffset + 300, 220, 32 );

	AddItem( background );
	AddItem( banner );
	AddItem( done );
	AddItem( SetToLow );
	AddItem( SetToMedium );
	AddItem( SetToHigh );
	AddItem( SetToMaximum );

	// diffusion - disabled some of those, I don't need them for now...
#if LEGACY_VIEWSIZE
	AddItem( screenSize );
#endif
	AddItem( gammaIntensity );
	AddItem( glareReduction );
	/*
			AddItem( vbo );
	*/
	//	AddItem( testImage );
	AddItem( gl_sunshafts );
	AddItem( sv_fadecorpses );
	AddItem( cl_muzzlelight );
	//	AddItem( r_shadows );
	AddItem( r_bloom );
	AddItem( r_blur );
	AddItem( maxFPS );
	AddItem( shadowQ );
	AddItem( mirrorQ );
	AddItem( ssaoQ );
	AddItem( gl_anisotropy );
	AddItem( gl_msaa );
	AddItem( gl_lensflare );
	AddItem( gl_emboss );
	AddItem( gl_water_refraction );
	AddItem( gl_exposure );
	AddItem( gl_cubemaps );
	AddItem( gl_water_planar );
	AddItem( gl_bump );
	AddItem( gl_specular );
#if LEGACY_VIEWSIZE
	screenSize.LinkCvar( "viewsize" );
#endif
	gammaIntensity.LinkCvar( "gamma" );
	glareReduction.LinkCvar( "brightness" );
//	vbo.LinkCvar( "gl_vbo" );
}

void CMenuVidOptions::_VidInit()
{
	outlineWidth = 2;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

void CMenuVidOptions::Show( void )
{
	CMenuFramework::Show();
	EngFuncs::ClientCmd( FALSE, "menuactivate vidoptions\n" );
}

void CMenuVidOptions::Hide( void )
{
	CMenuFramework::Hide();
	EngFuncs::ClientCmd( FALSE, "menuactivate options\n" );
}

ADD_MENU( menu_vidoptions, CMenuVidOptions, UI_VidOptions_Menu );
