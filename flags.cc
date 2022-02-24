#include "flags.h"

#include "flagalias.h"
#include <fstream>

namespace flags = GFLAGS_NAMESPACE;

namespace gflags {
namespace ext {

Flag& Flag::instance() {
    static Flag f;
    return f;
}

int Flag::create(const std::string& name, const FlagDetail& flag) {
    instance().flags_.insert(std::make_pair(name, flag));
    return 0;
}

int Flag::createAlias(const std::string& alias, const FlagDetail& flag) {
    instance().aliases_.insert(std::make_pair(alias, flag));
    return 0;
}

bool Flag::getDefaultValue(const std::string& name, std::string& value) {
    flags::CommandLineFlagInfo info;
    if (!flags::GetCommandLineFlagInfo(name.c_str(), &info)) {
        return false;
    }

    value = info.default_value;
    return true;
}

bool Flag::isDefault(const std::string& name) {
    flags::CommandLineFlagInfo info;
    if (!flags::GetCommandLineFlagInfo(name.c_str(), &info)) {
        return false;
    }

    return info.is_default;
}

std::string Flag::getValue(const std::string& name) {
    const auto& custom      = instance().custom_;
    auto        custom_flag = custom.find(name);
    if (custom_flag != custom.end()) {
        return custom_flag->second;
    }

    std::string current_value;
    auto        found = flags::GetCommandLineOption(name.c_str(), &current_value);

    return current_value;
}

long int Flag::getInt32Value(const std::string& name) {
    return strtol(name.c_str(), nullptr, 10);
}

std::string Flag::getType(const std::string& name) {
    flags::CommandLineFlagInfo info;
    if (!flags::GetCommandLineFlagInfo(name.c_str(), &info)) {
        return "";
    }
    return info.type;
}

std::string Flag::getDescription(const std::string& name) {
    if (instance().flags_.count(name)) {
        return instance().flags_.at(name).description;
    }

    if (instance().aliases_.count(name)) {
        return getDescription(instance().aliases_.at(name).description);
    }
    return "";
}

bool Flag::updateValue(const std::string& name, const std::string& value) {
    if (instance().flags_.count(name) > 0) {
        flags::SetCommandLineOption(name.c_str(), value.c_str());
        return true;
    } else if (instance().aliases_.count(name) > 0) {
        // Updating a flag by an alias name.
        auto& real_name = instance().aliases_.at(name).description;
        flags::SetCommandLineOption(real_name.c_str(), value.c_str());
        return true;
    } else if (name.find("custom_") == 0) {
        instance().custom_[name] = value;
    }
    return false;
}

std::map<std::string, FlagInfo> Flag::flags() {
    std::vector<flags::CommandLineFlagInfo> info;
    flags::GetAllFlags(&info);

    std::map<std::string, FlagInfo> flags;
    for (const auto& flag : info) {
        if (instance().flags_.count(flag.name) == 0) {
            // This flag info was not defined within osquery.
            continue;
        }

        // Set the flag info from the internal info kept by Gflags, except for
        // the stored description. Gflag keeps an "unknown" value if the flag
        // was declared without a definition.
        flags[flag.name] = {
            flag.type, instance().flags_.at(flag.name).description, flag.default_value, flag.current_value,
            instance().flags_.at(flag.name)};
    }
    for (const auto& flag : instance().custom_) {
        flags[flag.first] = {"string", "", "", flag.second, {}};
    }
    return flags;
}

void Flag::printFlags(bool shell, bool external, bool cli) {
    std::vector<flags::CommandLineFlagInfo> info;
    flags::GetAllFlags(&info);
    auto& details = instance().flags_;

    std::map<std::string, const flags::CommandLineFlagInfo*> ordered_info;
    for (const auto& flag : info) {
        ordered_info[flag.name] = &flag;
    }

    // Determine max indent needed for all flag names.
    size_t max = 0;
    for (const auto& flag : details) {
        max = (max > flag.first.size()) ? max : flag.first.size();
    }
    // Additional index for flag values.
    max += 6;

    // Show the Gflags-specific 'flagfile'.
    if (!shell && cli) {
        fprintf(stdout, "    --flagfile PATH");
        fprintf(stdout, "%s", std::string(max - 8 - 5, ' ').c_str());
        fprintf(stdout, "  Line-delimited file of additional flags\n");
    }

    auto& aliases = instance().aliases_;
    for (const auto& flag : ordered_info) {
        if (details.count(flag.second->name) > 0) {
            const auto& detail = details.at(flag.second->name);
            if ((shell && !detail.shell) || (!shell && detail.shell) || (external && !detail.external) ||
                (!external && detail.external) || (cli && !detail.cli) || (!cli && detail.cli) || detail.hidden) {
                continue;
            }
        } else if (aliases.count(flag.second->name) > 0) {
            const auto& alias = aliases.at(flag.second->name);
            // Aliases are only printed if this is an external tool and the alias
            // is external.
            if (!alias.external || !external) {
                continue;
            }
        } else {
            // This flag was not defined as an osquery flag or flag alias.
            continue;
        }

        fprintf(stdout, "    --%s", flag.second->name.c_str());

        int pad = static_cast<int>(max);
        if (flag.second->type != "bool") {
            fprintf(stdout, " VALUE");
            pad -= 6;
        }
        pad -= static_cast<int>(flag.second->name.size());

        if (pad > 0 && pad < 80) {
            // Never pad more than 80 characters.
            fprintf(stdout, "%s", std::string(pad, ' ').c_str());
        }
        fprintf(stdout, "  %s\n", getDescription(flag.second->name).c_str());
    }
}

void Flag::dumpDefaultFlags(std::ofstream& of, bool shell, bool external, bool cli) {
    // open file.
    std::vector<flags::CommandLineFlagInfo> info;
    flags::GetAllFlags(&info);
    auto& details = instance().flags_;

    std::map<std::string, const flags::CommandLineFlagInfo*> ordered_info;
    for (const auto& flag : info) {
        ordered_info[flag.name] = &flag;
    }

    auto& aliases = instance().aliases_;
    for (const auto& flag : ordered_info) {
        if (details.count(flag.second->name) > 0) {
            const auto& detail = details.at(flag.second->name);
            if ((shell && !detail.shell) || (!shell && detail.shell) || (external && !detail.external) || (!external && detail.external) || (cli && !detail.cli) || (!cli && detail.cli) ||
                detail.hidden) {
                continue;
            }
        } else if (aliases.count(flag.second->name) > 0) {
            const auto& alias = aliases.at(flag.second->name);
            // Aliases are only printed if this is an external tool and the alias
            // is external.
            if (!alias.external || !external) {
                continue;
            }
        } else {
            // This flag was not defined as an osquery flag or flag alias.
            continue;
        }

        of << flag.second->name << "=" << flag.second->default_value << std::endl;
    }
}

} // namespace ext
} // namespace gflags
