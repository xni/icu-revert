#include <queue>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include <brkiter.h>
#include <locid.h>
#include <unistr.h>


class UnicodeStringReverter {
 public:
  UnicodeStringReverter(UnicodeString& data)
      : data_(data),
        free_space_in_front_(0),
        free_space_in_back_(0),
        index_of_empty_space_in_front_(0),
        index_of_empty_space_in_back_(data.length()) {}

  void FillBackBuffer() {
    int32_t current_letter_index = fwd_iter_->current();
    int32_t next_letter_index = fwd_iter_->next();
    size_t characters_subsequence_len = next_letter_index - current_letter_index;
    icu::UnicodeString characters_subsequence(characters_subsequence_len, 0, 0);
    data_.extract(current_letter_index, characters_subsequence_len, characters_subsequence);
    back_buffer_.push(characters_subsequence);
    free_space_in_front_ += characters_subsequence_len;
  }

  void FillFrontBuffer() {
    int32_t current_letter_end = bck_iter_->current();
    int32_t current_letter_begin = bck_iter_->previous();
    size_t characters_subsequence_len = current_letter_end - current_letter_begin;
    icu::UnicodeString characters_subsequence(characters_subsequence_len, 0, 0);
    data_.extract(current_letter_begin, characters_subsequence_len, characters_subsequence);
    front_buffer_.push(characters_subsequence);
    free_space_in_back_ += characters_subsequence_len;
  }

  void ReleaseFrontBuffer() {
    while (!front_buffer_.empty() &&
           free_space_in_front_ >= front_buffer_.front().length()) {
      size_t subsequence_len = front_buffer_.front().length();
      data_.replace(index_of_empty_space_in_front_,
                    subsequence_len,
                    front_buffer_.front());
      index_of_empty_space_in_front_ += subsequence_len;
      free_space_in_front_ -= subsequence_len;
      front_buffer_.pop();
    }
  }

  void ReleaseBackBuffer() {
    while (!back_buffer_.empty() &&
           free_space_in_back_ >= back_buffer_.front().length()) {
      size_t subsequence_len = back_buffer_.front().length();
      data_.replace(index_of_empty_space_in_back_ - subsequence_len,
                    subsequence_len,
                    back_buffer_.front());
      index_of_empty_space_in_back_ -= subsequence_len;
      free_space_in_back_ -= subsequence_len;
      back_buffer_.pop();
    }
  }

  bool ItersAreEqual() const {
    return fwd_iter_->current() == bck_iter_->current();
  }

  void Revert() {
    if (data_.length() < 2)
      return;

    UErrorCode ec = U_ZERO_ERROR;
    icu::Locale ru_locale = icu::Locale("ru");
    fwd_iter_.reset(icu::BreakIterator::createCharacterInstance(ru_locale, ec));
    fwd_iter_->setText(data_);
    fwd_iter_->first();
    bck_iter_.reset(icu::BreakIterator::createCharacterInstance(ru_locale, ec));
    bck_iter_->setText(data_);
    bck_iter_->last();

    while (true) {
      FillBackBuffer();
      ReleaseFrontBuffer();

      // Forward iterator met backward.
      if (ItersAreEqual())
        break;

      FillFrontBuffer();
      ReleaseBackBuffer();

      // Backward iterator met forward.
      if (ItersAreEqual())
        break;
    }

    // All data is stored in buffers, so free space in front and back is common.
    free_space_in_front_ += free_space_in_back_;
    ReleaseFrontBuffer();

    free_space_in_back_ = free_space_in_front_;
    ReleaseBackBuffer();
  }

 private:
  size_t index_of_empty_space_in_front_;
  size_t index_of_empty_space_in_back_;
  size_t free_space_in_front_;
  size_t free_space_in_back_;

  std::unique_ptr<icu::BreakIterator> fwd_iter_;
  std::unique_ptr<icu::BreakIterator> bck_iter_;

  icu::UnicodeString& data_;
  std::queue<icu::UnicodeString> front_buffer_;
  std::queue<icu::UnicodeString> back_buffer_;
};
