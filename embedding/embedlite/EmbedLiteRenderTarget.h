/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef EMBED_LITE_RENDER_TARGET_H
#define EMBED_LITE_RENDER_TARGET_H

#include "mozilla/embedlite/EmbedLiteApp.h"

namespace mozilla {
namespace layers {
class CompositingRenderTarget;
class CompositorParent;
}
namespace embedlite {

class EmbedLiteRenderTarget
{
public:
  virtual ~EmbedLiteRenderTarget();

  virtual int texture();
  virtual int fbo();

private:
  friend class EmbedLiteCompositorParent;
  friend class EmbedLiteViewThreadParent;
  EmbedLiteRenderTarget(int width, int height, mozilla::layers::CompositorParent* aComposite);
  virtual mozilla::layers::CompositingRenderTarget* GetRenderSurface() { return mCurrentRenderTarget; }

  RefPtr<mozilla::layers::CompositingRenderTarget> mCurrentRenderTarget;
};

} // namespace embedlite
} // namespace mozilla

#endif // EMBED_LITE_RENDER_TARGET_H
