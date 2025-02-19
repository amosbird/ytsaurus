#pragma once

#include <yt/yt/server/master/cell_master/public.h>

#include <yt/yt/server/master/cypress_server/public.h>

#include <yt/yt/server/master/object_server/public.h>

#include <yt/yt/ytlib/cellar_client/public.h>

namespace NYT::NCellServer {

////////////////////////////////////////////////////////////////////////////////

NCypressServer::INodeTypeHandlerPtr CreateAreaMapTypeHandler(
    NCellMaster::TBootstrap* bootstrap);
NCypressServer::INodeTypeHandlerPtr CreateCellNodeTypeHandler(
    NCellMaster::TBootstrap* bootstrap);
NCypressServer::INodeTypeHandlerPtr CreateCellBundleMapTypeHandler(
    NCellMaster::TBootstrap* bootstrap,
    NCellarClient::ECellarType cellType,
    NObjectClient::EObjectType cellBundleMapType);

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT::NCellServer
