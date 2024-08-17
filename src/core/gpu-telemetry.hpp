#ifndef SRC_CORE_GPU_TELEMETRY_HPP_
#define SRC_CORE_GPU_TELEMETRY_HPP_

#include "src/core/json.hpp"

namespace GpuTelemetry {

extern void generateReport(JsonWriter &jw);
extern string generateReport();

} // namespace GpuTelemetry

#endif /* SRC_CORE_GPU_TELEMETRY_HPP_ */
