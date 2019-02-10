//
//
//

#ifndef PEDIGREE_INFO_BASICDEPENDENCEINFO_HPP
#define PEDIGREE_INFO_BASICDEPENDENCEINFO_HPP

#include "Pedigree/Config.hpp"

#include "EdgeInfoDOTTraits.hpp"

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "boost/operators.hpp"
// using boost::orable
// using boost::andable

#include <sstream>
// using std::stringstream

#include <string>
// using std::string

#include <iomanip>
// using std::setprecision

#include <algorithm>
// using std::copy

#include <iterator>
// using std::ostream_iterator

#include <bitset>
// using std::bitset

namespace pedigree {

enum DependenceOrigin : unsigned {
  DO_Unknown = 0,
  DO_Data,
  DO_Memory,
  DO_Control,
  DO_COUNT
};

// also known as load-store classification (see OCMA book)
enum DependenceHazard : unsigned {
  DH_Unknown = 0,
  DH_Flow,
  DH_Anti,
  DH_Out,
  DH_COUNT
};

struct BasicDependenceInfo {
  struct value_type : boost::orable<value_type>, boost::andable<value_type> {
    std::bitset<DO_COUNT> origins;
    llvm::SmallVector<std::bitset<DH_COUNT>, DO_COUNT> hazards{};

  private:
    void reset() {
      origins.reset();
      for (auto &e : hazards) {
        e.reset();
      }

      origins[DO_Unknown] = true;
      hazards[DO_Unknown][DH_Unknown] = true;
    }

    bool isValid() const {
      if (!origins.count()) {
        return false;
      }

      if (origins[DO_Unknown]) {
        if (origins.count() > 1) {
          return false;
        }

        if (hazards[DO_Unknown].count() > 1) {
          return false;
        }

        if (!hazards[DO_Unknown][DH_Unknown]) {
          return false;
        }
      }

      bool status = true;

      if (origins[DO_Control]) {
        if (!hazards[DO_Control][DH_Unknown]) {
          status |= false;
        }

        if (hazards[DO_Control].count() > 1) {
          status |= false;
        }
      }

      if (origins[DO_Data] && hazards[DO_Data][DH_Unknown]) {
        status |= false;
      }

      if (origins[DO_Memory] && hazards[DO_Memory][DH_Unknown]) {
        status |= false;
      }

      return status;
    }

  public:
    value_type() {
      for (auto i = 0u; i < DO_COUNT; ++i) {
        hazards.push_back({});
      }
      reset();
    }

    value_type(DependenceOrigin O, DependenceHazard H) : value_type() {
      origins[DO_Unknown] = false;
      hazards[DH_Unknown].reset();

      origins[O] = true;
      hazards[O][H] = true;

      assert(isValid() && "Dependence info is not valid!");
    }

    value_type(const value_type &) = default;

    value_type &operator|=(const value_type &Other) {
      if (Other.origins[DO_Unknown]) {
        ;
      } else if (origins[DO_Unknown]) {
        origins = Other.origins;
        hazards = Other.hazards;
      } else {
        origins |= Other.origins;

        for (auto i = 0u; i < hazards.size(); ++i) {
          hazards[i] |= Other.hazards[i];
        }
      }

      assert(isValid() && "Dependence info is not valid!");

      return *this;
    }

    value_type &operator&=(const value_type &Other) {
      if (Other.origins[DO_Unknown]) {
        ;
      } else if (origins[DO_Unknown]) {
        origins = Other.origins;
        hazards = Other.hazards;
      } else {
        origins &= Other.origins;

        for (auto i = 0u; i < hazards.size(); ++i) {
          hazards[i] &= Other.hazards[i];
        }
      }

      if (origins.none()) {
        reset();
      }

      assert(isValid() && "Dependence info is not valid!");

      return *this;
    }

    explicit operator bool() const { return !origins[DO_Unknown]; }

    bool has(DependenceOrigin O) const { return origins[O]; }

    bool has(DependenceHazard H) const {
      for (auto i = 0u; i < origins.size(); ++i) {
        if (origins[i] && hazards[i][H]) {
          return true;
        }
      }

      return false;
    }

    bool has(DependenceOrigin O, DependenceHazard H) const {
      return has(O) && has(H);
    }
  };
};

// traits

template <> struct EdgeInfoDOTTraits<BasicDependenceInfo::value_type> {
  static std::string toDOTAttributes(const BasicDependenceInfo::value_type &I) {
    auto attr = toDOTColor(I);

    if (I.origins[DO_Memory]) {
      attr += " " + toDOTLabel(I);
    }

    return attr;
  }

  static std::string toDOTColor(const BasicDependenceInfo::value_type &I) {
    std::stringstream colorAttribute{};
    std::stringstream sep{};
    llvm::SmallVector<std::string, 4> colors{};

    colorAttribute << "color=\"";

    if (I.origins.none()) {
      colors.emplace_back("grey");
    } else {
      auto ratio = 1.0 / DO_COUNT;
      sep << std::setprecision(2) << ";" << ratio << ":";

      if (I.origins[DO_Control]) {
        colors.emplace_back("red");
      }

      if (I.origins[DO_Memory]) {
        colors.emplace_back("purple");
      }

      if (I.origins[DO_Data]) {
        colors.emplace_back("blue");
      }
    }

    std::copy(
        colors.begin(), colors.end(),
        std::ostream_iterator<std::string>(colorAttribute, sep.str().c_str()));

    colorAttribute << "\"";

    return colorAttribute.str();
  }

  static std::string toDOTLabel(const BasicDependenceInfo::value_type &I) {
    std::string label{"label=\""};

    if (I) {
      label = "label=\"U\"";
    } else {
      if (I.has(DH_Flow)) {
        label += "F";
      }

      if (I.has(DH_Anti)) {
        label += "A";
      }

      if (I.has(DH_Out)) {
        label += "O";
      }

      label += "\"";
    }

    return label;
  }
};

} // namespace pedigree

#endif // header
