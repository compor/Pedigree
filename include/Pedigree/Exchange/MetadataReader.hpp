//
//
//

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/Metadata.h"
// using llvm::Metadata

#include "llvm/ADT/Twine.h"
// using llvm::Twine

#include <cassert>
// using assert

#ifndef PEDIGREE_METADATAREADER_HPP
#define PEDIGREE_METADATAREADER_HPP

namespace pedigree {

template <typename T> struct MetadataExchangeTraits {
  static bool read(T &, llvm::Metadata *) { return false; }
};

//

class MetadataAnnotationReader {
public:
  MetadataAnnotationReader() = default;

  template <typename T, typename ET = MetadataExchangeTraits<T>>
  bool read(const llvm::Instruction &CurInstruction, const llvm::Twine &Key,
            T &Out) {
    bool wasRead = false;

    auto *data = CurInstruction.getMetadata(Key.str());
    if (data) {
      ET::read(Out, data);
    }

    return wasRead;
  }

  template <typename T, typename ET = MetadataExchangeTraits<T>>
  bool read(const llvm::Instruction *CurInstruction, const llvm::Twine &Key,
            T &Out) {
    assert(CurInstruction && "Instruction is null!");
    read(*CurInstruction, Key, Out);
  }

  bool has(const llvm::Instruction &CurInstruction, const llvm::Twine &Key) {
    return CurInstruction.getMetadata(Key.str()) != nullptr;
  }

  bool has(const llvm::Instruction *CurInstruction, const llvm::Twine &Key) {
    assert(CurInstruction && "Instruction is null!");
    return has(*CurInstruction, Key);
  }
};

} // namespace pedigree

#endif // header
