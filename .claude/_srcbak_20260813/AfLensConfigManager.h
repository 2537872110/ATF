#ifndef AF_LENS_CONFIG_MANAGER_H
#define AF_LENS_CONFIG_MANAGER_H

#include "LensParameterStorage_CPP14.h"

#include <string>
#include <vector>

// C++14 facade used by AF_Main.cpp. All persistent values are stored in the
// encrypted lens_parameters.json file managed by LensParameterStorage.
class AfLensConfigManager
{
private:
    LensParameterStorage storage_;
    std::string currentLens_;
    LensParameter currentParam_;
    std::string lastError_;

private:
    void SetError(const std::string& msg);
    bool LoadLensFromJson(const std::string& lensName, LensParameter& outParam);

public:
    explicit AfLensConfigManager(const std::string& jsonPath = "lens_parameters.json");
    bool AddObjective(
        const std::string& objectiveName,
        int step,
        int lightIntensity,
        int exposureTime,
        int focusDoneRange,
        std::string& errorMessage);
    // Objective CRUD. None of these methods silently creates an objective.
    bool SelectObjective(const std::string& lensName);
    bool ObjectiveExists(const std::string& lensName);
    bool GetObjective(const std::string& lensName, LensParameter& outParam);
    bool DeleteObjective(const std::string& lensName);
    bool RenameObjective(const std::string& oldName, const std::string& newName);
    bool UpdateObjective(const std::string& lensName, const LensParameter& updateParam);

    const std::string& CurrentLens() const;
    const std::string& GetCurrentLensName() const; // AF_Main.cpp compatibility alias
    const LensParameter& CurrentParameter() const;
    const std::string& LastError() const;
    bool HasCurrentLens() const;

    std::vector<std::string> GetObjectiveList();
    bool GetObjectiveList(std::vector<std::string>& outNames);

    // Creates defaults only when explicitly requested.
    bool EnsureDefaultObjectives(const std::vector<std::string>& lensNames);

    // LaserPowerConfig.txt replacement.
    bool GetCurrentLaserPower(int& power) const;
    bool GetLaserPower(const std::string& lensName, int& power);
    bool SaveCurrentLaserPower(int power);
    bool SaveLaserPower(const std::string& lensName, int power);

    // step.txt replacement.
    bool GetCurrentStep(int& stepValue) const;
    bool GetStep(const std::string& lensName, int& stepValue);
    bool SaveCurrentStep(int stepValue);
    bool SaveStep(const std::string& lensName, int stepValue);

    // LinearRegression.txt replacement. Vector order: slope, targetCx, maxPos.
    bool GetCurrentFocusData(std::vector<double>& data) const;
    bool GetFocusData(const std::string& lensName, std::vector<double>& data);
    bool GetFocusData(double& slope, double& targetCx, double& maxPos) const;
    bool SaveCurrentFocusData(double slope, double centroidX, double maxPos);
    bool SaveFocusData(const std::string& lensName,
        double slope,
        double centroidX,
        double maxPos);

    // Make0 replacement.
    bool SaveCurrentZeroCentroid(double centroidX);
    bool SaveZeroCentroid(const std::string& lensName, double centroidX);

    // 焦点参考位置（Centroid_X）：与零点（Origin_Centroid_X）分离管理
    bool SaveCurrentReferenceCentroid(double centroidX); // 只存 Centroid_X，不动 Origin
    bool ResetCurrentReferenceCentroid();                // Centroid_X ← Origin_Centroid_X
};

#endif // AF_LENS_CONFIG_MANAGER_H
