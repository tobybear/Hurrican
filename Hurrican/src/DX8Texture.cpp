//DKS - DX8Texture.cpp is a new file I have added.
//      It contains a new texture system class that allows sprites to share textures,
//      where before the game was loading duplicate copies in places. It also
//      abstracts away the details like the alpha textures needed by ETC1 compression.
//      It also unifies the interface that was once different between DX8 and SDL.

#if defined(PLATFORM_SDL)
#include "SDLPort/SDL_port.h"
#endif

#include "DX8Texture.hpp"

#if defined(PLATFORM_SDL)
#include "SDLPort/texture.h"
#endif

#include "Main.hpp"
#include "Gameplay.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#ifdef USE_UNRARLIB
#include "unrarlib.h"
#endif

const std::string TexturesystemClass::scalefactors_filename("scalefactors.txt");

// --------------------------------------------------------------------------------------
// TexturesystemClass functions
// --------------------------------------------------------------------------------------

void TexturesystemClass::Exit()
{
    // If there are any remaining textures in memory that have not already been
    // unloaded through DirectGraphicsSprite destructors, mop them up now:
    //   A good example would be textures that are member sprites of
    // global static objects, whose order of destruction can't be guaranteed,
    // such as PlayerClass.

    for (unsigned int i=0; i < _loaded_textures.size(); ++i) {
        // Force unloading of the texture if it's still loaded, by making its
        //  instances equal 1 and calling UnloadTexture():
        if (_loaded_textures[i].instances > 0) {
            _loaded_textures[i].instances = 1;
            UnloadTexture(i);
        }
    }
}


//DKS - This largely replaces and expands on a lot of the code
//      DirectGraphicsSprite::LoadImage used to handle.
int16_t TexturesystemClass::LoadTexture( const std::string &filename )
{
    if (filename.empty())
        return -1;

    int16_t idx = -1;

    // Check to see if the texture was previously loaded at some point
    std::map<std::string, int16_t>::iterator it = _texture_map.find(filename);
    if (it != _texture_map.end()) {
        // Texture has been loaded previously, so it at least has a slot in _loaded_textures,
        //  but if its instances == 0, it will need to be re-loaded from disk.
        idx = (*it).second;
#ifdef _DEBUG
        if (idx < 0 || static_cast<int>(i)dx >= (int)_loaded_textures.size()) {
            Protokoll << "-> Error: texture handle idx " << idx << " acquired from _texture_map is outside\n"
                    "\t_loaded_textures array bounds. Lower bound: 0  Upper bound: " << _loaded_textures.size()-1 << "\n"
                    "\tfilename: " << filename << std::endl;
            return -1;
        }
#endif
    } 
    
    if (idx == -1) {
        // There were no previous loadings of the texture, so create a new slot for it
        TextureHandle th;
        _loaded_textures.push_back(th);
        idx = _loaded_textures.size()-1;
        // Create entry in _texture_map mapping texture's filename to the new _loaded_textures index
        _texture_map[filename] = idx;
    }
        
    TextureHandle &th = _loaded_textures[idx];

    if (th.instances > 0) {
        // This texture is already loaded in VRAM
        ++th.instances;
#ifdef _DEBUG
        Protokoll << "-> Prevented loading of duplicate texture: " << filename << ", total references: " <<
                th.instances << std::endl;
#endif
    } else {
        // This texture is not in VRAM, load it..
        if ( !LoadTextureFromFile(filename, th)) {
            Protokoll << "-> Error loading texture from disk: " << filename << std::endl;
            GameRunning = false;
            th.instances = 0;
        } else {
            th.instances = 1;
        }
    }

    // If we have loaded npot scale factors from an external file, see if we have some for this texture:
    if (!_scalefactors_map.empty()) {
        std::string filename_sans_ext(filename);
        ReplaceAll( filename_sans_ext, ".png", "" );
        std::map< std::string, std::pair<double,double> >::iterator it = _scalefactors_map.find(filename_sans_ext);
        if ( it != _scalefactors_map.end() ) {
            th.npot_scalex = (*it).second.first;
            th.npot_scaley = (*it).second.second;
#ifdef _DEBUG
            Protokoll << "Using external npot scalefactors " << th.npot_scalex << " " << th.npot_scaley << " for texture " << filename << std::endl;
#endif
        }
    }

    return idx;
}

void TexturesystemClass::UnloadTexture(const int idx)
{
    if (idx >= 0 && idx < static_cast<int>(_loaded_textures.size())) {
        TextureHandle &th = _loaded_textures[idx];
        if (th.instances > 0) {
            --th.instances;
            if (th.instances == 0) {
#if defined(PLATFORM_DIRECTX)
                DX8_UnloadTexture(th);
#else
                SDL_UnloadTexture(th);
#endif
#ifdef _DEBUG
		        Protokoll << "-> Texture successfully released !" << std::endl;
#endif
            }
        }
    }
}

void TexturesystemClass::ReadScaleFactorsFile( const std::string &fullpath )
{
    std::ifstream file(fullpath.c_str(), std::ios::in);
    if (!file.is_open())
        return;

    Protokoll << "Reading texture NPOT scale factors from " << fullpath << std::endl;

    std::string name;
    double xscale = 0, yscale = 0;
    while (file >> name >> xscale >> yscale) {
        if (!name.empty() && xscale != 0.0 && yscale != 0.0) {
            _scalefactors_map[name] = std::make_pair(xscale, yscale);
#ifdef _DEBUG
            Protokoll << "Read name= " << name << " xscale=" << xscale << " yscale=" << yscale << std::endl;
#endif
        }
    }

    file.close();
}

void TexturesystemClass::ReadScaleFactorsFiles()
{
    std::string path, fullpath;

    if (CommandLineParams.RunOwnLevelList) {
        path = std::string(g_storage_ext) + "/levels/" +
               std::string(CommandLineParams.OwnLevelList) + "/data/textures/";
    } else {
        path = std::string(g_storage_ext) + "/data/textures/";
    }

    // First, see if there is a file in data/textures/ where plain old .PNG files are and load its data:
    fullpath = path + scalefactors_filename;
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);

    // Then, handle any files in the compressed-textures subfolders, their data will also be loaded,
    // and any data they contain will override what's already loaded, on a file-by-file basis.
#if defined(USE_ETC1)
    fullpath = path + "etc1/" + scalefactors_filename; 
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);
#endif

#if defined(USE_PVRTC)
    fullpath = path + "pvr/" + scalefactors_filename; 
    if (FileExists(fullpath.c_str()))
        ReadScaleFactorsFile(fullpath);
#endif
}

bool TexturesystemClass::LoadTextureFromFile( const std::string &filename, TextureHandle &th )
{
    if (filename.empty()) {
        Protokoll << "Error: empty filename passed to LoadTextureFromFile()" << std::endl;
        return false;
    }

    std::string path = std::string(g_storage_ext);

    //DKS - All textures are now stored in their own data/textures/ subdir:
    // Are we using a custom level set?
    if (CommandLineParams.RunOwnLevelList) {
        path += "/levels/" + std::string(CommandLineParams.OwnLevelList);
    }

    path += "/data/textures";

    bool success = false;

#if defined(USE_UNRARLIB)
    // Are we using unrarlib to read all game data from a single RAR archive?
    void  *buf_data   = NULL;   // Memory  buffer file is read into, if using unrarlib
    unsigned long buf_size = 0;  // Size of memory buffer file is read into, if using unrarlib
    if ( FileExists(RARFILENAME) &&
            urarlib_get(&buf_data, &buf_size, filename.c_str(), RARFILENAME, convertText(RARFILEPASSWORD)) &&
            buf_data != NULL )
    {
        // Load the texture from the image that is now in buf_data[]
#if defined(PLATFORM_DIRECTX)
        success = DX8_LoadTexture( NULL, NULL, buf_data, buf_size, th );
#elif defined(PLATFORM_SDL)
        success = SDL_LoadTexture( NULL, NULL, buf_data, buf_size, th );
#endif
        if (buf_data)
            free(buf_data);

        if (success) {
            goto loaded;
        } else {
            Protokoll << "Error loading texture " << filename << " from archive " << RARFILENAME << std::endl;
            Protokoll << "->Trying elsewhere.." << std::endl;
        }
    }
#endif // USE_UNRARLIB

    // Load the texture from disk:
#if defined(PLATFORM_DIRECTX)
    success = DX8_LoadTexture( path, filename, NULL, 0, th );
#elif defined(PLATFORM_SDL)
    success = SDL_LoadTexture( path, filename, NULL, 0, th );
#endif
    if (success)
        goto loaded;

loaded:
    if (!success) {
        Protokoll << "Error loading texture " << filename << std::endl;
        GameRunning = false;
    } else {
        std::string tmpstr( std::string(TextArray[TEXT_LADE_BITMAP]) + ' ' + filename +
                ' ' + std::string(TextArray[TEXT_LADEN_ERFOLGREICH]) + '\n' );
        DisplayLoadInfo(tmpstr.c_str());
    }
        
    return success;
}


//DKS - This is an UNTESTED effort at incorporating DirectX support back into the new texture system.
//      It should be more flexible than the original game's DirectX texture support, in that
//      it allows for loading textures from disk that are arbitrarily resized.
#if defined(PLATFORM_DIRECTX)
bool TexturesystemClass::DX8_LoadTexture( const std::string &path, const std::string &filename, 
                                          void *buf, unsigned int buf_size, TextureHandle &th )
{
    HRESULT  hresult;
    bool     load_from_memory = buf_size > 0;
    std::string   fullpath = path + "/" + filename;


    if (load_from_memory && !buf) {
        Protokoll << "Error: null ptr passed to DX8_LoadTexture() reading file from memory" << std::endl;
        GameRunning = false;
        return false;
    } else if (filename.empty()) {
        Protokoll << "Error: empty filename passed to DX8_LoadTexture()" << std::endl;
        GameRunning = false;
        return false;
    }

    if (load_from_memory) {
        // Load texture from memory buffer
        hresult = D3DXCreateTextureFromFileInMemoryEx(
                      lpD3DDevice,
                      (LPVOID)buf,
                      buf_size,
                      NULL, NULL,				  // x und y GrÃ¶sse des Sprites (aus Datei Ã¼bernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &th.tex);
    } else {
        if (!FileExists(fullpath.c_str()))
            return false;

        hresult = D3DXCreateTextureFromFileEx(
                      lpD3DDevice,
                      fullpath.c_str(),
                      NULL, NULL,				  // x und y GrÃ¶sse des Sprites (aus Datei Ã¼bernehmen)
                      1,                          // Nur eine Version der Textur
                      0,                          // Immer 0 setzen
                      D3DFMT_UNKNOWN,			  // Format aus der Datei lesen
                      D3DPOOL_MANAGED,            // DX bestimmt wo die Textur gespeichert wird
                      D3DX_FILTER_NONE,			  // Keine Filter verwenden
                      D3DX_FILTER_NONE,
                      0xFFFF00FF,                 // Colorkeyfarbe (Lila)
                      NULL,						  // Keine Image Info
                      NULL,						  // Keine Palette angeben
                      &th.tex);
    }

    if (hresult != D3D_OK) {
        if (load_from_memory)
        {
            Protokoll << "Error in DirectX loading texture" << std::endl;
            GameRunning = false;
        }
        else
        {
            Protokoll << "Error in DirectX loading texture: " << fullpath << std::endl;
            GameRunning = false;
        }

        return false;
    } else {
        // Get the dimensions of texture in VRAM:
        D3DSURFACE_DESC tex_info;
        tex_info.Width = tex_info.Height = 0;
        th.tex->GetLevelDesc(0,&tex_info);

        // Get the dimensions of the file directly:
        D3DXIMAGE_INFO img_info;
        img_info.Width = img_info.Height = 0;

        if (load_from_memory)
            hresult = D3DXGetimgInfoFromFileInMemory( buf, buf_size, &img_info );
        else
            hresult = D3DXGetimgInfoFromFile( fullpath.c_str(), &img_info );

        if (hresult != D3D_OK ||
                tex_info.Width == 0 || tex_info.Height == 0 ||
                img_info.Width == 0 || img_info.Height == 0) {
            Protokoll << "Error in DirectX reading image dimensions" << std::endl;
            GameRunning = false;
        } else {
            th.npot_scalex = (double)img_info.Width  / (double)tex_info.Width;
            th.npot_scaley = (double)img_info.Height / (double)tex_info.Height;
        }
    }
    
    th.instances = 1;
    return true;
}

void TexturesystemClass::DX8_UnloadTexture( TextureHandle &th )
{
    SafeRelease(th.tex);
    th.instances = 0;
}
#endif //PLATFORM_DIRECTX
