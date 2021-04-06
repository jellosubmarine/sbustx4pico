#include <array>

// https://github.com/bolderflight/sbus

struct SbusTx {
  static constexpr uint8_t HEADER_ = 0x0F;
  static constexpr uint8_t FOOTER_ = 0x00;
  static constexpr uint8_t LEN_ = 25;
  static constexpr uint8_t CH17_ = 0x01;
  static constexpr uint8_t CH18_ = 0x02;
  static constexpr uint8_t LOST_FRAME_ = 0x04;
  static constexpr uint8_t FAILSAFE_ = 0x08;
  uint8_t buf_[LEN_];
  bool failsafe_ = false, lost_frame_ = false, ch17_ = false, ch18_ = false;
  std::array<uint16_t, 16> ch_;
  void updateBuffer() {
    buf_[0] = HEADER_;
    buf_[1] = static_cast<uint8_t>((ch_[0] & 0x07FF));
    buf_[2] =
        static_cast<uint8_t>((ch_[0] & 0x07FF) >> 8 | (ch_[1] & 0x07FF) << 3);
    buf_[3] =
        static_cast<uint8_t>((ch_[1] & 0x07FF) >> 5 | (ch_[2] & 0x07FF) << 6);
    buf_[4] = static_cast<uint8_t>((ch_[2] & 0x07FF) >> 2);
    buf_[5] =
        static_cast<uint8_t>((ch_[2] & 0x07FF) >> 10 | (ch_[3] & 0x07FF) << 1);
    buf_[6] =
        static_cast<uint8_t>((ch_[3] & 0x07FF) >> 7 | (ch_[4] & 0x07FF) << 4);
    buf_[7] =
        static_cast<uint8_t>((ch_[4] & 0x07FF) >> 4 | (ch_[5] & 0x07FF) << 7);
    buf_[8] = static_cast<uint8_t>((ch_[5] & 0x07FF) >> 1);
    buf_[9] =
        static_cast<uint8_t>((ch_[5] & 0x07FF) >> 9 | (ch_[6] & 0x07FF) << 2);
    buf_[10] =
        static_cast<uint8_t>((ch_[6] & 0x07FF) >> 6 | (ch_[7] & 0x07FF) << 5);
    buf_[11] = static_cast<uint8_t>((ch_[7] & 0x07FF) >> 3);
    buf_[12] = static_cast<uint8_t>((ch_[8] & 0x07FF));
    buf_[13] =
        static_cast<uint8_t>((ch_[8] & 0x07FF) >> 8 | (ch_[9] & 0x07FF) << 3);
    buf_[14] =
        static_cast<uint8_t>((ch_[9] & 0x07FF) >> 5 | (ch_[10] & 0x07FF) << 6);
    buf_[15] = static_cast<uint8_t>((ch_[10] & 0x07FF) >> 2);
    buf_[16] = static_cast<uint8_t>((ch_[10] & 0x07FF) >> 10 |
                                    (ch_[11] & 0x07FF) << 1);
    buf_[17] =
        static_cast<uint8_t>((ch_[11] & 0x07FF) >> 7 | (ch_[12] & 0x07FF) << 4);
    buf_[18] =
        static_cast<uint8_t>((ch_[12] & 0x07FF) >> 4 | (ch_[13] & 0x07FF) << 7);
    buf_[19] = static_cast<uint8_t>((ch_[13] & 0x07FF) >> 1);
    buf_[20] =
        static_cast<uint8_t>((ch_[13] & 0x07FF) >> 9 | (ch_[14] & 0x07FF) << 2);
    buf_[21] =
        static_cast<uint8_t>((ch_[14] & 0x07FF) >> 6 | (ch_[15] & 0x07FF) << 5);
    buf_[22] = static_cast<uint8_t>((ch_[15] & 0x07FF) >> 3);
    buf_[23] = 0x00 | (ch17_ * CH17_) | (ch18_ * CH18_) |
               (failsafe_ * FAILSAFE_) | (lost_frame_ * LOST_FRAME_);
    buf_[24] = FOOTER_;
  }
};