#ifndef STRING_TEST_HPP_
#define STRING_TEST_HPP_

#include "catch2/catch_test_macros.hpp"

#include "efp.hpp"
#include "test_common.hpp"

using namespace efp;

TEST_CASE("BasicString<Char>", "[BasicString<Char>]") {
    SECTION("Default Constructor") {
        String str;
        CHECK(str.empty());
        CHECK(str.size() == 0);
        CHECK(str.capacity() == 0);

        WString wstr;
        CHECK(wstr.empty());
        CHECK(wstr.size() == 0);
        CHECK(wstr.capacity() == 0);
    }

    SECTION("Constructor from C-Style String") {
        String str("Hello");
        CHECK(str.size() == 5);
        // CHECK(std::strcmp(str.data(), "Hello") == 0);
        CHECK(std::strcmp(str.c_str(), "Hello") == 0);

        WString wstr(L"Hello");
        CHECK(wstr.size() == 5);
        // CHECK(std::wcscmp(wstr.data(), L"Hello") == 0);
        CHECK(std::wcscmp(wstr.c_str(), L"Hello") == 0);
    }

    SECTION("Copy Constructor") {
        String original("Hello");
        String copy = original;
        CHECK(copy == original);
        CHECK(copy.size() == original.size());

        WString woriginal(L"Hello");
        WString wcopy = woriginal;
        CHECK(wcopy == woriginal);
    }

    SECTION("Move Constructor") {
        String original("Hello");
        String moved(efp::move(original));
        // CHECK(std::strcmp(moved.data(), "Hello") == 0);
        CHECK(std::strcmp(moved.c_str(), "Hello") == 0);
        CHECK(original.empty());

        WString woriginal(L"Hello");
        WString wmoved(efp::move(woriginal));
        // CHECK(std::wcscmp(wmoved.data(), L"Hello") == 0);
        CHECK(std::wcscmp(wmoved.c_str(), L"Hello") == 0);
        CHECK(woriginal.empty());
    }

    SECTION("Copy Assignment Operator") {
        String str1("Hello");
        String str2;
        str2 = str1;
        CHECK(str2 == str1);
        CHECK(str2.size() == str1.size());

        WString wstr1(L"Hello");
        WString wstr2;
        wstr2 = wstr1;
        CHECK(wstr2 == wstr1);
        CHECK(wstr2.size() == wstr1.size());
    }

    SECTION("Move Assignment Operator") {
        String str1("Hello");
        String str2;
        str2 = efp::move(str1);
        // CHECK(std::strcmp(str2.data(), "Hello") == 0);
        CHECK(std::strcmp(str2.c_str(), "Hello") == 0);
        CHECK(str1.empty());

        WString wstr1(L"Hello");
        WString wstr2;
        wstr2 = efp::move(wstr1);
        // CHECK(std::wcscmp(wstr2.data(), L"Hello") == 0);
        CHECK(std::wcscmp(wstr2.c_str(), L"Hello") == 0);
        CHECK(wstr1.empty());
    }

    SECTION("Destruction") {
        // This tests whether destruction happens without errors.
        { String str("Temporary String"); } // str goes out of scope here and should be destroyed
        CHECK(true);                        // If we reach this point, destruction didn't cause a crash
    }

    SECTION("BasicString<Char>::operator[]") {
        String str("Hello");
        CHECK(str[0] == 'H');
        CHECK(str[4] == 'o');

        WString wstr(L"Hello");
        CHECK(wstr[0] == L'H');
        CHECK(wstr[4] == L'o');
    }

    SECTION("BasicString<Char>::at") {
        String str("Hello");
        CHECK(str.at(0) == 'H');
        CHECK(str.at(4) == 'o');

        WString wstr(L"Hello");
        CHECK(wstr.at(0) == L'H');
        CHECK(wstr.at(4) == L'o');
    }

    SECTION("BasicString<Char>::front") {
        String str("Hello");
        CHECK(str.front() == 'H');

        WString wstr(L"Hello");
        CHECK(wstr.front() == L'H');
    }

    SECTION("BasicString<Char>::back") {
        String str("Hello");
        CHECK(str.back() == 'o');

        WString wstr(L"Hello");
        CHECK(wstr.back() == L'o');
    }

    SECTION("BasicString<Char>::resize") {
        String str("Hello");
        str.resize(10);
        CHECK(str.size() == 10);

        WString wstr(L"Hello");
        wstr.resize(10);
        CHECK(wstr.size() == 10);
    }

    SECTION("BasicString<Char>::reserve") {
        String str("Hello");
        str.reserve(10);
        CHECK(str.capacity() >= 10);
    }

    SECTION("BasicString<Char>::push_back") {
        String str;
        str.push_back('H');
        str.push_back('i');
        CHECK(str.size() == 2);
        // CHECK(std::strcmp(str.data(), "Hi") == 0);
        CHECK(std::strcmp(str.c_str(), "Hi") == 0);
    }

    SECTION("BasicString<Char>::pop_back") {
        String str("Hello");
        str.erase(0); // Remove 'H'
        CHECK(str.size() == 4);
        CHECK(str == "ello");

        WString wstr(L"Hello");
        wstr.erase(0); // Remove 'H'
        CHECK(wstr.size() == 4);
        CHECK(wstr == L"ello");
    }

    SECTION("BasicString<Char>::c_str") {
        String str("Hello");
        CHECK(std::strcmp(str.c_str(), "Hello") == 0);

        WString wstr(L"Hello");
        CHECK(std::wcscmp(wstr.c_str(), L"Hello") == 0);
    }

    SECTION("BasicString<Char>::operator+=") {
        String str("Hello");
        str += " World";
        CHECK(str == "Hello World");

        WString wstr(L"Hello");
        wstr += L" World";
        CHECK(wstr == L"Hello World");
    }

    SECTION("BasicString<Char>::append(const Char*)") {
        String str("Hello");
        str.append(" World");
        CHECK(str == "Hello World");

        WString wstr(L"Hello");
        wstr.append(L" World");
        CHECK(wstr == L"Hello World");
    }

    SECTION("BasicString<Char>::append(size_t, Char)") {
        String str("Hello");
        str.append(3, '!');
        CHECK(str == "Hello!!!");

        WString wstr(L"Hello");
        wstr.append(3, L'!');
        CHECK(wstr == L"Hello!!!");
    }

    SECTION("BasicString<Char>::assign") {
        String str("Hello");
        str.assign("World");
        CHECK(str == "World");

        WString wstr(L"Hello");
        wstr.assign(L"World");
        CHECK(wstr == L"World");
    }

    SECTION("BasicString<Char>::insert") {
        String str("Hello");
        str.insert(5, " World!!!", 6);
        CHECK(str == "Hello World");

        WString wstr(L"Hello");
        wstr.insert(5, L" World!!!", 6);
        CHECK(wstr == L"Hello World");
    }

    SECTION("BasicString<Char>::substr") {
        String str("Hello World");
        String sub = str.substr(6, 5);
        CHECK(sub == "World");
        sub = str.substr(0, 5);
        CHECK(sub == "Hello");
        sub = str.substr(6);
        CHECK(sub == "World");

        WString wstr(L"Hello World");
        WString wsub = wstr.substr(6, 5);
        CHECK(wsub == L"World");
        wsub = wstr.substr(0, 5);
        CHECK(wsub == L"Hello");
        wsub = wstr.substr(6);
        CHECK(wsub == L"World");
    }

    SECTION("BasicString<Char>::copy") {
        String str("Hello");
        char buffer[10];
        str.copy(buffer, 5);
        CHECK(std::strncmp(buffer, "Hello", 5) == 0);

        WString wstr(L"Hello");
        wchar_t wbuffer[10];
        wstr.copy(wbuffer, 5);
        CHECK(std::wcsncmp(wbuffer, L"Hello", 5) == 0);
    }

    SECTION("BasicString<Char>::compare(const String& other)") {
        String str1("Hello");
        String str2("Hello");
        String str3("World");
        CHECK(str1.compare(str2) == 0);
        CHECK(str1.compare(str3) < 0);
        CHECK(str3.compare(str1) > 0);

        WString wstr1(L"Hello");
        WString wstr2(L"Hello");
        WString wstr3(L"World");
        CHECK(wstr1.compare(wstr2) == 0);
        CHECK(wstr1.compare(wstr3) < 0);
        CHECK(wstr3.compare(wstr1) > 0);
    }

    SECTION("BasicString<Char>::compare(size_t pos, size_t count, const String& other)") {
        String str1("Hello");
        String str2("Hello");
        String str3("World");
        CHECK(str1.compare(0, 5, str2) == 0);
        CHECK(str1.compare(0, 5, str3) < 0);
        CHECK(str3.compare(0, 5, str1) > 0);

        WString wstr1(L"Hello");
        WString wstr2(L"Hello");
        WString wstr3(L"World");
        CHECK(wstr1.compare(0, 5, wstr2) == 0);
        CHECK(wstr1.compare(0, 5, wstr3) < 0);
        CHECK(wstr3.compare(0, 5, wstr1) > 0);
    }

    SECTION("Equality with C-Style String") {
        String str("Hello");
        CHECK(str == "Hello");
        CHECK_FALSE(str == "Hi");

        WString wstr(L"Hello");
        CHECK(wstr == L"Hello");
        CHECK_FALSE(wstr == L"Hi");
    }
}

TEST_CASE("BasicStringView<Char>", "[BasicStringView<Char>]") {
    SECTION("Default Construction") {
        StringView view;
        CHECK(view.empty());
        CHECK(view.size() == 0);

        WStringView wview;
        CHECK(wview.empty());
        CHECK(wview.size() == 0);
    }

    SECTION("Construction from C-Style String Literal") {
        const char* str = "Hello, World!";
        StringView view(str);
        CHECK(view.size() == strlen(str));
        CHECK(std::strncmp(view.data(), str, view.size()) == 0);

        const wchar_t* wstr = L"Hello, World!";
        WStringView wview(wstr);
        CHECK(wview.size() == std::wcslen(wstr));
        CHECK(std::wcsncmp(wview.data(), wstr, wview.size()) == 0);
    }

    SECTION("Copy Construction") {
        StringView original("Hello");
        StringView copy = original;
        CHECK(copy == original);

        WStringView woriginal(L"Hello");
        WStringView wcopy = woriginal;
        CHECK(wcopy == woriginal);
    }

    SECTION("BasicStringView<Char>::operator[]") {
        StringView view("Hello");
        CHECK(view[0] == 'H');
        CHECK(view[4] == 'o');

        WStringView wview(L"Hello");
        CHECK(wview[0] == L'H');
        CHECK(wview[4] == L'o');
    }

    SECTION("BasicStringView<Char>::at") {
        StringView view1("Hello");
        StringView view2("Hello");
        StringView view3("World");
        CHECK(view1 == view2);
        CHECK_FALSE(view1 == view3);

        WStringView wview1(L"Hello");
        WStringView wview2(L"Hello");
        WStringView wview3(L"World");
        CHECK(wview1 == wview2);
        CHECK_FALSE(wview1 == wview3);
    }

    SECTION("BasicStringView<Char>::operator==") {
        StringView view("Hello");
        CHECK(view == "Hello");
        CHECK_FALSE(view == "World");

        WStringView wview(L"Hello");
        CHECK(wview == L"Hello");
        CHECK_FALSE(wview == L"World");
    }

    // ! StringView does not have a c_str method
    // SECTION("Conversion to CString") {
    //     StringView view("Hello");
    //     CHECK(std::strcmp(view.c_str(), "Hello") == 0);
    // }

    SECTION("BasicStringView<Char>::begin and end") {
        StringView view("Hello");
        CHECK(*view.begin() == 'H');
        CHECK(*(view.end() - 1) == 'o');

        WStringView wview(L"Hello");
        CHECK(*wview.begin() == L'H');
        CHECK(*(wview.end() - 1) == L'o');
    }
}

TEST_CASE("Strings are joined with a delimiter", "[string]") {
    SECTION("Joining empty list returns an empty string") {
        Vector<String> empty_strings;
        String delimiter(",");
        String result = intercalate(delimiter, empty_strings);
        CHECK(result.size() == 0);
    }

    SECTION("Joining with an empty delimiter") {
        Vector<String> strings = {String("Hello"), String("World")};
        String delimiter("");
        String result = intercalate(delimiter, strings);
        CHECK(result == String("HelloWorld"));
    }

    SECTION("Joining with a space delimiter") {
        Vector<String> strings = {String("Hello"), String("World")};
        String delimiter(" ");
        String result = intercalate(delimiter, strings);
        CHECK(result == String("Hello World"));
    }

    SECTION("Joining with a comma delimiter") {
        Vector<String> strings = {String("apple"), String("banana"), String("cherry")};
        String delimiter(", ");
        String result = intercalate(delimiter, strings);
        CHECK(result == String("apple, banana, cherry"));
    }

    SECTION("Joining single element should not append delimiter") {
        Vector<String> strings = {String("Hello")};
        String delimiter(", ");
        String result = intercalate(delimiter, strings);
        CHECK(result == String("Hello"));
    }
}

#endif