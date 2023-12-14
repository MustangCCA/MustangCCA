/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_PLI_H_
#define MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_PLI_H_

#include "modules/rtp_rtcp/source/rtcp_packet/psfb.h"

namespace webrtc {
namespace rtcp {
class CommonHeader;
// Picture loss indication (PLI) (RFC 4585).
class Pli : public Psfb {
 public:
  static constexpr uint8_t kFeedbackMessageType = 1;

  Pli();
  Pli(const Pli& pli);
  ~Pli() override;

  bool Parse(const CommonHeader& packet);

  size_t BlockLength() const override;

  bool Create(uint8_t* packet,
              size_t* index,
              size_t max_length,
              PacketReadyCallback callback) const override;
};

// Diff feedback 
class Dfb : public Psfb {
 public:
  Dfb();
  Dfb(const Dfb&);
  ~Dfb() override;

  bool Parse(const CommonHeader& packet);

  void Setdiffinfo(int32_t diff) { this->diff = diff; }

  int32_t get_diff() const { return diff; }

  size_t BlockLength() const override;

  bool Create(uint8_t* packet,
              size_t* index,
              size_t max_length,
              PacketReadyCallback callback) const override;

 private:
  static constexpr uint32_t kUniqueIdentifier = 0x44494642;  // 'D' 'I' 'F' 'B'.
  int32_t diff;
};

}  // namespace rtcp
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_PLI_H_
