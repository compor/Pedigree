//
//
//

#include "Pedigree/Debug.hpp"

#ifdef BOOST_NO_EXCEPTIONS

namespace boost {

[[noreturn]] inline void throw_exception(std::exception const &e) {
  std::cerr << e.what() << '\n';

  std::terminate();
}

} // namespace boost

#endif // BOOST_NO_EXCEPTIONS

#if PEDIGREE_DEBUG

//

#endif // PEDIGREE_DEBUG

