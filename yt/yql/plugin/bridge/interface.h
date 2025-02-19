#pragma once

#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////

// This is a plain C version of an interface described in yt/yql/plugin/plugin.h.
// All strings without separate length field are assumed to be null-terminated.

////////////////////////////////////////////////////////////////////////////////

struct TBridgeYqlPluginOptions
{
    const char* MRJobBinary;
    const char* UdfDirectory;

    struct TBridgeCluster
    {
        const char* Cluster;
        const char* Proxy;
    };
    ssize_t ClusterCount;
    TBridgeCluster* Clusters;
    const char* DefaultCluster;

    const char* OperationAttributes;

    const char* YTTokenPath;

    // TODO(max42): passing C++ objects across shared libraries is incredibly
    // fragile. This is a temporary mean until we come up with something more
    // convenient; get rid of this ASAP.
    using TLogBackendHolder = void;
    TLogBackendHolder* LogBackend;
};

// Opaque type representing a YQL plugin.
using TBridgeYqlPlugin = void;

using TFuncBridgeCreateYqlPlugin = TBridgeYqlPlugin*(const TBridgeYqlPluginOptions* options);
using TFuncBridgeFreeYqlPlugin = void(TBridgeYqlPlugin* plugin);

////////////////////////////////////////////////////////////////////////////////

// TODO(max42): consider making structure an opaque type with accessors a-la
// const char* BridgeGetYsonResult(const TBridgeQueryResult*). This would remove the need
// to manually free string data.
struct TBridgeQueryResult
{
    const char* YsonResult = nullptr;
    ssize_t YsonResultLength = 0;
    const char* Plan = nullptr;
    ssize_t PlanLength = 0;
    const char* Statistics = nullptr;
    ssize_t StatisticsLength = 0;
    const char* TaskInfo = nullptr;
    ssize_t TaskInfoLength = 0;

    const char* YsonError = nullptr;
    ssize_t YsonErrorLength = 0;
};

using TFuncBridgeFreeQueryResult = void(TBridgeQueryResult* result);
using TFuncBridgeRun = TBridgeQueryResult*(TBridgeYqlPlugin* plugin, const char* impersonationUser, const char* queryText, const char* settings);

////////////////////////////////////////////////////////////////////////////////

#define FOR_EACH_BRIDGE_INTERFACE_FUNCTION(XX) \
    XX(BridgeCreateYqlPlugin) \
    XX(BridgeFreeYqlPlugin) \
    XX(BridgeFreeQueryResult) \
    XX(BridgeRun)

////////////////////////////////////////////////////////////////////////////////
