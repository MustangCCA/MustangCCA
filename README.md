# Mustang
This project is an open-source implementation of the Mustang algorithm proposed in the paper "Mustang: Improving QoE for Real Time Video in Cellular Network by Masking Jitter."

Due to the extensive size of Google's official WebRTC source code, we are only uploading the modified files. Please replace the corresponding files on your own before compiling, keeping the other files consistent with the source.

Below is a brief description of the compilation process for the WebRTC source code:

1. Download Google's Depot Tools.
```
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=/path/to/depot_tools:$PATH
```
2. Start downloading the WebRTC source code.
```
mkdir webrtc-checkout
cd webrtc-checkout
fetch --nohooks webrtc
gclient sync
```
3. Install dependencies on a Linux system.
```
./build/install-build-deps.sh
./build/install-build-deps.sh --no-chromeos-fonts
# Switch to the required branch.
git reset --hard ac0d183
```
4. Clone this project using git, then replace the corresponding files in the 'src' folder with those from the cloned project.
5. Start the compilation.
```
gn gen out/Default
ninja -C out/Default
```
5. How to enable each algorithm

**Mustang:** 

rtp_transport_controller_send.h : #define CCA_GCC

goog_cc_network_control.h:   bool diff_flag = true;   bool ack_flag = true;

send_side_bandwidth_estimation.h:   bool gcc_beta_flag = false;

**GCC:** 

rtp_transport_controller_send.h : #define CCA_GCC

goog_cc_network_control.h:   bool diff_flag = false;   bool ack_flag = false;

send_side_bandwidth_estimation.h:   bool gcc_beta_flag = false;

**GCC-beta:** 

rtp_transport_controller_send.h : #define CCA_GCC

goog_cc_network_control.h:   bool diff_flag = false;   bool ack_flag = false;

send_side_bandwidth_estimation.h:   bool gcc_beta_flag = false;

**PCC:** 

rtp_transport_controller_send.h : #define CCA_PCC