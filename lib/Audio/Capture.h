#pragma once
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <Propsys.h>//IPropertyStore
#include <Propvarutil.h>
#include <SDKDDKVer.h>
#include "../Util/BasicTypes.h"
#include "../Util/Concepts/Sig.h"
#include "../Util/DataStructures/Optional.h"
#include "../Util/DataStructures/StringSso.h"
#include "../Util/DataStructures/TString.h"
#include "../Util/IterationControl.h"
#include "../Util/Macro/NoCopyMove.h"
#include "../Windows/COM.h"
#include "../Windows/WinAPI/Combase.h"
DEFINE_ENUM_PRINTABLE(WaveFormat, u16,
	(unknown, 0x0000),//Microsoft Corporation
	(adpcm, 0x0002),//Microsoft Corporation
	(ieee_float, 0x0003),//Microsoft Corporation
	(vselp, 0x0004),//Compaq Computer Corp.
	(ibm_cvsd, 0x0005),//IBM Corporation
	(alaw, 0x0006),//Microsoft Corporation
	(mulaw, 0x0007),//Microsoft Corporation
	(dts, 0x0008),//Microsoft Corporation
	(drm, 0x0009),//Microsoft Corporation
	(wmavoice9, 0x000A),//Microsoft Corporation
	(wmavoice10, 0x000B),//Microsoft Corporation
	(oki_adpcm, 0x0010),//OKI
	(dvi_adpcm, 0x0011),//Intel Corporation
	(mediaspace_adpcm, 0x0012),//Videologic
	(sierra_adpcm, 0x0013),//Sierra Semiconductor Corp
	(g723_adpcm, 0x0014),//Antex Electronics Corporation
	(digistd, 0x0015),//DSP Solutions, Inc.
	(digifix, 0x0016),//DSP Solutions, Inc.
	(dialogic_oki_adpcm, 0x0017),//Dialogic Corporation
	(mediavision_adpcm, 0x0018),//Media Vision, Inc.
	(cu_codec, 0x0019),//Hewlett-Packard Company
	(hp_dyn_voice, 0x001A),//Hewlett-Packard Company
	(yamaha_adpcm, 0x0020),//Yamaha Corporation of America
	(sonarc, 0x0021),//Speech Compression
	(dspgroup_truespeech, 0x0022),//DSP Group, Inc
	(echosc1, 0x0023),//Echo Speech Corporation
	(audiofile_af36, 0x0024),//Virtual Music, Inc.
	(aptx, 0x0025),//Audio Processing Technology
	(audiofile_af10, 0x0026),//Virtual Music, Inc.
	(prosody_1612, 0x0027),//Aculab plc
	(lrc, 0x0028),//Merging Technologies S.A.
	(dolby_ac2, 0x0030),//Dolby Laboratories
	(gsm610, 0x0031),//Microsoft Corporation
	(msnaudio, 0x0032),//Microsoft Corporation
	(antex_adpcme, 0x0033),//Antex Electronics Corporation
	(control_res_vqlpc, 0x0034),//Control Resources Limited
	(digireal, 0x0035),//DSP Solutions, Inc.
	(digiadpcm, 0x0036),//DSP Solutions, Inc.
	(control_res_cr10, 0x0037),//Control Resources Limited
	(nms_vbxadpcm, 0x0038),//Natural MicroSystems
	(cs_imaadpcm, 0x0039),//Crystal Semiconductor IMA ADPCM
	(echosc3, 0x003A),//Echo Speech Corporation
	(rockwell_adpcm, 0x003B),//Rockwell International
	(rockwell_digitalk, 0x003C),//Rockwell International
	(xebec, 0x003D),//Xebec Multimedia Solutions Limited
	(g721_adpcm, 0x0040),//Antex Electronics Corporation
	(g728_celp, 0x0041),//Antex Electronics Corporation
	(msg723, 0x0042),//Microsoft Corporation
	(intel_g723_1, 0x0043),//Intel Corp.
	(intel_g729, 0x0044),//Intel Corp.
	(sharp_g726, 0x0045),//Sharp
	(mpeg, 0x0050),//Microsoft Corporation
	(rt24, 0x0052),//InSoft, Inc.
	(pac, 0x0053),//InSoft, Inc.
	(mpeglayer3, 0x0055),//ISO/MPEG Layer3 Format Tag
	(lucent_g723, 0x0059),//Lucent Technologies
	(cirrus, 0x0060),//Cirrus Logic
	(espcm, 0x0061),//ESS Technology
	(voxware, 0x0062),//Voxware Inc
	(canopus_atrac, 0x0063),//Canopus, co., Ltd.
	(g726_adpcm, 0x0064),//APICOM
	(g722_adpcm, 0x0065),//APICOM
	(dsat, 0x0066),//Microsoft Corporation
	(dsat_display, 0x0067),//Microsoft Corporation
	(voxware_byte_aligned, 0x0069),//Voxware Inc
	(voxware_ac8, 0x0070),//Voxware Inc
	(voxware_ac10, 0x0071),//Voxware Inc
	(voxware_ac16, 0x0072),//Voxware Inc
	(voxware_ac20, 0x0073),//Voxware Inc
	(voxware_rt24, 0x0074),//Voxware Inc
	(voxware_rt29, 0x0075),//Voxware Inc
	(voxware_rt29hw, 0x0076),//Voxware Inc
	(voxware_vr12, 0x0077),//Voxware Inc
	(voxware_vr18, 0x0078),//Voxware Inc
	(voxware_tq40, 0x0079),//Voxware Inc
	(voxware_sc3, 0x007A),//Voxware Inc
	(voxware_sc3_1, 0x007B),//Voxware Inc
	(softsound, 0x0080),//Softsound, Ltd.
	(voxware_tq60, 0x0081),//Voxware Inc
	(msrt24, 0x0082),//Microsoft Corporation
	(g729a, 0x0083),//AT&T Labs, Inc.
	(mvi_mvi2, 0x0084),//Motion Pixels
	(df_g726, 0x0085),//DataFusion Systems (Pty) (Ltd)
	(df_gsm610, 0x0086),//DataFusion Systems (Pty) (Ltd)
	(isiaudio, 0x0088),//Iterated Systems, Inc.
	(onlive, 0x0089),//OnLive! Technologies, Inc.
	(multitude_ft_sx20, 0x008A),//Multitude Inc.
	(infocom_its_g721_adpcm, 0x008B),//Infocom
	(convedia_g729, 0x008C),//Convedia Corp.
	(congruency, 0x008D),//Congruency Inc.
	(sbc24, 0x0091),//Siemens Business Communications Sys
	(dolby_ac3_spdif, 0x0092),//Sonic Foundry
	(mediasonic_g723, 0x0093),//MediaSonic
	(prosody_8kbps, 0x0094),//Aculab plc
	(zyxel_adpcm, 0x0097),//ZyXEL Communications, Inc.
	(philips_lpcbb, 0x0098),//Philips Speech Processing
	(packed, 0x0099),//Studer Professional Audio AG
	(malden_phonytalk, 0x00A0),//Malden Electronics Ltd.
	(racal_recorder_gsm, 0x00A1),//Racal recorders
	(racal_recorder_g720_a, 0x00A2),//Racal recorders
	(racal_recorder_g723_1, 0x00A3),//Racal recorders
	(racal_recorder_tetra_acelp, 0x00A4),//Racal recorders
	(nec_aac, 0x00B0),//NEC Corp.
	(raw_aac1, 0x00FF),//For Raw AAC, with format block AudioSpecificConfig() (as defined by MPEG-4), that follows WAVEFORMATEX
	(rhetorex_adpcm, 0x0100),//Rhetorex Inc.
	(irat, 0x0101),//BeCubed Software Inc.
	(vivo_g723, 0x0111),//Vivo Software
	(vivo_siren, 0x0112),//Vivo Software
	(philips_celp, 0x0120),//Philips Speech Processing
	(philips_grundig, 0x0121),//Philips Speech Processing
	(digital_g723, 0x0123),//Digital Equipment Corporation
	(sanyo_ld_adpcm, 0x0125),//Sanyo Electric Co., Ltd.
	(siprolab_aceplnet, 0x0130),//Sipro Lab Telecom Inc.
	(siprolab_acelp4800, 0x0131),//Sipro Lab Telecom Inc.
	(siprolab_acelp8v3, 0x0132),//Sipro Lab Telecom Inc.
	(siprolab_g729, 0x0133),//Sipro Lab Telecom Inc.
	(siprolab_g729a, 0x0134),//Sipro Lab Telecom Inc.
	(siprolab_kelvin, 0x0135),//Sipro Lab Telecom Inc.
	(voiceage_amr, 0x0136),//VoiceAge Corp.
	(g726adpcm, 0x0140),//Dictaphone Corporation
	(dictaphone_celp68, 0x0141),//Dictaphone Corporation
	(dictaphone_celp54, 0x0142),//Dictaphone Corporation
	(qualcomm_purevoice, 0x0150),//Qualcomm, Inc.
	(qualcomm_halfrate, 0x0151),//Qualcomm, Inc.
	(tubgsm, 0x0155),//Ring Zero Systems, Inc.
	(msaudio1, 0x0160),//Microsoft Corporation
	(wmaudio2, 0x0161),//Microsoft Corporation
	(wmaudio3, 0x0162),//Microsoft Corporation
	(wmaudio_lossless, 0x0163),//Microsoft Corporation
	(wmaspdif, 0x0164),//Microsoft Corporation
	(unisys_nap_adpcm, 0x0170),//Unisys Corp.
	(unisys_nap_ulaw, 0x0171),//Unisys Corp.
	(unisys_nap_alaw, 0x0172),//Unisys Corp.
	(unisys_nap_16k, 0x0173),//Unisys Corp.
	(sycom_acm_syc008, 0x0174),//SyCom Technologies
	(sycom_acm_syc701_g726l, 0x0175),//SyCom Technologies
	(sycom_acm_syc701_celp54, 0x0176),//SyCom Technologies
	(sycom_acm_syc701_celp68, 0x0177),//SyCom Technologies
	(knowledge_adventure_adpcm, 0x0178),//Knowledge Adventure, Inc.
	(fraunhofer_iis_mpeg2_aac, 0x0180),//Fraunhofer IIS
	(dts_ds, 0x0190),//Digital Theatre Systems, Inc.
	(creative_adpcm, 0x0200),//Creative Labs, Inc
	(creative_fastspeech8, 0x0202),//Creative Labs, Inc
	(creative_fastspeech10, 0x0203),//Creative Labs, Inc
	(uher_adpcm, 0x0210),//UHER informatic GmbH
	(ulead_dv_audio, 0x0215),//Ulead Systems, Inc.
	(ulead_dv_audio_1, 0x0216),//Ulead Systems, Inc.
	(quarterdeck, 0x0220),//Quarterdeck Corporation
	(ilink_vc, 0x0230),//I-link Worldwide
	(raw_sport, 0x0240),//Aureal Semiconductor
	(esst_ac3, 0x0241),//ESS Technology, Inc.
	(generic_passthru, 0x0249),
	(ipi_hsx, 0x0250),//Interactive Products, Inc.
	(ipi_rpelp, 0x0251),//Interactive Products, Inc.
	(cs2, 0x0260),//Consistent Software
	(sony_scx, 0x0270),//Sony Corp.
	(sony_scy, 0x0271),//Sony Corp.
	(sony_atrac3, 0x0272),//Sony Corp.
	(sony_spc, 0x0273),//Sony Corp.
	(telum_audio, 0x0280),//Telum Inc.
	(telum_ia_audio, 0x0281),//Telum Inc.
	(norcom_voice_systems_adpcm, 0x0285),//Norcom Electronics Corp.
	(fm_towns_snd, 0x0300),//Fujitsu Corp.
	(micronas, 0x0350),//Micronas Semiconductors, Inc.
	(micronas_celp833, 0x0351),//Micronas Semiconductors, Inc.
	(btv_digital, 0x0400),//Brooktree Corporation
	(intel_music_coder, 0x0401),//Intel Corp.
	(indeo_audio, 0x0402),//Ligos
	(qdesign_music, 0x0450),//QDesign Corporation
	(on2_vp7_audio, 0x0500),//On2 Technologies
	(on2_vp6_audio, 0x0501),//On2 Technologies
	(vme_vmpcm, 0x0680),//AT&T Labs, Inc.
	(tpc, 0x0681),//AT&T Labs, Inc.
	(lightwave_lossless, 0x08AE),//Clearjump
	(oligsm, 0x1000),//Ing C. Olivetti & C., S.p.A.
	(oliadpcm, 0x1001),//Ing C. Olivetti & C., S.p.A.
	(olicelp, 0x1002),//Ing C. Olivetti & C., S.p.A.
	(olisbc, 0x1003),//Ing C. Olivetti & C., S.p.A.
	(oliopr, 0x1004),//Ing C. Olivetti & C., S.p.A.
	(lh_codec, 0x1100),//Lernout & Hauspie
	(lh_codec_celp, 0x1101),//Lernout & Hauspie
	(lh_codec_sbc8, 0x1102),//Lernout & Hauspie
	(lh_codec_sbc12, 0x1103),//Lernout & Hauspie
	(lh_codec_sbc16, 0x1104),//Lernout & Hauspie
	(norris, 0x1400),//Norris Communications, Inc.
	(isiaudio_2, 0x1401),//ISIAudio
	(soundspace_musicompress, 0x1500),//AT&T Labs, Inc.
	(mpeg_adts_aac, 0x1600),//Microsoft Corporation
	(mpeg_raw_aac, 0x1601),//Microsoft Corporation
	(mpeg_loas, 0x1602),//Microsoft Corporation (MPEG-4 Audio Transport Streams (LOAS/LATM)
	(nokia_mpeg_adts_aac, 0x1608),//Microsoft Corporation
	(nokia_mpeg_raw_aac, 0x1609),//Microsoft Corporation
	(vodafone_mpeg_adts_aac, 0x160A),//Microsoft Corporation
	(vodafone_mpeg_raw_aac, 0x160B),//Microsoft Corporation
	(mpeg_heaac, 0x1610),//Microsoft Corporation (MPEG-2 AAC or MPEG-4 HE-AAC v1/v2 streams with any payload (ADTS, ADIF, LOAS/LATM, RAW). Format block includes MP4 AudioSpecificConfig() -- see HEAACWAVEFORMAT below
	(voxware_rt24_speech, 0x181C),//Voxware Inc.
	(sonicfoundry_lossless, 0x1971),//Sonic Foundry
	(innings_telecom_adpcm, 0x1979),//Innings Telecom Inc.
	(lucent_sx8300p, 0x1C07),//Lucent Technologies
	(lucent_sx5363s, 0x1C0C),//Lucent Technologies
	(cuseeme, 0x1F03),//CUSeeMe
	(ntcsoft_alf2cm_acm, 0x1FC4),//NTCSoft
	(dvm, 0x2000),//FAST Multimedia AG
	(dts2, 0x2001),
	(makeavis, 0x3313),
	(divio_mpeg4_aac, 0x4143),//Divio, Inc.
	(nokia_adaptive_multirate, 0x4201),//Nokia
	(divio_g726, 0x4243),//Divio, Inc.
	(lead_speech, 0x434C),//LEAD Technologies
	(lead_vorbis, 0x564C),//LEAD Technologies
	(wavpack_audio, 0x5756),//xiph.org
	(alac, 0x6C61),//Apple Lossless
	(ogg_vorbis_mode_1, 0x674F),//Ogg Vorbis
	(ogg_vorbis_mode_2, 0x6750),//Ogg Vorbis
	(ogg_vorbis_mode_3, 0x6751),//Ogg Vorbis
	(ogg_vorbis_mode_1_plus, 0x676F),//Ogg Vorbis
	(ogg_vorbis_mode_2_plus, 0x6770),//Ogg Vorbis
	(ogg_vorbis_mode_3_plus, 0x6771),//Ogg Vorbis
	(_3com_nbx, 0x7000),//3COM Corp.
	(opus, 0x704F),//Opus
	(faad_aac, 0x706D),
	(amr_nb, 0x7361),//AMR Narrowband
	(amr_wb, 0x7362),//AMR Wideband
	(amr_wp, 0x7363),//AMR Wideband Plus
	(gsm_amr_cbr, 0x7A21),//GSMA/3GPP
	(gsm_amr_vbr_sid, 0x7A22),//GSMA/3GPP
	(comverse_infosys_g723_1, 0xA100),//Comverse Infosys
	(comverse_infosys_avqsbc, 0xA101),//Comverse Infosys
	(comverse_infosys_sbc, 0xA102),//Comverse Infosys
	(symbol_g729_a, 0xA103),//Symbol Technologies
	(voiceage_amr_wb, 0xA104),//VoiceAge Corp.
	(ingenient_g726, 0xA105),//Ingenient Technologies, Inc.
	(mpeg4_aac, 0xA106),//ISO/MPEG-4
	(encore_g726, 0xA107),//Encore Software
	(zoll_asao, 0xA108),//ZOLL Medical Corp.
	(speex_voice, 0xA109),//xiph.org
	(vianix_masc, 0xA10A),//Vianix LLC
	(wm9_spectrum_analyzer, 0xA10B),//Microsoft
	(wmf_spectrum_anayzer, 0xA10C),//Microsoft
	(gsm_610, 0xA10D),
	(gsm_620, 0xA10E),
	(gsm_660, 0xA10F),
	(gsm_690, 0xA110),
	(gsm_adaptive_multirate_wb, 0xA111),
	(polycom_g722, 0xA112),//Polycom
	(polycom_g728, 0xA113),//Polycom
	(polycom_g729_a, 0xA114),//Polycom
	(polycom_siren, 0xA115),//Polycom
	(global_ip_ilbc, 0xA116),//Global IP
	(radiotime_time_shift_radio, 0xA117),//RadioTime
	(nice_aca, 0xA118),//Nice Systems
	(nice_adpcm, 0xA119),//Nice Systems
	(vocord_g721, 0xA11A),//Vocord Telecom
	(vocord_g726, 0xA11B),//Vocord Telecom
	(vocord_g722_1, 0xA11C),//Vocord Telecom
	(vocord_g728, 0xA11D),//Vocord Telecom
	(vocord_g729, 0xA11E),//Vocord Telecom
	(vocord_g729_a, 0xA11F),//Vocord Telecom
	(vocord_g723_1, 0xA120),//Vocord Telecom
	(vocord_lbc, 0xA121),//Vocord Telecom
	(nice_g728, 0xA122),//Nice Systems
	(frace_telecom_g729, 0xA123),//France Telecom
	(codian, 0xA124),//CODIAN
	(dolby_ac4, 0xAC40),//Dolby AC-4
	(flac, 0xF1AC),//flac.sourceforge.net
	(extensible, 0xFFFE),
	(development, 0xFFFF)
);
DEFINE_ENUM(AudioCaptureType, u8, input, output);
DEFINE_ENUM(AudioCaptureResult, u8, normal, silent);
struct AudioCapture
{
	NO_COPY_MOVE(AudioCapture)
private:
	IMMDeviceEnumerator* m_pEnumerator{};
	IMMDevice* m_pDevice{};
	IAudioClient* m_pAudioClient{};
	IAudioCaptureClient* m_pCaptureClient{};
	WAVEFORMATEX* m_pWaveFormatEx{};
public:
	INLINE AudioCapture()noexcept = default;
	~AudioCapture()noexcept
	{
		if (m_pWaveFormatEx)Combase::CoTaskMemFree(m_pWaveFormatEx);
		if (m_pCaptureClient) m_pCaptureClient->Release();
		if (m_pAudioClient) m_pAudioClient->Release();
		if (m_pDevice) m_pDevice->Release();
		if (m_pEnumerator)
		{
			m_pEnumerator->Release();
			Windows::COM::uninit();
		}
	}
	//INLINE void reset()noexcept
	//{
	//	this->~AudioCapture();
	//	m_pEnumerator = {};
	//	m_pDevice = {};
	//	m_pAudioClient = {};
	//	m_pCaptureClient = {};
	//	m_pWaveFormatEx = {};
	//}
	INLINE bool initialized()const noexcept
	{
		return m_pEnumerator
			&& m_pDevice
			&& m_pAudioClient
			&& m_pCaptureClient
			&& m_pWaveFormatEx;
	}

	INLINE const WAVEFORMATEX& format()const noexcept { return *m_pWaveFormatEx; }
	INLINE bool isF32()const noexcept
	{
		if (m_pWaveFormatEx->wFormatTag == UNDERLYING(WaveFormat::ieee_float))return true;
		if (m_pWaveFormatEx->wFormatTag == UNDERLYING(WaveFormat::extensible))
			return IsEqualGUID(reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(m_pWaveFormatEx)->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
		return false;
	}
	INLINE Opt<u64, 0_u64> sampleWindowTimeMs()const noexcept
	{
		u32 bufferFrameCount;
		if (FAILED(m_pAudioClient->GetBufferSize(&bufferFrameCount)))
			return {};
		return 1000_u64 * bufferFrameCount / m_pWaveFormatEx->nSamplesPerSec;
	}

	INLINE Opt<String16> endpointName() const noexcept
	{
		//DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);
		static constexpr PROPERTYKEY device_friendly_name{ .fmtid{.Data1{0xa45c254e},.Data2{0xdf1c},.Data3{0x4efd},.Data4{0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},.pid{14} };
		Opt<String16> result;
		if (!m_pDevice)
			return result;
		IPropertyStore* properties;
		if (FAILED(m_pDevice->OpenPropertyStore(STGM_READ, &properties)))
			return result;
		PROPVARIANT value;
		PropVariantInit(&value);
		if (SUCCEEDED(properties->GetValue(device_friendly_name, &value)) && value.vt == VT_LPWSTR && value.pwszVal)
			result.unchecked_emplace(StringView16{ value.pwszVal });
		Combase::PropVariantClear(&value);
		properties->Release();
		return result;
	}
	template<AudioCaptureType CaptureType>
	bool init()noexcept
	{
		if (!Windows::COM::init())
			return false;

		if (!Windows::COM::createInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_pEnumerator))
			return false;

		static constexpr EDataFlow dataFlow{ (CaptureType == AudioCaptureType::output) ? eRender : eCapture };
		if (FAILED(m_pEnumerator->GetDefaultAudioEndpoint(dataFlow, eMultimedia, &m_pDevice)))
			return false;

		if (FAILED(m_pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pAudioClient)))
			return false;

		if (FAILED(m_pAudioClient->GetMixFormat(&m_pWaveFormatEx)))
			return false;

		static constexpr u32 streamFlags{ (CaptureType == AudioCaptureType::output) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0 };
		if (FAILED(m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 1, 0, m_pWaveFormatEx, nullptr)))
			return false;

		if (FAILED(m_pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient)))
			return false;

		return true;
	}

	INLINE bool start()const noexcept { return SUCCEEDED(m_pAudioClient->Start()); }
	INLINE bool stop()const noexcept { return SUCCEEDED(m_pAudioClient->Stop()); }

	INLINE bool read($Sig<void(const u8*, const u32, const AudioCaptureResult)> auto&& action) noexcept
	{
		u8* pData;
		DWORD flags;
		u32 numFrames;
		for (;;)
		{
			const HRESULT hr{ m_pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr) };
			if (hr == AUDCLNT_S_BUFFER_EMPTY)return true;
			if (FAILED(hr)) [[unlikely]] return false;
			action(pData, numFrames, (flags & AUDCLNT_BUFFERFLAGS_SILENT) ? AudioCaptureResult::silent : AudioCaptureResult::normal);
			if (FAILED(m_pCaptureClient->ReleaseBuffer(numFrames))) [[unlikely]] return false;
		}
	}

	template<class Stream>
	friend Stream& operator<<(Stream& os, const AudioCapture& capture)noexcept
	{
		if (!capture.m_pWaveFormatEx)os << TS("Not initialized");
		else
		{
			os << TS(  "Name           = ") << capture.endpointName();
			os << TS("\nDuration       = ") << capture.sampleWindowTimeMs() << TS("ms");
			os << TS("\nIsF32          = ") << capture.isF32();
			os << TS("\nTag            = ") << static_cast<WaveFormat>(capture.m_pWaveFormatEx->wFormatTag);
			os << TS("\nChannels       = ") << capture.m_pWaveFormatEx->nChannels;
			os << TS("\nSamplesPerSec  = ") << capture.m_pWaveFormatEx->nSamplesPerSec;
			os << TS("\nAvgBytesPerSec = ") << capture.m_pWaveFormatEx->nAvgBytesPerSec;
			os << TS("\nBlockAlign     = ") << capture.m_pWaveFormatEx->nBlockAlign;
			os << TS("\nBitsPerSample  = ") << capture.m_pWaveFormatEx->wBitsPerSample;
			os << TS("\nbSize          = ") << capture.m_pWaveFormatEx->cbSize;
		}
		return os;
	}
};