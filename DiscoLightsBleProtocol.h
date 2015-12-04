
/********************************************************************************

  "Disco Lights BLE" Communication Protocol

  Copyright (c) 2015 Irina Riegert
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

********************************************************************************/

#if !defined( __DISCO_LIGHT_BLE_PROTOCOL_H__ )
#define  __DISCO_LIGHT_BLE_PROTOCOL_H__

# if defined(__cplusplus)
extern "C" {
# endif

  typedef enum tag_of_eProtocolID
  { ProtoID_Unknown          = 0
  , ProtoID_GeneralInfo      = 1
  , ProtoID_BeatChannel      = 2
  , ProtoID_FiveFreqBands    = 3
  // 4-16: Preallocated for future use    //
  // 17-199: Reserved                     //
  , ProtoID_DeviceCapability = 200
  , ProtoID_EffectInfo       = 201
  , ProtoID_GetDevStatus     = 202
  // 203-215: Preallocated for future use //
  // 216-254: Reserved                    //
  , ProtoID_DevNotification  = 255
  } eProtocolID;

  typedef enum tag_of_eExtFrameLength
  { eExtPacketLengthMinimumInBytes  = 2
  , eExtFrameLengthMinimumInBytes   = 3
  , eExtFrameBufferSzMinimumInBytes = 20
  , eExtFrameBufferSzMaximumInBytes = 40
  } eExtFrameLength;

  # if defined(WIN32) && (WIN32==1)
  #  pragma pack(push)
  #  pragma pack(1)
  # endif

  // DEFINITION: struct CmuExtPacketHeader_t

  typedef struct tag_of_CmuExtPacketHeader_t
  {
    unsigned char mProtocolID;   // [1,255] //
    unsigned char mFrameCount;   // [1,255] //
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketHeader_t
  , *PCMUEXTPKTHDR;

  // DEFINITION: struct CmuExtPacketGeneralInfo_t

 typedef enum tag_of_eGeneralInfoBitmap
 { eGeneralInfoInputSignalPresenceBit = 0x80
 } eGeneralInfoBitmap;

 typedef struct tag_of_CmuExtPacketGeneralInfo_t
 {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mFsInHz[2];         // byte 0       : bit 7      InputSignalPresenceBit         //
                                             //              : bits [0,6] integer part of UQ7.8  [0,100] //
                                             // byte 1       : bits [0,7] fraction part of UQ7.8 [1,255] //
    unsigned char        mTotalLogIntensity; // [0,255]  0   :-90dB                                      //
                                             //          255 : 0dB                                       //
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketGeneralInfo_t
  , *PCMUEXTPKTGENERAL;

  // DEFINITION: struct CmuExtPacketBeatInfo_t

  typedef enum tag_of_eBeatInfoBitmap
  { eBeatInfoBeatTrackingBit = 0x80
  } eBeatlInfoBitmap;

  typedef struct tag_of_CmuExtPacketBeatInfo_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mBeatPeriodInSamples[2]; // byte 0 : bit 7      BeatTrackingBit        //
                                                  //        : bits [0,6] integer part of UQ7.8  //
                                                  // byte 1 : bits [0,7] fraction part of UQ7.8 //
    unsigned char        mBeatStateInSamples[2];  // UQ8.0 0: beat cursor                       // 
                                                  // UQ8.0 1: beat end                          //
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketBeatInfo_t
  , *PCMUEXTPKTBEATINFO;

  // DEFINITION: struct CmuExtPacket5FreqBands_t

  typedef enum tag_of_e5FreqBands
  {
    e5FreqBandsLength = 5

  } e5FreqBands;

  typedef struct tag_of_CmuExtPacket5FreqBands_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mFreqBandLogIntensity[e5FreqBandsLength]; // [0-255] where 0:OFF 255:MAX_BRIGHTNESS //
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacket5FreqBands_t
  , *PCMUEXTPKT5FREQBANDS;

  // DEFINITION: struct CmuExtPacketReqDevCap_t

  typedef struct tag_of_CmuExtPacketReqDevCap_t
  {
    CmuExtPacketHeader_t mHeader;
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketReqDevCap_t
  , *PCMUEXTPKTREQDEVCAP;

  // DEFINITION: struct CmuExtPacketRspDevCap_t

  typedef enum tag_of_eLedType
  { eLedTypeUnknown = 0
  , eLedTypeSingle  = 1
  , eLedTypeStick   = 2
  , eLedTypeStrip   = 3
  , eLedTypeRing    = 4
  , eLedTypeMatrix  = 5
  , eNumofLedTypes
  } eLedType;

  typedef enum tag_of_eFeatureBitmap
  {
    eFeatureSupportEffectInfo = 0x1

  } eFeatureBitmap;

  typedef enum tag_of_eCapabilityVersion
  {
    eCapabilityVersionNumber = 1

  } eCapabilityVersion;

  typedef struct tag_of_CmuExtPacketRspDevCap_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mLedDevice;
    unsigned char        mSize[2];  // 0:Rows 1:Columns //
    unsigned char        mNumofEffects;
    unsigned char        mFeatureBitmap;
    unsigned char        mCapabilityVersion;
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketRspDevCap_t
  , *PCMUEXTPKTRSPDEVCAP;

  // DEFINITION: struct CmuExtPacketReqEffectInfo_t

  typedef struct tag_of_CmuExtPacketReqEffectInfo_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mIndex;

  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketReqEffectInfo_t
  , *PCMUEXTPKTREQEFFECTINFO;

  // DEFINITION: struct CmuExtPacketRspEffectInfo_t

  typedef enum tag_of_eEffectProperty
  {
    eEffectMaxNameLength = 12
    
  } eEffectProperty;

  typedef struct tag_of_CmuExtPacketRspEffectInfo_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mIndex;
    unsigned char        mBitmap;
    unsigned char        mNameBytes;
    unsigned char        mName[eEffectMaxNameLength];
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketRspEffectInfo_t
  , *PCMUEXTPKTRSPEFFECTINFO;

  // DEFINITION: struct CmuExtPacketGetDevStatus_t

  typedef struct tag_of_CmuExtPacketGetDevStatus_t
  {
    CmuExtPacketHeader_t mHeader;
  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketGetDevStatus_t
  , *PCMUEXTPKTGETDEVSTATUS;

  // DEFINITION: struct CmuExtPacketDevNotif_t

  typedef struct tag_of_CmuExtPacketDevNotif_t
  {
    CmuExtPacketHeader_t mHeader;
    unsigned char        mActiveEffectIndex;

  }
  # if !defined(WIN32) || (WIN32==0)
  __attribute__((aligned(1), packed))
  # endif
    CmuExtPacketDevNotif_t
  , *PCMUEXTPKTDEVNOTIF;

  # if defined(WIN32) && (WIN32==1)
  #  pragma pack(pop)
  #endif

# ifdef __cplusplus
}
# endif

// DEFINITION: class DiscoLightsBleProtocol

class DiscoLightsBleProtocol
{
private:

  enum { eCobsIdleByte = 0x00 };

  unsigned char mBuffer[eExtFrameBufferSzMinimumInBytes];

  unsigned int  mRcvCursor;
  unsigned int  mRcvCobsCode;
  unsigned char mRcvCobsState;

  unsigned char mRcvFrmState;
  unsigned char mRcvFrmId;
  unsigned char mRcvEopIndex;
  unsigned char mRcvSopIndex;

private:

  inline
  void CobsDecodeReset()
  {
    mRcvCursor = 0;
    mRcvCobsCode = 0;
    mRcvCobsState = 0;
  }

  inline
  int CobsDecode(unsigned char rcv_byte)
  {
    if (mRcvCursor > eExtFrameBufferSzMinimumInBytes)
    {
      return -1;
    }

    if (mRcvCobsState == 0)
    {
      if (rcv_byte == 1)
      {
        mBuffer[mRcvCursor++] = 0;
        return 2;
      }

      mRcvCobsCode          = rcv_byte - 1;
      mRcvCobsState         = 1;
      mBuffer[mRcvCursor++] = 0;

      return 1;
    }

    mBuffer[mRcvCursor++] = rcv_byte;

    if (--mRcvCobsCode <= 0)
    {
      mRcvCobsState = 0;

      return 2;
    }

    return 0;
  }

  inline
  void FrameDecodeReset()
  {
    CobsDecodeReset();

    mRcvFrmState =
    mRcvEopIndex =
    mRcvSopIndex = 0;
  }

  inline
  void FrameDecodeInit()
  {
    mRcvFrmId = 0;
    //
    FrameDecodeReset();
    CommandQueueReset();
    DecodedDataReset();
  }

  inline
  void FrameDecodeUpdateState(unsigned char current_frm_id)
  {
    if (mRcvFrmId == current_frm_id)
    {
      mRcvFrmState = 1;
    }
    else
    {
      mRcvFrmState = 0;
    }

    mRcvFrmId = current_frm_id;
    if (mRcvFrmId == 255)
    {
      mRcvFrmId = 1;
    }
    else
    {
      ++mRcvFrmId;
    }
  }

  inline
  void FrameDecodeShiftCobs()
  {
    mRcvCursor   = 0;
    mRcvEopIndex = 0;
    mRcvSopIndex = 0;
  }

public:

  // --- API --- //

  DiscoLightsBleProtocol()
  {
    FrameDecodeInit();
  }

  void Init()
  {
    FrameDecodeInit();
  }

  inline
  unsigned char BufferLength() const
  {
    return eExtFrameBufferSzMinimumInBytes;
  }

  inline
  unsigned char* Buffer()
  {
    return mBuffer;
  }
  
  // --- Frame Encoding --- // 

  unsigned char Encode_I(unsigned char *frm_buffer_ptr)
  {
    unsigned char frm_length = frm_buffer_ptr[0];
    unsigned char cursor_idx = 1;
    unsigned char code_idx   = 0;
    unsigned char code       = 1;

    if (frm_length == 0 || frm_length > eExtFrameBufferSzMaximumInBytes)
      return 0;

    while (cursor_idx < frm_length)
    {
      if (frm_buffer_ptr[cursor_idx] != eCobsIdleByte)
      {
        ++cursor_idx;
        ++code;
        continue;
      }

      frm_buffer_ptr[code_idx] = code;
      code = 1;
      code_idx = cursor_idx++;
    }

    frm_buffer_ptr[code_idx] = code;

    return frm_length;
  }

  inline
  unsigned char Encode()
  {
    return Encode_I(mBuffer);
  }

  bool FillBufferWithDeviceCapabilityFrame
  ( unsigned char device_type
  , unsigned char rows
  , unsigned char columns
  , unsigned char numof_effects
  , unsigned char feature_bitmap )
  {
    if ( eLedTypeUnknown == device_type || device_type > eNumofLedTypes || (rows==0 && eLedTypeMatrix==device_type) || columns==0 || numof_effects==0 )
      return false;

    mBuffer[0]  = sizeof(CmuExtPacketRspDevCap_t) + 1;
    mBuffer[1]  = ProtoID_DeviceCapability;
    mBuffer[2]  = mRcvFrmId;
    mBuffer[3]  = device_type;
    mBuffer[4]  = rows;
    mBuffer[5]  = columns;
    mBuffer[6]  = numof_effects;
    mBuffer[7]  = feature_bitmap;
    mBuffer[8]  = eCapabilityVersionNumber;
    mBuffer[9]  = 0;

    return true;
  }

  bool FillBufferWithEffectInfo(unsigned char effect_index, unsigned char bitmap, const char* name_string_ptr )
  {
    if (effect_index == 0 || name_string_ptr == NULL || name_string_ptr[0] == '\0')
      return false;

    int length = strlen(name_string_ptr);

    if (length > eEffectMaxNameLength)
      length = eEffectMaxNameLength;

    mBuffer[1] = ProtoID_EffectInfo;
    mBuffer[2] = mRcvFrmId;
    mBuffer[3] = effect_index;
    mBuffer[4] = bitmap;
    mBuffer[5] = length;
    
    memcpy(&mBuffer[6], name_string_ptr, length);

    length += 6;

    mBuffer[0]      = length;
    mBuffer[length] = 0;

    return true;
  }

  bool FillBufferWithNotification(unsigned char active_effect_index)
  {
    if (active_effect_index == 0 )
      return false;

    mBuffer[0] = sizeof(CmuExtPacketDevNotif_t) + 1;
    mBuffer[1] = ProtoID_DevNotification;
    mBuffer[2] = mRcvFrmId;
    mBuffer[3] = active_effect_index;
    mBuffer[4] = 0;

    return true;
  }

  // --- Frame Decoding --- //

private: // Command Queue //

  enum { QCmdBufferSize = 2 };
  enum { QCmdBufferMask = QCmdBufferSize-1 };
  unsigned short QCmdBuffer[QCmdBufferSize];
  unsigned char  QCmdRead;
  unsigned char  QCmdWrite;
  unsigned char  QCmdElements;

  inline
  void CommandQueueWrite(unsigned char cmd, unsigned char param = 0)
  {
    unsigned short val;

    if (QCmdElements >= QCmdBufferSize)
    {
      val = CommandQueueRead();
    }

    val = param;
    val <<= 8;
    val |= cmd;

    QCmdBuffer[QCmdWrite++] = val;

    QCmdWrite &= QCmdBufferMask;
    ++QCmdElements;
  }

public:

  inline 
  void CommandQueueReset()
  {
    QCmdRead = QCmdWrite = QCmdElements = 0;
  }
  
  inline
  unsigned short CommandQueueRead()
  {
    unsigned short val = 0;

    if (QCmdElements != 0)
    {
      val = QCmdBuffer[QCmdRead++];

      QCmdRead &= QCmdBufferMask;
      --QCmdElements;
    }
    
    return val;
  }

public: 

  void FrameDecode(unsigned char rcv_byte)
  {
    if (rcv_byte == eCobsIdleByte)
    {
      goto FrameResetAndFnExit;
    }

    CobsDecode(rcv_byte);

    if (mRcvEopIndex == 0)
    {
      unsigned char sop_set  = 0;
      unsigned char proto_id = ProtoID_Unknown;

      if (mRcvCursor >= eExtFrameLengthMinimumInBytes)
      {
        if (mBuffer[0] == 0 && mBuffer[1] != 0 && mBuffer[2] != 0)
        {
          proto_id     = mBuffer[1];
          sop_set      = 1;
          mRcvSopIndex = 1;

          FrameDecodeUpdateState(mBuffer[2]);
        }
      }
      else
      if (mRcvCursor >= eExtPacketLengthMinimumInBytes)
      {
        if (mBuffer[0] != 0 && mBuffer[1] != 0)
        {
          proto_id     = mBuffer[0];
          sop_set      = 1;
          mRcvSopIndex = 0;
        }
      }

      if (sop_set != 0)
      {
        switch (proto_id)
        {
        case ProtoID_Unknown:
        default:
          goto FrameResetAndFnExit;
        case ProtoID_GeneralInfo:
          mRcvEopIndex = sizeof(CmuExtPacketGeneralInfo_t) + mRcvSopIndex;
          break;
        case ProtoID_BeatChannel:
          mRcvEopIndex = sizeof(CmuExtPacketBeatInfo_t) + mRcvSopIndex;
          break;
        case ProtoID_FiveFreqBands:
          mRcvEopIndex = sizeof(CmuExtPacket5FreqBands_t) + mRcvSopIndex;
          break;
        case ProtoID_DeviceCapability:
          mRcvEopIndex = sizeof(CmuExtPacketReqDevCap_t) + mRcvSopIndex;
          break;
        case ProtoID_EffectInfo:
          mRcvEopIndex = sizeof(CmuExtPacketReqEffectInfo_t) + mRcvSopIndex;
          break;
        case ProtoID_GetDevStatus:
          mRcvEopIndex = sizeof(CmuExtPacketGetDevStatus_t) + mRcvSopIndex;
          break;
        }
      }
    }
    
    if ( mRcvEopIndex!=0 && mRcvCursor >= mRcvEopIndex)
    {
      unsigned char proto_id = mBuffer[mRcvSopIndex];
      
      if (  mRcvFrmState == 1 
         || proto_id == ProtoID_DeviceCapability
         || proto_id == ProtoID_EffectInfo
         || proto_id == ProtoID_GetDevStatus )
      {
        switch (proto_id)
        {
        case ProtoID_GeneralInfo:
          updateGeneralInfo((PCMUEXTPKTGENERAL)&mBuffer[mRcvSopIndex]);
          break;

        case ProtoID_BeatChannel:
          updateBeatChannel((PCMUEXTPKTBEATINFO)&mBuffer[mRcvSopIndex]);
          break;

        case ProtoID_FiveFreqBands:
          updateFreqBands((PCMUEXTPKT5FREQBANDS)&mBuffer[mRcvSopIndex]);
          break;

        case ProtoID_DeviceCapability:
          CommandQueueWrite(ProtoID_DeviceCapability);
          break;

        case ProtoID_EffectInfo:
          {
            PCMUEXTPKTREQEFFECTINFO pkt = (PCMUEXTPKTREQEFFECTINFO)&mBuffer[mRcvSopIndex];
            CommandQueueWrite(ProtoID_EffectInfo, pkt->mIndex);
          }
          break;

        case ProtoID_GetDevStatus:
          CommandQueueWrite(ProtoID_GetDevStatus);
          break;

        }
      }

      FrameDecodeShiftCobs();
    }

    return;

  FrameResetAndFnExit:
    FrameDecodeReset();
  }

private: // Data Packets Decoding //

  enum { eInputSignalPresenceBit = 0x01
       , eBeatTrackingBit        = 0x02 };

  unsigned char  mBitmap;

  inline 
  void updateGeneralInfo(PCMUEXTPKTGENERAL pkt)
  {
    mQ8p8FsInHz        = pkt->mFsInHz[0];
    mBitmap           &=~eInputSignalPresenceBit;
    mBitmap           |=(mQ8p8FsInHz & eGeneralInfoInputSignalPresenceBit)?eInputSignalPresenceBit:0;
    mQ8p8FsInHz       &=~eGeneralInfoInputSignalPresenceBit;
    mQ8p8FsInHz      <<= 8;
    mQ8p8FsInHz       |= pkt->mFsInHz[1];
    mTotalLogIntensity = pkt->mTotalLogIntensity;

    // reset beat info //
    mBitmap                  &=~eBeatTrackingBit;
    mQ8p8BeatPeriodInSamples = 0;
    mBeatCursorInSamples     = 0;
    mBeatEndInSamples        = 0;
  }

  inline
  void updateBeatChannel(PCMUEXTPKTBEATINFO pkt)
  {
    mQ8p8BeatPeriodInSamples   = pkt->mBeatPeriodInSamples[0];
    mBitmap                   |=(mQ8p8BeatPeriodInSamples & eBeatInfoBeatTrackingBit)?eBeatTrackingBit:0;
    mQ8p8BeatPeriodInSamples  &=~eBeatInfoBeatTrackingBit;
    mQ8p8BeatPeriodInSamples <<= 8;
    mQ8p8BeatPeriodInSamples  |= pkt->mBeatPeriodInSamples[1];
    mBeatCursorInSamples       = pkt->mBeatStateInSamples[0];
    mBeatEndInSamples          = pkt->mBeatStateInSamples[1];
  }

  inline
  void updateFreqBands(PCMUEXTPKT5FREQBANDS pkt)
  {
    memcpy(mFreqBandIntensity,pkt->mFreqBandLogIntensity,NumofFreqBands*sizeof(unsigned char));
  }

  inline
  void DecodedDataReset()
  {
    mBitmap                  = 0;
    mQ8p8FsInHz              = 0;
    mQ8p8BeatPeriodInSamples = 0;
    mBeatCursorInSamples     = 0;
    mBeatEndInSamples        = 0;
    memset(mFreqBandIntensity,0,NumofFreqBands*sizeof(unsigned char));    
  }

public: // Received Streaming Data //

  inline 
  unsigned char GetBpm() const
  {
    unsigned long bpm = 0;

    if (mQ8p8FsInHz != 0)
    {
      bpm  = mQ8p8BeatPeriodInSamples;
      bpm *= 100;
      bpm /= mQ8p8FsInHz;
      bpm  = 6000 / bpm;
      bpm &= 0xff;
    }

    return (unsigned char)bpm;
  }

  inline
  bool IsInputSignalPresent() const 
  {
    return mBitmap&eInputSignalPresenceBit?true:false;
  }
  
  inline 
  bool IsBeatTracked() const 
  {
    return mBitmap&eBeatTrackingBit?true:false;
  }

  unsigned char  mTotalLogIntensity;
  unsigned short mQ8p8FsInHz;

  unsigned short mQ8p8BeatPeriodInSamples;
  unsigned char  mBeatCursorInSamples;
  unsigned char  mBeatEndInSamples;
 
  enum { NumofFreqBands = e5FreqBandsLength };

  unsigned char  mFreqBandIntensity[NumofFreqBands];
};

#endif


