#pragma once

// Copyright 2018 Diamnet Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "crypto/ShortHash.h"
#include "ledger/InternalLedgerEntry.h"
#include "util/HashOfHash.h"
#include "xdr/Diamnet-ledger.h"
#include <functional>

namespace diamnet
{

static PoolID const&
getLiquidityPoolID(Asset const& asset)
{
    throw std::runtime_error("cannot get PoolID from Asset");
}

static PoolID const&
getLiquidityPoolID(TrustLineAsset const& tlAsset)
{
    return tlAsset.liquidityPoolID();
}

static inline void
hashMix(size_t& h, size_t v)
{
    // from https://github.com/ztanml/fast-hash (MIT license)
    v ^= v >> 23;
    v *= 0x2127599bf4325c37ULL;
    v ^= v >> 47;
    h ^= v;
    h *= 0x880355f21e6d1965ULL;
}

template <typename T>
static size_t
getAssetHash(T const& asset)
{
    size_t res = asset.type();

    switch (asset.type())
    {
    case diamnet::ASSET_TYPE_NATIVE:
        break;
    case diamnet::ASSET_TYPE_CREDIT_ALPHANUM4:
    {
        auto& a4 = asset.alphaNum4();
        hashMix(res, std::hash<diamnet::uint256>()(a4.issuer.ed25519()));
        hashMix(res, diamnet::shortHash::computeHash(diamnet::ByteSlice(
                         a4.assetCode.data(), a4.assetCode.size())));
        break;
    }
    case diamnet::ASSET_TYPE_CREDIT_ALPHANUM12:
    {
        auto& a12 = asset.alphaNum12();
        hashMix(res, std::hash<diamnet::uint256>()(a12.issuer.ed25519()));
        hashMix(res, diamnet::shortHash::computeHash(diamnet::ByteSlice(
                         a12.assetCode.data(), a12.assetCode.size())));
        break;
    }
    case diamnet::ASSET_TYPE_POOL_SHARE:
    {
        hashMix(res, std::hash<diamnet::uint256>()(getLiquidityPoolID(asset)));
        break;
    }
    default:
        throw std::runtime_error("unknown Asset type");
    }
    return res;
}

}

// implements a default hasher for "LedgerKey"
namespace std
{
template <> class hash<diamnet::Asset>
{
  public:
    size_t
    operator()(diamnet::Asset const& asset) const
    {
        return diamnet::getAssetHash<diamnet::Asset>(asset);
    }
};

template <> class hash<diamnet::TrustLineAsset>
{
  public:
    size_t
    operator()(diamnet::TrustLineAsset const& asset) const
    {
        return diamnet::getAssetHash<diamnet::TrustLineAsset>(asset);
    }
};

template <> class hash<diamnet::LedgerKey>
{
  public:
    size_t
    operator()(diamnet::LedgerKey const& lk) const
    {
        size_t res = lk.type();
        switch (lk.type())
        {
        case diamnet::ACCOUNT:
            diamnet::hashMix(res, std::hash<diamnet::uint256>()(
                                      lk.account().accountID.ed25519()));
            break;
        case diamnet::TRUSTLINE:
        {
            auto& tl = lk.trustLine();
            diamnet::hashMix(
                res, std::hash<diamnet::uint256>()(tl.accountID.ed25519()));
            diamnet::hashMix(res, hash<diamnet::TrustLineAsset>()(tl.asset));
            break;
        }
        case diamnet::DATA:
            diamnet::hashMix(res, std::hash<diamnet::uint256>()(
                                      lk.data().accountID.ed25519()));
            diamnet::hashMix(
                res,
                diamnet::shortHash::computeHash(diamnet::ByteSlice(
                    lk.data().dataName.data(), lk.data().dataName.size())));
            break;
        case diamnet::OFFER:
            diamnet::hashMix(
                res, diamnet::shortHash::computeHash(diamnet::ByteSlice(
                         &lk.offer().offerID, sizeof(lk.offer().offerID))));
            break;
        case diamnet::CLAIMABLE_BALANCE:
            diamnet::hashMix(res, std::hash<diamnet::uint256>()(
                                      lk.claimableBalance().balanceID.v0()));
            break;
        case diamnet::LIQUIDITY_POOL:
            diamnet::hashMix(res, std::hash<diamnet::uint256>()(
                                      lk.liquidityPool().liquidityPoolID));
            break;
        default:
            abort();
        }
        return res;
    }
};

template <> class hash<diamnet::InternalLedgerKey>
{
  public:
    size_t
    operator()(diamnet::InternalLedgerKey const& glk) const
    {
        return glk.hash();
    }
};
}
