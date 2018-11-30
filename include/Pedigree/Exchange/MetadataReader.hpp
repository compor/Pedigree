//
//
//

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/Metadata.h"
// using llvm::Metadata

#include "llvm/ADT/Twine.h"
// using llvm::Twine

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

  bool has(const llvm::Instruction &CurInstruction, const llvm::Twine &Key) {
    return CurInstruction.getMetadata(Key.str()) != nullptr;
  }
};

} // namespace pedigree

#endif // header
