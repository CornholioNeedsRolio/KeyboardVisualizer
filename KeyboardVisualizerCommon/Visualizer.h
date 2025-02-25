/*---------------------------------------------------------*\
|  Definitions for Keyboard Visualizer                      |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/11/2016       |
\*---------------------------------------------------------*/

#ifndef VISUALIZER_H
#define VISUALIZER_H

/*---------------------------------------------------------*\
| C/C++ Standard Library Includes                           |
\*---------------------------------------------------------*/
#include <math.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <thread>

/*---------------------------------------------------------*\
| Project Includes                                          |
\*---------------------------------------------------------*/
#include "VisualizerDefines.h"
#include "chuck_fft.h"
#include "hsv.h"
#include "net_port.h"

/*---------------------------------------------------------*\
| OpenRGB SDK Includes                                      |
\*---------------------------------------------------------*/
#include "OpenRGB.h"
#include "NetworkClient.h"
#include "RGBController.h"

/*---------------------------------------------------------*\
| Audio Library Includes                                    |
\*---------------------------------------------------------*/
#ifdef _WIN32
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#endif

#ifdef __linux__
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

typedef struct
{
    int amplitude;
    int avg_mode;
    int avg_size;
    int window_mode;
    int decay;
    int delay;
    float anim_speed;
    int bkgd_bright;
    int bkgd_mode;
    int single_color_mode;
    float nrml_ofst;
    float nrml_scl;
    float filter_constant;
    int frgd_mode;
    bool reactive_bkgd;
    bool silent_bkgd;
    bool start_from_bottom;
    bool start_from_bot_inv;
    unsigned int background_timeout;
} settings_pkt_type;

typedef int net_mode;
enum
{
    NET_MODE_DISABLED,
    NET_MODE_CLIENT,
    NET_MODE_SERVER
};

typedef struct
{
    int                 x_count;
    int                 y_count;
    int *               x_index;
    int *               y_index;
} ZoneIndexType;

typedef struct
{
    RGBController *             controller_ptr;
    bool                        enabled;
    std::vector<ZoneIndexType>  zones;
} ControllerSettingsType;

typedef struct
{
    NetworkClient *                         client_ptr;
    std::vector<ControllerSettingsType *>   controller_settings;
} ClientSettingsType;

class Visualizer
{
public:
    Visualizer();

    //Initializes the visualizer
    void Initialize();

    //Initialize the audio devices list
    void InitAudioDeviceList();

    //Function to start thread
    void StartThread();

    //Thread Functions
    void LEDUpdateThreadFunction();
    void NetConnectThreadFunction();
    void NetUpdateThreadFunction();
    void VisThreadFunction();

    //Connect to OpenRGB
    NetworkClient *OpenRGBConnect(const char *ip, unsigned short port);

    //Disconnect from OpenRGB
    void OpenRGBDisconnect(NetworkClient * client);

    //Called when settings changed
    void OnSettingsChanged();

    //Send Settings to network clients
    void SendSettings();

    //Initialize Server
    void InitServer(char * serverstring);

    //Initialize CLient
    void InitClient(char * clientstring);

    //Change Audio Device
    void ChangeAudioDevice();

    //Update function
    void Update();

    //Draw Pattern
    void DrawPattern(VISUALIZER_PATTERN pattern, int bright, vis_pixels *pixels);

    //Shut Down
    void Shutdown();

    //Save Settings File
    void SaveSettings();

    //Compute normalization line
    void SetNormalization(float offset, float scale);

    //Calculated FFT
    float fft[256];

    //Amplitude of input waveform
    int amplitude;
    int avg_mode;
    int avg_size;
    int window_mode;
    int decay;
    int delay;
    unsigned int audio_device_idx;

    //Flag to update UI
    bool update_ui;

    //Visualizer Background
    vis_pixels pixels_bg;

    //Visualizer Foreground
    vis_pixels pixels_fg;

    //Visualizer Image 1
    vis_pixels pixels_vs1;

    //Visualizer Image 2
    vis_pixels pixels_vs2;

    //Visualizer Output Image Pointer
    vis_pixels *pixels_out;

    //Visualizer Render Image Pointer
    vis_pixels *pixels_render;

    //Background Variables
    float anim_speed;
    int bkgd_bright;
    int bkgd_mode;
    bool reactive_bkgd;
    bool silent_bkgd;
    bool start_from_bottom;
    bool start_from_bot_inv;
    unsigned int background_timeout;
    unsigned int background_timer;

    //Single Color Mode
    int single_color_mode;

    //Normalization Offset and Scale
    float nrml_ofst;
    float nrml_scl;

    //Filter Constant
    float filter_constant;

    //Foreground Variables
    int frgd_mode;

    //Audio Device List
    std::vector<char *> audio_devices;

    //OpenRGB SDK
    std::vector<NetworkClient*> rgb_clients;
    std::vector<RGBController*> rgb_controllers;

    std::vector<ClientSettingsType*> rgb_client_settings;

    void UpdateClientSettings();
    void RegisterClientInfoChangeCallback(NetClientCallback new_callback, void * new_callback_arg);
    void ClientInfoChanged();

private:
#ifdef WIN32
    //WASAPI objects if building for Windows
    IMMDeviceEnumerator *pMMDeviceEnumerator;
    std::vector<IMMDevice *> pMMDevices;
    std::vector<bool> isCapture;
    IMMDeviceCollection *pMMDeviceCollection;
    IAudioClient *pAudioClient;
    IAudioCaptureClient *pAudioCaptureClient;
    WAVEFORMATEX *waveformat;
#else
    //Audio Device Pointer
    ALCdevice *device;
#endif

    //Threads
    std::thread * LEDUpdateThread;
    std::thread * NetConnectThread;
    std::thread * NetUpdateThread;
    std::thread * VisThread;

    //Background Step
    float bkgd_step;

    //Network Port Pointer
    net_port * port;

    //Network Mode
    net_mode netmode;

    //FFT Variables
    float win_hanning[256];
    float win_hamming[256];
    float win_blackman[256];
    float fft_tmp[512];

    //Settings Changed Flag
    bool settings_changed;

    //Application Running Flag
    bool running;

    //Shutting Down Flag
    bool shutdown_flag;

    //Audio Sample Buffer
    unsigned char buffer[256];

    void DrawSingleColorForeground(float amplitude, vis_pixels *fg_pixels, vis_pixels *out_pixels);

    std::mutex                          ClientInfoChangeMutex;
    std::vector<NetClientCallback>      ClientInfoChangeCallbacks;
    std::vector<void *>                 ClientInfoChangeCallbackArgs;
};

#endif
