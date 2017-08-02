#ifndef INFRA_JSON_FORMATTER_HPP
#define INFRA_JSON_FORMATTER_HPP

#include "infra/stream/StringOutputStream.hpp"
#include "infra/util/BoundedString.hpp"
#include "infra/util/WithStorage.hpp"

namespace infra
{
    class JsonArrayFormatter;

    class JsonObjectFormatter
    {
    public:
        using WithStringStream = infra::WithStorage<JsonObjectFormatter, infra::StringOutputStream>;

        explicit JsonObjectFormatter(infra::TextOutputStream& stream);
        JsonObjectFormatter(const JsonObjectFormatter& other) = delete;
        JsonObjectFormatter(JsonObjectFormatter&& other);
        JsonObjectFormatter& operator=(const JsonObjectFormatter& other) = delete;
        JsonObjectFormatter& operator=(JsonObjectFormatter&& other);
        ~JsonObjectFormatter();

        void Add(const char* tagName, bool tag);
        void Add(const char* tagName, int32_t tag);
        void Add(const char* tagName, uint32_t tag);
        void Add(const char* tagName, int64_t tag);
        void Add(const char* tagName, const char* tag);
        void Add(const char* tagName, infra::BoundedConstString tag);
        void AddMilliFloat(const char* tagName, uint32_t intValue, uint32_t milliFractionalValue);
        void AddSubObject(const char* tagName, infra::BoundedConstString json);
        JsonObjectFormatter SubObject(infra::BoundedConstString tagName);
        JsonArrayFormatter SubArray(infra::BoundedConstString tagName);

        bool Failed() const;

    private:
        void InsertSeparation();

    private:
        infra::TextOutputStream* stream;
        bool empty = true;
    };

    class JsonArrayFormatter
    {
    public:
        using WithStringStream = infra::WithStorage<JsonArrayFormatter, infra::StringOutputStream>;

        explicit JsonArrayFormatter(infra::TextOutputStream& stream);
        JsonArrayFormatter(const JsonArrayFormatter& other) = delete;
        JsonArrayFormatter(JsonArrayFormatter&& other);
        JsonArrayFormatter& operator=(const JsonArrayFormatter& other) = delete;
        JsonArrayFormatter& operator=(JsonArrayFormatter&& other);
        ~JsonArrayFormatter();

        void Add(bool tag);
        void Add(int32_t tag);
        void Add(uint32_t tag);
        void Add(int64_t tag);
        void Add(const char* tag);
        void Add(infra::BoundedConstString tag);
        JsonObjectFormatter SubObject();
        JsonArrayFormatter SubArray();

        bool Failed() const;

    private:
        void InsertSeparation();

    private:
        infra::TextOutputStream* stream;
        bool empty = true;
    };
}

#endif
