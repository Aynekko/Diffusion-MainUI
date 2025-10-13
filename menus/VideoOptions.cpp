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
#include "Field.h"

#define ART_BANNER	  	"gfx/shell/head_vidoptions"
#define ART_GAMMA		"gfx/shell/gamma"

#define GAMMA_MAX 3.0f
#define BRIGHTNESS_MAX 3.0f

#define GAMMA_SLIDER 0

#define LEGACY_VIEWSIZE 0
#define FPS_LIMITER_CHECKBOX 0

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
	void UpdateConfig();
	void GetConfig();
	void Show();
	void Hide();
	void Draw( void );

	void WriteAnisotropy( void );
	void SetMaxFps( void );
	void SetCurFps( void );
	void SetVSync( void );
	void HideMenus( void );

	int	outlineWidth;

	int fps_remember;

	CMenuPicButton done;
#if FPS_LIMITER_CHECKBOX
	CMenuSpinControl maxFPS;
#else
	CMenuSlider	maxFPS;
	CMenuField maxFPSdisplay;
#endif
	CMenuCheckBox FPSunlimited;
	CMenuCheckBox gl_vsync;
	CMenuCheckBox hideMenus;

	CMenuPicButton	SetToLow;
	CMenuPicButton	SetToMedium;
	CMenuPicButton	SetToHigh;
	CMenuPicButton	SetToMaximum;

#if GAMMA_SLIDER
	CMenuSlider	gammaIntensity;
#endif
	CMenuSlider	Brightness;
	CMenuCheckBox	fastSky;
	CMenuCheckBox   vbo;
	CMenuCheckBox   bump;
	CMenuCheckBox	cl_muzzlelight;
	CMenuCheckBox	gl_hbao;
	CMenuCheckBox	gl_sunshafts;
//	CMenuCheckBox	r_shadows;
	CMenuCheckBox	r_bloom;
	CMenuCheckBox	r_blur;
	CMenuSpinControl shadowQ;
	CMenuSpinControl mirrorQ;
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
	CMenuSpinControl gl_renderscale;
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
		EngFuncs::CvarSetValue( "gl_hbao", 0 );
		EngFuncs::CvarSetValue( "cl_lod_enable", 1 );
	}
	else if( Quality == PRESET_MEDIUM )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 4 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 0 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 1 );
		EngFuncs::CvarSetValue( "gl_msaa", 1 );
		EngFuncs::CvarSetValue( "r_bloom", 0 );
		EngFuncs::CvarSetValue( "r_blur", 0 );
		EngFuncs::CvarSetValue( "gl_lensflare", 0 );
		EngFuncs::CvarSetValue( "gl_emboss", 0 );
		EngFuncs::CvarSetValue( "gl_water_refraction", 1 );
		EngFuncs::CvarSetValue( "gl_exposure", 0 );
		EngFuncs::CvarSetValue( "gl_bump", 1 );
		EngFuncs::CvarSetValue( "gl_specular", 0 );
		EngFuncs::CvarSetValue( "gl_cubemaps", 1 );
		EngFuncs::CvarSetValue( "gl_water_planar", 0 );
		EngFuncs::CvarSetValue( "r_mirrorquality", 2 );
		EngFuncs::CvarSetValue( "r_shadowquality", 1 );
		EngFuncs::CvarSetValue( "gl_hbao", 0 );
		EngFuncs::CvarSetValue( "cl_lod_enable", 1 );
	}
	else if( Quality == PRESET_HIGH )
	{
		EngFuncs::CvarSetValue( "gl_anisotropy", 8 );
		EngFuncs::CvarSetValue( "gl_sunshafts", 1 );
		EngFuncs::CvarSetValue( "cl_muzzlelight", 1 );
		EngFuncs::CvarSetValue( "gl_msaa", 1 );
		EngFuncs::CvarSetValue( "r_bloom", 0 );
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
		EngFuncs::CvarSetValue( "gl_hbao", 0 );
		EngFuncs::CvarSetValue( "cl_lod_enable", 0 );
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
		EngFuncs::CvarSetValue( "gl_hbao", 1 );
		EngFuncs::CvarSetValue( "cl_lod_enable", 0 );

		EngFuncs::ClientCmd( false, "menuactivate video_maximum\n" );
	}
}

void CMenuVidOptions::UpdateConfig( void )
{
#if GAMMA_SLIDER
	float val1 = RemapVal( gammaIntensity.GetCurrentValue(), 0.0, 1.0, 1.8, GAMMA_MAX );
	EngFuncs::CvarSetValue( "gamma", val1 );
#endif
	float val2 = RemapVal( Brightness.GetCurrentValue(), 0.0, 1.0, 0.01, BRIGHTNESS_MAX );
	EngFuncs::CvarSetValue( "brightness", val2 );
}

void CMenuVidOptions::GetConfig( void )
{
#if GAMMA_SLIDER
	float val1 = EngFuncs::GetCvarFloat( "gamma" );
	gammaIntensity.SetCurrentValue( RemapVal( val1, 1.8f, GAMMA_MAX, 0.0f, 1.0f ) );
	gammaIntensity.SetOriginalValue( val1 );
#endif
	float val2 = EngFuncs::GetCvarFloat( "brightness" );
	Brightness.SetCurrentValue( RemapVal( val2, 0.0f, BRIGHTNESS_MAX, 0.0f, 1.0f ) );
	Brightness.SetOriginalValue( val2 );

	cl_muzzlelight.LinkCvar( "cl_muzzlelight" );
	gl_hbao.LinkCvar( "gl_hbao" );
	gl_sunshafts.LinkCvar( "gl_sunshafts" );
	//	r_shadows.LinkCvar( "r_shadows" );
	r_bloom.LinkCvar( "r_bloom" );
	r_blur.LinkCvar( "r_blur" );
	shadowQ.LinkCvar( "r_shadowquality", CMenuEditable::CVAR_VALUE );
	mirrorQ.LinkCvar( "r_mirrorquality", CMenuEditable::CVAR_VALUE );
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

	bool vsync = (EngFuncs::GetCvarFloat( "gl_vsync" ) == 1.0f);
	float fps = EngFuncs::GetCvarFloat( "fps_max" );
	if( fps == 0.0f )
	{
		maxFPS.iFlags |= QMF_GRAYED;
		if( fps_remember > 0 )
			maxFPS.SetCurrentValue( fps_remember );
		else
		{
			fps_remember = 100;
			maxFPS.SetCurrentValue( 100 );
		}
		FPSunlimited.bChecked = true;
		EngFuncs::CvarSetValue( "fps_max", 0 ); // SetCurrentValue sets the cvar...duh
		if( vsync )
		{
			gl_vsync.bChecked = true;
			FPSunlimited.iFlags |= QMF_GRAYED;
		}
	}
	else
	{
		maxFPS.iFlags &= ~QMF_GRAYED;
		fps_remember = fps; // remember fps if user changes his mind
		maxFPS.SetCurrentValue( fps );
		FPSunlimited.bChecked = false;
		if( vsync )
		{
			gl_vsync.bChecked = true;
			FPSunlimited.iFlags |= QMF_GRAYED;
			maxFPS.iFlags |= QMF_GRAYED;
		}
	}

#if !FPS_LIMITER_CHECKBOX
	char m_szValue[8];
	snprintf( m_szValue, sizeof( m_szValue ), "%i", (int)maxFPS.GetCurrentValue() );
	maxFPSdisplay.SetBuffer( m_szValue );
#endif
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
	
	cl_muzzlelight.WriteCvar();
	gl_hbao.WriteCvar();
	gl_sunshafts.WriteCvar();
	//	r_shadows.WriteCvar();
	r_bloom.WriteCvar();
	r_blur.WriteCvar();
	shadowQ.WriteCvar();
	mirrorQ.WriteCvar();
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

	SetMaxFps();

	CMenuFramework::SaveAndPopMenu();
}

void CMenuVidOptions::Draw( void )
{
	char fps[32];
	int fpsvalue = UI_GetFPS();
	sprintf_s( fps, "%i FPS", fpsvalue );

	unsigned int txtcolor = g_iColorTable[1]; // red
	if( fpsvalue >= 30 && fpsvalue < 60 )
		txtcolor = g_iColorTable[3]; // yellow
	else if( fpsvalue >= 60 )
		txtcolor = g_iColorTable[2]; // green
	
	// I can't figure out how point positioning works in this function, but this does nicely.
	UI_DrawString( font, Point( 0, ScreenHeight / 2 ), m_scSize, fps, txtcolor, m_scChSize, QM_CENTER, ETF_SHADOW );

	BaseClass::Draw();
}

void CMenuVidOptions::SetMaxFps( void )
{
	maxFPS.SetCurrentValue( fps_remember );

	if( FPSunlimited.bChecked )
	{
		maxFPS.iFlags |= QMF_GRAYED;
		EngFuncs::CvarSetValue( "fps_max", 0 );
	}
	else
	{
		maxFPS.iFlags &= ~QMF_GRAYED;
		EngFuncs::CvarSetValue( "fps_max", fps_remember );
	}
}

void CMenuVidOptions::SetCurFps( void )
{
	EngFuncs::CvarSetValue( "fps_max", (int)maxFPS.GetCurrentValue() );
	fps_remember = (int)maxFPS.GetCurrentValue();

#if !FPS_LIMITER_CHECKBOX
	char m_szValue[8];
	snprintf( m_szValue, sizeof( m_szValue ), "%i", fps_remember );
	maxFPSdisplay.SetBuffer( m_szValue );
#endif
}

void CMenuVidOptions::SetVSync( void )
{
	if( gl_vsync.bChecked )
	{
		EngFuncs::CvarSetValue( "gl_vsync", 1 );
		maxFPS.iFlags |= QMF_GRAYED;
		FPSunlimited.iFlags |= QMF_GRAYED;
	}
	else
	{
		EngFuncs::CvarSetValue( "gl_vsync", 0 );
		maxFPS.iFlags &= ~QMF_GRAYED;
		FPSunlimited.iFlags &= ~QMF_GRAYED;
	}
}

void CMenuVidOptions::HideMenus( void )
{
	if( hideMenus.bChecked )
	{
		SetToLow.iFlags |= QMF_HIDDEN;
		SetToMedium.iFlags |= QMF_HIDDEN;
		SetToHigh.iFlags |= QMF_HIDDEN;
		SetToMaximum.iFlags |= QMF_HIDDEN;
#if GAMMA_SLIDER
		gammaIntensity.iFlags |= QMF_HIDDEN;
#endif
		Brightness.iFlags |= QMF_HIDDEN;
		gl_sunshafts.iFlags |= QMF_HIDDEN;
		gl_hbao.iFlags |= QMF_HIDDEN;
		cl_muzzlelight.iFlags |= QMF_HIDDEN;
		r_bloom.iFlags |= QMF_HIDDEN;
		r_blur.iFlags |= QMF_HIDDEN;
		maxFPS.iFlags |= QMF_HIDDEN;
#if !FPS_LIMITER_CHECKBOX
		maxFPSdisplay.iFlags |= QMF_HIDDEN;
#endif
		FPSunlimited.iFlags |= QMF_HIDDEN;
		gl_vsync.iFlags |= QMF_HIDDEN;
		shadowQ.iFlags |= QMF_HIDDEN;
		mirrorQ.iFlags |= QMF_HIDDEN;
		gl_anisotropy.iFlags |= QMF_HIDDEN;
		gl_msaa.iFlags |= QMF_HIDDEN;
		gl_lensflare.iFlags |= QMF_HIDDEN;
		gl_emboss.iFlags |= QMF_HIDDEN;
		gl_water_refraction.iFlags |= QMF_HIDDEN;
		gl_exposure.iFlags |= QMF_HIDDEN;
		gl_cubemaps.iFlags |= QMF_HIDDEN;
		gl_water_planar.iFlags |= QMF_HIDDEN;
		gl_bump.iFlags |= QMF_HIDDEN;
		gl_specular.iFlags |= QMF_HIDDEN;
		gl_renderscale.iFlags |= QMF_HIDDEN;
	}
	else
	{
		SetToLow.iFlags &= ~QMF_HIDDEN;
		SetToMedium.iFlags &= ~QMF_HIDDEN;
		SetToHigh.iFlags &= ~QMF_HIDDEN;
		SetToMaximum.iFlags &= ~QMF_HIDDEN;
#if GAMMA_SLIDER
		gammaIntensity.iFlags &= ~QMF_HIDDEN;
#endif
		Brightness.iFlags &= ~QMF_HIDDEN;
		gl_sunshafts.iFlags &= ~QMF_HIDDEN;
		gl_hbao.iFlags &= ~QMF_HIDDEN;
		cl_muzzlelight.iFlags &= ~QMF_HIDDEN;
		r_bloom.iFlags &= ~QMF_HIDDEN;
		r_blur.iFlags &= ~QMF_HIDDEN;
		maxFPS.iFlags &= ~QMF_HIDDEN;
#if !FPS_LIMITER_CHECKBOX
		maxFPSdisplay.iFlags &= ~QMF_HIDDEN;
#endif
		FPSunlimited.iFlags &= ~QMF_HIDDEN;
		gl_vsync.iFlags &= ~QMF_HIDDEN;
		shadowQ.iFlags &= ~QMF_HIDDEN;
		mirrorQ.iFlags &= ~QMF_HIDDEN;
		gl_anisotropy.iFlags &= ~QMF_HIDDEN;
		gl_msaa.iFlags &= ~QMF_HIDDEN;
		gl_lensflare.iFlags &= ~QMF_HIDDEN;
		gl_emboss.iFlags &= ~QMF_HIDDEN;
		gl_water_refraction.iFlags &= ~QMF_HIDDEN;
		gl_exposure.iFlags &= ~QMF_HIDDEN;
		gl_cubemaps.iFlags &= ~QMF_HIDDEN;
		gl_water_planar.iFlags &= ~QMF_HIDDEN;
		gl_bump.iFlags &= ~QMF_HIDDEN;
		gl_specular.iFlags &= ~QMF_HIDDEN;
		gl_renderscale.iFlags &= ~QMF_HIDDEN;
	}
}

/*
=================
CMenuVidOptions::Init
=================
*/
void CMenuVidOptions::_Init( void )
{
	fps_remember = EngFuncs::GetCvarFloat( "fps_max" );

	static const char *ShadowqualityStr[] =
	{
	//	L( "Low (256p)" ), L( "Medium (512p)" ), L( "High (1024p)" ), L( "Maximum (2048p)" ), L( "Extreme (4096p)" )
		L( "GameUI_Low256" ), L( "GameUI_Medium512" ), L( "GameUI_High1024" ), L( "GameUI_Maximum2048" )
	};

	static const char *MirrorqualityStr[] =
	{
		L( "GameUI_Off" ), L( "GameUI_Low" ), L( "GameUI_Medium" ), L( "GameUI_High" ), L( "GameUI_Maximum" )
	};

	static const char *AnisotropyStr[] =
	{
		L( "1x" ), L( "2x" ), L( "4x" ), L( "8x" ), L( "16x" )
	};

	banner.SetPicture( ART_BANNER );

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
	cl_muzzlelight.onChanged = CMenuEditable::WriteCvarCb;

	gl_hbao.SetNameAndStatus( L( "GameUI_AO" ), L( "-" ) );
	gl_hbao.iFlags |= QMF_NOTIFY;
	gl_hbao.SetCoord( 372, MenuYOffset + 50 );
	gl_hbao.onChanged = CMenuEditable::WriteCvarCb;

	maxFPS.szName = L( "GameUI_FPSlimit" );
	maxFPS.szStatusText = L( "Cap your game frame rate" );
#if FPS_LIMITER_CHECKBOX
	maxFPS.Setup( 25, 200, 5 );
	maxFPS.SetRect( 72, MenuYOffset + 200, 220, 32 );
#else
	const int fps_w = 275;
	maxFPS.Setup( 20, 200, 1 );
	maxFPS.SetCoord( 72, MenuYOffset + 220 );
	maxFPS.SetSize( fps_w, 0 );
	maxFPSdisplay.bNumbersOnly = true;
	maxFPSdisplay.SetRect( maxFPS.pos.x + fps_w - 80, maxFPS.pos.y - 38, 80, 32 );
	maxFPSdisplay.bDisplayOnly = true;
#endif
	maxFPS.onChanged = VoidCb( &CMenuVidOptions::SetCurFps );

	FPSunlimited.SetNameAndStatus( L( "GameUI_FPSunlimited" ), L( "-" ) );
	FPSunlimited.iFlags |= QMF_NOTIFY;
	FPSunlimited.SetCoord( 72, MenuYOffset + 250 );
	FPSunlimited.onChanged = VoidCb( &CMenuVidOptions::SetMaxFps );

	gl_vsync.SetNameAndStatus( L( "GameUI_VSync" ), L( "-" ) );
	gl_vsync.iFlags |= QMF_NOTIFY;
	gl_vsync.SetCoord( 72, MenuYOffset + 300 );
	gl_vsync.onChanged = VoidCb( &CMenuVidOptions::SetVSync );

	hideMenus.SetNameAndStatus( L( "GameUI_HideMenus" ), L( "-" ) );
	hideMenus.iFlags |= QMF_NOTIFY;
	hideMenus.SetCoord( 300, MenuYOffset + 600 );
	hideMenus.onChanged = VoidCb( &CMenuVidOptions::HideMenus );
	
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
	done.SetCoord( 72, MenuYOffset + 600 );
	done.SetPicture( PC_DONE );
	done.onReleased = VoidCb( &CMenuVidOptions::SaveAndPopMenu );

	SetToLow.SetNameAndStatus( L( "GameUI_SetToLow" ), L( "-" ) );
	SetToLow.SetCoord( 72, MenuYOffset - 50 );
	SET_EVENT( SetToLow.onPressed, SetSettingsTo( 0 ) );
	SetToLow.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToMedium.SetNameAndStatus( L( "GameUI_SetToMedium" ), L( "-" ) );
	SetToMedium.SetCoord( 72, MenuYOffset + 0 );
	SET_EVENT( SetToMedium.onPressed, SetSettingsTo( 1 ) );
	SetToMedium.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToHigh.SetNameAndStatus( L( "GameUI_SetToHigh" ), L( "-" ) );
	SetToHigh.SetCoord( 72, MenuYOffset + 50 );
	SET_EVENT( SetToHigh.onPressed, SetSettingsTo( 2 ) );
	SetToHigh.onReleased = VoidCb( &CMenuVidOptions::GetConfig );

	SetToMaximum.SetNameAndStatus( L( "GameUI_SetToMaximum" ), L( "-" ) );
	SetToMaximum.SetCoord( 72, MenuYOffset + 100 );
	SET_EVENT( SetToMaximum.onPressed, SetSettingsTo( 3 ) );
	SetToMaximum.onReleased = VoidCb( &CMenuVidOptions::GetConfig );
	int gamma_slider_offset = 0;
#if GAMMA_SLIDER
	gammaIntensity.SetNameAndStatus( L( "GameUI_Gamma" ), L( "Set gamma value" ) );
	gammaIntensity.SetCoord( 72, MenuYOffset + 400 );
	gammaIntensity.Setup( 0.0f, 1.0f, 0.1f );
	gammaIntensity.onChanged = VoidCb( &CMenuVidOptions::UpdateConfig );
	gamma_slider_offset += 60;
#endif
	Brightness.SetCoord( 72, MenuYOffset + 400 + gamma_slider_offset );
	Brightness.SetNameAndStatus( L( "GameUI_Brightness" ), L( "Set brightness level" ) );
	Brightness.Setup( 0.0f, 1.0f, 0.1f );
	Brightness.onChanged = VoidCb( &CMenuVidOptions::UpdateConfig );

	gl_renderscale.szName = L( "GameUI_RenderScale" );
	gl_renderscale.szStatusText = L( "-" );
	gl_renderscale.SetDisplayPrecision( 2 );
	gl_renderscale.Setup( 0.25f, 1.00f, 0.05f );
	gl_renderscale.SetRect( 72, MenuYOffset + 480 + gamma_slider_offset, 260, 32 );
	gl_renderscale.LinkCvar( "gl_renderscale", CMenuEditable::CVAR_VALUE );
	gl_renderscale.onChanged = CMenuEditable::WriteCvarCb;

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
#if GAMMA_SLIDER
	AddItem( gammaIntensity );
#endif
	AddItem( Brightness );
	AddItem( gl_sunshafts );
	AddItem( gl_hbao );
	AddItem( cl_muzzlelight );
	//	AddItem( r_shadows );
	AddItem( r_bloom );
	AddItem( r_blur );
	AddItem( maxFPS );
#if !FPS_LIMITER_CHECKBOX
	AddItem( maxFPSdisplay );
#endif
	AddItem( FPSunlimited );
	AddItem( gl_vsync );
	AddItem( hideMenus );
	AddItem( shadowQ );
	AddItem( mirrorQ );
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
#if GAMMA_SLIDER
	gammaIntensity.LinkCvar( "gamma" );
#endif
	Brightness.LinkCvar( "brightness" );
	AddItem( gl_renderscale );
}

void CMenuVidOptions::_VidInit()
{
	outlineWidth = 2;
	UI_ScaleCoords( NULL, NULL, &outlineWidth, NULL );
	GetConfig();
}

void CMenuVidOptions::Show( void )
{
	EngFuncs::ClientCmd( false, "menuactivate vidoptions\n" );
	hideMenus.bChecked = false;
	HideMenus(); // unhide them
	EngFuncs::CvarSetValue( "ui_videooptions_active", 1 );
	CMenuFramework::Show();
}

void CMenuVidOptions::Hide( void )
{
	hideMenus.bChecked = false;
	EngFuncs::ClientCmd( false, "menuactivate options\n" );
	EngFuncs::CvarSetValue( "ui_videooptions_active", 0 );
	CMenuFramework::Hide();
}

ADD_MENU( menu_vidoptions, CMenuVidOptions, UI_VidOptions_Menu );
