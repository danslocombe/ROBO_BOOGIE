#pragma once

#include <variant>
#include <unordered_map>
#include <optional>
#include <memory>

class ConstantObj;

typedef std::variant<bool, double, std::string, std::shared_ptr<const ConstantObj>> _ConstantReader_Constant;

class ConstantObj
{
public:
    ConstantObj(std::unordered_map<std::string, _ConstantReader_Constant> fields);
    ConstantObj() = default;

    bool GetBool(const std::string& name) const;
    int GetInt(const std::string& name) const;
    uint32_t GetUint(const std::string& name) const;
    double GetDouble(const std::string& name) const;
    std::string GetString(const std::string& name) const;
    std::shared_ptr<const ConstantObj> GetObj(const std::string& name) const;

private:
    std::optional<_ConstantReader_Constant> FindConstant(const std::string& name) const;
    std::unordered_map<std::string, _ConstantReader_Constant> m_values;
};

ConstantObj ParseValues(const std::string& str);
ConstantObj ParseValues(const std::vector<std::string>& lines);
