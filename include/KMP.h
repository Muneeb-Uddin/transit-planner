#ifndef KMP_H
#define KMP_H

#include "DynamicArray.h"
#include <string>

inline string lowercase(string text) {
    for (size_t i = 0; i < text.size(); ++i)
        if (text[i] >= 'A' && text[i] <= 'Z')
            text[i] = static_cast<char>(text[i] - 'A' + 'a');
    return text;
}

class KmpPattern {
    string pattern;
    DynamicArray<size_t> prefixLength;

public:
    explicit KmpPattern(const string& term) : pattern(lowercase(term)) {
        if (pattern.empty()) return;
        prefixLength.push_back(0);
        size_t matched = 0;
        for (size_t i = 1; i < pattern.size(); ++i) {
            while (matched > 0 && pattern[i] != pattern[matched])
                matched = prefixLength[static_cast<int>(matched - 1)];
            if (pattern[i] == pattern[matched]) ++matched;
            prefixLength.push_back(matched);
        }
    }

    bool matches(const string& text) const {
        if (pattern.empty()) return true;
        const string normalized = lowercase(text);
        size_t matched = 0;
        for (size_t i = 0; i < normalized.size(); ++i) {
            while (matched > 0 && normalized[i] != pattern[matched])
                matched = prefixLength[static_cast<int>(matched - 1)];
            if (normalized[i] == pattern[matched]) ++matched;
            if (matched == pattern.size()) return true;
        }
        return false;
    }
};

#endif
