#pragma once

#include "public.h"
#include "store_detail.h"

#include <yt/yt/ytlib/chunk_client/public.h>

#include <yt/yt/ytlib/node_tracker_client/public.h>

#include <yt/yt/client/chunk_client/read_limit.h>

#include <yt/yt/client/table_client/unversioned_row.h>
#include <yt/yt/client/table_client/versioned_row.h>

#include <yt/yt/library/xor_filter/xor_filter.h>

namespace NYT::NTabletNode {

////////////////////////////////////////////////////////////////////////////////

class TSortedChunkStore
    : public TChunkStoreBase
    , public TSortedStoreBase
{
public:
    TSortedChunkStore(
        TTabletManagerConfigPtr config,
        TStoreId id,
        NChunkClient::TChunkId chunkId,
        const NChunkClient::TLegacyReadRange& readRange,
        TTimestamp overrideTimestamp,
        TTimestamp maxClipTimestamp,
        TTablet* tablet,
        const NTabletNode::NProto::TAddStoreDescriptor* addStoreDescriptor,
        NChunkClient::IBlockCachePtr blockCache,
        IVersionedChunkMetaManagerPtr chunkMetaManager,
        IBackendChunkReadersHolderPtr backendReadersHolder);

    void Initialize() override;

    // IStore implementation.
    EStoreType GetType() const override;

    TSortedChunkStorePtr AsSortedChunk() override;

    void BuildOrchidYson(NYTree::TFluentMap fluent) override;

    // ISortedStore implementation.
    TLegacyOwningKey GetMinKey() const override;
    TLegacyOwningKey GetUpperBoundKey() const override;
    bool HasNontrivialReadRange() const override;

    NTableClient::IVersionedReaderPtr CreateReader(
        const TTabletSnapshotPtr& tabletSnapshot,
        TSharedRange<NTableClient::TRowRange> bounds,
        TTimestamp timestamp,
        bool produceAllVersions,
        const TColumnFilter& columnFilter,
        const NChunkClient::TClientChunkReadOptions& chunkReadOptions,
        std::optional<EWorkloadCategory> workloadCategory) override;

    NTableClient::IVersionedReaderPtr CreateReader(
        const TTabletSnapshotPtr& tabletSnapshot,
        TSharedRange<TLegacyKey> keys,
        TTimestamp timestamp,
        bool produceAllVersions,
        const TColumnFilter& columnFilter,
        const NChunkClient::TClientChunkReadOptions& chunkReadOptions,
        std::optional<EWorkloadCategory> workloadCategory) override;

    bool CheckRowLocks(
        TUnversionedRow row,
        TLockMask lockMask,
        TWriteContext* context) override;

    void Save(TSaveContext& context) const override;
    void Load(TLoadContext& context) override;

private:
    class TSortedChunkStoreVersionedReader;
    friend class TSortedChunkStoreVersionedReader;

    // Cached for fast retrieval from ChunkMeta_.
    TLegacyOwningKey MinKey_;
    TLegacyOwningKey UpperBoundKey_;

    TSharedRange<NTableClient::TRowRange> ReadRange_;

    const NTableClient::TKeyComparer KeyComparer_;

    YT_DECLARE_SPIN_LOCK(NThreading::TReaderWriterSpinLock, ChunkColumnMappingLock_);
    NTableClient::TChunkColumnMappingPtr ChunkColumnMapping_;

    TTimestamp MaxClipTimestamp_ = NullTimestamp;

    TSharedRange<TLegacyKey> FilterKeysByReadRange(
        TSharedRange<TLegacyKey> keys,
        int* skippedBefore,
        int* skippedAfter) const;

    TSharedRange<NTableClient::TRowRange> FilterRowRangesByReadRange(
        const TSharedRange<NTableClient::TRowRange>& ranges) const;

    NTableClient::IVersionedReaderPtr CreateCacheBasedReader(
        const NTableClient::TChunkStatePtr& chunkState,
        TSharedRange<TLegacyKey> keys,
        TTimestamp timestamp,
        bool produceAllVersions,
        const TColumnFilter& columnFilter,
        const NChunkClient::TClientChunkReadOptions& chunkReadOptions,
        bool enableNewScanReader) const;
    NTableClient::IVersionedReaderPtr CreateCacheBasedReader(
        const NTableClient::TChunkStatePtr& chunkState,
        TSharedRange<NTableClient::TRowRange> bounds,
        TTimestamp timestamp,
        bool produceAllVersions,
        const TColumnFilter& columnFilter,
        const NChunkClient::TClientChunkReadOptions& chunkReadOptions,
        const TSharedRange<NTableClient::TRowRange>& singletonClippingRange,
        bool enableNewScanReader) const;

    NTableClient::IVersionedReaderPtr MaybeWrapWithTimestampResettingAdapter(
        NTableClient::IVersionedReaderPtr underlyingReader) const;

    NTableClient::TChunkColumnMappingPtr GetChunkColumnMapping(
        const NTableClient::TTableSchemaPtr& tableSchema,
        const NTableClient::TTableSchemaPtr& chunkSchema);

    NTableClient::TChunkStatePtr PrepareChunkState(
        NTableClient::TCachedVersionedChunkMetaPtr meta);

    void ValidateBlockSize(
        const TTabletSnapshotPtr& tabletSnapshot,
        const NTableClient::TCachedVersionedChunkMetaPtr& chunkMeta,
        const TWorkloadDescriptor& workloadDescriptor);

    const NTableClient::TKeyComparer& GetKeyComparer() const override;

    ISortedStorePtr GetSortedBackingStore() const;

    struct TKeyFilteringResult
    {
        TSharedRange<TLegacyKey> FilteredKeys;
        std::vector<ui8> MissingKeyMask;
    };

    struct TXorFilterBlockInfo
    {
        int BlockIndex;
        TXorFilter XorFilter;
        TRange<TLegacyKey> Keys;
    };

    TFuture<TKeyFilteringResult> PerformXorKeyFiltering(
        const NTableClient::TCachedVersionedChunkMetaPtr& chunkMeta,
        const NChunkClient::IChunkReaderPtr& chunkReader,
        const NChunkClient::TClientChunkReadOptions& chunkReadOptions,
        TSharedRange<TLegacyKey> keys) const;

    TKeyFilteringResult OnXorKeyFilterBlocksRead(
        NCompression::ECodec codecId,
        std::vector<TXorFilterBlockInfo> blockInfos,
        int chunkKeyColumnCount,
        TSharedRange<TLegacyKey> keys,
        std::vector<NChunkClient::TBlock>&& requestedBlocks) const;
};

DEFINE_REFCOUNTED_TYPE(TSortedChunkStore)

////////////////////////////////////////////////////////////////////////////////

//! Returns the slice of |keys| falling into the half-interval |readRange|
//! and the number of skipped keys at the beginning and at the end.
TSharedRange<TLegacyKey> FilterKeysByReadRange(
    const NTableClient::TRowRange& readRange,
    TSharedRange<TLegacyKey> keys,
    int* skippedBefore,
    int* skippedAfter);

//! Returns the slice of |ranges| having non-empty intersection with the half-interval |readRange|.
TSharedRange<NTableClient::TRowRange> FilterRowRangesByReadRange(
    const NTableClient::TRowRange& readRange,
    const TSharedRange<NTableClient::TRowRange>& ranges);

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT::NTabletNode
