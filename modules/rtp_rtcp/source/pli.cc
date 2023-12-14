/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/rtp_rtcp/source/rtcp_packet/pli.h"

#include "modules/rtp_rtcp/source/rtcp_packet/common_header.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "modules/rtp_rtcp/source/byte_io.h"
namespace webrtc {
namespace rtcp {
constexpr uint8_t Pli::kFeedbackMessageType;
// RFC 4585: Feedback format.
//
// Common packet format:
//
//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |V=2|P|   FMT   |       PT      |          length               |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                  SSRC of packet sender                        |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                  SSRC of media source                         |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  :            Feedback Control Information (FCI)                 :
//  :                                                               :

Pli::Pli() = default;

Pli::Pli(const Pli& pli) = default;

Pli::~Pli() = default;

//
// Picture loss indication (PLI) (RFC 4585).
// FCI: no feedback control information.
bool Pli::Parse(const CommonHeader& packet) {
  RTC_DCHECK_EQ(packet.type(), kPacketType);
  RTC_DCHECK_EQ(packet.fmt(), kFeedbackMessageType);

  if (packet.payload_size_bytes() < kCommonFeedbackLength) {
    RTC_LOG(LS_WARNING) << "Packet is too small to be a valid PLI packet";
    return false;
  }

  ParseCommonFeedback(packet.payload());
  return true;
}

size_t Pli::BlockLength() const {
  return kHeaderLength + kCommonFeedbackLength;
}

bool Pli::Create(uint8_t* packet,
                 size_t* index,
                 size_t max_length,
                 PacketReadyCallback callback) const {
  while (*index + BlockLength() > max_length) {
    if (!OnBufferFull(packet, index, callback))
      return false;
  }

  CreateHeader(kFeedbackMessageType, kPacketType, HeaderLength(), packet,
               index);
  CreateCommonFeedback(packet + *index);
  *index += kCommonFeedbackLength;
  return true;
}

// Diff Information FeedBack
//
//     0                   1                   2                   3
//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |V=2|P| FMT=15  |   PT=206      |             length            |
//    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//  0 |                  SSRC of packet sender                        |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  4 |                       Unused = 0                              |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  8 |             Unique identifier 'D' 'I' 'F' 'B'                 |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 12 |                   Diff Information 1                          |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+




Dfb::Dfb():diff(0) {}

Dfb::Dfb(const Dfb& dfb) = default;

Dfb::~Dfb() = default;

// diff feedback
bool Dfb::Parse(const CommonHeader& packet) {
  RTC_DCHECK(packet.type() == kPacketType);
  RTC_DCHECK_EQ(packet.fmt(), Psfb::kAfbMessageType);
  if (packet.payload_size_bytes() < 16) {
    RTC_LOG(LS_WARNING) << "Payload length " << packet.payload_size_bytes()
                        << " is too small for Dfb packet.";
    return false;
  }
  const uint8_t* const payload = packet.payload();
  if (kUniqueIdentifier != ByteReader<uint32_t>::ReadBigEndian(&payload[8])) {
    return false;
  }
  ParseCommonFeedback(payload);
  // payload[12]-payload[15] 12 13 14 15
  diff=0;
  diff+=((uint32_t)payload[12])<<24;
  diff+=((uint32_t)payload[13])<<16;
  diff+=((uint32_t)payload[14])<<8;
  diff+=(uint32_t)payload[15];
  return true;
}

size_t Dfb::BlockLength() const {
  return kHeaderLength + kCommonFeedbackLength+2*4;
}
bool Dfb::Create(uint8_t* packet,
                 size_t* index,
                 size_t max_length,
                 PacketReadyCallback callback) const {
  while (*index + BlockLength() > max_length) {
    if (!OnBufferFull(packet, index, callback))
      return false;
  }
  size_t index_end = *index + BlockLength();
  CreateHeader(Psfb::kAfbMessageType, kPacketType, HeaderLength(), packet,
               index);
  RTC_DCHECK_EQ(0, Psfb::media_ssrc());
  CreateCommonFeedback(packet + *index);
  *index += kCommonFeedbackLength;
  ByteWriter<uint32_t>::WriteBigEndian(packet + *index, kUniqueIdentifier);
  *index += sizeof(uint32_t);
  uint32_t diffinfo=diff;
  ByteWriter<uint32_t>::WriteBigEndian(packet + *index, diffinfo);
  *index += sizeof(uint32_t);
  RTC_DCHECK_EQ(index_end, *index);
  return true;
}

}  // namespace rtcp
}  // namespace webrtc
