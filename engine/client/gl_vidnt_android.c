#ifdef __ANDROID__
#include "common.h"
#include "client.h"
#include "gl_local.h"
#include "mod_local.h"
#include "input.h"
#include <GL/nanogl.h>
#include "gl_vidnt.h"

#define VID_NOMODE -2.0f
#define VID_AUTOMODE	"-1"
#define VID_DEFAULTMODE	2.0f
#define DISP_CHANGE_BADDUALVIEW	-6 // MSVC 6.0 doesn't
#define WINDOW_NAME			"Xash Window" // Half-Life

byte		*r_temppool;

static char* opengl_110funcs[] =
{
 "glClearColor"         ,
 "glClear"              ,
 "glAlphaFunc"          ,
 "glBlendFunc"          ,
 "glCullFace"           ,
 "glDrawBuffer"         ,
 "glReadBuffer"         ,
 "glEnable"             ,
 "glDisable"            ,
 "glEnableClientState"  ,
 "glDisableClientState" ,
 "glGetBooleanv"        ,
 "glGetDoublev"         ,
 "glGetFloatv"          ,
 "glGetIntegerv"        ,
 "glGetError"           ,
 "glGetString"          ,
 "glFinish"             ,
 "glFlush"              ,
 "glClearDepth"         ,
 "glDepthFunc"          ,
 "glDepthMask"          ,
 "glDepthRange"         ,
 "glFrontFace"          ,
 "glDrawElements"       ,
 "glColorMask"          ,
 "glIndexPointer"       ,
 "glVertexPointer"      ,
 "glNormalPointer"      ,
 "glColorPointer"       ,
 "glTexCoordPointer"    ,
 "glArrayElement"       ,
 "glColor3f"            ,
 "glColor3fv"           ,
 "glColor4f"            ,
 "glColor4fv"           ,
 "glColor3ub"           ,
 "glColor4ub"           ,
 "glColor4ubv"          ,
 "glTexCoord1f"         ,
 "glTexCoord2f"         ,
 "glTexCoord3f"         ,
 "glTexCoord4f"         ,
 "glTexGenf"            ,
 "glTexGenfv"           ,
 "glTexGeni"            ,
 "glVertex2f"           ,
 "glVertex3f"           ,
 "glVertex3fv"          ,
 "glNormal3f"           ,
 "glNormal3fv"          ,
 "glBegin"              ,
 "glEnd"                ,
 "glLineWidth"          ,
 "glPointSize"          ,
 "glMatrixMode"         ,
 "glOrtho"              ,
 "glRasterPos2f"        ,
 "glFrustum"            ,
 "glViewport"           ,
 "glPushMatrix"         ,
 "glPopMatrix"          ,
 "glPushAttrib"         ,
 "glPopAttrib"          ,
 "glLoadIdentity"       ,
 "glLoadMatrixd"        ,
 "glLoadMatrixf"        ,
 "glMultMatrixd"        ,
 "glMultMatrixf"        ,
 "glRotated"            ,
 "glRotatef"            ,
 "glScaled"             ,
 "glScalef"             ,
 "glTranslated"         ,
 "glTranslatef"         ,
 "glReadPixels"         ,
 "glDrawPixels"         ,
 "glStencilFunc"        ,
 "glStencilMask"        ,
 "glStencilOp"          ,
 "glClearStencil"       ,
 "glIsEnabled"          ,
 "glIsList"             ,
 "glIsTexture"          ,
 "glTexEnvf"            ,
 "glTexEnvfv"           ,
 "glTexEnvi"            ,
 "glTexParameterf"      ,
 "glTexParameterfv"     ,
 "glTexParameteri"      ,
 "glHint"               ,
 "glPixelStoref"        ,
 "glPixelStorei"        ,
 "glGenTextures"        ,
 "glDeleteTextures"     ,
 "glBindTexture"        ,
 "glTexImage1D"         ,
 "glTexImage2D"         ,
 "glTexSubImage1D"      ,
 "glTexSubImage2D"      ,
 "glCopyTexImage1D"     ,
 "glCopyTexImage2D"     ,
 "glCopyTexSubImage1D"  ,
 "glCopyTexSubImage2D"  ,
 "glScissor"            ,
 "glGetTexEnviv"        ,
 "glPolygonOffset"      ,
 "glPolygonMode"        ,
 "glPolygonStipple"     ,
 "glClipPlane"          ,
 "glGetClipPlane"       ,
 "glShadeModel"         ,
 "glFogfv"              ,
 "glFogf"               ,
 "glFogi"               ,
 NULL
};

static char* pointparametersfunc[] =
{
"glPointParameterfEXT"  ,
"glPointParameterfvEXT" ,
NULL
};

static char* drawrangeelementsfuncs[] =
{
 "glDrawRangeElements" ,
 NULL
};

static char* drawrangeelementsextfuncs[] =
{
 "glDrawRangeElementsEXT" ,
 NULL
};

static char* sgis_multitexturefuncs[] =
{
 "glSelectTextureSGIS" ,
 "glMTexCoord2fSGIS"   ,
 NULL
};

static char* multitexturefuncs[] =
{
 "glMultiTexCoord1fARB"     ,
 "glMultiTexCoord2fARB"     ,
 "glMultiTexCoord3fARB"     ,
 "glMultiTexCoord4fARB"     ,
 "glActiveTextureARB"       ,
 "glClientActiveTextureARB" ,
 "glClientActiveTextureARB" ,
 NULL
};

static char* compiledvertexarrayfuncs[] =
{
 "glLockArraysEXT"   ,
 "glUnlockArraysEXT" ,
 "glDrawArrays"      ,
 NULL
};

static char* texture3dextfuncs[] =
{
 "glTexImage3DEXT"        ,
 "glTexSubImage3DEXT"     ,
 "glCopyTexSubImage3DEXT" ,
 NULL
};

static char* atiseparatestencilfuncs[] =
{
 "glStencilOpSeparateATI"   ,
 "glStencilFuncSeparateATI" ,
 NULL
};

static char* gl2separatestencilfuncs[] =
{
 "glStencilOpSeparate"   ,
 "glStencilFuncSeparate" ,
 NULL
};

static char* stenciltwosidefuncs[] =
{
 "glActiveStencilFaceEXT" ,
 NULL
};

static char* blendequationfuncs[] =
{
 "glBlendEquationEXT" ,
 NULL
};

static char* shaderobjectsfuncs[] =
{
 "glDeleteObjectARB"             ,
 "glGetHandleARB"                ,
 "glDetachObjectARB"             ,
 "glCreateShaderObjectARB"       ,
 "glShaderSourceARB"             ,
 "glCompileShaderARB"            ,
 "glCreateProgramObjectARB"      ,
 "glAttachObjectARB"             ,
 "glLinkProgramARB"              ,
 "glUseProgramObjectARB"         ,
 "glValidateProgramARB"          ,
 "glUniform1fARB"                ,
 "glUniform2fARB"                ,
 "glUniform3fARB"                ,
 "glUniform4fARB"                ,
 "glUniform1iARB"                ,
 "glUniform2iARB"                ,
 "glUniform3iARB"                ,
 "glUniform4iARB"                ,
 "glUniform1fvARB"               ,
 "glUniform2fvARB"               ,
 "glUniform3fvARB"               ,
 "glUniform4fvARB"               ,
 "glUniform1ivARB"               ,
 "glUniform2ivARB"               ,
 "glUniform3ivARB"               ,
 "glUniform4ivARB"               ,
 "glUniformMatrix2fvARB"         ,
 "glUniformMatrix3fvARB"         ,
 "glUniformMatrix4fvARB"         ,
 "glGetObjectParameterfvARB"     ,
 "glGetObjectParameterivARB"     ,
 "glGetInfoLogARB"               ,
 "glGetAttachedObjectsARB"       ,
 "glGetUniformLocationARB"       ,
 "glGetActiveUniformARB"         ,
 "glGetUniformfvARB"             ,
 "glGetUniformivARB"             ,
 "glGetShaderSourceARB"          ,
 "glVertexAttribPointerARB"      ,
 "glEnableVertexAttribArrayARB"  ,
 "glDisableVertexAttribArrayARB" ,
 "glBindAttribLocationARB"       ,
 "glGetActiveAttribARB"          ,
 "glGetAttribLocationARB"        ,
 NULL
};

static char* vertexshaderfuncs[] =
{
 "glVertexAttribPointerARB"      ,
 "glEnableVertexAttribArrayARB"  ,
 "glDisableVertexAttribArrayARB" ,
 "glBindAttribLocationARB"       ,
 "glGetActiveAttribARB"          ,
 "glGetAttribLocationARB"        ,
 NULL
};

static char* vbofuncs[] =
{
 "glBindBufferARB"    ,
 "glDeleteBuffersARB" ,
 "glGenBuffersARB"    ,
 "glIsBufferARB"      ,
 "glMapBufferARB"     ,
 "glUnmapBufferARB"   ,
 "glBufferDataARB"    ,
 "glBufferSubDataARB" ,
 NULL
};

static char* occlusionfunc[] =
{
 "glGenQueriesARB"        ,
 "glDeleteQueriesARB"     ,
 "glIsQueryARB"           ,
 "glBeginQueryARB"        ,
 "glEndQueryARB"          ,
 "glGetQueryivARB"        ,
 "glGetQueryObjectivARB"  ,
 "glGetQueryObjectuivARB" ,
 NULL
};

static char* texturecompressionfuncs[] =
{
 "glCompressedTexImage3DARB"    ,
 "glCompressedTexImage2DARB"    ,
 "glCompressedTexImage1DARB"    ,
 "glCompressedTexSubImage3DARB" ,
 "glCompressedTexSubImage2DARB" ,
 "glCompressedTexSubImage1DARB" ,
 "glGetCompressedTexImageARB"   ,
 NULL
};


/*
=================
GL_GetProcAddress
=================
*/
void *GL_GetProcAddress( const char *name )
{
#ifdef XASH_SDL
	void *func = SDL_GL_GetProcAddress(name);
#elif defined (XASH_GLES)
	void *func = nanoGL_GetProcAddress(name);
#else //No opengl implementation
	void *func = NULL;
#endif
	if(!func)
	{
		MsgDev(D_ERROR, "Error: GL_GetProcAddress failed for %s", name);
	}
	return func;
}

/*
=================
GL_CheckExtension
=================
*/
void GL_CheckExtension( const char *name, const dllfunc_t *funcs, const char *cvarname, int r_ext )
{
	const dllfunc_t	*func;
	convar_t		*parm;

	MsgDev( D_NOTE, "GL_CheckExtension: %s ", name );

	if( cvarname )
	{
		// system config disable extensions
		parm = Cvar_Get( cvarname, "1", CVAR_GLCONFIG, va( "enable or disable %s", name ));

		if( parm->integer == 0 || ( gl_extensions->integer == 0 && r_ext != GL_OPENGL_110 ))
		{
			MsgDev( D_NOTE, "- disabled\n" );
			GL_SetExtension( r_ext, 0 );
			return; // nothing to process at
		}
		GL_SetExtension( r_ext, 1 );
	}

	if(( name[2] == '_' || name[3] == '_' ) && !Q_strstr( glConfig.extensions_string, name ))
	{
		GL_SetExtension( r_ext, false );	// update render info
		MsgDev( D_NOTE, "- ^1failed\n" );
		return;
	}


	GL_SetExtension( r_ext, true ); // predict extension state

	if( GL_Support( r_ext ))
		MsgDev( D_NOTE, "- ^2enabled\n" );
	else MsgDev( D_NOTE, "- ^1failed\n" );
}
/*
===============
GL_UpdateGammaRamp
===============
*/
void GL_UpdateGammaRamp( void )
{
}

/*
===============
GL_UpdateSwapInterval
===============
*/
void GL_UpdateSwapInterval( void )
{
	if( gl_swapInterval->modified )
	{
		gl_swapInterval->modified = false;
#ifdef XASH_SDL
		if( SDL_GL_SetSwapInterval(gl_swapInterval->integer) )
			MsgDev(D_ERROR, "SDL_GL_SetSwapInterval: %s\n", SDL_GetError());
#endif
	}
}

/*
===============
GL_ContextError
===============
*/
static void GL_ContextError( void )
{
#ifdef XASH_SDL
	MsgDev( D_ERROR, "GL_ContextError: %s\n", SDL_GetError() );
#endif
}

/*
==================
GL_SetupAttributes
==================
*/
void GL_SetupAttributes()
{
#ifdef XASH_SDL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

#endif // XASH_SDL
}

/*
=================
GL_CreateContext
=================
*/
qboolean GL_CreateContext( void )
{
	nanoGL_Init();
	/*if( !Sys_CheckParm( "-gldebug" ) || host.developer < 1 ) // debug bit the kills perfomance
		return true;*/
#ifdef XASH_SDL
	if( ( glw_state.context = SDL_GL_CreateContext( host.hWnd ) ) == NULL)
	{
		MsgDev(D_ERROR, "GL_CreateContext: %s\n", SDL_GetError());
		return GL_DeleteContext();
	}
#endif
	return true;
}

/*
=================
GL_UpdateContext
=================
*/
qboolean GL_UpdateContext( void )
{
#ifdef XASH_SDL
	if(!( SDL_GL_MakeCurrent( host.hWnd, glw_state.context ) ) )
	{
		MsgDev(D_ERROR, "GL_UpdateContext: %s", SDL_GetError());
		return GL_DeleteContext();
	}
#endif
	return true;
}

/*
=================
GL_DeleteContext
=================
*/
qboolean GL_DeleteContext( void )
{
#ifdef XASH_SDL
	SDL_GL_DeleteContext(glw_state.context);
#endif
	glw_state.context = NULL;

	return false;
}

uint VID_EnumerateInstances( void )
{
	return 1;
}

void VID_StartupGamma( void )
{
	// Device supports gamma anyway, but cannot do anything with it.
	fs_offset_t	gamma_size;
	byte	*savedGamma;
	size_t	gammaTypeSize = sizeof(glState.stateRamp);

	// init gamma ramp
	Q_memset( glState.stateRamp, 0, gammaTypeSize);

	// force to set cvar
	Cvar_FullSet( "gl_ignorehwgamma", "1", CVAR_GLCONFIG );

	glConfig.deviceSupportsGamma = false;	// even if supported!
	BuildGammaTable( vid_gamma->value, vid_texgamma->value );
	MsgDev( D_NOTE, "VID_StartupGamma: software gamma initialized\n" );
}

void VID_RestoreGamma( void )
{
	// no hardware gamma
}

qboolean VID_CreateWindow( int width, int height, qboolean fullscreen )
{
#ifdef XASH_SDL
	static string	wndname;
	Uint32 wndFlags = SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

	Q_strncpy( wndname, GI->title, sizeof( wndname ));

	host.hWnd = SDL_CreateWindow(wndname, r_xpos->integer,
		r_ypos->integer, width, height, wndFlags);

	if( !host.hWnd )
	{
		MsgDev( D_ERROR, "VID_CreateWindow: couldn't create '%s': %s\n", wndname, SDL_GetError());
		return false;
	}

	if( fullscreen )
	{
		SDL_DisplayMode want, got;

		want.w = width;
		want.h = height;
		want.driverdata = NULL;
		want.format = want.refresh_rate = 0; // don't care

		if( !SDL_GetClosestDisplayMode(0, &want, &got) )
			return false;

		MsgDev(D_NOTE, "Got closest display mode: %ix%i@%i\n", got.w, got.h, got.refresh_rate);

		if( SDL_SetWindowDisplayMode(host.hWnd, &got) == -1 )
			return false;

		if( SDL_SetWindowFullscreen(host.hWnd, SDL_WINDOW_FULLSCREEN) == -1 )
			return false;

	}

	host.window_center_x = width / 2;
	host.window_center_y = height / 2;

#if defined(_WIN32)
	{
		HICON ico;
		SDL_SysWMinfo info;

		if( FS_FileExists( GI->iconpath, true ) )
		{
			char	localPath[MAX_PATH];

			Q_snprintf( localPath, sizeof( localPath ), "%s/%s", GI->gamedir, GI->iconpath );
			ico = LoadImage( NULL, localPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE );

			if( !ico )
			{
				MsgDev( D_INFO, "Extract %s from pak if you want to see it.\n", GI->iconpath );
				ico = LoadIcon( host.hInst, MAKEINTRESOURCE( 101 ) );
			}
		}
		else ico = LoadIcon( host.hInst, MAKEINTRESOURCE( 101 ) );

		if( SDL_GetWindowWMInfo( host.hWnd, &info ) )
		{
			// info.info.info.info.info... Holy shit, SDL?
			SetClassLong( info.info.win.window, GCL_HICON, ico );
		}
	}
#endif

	SDL_ShowWindow( host.hWnd );
#else
	host.hWnd = 1; //fake window
	host.window_center_x = width / 2;
	host.window_center_y = height / 2;
#endif
	if( !glw_state.initialized )
	{
		if( !GL_CreateContext( ))
			return false;

		VID_StartupGamma();
	}
	else
	{
		if( !GL_UpdateContext( ))
			return false;
	}
	return true;
}

void VID_DestroyWindow( void )
{
#ifdef XASH_SDL
	if( glw_state.context )
	{
		SDL_GL_DeleteContext( glw_state.context );
		glw_state.context = NULL;
	}

	if( host.hWnd )
	{
		SDL_DestroyWindow ( host.hWnd );
		host.hWnd = NULL;
	}
#endif
	if( glState.fullScreen )
	{
		glState.fullScreen = false;
	}
}

/*
==================
R_ChangeDisplaySettingsFast

Change window size fastly to custom values, without setting vid mode
==================
*/
void R_ChangeDisplaySettingsFast( int width, int height )
{
	//Cvar_SetFloat("vid_mode", VID_NOMODE);
	Cvar_SetFloat("width", width);
	Cvar_SetFloat("height", height);

	glState.width = width;
	glState.height = height;

	SCR_VidInit();
}


rserr_t R_ChangeDisplaySettings( int width, int height, qboolean fullscreen )
{
#ifdef XASH_SDL
	SDL_DisplayMode displayMode;

	SDL_GetCurrentDisplayMode(0, &displayMode);

	width = displayMode.w;
	height = displayMode.h;
	fullscreen = false;

	R_SaveVideoMode( width, height );

	// check our desktop attributes
	glw_state.desktopBitsPixel = SDL_BITSPERPIXEL(displayMode.format);
	glw_state.desktopWidth = displayMode.w;
	glw_state.desktopHeight = displayMode.h;

	glState.fullScreen = fullscreen;

	if(!host.hWnd)
	{
		if( !VID_CreateWindow( width, height, fullscreen ) )
			return rserr_invalid_mode;
	}
#endif // XASH_SDL
	return rserr_ok;
}



/*
==================
VID_SetMode

Set the described video mode
==================
*/
qboolean VID_SetMode( void )
{
#ifdef XASH_SDL
	qboolean	fullscreen = false;
	int iScreenWidth, iScreenHeight;
	rserr_t	err;

	if( vid_mode->integer == -1 )	// trying to get resolution automatically by default
	{
		SDL_DisplayMode mode;

		SDL_GetDesktopDisplayMode(0, &mode);

		iScreenWidth = mode.w;
		iScreenHeight = mode.h;

		Cvar_SetFloat( "fullscreen", 1 );
	}
	else if( vid_mode->modified && vid_mode->integer >= 0 && vid_mode->integer <= num_vidmodes )
	{
		iScreenWidth = vidmode[vid_mode->integer].width;
		iScreenHeight = vidmode[vid_mode->integer].height;
	}
	else
	{
		iScreenHeight = scr_height->integer;
		iScreenWidth = scr_width->integer;
	}

	gl_swapInterval->modified = true;
	fullscreen = Cvar_VariableInteger("fullscreen") != 0;

	if(( err = R_ChangeDisplaySettings( iScreenWidth, iScreenHeight, fullscreen )) == rserr_ok )
	{
		glConfig.prev_width = iScreenWidth;
		glConfig.prev_height = iScreenHeight;
	}
	else
	{
		if( err == rserr_invalid_fullscreen )
		{
			Cvar_SetFloat( "fullscreen", 0 );
			MsgDev( D_ERROR, "VID_SetMode: fullscreen unavailable in this mode\n" );
			if(( err = R_ChangeDisplaySettings( iScreenWidth, iScreenHeight, false )) == rserr_ok )
				return true;
		}
		else if( err == rserr_invalid_mode )
		{
			Cvar_SetFloat( "vid_mode", glConfig.prev_mode );
			MsgDev( D_ERROR, "VID_SetMode: invalid mode\n" );
		}

		// try setting it back to something safe
		if(( err = R_ChangeDisplaySettings( glConfig.prev_width, glConfig.prev_height, false )) != rserr_ok )
		{
			MsgDev( D_ERROR, "VID_SetMode: could not revert to safe mode\n" );
			return false;
		}
	}
#endif
	return true;
}



/*
==================
R_Init_OpenGL
==================
*/
qboolean R_Init_OpenGL( void )
{
	GL_SetupAttributes();
#ifdef XASH_SDL
	if( SDL_GL_LoadLibrary( NULL ) )
	{
		MsgDev( D_ERROR, "Couldn't initialize OpenGL: %s\n", SDL_GetError());
		return false;
	}
#endif
	return VID_SetMode();
}


/*
==================
R_Free_OpenGL
==================
*/
void R_Free_OpenGL( void )
{
	VID_RestoreGamma ();

	GL_DeleteContext ();

	VID_DestroyWindow ();
#ifdef XASH_SDL
	SDL_GL_UnloadLibrary ();
#endif
	// now all extensions are disabled
	Q_memset( glConfig.extension, 0, sizeof( glConfig.extension[0] ) * GL_EXTCOUNT );
	glw_state.initialized = false;
}
#endif
