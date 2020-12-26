#include "ConstantObj.h"
#include "StringHelpers.h"

typedef _ConstantReader_Constant Constant;

ConstantObj::ConstantObj(std::unordered_map<std::string, Constant> values) : m_values(std::move(values))
{}

int ConstantObj::GetInt(const std::string& name) const
{
    return static_cast<int>(this->GetDouble(name));
}

uint32_t ConstantObj::GetUint(const std::string& name) const
{
    return static_cast<uint32_t>(this->GetDouble(name));
}

bool ConstantObj::GetBool(const std::string& val) const
{
    const auto x = this->FindConstant(val);

    if (x.has_value() && std::holds_alternative<bool>(*x))
    {
        return std::get<bool>(*x);
    }

    return false;
}

double ConstantObj::GetDouble(const std::string& val) const
{
    const auto x = this->FindConstant(val);

    if (x.has_value() && std::holds_alternative<double>(*x))
    {
        return std::get<double>(*x);
    }

    return 0.0;
}

std::string ConstantObj::GetString(const std::string& val) const
{
    const auto x = this->FindConstant(val);

    if (x.has_value() && std::holds_alternative<std::string>(*x))
    {
        return std::get<std::string>(*x);
    }

    return "";
}

std::shared_ptr<const ConstantObj> ConstantObj::GetObj(const std::string& name) const
{
    const auto x = this->FindConstant(name);

    if (x.has_value() && std::holds_alternative<std::shared_ptr<const ConstantObj>>(*x))
    {
        return std::get<std::shared_ptr<const ConstantObj>>(*x);
    }

    return nullptr;
}

std::optional<Constant> ConstantObj::FindConstant(const std::string& name) const
{
    const auto existing = m_values.find(name);
    if (existing != m_values.end())
    {
        return existing->second;
    }

    return std::nullopt;
}

std::vector<std::string> lineSplit(const std::string& str)
{
    constexpr char delimiter = '\n';
    std::vector<std::string> results;
    size_t startIndex = 0;
    size_t delimiterIndex = str.find(delimiter, startIndex);
    while (delimiterIndex != std::string::npos)
    {
        const size_t endIndex = ((delimiterIndex > 0) && (str[delimiterIndex - 1] == '\r')) ? (delimiterIndex - 1) : delimiterIndex;
        results.emplace_back(str.substr(startIndex, endIndex - startIndex));
        startIndex = delimiterIndex + 1;
        delimiterIndex = str.find(delimiter, startIndex);
    }

    results.emplace_back(str.substr(startIndex));
    return results;
}

std::string trimString(const std::string& str)
{
    const auto start = str.find_first_not_of(' ');
    if (start == std::string::npos)
    {
        // Whitespace or empty string
        return str;
    }

    const auto end = str.find_last_not_of(' ');
    return str.substr(start, (end + 1) - start);
}

std::optional<std::pair<std::string, std::string>> splitKV(const std::string& str)
{
    const auto trimmed = trimString(str);
    constexpr char delimiter = ' ';
    const size_t delimiterIndex = trimmed.find(delimiter);
    if (delimiterIndex != std::string::npos)
    {
        const auto value = trimmed.substr(delimiterIndex + 1, trimmed.size() - (delimiterIndex + 1));
        return { { trimmed.substr(0, delimiterIndex), (value) } };
    }

    return std::nullopt;

}

Constant ParseConstant(const std::string& str)
{
    if (stringEqualIgnoreCase(str, "true"))
    {
        return { true };
    }

    if (stringEqualIgnoreCase(str, "false"))
    {
        return { false };
    }

    char* end;
    const double doubleVal = strtod(str.c_str(), &end);
    if (*end == 0 || isspace(*end))
    {
      return { doubleVal };
    }

    // Could parse as anything
    // Interpret as string
    return { std::string(str) };
}

std::optional<std::pair<std::string, Constant>> ParseObject(uint32_t& i, const std::vector<std::string>& lines)
{
    const auto& line = lines[i];
    const auto kv = splitKV(line);
    i++;

    const bool nextLineOpeningBrace = i < lines.size() && lines[i] == "{";

    if (kv.has_value() || nextLineOpeningBrace)
    {
        // Todo strip whitespace in {} checks
        // Warning kv.value() may not be defined
        if (nextLineOpeningBrace || kv->second == "{")
        {
            // Define an object
            std::string objectName;

            if (nextLineOpeningBrace)
            {
                // Skip over this line in next call
                i++;

                objectName = line;
            }
            else
            {
                objectName = kv->first;
            }

            std::unordered_map<std::string, Constant> fields;
            while (i < lines.size() && lines[i] != "}")
            {
                const auto next = ParseObject(i, lines);
                if (next.has_value())
                {
                    fields[next.value().first] = next.value().second;
                }
            }

            auto co = std::make_shared<const ConstantObj>(std::move(fields));
            return std::make_optional(std::make_pair(objectName, std::move(co)));
        }
        else
        {
            // We know kv is defined
            const auto value = ParseConstant(kv->second);
            return std::make_optional(std::make_pair(kv->first, value));
        }
    }

    return std::nullopt;
}

ConstantObj ParseValues(const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, Constant> map;
    uint32_t i = 0;

    while (i < lines.size())
    {
        const auto object = ParseObject(i, lines);
        if (object.has_value())
        {
            map.insert(object.value());
        }
    }

    return ConstantObj(std::move(map));
}

ConstantObj ParseValues(const std::string& str)
{
    const auto lines = lineSplit(str);
    return ParseValues(lines);
}
