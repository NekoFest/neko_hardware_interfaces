/*
 * Copyright (C) 2017, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/logging.h>
#include <android-base/macros.h>
#include <gmock/gmock.h>

#undef NAN
#include "hidl_struct_util.h"

using testing::Test;

namespace {
constexpr uint32_t kMacId1 = 1;
constexpr uint32_t kMacId2 = 2;
constexpr uint32_t kIfaceChannel1 = 3;
constexpr uint32_t kIfaceChannel2 = 5;
constexpr char kIfaceName1[] = "wlan0";
constexpr char kIfaceName2[] = "wlan1";
}  // namespace
namespace android {
namespace hardware {
namespace wifi {
namespace V1_6 {
namespace implementation {
using namespace android::hardware::wifi::V1_0;
using ::android::hardware::wifi::V1_6::WifiChannelWidthInMhz;

class HidlStructUtilTest : public Test {};

TEST_F(HidlStructUtilTest, CanConvertLegacyWifiMacInfosToHidlWithOneMac) {
    std::vector<legacy_hal::WifiMacInfo> legacy_mac_infos;
    legacy_hal::WifiMacInfo legacy_mac_info1 = {
            .wlan_mac_id = kMacId1,
            .mac_band = legacy_hal::WLAN_MAC_5_0_BAND | legacy_hal::WLAN_MAC_2_4_BAND};
    legacy_hal::WifiIfaceInfo legacy_iface_info1 = {.name = kIfaceName1, .channel = kIfaceChannel1};
    legacy_hal::WifiIfaceInfo legacy_iface_info2 = {.name = kIfaceName2, .channel = kIfaceChannel2};
    legacy_mac_info1.iface_infos.push_back(legacy_iface_info1);
    legacy_mac_info1.iface_infos.push_back(legacy_iface_info2);
    legacy_mac_infos.push_back(legacy_mac_info1);

    std::vector<V1_4::IWifiChipEventCallback::RadioModeInfo> hidl_radio_mode_infos;
    ASSERT_TRUE(hidl_struct_util::convertLegacyWifiMacInfosToHidl(legacy_mac_infos,
                                                                  &hidl_radio_mode_infos));

    ASSERT_EQ(1u, hidl_radio_mode_infos.size());
    auto hidl_radio_mode_info1 = hidl_radio_mode_infos[0];
    EXPECT_EQ(legacy_mac_info1.wlan_mac_id, hidl_radio_mode_info1.radioId);
    EXPECT_EQ(V1_4::WifiBand::BAND_24GHZ_5GHZ, hidl_radio_mode_info1.bandInfo);
    ASSERT_EQ(2u, hidl_radio_mode_info1.ifaceInfos.size());
    auto hidl_iface_info1 = hidl_radio_mode_info1.ifaceInfos[0];
    EXPECT_EQ(legacy_iface_info1.name, hidl_iface_info1.name);
    EXPECT_EQ(static_cast<uint32_t>(legacy_iface_info1.channel), hidl_iface_info1.channel);
    auto hidl_iface_info2 = hidl_radio_mode_info1.ifaceInfos[1];
    EXPECT_EQ(legacy_iface_info2.name, hidl_iface_info2.name);
    EXPECT_EQ(static_cast<uint32_t>(legacy_iface_info2.channel), hidl_iface_info2.channel);
}

TEST_F(HidlStructUtilTest, CanConvertLegacyWifiMacInfosToHidlWithTwoMac) {
    std::vector<legacy_hal::WifiMacInfo> legacy_mac_infos;
    legacy_hal::WifiMacInfo legacy_mac_info1 = {.wlan_mac_id = kMacId1,
                                                .mac_band = legacy_hal::WLAN_MAC_5_0_BAND};
    legacy_hal::WifiIfaceInfo legacy_iface_info1 = {.name = kIfaceName1, .channel = kIfaceChannel1};
    legacy_hal::WifiMacInfo legacy_mac_info2 = {.wlan_mac_id = kMacId2,
                                                .mac_band = legacy_hal::WLAN_MAC_2_4_BAND};
    legacy_hal::WifiIfaceInfo legacy_iface_info2 = {.name = kIfaceName2, .channel = kIfaceChannel2};
    legacy_mac_info1.iface_infos.push_back(legacy_iface_info1);
    legacy_mac_infos.push_back(legacy_mac_info1);
    legacy_mac_info2.iface_infos.push_back(legacy_iface_info2);
    legacy_mac_infos.push_back(legacy_mac_info2);

    std::vector<V1_4::IWifiChipEventCallback::RadioModeInfo> hidl_radio_mode_infos;
    ASSERT_TRUE(hidl_struct_util::convertLegacyWifiMacInfosToHidl(legacy_mac_infos,
                                                                  &hidl_radio_mode_infos));

    ASSERT_EQ(2u, hidl_radio_mode_infos.size());

    // Find mac info 1.
    const auto hidl_radio_mode_info1 =
            std::find_if(hidl_radio_mode_infos.begin(), hidl_radio_mode_infos.end(),
                         [&legacy_mac_info1](const V1_4::IWifiChipEventCallback::RadioModeInfo& x) {
                             return x.radioId == legacy_mac_info1.wlan_mac_id;
                         });
    ASSERT_NE(hidl_radio_mode_infos.end(), hidl_radio_mode_info1);
    EXPECT_EQ(V1_4::WifiBand::BAND_5GHZ, hidl_radio_mode_info1->bandInfo);
    ASSERT_EQ(1u, hidl_radio_mode_info1->ifaceInfos.size());
    auto hidl_iface_info1 = hidl_radio_mode_info1->ifaceInfos[0];
    EXPECT_EQ(legacy_iface_info1.name, hidl_iface_info1.name);
    EXPECT_EQ(static_cast<uint32_t>(legacy_iface_info1.channel), hidl_iface_info1.channel);

    // Find mac info 2.
    const auto hidl_radio_mode_info2 =
            std::find_if(hidl_radio_mode_infos.begin(), hidl_radio_mode_infos.end(),
                         [&legacy_mac_info2](const V1_4::IWifiChipEventCallback::RadioModeInfo& x) {
                             return x.radioId == legacy_mac_info2.wlan_mac_id;
                         });
    ASSERT_NE(hidl_radio_mode_infos.end(), hidl_radio_mode_info2);
    EXPECT_EQ(V1_4::WifiBand::BAND_24GHZ, hidl_radio_mode_info2->bandInfo);
    ASSERT_EQ(1u, hidl_radio_mode_info2->ifaceInfos.size());
    auto hidl_iface_info2 = hidl_radio_mode_info2->ifaceInfos[0];
    EXPECT_EQ(legacy_iface_info2.name, hidl_iface_info2.name);
    EXPECT_EQ(static_cast<uint32_t>(legacy_iface_info2.channel), hidl_iface_info2.channel);
}

TEST_F(HidlStructUtilTest, canConvertLegacyLinkLayerStatsToHidl) {
    legacy_hal::LinkLayerStats legacy_stats{};
    legacy_stats.radios.push_back(legacy_hal::LinkLayerRadioStats{});
    legacy_stats.radios.push_back(legacy_hal::LinkLayerRadioStats{});
    legacy_stats.peers.push_back(legacy_hal::WifiPeerInfo{});
    legacy_stats.peers.push_back(legacy_hal::WifiPeerInfo{});
    legacy_stats.iface.beacon_rx = rand();
    legacy_stats.iface.rssi_mgmt = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].rx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].tx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].mpdu_lost = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].retries = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_min = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_max = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_avg = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_num_samples = rand();

    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].rx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].tx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].mpdu_lost = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].retries = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_min = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_max = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_avg = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_num_samples = rand();

    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].rx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].tx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].mpdu_lost = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].retries = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_min = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_max = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_avg = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_num_samples = rand();

    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].rx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].tx_mpdu = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].mpdu_lost = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].retries = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_min = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_max = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_avg = rand();
    legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_num_samples = rand();

    legacy_stats.iface.info.time_slicing_duty_cycle_percent = rand();
    legacy_stats.iface.num_peers = 1;

    for (auto& radio : legacy_stats.radios) {
        radio.stats.radio = rand();
        radio.stats.on_time = rand();
        radio.stats.tx_time = rand();
        radio.stats.rx_time = rand();
        radio.stats.on_time_scan = rand();
        radio.stats.on_time_nbd = rand();
        radio.stats.on_time_gscan = rand();
        radio.stats.on_time_roam_scan = rand();
        radio.stats.on_time_pno_scan = rand();
        radio.stats.on_time_hs20 = rand();
        for (int i = 0; i < 4; i++) {
            radio.tx_time_per_levels.push_back(rand());
        }

        legacy_hal::wifi_channel_stat channel_stat1 = {
                .channel = {legacy_hal::WIFI_CHAN_WIDTH_20, 2437, 2437, 0},
                .on_time = 0x1111,
                .cca_busy_time = 0x55,
        };
        legacy_hal::wifi_channel_stat channel_stat2 = {
                .channel = {legacy_hal::WIFI_CHAN_WIDTH_20, 5180, 5180, 0},
                .on_time = 0x2222,
                .cca_busy_time = 0x66,
        };
        radio.channel_stats.push_back(channel_stat1);
        radio.channel_stats.push_back(channel_stat2);
    }

    for (auto& peer : legacy_stats.peers) {
        peer.peer_info.bssload.sta_count = rand();
        peer.peer_info.bssload.chan_util = rand();
        wifi_rate_stat rate_stat1 = {
                .rate = {3, 1, 2, 5, 0, 0},
                .tx_mpdu = 0,
                .rx_mpdu = 1,
                .mpdu_lost = 2,
                .retries = 3,
                .retries_short = 4,
                .retries_long = 5,
        };
        wifi_rate_stat rate_stat2 = {
                .rate = {2, 2, 1, 6, 0, 1},
                .tx_mpdu = 6,
                .rx_mpdu = 7,
                .mpdu_lost = 8,
                .retries = 9,
                .retries_short = 10,
                .retries_long = 11,
        };
        peer.rate_stats.push_back(rate_stat1);
        peer.rate_stats.push_back(rate_stat2);
    }

    V1_6::StaLinkLayerStats converted{};
    hidl_struct_util::convertLegacyLinkLayerStatsToHidl(legacy_stats, &converted);
    EXPECT_EQ(legacy_stats.iface.beacon_rx, converted.iface.V1_0.beaconRx);
    EXPECT_EQ(legacy_stats.iface.rssi_mgmt, converted.iface.V1_0.avgRssiMgmt);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].rx_mpdu,
              converted.iface.V1_0.wmeBePktStats.rxMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].tx_mpdu,
              converted.iface.V1_0.wmeBePktStats.txMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].mpdu_lost,
              converted.iface.V1_0.wmeBePktStats.lostMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].retries,
              converted.iface.V1_0.wmeBePktStats.retries);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_min,
              converted.iface.wmeBeContentionTimeStats.contentionTimeMinInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_max,
              converted.iface.wmeBeContentionTimeStats.contentionTimeMaxInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_time_avg,
              converted.iface.wmeBeContentionTimeStats.contentionTimeAvgInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BE].contention_num_samples,
              converted.iface.wmeBeContentionTimeStats.contentionNumSamples);

    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].rx_mpdu,
              converted.iface.V1_0.wmeBkPktStats.rxMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].tx_mpdu,
              converted.iface.V1_0.wmeBkPktStats.txMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].mpdu_lost,
              converted.iface.V1_0.wmeBkPktStats.lostMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].retries,
              converted.iface.V1_0.wmeBkPktStats.retries);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_min,
              converted.iface.wmeBkContentionTimeStats.contentionTimeMinInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_max,
              converted.iface.wmeBkContentionTimeStats.contentionTimeMaxInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_time_avg,
              converted.iface.wmeBkContentionTimeStats.contentionTimeAvgInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_BK].contention_num_samples,
              converted.iface.wmeBkContentionTimeStats.contentionNumSamples);

    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].rx_mpdu,
              converted.iface.V1_0.wmeViPktStats.rxMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].tx_mpdu,
              converted.iface.V1_0.wmeViPktStats.txMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].mpdu_lost,
              converted.iface.V1_0.wmeViPktStats.lostMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].retries,
              converted.iface.V1_0.wmeViPktStats.retries);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_min,
              converted.iface.wmeViContentionTimeStats.contentionTimeMinInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_max,
              converted.iface.wmeViContentionTimeStats.contentionTimeMaxInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_time_avg,
              converted.iface.wmeViContentionTimeStats.contentionTimeAvgInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VI].contention_num_samples,
              converted.iface.wmeViContentionTimeStats.contentionNumSamples);

    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].rx_mpdu,
              converted.iface.V1_0.wmeVoPktStats.rxMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].tx_mpdu,
              converted.iface.V1_0.wmeVoPktStats.txMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].mpdu_lost,
              converted.iface.V1_0.wmeVoPktStats.lostMpdu);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].retries,
              converted.iface.V1_0.wmeVoPktStats.retries);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_min,
              converted.iface.wmeVoContentionTimeStats.contentionTimeMinInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_max,
              converted.iface.wmeVoContentionTimeStats.contentionTimeMaxInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_time_avg,
              converted.iface.wmeVoContentionTimeStats.contentionTimeAvgInUsec);
    EXPECT_EQ(legacy_stats.iface.ac[legacy_hal::WIFI_AC_VO].contention_num_samples,
              converted.iface.wmeVoContentionTimeStats.contentionNumSamples);

    EXPECT_EQ(legacy_stats.iface.info.time_slicing_duty_cycle_percent,
              converted.iface.timeSliceDutyCycleInPercent);

    EXPECT_EQ(legacy_stats.radios.size(), converted.radios.size());
    for (size_t i = 0; i < legacy_stats.radios.size(); i++) {
        EXPECT_EQ(legacy_stats.radios[i].stats.radio, converted.radios[i].radioId);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time, converted.radios[i].V1_0.onTimeInMs);
        EXPECT_EQ(legacy_stats.radios[i].stats.tx_time, converted.radios[i].V1_0.txTimeInMs);
        EXPECT_EQ(legacy_stats.radios[i].stats.rx_time, converted.radios[i].V1_0.rxTimeInMs);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_scan,
                  converted.radios[i].V1_0.onTimeInMsForScan);
        EXPECT_EQ(legacy_stats.radios[i].tx_time_per_levels.size(),
                  converted.radios[i].V1_0.txTimeInMsPerLevel.size());
        for (size_t j = 0; j < legacy_stats.radios[i].tx_time_per_levels.size(); j++) {
            EXPECT_EQ(legacy_stats.radios[i].tx_time_per_levels[j],
                      converted.radios[i].V1_0.txTimeInMsPerLevel[j]);
        }
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_nbd,
                  converted.radios[i].onTimeInMsForNanScan);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_gscan,
                  converted.radios[i].onTimeInMsForBgScan);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_roam_scan,
                  converted.radios[i].onTimeInMsForRoamScan);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_pno_scan,
                  converted.radios[i].onTimeInMsForPnoScan);
        EXPECT_EQ(legacy_stats.radios[i].stats.on_time_hs20,
                  converted.radios[i].onTimeInMsForHs20Scan);
        EXPECT_EQ(legacy_stats.radios[i].channel_stats.size(),
                  converted.radios[i].channelStats.size());
        for (size_t k = 0; k < legacy_stats.radios[i].channel_stats.size(); k++) {
            auto& legacy_channel_st = legacy_stats.radios[i].channel_stats[k];
            EXPECT_EQ(WifiChannelWidthInMhz::WIDTH_20,
                      converted.radios[i].channelStats[k].channel.width);
            EXPECT_EQ(WifiChannelInMhz(legacy_channel_st.channel.center_freq),
                      converted.radios[i].channelStats[k].channel.centerFreq);
            EXPECT_EQ(WifiChannelInMhz(legacy_channel_st.channel.center_freq0),
                      converted.radios[i].channelStats[k].channel.centerFreq0);
            EXPECT_EQ(WifiChannelInMhz(legacy_channel_st.channel.center_freq1),
                      converted.radios[i].channelStats[k].channel.centerFreq1);
            EXPECT_EQ(legacy_channel_st.cca_busy_time,
                      converted.radios[i].channelStats[k].ccaBusyTimeInMs);
            EXPECT_EQ(legacy_channel_st.on_time, converted.radios[i].channelStats[k].onTimeInMs);
        }
    }

    EXPECT_EQ(legacy_stats.peers.size(), converted.iface.peers.size());
    for (size_t i = 0; i < legacy_stats.peers.size(); i++) {
        EXPECT_EQ(legacy_stats.peers[i].peer_info.bssload.sta_count,
                  converted.iface.peers[i].staCount);
        EXPECT_EQ(legacy_stats.peers[i].peer_info.bssload.chan_util,
                  converted.iface.peers[i].chanUtil);
        for (size_t j = 0; j < legacy_stats.peers[i].rate_stats.size(); j++) {
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].rate.preamble,
                      (uint32_t)converted.iface.peers[i].rateStats[j].rateInfo.preamble);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].rate.nss,
                      (uint32_t)converted.iface.peers[i].rateStats[j].rateInfo.nss);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].rate.bw,
                      (uint32_t)converted.iface.peers[i].rateStats[j].rateInfo.bw);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].rate.rateMcsIdx,
                      converted.iface.peers[i].rateStats[j].rateInfo.rateMcsIdx);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].tx_mpdu,
                      converted.iface.peers[i].rateStats[j].txMpdu);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].rx_mpdu,
                      converted.iface.peers[i].rateStats[j].rxMpdu);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].mpdu_lost,
                      converted.iface.peers[i].rateStats[j].mpduLost);
            EXPECT_EQ(legacy_stats.peers[i].rate_stats[j].retries,
                      converted.iface.peers[i].rateStats[j].retries);
        }
    }
}

TEST_F(HidlStructUtilTest, CanConvertLegacyFeaturesToHidl) {
    using HidlChipCaps = V1_3::IWifiChip::ChipCapabilityMask;

    uint32_t hidle_caps;

    uint32_t legacy_feature_set = WIFI_FEATURE_D2D_RTT | WIFI_FEATURE_SET_LATENCY_MODE;
    uint32_t legacy_logger_feature_set = legacy_hal::WIFI_LOGGER_DRIVER_DUMP_SUPPORTED;

    ASSERT_TRUE(hidl_struct_util::convertLegacyFeaturesToHidlChipCapabilities(
            legacy_feature_set, legacy_logger_feature_set, &hidle_caps));

    EXPECT_EQ(HidlChipCaps::DEBUG_RING_BUFFER_VENDOR_DATA |
                      HidlChipCaps::DEBUG_HOST_WAKE_REASON_STATS |
                      HidlChipCaps::DEBUG_ERROR_ALERTS | HidlChipCaps::D2D_RTT |
                      HidlChipCaps::SET_LATENCY_MODE | HidlChipCaps::DEBUG_MEMORY_DRIVER_DUMP,
              hidle_caps);
}

void insertRadioCombination(legacy_hal::wifi_radio_combination* dst_radio_combination_ptr,
                            int num_radio_configurations,
                            legacy_hal::wifi_radio_configuration* radio_configuration) {
    dst_radio_combination_ptr->num_radio_configurations = num_radio_configurations;
    memcpy(dst_radio_combination_ptr->radio_configurations, radio_configuration,
           num_radio_configurations * sizeof(legacy_hal::wifi_radio_configuration));
}

void verifyRadioCombination(WifiRadioCombination* radioCombination, size_t num_radio_configurations,
                            legacy_hal::wifi_radio_configuration* radio_configuration) {
    EXPECT_EQ(num_radio_configurations, radioCombination->radioConfigurations.size());
    for (size_t i = 0; i < num_radio_configurations; i++) {
        EXPECT_EQ(hidl_struct_util::convertLegacyMacBandToHidlWifiBand(radio_configuration->band),
                  radioCombination->radioConfigurations[i].bandInfo);
        EXPECT_EQ(hidl_struct_util::convertLegacyAntennaConfigurationToHidl(
                          radio_configuration->antenna_cfg),
                  radioCombination->radioConfigurations[i].antennaMode);
        radio_configuration++;
    }
}

TEST_F(HidlStructUtilTest, canConvertLegacyRadioCombinationsMatrixToHidl) {
    legacy_hal::wifi_radio_configuration radio_configurations_array1[] = {
            {.band = legacy_hal::WLAN_MAC_2_4_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_1X1},
    };
    legacy_hal::wifi_radio_configuration radio_configurations_array2[] = {
            {.band = legacy_hal::WLAN_MAC_2_4_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_2X2},
            {.band = legacy_hal::WLAN_MAC_5_0_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_3X3},
    };
    legacy_hal::wifi_radio_configuration radio_configurations_array3[] = {
            {.band = legacy_hal::WLAN_MAC_2_4_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_2X2},
            {.band = legacy_hal::WLAN_MAC_6_0_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_1X1},
            {.band = legacy_hal::WLAN_MAC_5_0_BAND, .antenna_cfg = legacy_hal::WIFI_ANTENNA_4X4},
    };

    int num_radio_configs = 0;
    int num_combinations = 0;
    std::array<char, 256> buffer;
    buffer.fill(0);
    legacy_hal::wifi_radio_combination_matrix* legacy_matrix =
            reinterpret_cast<wifi_radio_combination_matrix*>(buffer.data());
    legacy_hal::wifi_radio_combination* radio_combinations;

    // Prepare a legacy wifi_radio_combination_matrix
    legacy_matrix->num_radio_combinations = 3;
    // Insert first combination
    radio_combinations =
            (legacy_hal::wifi_radio_combination*)((char*)legacy_matrix->radio_combinations);
    insertRadioCombination(
            radio_combinations,
            sizeof(radio_configurations_array1) / sizeof(radio_configurations_array1[0]),
            radio_configurations_array1);
    num_combinations++;
    num_radio_configs +=
            sizeof(radio_configurations_array1) / sizeof(radio_configurations_array1[0]);

    // Insert second combination
    radio_combinations =
            (legacy_hal::wifi_radio_combination*)((char*)legacy_matrix->radio_combinations +
                                                  (num_combinations *
                                                   sizeof(legacy_hal::wifi_radio_combination)) +
                                                  (num_radio_configs *
                                                   sizeof(wifi_radio_configuration)));
    insertRadioCombination(
            radio_combinations,
            sizeof(radio_configurations_array2) / sizeof(radio_configurations_array2[0]),
            radio_configurations_array2);
    num_combinations++;
    num_radio_configs +=
            sizeof(radio_configurations_array2) / sizeof(radio_configurations_array2[0]);

    // Insert third combination
    radio_combinations =
            (legacy_hal::wifi_radio_combination*)((char*)legacy_matrix->radio_combinations +
                                                  (num_combinations *
                                                   sizeof(legacy_hal::wifi_radio_combination)) +
                                                  (num_radio_configs *
                                                   sizeof(wifi_radio_configuration)));
    insertRadioCombination(
            radio_combinations,
            sizeof(radio_configurations_array3) / sizeof(radio_configurations_array3[0]),
            radio_configurations_array3);

    V1_6::WifiRadioCombinationMatrix converted_matrix{};
    hidl_struct_util::convertLegacyRadioCombinationsMatrixToHidl(legacy_matrix, &converted_matrix);

    // Verify the conversion
    EXPECT_EQ(legacy_matrix->num_radio_combinations, converted_matrix.radioCombinations.size());
    verifyRadioCombination(
            &converted_matrix.radioCombinations[0],
            sizeof(radio_configurations_array1) / sizeof(radio_configurations_array1[0]),
            radio_configurations_array1);
    verifyRadioCombination(
            &converted_matrix.radioCombinations[1],
            sizeof(radio_configurations_array2) / sizeof(radio_configurations_array2[0]),
            radio_configurations_array2);
    verifyRadioCombination(
            &converted_matrix.radioCombinations[2],
            sizeof(radio_configurations_array3) / sizeof(radio_configurations_array3[0]),
            radio_configurations_array3);
}
}  // namespace implementation
}  // namespace V1_6
}  // namespace wifi
}  // namespace hardware
}  // namespace android
