#include "TypeTable.hpp"

#include <stdexcept>
#include <unordered_set>

namespace sluggish {
namespace {

void check_members(const Type* type, std::unordered_set<const Type*>& visiting,
    std::unordered_set<const Type*>& finished) {

    if (!type->is_struct() || finished.find(type) != finished.end()) {
        return;
    }

    if (!visiting.insert(type).second) {
        throw std::runtime_error("Recursive struct containment: " + type->name());
    }

    for (const auto& member : type->members) {
        check_members(member.type, visiting, finished);
    }

    visiting.erase(type);
    finished.insert(type);

}

} // namespace

void TypeTable::register_struct(const std::string& name) {

    if (name == word_8.name() || name == word_16.name() || structs.find(name) != structs.end()) {
        throw std::runtime_error("Duplicate type: " + name);
    }

    structs.emplace(name, std::make_unique<Type>(name));

}

const Type* TypeTable::resolve(const std::string& name) const {

    const auto type = structs.find(name);

    if (type != structs.end()) {
        return type->second.get();
    }

    return resolve_type(name);

}

void TypeTable::check_recursive_members() const {

    std::unordered_set<const Type*> visiting;
    std::unordered_set<const Type*> finished;

    for (const auto& entry : structs) {
        check_members(entry.second.get(), visiting, finished);
    }

}

} // namespace sluggish
