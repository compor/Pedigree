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

class MetadataAnnotationReader {
public:
  MetadataAnnotationReader() = default;

  template <typename T>
  bool read(const llvm::Instruction &CurInstruction, const llvm::Twine &Key,
            T &Out) {
    bool wasRead = false;

    auto *data = CurInstruction.getMetadata(Key.str());
    if (data) {
      Out.read(data);
    }

    return wasRead;
  }

  bool has(const llvm::Instruction &CurInstruction, const llvm::Twine &Key) {
    return CurInstruction.getMetadata(Key.str()) != nullptr;
  }
};

} // namespace pedigree

#endif // header
