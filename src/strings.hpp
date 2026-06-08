#pragma once
#include <fstream>
#include <sstream>
#include <string>

namespace Strings {
    /// Grabs a line from a file, taking into account all 3 possible line delimiters.
    ///
    /// @param stream  the file stream to read from
    /// @param s       the string to assign the line to
    ///
    /// @return a boolean determining if there was a line to read
    inline bool getLine(std::ifstream& stream, std::string& s) {
        s.clear();
        char c;

        if(!stream.get(c)) {
            return false;
        }

        while(true) {
            if (c == '\n') {
                return true;
            }

            if (c == '\r') {
                if (stream.peek() == '\n') {
                    stream.get();
                }
                return true;
            }

            s.push_back(c);

            if(!stream.get(c)) {
                return true;
            }
        }
    }

    /// Grabs a line from a UTF-32 encoded string, taking into account all 3 possible line delimiters.
    ///
    /// @param stream  the string stream to read from
    /// @param s       the string to assign the line to
    ///
    /// @return a boolean determining if there was a line to read
    inline bool getLine(std::basic_stringstream<char32_t>& stream, std::u32string& s) {
        s.clear();
        char32_t c;

        if(!stream.get(c)) {
            return false;
        }

        while(true) {
            if (c == U'\n') {
                return true;
            }

            if (c == U'\r') {
                if (stream.peek() == U'\n') {
                    stream.get();
                }
                return true;
            }

            s.push_back(c);

            if(!stream.get(c)) {
                return true;
            }
        }
    }

    /// Converts a string from UTF-32 to UTF-8.
    ///
    /// @param s  the UTF-32 string to convert
    ///
    /// @return  a UTF-8 representation of the supplied string
    inline std::string toUtf8(const std::u32string& s) {
        std::string result;
        for(const char32_t c : s) {
            if(c <= 0x7F) {
                result += static_cast<char>(c);
            } else if(c <= 0x7FF) {
                result += static_cast<char>(0xC0 | c >> 6 & 0x1F);
                result += static_cast<char>(0x80 | c & 0x3F);
            } else if(c <= 0xFFFF) {
                result += static_cast<char>(0xE0 | c >> 12 & 0x0F);
                result += static_cast<char>(0x80 | c >> 6 & 0x3F);
                result += static_cast<char>(0x80 | c & 0x3F);
            } else if(c <= 0x10FFFF) {
                result += static_cast<char>(0xF0 | c >> 18 & 0x07);
                result += static_cast<char>(0x80 | c >> 12 & 0x3F);
                result += static_cast<char>(0x80 | c >> 6 & 0x3F);
                result += static_cast<char>(0x80 | c & 0x3F);
            }
        }
        return result;
    }

    /// Converts a string from UTF-8 to UTF-32.
    ///
    /// @param s  the UTF-8 string to convert
    ///
    /// @return  a UTF-32 representation of the supplied string
    inline std::u32string fromUtf8(const std::string& s) {
        std::u32string result;
        size_t i = 0;

        while(i < s.length()) {
            const auto b = static_cast<uint8_t>(s[i]);
            char32_t c = 0;
            size_t bytesToRead = 0;

            if(b <= 0x7F) {
                c = b;
                bytesToRead = 0;
            } else if((b & 0xE0) == 0xC0) {
                c = b & 0x1F;
                bytesToRead = 1;
            } else if((b & 0xF0) == 0xE0) {
                c = b & 0x0F;
                bytesToRead = 2;
            } else if((b & 0xF8) == 0xF0) {
                c = b & 0x07;
                bytesToRead = 3;
            } else {
                c = 0xFFFD;
                bytesToRead = 0;
            }

            if(i + bytesToRead >= s.length()) {
                result += 0xFFFD;
                break;
            }

            for(size_t j = 0; j < bytesToRead; j++) {
                i++;
                if (const auto next = static_cast<uint8_t>(s[i]); (next & 0xC0) == 0x80) {
                    c = c << 6 | next & 0x3F;
                } else {
                    c = 0xFFFD;
                    break;
                }
            }

            result += c;
            i++;
        }
        return result;
    }
}