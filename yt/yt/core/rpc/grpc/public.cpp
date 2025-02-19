#include "public.h"

namespace NYT::NRpc::NGrpc {

////////////////////////////////////////////////////////////////////////////////


const char* const TracingTraceIdMetadataKey = "yt-tracing-trace-id";
const char* const TracingSpanIdMetadataKey = "yt-tracing-span-id";
const char* const TracingSampledMetadataKey = "yt-tracing-sampled";
const char* const TracingDebugMetadataKey = "yt-tracing-debug";

const char* const RequestIdMetadataKey = "yt-request-id";
const char* const UserMetadataKey = "yt-user";
const char* const UserTagMetadataKey = "yt-user-tag";
const char* const UserAgentMetadataKey = "user-agent";
const char* const AuthTokenMetadataKey = "yt-auth-token";
const char* const AuthSessionIdMetadataKey = "yt-auth-session-id";
const char* const AuthSslSessionIdMetadataKey = "yt-auth-ssl-session-id";
const char* const AuthUserTicketMetadataKey = "yt-auth-user-ticket";
const char* const AuthServiceTicketMetadataKey = "yt-auth-service-ticket";
const char* const ErrorMetadataKey = "yt-error-bin";
const char* const MessageBodySizeMetadataKey = "yt-message-body-size";
const char* const ProtocolVersionMetadataKey = "yt-protocol-version";

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT::NRpc::NGrpc
