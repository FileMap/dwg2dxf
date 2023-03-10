/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2dxf.c: save a DWG as DXF.
 * optionally as a different version.
 *
 * written by Reini Urban
 */

#include <napi.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

extern "C" {
  #include <dwg.h>
  #include "common.h"
  #include "bits.h"
  #include "logging.h"
  #include "suffix.inc"
  #include "out_dxf.h"
}

static int opts = 1;
int minimal = 0;
int binary = 0;
int overwrite = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;
bool dwg2dxfConvert(std::string inName, std::string outName) {
    int error = 0;
    Dwg_Data dwg;
    char *filename_in;
    const char *version = NULL;
    char *filename_out = NULL;
    Dwg_Version_Type dwg_version = R_2000;
    Bit_Chain dat = { 0 };
    int do_free = 0;
    int need_free = 0;
    overwrite = 1;
    filename_out = strcpy(new char[outName.length() + 1], outName.c_str());
    #if defined(USE_TRACING) && defined(HAVE_SETENV)
          {
            char v[2];
            *v = opts + '0';
            *(v + 1) = 0;
            setenv ("LIBREDWG_TRACE", v, 1);
          }
    #endif
    filename_in = strcpy(new char[inName.length() + 1], inName.c_str());
    fclose(stdout);
    fclose(stderr);
    if (!filename_out)
        {
          need_free = 1;
          filename_out = suffix (filename_in, "dxf");
        }
    if (strEQ (filename_in, filename_out))
        {
          if (need_free)
            free (filename_out);
          return false;
        }

    memset (&dwg, 0, sizeof (Dwg_Data));
    dwg.opts = opts;
    printf ("Reading DWG file %s\n", filename_in);
    error = dwg_read_file (filename_in, &dwg);
    if (error >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "READ ERROR 0x%x\n", error);
          goto final;
        }

    printf ("Writing DXF file %s", filename_out);
    if (version)
        {
          printf (" as %s\n", version);
          if (dwg.header.from_version != dwg.header.version)
            dwg.header.from_version = dwg.header.version;
          // else keep from_version = 0
          dwg.header.version = dwg_version;
        }
    else
        {
          printf ("\n");
        }
    dat.version = dwg.header.version;
    dat.from_version = dwg.header.from_version;

    if (minimal) dwg.opts |= DWG_OPTS_MINIMAL;
    dat.fh = fopen (filename_out, "wb");
    if (!dat.fh)
        {
          fprintf (stderr, "WRITE ERROR %s\n", filename_out);
          error = DWG_ERR_IOERROR;
        }
    else
        {
          error = binary ? dwg_write_dxfb (&dat, &dwg)
                         : dwg_write_dxf (&dat, &dwg);
        }

    if (error >= DWG_ERR_CRITICAL)
        fprintf (stderr, "WRITE ERROR %s\n", filename_out);

    if (dat.fh)
        fclose (dat.fh);

    final:
    #if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
      {
        char *asanenv = getenv ("ASAN_OPTIONS");
        if (!asanenv)
          do_free = 1;
        // detect_leaks is enabled by default. see if it's turned off
        else if (strstr (asanenv, "detect_leaks=0") == NULL) /* not found */
          do_free = 1;
      }
    #endif
      // forget about leaks. really huge DWG's need endlessly here.
      if (do_free
    #ifdef HAVE_VALGRIND_VALGRIND_H
          || (RUNNING_ON_VALGRIND)
    #endif
      )
        {
          dwg_free (&dwg);
          if (need_free)
            free (filename_out);
        }
    filename_out = NULL;

  // but only the result of the last conversion
    return error >= DWG_ERR_CRITICAL ? 1 : 0;
}


Napi::Boolean dwg2dxf(const Napi::CallbackInfo &info) {
    auto env = info.Env();

    if (info.Length() < 2) {
        return Napi::Boolean::New(env, false);;
    }

    dwg2dxfConvert(
        (std::string) info[0].ToString(),
        (std::string) info[1].ToString()
    );

    return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("dwg2dxf", Napi::Function::New(env, dwg2dxf));
    return exports;
}

NODE_API_MODULE(dwg2dxf, Init)
