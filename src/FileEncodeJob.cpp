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
#include <lame/lame.h>
#include <cstdint>
#include <cstring>
#include <cinttypes>

#include "utils.h"
#include "FileEncodeJob.h"

int FileEncodeJob::readPcmData(int inBuffer[2][1152]) const
{
    int dataBuffer[2 * 1152];
    const int framesize = lame_get_framesize(m_lgf);
    size_t bytesPerSample = m_wavHeader.bitsPerSample / 8;
    size_t read = fread(dataBuffer, bytesPerSample,
                        static_cast<size_t>(m_wavHeader.numChannels * framesize),
                        m_iFile);

    if (read > 0) {
        // convert from little to big endian:
        int bytesRead = read * bytesPerSample;
        char* srcPtr = reinterpret_cast<char*>(dataBuffer);
        char* dstPtrL = reinterpret_cast<char*>(inBuffer[0]);
        char* dstPtrR = reinterpret_cast<char*>(inBuffer[1]);
        for (int i = 0; i < bytesRead; i += bytesPerSample*2) {
            // FIXME: handle bitsPerSample values != 16:
            dstPtrL[i] = 0;
            dstPtrL[i+1] = 0;
            dstPtrL[i+2] = srcPtr[i];
            dstPtrL[i+3] = srcPtr[i+1];

            dstPtrR[i] = 0;
            dstPtrR[i+1] = 0;
            dstPtrR[i+2] = srcPtr[i+2];
            dstPtrR[i+3] = srcPtr[i+3];
        }
    }

    return read / 2;
}

void FileEncodeJob::driveEncoder() const
{
    unsigned char mp3buffer[LAME_MAXMP3BUFFER];
    int inBuffer[2][1152];

    int maxSamples = lame_get_maximum_number_of_samples(m_lgf, sizeof(mp3buffer));

    DBG("Got max samples %d", maxSamples);

    int sizeRead = 0;
    do {
        sizeRead = readPcmData(inBuffer);
        if (sizeRead > 0) {
            int* bufferL = inBuffer[0];
            int* bufferR = inBuffer[1];

            int sizeRemaining = sizeRead;
            do {
                int const sizeEncode = sizeRemaining < maxSamples ? sizeRemaining : maxSamples;

                int sizeDone = lame_encode_buffer_int(m_lgf, bufferL, bufferR,
                                                      sizeEncode, mp3buffer,
                                                      sizeof(mp3buffer));
                bufferL += sizeEncode;
                bufferR += sizeEncode;
                sizeRemaining -= sizeEncode;

                if (sizeDone < 0) {
                    ERR("Encoder error %d", sizeDone);
                    return;
                }
                int sizeWritten = (int) fwrite(mp3buffer, 1, sizeDone, m_oFile);
                if (sizeWritten != sizeDone) {
                    ERR("Error writing %d bytes mp3 data", sizeDone);
                    return;
                }
            } while (sizeRemaining > 0);
        }
    } while (sizeRead > 0);

    int sizeDone = lame_encode_flush(m_lgf, mp3buffer, sizeof(mp3buffer));

    if (sizeDone < 0) {
        ERR("Encoder error %d", sizeDone);
        return;
    }

    int sizeWritten = (int) fwrite(mp3buffer, 1, sizeDone, m_oFile);
    if (sizeWritten != sizeDone) {
        ERR("Error writing %d bytes mp3 data", sizeDone);
        return;
    }
//    lame_mp3_tags_fid(m_lgf, m_oFile);

    return;
}

bool FileEncodeJob::parseWavHeader()
{
    //Read the header
    size_t size = fread(&m_wavHeader, 1, sizeof(m_wavHeader), m_iFile);
    if (size != sizeof(m_wavHeader)) {
        ERR("Could not read complete wave header");
        return false;
    }
    DBG("Read %" PRIuPTR " (%" PRIuPTR ") bytes into header: #channels=%u, samling rate=%u, "
                                       "audioFormat=%u, block alignment=%u, bits per sample=%u",
        size, sizeof(m_wavHeader), m_wavHeader.numChannels,
        m_wavHeader.samplingRate, m_wavHeader.audioFormat,
        m_wavHeader.blockAlign, m_wavHeader.bitsPerSample);
    if (memcmp(m_wavHeader.wave, "WAVE", 4) != 0) {
        ERR("Did not find WAVE header!");
        return false;
    }

    if (m_wavHeader.audioFormat != 1) {
        ERR("Only supporting PCM audio format, found %d", m_wavHeader.audioFormat);
        return false;
    }

    if (m_wavHeader.bitsPerSample != 16) {
        ERR("Only supporting 16 bits per sample, found %d", m_wavHeader.bitsPerSample);
        return false;
    }

    lame_set_num_channels(m_lgf, m_wavHeader.numChannels);
    lame_set_in_samplerate(m_lgf, m_wavHeader.samplingRate);
    lame_set_brate(m_lgf, 128);
    lame_set_mode(m_lgf, JOINT_STEREO);
    lame_set_quality(m_lgf, 4);  // 2?

    // lame --preset standard
    lame_set_VBR_q(m_lgf, 2);
    lame_set_VBR(m_lgf, vbr_default);

    int ret = lame_init_params(m_lgf);

    if (ret == 1) {
        ERR("Could not initialize lame params");
        return false;
    }

    return true;
}

bool FileEncodeJob::process()
{
    std::string dirName = get_dir_name(m_iFilename);
    std::string fileName = get_file_name(m_iFilename);

    std::string oFilename = dirName + PATH_SEPERATOR
                            + fileName.substr(0, fileName.find_last_of("."))
                            + ".mp3";

    LOG("Encoding %s -> %s", m_iFilename.c_str(), oFilename.c_str());

    m_lgf = lame_init();

    if (!m_lgf) {
        ERR("Could not init lame");
        return false;
    }

    m_iFile = fopen(m_iFilename.c_str(), "r+b");
    if (!m_iFile) {
        ERR("Could not open input file: %s", m_iFilename.c_str());
        return false;
    }

    if (!parseWavHeader()) {
        ERR("Could not read wav header of file: %s", m_iFilename.c_str());
        fclose(m_iFile);
        return false;
    }

    m_oFile = fopen(oFilename.c_str(), "w+b");

    if (!m_oFile) {
        ERR("Could not open output file for writing: %s", oFilename.c_str());
        fclose(m_iFile);
        return false;
    }

    DBG("Now starting encoding");
    driveEncoder();

    fclose(m_oFile);
    fclose(m_iFile);

    lame_close(m_lgf);

    return true;
}
