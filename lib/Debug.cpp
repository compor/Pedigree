//
//
//

#include "Pedigree/Debug.hpp"

namespace boost {

[[noreturn]] inline void throw_exception(std::exception const &e) {
  std::cerr << e.what() << '\n';

  std::terminate();
}

} // namespace boost

#if PEDIGREE_DEBUG

namespace pedigree {
namespace debug {

bool passDebugFlag = false;
LogLevel passLogLevel = LogLevel::Info;

} // namespace debug
} // namespace pedigree

#endif // PEDIGREE_DEBUG

