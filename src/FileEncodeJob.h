/*
 * Copyright (C) 2018 Holger Kälberer <holger.k@elberer.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstdint>
#include <string>

typedef struct lame_global_struct lame_global_flags;

class FileEncodeJob
{
private:
    struct WavHeader
    {
        uint8_t riff[4];
        uint32_t chunkSize;
        uint8_t wave[4];        // WAVE Header
        uint8_t fmt[4];
        uint32_t subchunk1Size;
        uint16_t audioFormat;   // Audio format: 1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t numChannels;   // # channels 1=Mono 2=Stereo
        uint32_t samplingRate;  // sampling frequency Hz
        uint32_t bytesPerSec;   // bytes per second
        uint16_t blockAlign;    // 2=16-bit mono, 4=16-bit stereo
        uint16_t bitsPerSample; // Number of bits per sample
        uint8_t subchunk2ID[4];
        uint32_t subchunk2Size;
    } m_wavHeader;

    std::string m_iFilename;
    FILE *m_iFile = nullptr;
    FILE *m_oFile = nullptr;
    lame_global_flags *m_lgf = nullptr;

    void driveEncoder() const;
    bool parseWavHeader();
    int readPcmData(int inBuffer[2][1152]) const;

public:
    explicit FileEncodeJob(const std::string& f)
        : m_iFilename(f) {}

    ~FileEncodeJob() {}

    bool process();
};
