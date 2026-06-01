#ifndef ISH_IDEN_TABLE_H
#define ISH_IDEN_TABLE_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Ishlang {

    using IdenType = uint64_t;

    class IdenTable {
    public:
        IdenType mapName(const std::string & name);

        inline const std::string & getName(IdenType iden) const;

        inline bool exists(const std::string & name) const noexcept;
        inline bool empty() const noexcept;
        inline std::size_t size() const noexcept;
        inline void clear() noexcept;

    private:
        inline IdenType nextIden() noexcept;

    private:
        IdenType nextIden_ = 0;
        std::unordered_map<std::string, IdenType> table_;
        std::unordered_map<IdenType, std::string> reverseTable_;
    };

    // --------------------------------------------------------------------------------
    // INLINE

    inline const std::string & IdenTable::getName(IdenType iden) const {
        static std::string Empty;

        auto iter = reverseTable_.find(iden);
        return iter != reverseTable_.end() ? iter->second : Empty;
    }

    inline bool IdenTable::exists(const std::string & name) const noexcept {
        return table_.contains(name);
    }

    inline bool IdenTable::empty() const noexcept {
        return table_.empty();
    }

    inline std::size_t IdenTable::size() const noexcept {
        return table_.size();
    }

    inline void IdenTable::clear() noexcept {
        table_.clear();
        reverseTable_.clear();
        nextIden_ = 0;
    }

    inline IdenType IdenTable::nextIden() noexcept {
        return ++nextIden_;
    }

}

#endif	// ISH_IDEN_TABLE_H
