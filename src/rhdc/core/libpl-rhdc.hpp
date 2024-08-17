#ifndef SRC_RHDC_CORE_LIBPL_RHDC_HPP_
#define SRC_RHDC_CORE_LIBPL_RHDC_HPP_

#include "src/polyfill/fifo-pipe.hpp"

extern bool handleRhdcLibplRequest(ubyte commandId, ushort payloadSize,
                                   const ubyte *payload,
                                   const WritableFifoPipe &pipe,
                                   std::error_code &err);

extern void clearRhdcLibplCache();

#endif
