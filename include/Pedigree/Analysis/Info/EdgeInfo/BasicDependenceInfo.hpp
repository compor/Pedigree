//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "EdgeInfoDOTTraits.hpp"

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

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

#include <cassert>
// using assert

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
  struct value_type : boost::orable<value_type, boost::andable<value_type>> {
  private:
    std::bitset<DO_COUNT> origins;
    llvm::SmallVector<std::bitset<DH_COUNT>, DO_COUNT> hazards{};

    void blank() {
      origins.reset();
      for (auto &e : hazards) {
        e.reset();
      }
    }

    void reset() {
      blank();

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
      blank();

      origins[O] = true;
      hazards[O][H] = true;

      assert(isValid() && "Dependence info is not valid after construction!");
    }

    value_type(const value_type &) = default;

    value_type &operator|=(const value_type &Other) {
      if (Other.origins[DO_Unknown]) {
        ;
      } else if (origins[DO_Unknown]) {
        blank();

        origins = Other.origins;
        hazards = Other.hazards;
      } else {
        origins |= Other.origins;

        for (auto i = 0u; i < hazards.size(); ++i) {
          hazards[i] |= Other.hazards[i];
        }
      }

      assert(isValid() && "Dependence info is not valid after disjunction!");

      return *this;
    }

    value_type &operator&=(const value_type &Other) {
      if (Other.origins[DO_Unknown]) {
        ;
      } else if (origins[DO_Unknown]) {
        blank();

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

      assert(isValid() && "Dependence info is not valid after conjunction!");

      return *this;
    }

    value_type &operator&=(DependenceOrigin O) {
      value_type v;

      if (O && origins[O]) {
        v.blank();
        v.origins[O] = true;
        v.hazards[O] = hazards[O];
      }

      origins = v.origins;
      hazards = v.hazards;

      return *this;
    }

    explicit operator bool() const { return !origins[DO_Unknown]; }

    void reset(DependenceOrigin O) {
      if (O && origins[O]) {
        origins[O] = false;
        hazards[O].reset();
      }
    }

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
  static std::string
  toDOTAttributes(const BasicDependenceInfo::value_type &Info) {
    auto attr = toDOTColor(Info);

    if (Info.has(DO_Memory)) {
      attr += " " + toDOTLabel(Info);
    }

    return attr;
  }

  static std::string toDOTColor(const BasicDependenceInfo::value_type &Info) {
    std::stringstream colorAttribute{};
    std::stringstream sep{};
    llvm::SmallVector<std::string, 4> colors{};

    colorAttribute << "color=\"";

    if (!Info) {
      colors.emplace_back("grey");
    } else {
      auto ratio = 1.0 / DO_COUNT;
      sep << std::setprecision(2) << ";" << ratio << ":";

      if (Info.has(DO_Control)) {
        colors.emplace_back("red");
      }

      if (Info.has(DO_Memory)) {
        colors.emplace_back("purple");
      }

      if (Info.has(DO_Data)) {
        colors.emplace_back("blue");
      }
    }

    std::copy(
        colors.begin(), colors.end(),
        std::ostream_iterator<std::string>(colorAttribute, sep.str().c_str()));

    colorAttribute << "\"";

    return colorAttribute.str();
  }

  static std::string toDOTLabel(const BasicDependenceInfo::value_type &Info) {
    std::string label{"label=\""};

    if (!Info) {
      label = "label=\"U\"";
    } else {
      if (Info.has(DH_Flow)) {
        label += "F";
      }

      if (Info.has(DH_Anti)) {
        label += "A";
      }

      if (Info.has(DH_Out)) {
        label += "O";
      }

      label += "\"";
    }

    return label;
  }
};

} // namespace pedigree

