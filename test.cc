#include "main.cc"

#include <iostream>
#include <string>

const char* cases[][2] = {
  {"i\xd0\x98\xcc\x86", "\xd0\x98\xcc\x86i"},
  {"", ""},
  {"a", "a"},
  {"ab", "ba"},
  {"a b", "b a"},
  {"\xd1\x84", "\xd1\x84"},  // single russian letter
  {"x\xd1\x84", "\xd1\x84x"},
  {"y\xd1\x84z", "z\xd1\x84y"},
  {"\xd1\x84\xd1\x85", "\xd1\x85\xd1\x84"},
  {"\xd0\x98\xcc\x86", "\xd0\x98\xcc\x86"},  // single russian letter
  {"i\xd0\x98\xcc\x86", "\xd0\x98\xcc\x86i"},
  {"\xd0\x98\xcc\x86i", "i\xd0\x98\xcc\x86"},
  {"\xd0\x98\xcc\x86\xd1\x84", "\xd1\x84\xd0\x98\xcc\x86"},
  {"z\xd0\x98\xcc\x86\xcc\x88y", "y\xd0\x98\xcc\x86\xcc\x88z"}
};

int main() {
  size_t arraysize = sizeof(cases) / sizeof(cases[0]);
  for (size_t test_case = 0; test_case < arraysize; ++test_case) {
    icu::UnicodeString s = icu::UnicodeString::fromUTF8(cases[test_case][0]);
    UnicodeStringReverter r(s);
    r.Revert();
    std::string result;
    s.toUTF8String(result);
    if (result == std::string(cases[test_case][1])) {
      std::cout << "Test " << test_case + 1 << '/' << arraysize << " Ok\n";
    } else {
      std::cout << "Test " << test_case + 1 << '/' << arraysize << " Failed!\n"
                << "     Expected: " << cases[test_case][1] << " but got "
                << result << '\n';
    }
  }
}
