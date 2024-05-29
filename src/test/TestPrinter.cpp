// Copyright 2017 Diamnet Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "test/TestPrinter.h"
#include "catchup/CatchupRange.h"
#include "test/TestMarket.h"
#include "util/XDRCereal.h"
#include <fmt/format.h>

namespace Catch
{
std::string
StringMaker<diamnet::OfferState>::convert(diamnet::OfferState const& os)
{
    return fmt::format("{}, {}, {}, amount: {}, type: {}",
                       xdr_to_string(os.selling, "selling"),
                       xdr_to_string(os.buying, "buying"),
                       xdr_to_string(os.price, "price"), os.amount,
                       os.type == diamnet::OfferType::PASSIVE ? "passive"
                                                              : "active");
}

std::string
StringMaker<diamnet::CatchupRange>::convert(diamnet::CatchupRange const& cr)
{
    return fmt::format("[{},{}), applyBuckets: {}", cr.getReplayFirst(),
                       cr.getReplayLimit(),
                       cr.applyBuckets() ? cr.getBucketApplyLedger() : 0);
}

std::string
StringMaker<diamnet::historytestutils::CatchupPerformedWork>::convert(
    diamnet::historytestutils::CatchupPerformedWork const& cm)
{
    return fmt::format(
        "{}, {}, {}, {}, {}, {}, {}, {}", cm.mHistoryArchiveStatesDownloaded,
        cm.mCheckpointsDownloaded, cm.mLedgersVerified,
        cm.mLedgerChainsVerificationFailed, cm.mBucketsDownloaded,
        cm.mBucketsApplied, cm.mTxSetsDownloaded, cm.mTxSetsApplied);
}
}
