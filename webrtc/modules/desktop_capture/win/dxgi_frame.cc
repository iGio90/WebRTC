/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/desktop_capture/win/dxgi_frame.h"

#include <utility>

#include "webrtc/base/checks.h"
#include "webrtc/modules/desktop_capture/desktop_frame.h"
#include "webrtc/modules/desktop_capture/win/dxgi_duplicator_controller.h"

namespace webrtc {

DxgiFrame::DxgiFrame(SharedMemoryFactory* factory)
    : factory_(factory) {}

DxgiFrame::~DxgiFrame() = default;

bool DxgiFrame::Prepare(DesktopSize size, DesktopCapturer::SourceId source_id) {
  if (source_id != source_id_) {
    // Once the source has been changed, the entire source should be copied.
    source_id_ = source_id;
    context_.Reset();
  }

  if (resolution_change_detector_.IsChanged(size)) {
    // Once the output size changed, recreate the SharedDesktopFrame.
    frame_.reset();
    resolution_change_detector_.Reset();
  }

  if (!frame_) {
    std::unique_ptr<DesktopFrame> frame;
    if (factory_) {
      frame = SharedMemoryDesktopFrame::Create(size, factory_);
    } else {
      frame.reset(new BasicDesktopFrame(size));
    }
    if (!frame) {
      return false;
    }

    frame_ = SharedDesktopFrame::Wrap(std::move(frame));
  }

  return !!frame_;
}

SharedDesktopFrame* DxgiFrame::frame() const {
  RTC_DCHECK(frame_);
  return frame_.get();
}

DxgiFrame::Context* DxgiFrame::context() {
  RTC_DCHECK(frame_);
  return &context_;
}

}  // namespace webrtc
