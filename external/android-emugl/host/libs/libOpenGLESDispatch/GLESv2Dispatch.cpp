/*
* Copyright (C) 2011 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "OpenGLESDispatch/GLESv2Dispatch.h"
#include "OpenGLESDispatch/EGLDispatch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emugl/common/shared_library.h"

extern EGLDispatch s_egl;

static emugl::SharedLibrary *s_gles2_lib = NULL;

namespace {
constexpr const char *glesv2_lib_env_var{"ANDROID_GLESv2_LIB"};
}

// An unimplemented function which prints out an error message.
// To make it consistent with the guest, all GLES2 functions not supported by
// the driver should be redirected to this function.

static void gles2_unimplemented() {
  fprintf(stderr, "Called unimplemented GLESv2 API\n");
}

// Holds the level of GLES 3.x support after gles2_dispatch_init runs.
static GLESDispatchMaxVersion s_max_supported_gles_version = GLES_DISPATCH_MAX_VERSION_2;

//
// This function is called only once during initialiation before
// any thread has been created - hence it should NOT be thread safe.
//
bool gles2_dispatch_init(const char *path, GLESv2Dispatch *dispatch_table) {
  const char *libName = getenv(glesv2_lib_env_var);
  if (!libName)
    libName = path;
  if (!libName)
    return false;

  char error[256];
  s_gles2_lib = emugl::SharedLibrary::open(libName, error, sizeof(error));
  if (!s_gles2_lib) {
    fprintf(stderr, "%s: Could not load %s [%s]\n", __FUNCTION__, libName,
            error);
    return false;
  }

//
// init the GLES dispatch table
//
#define LOOKUP_SYMBOL(return_type, function_name, signature, callargs)         \
  dispatch_table->function_name = reinterpret_cast<function_name##_t>(         \
      s_gles2_lib->findSymbol(#function_name));

#define LOOKUP_EXT_SYMBOL(return_type, function_name, signature, callargs)     \
  dispatch_table->function_name = reinterpret_cast<function_name##_t>(         \
      s_egl.eglGetProcAddress(#function_name));

  LIST_GLES2_FUNCTIONS(LOOKUP_SYMBOL, LOOKUP_EXT_SYMBOL)

  bool gles30_supported = true;
  bool gles31_supported = true;
  bool gles32_supported = false;
// For 3.0, we don't really need glInvalidate(Sub)Framebuffer.
#define DETECT_GLES30_SUPPORT(return_type, function_name, signature, callargs) do { \
    if (!dispatch_table->function_name && \
        strcmp(#function_name, "glInvalidateFramebuffer") && \
        strcmp(#function_name, "glInvalidateSubFramebuffer") ) { \
        gles30_supported = false; \
    } \
    } while(0); \

    LIST_GLES3_ONLY_FUNCTIONS(DETECT_GLES30_SUPPORT)

#define DETECT_GLES31_SUPPORT(return_type, function_name, signature, callargs) do { \
    if (!dispatch_table->function_name) { \
        gles31_supported = false; } \
    } while(0); \

    LIST_GLES31_ONLY_FUNCTIONS(DETECT_GLES31_SUPPORT)
    if (gles30_supported && gles31_supported && gles32_supported) {
        s_max_supported_gles_version = GLES_DISPATCH_MAX_VERSION_3_2;
    } else if (gles30_supported && gles31_supported) {
        s_max_supported_gles_version = GLES_DISPATCH_MAX_VERSION_3_1;
    } else if (gles30_supported) {
        s_max_supported_gles_version = GLES_DISPATCH_MAX_VERSION_3_0;
    } else {
        s_max_supported_gles_version = GLES_DISPATCH_MAX_VERSION_2;
    }

  dispatch_table->initialized = true;
  return true;
}

//
// This function is called only during initialization before
// any thread has been created - hence it should NOT be thread safe.
//
void *gles2_dispatch_get_proc_func(const char *name, void *userData) {
  void *func = NULL;

  if (s_gles2_lib && !func) {
    func = (void *)s_gles2_lib->findSymbol(name);
  }

  if (!func) {
    func = (void *)s_egl.eglGetProcAddress(name);
  }

  // To make it consistent with the guest, redirect any unsupported functions
  // to gles2_unimplemented.
  if (!func) {
    func = (void *)gles2_unimplemented;
  }
  return func;
}

GLESDispatchMaxVersion gles2_dispatch_get_max_version() {
    return s_max_supported_gles_version;
}
