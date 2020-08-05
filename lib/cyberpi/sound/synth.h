#ifndef _SYNTH
#define _SYNTH
//*************************************************************************************
//  Arduino synth V4.1
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Dzl/Illutron 2014
//
//*************************************************************************************
#include "tables.h"

#define DIFF 1
#define CHA 2
#define CHB 3
#define CHC 4

#define SINE     0
#define RAMP     1
#define TRIANGLE 2
#define SQUARE     3
#define NOISE     4
#define SAW   5
#define A     6 //waves A-F added by AM 2015
#define B     7
#define C     8
#define D     9
#define E     10
#define F     11
#define G     12
#define H     13
#define I     14


#define FSample 20000.0                                              //-Sample rate (NOTE: must match tables.h)

volatile unsigned int PCW[16] = {
  0, 0, 0, 0,0, 0, 0, 0,
  0, 0, 0, 0,0, 0, 0, 0};			//-Wave phase accumulators
volatile unsigned int FTW[16] = {
  1000, 200, 300, 400,1000, 200, 300, 400,
  1000, 200, 300, 400,1000, 200, 300, 400};           //-Wave frequency tuning words
volatile unsigned char AMP[16] = {
  255, 255, 255, 255,255, 255, 255, 255,
  255, 255, 255, 255,255, 255, 255, 255};           //-Wave amplitudes [0-255]
volatile unsigned int PITCH[16] = {
  500, 500, 500, 500,500, 500, 500, 500,
  500, 500, 500, 500,500, 500, 500, 500};          //-Voice pitch
volatile int MOD[16] = {
  20, 0, 64, 127,20, 0, 64, 127,
  20, 0, 64, 127,20, 0, 64, 127};                         //-Voice envelope modulation [0-127 64=no mod. <64 pitch down >64 pitch up]
volatile unsigned int wavs[16];                                  //-Wave table selector [address of wave in memory]
volatile unsigned int envs[16];                                  //-Envelope selector [address of envelope in memory]
volatile unsigned int EPCW[16] = {
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000}; //-Envelope phase accumulator
volatile unsigned int EFTW[16] = {
  10, 10, 10, 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10};               //-Envelope speed tuning word
volatile unsigned char divider = 8;                             //-Sample rate decimator for envelope
volatile unsigned int tim = 0;
volatile unsigned char tik = 0;
volatile unsigned char output_mode;

//variables added for MintySynth:
unsigned char volume[16]={10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
typedef void (*AudioBack)(uint8_t* audio_buf,uint16_t audio_buf_len);
class MSynth
{
  public:
        
    MSynth()
    {
      _audioBufLength = 2048;
      _audioBufIndex = 0;
      _instrumentId = 8;
      _trackCount = 16;
      _audioBuf = (uint8_t*)malloc(_audioBufLength);
    }
    ~MSynth()
    {
      free(_audioBuf);
      _audioBufLength = 0;
    }
    //*********************************************************************
    //  Startup default
    //*********************************************************************

    void begin(AudioBack audioBack)
    {
      _audioBack = audioBack;
      output_mode=CHA;
      for(int i=0;i<_trackCount;i++)
      {
        wavs[i] = (unsigned int)EmptyTable;
        volume[i] = 8;
      }
    }
    void setInstrument(uint8_t instrumentId)
    {
        _instrumentId = instrumentId>14?14:(instrumentId<0?0:instrumentId);
    }
    void setVolume(unsigned char voice,uint8_t vol)
    {
      volume[voice] = vol;
    }
    //*********************************************************************
    //  Setup all voice parameters in MIDI range
    //  voice[0-3],wave[0-13],pitch[0-127],envelope[0-4],length[0-127],mod[0-127:64=no mod]
    //*********************************************************************

    void setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod)
    {
      setWave(voice,wave);
      setPitch(voice,pitch);
      setEnvelope(voice,env);
      setLength(voice,length);
      setMod(voice,mod);
    }

    //*********************************************************************
    //  Setup wave [0-6]
    //*********************************************************************

    void setWave(unsigned char voice, unsigned char wave)
    {

      switch (wave)
      {
        case SINE:
          wavs[voice] = (unsigned int)SinTable;
          break;
        case TRIANGLE:
          wavs[voice] = (unsigned int)TriangleTable;
          break;
        case SQUARE:
          wavs[voice] = (unsigned int)SquareTable;
          break;
        case SAW:
          wavs[voice] = (unsigned int)SawTable;
          break;
        case RAMP:
          wavs[voice] = (unsigned int)RampTable;
          break;
        case NOISE:
          wavs[voice] = (unsigned int)NoiseTable;
          break;
        case A:
          wavs[voice] = (unsigned int)ATable;
          break;
        case B:
          wavs[voice] = (unsigned int)BTable;
          break;
        case C:
          wavs[voice] = (unsigned int)CTable;
          break;
        case D:
          wavs[voice] = (unsigned int)DTable;
          break;
        case E:
          wavs[voice] = (unsigned int)ETable;
          break;
        case F:
          wavs[voice] = (unsigned int)FTable;
          break;
        case G:
          wavs[voice] = (unsigned int)GTable;
          break;
        case H:
          wavs[voice] = (unsigned int)HTable;
          break;
        default:
          wavs[voice] = (unsigned int)ITable;
          break;
      }
    }
    //*********************************************************************
    //  Setup Pitch [0-127]
    //*********************************************************************

    void setPitch(unsigned char voice,unsigned char MIDInote)
    {
      PITCH[voice]=(uint)(PITCHS[MIDInote]);
    }

    //*********************************************************************
    //  Setup Envelope [0-4]
    //*********************************************************************

    void setEnvelope(unsigned char voice, unsigned char env)
    {
      switch (env)
      {
      case 0:
        envs[voice] = (unsigned int)Env0;
        break;
      case 1:
        envs[voice] = (unsigned int)Env1;
        break;
      case 2:
        envs[voice] = (unsigned int)Env2;
        break;
      case 3:
        envs[voice] = (unsigned int)Env3;
        break;
      case 4:
        envs[voice] = (unsigned int)Env4;
        break;
      default:
        envs[voice] = (unsigned int)Env0;
        break;
      }
    }

    //*********************************************************************
    //  Setup Length [0-128]
    //*********************************************************************

    void setLength(unsigned char voice,unsigned char length)
    {
      EFTW[voice]=(uint)(EFTWS[length]);
    }

    //*********************************************************************
    //  Setup mod
    //*********************************************************************

    void setMod(unsigned char voice,unsigned char mod)
    {
      MOD[voice]=(int)mod-64;//0-127 64 = no mod
    }

    //*********************************************************************
    //  Set frequency direct
    //*********************************************************************

    void setFrequency(unsigned char voice,float f)
    {
      PITCH[voice]=f/(FSample/65535.0);

    }

    //*********************************************************************
    //  Set time
    //*********************************************************************

    void setTime(unsigned char voice,float t)
    {
      EFTW[voice]=(1.0/t)/(FSample/(32767.5*10.0));//[s];
    }

    //*********************************************************************
    //  Simple trigger
    //*********************************************************************

    void trigger(unsigned char voice)
    {
      EPCW[voice]=0;
      FTW[voice]=PITCH[voice];
      // FTW[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
    }

    void render()
    {
      divider++;
      if(!(divider%=_trackCount))
        tik=1;
      if (!(((uint8_t*)&EPCW[divider])[1]&0x80))
      {
        uint8_t*address = (uint8_t*)(envs[divider]);
        AMP[divider] = address[((uint8_t*)&(EPCW[divider]+=EFTW[divider]))[1]];
      }
      else
        AMP[divider] = 0;

      int16_t data =  0;
      for(int i=0;i<_trackCount;i++)
      {
        int8_t*address = (int8_t*)(wavs[i]);
        data+=(address[((uint8_t *)&(PCW[i] += FTW[i]))[1]] * AMP[i]) >>volume[i];
      }
      data = data/_trackCount;
      _audioBuf[_audioBufIndex] = 127+(int8_t)data;
      _audioBufIndex++;
      if(_audioBufIndex>=_audioBufLength)
      {
          _audioBufIndex = 0;
           (*_audioBack)(_audioBuf,_audioBufLength);
      }
      FTW[divider] = PITCH[divider] + (int) (((PITCH[divider]>>6)*(EPCW[divider]>>6))/128)*MOD[divider];
    }
    void addNote(int track, int note,int8_t time=64){
      track = track%_trackCount;
      setupVoice(track, _instrumentId, note, 1, time, 64);
      trigger(track);
    }
  private:
    AudioBack _audioBack;
    uint8_t *_audioBuf;
    uint8_t _trackCount;
    int _audioBufIndex;
    int _instrumentId;
    uint16_t _audioBufLength;
};

#endif
