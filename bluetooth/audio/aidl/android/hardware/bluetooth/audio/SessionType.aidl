/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.hardware.bluetooth.audio;

@VintfStability
@Backing(type="byte")
enum SessionType {
    UNKNOWN,
    /**
     * A2DP legacy that AVDTP media is encoded by Bluetooth Stack
     */
    A2DP_SOFTWARE_ENCODING_DATAPATH,
    /**
     * The encoding of AVDTP media is done by HW and there is control only
     */
    A2DP_HARDWARE_OFFLOAD_DATAPATH,
    /**
     * Used when encoded by Bluetooth Stack and streaming to Hearing Aid
     */
    HEARING_AID_SOFTWARE_ENCODING_DATAPATH,
    /**
     * Used when encoded by Bluetooth Stack and streaming to LE Audio device
     */
    LE_AUDIO_SOFTWARE_ENCODING_DATAPATH,
    /**
     * Used when decoded by Bluetooth Stack and streaming to audio framework
     */
    LE_AUDIO_SOFTWARE_DECODING_DATAPATH,
    /**
     * Encoding is done by HW an there is control only
     */
    LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH,
    /**
     * Decoding is done by HW an there is control only
     */
    LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH,
}
