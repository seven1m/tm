#pragma once

#include "string.hpp"
#include <assert.h>

namespace TM {

class StringView {
public:
    /**
     * Constructs an empty StringView.
     *
     * ```
     * auto view = StringView();
     * assert_eq(0, view.size());
     * assert_str_eq("", view);
     * ```
     */
    StringView() { }

    /**
     * Constructs a basic StringView, pointing at the given String.
     *
     * ```
     * auto str = String("foo");
     * auto view = StringView(&str);
     * assert_eq(3, view.size());
     * ```
     */
    explicit StringView(const String *string)
        : m_string { string }
        , m_length { string->length() } { }

    /**
     * Constructs a StringView with given offset.
     *
     * ```
     * auto str = String("foobar");
     * auto view = StringView(&str, 3);
     * assert_str_eq("bar", view);
     *
     * auto str2 = String("foo");
     * auto view2 = StringView(&str2, 3);
     * assert_str_eq("", view2);
     * ```
     *
     * This constructor aborts if the offset is past the end of the String.
     *
     * ```should_abort
     * auto str = String("foo");
     * auto view = StringView(&str, 4);
     * (void)view;
     * ```
     */
    explicit StringView(const String *string, size_t offset)
        : m_string { string }
        , m_offset { offset }
        , m_length { string->length() - offset } {
        assert(offset <= string->length());
    }

    /**
     * Constructs a StringView with given offset and length.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * assert_str_eq("bar", view);
     * ```
     *
     * This constructor aborts if the length is too long.
     *
     * ```should_abort
     * auto str = String("foobar");
     * auto view = StringView(&str, 3, 4);
     * (void)view;
     * ```
     */
    explicit StringView(const String *string, size_t offset, size_t length)
        : m_string { string }
        , m_offset { offset }
        , m_length { length } {
        assert(offset <= string->length());
        assert(length <= string->length() - offset);
    }

    /**
     * Constructs a StringView by copying an existing StringView.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view1 = StringView(&str, 4, 3);
     * auto view2 = StringView(view1);
     * assert_str_eq("bar", view2);
     * ```
     */
    StringView(const StringView &other) = default;

    /**
     * Replaces the StringView by copying from an another StringView.
     *
     * ```
     * auto str1 = String("foo");
     * auto view1 = StringView(&str1);
     * auto str2 = String("bar");
     * auto view2 = StringView(&str2);
     * view1 = view2;
     * assert_str_eq("bar", view1);
     * ```
     */
    StringView &operator=(const StringView &other) = default;

    /**
     * Returns the offset into the String.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4);
     * assert_eq(4, view.offset());
     * ```
     */
    size_t offset() const { return m_offset; }

    /**
     * Returns the number of bytes in the View.
     *
     * ```
     * auto str = String("🤖"); // 4-byte emoji
     * auto view = StringView(&str);
     * assert_eq(4, view.size());
     *
     * auto str2 = String("foobar");
     * auto view2 = StringView(&str2, 3);
     * assert_eq(3, view2.size());
     * ```
     */
    size_t size() const { return m_length; }

    /**
     * Returns the number of bytes in the View.
     *
     * ```
     * auto str = String("🤖"); // 4-byte emoji
     * auto view = StringView(&str);
     * assert_eq(4, view.length());
     *
     * auto str2 = String("foobar");
     * auto view2 = StringView(&str2, 3);
     * assert_eq(3, view2.length());
     * ```
     */
    size_t length() const { return m_length; }

    /**
     * Returns true if this and the given C string are equivalent.
     *
     * ```
     * auto str = String("abc");
     * auto view = StringView(&str);
     * auto cstr1 = "abc";
     * assert(view == cstr1);
     * auto cstr2 = "xyz";
     * assert_not(view == cstr2);
     * ```
     */
    bool operator==(const char *other) const {
        if (!other && !m_string)
            return true;
        auto other_length = strlen(other);
        if (!m_string && other_length == 0)
            return true;
        if (!m_string)
            return false;
        if (m_length != other_length)
            return false;
        return memcmp(m_string->c_str() + m_offset, other, sizeof(char) * m_length) == 0;
    }

    bool operator!=(const char *other) const {
        return !(*this == other);
    }

    /**
     * Returns true if this and the given character are equivalent.
     *
     * ```
     * auto str = String("abc");
     * auto view = StringView(&str, 1, 1);
     * assert(view == 'b');
     * assert_not(view == 'c');
     *
     * auto longerview = StringView(&str, 1, 2);
     * assert_not(longerview == 'b');
     * ```
     */
    bool operator==(const char other) const {
        return m_length == 1 && (*m_string)[m_offset] == other;
    }

    bool operator!=(const char other) const {
        return !(*this == other);
    }

    /**
     * Returns true if this and the given StringView are equivalent.
     *
     * ```
     * auto str1 = String("abc");
     * auto view1 = StringView(&str1);
     * auto view1b = StringView(&str1);
     * assert(view1 == view1b);
     *
     * auto str2 = String("xyz");
     * auto view2 = StringView(&str2);
     * assert_not(view1 == view2);
     *
     * assert(StringView() == StringView());
     * assert(StringView() == StringView(&str2, 0, 0));
     * assert(StringView(&str2, 0, 0) == StringView());
     *
     * auto str3 = String("abcabc");
     * auto view3 = StringView(&str3, 0, 3);
     * auto view3b = StringView(&str3, 3, 3);
     * assert(view3 == view3b);
     *
     * auto view4 = StringView(&str3, 1, 2);
     * auto view4b = StringView(&str3, 4, 2);
     * assert(view4 == view4b);
     * ```
     */
    bool operator==(const StringView &other) const {
        if (m_length != other.m_length)
            return false;
        if (m_length == 0)
            return true;
        if (m_string == other.m_string && m_offset == other.m_offset) // shortcut
            return true;
        return memcmp(m_string->c_str() + m_offset, other.m_string->c_str() + other.m_offset, sizeof(char) * m_length) == 0;
    }

    bool operator!=(const StringView &other) const {
        return !(*this == other);
    }

    /**
     * Returns true if this and the given String are equivalent.
     *
     * ```
     * auto str = String("abc");
     * auto view = StringView(&str);
     * assert(view == str);
     * assert_not(view == String());
     * ```
     */
    bool operator==(const String &other) const {
        return *this == StringView(&other);
    }

    bool operator!=(const String &other) const {
        return !(*this == other);
    }

    /**
     * Returns -1, 0, or 1 by comparing this StringView to the given StringView.
     * -1 is returned if this StringView is alphanumerically less than the other one.
     * 0 is returned if they are equivalent.
     * 1 is returned if this StringView is alphanumerically greater than the other one.
     *
     * ```
     * String str { "abcdef" };
     * StringView sv1 { &str, 3, 3 };
     * StringView sv2 { &str, 0, 3 };
     * assert_eq(1, sv1.cmp(sv2));
     * assert_eq(-1, sv2.cmp(sv1));
     * String str2 { "abc" };
     * StringView sv3 { &str2 };
     * assert_eq(0, sv2.cmp(sv3));
     * String str3 { "abcabc" };
     * StringView sv4 { &str3 };
     * assert_eq(-1, sv2.cmp(sv4));
     * ```
     */
    int cmp(const StringView &other) const {
        if (m_length == 0) {
            if (other.m_length == 0)
                return 0;
            return -1;
        }
        for (size_t i = 0; i < std::min(m_length, other.m_length); ++i) {
            const auto c1 = (unsigned char)(*this)[i], c2 = (unsigned char)other[i];
            if (c1 < c2)
                return -1;
            else if (c1 > c2)
                return 1;
        }
        if (m_length == other.m_length)
            return 0;
        else if (m_length < other.m_length)
            return -1;
        else
            return 1;
    }

    /**
     * Returns -1, 0, or 1 by comparing this StringView to the given String.
     * -1 is returned if this StringView is alphanumerically less than the other one.
     * 0 is returned if they are equivalent.
     * 1 is returned if this StringView is alphanumerically greater than the other one.
     *
     * ```
     * String str1 { "def" };
     * String str2 { "abc" };
     * StringView sv1 { &str1 };
     * StringView sv2 { &str2 };
     * assert_eq(1, sv1.cmp(str2));
     * assert_eq(-1, sv2.cmp(str1));
     * String str3 { "abc" };
     * assert_eq(0, sv2.cmp(str3));
     * String str4 { "abcabc" };
     * assert_eq(-1, sv2.cmp(str4));
     * ```
     */
    int cmp(const String &other) const {
        return cmp(StringView { &other });
    }

    /**
     * Returns a new String constructed from this view.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * auto str2 = view.to_string();
     * assert_str_eq("bar", str2);
     * ```
     */
    String to_string() const {
        if (!m_string)
            return String();
        return String { m_string->c_str() + m_offset, m_length };
    }

    /**
     * Returns a new String constructed from this view.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * String str2 = view;
     * assert_str_eq("bar", str2);
     * ```
     */
    operator String() const {
        return to_string();
    }

    /**
     * Returns a new String constructed from this view.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * auto str2 = view.clone();
     * assert_str_eq("bar", str2);
     * ```
     */
    String clone() const { return to_string(); }

    /**
     * Returns the character at the specified index.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * assert_eq('a', view.at(1));
     * ```
     *
     * This method aborts if the given index is beyond the end of the String.
     *
     * ```should_abort
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * view.at(10);
     * ```
     */
    char at(size_t index) const {
        assert(m_string);
        return m_string->at(m_offset + index);
    }

    /**
     * Returns the character at the specified index.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * assert_eq('a', view[1]);
     * ```
     *
     * WARNING: This method does *not* check that the given
     * index is within the bounds of the string data!
     */
    char operator[](size_t index) const {
        assert(m_string);
        return (*m_string)[m_offset + index];
    }

    /**
     * Returns a pointer to the underlying C string data.
     * This method should be used with care, because it does not
     * contain a null termination character at the expected location.
     * It only points to the offset of the String where this
     * StringView starts.
     *
     * ```
     * auto str = String("foo-bar-baz");
     * auto view = StringView(&str, 4, 3);
     * assert_eq(0, strcmp("bar-baz", view.dangerous_pointer_to_underlying_data()));
     * ```
     */
    const char *dangerous_pointer_to_underlying_data() const {
        assert(m_string);
        return m_string->c_str() + m_offset;
    }

    /**
     * Returns true if the StringView has a length of zero.
     *
     * ```
     * auto str = String("abc");
     * auto view1 = StringView(&str);
     * assert_not(view1.is_empty());
     * auto view2 = StringView(&str, 0, 0);
     * assert(view2.is_empty());
     * ```
     */
    bool is_empty() const { return m_length == 0; }

private:
    const String *m_string { nullptr };
    size_t m_offset { 0 };
    size_t m_length { 0 };
};

/**
 * Concatenate a StringView to a String without having to convert to
 * StringView to a String first.
 * ```
 * const auto str1 = String { "abc" };
 * const auto str2 = String { "cdefg" };
 * const auto sv = StringView { &str2, 1, 3 };
 *
 * assert_str_eq("abcdef", str1 + sv);
 * assert_str_eq("abc", str1);
 * ```
 */
inline String operator+(const String &lhs, const StringView &rhs) {
    auto res = lhs.clone();
    res.append(rhs.dangerous_pointer_to_underlying_data(), rhs.size());
    return res;
}

/**
 * Append a StringView to a String without having to create a new String first
 * ```
 * auto str1 = String { "abc" };
 * const auto str2 = String { "cdefg" };
 * const auto sv = StringView { &str2, 1, 3 };
 * str1 += sv;
 *
 * assert_str_eq("abcdef", str1);
 * ```
 */
inline String &operator+=(String &lhs, const StringView &rhs) {
    lhs.append(rhs.dangerous_pointer_to_underlying_data(), rhs.size());
    return lhs;
}

}
