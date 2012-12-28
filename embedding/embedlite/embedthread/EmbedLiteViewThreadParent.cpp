/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define LOG_COMPONENT "EmbedLiteViewThreadParent"

#include "EmbedLiteViewThreadParent.h"
#include "EmbedLog.h"
#include "EmbedLiteApp.h"
#include "EmbedLiteView.h"

#include "EmbedLiteCompositorParent.h"
#include "mozilla/unused.h"
#include "mozilla/layers/AsyncPanZoomController.h"
#include "mozilla/layers/GeckoContentController.h"

using namespace mozilla::layers;

namespace mozilla {
namespace embedlite {

class EmbedGeckoContentController : public GeckoContentController
{
public:
    NS_INLINE_DECL_THREADSAFE_REFCOUNTING(EmbedGeckoContentController)

    virtual void RequestContentRepaint(const FrameMetrics& aFrameMetrics)
    {
        LOGC("EmbedGeckoContentController", "");
        if (mParent) {
            unused << mParent->SendUpdateFrame(aFrameMetrics);
        }
    }
    virtual void HandleDoubleTap(const nsIntPoint& aPoint)
    {
        LOGC("EmbedGeckoContentController", "pt[%i,%i]", aPoint.x, aPoint.y);
        if (mParent) {
            unused << mParent->SendHandleDoubleTap(aPoint);
        }
    }
    virtual void HandleSingleTap(const nsIntPoint& aPoint)
    {
        LOGC("EmbedGeckoContentController", "pt[%i,%i]", aPoint.x, aPoint.y);
        if (mParent) {
            unused << mParent->SendHandleSingleTap(aPoint);
        }
    }
    virtual void HandleLongTap(const nsIntPoint& aPoint)
    {
        LOGC("EmbedGeckoContentController", "pt[%i,%i]", aPoint.x, aPoint.y);
        if (mParent) {
            unused << mParent->SendHandleLongTap(aPoint);
        }
    }

    EmbedGeckoContentController() {}
    virtual ~EmbedGeckoContentController() {}
    EmbedLiteViewThreadParent* mParent;
};

EmbedLiteViewThreadParent::EmbedLiteViewThreadParent(const uint32_t& id)
  : mId(id)
  , mView(EmbedLiteApp::GetInstance()->GetViewByID(id))
  , mCompositor(nullptr)
  , mScrollOffset(0, 0)
  , mLastScale(1.0f)
{
    MOZ_COUNT_CTOR(EmbedLiteViewThreadParent);
    LOGT("id:%u", mId);
    mView->SetImpl(this);
}

EmbedLiteViewThreadParent::~EmbedLiteViewThreadParent()
{
    MOZ_COUNT_DTOR(EmbedLiteViewThreadParent);
    LOGT();
    if (mGeckoController) {
        mGeckoController->mParent = NULL;
    }
    if (mView) {
        mView->SetImpl(NULL);
    }
}

void
EmbedLiteViewThreadParent::ActorDestroy(ActorDestroyReason aWhy)
{
    LOGT("reason:%i", aWhy);
}

void
EmbedLiteViewThreadParent::SetCompositor(EmbedLiteCompositorParent* aCompositor)
{
    LOGT();
    mCompositor = aCompositor;
    mGeckoController = new EmbedGeckoContentController();
    mGeckoController->mParent = this;
    UpdateScrollController();
}

void
EmbedLiteViewThreadParent::UpdateScrollController()
{
    mController = nullptr;
    if (mView->GetPanZoomControlType() != EmbedLiteView::PanZoomControlType::EXTERNAL) {
        AsyncPanZoomController::GestureBehavior type;
        if (mView->GetPanZoomControlType() == EmbedLiteView::PanZoomControlType::GECKO_SIMPLE) {
            type = AsyncPanZoomController::DEFAULT_GESTURES;
        } else if (mView->GetPanZoomControlType() == EmbedLiteView::PanZoomControlType::GECKO_TOUCH) {
            type = AsyncPanZoomController::USE_GESTURE_DETECTOR;
        }
        mController = new AsyncPanZoomController(mGeckoController, type);
        mController->SetCompositorParent(mCompositor);
    }
}

AsyncPanZoomController*
EmbedLiteViewThreadParent::GetDefaultPanZoomController()
{
    LOGT("t");
    return mController;
}

// Child notification

bool
EmbedLiteViewThreadParent::RecvInitialized()
{
    mView->GetListener()->ViewInitialized();
    return true;
}

bool
EmbedLiteViewThreadParent::RecvOnTitleChanged(const nsString& aTitle)
{
    LOGNI();
    mView->GetListener()->OnTitleChanged(aTitle.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLocationChanged(const nsCString& aLocation,
                                                 const bool& aCanGoBack,
                                                 const bool& aCanGoForward)
{
    LOGNI();
    mView->GetListener()->OnLocationChanged(aLocation.get(), aCanGoBack, aCanGoForward);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLoadStarted(const nsCString& aLocation)
{
    LOGNI();
    mView->GetListener()->OnLoadStarted(aLocation.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLoadFinished()
{
    LOGNI();
    mView->GetListener()->OnLoadFinished();
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLoadRedirect()
{
    LOGNI();
    mView->GetListener()->OnLoadRedirect();
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLoadProgress(const int32_t& aProgress, const int32_t& aCurTotal, const int32_t& aMaxTotal)
{
    LOGNI("progress:%i", aProgress);
    mView->GetListener()->OnLoadProgress(aProgress, aCurTotal, aMaxTotal);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnSecurityChanged(const nsCString& aStatus,
                                                 const uint32_t& aState)
{
    LOGNI();
    mView->GetListener()->OnSecurityChanged(aStatus.get(), aState);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnFirstPaint(const int32_t& aX,
                                            const int32_t& aY)
{
    LOGNI();
    mView->GetListener()->OnFirstPaint(aX, aY);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnContentLoaded(const nsString& aDocURI)
{
    LOGNI();
    mView->GetListener()->OnContentLoaded(aDocURI.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnLinkAdded(const nsString& aHref,
                                           const nsString& aCharset,
                                           const nsString& aTitle,
                                           const nsString& aRel,
                                           const nsString& aSizes,
                                           const nsString& aType)
{
    LOGNI();
    mView->GetListener()->OnLinkAdded(aHref.get(),
                                      aCharset.get(),
                                      aTitle.get(),
                                      aRel.get(),
                                      aSizes.get(),
                                      aType.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnWindowOpenClose(const nsString& aType)
{
    LOGNI();
    mView->GetListener()->OnWindowOpenClose(aType.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnPopupBlocked(const nsCString& aSpec,
                                              const nsCString& aCharset,
                                              const nsString& aPopupFeatures,
                                              const nsString& aPopupWinName)
{
    LOGNI();
    mView->GetListener()->OnPopupBlocked(aSpec.get(),
                                         aCharset.get(),
                                         aPopupFeatures.get(),
                                         aPopupWinName.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnPageShowHide(const nsString& aType,
                                              const bool& aPersisted)
{
    LOGNI();
    mView->GetListener()->OnPageShowHide(aType.get(),
                                         aPersisted);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnScrolledAreaChanged(const uint32_t& aWidth,
                                                     const uint32_t& aHeight)
{
    LOGNI("area[%u,%u]", aWidth, aHeight);
    mView->GetListener()->OnScrolledAreaChanged(aWidth, aHeight);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnScrollChanged(const int32_t& offSetX,
                                               const int32_t& offSetY)
{
    LOGNI("off[%i,%i]", offSetX, offSetY);
    mView->GetListener()->OnScrollChanged(offSetX, offSetY);
    return false;
}

bool
EmbedLiteViewThreadParent::RecvOnObserve(const nsCString& aTopic,
                                         const nsString& aData)
{
    LOGNI("data:%p, top:%s\n", NS_ConvertUTF16toUTF8(aData).get(), aTopic.get());
    mView->GetListener()->OnObserve(aTopic.get(), aData.get());
    return false;
}

bool
EmbedLiteViewThreadParent::RecvUpdateZoomConstraints(const bool& val, const float& min, const float& max)
{
    if (mController) {
        mController->UpdateZoomConstraints(val, min, max);
    }
    return true;
}

bool
EmbedLiteViewThreadParent::RecvZoomToRect(const gfxRect& aRect)
{
    if (mController) {
        mController->ZoomToRect(aRect);
    }
    return true;
}

// Incoming API calls

void
EmbedLiteViewThreadParent::LoadURL(const char* aUrl)
{
    LOGT();
    unused << SendLoadURL(NS_ConvertUTF8toUTF16(nsCString(aUrl)));
}

void
EmbedLiteViewThreadParent::RenderToImage(unsigned char *aData, int imgW, int imgH, int stride, int depth)
{
    LOGT();
    if (mCompositor) {
        mCompositor->RenderToImage(aData, imgW, imgH, stride, depth);
    }
}

void
EmbedLiteViewThreadParent::RenderGL()
{
    LOGT();
    if (mCompositor) {
        mCompositor->RenderGL();
    }
}

bool
EmbedLiteViewThreadParent::ScrollBy(int aDX, int aDY, bool aDoOverflow)
{
    LOGT("d[%i,%i]", aDX, aDY);
    mScrollOffset.MoveBy(-aDX, -aDY);
    if (mCompositor) {
        mCompositor->SetTransformation(mLastScale, nsIntPoint(mScrollOffset.x, mScrollOffset.y));
        mCompositor->ScheduleRenderOnCompositorThread();
    }

    return true;
}

void
EmbedLiteViewThreadParent::SetViewSize(int width, int height)
{
    LOGT("sz[%i,%i]", width, height);
    mViewSize = gfxSize(width, height);
    unused << SendSetViewSize(mViewSize);
    if (mController) {
        mController->UpdateCompositionBounds(nsIntRect(0, 0, width, height));
    }
}

void
EmbedLiteViewThreadParent::SetGLViewPortSize(int width, int height)
{
    if (mCompositor) {
        mCompositor->SetSurfaceSize(width, height);
    }
}

void
EmbedLiteViewThreadParent::SetGLViewTransform(gfxMatrix matrix)
{
    if (mCompositor) {
        mCompositor->SetWorldTransform(matrix);
        mCompositor->SetClipping(gfxRect(gfxPoint(0, 0), mViewSize));
    }
}

void
EmbedLiteViewThreadParent::SetTransformation(float aScale, nsIntPoint aScrollOffset)
{
    if (mCompositor) {
        mCompositor->SetTransformation(aScale, aScrollOffset);
    }
}

void
EmbedLiteViewThreadParent::ScheduleRender()
{
    if (mCompositor) {
        mCompositor->ScheduleRenderOnCompositorThread();
    }
}

void
EmbedLiteViewThreadParent::ReceiveInputEvent(const InputData& aEvent)
{
    if (mController) {
        nsEventStatus status;
        status = mController->ReceiveInputEvent(aEvent);
    }
}

void
EmbedLiteViewThreadParent::MousePress(int x, int y, int mstime, unsigned int buttons, unsigned int modifiers)
{
    LOGT("pt[%i,%i], t:%i, bt:%u, mod:%u", x, y, mstime, buttons, modifiers);
    if (mController) {
        nsEventStatus status;
        MultiTouchInput event(MultiTouchInput::MULTITOUCH_START, mstime);
        event.mTouches.AppendElement(SingleTouchData(0,
                                     nsIntPoint(x, y),
                                     nsIntPoint(1, 1),
                                     180.0f,
                                     1.0f));
        status = mController->ReceiveInputEvent(event);
    }
}

void
EmbedLiteViewThreadParent::MouseRelease(int x, int y, int mstime, unsigned int buttons, unsigned int modifiers)
{
    LOGT("pt[%i,%i], t:%i, bt:%u, mod:%u", x, y, mstime, buttons, modifiers);
    if (mController) {
        nsEventStatus status;
        MultiTouchInput event(MultiTouchInput::MULTITOUCH_END, mstime);
        event.mTouches.AppendElement(SingleTouchData(0,
                                     nsIntPoint(x, y),
                                     nsIntPoint(1, 1),
                                     180.0f,
                                     1.0f));
        status = mController->ReceiveInputEvent(event);
    }
}

void
EmbedLiteViewThreadParent::MouseMove(int x, int y, int mstime, unsigned int buttons, unsigned int modifiers)
{
    LOGT("pt[%i,%i], t:%i, bt:%u, mod:%u", x, y, mstime, buttons, modifiers);
    if (mController) {
        nsEventStatus status;
        MultiTouchInput event(MultiTouchInput::MULTITOUCH_MOVE, mstime);
        event.mTouches.AppendElement(SingleTouchData(0,
                                     nsIntPoint(x, y),
                                     nsIntPoint(1, 1),
                                     180.0f,
                                     1.0f));
        status = mController->ReceiveInputEvent(event);
    }
}

} // namespace embedlite
} // namespace mozilla
