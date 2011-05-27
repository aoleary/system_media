/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FILTERFW_CORE_GL_FRAME_H__
#define FILTERFW_CORE_GL_FRAME_H__

#include <GLES2/gl2.h>

#include "core/gl_buffer_interface.h"

namespace android {
namespace filterfw {

class ShaderProgram;

// A GLFrame stores pixel data on the GPU. While pixel data may be uploaded to
// a GLFrame and also read out of a GLFrame (access in place is not supported),
// it is strongly recommended to use ShaderProgram objects for any kind of
// processing from one GLFrame to another.
class GLFrame : public GLBufferHandle {
  public:
    // Create an empty GL frame.
    GLFrame();

    // Deallocate a GL frame.
    ~GLFrame();

    // Initialize a GL frame to the given width, height, format. Also specify
    // whether this is a read-only GL frame or not.
    bool Init(int width, int height);

    // Initialize as using an external texture.
    bool InitWithExternalTexture();

    // Initialize using an existing texture.
    bool InitWithTexture(GLint texture_id, int width, int height, bool owns, bool create_tex);

    // Initialize using an existing FBO.
    bool InitWithFbo(GLint fbo_id, int width, int height, bool owns, bool create_fbo);

    // Write the data with the given size in bytes to the frame. The frame size must match the
    // size of the data.
    bool WriteData(const uint8* data, int size);

    // Copies the frame data to the given buffer.
    bool CopyDataTo(uint8* buffer, int size);

    // Copies the pixels from another GL frame to this frame.
    bool CopyPixelsFrom(const GLFrame* frame);

    // Returns the size of the buffer in bytes.
    int Size() const;

    // Clone the current frame by creating a new GL frame and copying all data to it.
    GLFrame* Clone() const;

    // Returns the held texture id. Only call this if the GLFrame holds a
    // texture. You can check this by calling HoldsTexture().
    // Note, that a texture is created only when needed. If you are creating a
    // new GLFrame, and you need it to be bound to a texture, upload (zeroed)
    // data to it first, before calling this method.
    GLuint GetTextureId() const;

    // Returns the held FBO id. Only call this if the GLFrame holds an FBO. You
    // can check this by calling HoldsFbo().
    GLuint GetFboId() const;

    // Returns the texture target: GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES.
    GLuint GetTextureTarget() const {
      return texture_target_;
    }

    // Set the viewport that will be used when focusing this frame for rendering. Defaults to
    // the dimensions of the frame.
    bool SetViewport(int x, int y, int width, int height);

    // Binds the held texture. This may result in creating the texture if it
    // is not yet available.
    bool FocusTexture();

    // Binds the held FBO. This may result in creating the FBO if it
    // is not yet available.
    bool FocusFrameBuffer();

    // Generates the mipmap chain of the held texture. Returns true, iff
    // generating was successful.
    bool GenerateMipMap();

    // Set a texture parameter (see glTextureParameter documentation). Returns
    // true iff the parameter was set successfully.
    bool SetTextureParameter(GLenum pname, GLint value);

  private:
    // Type to keep track of texture and FBO states
    enum GLObjectState {
      kStateUninitialized,  // Not yet initialized
      kStateGenerated,      // Tex/FBO id is generated
      kStateBound,          // Tex/FBO are bound
      kStateAllocated       // Tex is allocated with (pixel) data
    };

    // Sets the frame and viewport dimensions.
    void InitDimensions(int width, int height);

    // Creates the internal texture.
    bool CreateTexture();

    // Creates the internal FBO.
    bool CreateFBO();

    // Copies pixels from texture or FBO to the specified buffer.
    bool CopyPixelsTo(uint8* buffer);

    // Reads the pixels from the internal texture to the given buffer.
    bool ReadTexturePixels(uint8* pixels) const;

    // Reads the pixels from the internal FBO to the given buffer.
    bool ReadFboPixels(uint8* pixels) const;

    // Writes the specified pixels to the internal texture.
    bool UploadTexturePixels(const uint8* pixels);

    // Binds the internal texture.
    bool BindTexture() const;

    // Binds the internal FBO.
    bool BindFrameBuffer() const;

    // Binds the internal texture to the internal FBO.
    bool BindTextureToFBO();

    // Get the (cached) identity shader.
    ShaderProgram* GetIdentity() const;

    // The width, height and format of the frame
    int width_;
    int height_;

    // The viewport dimensions
    int vp_x_;
    int vp_y_;
    int vp_width_;
    int vp_height_;

    // The texture and FBO ids
    GLuint texture_id_;
    GLuint fbo_id_;

    // The texture target: GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES
    GLuint texture_target_;

    // Flags whether or not frame holds a texture and FBO
    GLObjectState texture_state_;
    GLObjectState fbo_state_;

    // Flag whether frame owns the texture and FBO
    bool owns_;

    // The identity shader (cache).
    mutable ShaderProgram* identity_cache_;
};

} // namespace filterfw
} // namespace android

#endif  // FILTERFW_CORE_GL_FRAME_H__