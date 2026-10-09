#ifndef LENS_PARAMETER_STORAGE_CPP14_H
#define LENS_PARAMETER_STORAGE_CPP14_H

// Visual Studio: set C++ language standard to ISO C++14.
// No std::filesystem. No nlohmann/json.hpp.
// AES/SHA256/Base64 uses Windows built-in CryptoAPI, so no third-party library is required.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Crypt32.lib")

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <limits>

class AesEncryption
{
private:
    static const char* OriginalKey()
    {
        return "PangHu604";
    }

    static std::string LastErrorText(const char* where)
    {
        std::ostringstream os;
        os << where << " failed, GetLastError=" << GetLastError();
        return os.str();
    }

    static HCRYPTPROV AcquireProvider()
    {
        HCRYPTPROV hProv = 0;
        if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
        {
            throw std::runtime_error(LastErrorText("CryptAcquireContextA"));
        }
        return hProv;
    }

    static std::vector<BYTE> Sha256(const std::string& text)
    {
        HCRYPTPROV hProv = AcquireProvider();
        HCRYPTHASH hHash = 0;
        std::vector<BYTE> hash(32, 0);

        try
        {
            if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
                throw std::runtime_error(LastErrorText("CryptCreateHash"));

            if (!CryptHashData(hHash,
                reinterpret_cast<const BYTE*>(text.data()),
                static_cast<DWORD>(text.size()),
                0))
                throw std::runtime_error(LastErrorText("CryptHashData"));

            DWORD hashLen = static_cast<DWORD>(hash.size());
            if (!CryptGetHashParam(hHash, HP_HASHVAL, &hash[0], &hashLen, 0))
                throw std::runtime_error(LastErrorText("CryptGetHashParam"));

            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return hash;
        }
        catch (...)
        {
            if (hHash) CryptDestroyHash(hHash);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }

    static HCRYPTKEY ImportAes256Key(HCRYPTPROV hProv, const std::vector<BYTE>& key32)
    {
        if (key32.size() != 32)
            throw std::runtime_error("AES-256 key length must be 32 bytes.");

        struct PlainTextKeyBlob
        {
            BLOBHEADER hdr;
            DWORD keySize;
            BYTE key[32];
        };

        PlainTextKeyBlob blob;
        std::memset(&blob, 0, sizeof(blob));
        blob.hdr.bType = PLAINTEXTKEYBLOB;
        blob.hdr.bVersion = CUR_BLOB_VERSION;
        blob.hdr.reserved = 0;
        blob.hdr.aiKeyAlg = CALG_AES_256;
        blob.keySize = 32;
        std::memcpy(blob.key, &key32[0], 32);

        HCRYPTKEY hKey = 0;
        if (!CryptImportKey(hProv,
            reinterpret_cast<const BYTE*>(&blob),
            sizeof(blob),
            0,
            0,
            &hKey))
            throw std::runtime_error(LastErrorText("CryptImportKey"));

        DWORD mode = CRYPT_MODE_CBC;
        if (!CryptSetKeyParam(hKey, KP_MODE, reinterpret_cast<BYTE*>(&mode), 0))
        {
            CryptDestroyKey(hKey);
            throw std::runtime_error(LastErrorText("CryptSetKeyParam KP_MODE"));
        }

        const char ivText[] = "PangHuIV12345678"; // 16 bytes, same as C# Encoding.UTF8.GetBytes(...)
        BYTE iv[16];
        std::memcpy(iv, ivText, 16);

        if (!CryptSetKeyParam(hKey, KP_IV, iv, 0))
        {
            CryptDestroyKey(hKey);
            throw std::runtime_error(LastErrorText("CryptSetKeyParam KP_IV"));
        }

        return hKey;
    }

    static std::string Base64Encode(const std::vector<BYTE>& data)
    {
        if (data.empty()) return std::string();

        DWORD outLen = 0;
        if (!CryptBinaryToStringA(&data[0],
            static_cast<DWORD>(data.size()),
            CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
            NULL,
            &outLen))
            throw std::runtime_error(LastErrorText("CryptBinaryToStringA length"));

        std::string out(outLen, '\0');
        if (!CryptBinaryToStringA(&data[0],
            static_cast<DWORD>(data.size()),
            CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
            &out[0],
            &outLen))
            throw std::runtime_error(LastErrorText("CryptBinaryToStringA"));

        out.resize(outLen);
        if (!out.empty() && out[out.size() - 1] == '\0')
            out.resize(out.size() - 1);
        return out;
    }

    static std::vector<BYTE> Base64Decode(const std::string& text)
    {
        if (text.empty()) return std::vector<BYTE>();

        DWORD outLen = 0;
        if (!CryptStringToBinaryA(text.c_str(),
            static_cast<DWORD>(text.size()),
            CRYPT_STRING_BASE64,
            NULL,
            &outLen,
            NULL,
            NULL))
            throw std::runtime_error(LastErrorText("CryptStringToBinaryA length"));

        std::vector<BYTE> out(outLen, 0);
        if (!CryptStringToBinaryA(text.c_str(),
            static_cast<DWORD>(text.size()),
            CRYPT_STRING_BASE64,
            &out[0],
            &outLen,
            NULL,
            NULL))
            throw std::runtime_error(LastErrorText("CryptStringToBinaryA"));

        out.resize(outLen);
        return out;
    }

    static std::vector<BYTE> GetKey()
    {
        return Sha256(OriginalKey());
    }

public:
    static std::string Encrypt(const std::string& plainText)
    {
        if (plainText.empty()) return plainText;

        HCRYPTPROV hProv = AcquireProvider();
        HCRYPTKEY hKey = 0;

        try
        {
            hKey = ImportAes256Key(hProv, GetKey());

            DWORD dataLen = static_cast<DWORD>(plainText.size());
            DWORD bufLen = dataLen + 16; // PKCS7 may add one full block
            std::vector<BYTE> buffer(bufLen, 0);
            std::memcpy(&buffer[0], plainText.data(), dataLen);

            if (!CryptEncrypt(hKey, 0, TRUE, 0, &buffer[0], &dataLen, bufLen))
                throw std::runtime_error(LastErrorText("CryptEncrypt"));

            buffer.resize(dataLen);
            std::string b64 = Base64Encode(buffer);

            CryptDestroyKey(hKey);
            CryptReleaseContext(hProv, 0);
            return b64;
        }
        catch (...)
        {
            if (hKey) CryptDestroyKey(hKey);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }

    static std::string Decrypt(const std::string& cipherText)
    {
        if (cipherText.empty()) return cipherText;

        HCRYPTPROV hProv = AcquireProvider();
        HCRYPTKEY hKey = 0;

        try
        {
            hKey = ImportAes256Key(hProv, GetKey());

            std::vector<BYTE> buffer = Base64Decode(cipherText);
            if (buffer.empty())
                throw std::runtime_error("AES cipher data is empty.");
            DWORD dataLen = static_cast<DWORD>(buffer.size());

            if (!CryptDecrypt(hKey, 0, TRUE, 0, &buffer[0], &dataLen))
                throw std::runtime_error(LastErrorText("CryptDecrypt"));

            std::string plain(reinterpret_cast<char*>(&buffer[0]),
                reinterpret_cast<char*>(&buffer[0]) + dataLen);

            CryptDestroyKey(hKey);
            CryptReleaseContext(hProv, 0);
            return plain;
        }
        catch (...)
        {
            if (hKey) CryptDestroyKey(hKey);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }
};

struct LensParameter
{
    std::string LensName;

    double Step;
    double Slope;
    double Centroid_X;
    double Origin_Centroid_X;
    double LightIntensity;
    double ExposureTime;
    double PValue;
    double MaxPos; // replaces old LinearRegression.txt 4th line, optional
    int FocusDoneRange;
    int MaxFocusStep;
    int LinearRangeCount;
    int CaptureRangeCount;
    int StableCount;
    int SegmentMode;
    int TrackingLimitCount;
    double CalibrationR2;
    double FullStepsPerMm;
    int Microstepping;

    bool HasStep;
    bool HasSlope;
    bool HasCentroid_X;
    bool HasOrigin_Centroid_X;
    bool HasLightIntensity;
    bool HasExposureTime;
    bool HasPValue;
    bool HasMaxPos;
    bool HasFocusDoneRange;
    bool HasMaxFocusStep;
    bool HasLinearRangeCount;
    bool HasCaptureRangeCount;
    bool HasStableCount;
    bool HasSegmentMode;
    bool HasTrackingLimitCount;
    bool HasCalibrationR2;
    bool HasFullStepsPerMm;
    bool HasMicrostepping;

    LensParameter()
        : Step(0), Slope(0), Centroid_X(0), Origin_Centroid_X(0),
        LightIntensity(0), ExposureTime(0), PValue(0), MaxPos(0), FocusDoneRange(0),
        MaxFocusStep(0), LinearRangeCount(0), CaptureRangeCount(0),
        StableCount(0), SegmentMode(0), TrackingLimitCount(0),
        CalibrationR2(0), FullStepsPerMm(0), Microstepping(0),
        HasStep(false), HasSlope(false), HasCentroid_X(false),
        HasOrigin_Centroid_X(false), HasLightIntensity(false),
        HasExposureTime(false), HasPValue(false), HasMaxPos(false),
        HasFocusDoneRange(false), HasMaxFocusStep(false),
        HasLinearRangeCount(false), HasCaptureRangeCount(false),
        HasStableCount(false), HasSegmentMode(false),
        HasTrackingLimitCount(false), HasCalibrationR2(false),
        HasFullStepsPerMm(false), HasMicrostepping(false)
    {
    }

    void SetStep(double v) { Step = v; HasStep = true; }
    void SetSlope(double v) { Slope = v; HasSlope = true; }
    void SetCentroidX(double v) { Centroid_X = v; HasCentroid_X = true; }
    void SetOriginCentroidX(double v) { Origin_Centroid_X = v; HasOrigin_Centroid_X = true; }
    void SetLightIntensity(double v) { LightIntensity = v; HasLightIntensity = true; }
    void SetExposureTime(double v) { ExposureTime = v; HasExposureTime = true; }
    void SetPValue(double v) { PValue = v; HasPValue = true; }
    void SetMaxPos(double v) { MaxPos = v; HasMaxPos = true; }
    void SetFocusDoneRange(int v) { FocusDoneRange = v; HasFocusDoneRange = true; }
    void SetMaxFocusStep(int v) { MaxFocusStep = v; HasMaxFocusStep = true; }
    void SetLinearRangeCount(int v) { LinearRangeCount = v; HasLinearRangeCount = true; }
    void SetCaptureRangeCount(int v) { CaptureRangeCount = v; HasCaptureRangeCount = true; }
    void SetStableCount(int v) { StableCount = v; HasStableCount = true; }
    void SetSegmentMode(int v) { SegmentMode = v; HasSegmentMode = true; }
    void SetTrackingLimitCount(int v) { TrackingLimitCount = v; HasTrackingLimitCount = true; }
    void SetCalibrationR2(double v) { CalibrationR2 = v; HasCalibrationR2 = true; }
    void SetFullStepsPerMm(double v) { FullStepsPerMm = v; HasFullStepsPerMm = true; }
    void SetMicrostepping(int v) { Microstepping = v; HasMicrostepping = true; }
};

class LensParameterStorage
{
private:
    std::string _filePath;

    enum
    {
        DEFAULT_FOCUS_DONE_RANGE = 10,
        DEFAULT_STABLE_COUNT = 3,
        DEFAULT_SEGMENT_MODE = 0,
        DEFAULT_MICROSTEPPING = 1,
        MAX_LASER_POWER = 1000
    };
private:
    static bool FileExists(const std::string& path)
    {
        DWORD attr = GetFileAttributesA(path.c_str());
        return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool DirectoryExists(const std::string& path)
    {
        DWORD attr = GetFileAttributesA(path.c_str());
        return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    static void CreateDirectoryRecursive(const std::string& dir)
    {
        if (dir.empty()) return;
        if (DirectoryExists(dir)) return;

        for (size_t i = 0; i < dir.size(); ++i)
        {
            char c = dir[i];
            if (c == '\\' || c == '/')
            {
                std::string part = dir.substr(0, i);
                if (part.empty()) continue;
                if (part.size() == 2 && part[1] == ':') continue; // C:
                if (!DirectoryExists(part))
                    CreateDirectoryA(part.c_str(), NULL);
            }
        }

        if (!DirectoryExists(dir))
            CreateDirectoryA(dir.c_str(), NULL);
    }

    static void EnsureDirectoryForFile(const std::string& filePath)
    {
        size_t pos1 = filePath.find_last_of('\\');
        size_t pos2 = filePath.find_last_of('/');
        size_t pos = std::string::npos;

        if (pos1 == std::string::npos) pos = pos2;
        else if (pos2 == std::string::npos) pos = pos1;
        else pos = (pos1 > pos2 ? pos1 : pos2);

        if (pos == std::string::npos) return;

        std::string dir = filePath.substr(0, pos);
        CreateDirectoryRecursive(dir);
    }

    static std::string ReadAllText(const std::string& path)
    {
        std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
        if (!in.is_open()) return std::string();

        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    static void WriteAllText(const std::string& path, const std::string& text)
    {
        EnsureDirectoryForFile(path);

        const std::string tempPath = path + ".tmp";
        std::ofstream out(tempPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
        if (!out.is_open())
            throw std::runtime_error("Cannot open temporary file for write: " + tempPath);

        out.write(text.data(), static_cast<std::streamsize>(text.size()));
        out.flush();
        if (!out.good())
        {
            out.close();
            DeleteFileA(tempPath.c_str());
            throw std::runtime_error("Cannot write temporary file: " + tempPath);
        }
        out.close();

        if (!MoveFileExA(tempPath.c_str(), path.c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            DWORD errorCode = GetLastError();
            DeleteFileA(tempPath.c_str());
            std::ostringstream os;
            os << "Cannot replace file: " << path << ", GetLastError=" << errorCode;
            throw std::runtime_error(os.str());
        }
    }

    static void SkipWhite(const std::string& s, size_t& i)
    {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    }

    static std::string JsonEscape(const std::string& text)
    {
        std::ostringstream os;
        for (size_t i = 0; i < text.size(); ++i)
        {
            unsigned char c = static_cast<unsigned char>(text[i]);
            switch (c)
            {
            case '"': os << "\\\""; break;
            case '\\': os << "\\\\"; break;
            case '\b': os << "\\b"; break;
            case '\f': os << "\\f"; break;
            case '\n': os << "\\n"; break;
            case '\r': os << "\\r"; break;
            case '\t': os << "\\t"; break;
            default:
                if (c < 0x20)
                {
                    os << "\\u"
                        << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(c)
                        << std::dec << std::setfill(' ');
                }
                else
                {
                    os << static_cast<char>(c);
                }
                break;
            }
        }
        return os.str();
    }

    static std::string ParseJsonString(const std::string& s, size_t& i)
    {
        SkipWhite(s, i);
        if (i >= s.size() || s[i] != '"')
            throw std::runtime_error("Invalid JSON string.");

        ++i;
        std::ostringstream os;
        while (i < s.size())
        {
            char c = s[i++];
            if (c == '"') break;

            if (c == '\\')
            {
                if (i >= s.size()) break;
                char e = s[i++];
                switch (e)
                {
                case '"': os << '"'; break;
                case '\\': os << '\\'; break;
                case '/': os << '/'; break;
                case 'b': os << '\b'; break;
                case 'f': os << '\f'; break;
                case 'n': os << '\n'; break;
                case 'r': os << '\r'; break;
                case 't': os << '\t'; break;
                case 'u':
                    // This parser is intended for the JSON generated by this file.
                    // For \uXXXX, keep it simple and ignore conversion for non-ASCII unicode.
                    if (i + 4 <= s.size()) i += 4;
                    break;
                default:
                    os << e;
                    break;
                }
            }
            else
            {
                os << c;
            }
        }
        return os.str();
    }

    static double ParseJsonNumber(const std::string& s, size_t& i)
    {
        SkipWhite(s, i);
        size_t start = i;
        if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
        if (i < s.size() && s[i] == '.')
        {
            ++i;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
        }
        if (i < s.size() && (s[i] == 'e' || s[i] == 'E'))
        {
            ++i;
            if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
        }

        std::string num = s.substr(start, i - start);
        if (num.empty()) throw std::runtime_error("Invalid JSON number.");
        return std::atof(num.c_str());
    }

    static void SkipJsonValue(const std::string& s, size_t& i)
    {
        SkipWhite(s, i);
        if (i >= s.size()) return;

        if (s[i] == '"')
        {
            ParseJsonString(s, i);
            return;
        }

        if (s[i] == '{')
        {
            int depth = 0;
            do
            {
                if (s[i] == '"')
                {
                    ParseJsonString(s, i);
                    continue;
                }
                if (s[i] == '{') ++depth;
                if (s[i] == '}') --depth;
                ++i;
            } while (i < s.size() && depth > 0);
            return;
        }

        if (s[i] == '[')
        {
            int depth = 0;
            do
            {
                if (s[i] == '"')
                {
                    ParseJsonString(s, i);
                    continue;
                }
                if (s[i] == '[') ++depth;
                if (s[i] == ']') --depth;
                ++i;
            } while (i < s.size() && depth > 0);
            return;
        }

        while (i < s.size() && s[i] != ',' && s[i] != '}' && s[i] != ']') ++i;
    }

    static std::string ReadObjectText(const std::string& s, size_t& i)
    {
        SkipWhite(s, i);
        if (i >= s.size() || s[i] != '{')
            throw std::runtime_error("Invalid JSON object.");

        size_t start = i;
        int depth = 0;
        do
        {
            if (s[i] == '"')
            {
                ParseJsonString(s, i);
                continue;
            }
            if (s[i] == '{') ++depth;
            if (s[i] == '}') --depth;
            ++i;
        } while (i < s.size() && depth > 0);

        return s.substr(start, i - start);
    }

    static LensParameter ParseLensObject(const std::string& obj)
    {
        LensParameter p;
        size_t i = 0;
        SkipWhite(obj, i);
        if (i >= obj.size() || obj[i] != '{') return p;
        ++i;

        while (i < obj.size())
        {
            SkipWhite(obj, i);
            if (i < obj.size() && obj[i] == '}') break;

            std::string key = ParseJsonString(obj, i);
            SkipWhite(obj, i);
            if (i >= obj.size() || obj[i] != ':') break;
            ++i;
            SkipWhite(obj, i);

            if (key == "LensName")
            {
                if (i < obj.size() && obj[i] == '"')
                    p.LensName = ParseJsonString(obj, i);
                else
                    SkipJsonValue(obj, i);
            }
            else if (key == "Step")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetStep(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "Slope")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetSlope(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "Centroid_X")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetCentroidX(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "Origin_Centroid_X")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetOriginCentroidX(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "LightIntensity")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetLightIntensity(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "ExposureTime")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetExposureTime(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "PValue")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetPValue(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "MaxPos")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetMaxPos(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "FocusDoneRange")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetFocusDoneRange(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "MaxFocusStep")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetMaxFocusStep(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "LinearRangeCount")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetLinearRangeCount(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "CaptureRangeCount")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetCaptureRangeCount(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "StableCount")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetStableCount(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "SegmentMode")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetSegmentMode(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "TrackingLimitCount")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetTrackingLimitCount(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else if (key == "CalibrationR2")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetCalibrationR2(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "FullStepsPerMm")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetFullStepsPerMm(ParseJsonNumber(obj, i));
                else SkipJsonValue(obj, i);
            }
            else if (key == "Microstepping")
            {
                if (i < obj.size() && obj[i] != 'n') p.SetMicrostepping(static_cast<int>(ParseJsonNumber(obj, i)));
                else SkipJsonValue(obj, i);
            }
            else
            {
                SkipJsonValue(obj, i);
            }

            SkipWhite(obj, i);
            if (i < obj.size() && obj[i] == ',') ++i;
        }

        return p;
    }

    static std::string NumberToJson(double v)
    {
        std::ostringstream os;
        os << std::setprecision(std::numeric_limits<double>::max_digits10) << v;
        return os.str();
    }

    static void AppendNumberField(std::ostringstream& os,
        bool& first,
        const char* name,
        double value)
    {
        if (!first) os << ",\n";
        os << "    \"" << name << "\": " << NumberToJson(value);
        first = false;
    }

    static std::string SerializeLensObject(const LensParameter& p)
    {
        std::ostringstream os;
        os << "{\n";
        bool first = true;

        if (!p.LensName.empty())
        {
            os << "    \"LensName\": \"" << JsonEscape(p.LensName) << "\"";
            first = false;
        }

        if (p.HasStep)             AppendNumberField(os, first, "Step", p.Step);
        if (p.HasSlope)            AppendNumberField(os, first, "Slope", p.Slope);
        if (p.HasCentroid_X)       AppendNumberField(os, first, "Centroid_X", p.Centroid_X);
        if (p.HasOrigin_Centroid_X)AppendNumberField(os, first, "Origin_Centroid_X", p.Origin_Centroid_X);
        if (p.HasLightIntensity)   AppendNumberField(os, first, "LightIntensity", p.LightIntensity);
        if (p.HasExposureTime)     AppendNumberField(os, first, "ExposureTime", p.ExposureTime);
        if (p.HasPValue)           AppendNumberField(os, first, "PValue", p.PValue);
        if (p.HasMaxPos)           AppendNumberField(os, first, "MaxPos", p.MaxPos);
        if (p.HasFocusDoneRange)   AppendNumberField(os, first, "FocusDoneRange", p.FocusDoneRange);
        if (p.HasMaxFocusStep)     AppendNumberField(os, first, "MaxFocusStep", p.MaxFocusStep);
        if (p.HasLinearRangeCount) AppendNumberField(os, first, "LinearRangeCount", p.LinearRangeCount);
        if (p.HasCaptureRangeCount)AppendNumberField(os, first, "CaptureRangeCount", p.CaptureRangeCount);
        if (p.HasStableCount)      AppendNumberField(os, first, "StableCount", p.StableCount);
        if (p.HasSegmentMode)      AppendNumberField(os, first, "SegmentMode", p.SegmentMode);
        if (p.HasTrackingLimitCount)AppendNumberField(os, first, "TrackingLimitCount", p.TrackingLimitCount);
        if (p.HasCalibrationR2)    AppendNumberField(os, first, "CalibrationR2", p.CalibrationR2);
        if (p.HasFullStepsPerMm)   AppendNumberField(os, first, "FullStepsPerMm", p.FullStepsPerMm);
        if (p.HasMicrostepping)    AppendNumberField(os, first, "Microstepping", p.Microstepping);

        os << "\n  }";
        return os.str();
    }

    static std::string SerializeAll(const std::map<std::string, LensParameter>& all)
    {
        std::ostringstream os;
        os << "{\n";

        bool first = true;
        for (std::map<std::string, LensParameter>::const_iterator it = all.begin(); it != all.end(); ++it)
        {
            if (!first) os << ",\n";
            os << "  \"" << JsonEscape(it->first) << "\": " << SerializeLensObject(it->second);
            first = false;
        }

        os << "\n}";
        return os.str();
    }

    static std::map<std::string, LensParameter> ParseAll(const std::string& json)
    {
        std::map<std::string, LensParameter> result;
        size_t i = 0;
        SkipWhite(json, i);
        if (i >= json.size() || json[i] != '{') return result;
        ++i;

        while (i < json.size())
        {
            SkipWhite(json, i);
            if (i < json.size() && json[i] == '}') break;

            std::string lensKey = ParseJsonString(json, i);
            SkipWhite(json, i);
            if (i >= json.size() || json[i] != ':') break;
            ++i;

            std::string objText = ReadObjectText(json, i);
            LensParameter p = ParseLensObject(objText);
            if (p.LensName.empty()) p.LensName = lensKey;
            result[lensKey] = p;

            SkipWhite(json, i);
            if (i < json.size() && json[i] == ',') ++i;
        }

        return result;
    }

    std::map<std::string, LensParameter> LoadAllParameters()
    {
        if (!FileExists(_filePath))
            return std::map<std::string, LensParameter>();

        std::string encryptedJson = ReadAllText(_filePath);
        if (encryptedJson.empty())
            throw std::runtime_error("lens parameter file is empty: " + _filePath);

        std::string json = AesEncryption::Decrypt(encryptedJson);
        size_t first = json.find_first_not_of(" \t\r\n");
        size_t last = json.find_last_not_of(" \t\r\n");
        if (first == std::string::npos || last == std::string::npos ||
            json[first] != '{' || json[last] != '}')
        {
            throw std::runtime_error("decrypted lens parameter content is not a JSON object");
        }

        std::map<std::string, LensParameter> allParams = ParseAll(json);
        bool changed = false;

        for (std::map<std::string, LensParameter>::iterator it = allParams.begin();
            it != allParams.end();
            ++it)
        {
            LensParameter& p = it->second;
            if (p.HasLightIntensity &&
                (p.LightIntensity < 0.0 ||
                    p.LightIntensity > MAX_LASER_POWER))
            {
                p.SetLightIntensity(std::max(
                    0.0,
                    std::min(
                        static_cast<double>(MAX_LASER_POWER),
                        p.LightIntensity)));
                changed = true;
            }
            if (!p.HasFocusDoneRange || p.FocusDoneRange == 0)
            {
                p.SetFocusDoneRange(DEFAULT_FOCUS_DONE_RANGE);
                changed = true;
            }
            if (!p.HasPValue || p.PValue <= 0.0)
            {
                p.SetPValue(0.8);
                changed = true;
            }

            const int legacyStep =
                p.HasStep && p.Step > 0.0
                ? static_cast<int>(p.Step + 0.5)
                : 100;
            const int singleCorrectionStep = std::max(1, legacyStep / 5);

            if (!p.HasMaxFocusStep || p.MaxFocusStep <= 0)
            {
                p.SetMaxFocusStep(singleCorrectionStep);
                changed = true;
            }
            if (!p.HasLinearRangeCount || p.LinearRangeCount <= 0)
            {
                p.SetLinearRangeCount(
                    std::max(legacyStep, p.FocusDoneRange * 4));
                changed = true;
            }
            if (!p.HasCaptureRangeCount ||
                p.CaptureRangeCount < p.LinearRangeCount)
            {
                p.SetCaptureRangeCount(p.LinearRangeCount * 3);
                changed = true;
            }
            if (!p.HasStableCount || p.StableCount <= 0)
            {
                p.SetStableCount(DEFAULT_STABLE_COUNT);
                changed = true;
            }
            if (!p.HasSegmentMode ||
                p.SegmentMode < 0 || p.SegmentMode > 2)
            {
                p.SetSegmentMode(DEFAULT_SEGMENT_MODE);
                changed = true;
            }
            if (!p.HasTrackingLimitCount || p.TrackingLimitCount < 0)
            {
                p.SetTrackingLimitCount(0);
                changed = true;
            }
            if (!p.HasCalibrationR2)
            {
                p.SetCalibrationR2(0.0);
                changed = true;
            }
            if (!p.HasFullStepsPerMm || p.FullStepsPerMm < 0.0)
            {
                p.SetFullStepsPerMm(0.0);
                changed = true;
            }
            if (!p.HasMicrostepping || p.Microstepping <= 0)
            {
                p.SetMicrostepping(DEFAULT_MICROSTEPPING);
                changed = true;
            }
        }

        if (changed)
            WriteAllParameters(allParams);

        return allParams;
    }

    void WriteAllParameters(const std::map<std::string, LensParameter>& allParams)
    {
        std::string json = SerializeAll(allParams);
        std::string encryptedJson = AesEncryption::Encrypt(json);
        WriteAllText(_filePath, encryptedJson);
    }

public:
    explicit LensParameterStorage(const std::string& storageFilePath = "lens_parameters.json")
        : _filePath(storageFilePath)
    {
        EnsureDirectoryForFile(_filePath);
    }

    void SaveParameter(const std::string& lensName, const LensParameter& newParams)
    {
        if (lensName.empty())
            throw std::invalid_argument("lensName cannot be empty.");

        std::map<std::string, LensParameter> allParams = LoadAllParameters();

        std::map<std::string, LensParameter>::iterator it = allParams.find(lensName);
        if (it != allParams.end())
        {
            LensParameter& existing = it->second;

            if (newParams.HasStep)              existing.SetStep(newParams.Step);
            if (newParams.HasSlope)             existing.SetSlope(newParams.Slope);
            if (newParams.HasCentroid_X)        existing.SetCentroidX(newParams.Centroid_X);
            if (newParams.HasOrigin_Centroid_X) existing.SetOriginCentroidX(newParams.Origin_Centroid_X);
            if (newParams.HasLightIntensity)    existing.SetLightIntensity(newParams.LightIntensity);
            if (newParams.HasExposureTime)      existing.SetExposureTime(newParams.ExposureTime);
            if (newParams.HasPValue)            existing.SetPValue(newParams.PValue);
            if (newParams.HasMaxPos)            existing.SetMaxPos(newParams.MaxPos);
            if (newParams.HasFocusDoneRange)    existing.SetFocusDoneRange(newParams.FocusDoneRange);
            if (newParams.HasMaxFocusStep)      existing.SetMaxFocusStep(newParams.MaxFocusStep);
            if (newParams.HasLinearRangeCount)  existing.SetLinearRangeCount(newParams.LinearRangeCount);
            if (newParams.HasCaptureRangeCount) existing.SetCaptureRangeCount(newParams.CaptureRangeCount);
            if (newParams.HasStableCount)       existing.SetStableCount(newParams.StableCount);
            if (newParams.HasSegmentMode)       existing.SetSegmentMode(newParams.SegmentMode);
            if (newParams.HasTrackingLimitCount)existing.SetTrackingLimitCount(newParams.TrackingLimitCount);
            if (newParams.HasCalibrationR2)     existing.SetCalibrationR2(newParams.CalibrationR2);
            if (newParams.HasFullStepsPerMm)    existing.SetFullStepsPerMm(newParams.FullStepsPerMm);
            if (newParams.HasMicrostepping)     existing.SetMicrostepping(newParams.Microstepping);
            if (!newParams.LensName.empty())    existing.LensName = newParams.LensName;
        }
        else
        {
            LensParameter p = newParams;
            if (p.LensName.empty()) p.LensName = lensName;
            allParams[lensName] = p;
        }

        WriteAllParameters(allParams);
    }

    bool TryGetParameter(const std::string& lensName, LensParameter& outParam)
    {
        if (lensName.empty())
            return false;

        std::map<std::string, LensParameter> allParams = LoadAllParameters();
        std::map<std::string, LensParameter>::iterator it = allParams.find(lensName);
        if (it == allParams.end())
            return false;

        outParam = it->second;
        return true;
    }

    std::map<std::string, LensParameter> GetAllParameters()
    {
        return LoadAllParameters();
    }

    std::vector<std::string> GetLensNames()
    {
        std::vector<std::string> names;
        std::map<std::string, LensParameter> allParams = LoadAllParameters();
        for (std::map<std::string, LensParameter>::const_iterator it = allParams.begin(); it != allParams.end(); ++it)
            names.push_back(it->first);
        return names;
    }

    bool DeleteParameter(const std::string& lensName)
    {
        if (lensName.empty())
            return false;

        std::map<std::string, LensParameter> allParams = LoadAllParameters();
        std::map<std::string, LensParameter>::iterator it = allParams.find(lensName);
        if (it == allParams.end())
            return false;

        allParams.erase(it);
        WriteAllParameters(allParams);
        return true;
    }

    bool RenameParameter(const std::string& oldName, const std::string& newName)
    {
        if (oldName.empty() || newName.empty())
            return false;

        std::map<std::string, LensParameter> allParams = LoadAllParameters();
        std::map<std::string, LensParameter>::iterator oldIt = allParams.find(oldName);
        if (oldIt == allParams.end())
            return false;
        if (oldName != newName && allParams.find(newName) != allParams.end())
            return false;

        LensParameter renamed = oldIt->second;
        renamed.LensName = newName;
        allParams.erase(oldIt);
        allParams[newName] = renamed;
        WriteAllParameters(allParams);
        return true;
    }

    LensParameter GetParameter(const std::string& lensName)
    {
        if (lensName.empty())
            throw std::invalid_argument("lensName cannot be empty.");

        std::map<std::string, LensParameter> allParams = LoadAllParameters();
        std::map<std::string, LensParameter>::iterator it = allParams.find(lensName);

        if (it == allParams.end())
        {
            LensParameter p;
            p.LensName = lensName;
            p.SetStep(1000);
            p.SetCentroidX(0);
            p.SetOriginCentroidX(0);
            SaveParameter(lensName, p);
            return p;
        }

        return it->second;
    }
};

#endif // LENS_PARAMETER_STORAGE_CPP14_H
