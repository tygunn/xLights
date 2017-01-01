#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <list>
#include <string>
#include <wx/thread.h>

class Output;
class Controller;
class TestPreset;

#define NETWORKSFILE "xlights_networks.xml";

class OutputManager
{
    #pragma region Member Variables
    std::string _filename;
    std::list<Output*> _outputs;
    std::list<TestPreset*> _testPresets;
    int _syncUniverse;
    bool _syncEnabled;
    bool _dirty;
    bool _outputting; // true if we are currently sending out data
    wxCriticalSection _outputCriticalSection; // used to protect areas that must be single threaded
    #pragma endregion Member Variables

public:

    #pragma region Constructors and Destructors
    OutputManager();
    ~OutputManager();
    #pragma endregion Constructors and Destructors

    #pragma region Static Functions
    static std::string GetNetworksFileName() { return NETWORKSFILE; }
    #pragma endregion Static Functions

    #pragma region Save and Load
    bool Load(const std::string& showdir, bool syncEnabled = false);
    bool Save();
    #pragma endregion Save and Load

    #pragma region Output Management
    void AddOutput(Output* output, Output* after);
    void AddOutput(Output* output, int pos);
    void DeleteOutput(Output* output);
    void DeleteAllOutputs();
    void MoveOutput(Output* output, int toOutputNumber);
    bool AreAllIPOutputs(std::list<int> outputNumbers);
    std::list<Output*> GetAllOutputs(const std::string& ip) const;
    std::list<Output*> GetAllOutputs(const std::list<int>& outputNumbers) const;
    std::list<Output*> GetOutputs() const { return _outputs; } // returns a list like that on setup tab
    void Replace(Output* replacethis, Output* withthis);
    Output* GetOutput(int outputNumber) const;
    Output* GetOutput(long absoluteChannel, long& startChannel) const;
    Output* GetOutput(int universe, const std::string& ip) const;
    std::list<int> GetIPUniverses(const std::string& ip = "") const;
    int GetOutputCount() const { return _outputs.size(); }
    bool Discover(); // discover controllers and add them to the list if they are not already there
    #pragma endregion Output Management

    void SomethingChanged() const;
    bool IsDirty() const;

    #pragma region Channel Mapping
    long GetTotalChannels() const;
    long GetAbsoluteChannel(int outputNumber, int startChannel = 1) const;
    long GetAbsoluteChannel(const std::string& ip, int universe, int startChannel = 1) const;
    #pragma endregion Channel Mapping

    #pragma region Start and Stop
    bool StartOutput();
    void StopOutput();
    bool IsOutputting() const { return _outputting; }
    #pragma endregion Start and Stop

    #pragma region Frame Handling
    void StartFrame(long msec);
    void EndFrame();
    void ResetFrame();
    #pragma endregion Frame Handling

    #pragma region Packet Sync
    bool IsSyncEnabled() const { return _syncEnabled; }
    void SetSyncEnabled(bool syncEnabled) { _syncEnabled = syncEnabled; }
    int GetSyncUniverse() const { return _syncUniverse; }
    void SetSyncUniverse(int syncUniverse) { _syncUniverse = syncUniverse; }
    bool UseE131() const;
    bool UseArtnet() const;
    #pragma endregion Packet Sync

    #pragma region Data Setting
    void SetOneChannel(long channel, unsigned char data);
    void SetManyChannels(long channel, unsigned char* data, long size);
    void AllOff();
    #pragma endregion Data Setting

    #pragma region Test Presets
    std::list<std::string> GetTestPresets();
    TestPreset* GetTestPreset(std::string preset);
    TestPreset* CreateTestPreset(std::string preset);
    #pragma endregion Test Presets

    std::list<std::string> GetIps() const;
    size_t TxNonEmptyCount();
    bool TxEmpty();
    std::string GetChannelName(long channel);
    void SendHeartbeat();
};

#endif