//*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#* DTAPI.h *#*#*#*#*#*#*#*#*#* (C) 2000-2008 DekTec
//
// DTAPI: C++ API for DekTec's Digital-Video PCI Cards and USB-2 Adapters
//
// Main header file

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Change History -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// For a revision history of the DTAPI, please refer to the DTAPI documentation

//#pragma once
#ifndef __DTAPI_H
#define __DTAPI_H

#ifdef _USE_DTAPIDLL
	#ifdef DTAPIDLL_EXPORTS
	#define DTAPI_API __declspec(dllexport)
	#else
	#define DTAPI_API __declspec(dllimport)
	#endif
#else
	// Not using DLL, so define DTAPI_API as empty
	#define DTAPI_API 
#endif


//-.-.-.-.-.-.-.-.-.-.-.-.- Additional Libraries to be Linked In -.-.-.-.-.-.-.-.-.-.-.-.-
#if defined(WIN32) || defined(WIN64)
#ifndef _USE_DTAPIDLL
	#pragma comment(lib, "iphlpapi.lib")
	#pragma comment(lib, "ws2_32.lib")

	#ifndef _DTAPI_DISABLE_AUTO_LINK
		// Are we using multi-threaded-DLL or static runtime libraries?
		#ifdef _DLL
			// Link with DLL runtime version (/MD)
			#ifdef _DEBUG
				#pragma comment(lib, "DTAPIMDd.lib")	// Debug
				#pragma message("Automatically linking with DTAPIMDd.lib") 
			#else
				#pragma comment(lib, "DTAPIMD.lib")		// Release
				#pragma message("Automatically linking with DTAPIMD.lib") 
			#endif
		#else
			// Link to static runtime version (/MT)
			#ifdef _DEBUG
				#pragma comment(lib, "DTAPId.lib")		// Release
				#pragma message("Automatically linking with DTAPId.lib") 
			#else
				#pragma comment(lib, "DTAPI.lib")		// Release
				#pragma message("Automatically linking with DTAPI.lib") 
			#endif
		#endif
	#endif
#else
	#ifndef DTAPIDLL_EXPORTS
		// DLL import lib
		#pragma comment(lib, "DTAPIDLL.lib")	
		#pragma message("Automatically linking with DTAPIDLL.lib") 
	#else
		#pragma comment(lib, "iphlpapi.lib")
		#pragma comment(lib, "ws2_32.lib")
	#endif
#endif
#endif	// #if defined(WIN32) || defined(WIN64)

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DTAPI Version -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_VERSION_MAJOR		4
#define  DTAPI_VERSION_MINOR		6
#define  DTAPI_VERSION_BUGFIX		0
#define  DTAPI_VERSION_BUILD		130


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Includes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#if defined(WIN32) || defined(WIN64)
	// Windows specific includes
	#include <windows.h>			// For windows type HANDLE
#else
	// Linux specific includes	
#endif
#include <map>
#include <vector>


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Support Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

#if !defined(WIN32) && !defined(WIN64)
	typedef signed long long __int64;
#endif

namespace _XP
{
#if defined(WIN32) || defined(WIN64)
	typedef ::HANDLE HANDLE;
#else
	typedef int HANDLE;
#endif
}

// Use namespace if namespace usage is required
#ifdef _USE_NAMESPACE
namespace DtApi {
#endif

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Forward declarations -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
class DtLoopWorker;
class DtSdiUtility;
class DteDevice;

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Elementary Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
typedef unsigned int  DTAPI_RESULT;


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ HELPER STRUCTS +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtCmmbPars -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
// DtOutpChannel::SetModControl - Bandwidth
#define  DTAPI_CMMB_BW_2MHZ			0x00000000
#define  DTAPI_CMMB_BW_8MHZ			0x00000001

// CMMB parameters
struct DTAPI_API DtCmmbPars {
	int		m_Bandwidth;		// CMMB Bandwitdh
	int		m_TsRate;			// CMMB TS rate in bps
	int		m_TsPid;			// PID on which the CMMB stream is found
	int		m_AreaId;			// Area ID (0..127)
	int		m_TxId;				// Transmitter ID (0..127)
	
	// Member function
	DTAPI_RESULT  RetrieveTsRateFromTs(char* pBuffer, int NumBytes);

	// Operators
	bool  operator == (DtCmmbPars& Rhs);
	bool  operator != (DtCmmbPars& Rhs);

	// Constructor
	DtCmmbPars() :
		m_Bandwidth(DTAPI_CMMB_BW_8MHZ), m_TsRate(0), m_TsPid(-1), m_AreaId(0), m_TxId(0)
	{}
};

//-.-.-.-.-.-.-.-.-.-.-.- DtCmPars - Channel Modelling Parameters -.-.-.-.-.-.-.-.-.-.-.-.

// Maximum number of fading paths
#define DTAPI_CM_MAX_PATHS		32

// Path parameters
struct DtCmPath {
	enum Type {
		CONSTANT_DELAY,				// Constant phase
		CONSTANT_DOPPLER,			// Constant frequency shift
		RAYLEIGH_JAKES,				// Rayleigh fading with Jakes power spectral density
									// (mobile path model)
		RAYLEIGH_GAUSSIAN			// Rayleigh fading with Gaussian power spectral
									// density (ionospheric path model)
	};
    Type  m_Type;					// Type of path fading
    double  m_Attenuation;			// Attenuation in dB
    double  m_Delay;				// Delay in us
    double  m_Phase;				// Phase shift in radians for CONSTANT_DELAY paths
    double  m_Doppler;				// Doppler frequency in Hz
	// Constructor - Gives parameters a default value
	DtCmPath() :
		m_Type(CONSTANT_DELAY),
		m_Attenuation(0.0), m_Delay(0.0), m_Phase(0.0), m_Doppler(0.0)
	{}
	bool  operator == (DtCmPath& Rhs);
};

// Channel-modelling parameters
struct DtCmPars {
	bool  m_EnableAwgn;				// Enable noise injection
	double  m_Snr;					// Signal-to-noise ratio in dB
	bool  m_EnablePaths;			// Enable transmission paths simulation
	std::vector<DtCmPath>  m_Paths;	// Parameters per path
	// Constructor - Gives parameters a default value
	DtCmPars() : m_EnableAwgn(false), m_EnablePaths(false), m_Snr(6.0) {}
	// Operators
	bool  operator == (DtCmPars&);
	bool  operator != (DtCmPars& Rhs) {	return !(*this == Rhs);	}
};


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtDeviceDesc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure describing a device.
//
struct DtDeviceDesc {	
	int  m_Category;			// Device category
	__int64  m_Serial;			// Unique serial number of the device
	int  m_PciBusNumber;		// PCI-bus number
	int  m_SlotNumber;			// PCI-slot number
	int  m_UsbAddress;			// USB address
	int  m_TypeNumber;			// Device type number
	int  m_DeviceId;			// Device ID
	int  m_VendorId;			// Vendor ID
	int  m_SubsystemId;			// Subsystem
	int  m_SubVendorId;			// Subsystem Vendor ID
	int  m_NumHwFuncs;			// #Hardware funtions hosted by device
	int  m_FirmwareVersion;		// Firmware version
	int  m_FirmwareVariant;		// Firmware variant
	int  m_NumDtInpChan;		// Number of input channels 
	int  m_NumDtOutpChan;		// Number of output channels 
	int  m_NumPorts;			// Number of physical ports
	unsigned char  m_Ip[4];		// IP address (only valid for DTE-31xx devices)
	unsigned char  m_MacAddr[6];// MAC address (only valid for DTE-31xx devices)
};

// Device Categories
#define  DTAPI_CAT_PCI			0
#define  DTAPI_CAT_USB			1
#define  DTAPI_CAT_NDIS			2			// Legacy
#define  DTAPI_CAT_NW			2			// Not a real device category (intended for
											// getting driver version use)
#define  DTAPI_CAT_IP			3			

// Sub-channels
#define DTAPI_SUBCH_MAIN		0			// Default channel
#define DTAPI_SUBCH_ADC			1			// ADC channel

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtDvbT2Pars -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

#define  DTAPI_DVBT2_NUM_PLP_MAX	255		// Maximum number of PLPs
#define  DTAPI_DVBT2_NUM_RF_MAX		7		// Maximum number of RF output signals

// m_Bandwidth
#define  DTAPI_DVBT2_1_7MHZ			0		// 1.7 MHz
#define  DTAPI_DVBT2_5MHZ			1		// 5 MHz
#define  DTAPI_DVBT2_6MHZ			2		// 6 MHz
#define  DTAPI_DVBT2_7MHZ			3		// 7 MHz
#define  DTAPI_DVBT2_8MHZ			4		// 8 MHz
#define  DTAPI_DVBT2_10MHZ			5		// 10 MHz

// m_FftMode
#define  DTAPI_DVBT2_FFT_1K			0		// 1K FFT
#define  DTAPI_DVBT2_FFT_2K			1		// 2K FFT
#define  DTAPI_DVBT2_FFT_4K			2		// 4K FFT
#define  DTAPI_DVBT2_FFT_8K			3		// 8K FFT
#define  DTAPI_DVBT2_FFT_16K		4		// 16K FFT
#define  DTAPI_DVBT2_FFT_32K		5		// 32K FFT

// m_Miso
#define  DTAPI_DVBT2_MISO_OFF		0		// No MISO
#define  DTAPI_DVBT2_MISO_TX1		1		// TX1 only
#define  DTAPI_DVBT2_MISO_TX2		2		// TX2 only
#define  DTAPI_DVBT2_MISO_TX1TX2	3		// TX1+TX2

// m_Guard - Guard interval
// Warning: the codes are different from the corresponding L1 field
#define  DTAPI_DVBT2_GI_1_128		0		// 1/128
#define  DTAPI_DVBT2_GI_1_32		1		// 1/32
#define  DTAPI_DVBT2_GI_1_16		2		// 1/16
#define  DTAPI_DVBT2_GI_19_256		3		// 19/256
#define  DTAPI_DVBT2_GI_1_8			4		// 1/8
#define  DTAPI_DVBT2_GI_19_128		5		// 19/128
#define  DTAPI_DVBT2_GI_1_4			6		// 1/4

// m_Papr - PAPR - Peak to Average Power Reduction
#define  DTAPI_DVBT2_PAPR_NONE		0
#define  DTAPI_DVBT2_PAPR_ACE		1		// ACE - Active Constellation Extension
#define  DTAPI_DVBT2_PAPR_TR		2		// TR - Power reduction with reserved carriers
#define  DTAPI_DVBT2_PAPR_ACE_TR	3		// ACE and TR

// m_BwtExt - Bandwidth extension
#define  DTAPI_DVBT2_BWTEXT_OFF		0		// No bandwidth extension
#define  DTAPI_DVBT2_BWTEXT_ON		1		// Bandwidth extension on

// m_PilotPattern
#define  DTAPI_DVBT2_PP_1			1		// PP1
#define  DTAPI_DVBT2_PP_2			2		// PP2
#define  DTAPI_DVBT2_PP_3			3		// PP3
#define  DTAPI_DVBT2_PP_4			4		// PP4
#define  DTAPI_DVBT2_PP_5			5		// PP5
#define  DTAPI_DVBT2_PP_6			6		// PP6
#define  DTAPI_DVBT2_PP_7			7		// PP7
#define  DTAPI_DVBT2_PP_8			8		// PP8

// m_CodeRate - Code rate
#define  DTAPI_DVBT2_COD_1_2		0		// 1/2
#define  DTAPI_DVBT2_COD_3_5		1		// 3/5
#define  DTAPI_DVBT2_COD_2_3		2		// 2/3
#define  DTAPI_DVBT2_COD_3_4		3		// 3/4
#define  DTAPI_DVBT2_COD_4_5		4		// 4/5
#define  DTAPI_DVBT2_COD_5_6		5		// 5/6

// m_FefSignal - Type of signal generated during the FEF period
#define  DTAPI_DVBT2_FEF_ZERO		0		// Use zero I/Q samples during FEF
#define  DTAPI_DVBT2_FEF_1K_OFDM	1		// 1K OFDM symbols with 852 active carriers
											// containing BPSK symbols (same PRBS as the
											// T2 dummy cells, not reset between symbols)
#define  DTAPI_DVBT2_FEF_1K_OFDM_384 2		// 1K OFDM symbols with 384 active carriers 
											// containing BPSK symbols

// m_PlpConstel and m_L1Constel - Constellation
#define  DTAPI_DVBT2_BPSK			0		// BPSK
#define  DTAPI_DVBT2_QPSK			1		// QPSK
#define  DTAPI_DVBT2_QAM16			2		// 16-QAM
#define  DTAPI_DVBT2_QAM64			3		// 64-QAM
#define  DTAPI_DVBT2_QAM256			4		// 256-QAM

// m_Type - PLP type
#define  DTAPI_DVBT2_PLP_TYPE_COMM	0		// Common PLP
#define  DTAPI_DVBT2_PLP_TYPE_1		1		// PLP type 1
#define  DTAPI_DVBT2_PLP_TYPE_2		2		// PLP type 2

// m_FecType - PLP type
#define  DTAPI_DVBT2_LDPC_16K		0		// 16K LDPC
#define  DTAPI_DVBT2_LDPC_64K		1		// 64K LDPC

// m_TimeIlType - Time interleaver type
#define  DTAPI_DVBT2_IL_ONETOONE	0		// Interleaving frame in one T2 frame
#define  DTAPI_DVBT2_IL_MULTI		1		// Interleaving frame in multiple T2 frames

// DVB-T2 parameter info
struct DtDvbT2ParamInfo {
	int  m_TotalCellsPerFrame;	// Total number of cells per frame
	int  m_L1CellsPerFrame;		// Total number of cells per frame used for L1 signaling
								// L1 overhead = m_L1CellsPerFrame / m_TotalCellsPerFrame
	int  m_DummyCellsPerFrame;	// Total number of cells lost per frame; dummy cells
								// overhead = m_DummyCellsPerFrame / m_TotalCellsPerFrame
								// It is only computed for frame 0
};

// Parameters per PLP
struct DtDvbT2PlpPars {
	bool  m_Hem;				// HEM - High Efficiency Mode
	bool  m_Npd;				// NPD - Null-Packet Detection
	bool  m_IssyEnabled;		// ISSY Enabled yes/no
	int  m_Id;					// PLP_ID
	int  m_GroupId;				// PLP_GROUP_ID
	int  m_Type;				// PLP_TYPE - See DTAPI_DVBT2_PLP_TYPE_x
	int  m_CodeRate;			// PLP_COD - Code rate, see DTAPI_DVBT2_COD_x
	int  m_Modulation;			// PLP_MOD - See DTAPI_DVBT2_BPSK/...
	bool  m_Rotation;			// PLP_ROTATION - 0 or 1
	int  m_FecType;				// PLP_FEC_TYPE - 0=LDPC 16K, 1=LDPC 64K
	int  m_TimeIlLength;		// TIME_IL_LENGTH - 0..255
	int  m_TimeIlType;			// TIME_IL_TYPE - 0 or 1
	bool  m_InBandFlag;			// IN_BAND_FLAG - 0 or 1
	int  m_NumBlocks;			// Number of FEC blocks contained in one IL frame

	// Operations
	void  Init();				// Initialise to default values
};

// Compare modes
#define DTAPI_DVBT2_COMPA_ALL			0
#define DTAPI_DVBT2_COMPA_ESSENTIAL		1

// DVB-T2 modulation parameters
struct DtDvbT2Pars {
	int  m_Bandwidth;			// See DTAPI_DVBT2_8MHZ/...
	int  m_FftMode;				// See DTAPI_DVBT2_FFT_x
	int  m_Miso;				// See DTAPI_DVBT2_MISO_x
	int  m_GuardInterval;		// Guard interval, see DTAPI_DVBT2_GI_x
	int  m_Papr;				// See DTAPI_DVBT2_PAPR_x
	int  m_BwtExt;				// 0 or 1, bandwidth extension
	int  m_PilotPattern;		// 1 to 8
	int  m_NumT2Frames;			// T2 frames per super frame (1 ... 255)
	int  m_NumDataSyms;			// Symbols per frame
	int  m_L1Modulation;		// See DTAPI_DVBT2_BPSK/..., 0 ... 3
	bool  m_FefEnable;			// Enable FEF
	int  m_FefType;				// 0 ... 15
	int  m_FefLength;			// FEF length in T units
	int  m_FefS1;				// 2 <= m_FefS1 <= 7
	int  m_FefS2;				// 0 <= m_FefS2 <= 15 and (m_FefS2 & 1) != 0
	int  m_FefInterval;			// If FEF is enabled
								//  Requires: (m_NumT2Frames % m_FefInterval) == 0
	int  m_FefSignal;			// Selects the type of signal generated during the
								// FEF period (see DTAPI_DVBT2_FEF_XXX)
	int  m_CellId;
	int  m_NetworkId;
	int  m_T2SystemId;
	int  m_Frequency;			// Only used to fill the L1 post frequency field
	int  m_NumPlps;				// Number of PLPs
	DtDvbT2PlpPars  m_Plps[DTAPI_DVBT2_NUM_PLP_MAX];

	// Operations
	void  Init(void);
	DTAPI_RESULT  CheckValidity(void);
	DTAPI_RESULT  GetParamInfo(DtDvbT2ParamInfo&);
	DTAPI_RESULT  OptimisePlpNumBlocks(DtDvbT2ParamInfo&, int&);
	DTAPI_RESULT  OptimisePlpNumBlocks(DtDvbT2ParamInfo&, int&, int&);

	// Operators
	bool  operator == (DtDvbT2Pars& Rhs);
	bool  operator != (DtDvbT2Pars& Rhs);
	bool  IsEqual(DtDvbT2Pars& Rhs, int CompareMode=DTAPI_DVBT2_COMPA_ALL);
};


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtHwFuncDesc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
//
// Structure describing a hardware function.
//
struct DtHwFuncDesc {
	DtDeviceDesc  m_DvcDesc;		// Device descriptor
	int  m_ChanType;				// Channel type (OR-able)
	int  m_StreamType;				// Transport-Stream type
	int  m_Flags;					// Capability flags (OR-able)
	int  m_IndexOnDvc;				// Index of hardware function
	int  m_Port;					// Physical port number
	unsigned char  m_Ip[4];			// IP address (only valid for IP functions)
	unsigned char  m_MacAddr[6];	// MAC address (only valid for IP functions)
};

// Hardware Function - Channel types
#define  DTAPI_CHAN_DISABLED	0
#define  DTAPI_CHAN_OUTPUT		1
#define  DTAPI_TS_OUTPUT		1			// Obsolete, for backward compatibility
#define  DTAPI_CHAN_INPUT		2
#define  DTAPI_TS_INPUT			2			// Obsolete, for backward compatibility
#define  DTAPI_CHAN_DBLBUF		4
#define  DTAPI_CHAN_LOOPTHR		8

// Hardware Function - Stream types
#define  DTAPI_ASI_SDI			1
#define  DTAPI_TS_MOD			2
#define  DTAPI_TS_OVER_IP		3
#define  DTAPI_TS_SPI			4

// Hardware Function - Capabilities
#define  DTAPI_CAP_ASI			0x00000001	// ASI support
#define  DTAPI_CAP_BIDIR		0x00000002	// Bi-directional port
#define  DTAPI_CAP_DBLBUF		0x00000004	// Double-buffered output capability
#define  DTAPI_CAP_SDI			0x00000008	// SDI support
#define  DTAPI_CAP_FAILSAFE		0x00000010	// Fail-safe
#define  DTAPI_CAP_LOOPTHR		0x00000020	// Loop-through output capability
#define  DTAPI_CAP_TRPMODE		0x00000040	// Support for transparent mode
											// (DTAPI_RXMODE_STTRP)
#define  DTAPI_CAP_SDITIME		0x00000080	// Support for timestamping of SDI frames

// Capabilities - ASI/SDI and SPI
#define  DTAPI_CAP_IORATESEL	0x00000100	// TS rate clock selection
#define  DTAPI_CAP_IOCONFIG		0x00000200	// I/O standard and mode configuration
#define  DTAPI_CAP_IOCLKSEL		0x00000400	// I/O clock selection
#define  DTAPI_CAP_DVBRAW10B	0x00000800	// DVB 10 bit raw mode

// Capabilities - Modulation
#define  DTAPI_CAP_ATSC			0x00000100	// ATSC modulation
#define  DTAPI_CAP_CMMB			0x00000200	// CMMB modulation
#define  DTAPI_CAP_DTMB			0x00000400	// DTMB modulation
#define  DTAPI_CAP_DVBS			0x00000800	// DVB-S modulation
#define  DTAPI_CAP_DVBS2		0x00001000	// DVB-S2 modulation
#define  DTAPI_CAP_DVBT			0x00002000	// DVB-T modulation
#define  DTAPI_CAP_DVBT2		0x00004000	// DVB-T2 modulation
#define  DTAPI_CAP_IQ			0x00008000	// Direct I/Q samples
#define  DTAPI_CAP_ISDBT		0x00010000	// ISDBT modulation
#define  DTAPI_CAP_QAM_A		0x00020000	// QAM-A modulation
#define  DTAPI_CAP_QAM_B		0x00040000	// QAM-B modulation
#define  DTAPI_CAP_QAM_C		0x00080000	// QAM-C modulation
#define  DTAPI_CAP_VHF			0x00100000	// VHF-band 47-470MHz
#define  DTAPI_CAP_UHF			0x00200000	// UHF-band 400-862MHz
#define  DTAPI_CAP_LBAND		0x00400000	// L-band 950-2150MHz
#define  DTAPI_CAP_IF			0x00800000	// IF output
#define  DTAPI_CAP_DIGIQ		0x01000000	// Digital IQ output
#define  DTAPI_CAP_ADJLVL		0x02000000	// Adjustable output level
#define  DTAPI_CAP_DIVERSITY	0x04000000	// Support for diversity mode
#define  DTAPI_CAP_IF_ADC		0x08000000	// Support for access to downconverted signal
#define	 DTAPI_CAP_SHARED		0x10000000	// Support for shared antenna input
#define  DTAPI_CAP_SNR			0x20000000	// Support for setting SNR
#define  DTAPI_CAP_CM			0x40000000	// Channel modelling

// LEGACY
#define  DTAPI_CAP_8VSB		DTAPI_CAP_ATSC	// Replaced by DTAPI_CAP_ATSC
#define  DTAPI_CAP_OFDM		DTAPI_CAP_DVBT	// Replaced by DTAPI_CAP_DVBT
#define  DTAPI_CAP_QPSK		DTAPI_CAP_DVBS	// Replaced by DTAPI_CAP_DVBS

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- String Conversions Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-

// Device
// Device type number (e.g. "DTA-100", "DTA-102")
#define DTAPI_DVC2STR_TYPE_NMB			0	
// Device type number + location (e.g. "DTA-100 in slot 5");
#define DTAPI_DVC2STR_TYPE_AND_LOC		1

// Hardware Function
// Device type number (e.g. "DTA-100", "DTA-102")
#define DTAPI_HWF2STR_TYPE_NMB			0	
// Device type number + port (e.g. "DTA-124 port 1")
#define DTAPI_HWF2STR_TYPE_AND_PORT		1
// Device type number + location (e.g. "DTA-100 in slot 5");
#define DTAPI_HWF2STR_TYPE_AND_LOC		2
// Interface type (e.g. "DVB-ASI" or "DVB-C")
#define DTAPI_HWF2STR_ITF_TYPE			3
// Short version of interface type (e.g. "ASI" instead "DVB-ASI")
#define DTAPI_HWF2STR_ITF_TYPE_SHORT	4


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtIsdbtPars -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

// PID-to-layer mapping
#define  DTAPI_ISDBT_LAYER_A		1
#define  DTAPI_ISDBT_LAYER_B		2
#define  DTAPI_ISDBT_LAYER_C		4

// DtIsdbtPars.m_BType - Broadcast type
#define  DTAPI_ISDBT_BTYPE_TV		0			// 1/3/13-segment TV broadcast
#define  DTAPI_ISDBT_BTYPE_RAD1		1			// 1-segment radio broadcast
#define  DTAPI_ISDBT_BTYPE_RAD3		2			// 3-segment radio broadcast

// DtIsdbtPars.m_Guard - Guard interval
#define  DTAPI_ISDBT_GUARD_1_32		0
#define  DTAPI_ISDBT_GUARD_1_16		1
#define  DTAPI_ISDBT_GUARD_1_8		2
#define  DTAPI_ISDBT_GUARD_1_4		3

// DtIsdbtLayerPars.m_Modulation - Modulation type
#define  DTAPI_ISDBT_MOD_DQPSK		0
#define  DTAPI_ISDBT_MOD_QPSK		1
#define  DTAPI_ISDBT_MOD_QAM16		2
#define  DTAPI_ISDBT_MOD_QAM64		3

// DtIsdbtLayerPars.m_CodeRate - Code rate
#define  DTAPI_ISDBT_RATE_1_2		0
#define  DTAPI_ISDBT_RATE_2_3		1
#define  DTAPI_ISDBT_RATE_3_4		2
#define  DTAPI_ISDBT_RATE_5_6		3
#define  DTAPI_ISDBT_RATE_7_8		4

// DtOutpChannel::SetModControl - Initial Total Number of Segments
#define  DTAPI_ISDBT_SEGM_1			0x00000001
#define  DTAPI_ISDBT_SEGM_3			0x00000003
#define  DTAPI_ISDBT_SEGM_13		0x0000000D
#define  DTAPI_ISDBT_SEGM_MSK		0x0000000F

// DtOutpChannel::SetModControl - Bandwidth
#define  DTAPI_ISDBT_BW_5MHZ		0x00000010
#define  DTAPI_ISDBT_BW_6MHZ		0x00000020
#define  DTAPI_ISDBT_BW_7MHZ		0x00000030
#define  DTAPI_ISDBT_BW_8MHZ		0x00000040
#define  DTAPI_ISDBT_BW_MSK			0x000000F0

// DtOutpChannel::SetModControl - Sample Rate
#define  DTAPI_ISDBT_SRATE_1_1		0x00000100
#define  DTAPI_ISDBT_SRATE_1_2		0x00000200
#define  DTAPI_ISDBT_SRATE_1_4		0x00000300
#define  DTAPI_ISDBT_SRATE_1_8		0x00000400
#define  DTAPI_ISDBT_SRATE_MSK		0x00000F00

// DtOutpChannel::SetModControl - Sub Channel
#define  DTAPI_ISDBT_SUBCH_MSK		0x0003F000
#define  DTAPI_ISDBT_SUBCH_SHIFT	12

// Result codes for DtIsdbtPars::CheckValidity
#define  DTAPI_ISDBT_OK				0
#define	 DTAPI_ISDBT_E_BTYPE		1
#define  DTAPI_ISDBT_E_NSEGM		2
#define  DTAPI_ISDBT_E_PARTIAL		3
#define	 DTAPI_ISDBT_E_NOT_FILLED	4
#define	 DTAPI_ISDBT_E_SUBCHANNEL	5

// Parameters per hierarchical ISDB-T layer
struct DtIsdbtLayerPars {
    int  m_NumSegments;					// Number of segments
    int  m_Modulation;					// Modulation type
    int  m_CodeRate;					// Code rate
    int  m_TimeInterleave;				// Time interleaving
	// Derived:
	int  m_BitRate;						// Bits per second assuming 6 MHz channel
};

// ISDB-T parameters including per-layer parameters
struct DTAPI_API DtIsdbtPars {
	bool  m_DoMux;						// Hierarchical multiplexing yes/no
	bool  m_FilledOut;					// Members have been given a value
	int  m_BType;						// Broadcast type
	int  m_Mode;						// Transmission mode
	int  m_Guard;						// Guard interval
	int  m_PartialRx;					// Partal reception
	int  m_Emergency;					// Switch-on control for emergency broadcast
	int	 m_IipPid;						// PID used for multiplexing IIP packet
	DtIsdbtLayerPars  m_LayerPars[3];	// Layer-A/B/C parameters

#ifdef _MSC_VER
	// Disable C4251 with warning about our STL map below not being exported
	// See also: http://support.microsoft.com/kb/q168958/
	// NOTE: only use pragma with MS-compiler 
	#pragma warning (disable: 4251)
#endif // #ifdef _MSC_VER
	std::map<int, int>  m_Pid2Layer;	// PID-to-layer mapping
//#pragma warning (default: 4251)

	int  m_LayerOther;					// Other PIDs are mapped to this layer
	int	 m_ParXtra0;					// Extra parameters encoded like ParXtra0 in
										// SetModControl for DTAPI_MOD_ISDBT
	int  m_Virtual13Segm;				// Virtual 13-segment mode
	// Derived:
	bool  m_Valid;						// The parameter set is valid
	int	 m_TotalBitrate;				// Bitrate of entire stream

	// Member function
	DTAPI_RESULT  CheckValidity(int& ResultCode);
	DTAPI_RESULT  ComputeRates(void);
	DTAPI_RESULT  RetrieveParsFromTs(char* pBuffer, int NumBytes);
	static bool  BTypeCompat(int BType, int NumSegm);
	void  MakeConsistent();
	void  MakeNumSegmConsistent();

	// Total number of segment
	int  NumSegm() {
		return	  m_LayerPars[0].m_NumSegments + m_LayerPars[1].m_NumSegments
				+ m_LayerPars[2].m_NumSegments;
	}

	// Operators
	bool  operator == (DtIsdbtPars& Rhs);
	bool  operator != (DtIsdbtPars& Rhs);

	// Constructor
	DtIsdbtPars() :
		m_DoMux(false), m_FilledOut(false),
		m_ParXtra0(DTAPI_ISDBT_SEGM_13 | DTAPI_ISDBT_BW_6MHZ |
				   DTAPI_ISDBT_SRATE_1_1 | (22<<DTAPI_ISDBT_SUBCH_SHIFT)),
		m_Virtual13Segm(0), m_Valid(false)
	{}
};


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtTsIpPars -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure for storing TSoIP parameters
//
struct DtTsIpPars {
	unsigned char  m_Ip[4];			// IP Address
	unsigned short  m_Port;			// IP port
	unsigned char  m_SrcFltIp[4];	// Source filter: IP address
	unsigned short  m_SrcFltPort;	// Source filter: IP port 
	int  m_TimeToLive;				// Time-to-Live setting for IP TX
	int  m_NumTpPerIp;				// #TPs per IP packet
	int  m_Protocol;				// Protocol: UDP/RTP
	int  m_DiffServ;				// Differentiated Services
	int  m_FecMode;					// Error correction mode
	int  m_FecNumRows;				// �D� = #rows in FEC matrix
	int  m_FecNumCols;				// �L� = #columns in FEC matrix
	int  m_Flags;					// Optional controls/status flags
};


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtTsIpStat -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure for retrieving Ip statistics from drivers
// Statistics will be reset after read.
//
struct DtTsIpStat {
	unsigned int  m_TotNumIpPackets;
	unsigned int  m_LostIpPacketsBeforeFec;		// #Lost packets before FEC 
	unsigned int  m_LostIpPacketsAfterFec;		// #Lost packets after FEC
	unsigned int  m_LostIpPacketsBeforeSort;	// #Lost packets before RTP sorting
	unsigned int  m_LostIpPacketsAfterSort;		// #Lost packets after RTP sorting
};

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtRawIpHeader -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
//
// Header placed infront of all IP Packets when DTAPI_RXMODE_IPRAW mode is used
//
struct DtRawIpHeader {
	unsigned short  m_Tag;			// 0x44A0h = �D�160
	unsigned short  m_Length;		// IP Packet Length
	unsigned int  m_TimeStamp;		// Timestamp
};


//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtConstelPoint -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure representing a constellation point
//
struct DtConstelPoint
{
	int  m_X, m_Y;					// X and Y coordinates
};


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ MAIN DTAPI CLASSES +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtDevice +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent a DekTec Device.
//
class DTAPI_API  DtDevice {

	// Public attributes
public:
	DtDeviceDesc  m_DvcDesc;		// Device Descriptor
	DtHwFuncDesc*  m_pHwf;			// Hardware functions, filled after Attach

	// Public access functions
public:
	int  Category(void)			{ return m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_DvcDesc.m_FirmwareVersion; }
	int  TypeNumber(void)		{ return m_DvcDesc.m_TypeNumber; }
	bool IsAttached(void)		{ return m_Attached; }

	// Public member functions
public:
	DTAPI_RESULT  AttachToIpAddr(unsigned char Ip[4]);
	DTAPI_RESULT  AttachToSerial(__int64 SerialNumber);
	DTAPI_RESULT  AttachToSlot(int PciBusNumber, int SlotNumber);
	DTAPI_RESULT  AttachToType(int TypeNumber, int DeviceNo=0);
	DTAPI_RESULT  Detach(void);
	DTAPI_RESULT  FlashDisplay(int NumFlashes=5, int OnTime=100, int OffTime=100);
	DTAPI_RESULT  GetDescriptor(DtDeviceDesc& DvcDesc);
	DTAPI_RESULT  GetDeviceDriverVersion(int& DriverVersionMajor,
										 int& DriverVersionMinor);
	DTAPI_RESULT  GetDeviceDriverVersion(
								int& DriverVersionMajor, int& DriverVersionMinor,
								int& DriverVersionBugFix, int& DriverVersionBuild);
	DTAPI_RESULT  GetDisplayName(wchar_t* pName);
	DTAPI_RESULT  GetDisplayName(char* pName);
	DTAPI_RESULT  GetFirmwareVersion(int& FirmwareVersion);
	DTAPI_RESULT  GetIoConfig(int Port, int& IoConfig);
	DTAPI_RESULT  GetIoConfig(int Port, int& IoConfig, int& ParXtra);
	DTAPI_RESULT  GetIoConfig(int Port, int ConfigCode, int& IoConfig, int& ParXtra);
	DTAPI_RESULT  GetNwSpeed(int Port, bool& Enable, int& Speed);
	DTAPI_RESULT  GetRefClkCnt(int& RefClkCnt);
	DTAPI_RESULT  GetRefClkCnt(int& RefClkCnt, int& RefClkFreqHz);
	DTAPI_RESULT  GetRefClkFreq(int&  RefClkFreqHz);
	DTAPI_RESULT  GetUsbSpeed(int& UsbSpeed);
	DTAPI_RESULT  GetVcxoState(bool& Enable, int& Lock, int& VcxoClkFreqHz);
	DTAPI_RESULT  HwFuncScan(int NumEntries, int& NumEntriesResult,
							 DtHwFuncDesc* pHwFuncs);
	DTAPI_RESULT  I2CLock(int TimeOut);
	DTAPI_RESULT  I2CUnlock(void);
	DTAPI_RESULT  I2CRead(int DvcAddr, char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  I2CWrite(int DvcAddr, char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  LedControl(int LedControl);
	DTAPI_RESULT  SetDisplayName(wchar_t* pName);
	DTAPI_RESULT  SetDisplayName (char* pName);
	DTAPI_RESULT  SetIoConfig(int Port, int IoConfig, int ParXtra=-1);
	DTAPI_RESULT  SetIoConfig(int Port, int ConfigCode, int IoConfig, int ParXtra);
	DTAPI_RESULT  SetNwSpeed(int Port, bool Enable, int Speed);
	DTAPI_RESULT  UCodeLockUpload(void);
	DTAPI_RESULT  UCodeUnlockUpload(bool Uploaded);
	DTAPI_RESULT  UCodeWaitUploaded(int TimeOut);
	DTAPI_RESULT  VpdDelete(const char* pTag);
	DTAPI_RESULT  VpdDelete(const wchar_t* pTag);
	DTAPI_RESULT  VpdRead(const char* pTag, char* pVpdItem);
	DTAPI_RESULT  VpdRead(const wchar_t* pTag, wchar_t* pVpdItem);
	DTAPI_RESULT  VpdRead(const char*  pTag, char*  pVpdItem, int&  ItemSize);
	DTAPI_RESULT  VpdRead(const wchar_t*  pTag, char*  pVpdItem, int&   ItemSize);
	DTAPI_RESULT  VpdWrite(const char* pTag, char* pVpdItem);
	DTAPI_RESULT  VpdWrite(const wchar_t* pTag, wchar_t* pVpdItem);
	DTAPI_RESULT  VpdWrite(const char* pTag, char* pVpdItem, int ItemSize);
	DTAPI_RESULT  VpdWrite(const wchar_t* pTag, char* pVpdItem, int ItemSize);

	// Encapsulated data
public:
	bool  m_Attached;						// Attached-to-hardware flag
	_XP::HANDLE  m_hDriver;					// Handle to PCI-card's device driver
	volatile unsigned char*  m_pGenRegs;	// General registers
	DteDevice*	m_pDteDvc;					// Helper object for DTE devices 				

	// Constructor, destructor
public:
	DtDevice();
	virtual ~DtDevice();

	// Friends
	friend class  DtInpChannel;
	friend class  DtOutpChannel;
	friend class  TsInpChannel;
	friend class  TsOutpChannel;
};

// DtDevice - I/O ConfigCode
#define  DTAPI_IOCONFIG_IOCONFIG	1
#define  DTAPI_IOCONFIG_IOSTD		2
#define  DTAPI_IOCONFIG_IOCLKSEL	3
#define  DTAPI_IOCONFIG_IORATESEL	4
#define  DTAPI_IOCONFIG_IOMODE		5
 
 // DtDevice - I/O Configuration
#define  DTAPI_IOCONFIG_NOTSUP		0
#define  DTAPI_IOCONFIG_INPUT		1
#define  DTAPI_IOCONFIG_INPUT_APSK	11
#define  DTAPI_IOCONFIG_OUTPUT		2
#define  DTAPI_IOCONFIG_DBLBUF		3	
#define  DTAPI_IOCONFIG_IP			4
#define  DTAPI_IOCONFIG_FAILSAFE	5
#define  DTAPI_IOCONFIG_DIVERSITY	6
#define  DTAPI_IOCONFIG_LOOPTHR		7	
#define  DTAPI_IOCONFIG_SHARED		8
#define  DTAPI_IOCONFIG_GENREF		9
#define  DTAPI_IOCONFIG_GENLOCKED	10
#define  DTAPI_IOCONFIG_DISABLED	255

// DtDevice - I/O ConfigCode DTAPI_IOCONFIG_IOSTD
#define  DTAPI_IOSTD_DISABLED		1
#define  DTAPI_IOSTD_LVDS1			2
#define  DTAPI_IOSTD_LVDS2			3
#define  DTAPI_IOSTD_LVTTL			4

// DtDevice - I/O ConfigCode DTAPI_IOCONFIG_IOMODE
#define  DTAPI_IOMODE_STANDARD		1 // DVB-SPI (DVB-ASI), BT.656 27 MHz
#define  DTAPI_IOMODE_FIXEDCLK		2 // DVB fixed-clock mode, BT.656 variable clock
#define  DTAPI_IOMODE_SERIAL8B		3 // DVB 8-bit serial on parallel port
#define  DTAPI_IOMODE_SERIAL10B		4 // DVB 10-bit serial on parallel port

// DtDevice - I/O ConfigCode DTAPI_IOCONFIG_IOCLKSEL
#define  DTAPI_IOCLKSEL_INTCLK		1 // Default clock for this port
#define  DTAPI_IOCLKSEL_EXTCLK		2 // Clock from external clock input
#define  DTAPI_IOCLKSEL_PORT		3 // Clock from port <parXtra>

// DtDevice - I/O ConfigCode DTAPI_IOCONFIG_IORATESEL
#define  DTAPI_IORATESEL_INTCLK		1
#define  DTAPI_IORATESEL_EXTCLK		2

// Genlock ParXtra parameters
#define  DTAPI_GENLOCK_SDI625		0	// SDI 625-lines genlock mode
#define  DTAPI_GENLOCK_SDI525		1	// SDI 525-lines genlock mode

// Support flags used in DtInpChannel and DtOutpChannel
#define  DTAPI_SUPPORTS_TS			1
#define  DTAPI_SUPPORTS_SDI			2

// uCode upload states
#define  DTAPI_UCODE_NOT_LOADED		0
#define  DTAPI_UCODE_LOADING		1
#define  DTAPI_UCODE_LOADED			2

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtInpChannel +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent an input channel
//
class DTAPI_API  DtInpChannel {

	// Friends
	friend class  DtLoopWorker;
	friend class  Dta2135Opaque;
	friend class  Dta2136Opaque;
	friend class  Dta2137Opaque;
	friend class  Dtu235Opaque;

	// Public attributes
public:
	DtHwFuncDesc  m_HwFuncDesc;		// Hardware function descriptor

	// Public access functions
public:
	int  Category(void)			{ return m_HwFuncDesc.m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_HwFuncDesc.m_DvcDesc.m_FirmwareVersion; }
	int  GetChannelType(void)	{ return m_HwFuncDesc.m_ChanType; }
	int  GetIndexOnDevice(void)	{ return m_HwFuncDesc.m_IndexOnDvc; }
	bool IsAttached(void)		{ return (m_pOpaq != NULL); }
	int  TypeNumber(void)		{ return m_HwFuncDesc.m_DvcDesc.m_TypeNumber; }

public:
	DTAPI_RESULT  Attach(DtDevice* pDtDvc, int InpIndex=0, bool ProbeOnly=false);
	DTAPI_RESULT  AttachToPort(DtDevice* pDtDvc, int Port, bool ProbeOnly=false);
	DTAPI_RESULT  ClearFifo(int SubCh=DTAPI_SUBCH_MAIN);
	DTAPI_RESULT  ClearFlags(int Latched);
	DTAPI_RESULT  Detach(int DetachMode);
	DTAPI_RESULT  Equalise(int EqualiserSetting);
	DTAPI_RESULT  GetConstellationPoints(int NumPoints, DtConstelPoint* pPoint);
	DTAPI_RESULT  GetDescriptor(DtHwFuncDesc& HwFunDesc);
	DTAPI_RESULT  GetDemodControl(int& ModType, int& ParXtra0, int& ParXtra1,
								  int& ParXtra2);
	DTAPI_RESULT  GetDemodStatus(int& FLock, int& RLock, int& MER);
	DTAPI_RESULT  GetFecErrorCounters(int& NumUncorrectedErrors, int& NumCorrectedErrors,
									  int& NumBurstErrors);
	DTAPI_RESULT  GetFifoLoad(int& FifoLoad, int SubCh=DTAPI_SUBCH_MAIN);
	DTAPI_RESULT  GetFlags(int& Flags, int& Latched);
	DTAPI_RESULT  GetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  GetIpStat(DtTsIpStat* pTsIpStat);
	DTAPI_RESULT  GetMaxFifoSize(int& MaxFifoSize, int SubCh=DTAPI_SUBCH_MAIN);
	DTAPI_RESULT  GetReceiveByteCount(int& ByteCount);
	DTAPI_RESULT  GetRfLevel(int& LeveldBmV, int Bandwidth);
	DTAPI_RESULT  GetRxClkFreq(int& RxClkFreq);
	DTAPI_RESULT  GetRxControl(int& RxControl);
	DTAPI_RESULT  GetRxMode(int& RxMode);
	DTAPI_RESULT  GetStatistics(int& ViolCount);
	DTAPI_RESULT  GetStatistics(int Type, int& Statistic, bool UseCache=false);
	DTAPI_RESULT  GetStatistics(int Type, double& Statistic, bool UseCache=false);
	DTAPI_RESULT  GetStatus(int& PacketSize, int& NumInv, int& ClkDet,
							int& AsiLock, int& RateOk, int& AsiInv);
	/* DTA-2148
    DTAPI_RESULT  GetVidStd(bool& stdLock, int& vidStd, 
							int& rate, bool& interlaced, bool& M,
							int& words_per_actline, int& words_per_line, 
							int& lines_per_frame, int & actline_per_field);
    */
	DTAPI_RESULT  GetTargetId(int& Present, int& TargetId);
	DTAPI_RESULT  GetTsRateBps(int& TsRate);
	DTAPI_RESULT  GetTunerFrequency(__int64& FreqHz);
	DTAPI_RESULT  LedControl(int LedControl);
	DTAPI_RESULT  LnbEnable(bool Enable);
	DTAPI_RESULT  LnbEnableTone(bool Enable);
	DTAPI_RESULT  LnbSetVoltage(int Level);
	DTAPI_RESULT  LnbSendBurst(int BurstType);
	DTAPI_RESULT  LnbSendDiseqcMessage(const unsigned char* MsgOut, int NumBytesOut);
	DTAPI_RESULT  LnbSendDiseqcMessage(const unsigned char* MsgOut, int NumBytesOut, 
									   unsigned char* MsgIn, int& NumBytesIn);
	DTAPI_RESULT  PolarityControl(int Polarity);
	DTAPI_RESULT  Read(char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  Read(char* pBuffer, int NumBytesToRead, int Timeout);
	DTAPI_RESULT  ReadDirect(char* pBuffer,	int NumBytesToRead, int& NumBytesRead);
	DTAPI_RESULT  ReadFrame(unsigned int* pFrame, int& FrameSize, int TimeOut=-1);
	DTAPI_RESULT  ReadSubCh(char* pBuffer, int NumBytesToRead, int SubCh=DTAPI_SUBCH_MAIN);	
	DTAPI_RESULT  ReadUsingDma(char* pBuffer, int NumBytesToRead);
	DTAPI_RESULT  Reset(int ResetMode);
	DTAPI_RESULT  SetAntPower(int AntPower);
	DTAPI_RESULT  SetAdcSampleRate(int SampleRate);
	DTAPI_RESULT  SetDemodControl(int ModType, int ParXtra0, int ParXtra1,
								  int ParXtra2);
	DTAPI_RESULT  SetLoopBackMode(int Mode);
	DTAPI_RESULT  SetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  SetPower(int Power);
	DTAPI_RESULT  SetRxControl(int RxControl, int SubCh=DTAPI_SUBCH_MAIN);
	DTAPI_RESULT  SetRxMode(int RxMode);
	DTAPI_RESULT  SetTunerFrequency(__int64 FreqHz);
	DTAPI_RESULT  TuneChannel(int Band, int Channel, __int64& FreqHz);
	DTAPI_RESULT  WriteLoopBackData(char* pBuffer, int NumBytesToWrite, int SubCh=DTAPI_SUBCH_MAIN);
	
protected:
	bool  IsTsSupported()  { return ((m_Support & DTAPI_SUPPORTS_TS)!=0); }
	bool  IsSdiSupported() { return ((m_Support & DTAPI_SUPPORTS_SDI)!=0); }
	void  CloseDriverAndDeleteOpaque();

	// Encapsulated data
protected:
	void*  m_pOpaq;					// Opaque data; doubles as 'Attached' flag
	int  m_Support;					// Channel capabilities supported by this object
	
	// Constructor, destructor
public:
	DtInpChannel();
	virtual ~DtInpChannel();
};
typedef DtInpChannel Dtu234;		// For backwards compatibility

// Transport-Stream input channel is a specialised version of DtInpChannel
class DTAPI_API TsInpChannel : public DtInpChannel {
	// Constructor, destructor
public:
	TsInpChannel();
	virtual ~TsInpChannel();
};

// Feature not supported
#define  DTAPI_NOT_SUPPORTED		-1

// ASI Polarity-Control Status
#define  DTAPI_ASIINV_NORMAL		0
#define  DTAPI_ASIINV_INVERT		1

// ASI Input-Clock Lock
#define  DTAPI_ASI_NOLOCK			0
#define  DTAPI_ASI_INLOCK			1

// SDI Input-Clock Lock
#define  DTAPI_GENLOCK_NOLOCK		0
#define  DTAPI_GENLOCK_INLOCK		1

// Clock Detector
#define  DTAPI_CLKDET_FAIL			0
#define  DTAPI_CLKDET_OK			1

// Input Rate Ok
#define  DTAPI_INPRATE_LOW			0
#define  DTAPI_INPRATE_OK			1

// #Invalid bytes per packet
#define  DTAPI_NUMINV_NONE			0
#define  DTAPI_NUMINV_16			1
#define  DTAPI_NUMINV_OTHER			2

// Packet Size
#define  DTAPI_PCKSIZE_INV			0
#define  DTAPI_PCKSIZE_188			2
#define  DTAPI_PCKSIZE_204			3

// SDI Mode
#define  DTAPI_SDIMODE_INV			0
#define  DTAPI_SDIMODE_525			1
#define  DTAPI_SDIMODE_625			2

// Receive Control
#define  DTAPI_RXCTRL_IDLE			0
#define  DTAPI_RXCTRL_RCV			1

// Receive Mode
#define  DTAPI_RXMODE_ST188			0
#define  DTAPI_RXMODE_ST204			1
#define  DTAPI_RXMODE_STMP2			2
#define  DTAPI_RXMODE_STRAW			3
#define  DTAPI_RXMODE_IPRAW			5
#define  DTAPI_RXMODE_STTRP			7
#define  DTAPI_RX_TIMESTAMP			0x008	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI			0x010	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI_FULL		0x010	
#define  DTAPI_RXMODE_SDI_ACTVID	0x011	
#define  DTAPI_RXMODE_SDI_10B		0x020	// Flag: can be OR-ed with other modes
#define  DTAPI_RXMODE_SDI_HUFFMAN	0x080	// Flag: SDI Huffman compression on

// Demodulation status flags - FEC lock
#define  DTAPI_DEMOD_FECLOCK_FAIL	0
#define  DTAPI_DEMOD_FECLOCK_OK		1
// Demodulation status flags - Receiver lock
#define  DTAPI_DEMOD_RCVLOCK_FAIL	0
#define  DTAPI_DEMOD_RCVLOCK_OK		1	

// Channel bands
#define  DTAPI_BAND_BROADCAST_ONAIR	1
#define  DTAPI_BAND_FCC_CABLE		2
#define  DTAPI_BAND_IRC				3
#define  DTAPI_BAND_HRC				4

// RF level bandwith
#define DTAPI_RFLVL_CHANNEL			0
#define DTAPI_RFLVL_NARROWBAND		1

// Integer statistics
#define DTAPI_STAT_ASI_VIOLCNT		0x001		// ASI code violation count
#define DTAPI_STAT_BADPCKCNT		0x002		// Count of uncorrected packets
#define DTAPI_STAT_AGC1				0x003		// First (usually tuner) AGC value
#define DTAPI_STAT_AGC2				0x004		// Second (usually IF or baseband) AGC value

// Double statistics
#define DTAPI_STAT_BER_PREVIT		0x100		// Pre-Viterbi Bit error rate
#define DTAPI_STAT_BER_POSTVIT		0x101		// Post-Viterbi bit error rate
#define DTAPI_STAT_BER_PRERS		0x102		// Pre-Reed solomon bit error rate
#define DTAPI_STAT_SNR				0x102		// Signal-to-Noise ratio in dB
#define DTAPI_STAT_MER				0x103		// Modulation error rate in dB

// ADC sampling rates
#define DTAPI_ADCCLK_OFF			0			// Clock is off
#define DTAPI_ADCCLK_20M647			20647059	// 20.647059 MHz clock
#define DTAPI_ADCCLK_13M5			13500000	// 13.5 MHz clock
#define DTAPI_ADCCLK_27M			27000000	// 27.0 MHz clock

// LNB control values
#define DTAPI_LNB_13V				0			// LNB power 13V
#define DTAPI_LNB_18V				1			// LNB power 18V
#define DTAPI_LNB_14V				2			// LNB power 14V
#define DTAPI_LNB_19V				3			// LNB power 19V

// LNB burst types
#define DTAPI_LNB_BURST_A			0			// Burst A
#define DTAPI_LNB_BURST_B			1			// Burst B

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtOutpChannel +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Class to represent a transport-stream output channel.
//
class DTAPI_API DtOutpChannel {

	friend class  DtLoopWorker;			// Allow access to protected members
	friend class  Dtu215Opaque;			// Allow access to protected members

	// Public attributes
public:
	DtHwFuncDesc  m_HwFuncDesc;			// Hardware function descriptor

	// Public access functions
public:
	int  Category(void)			{ return m_HwFuncDesc.m_DvcDesc.m_Category; }
	int  FirmwareVersion(void)	{ return m_HwFuncDesc.m_DvcDesc.m_FirmwareVersion; }
	int  GetChannelType(void)	{ return m_HwFuncDesc.m_ChanType; }
	int  GetIndexOnDevice(void)	{ return m_HwFuncDesc.m_IndexOnDvc; }
	bool IsAttached(void)		{ return (m_pOpaq != NULL); }
	int  TypeNumber(void)		{ return m_HwFuncDesc.m_DvcDesc.m_TypeNumber; }

public:
	DTAPI_RESULT  Attach(DtDevice* pDtDvc, int OutpIndex=0, bool ProbeOnly=false);
	DTAPI_RESULT  AttachToPort(DtDevice* pDtDvc, int Port, bool ProbeOnly=false);
	DTAPI_RESULT  ClearFifo(void);
	DTAPI_RESULT  ClearFlags(int Latched);
	DTAPI_RESULT  Detach(int DetachMode);
	DTAPI_RESULT  GetDescriptor(DtHwFuncDesc& HwFunDesc);
	DTAPI_RESULT  GetExtClkFreq(int& ExtClkFreq);
	DTAPI_RESULT  GetFailsafeAlive(bool& Alive);
	DTAPI_RESULT  GetFailsafeConfig(bool& Enable, int& Timeout);
	DTAPI_RESULT  GetFifoLoad(int& FifoLoad);
	DTAPI_RESULT  GetFifoSize(int& FifoSize);
	DTAPI_RESULT  GetFifoSizeMax(int& FifoSizeMax);
	DTAPI_RESULT  GetFifoSizeTyp(int& FifoSizeTyp);
	DTAPI_RESULT  GetFlags(int& Status, int& Latched);
	DTAPI_RESULT  GetMaxFifoSize(int& MaxFifoSize);
	DTAPI_RESULT  GetModControl(int& ModType, int& CodeRate, int& ParXtra1, int& ParXtra2);
	DTAPI_RESULT  GetNcoPhaseIncr(int& PhaseIncr);
	DTAPI_RESULT  GetNcoPhaseIncr(int& PhaseIncr,int& PhaseMod);
	DTAPI_RESULT  GetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  GetOutputLevel(int& LeveldBm);
	DTAPI_RESULT  GetRfControl(__int64& RfFreq, int& LockStatus);
	DTAPI_RESULT  GetTargetId(int& Present, int& TargetId);
	DTAPI_RESULT  GetTransmitByteCount(int& ByteCount);
	DTAPI_RESULT  GetTsRateBps(int& ClockGenMode, int& TsRate);
	DTAPI_RESULT  GetTsRateBps(int& TsRate);
	DTAPI_RESULT  GetTxControl(int& TxControl);
	DTAPI_RESULT  GetTxMode(int& TxPacketMode, int& TxStuffMode);
	DTAPI_RESULT  LedControl(int LedControl);
	DTAPI_RESULT  ReadLoopBackData(char* pBuffer, int BytesToRead);
	DTAPI_RESULT  Reset(int ResetMode);
	DTAPI_RESULT  SetChannelModelling(bool CmEnable, DtCmPars& CmPars);
	DTAPI_RESULT  SetFailsafeConfig(bool Enable, int Timeout = 0);
	DTAPI_RESULT  SetFailsafeAlive();
	DTAPI_RESULT  SetFifoSize(int FifoSize);
	DTAPI_RESULT  SetFifoSizeMax(void);
	DTAPI_RESULT  SetFifoSizeTyp(void);
	DTAPI_RESULT  SetIpPars(DtTsIpPars* pTsIpPars);
	DTAPI_RESULT  SetIsdbtCaptFile(void* IsdbtFile);
	DTAPI_RESULT  SetLoopBackMode(int Mode);
	DTAPI_RESULT  SetModControl(DtIsdbtPars&);
	DTAPI_RESULT  SetModControl(DtCmmbPars&);
	DTAPI_RESULT  SetModControl(DtDvbT2Pars&);
	DTAPI_RESULT  SetModControl(int ModType, int CodeRate, int ParXtra1, int ParXtra2);
	DTAPI_RESULT  SetModControl(unsigned char*);
	DTAPI_RESULT  SetNcoPhaseIncr(int PhaseIncr,int PhaseMod=0);
	DTAPI_RESULT  SetOutputLevel(int LeveldBm);
	DTAPI_RESULT  SetPower(int Power);
	DTAPI_RESULT  SetRfControl(__int64 RfFreq);
	DTAPI_RESULT  SetRfMode(int RfMode);
	DTAPI_RESULT  SetSNR(int Mode, int SNR);
	DTAPI_RESULT  SetTsRateBps(int ClockGenMode, int TsRate);
	DTAPI_RESULT  SetTsRateBps(int TsRate);
	DTAPI_RESULT  SetTxControl(int TxControl);
	DTAPI_RESULT  SetTxMode(int TxPacketMode, int TxStuffMode);
	DTAPI_RESULT  SetTxPolarity(int TxPolarity);
	DTAPI_RESULT  Write(char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  WriteDirect(char* pBuffer, int NumBytesToWrite);
	DTAPI_RESULT  WriteUsingDma(char* pBuffer, int NumBytesToWrite);

protected:
	DTAPI_RESULT  SetModControl(int, int, int, int, void*, bool ForceInit=false);
	bool  IsTsSupported()  { return ((m_Support & DTAPI_SUPPORTS_TS)!=0); }
	bool  IsSdiSupported() { return ((m_Support & DTAPI_SUPPORTS_SDI)!=0); }
	void  CloseDriverAndDeleteOpaque();

	// Encapsulated data
protected:
	void*  m_pOpaq;				// Opaque data; doubles as 'Attached' flag
	int  m_Support;				// Channel capabilities supported by this object:
								// DTAPI_SUPPORTS_TS and/or DTAPI_SUPPORTS_SDI

	// Constructor, destructor
public:
	DtOutpChannel();
	virtual ~DtOutpChannel();
};

// Transport-Stream output channel is a specialised version of DtOutpChannel
class DTAPI_API TsOutpChannel : public DtOutpChannel {
	// Constructor, destructor
public:
	TsOutpChannel();
	virtual ~TsOutpChannel();
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtOutpChannel Constants -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Clock-Generator Modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXCLOCK_INTERNAL		0
#define  DTAPI_TXCLOCK_EXTERNAL		1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Detach Mode flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_INSTANT_DETACH		1
#define  DTAPI_ZERO_OUTPUT			2
#define  DTAPI_TRISTATE_OUTPUT		4
#define  DTAPI_WAIT_UNTIL_SENT		8

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Equaliser Settings -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_EQUALISER_OFF		0
#define  DTAPI_EQUALISER_ON			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- LED Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_LED_OFF				0
#define  DTAPI_LED_GREEN			1
#define  DTAPI_LED_RED				2
#define  DTAPI_LED_YELLOW			3
#define  DTAPI_LED_HARDWARE			4

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Loop-Back Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NO_LOOPBACK			0
#define  DTAPI_LOOPBACK_MODE		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Upconverter RF modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define DTAPI_UPCONV_NORMAL			0
#define DTAPI_UPCONV_MUTE			1
#define DTAPI_UPCONV_CW				2
#define DTAPI_UPCONV_CWI			3
#define DTAPI_UPCONV_CWQ			4

#define DTAPI_UPCONV_SPECINV		0x100	// Can be OR-ed with other RF modes


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_MOD_DVBS_QPSK		0		// Native DVB-S on DTA-107
#define  DTAPI_MOD_DVBS_BPSK		1
#define  DTAPI_MOD_QAM4				3
#define  DTAPI_MOD_QAM16			4
#define  DTAPI_MOD_QAM32			5
#define  DTAPI_MOD_QAM64			6
#define  DTAPI_MOD_QAM128			7
#define  DTAPI_MOD_QAM256			8
#define  DTAPI_MOD_DVBT				9	
#define  DTAPI_MOD_ATSC				10
#define  DTAPI_MOD_DVBT2			11
#define  DTAPI_MOD_ISDBT			12
#define  DTAPI_MOD_IQDIRECT			15
#define  DTAPI_MOD_DVBS_QPSK_SOFT	16		// Soft DVB-S for DTA-107S2
#define  DTAPI_MOD_DVBS2_QPSK		32
#define  DTAPI_MOD_DVBS2_8PSK		33
#define  DTAPI_MOD_DVBS2_16APSK		34
#define  DTAPI_MOD_DVBS2_32APSK		35
#define  DTAPI_MOD_DMBTH			48
#define  DTAPI_MOD_ADTBT			49
#define  DTAPI_MOD_CMMB				50
#define  DTAPI_MOD_T2MI				51

// LEGACY definitions
#define  DTAPI_MOD_QPSK				0
#define  DTAPI_MOD_BPSK				1
#define  DTAPI_MOD_OFDM				9
#define  DTAPI_MOD_DTMB				48

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (ATSC) -.-.-.-.-.-.-.-.-.-.-.-.-.-.

// ParXtra0
#define  DTAPI_MOD_ATSC_VSB8		0x00000000	// 8-VSB, 10.762MBd, 19.392Mbps
#define  DTAPI_MOD_ATSC_VSB16		0x00000001	// 16-VSB, 10.762MBd, 38.785Mbps
#define  DTAPI_MOD_ATSC_VSB_MSK		0x00000003	// Constellation mask

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DTMB) -.-.-.-.-.-.-.-.-.-.-.-.-.-.

// ParXtra0

// Bandwidth
#define  DTAPI_MOD_DTMB_5MHZ		0x00000001
#define  DTAPI_MOD_DTMB_6MHZ		0x00000002
#define  DTAPI_MOD_DTMB_7MHZ		0x00000003
#define  DTAPI_MOD_DTMB_8MHZ		0x00000004
#define  DTAPI_MOD_DTMB_BW_MSK		0x0000000F

// Code Rate
#define  DTAPI_MOD_DTMB_0_4			0x00000100	// 0.4
#define  DTAPI_MOD_DTMB_0_6			0x00000200	// 0.6
#define  DTAPI_MOD_DTMB_0_8			0x00000300	// 0.8
#define  DTAPI_MOD_DTMB_RATE_MSK	0x00000F00	// Mask

// Constellation
#define  DTAPI_MOD_DTMB_QAM4NR		0x00001000	// 4-QAM-NR
#define  DTAPI_MOD_DTMB_QAM4		0x00002000	// 4-QAM
#define  DTAPI_MOD_DTMB_QAM16		0x00003000	// 16-QAM
#define  DTAPI_MOD_DTMB_QAM32		0x00004000	// 32-QAM
#define  DTAPI_MOD_DTMB_QAM64		0x00005000	// 64-QAM
#define  DTAPI_MOD_DTMB_CO_MSK		0x0000F000	// Mask

// Frame Header Mode
#define  DTAPI_MOD_DTMB_PN420		0x00010000	// PN420
#define  DTAPI_MOD_DTMB_PN595		0x00020000	// PN595
#define  DTAPI_MOD_DTMB_PN945		0x00030000	// PN945
#define  DTAPI_MOD_DTMB_PN_MSK		0x000F0000	// Mask

// Interleaver Mode
#define  DTAPI_MOD_DTMB_IL_1		0x00100000	// Interleaver mode 1: B=54, M=240
#define  DTAPI_MOD_DTMB_IL_2		0x00200000	// Interleaver mode 2: B=54, M=720
#define  DTAPI_MOD_DTMB_IL_MSK		0x00F00000	// Mask

// Pilots
#define  DTAPI_MOD_DTMB_NO_PILOTS	0x01000000	// No pilots
#define  DTAPI_MOD_DTMB_PILOTS		0x02000000	// Pilots, C=1 only
#define  DTAPI_MOD_DTMB_PIL_MSK		0x0F000000	// Mask

// Use Frame Numbering
#define  DTAPI_MOD_DTMB_NO_FRM_NO	0x10000000	// No frame numbering
#define  DTAPI_MOD_DTMB_USE_FRM_NO	0x20000000	// Use frame numbers
#define  DTAPI_MOD_DTMB_UFRM_MSK	0xF0000000	// Mask

//.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DVB-S, DVB-S.2) -.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_MOD_1_2				0			// Code rate 1/2
#define  DTAPI_MOD_2_3				1			// Code rate 2/3
#define  DTAPI_MOD_3_4				2			// Code rate 3/4
#define  DTAPI_MOD_4_5				3			// Code rate 4/5
#define  DTAPI_MOD_5_6				4			// Code rate 5/6
#define  DTAPI_MOD_6_7				5			// Code rate 6/7
#define  DTAPI_MOD_7_8				6			// Code rate 7/8
#define  DTAPI_MOD_1_4				7			// Code rate 1/4
#define  DTAPI_MOD_1_3				8			// Code rate 1/3
#define  DTAPI_MOD_2_5				9			// Code rate 2/5
#define  DTAPI_MOD_3_5				10			// Code rate 3/5
#define  DTAPI_MOD_8_9				11			// Code rate 8/9
#define  DTAPI_MOD_9_10				12			// Code rate 9/10
#define  DTAPI_MOD_CR_UNK			15			// Unknown code rate

// ParXtra1
#define  DTAPI_MOD_SPECNONINV		0			// No spectrum inversion detected
#define  DTAPI_MOD_SPECINV			4			// Spectrum inversion detected
#define  DTAPI_MOD_SPECINV_MSK		4			// Mask for spectrum inversion field

//-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DVB-S.2) -.-.-.-.-.-.-.-.-.-.-.-.-.-.
// ParXtra1
#define  DTAPI_MOD_S2_NOPILOTS		0			// Pilots disabled
#define  DTAPI_MOD_S2_PILOTS		1			// Pilots enabled
#define  DTAPI_MOD_S2_PILOTS_MSK	1			// Mask for pilots field

#define  DTAPI_MOD_S2_SHORTFRM		2			// Short FECFRAME
#define  DTAPI_MOD_S2_LONGFRM		0			// Long FECFRAME
#define  DTAPI_MOD_S2_FRM_MSK		2			// Mask for FECFRAME field

//.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (DVB-T) -.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_MOD_DVBT_5MHZ		0x00000001
#define  DTAPI_MOD_DVBT_6MHZ		0x00000002
#define  DTAPI_MOD_DVBT_7MHZ		0x00000003
#define  DTAPI_MOD_DVBT_8MHZ		0x00000004
#define  DTAPI_MOD_DVBT_BW_UNK		0x0000000F	// Unknown bandwidth
#define  DTAPI_MOD_DVBT_BW_MSK		0x0000000F

#define  DTAPI_MOD_DVBT_QPSK		0x00000010
#define  DTAPI_MOD_DVBT_QAM16		0x00000020
#define  DTAPI_MOD_DVBT_QAM64		0x00000030
#define  DTAPI_MOD_DVBT_CO_UNK		0x000000F0	// Unknown constellation
#define  DTAPI_MOD_DVBT_CO_MSK		0x000000F0

#define  DTAPI_MOD_DVBT_G_1_32		0x00000100
#define  DTAPI_MOD_DVBT_G_1_16		0x00000200
#define  DTAPI_MOD_DVBT_G_1_8		0x00000300
#define  DTAPI_MOD_DVBT_G_1_4		0x00000400
#define  DTAPI_MOD_DVBT_GU_UNK		0x00000F00	// Unknown guard interval
#define  DTAPI_MOD_DVBT_GU_MSK		0x00000F00

#define  DTAPI_MOD_DVBT_INDEPTH		0x00001000
#define  DTAPI_MOD_DVBT_NATIVE		0x00002000
#define  DTAPI_MOD_DVBT_IL_MSK		0x0000F000

#define  DTAPI_MOD_DVBT_2K			0x00010000
#define  DTAPI_MOD_DVBT_4K			0x00020000
#define  DTAPI_MOD_DVBT_8K			0x00030000
#define  DTAPI_MOD_DVBT_MD_UNK		0x000F0000	// Unknown mode
#define  DTAPI_MOD_DVBT_MD_MSK		0x000F0000

#define  DTAPI_MOD_DVBT_S48_OFF		0x00000000
#define  DTAPI_MOD_DVBT_S48			0x00100000
#define  DTAPI_MOD_DVBT_S48_MSK		0x00100000

#define  DTAPI_MOD_DVBT_S49_OFF		0x00000000
#define  DTAPI_MOD_DVBT_S49			0x00200000
#define  DTAPI_MOD_DVBT_S49_MSK		0x00200000

#define  DTAPI_MOD_DVBT_ENA4849		0x00000000
#define  DTAPI_MOD_DVBT_DIS4849		0x00400000
#define  DTAPI_MOD_DVBT_4849_MSK	0x00400000

// DVB-T LEGACY
#define  DTAPI_MOD_OFDM_5MHZ		0x00000001
#define  DTAPI_MOD_OFDM_6MHZ		0x00000002
#define  DTAPI_MOD_OFDM_7MHZ		0x00000003
#define  DTAPI_MOD_OFDM_8MHZ		0x00000004
#define  DTAPI_MOD_OFDM_BW_MSK		0x0000000F
#define  DTAPI_MOD_OFDM_QPSK		0x00000010
#define  DTAPI_MOD_OFDM_QAM16		0x00000020
#define  DTAPI_MOD_OFDM_QAM64		0x00000030
#define  DTAPI_MOD_OFDM_CO_MSK		0x000000F0
#define  DTAPI_MOD_OFDM_G_1_32		0x00000100
#define  DTAPI_MOD_OFDM_G_1_16		0x00000200
#define  DTAPI_MOD_OFDM_G_1_8		0x00000300
#define  DTAPI_MOD_OFDM_G_1_4		0x00000400
#define  DTAPI_MOD_OFDM_GU_MSK		0x00000F00
#define  DTAPI_MOD_OFDM_INDEPTH		0x00001000
#define  DTAPI_MOD_OFDM_NATIVE		0x00002000
#define  DTAPI_MOD_OFDM_IL_MSK		0x0000F000
#define  DTAPI_MOD_OFDM_2K			0x00010000
#define  DTAPI_MOD_OFDM_4K			0x00020000
#define  DTAPI_MOD_OFDM_8K			0x00030000
#define  DTAPI_MOD_OFDM_MD_MSK		0x000F0000
#define  DTAPI_MOD_OFDM_S48_OFF		0x00000000
#define  DTAPI_MOD_OFDM_S48			0x00100000
#define  DTAPI_MOD_OFDM_S48_MSK		0x00100000
#define  DTAPI_MOD_OFDM_S49_OFF		0x00000000
#define  DTAPI_MOD_OFDM_S49			0x00200000
#define  DTAPI_MOD_OFDM_S49_MSK		0x00200000
#define  DTAPI_MOD_OFDM_ENA4849		0x00000000
#define  DTAPI_MOD_OFDM_DIS4849		0x00400000
#define  DTAPI_MOD_OFDM_4849_MSK	0x00400000

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (IQ) -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
// ParXtra0
#define  DTAPI_MOD_INTERPOL_OFDM	1			// Use OFDM interpolation
#define  DTAPI_MOD_INTERPOL_QAM		2			// Use QAM interpolation

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Modulation Parameters (QAM) -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_MOD_DVBC_SOFT		0x100		// Soft DVB-C flag

// J.83 Annex
#define  DTAPI_MOD_J83_MSK			0x0003
#define  DTAPI_MOD_J83_A			2			// J.83 annex A (DVB-C)
#define  DTAPI_MOD_J83_B			3			// J.83 annex B (�American QAM�)
#define  DTAPI_MOD_J83_C			1			// J.83 annex C (�Japanese QAM�)

// LEGACY definitions
#define  DTAPI_MOD_ROLLOFF_13		1			// Roll-off factor 13% (LEGACY)
#define  DTAPI_MOD_ROLLOFF_15		2			// Roll-off factor 15% (LEGACY)

// QAM-B interleaver mode
#define  DTAPI_MOD_QAMB_I128_J1D	0x1
#define  DTAPI_MOD_QAMB_I64_J2		0x3
#define  DTAPI_MOD_QAMB_I32_J4		0x5
#define  DTAPI_MOD_QAMB_I16_J8		0x7
#define  DTAPI_MOD_QAMB_I8_J16		0x9
#define  DTAPI_MOD_QAMB_I128_J1		0x0
#define  DTAPI_MOD_QAMB_I128_J2		0x2
#define  DTAPI_MOD_QAMB_I128_J3		0x4
#define  DTAPI_MOD_QAMB_I128_J4		0x6
#define  DTAPI_MOD_QAMB_I128_J5		0x8
#define  DTAPI_MOD_QAMB_I128_J6		0xA
#define  DTAPI_MOD_QAMB_I128_J7		0xC
#define  DTAPI_MOD_QAMB_I128_J8		0xE


//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Packet Transmit Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXMODE_188			0
#define  DTAPI_TXMODE_204			1
#define  DTAPI_TXMODE_ADD16			2
#define  DTAPI_TXMODE_RAW			3
#define  DTAPI_TXMODE_192			4		// Supported on DTA-102 Firmware Rev >= 2
#define  DTAPI_TXMODE_130			5		// Supported on DTA-102 Firmware Rev >= 7
#define  DTAPI_TXMODE_MIN16			6		// Supported on DTA-100 Firmware Rev >= 5
											// and          DTA-102 Firmware Rev >= 8
#define  DTAPI_TXMODE_IPRAW			7
#define  DTAPI_TXMODE_MASK			0x00F	// Mask for TxMode without burst flag
#define  DTAPI_TXMODE_BURST			0x010	// Can be OR-ed with one of 188/192/204/ADD16/RAW
#define	 DTAPI_TXMODE_SDI			0x040
#define  DTAPI_TXMODE_SDI_FULL		0x040
#define  DTAPI_TXMODE_SDI_ACTVID	0x041
#define	 DTAPI_TXMODE_SDI_10B		0x080	// Can be OR-ed with DTAPI_TXMODE_SDI
#define  DTAPI_TXMODE_SDI_525		0x000	// Can be OR-ed with DTAPI_TXMODE_SDI/10BIT
#define  DTAPI_TXMODE_SDI_625		0x100	// Can be OR-ed with DTAPI_TXMODE_SDI/10BIT
#define  DTAPI_TXMODE_SDI_HUFFMAN	0x200	// SDI huffman compression on

#define  DTAPI_TXSTUFF_MODE_OFF		0		// Stuff-mode is disabled
#define  DTAPI_TXSTUFF_MODE_ON		1		// Stuff-mode is enabled

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Transmit Polarity -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXPOL_NORMAL			0
#define  DTAPI_TXPOL_INVERTED		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Polarity Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_POLARITY_AUTO		0
#define  DTAPI_POLARITY_NORMAL		2
#define  DTAPI_POLARITY_INVERT		3

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Power Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_POWER_OFF			0
#define  DTAPI_POWER_ON				1

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Reset Mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_FIFO_RESET			0
#define  DTAPI_FULL_RESET			1
#define  DTAPI_ADC_FIFO_RESET		2		// DTA-2135 only, will result in a
											// DTAPI_FIFO_RESET for the ADC port

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Status Flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_RX_DMA_PENDING		0x0001
#define  DTAPI_RX_FIFO_OVF			0x0002
#define  DTAPI_RX_SYNC_ERR			0x0004
#define  DTAPI_RX_RATE_OVF			0x0008
#define  DTAPI_RX_TARGET_ERR		0x0010
#define  DTAPI_ADC_FIFO_OVF			0x0020
#define  DTAPI_RX_LINK_ERR			0x0040
#define  DTAPI_RX_DATA_ERR			0x0080

#define  DTAPI_TX_DMA_PENDING		0x0001
#define  DTAPI_TX_FIFO_UFL			0x0002
#define  DTAPI_TX_SYNC_ERR			0x0004
#define  DTAPI_TX_READBACK_ERR		0x0008
#define  DTAPI_TX_TARGET_ERR		0x0010
#define  DTAPI_TX_MUX_OVF			0x0020
#define  DTAPI_TX_LINK_ERR			0x0040
#define  DTAPI_TX_DATA_ERR			0x0080
#define  DTAPI_TX_CPU_UFL			0x0100

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Target Adapter Present -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NO_CONNECTION		0
#define  DTAPI_DVB_SPI_SINK			1		// For output channels
#define  DTAPI_DVB_SPI_SOURCE		1		// For input channels
#define  DTAPI_TARGET_PRESENT		2
#define  DTAPI_TARGET_UNKNOWN		3

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Transmit Control -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define  DTAPI_TXCTRL_IDLE			0
#define  DTAPI_TXCTRL_HOLD			1
#define  DTAPI_TXCTRL_SEND			3

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- RF PLL lock status -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_RFPLL_NO_LOCK		0
#define  DTAPI_RFPLL_LOCK			1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- USB-Speed Modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_USB_FULL_SPEED		0
#define  DTAPI_USB_HIGH_SPEED		1

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- SDRAM sizes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_SDRAM_SIZE_8MB		0
#define  DTAPI_SDRAM_SIZE_16MB		1
#define  DTAPI_SDRAM_SIZE_32MB		2
#define  DTAPI_SDRAM_SIZE_64MB		3

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Noise modes -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NOISE_DISABLED		0		// No noise generation
#define  DTAPI_NOISE_WNG_HW			1		// White noise generator (hardware)


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtLoop +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//
// DtLoop class represents an object that provides functionality to loop MPEG-2
// transport-streams from a DtInpChannel to a DtOutpChannel object
//
class DTAPI_API DtLoop {

	friend class DtLoopWorker;

	// Public access functions
public:
	bool  IsStarted();

public:
	DTAPI_RESULT  AttachToInput(DtInpChannel* pDtInp);
	DTAPI_RESULT  AttachToOutput(DtOutpChannel* pDtOutp);
	DTAPI_RESULT  Detach();
	DTAPI_RESULT  DetachFromInput();
	DTAPI_RESULT  DetachFromOutput();
	DTAPI_RESULT  SetStuffingMode(int Mode, int TsRate);
	DTAPI_RESULT  Start(bool Start=true);

	// Internal methods
protected:

	// Encapsulated data
protected:
	DtLoopWorker*  m_pWorker;	// Internal worker class
	
	// Constructor, destructor
public:
	DtLoop();
	virtual ~DtLoop();
};


//-.-.-.-.-.-.-.-.-.-.-.-.-.- SDI table-of-content entry types -.-.-.-.-.-.-.-.-.-.-.-.-.-

#define DTAPI_SDI_TOC_ENTRY_UNKNOWN		0
#define DTAPI_SDI_TOC_ENTRY_ACTVID		1
#define DTAPI_SDI_TOC_ENTRY_HANC		2
#define DTAPI_SDI_TOC_ENTRY_VANC		3

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtSdiTocEntry +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
class DTAPI_API DtSdiTocEntry
{
	friend class DtSdiUtility;

	// Public access functions
public:

	inline int  AncDataBlockNum() const 
	{ 
		if ( AncType() != 1 )	return -1;
		else					return m_SdidOrDbn;
	}
	inline int  AncDataId() const { return m_Did; }
	inline int  AncNumUserWords() const { return m_NumUserWords; }
	inline int  AncSecDataId() const
	{ 
		if ( AncType() != 2 )	return -1;
		else					return m_SdidOrDbn;
	}
	inline int  AncType() const { return m_AncType; }

	inline int  Field() const { return m_Field; }
	inline int  Line() const { return m_Line; }
	inline int  NumSymbols() const { return m_NumSymbols; }
	inline int  StartOffset() const { return m_StartOffset; }
	inline int  TocType() const { return m_TocType; }	

	// Encapsulated data
protected:
	int  m_TocType;				// Type of TOC entry
	int  m_Line;				// Line number where data is located
	int  m_Field;				// Field in where data is located
	int  m_StartOffset;			// Symbol offset, relative to start of line, to first data
	int  m_NumSymbols;			// Number of symbols

	// Following members are only valid if TOC type is DTAPI_SDI_TOC_ENTRY_HANC or 
	// DTAPI_SDI_TOC_ENTRY_VANC
	int  m_AncType;				// Ancillery data packet type (DTAPI_SDI_ANC_TYPE1 or
								// DTAPI_SDI_ANC_TYPE2)
	int  m_Did;					// Ancillary packet Data ID
	int  m_SdidOrDbn;			// Ancillary packet Data Block Number (Type 1 Packet)
								// or Secondary Data ID (Type 2 packet)
	int  m_NumUserWords;		// Number of ancillary data packet user words

	// Constructor, destructor
public:
	DtSdiTocEntry() : m_TocType(DTAPI_SDI_TOC_ENTRY_UNKNOWN), m_Line(0), m_Field(0),
					  m_StartOffset(0), m_NumSymbols(0) {}
	virtual ~DtSdiTocEntry() {};
};

//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ DtSdi +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// The DtSdi class provides helper functions for processing SDI data
// 
class DTAPI_API DtSdi
{
	friend class DtSdiUtility;

	// Public access functions
public:
	DTAPI_RESULT  ConvertFrame(unsigned int* pInFrame, int& InFrameSize, int InFrameFormat,
							   unsigned int* pOutFrame, int& OutFrameSize,
							   int OutFrameFormat);
	DTAPI_RESULT  CreateBlackFrame(unsigned int* pFrame, int& FrameSize, int FrameFormat);
	DTAPI_RESULT  GetActiveVideo(const DtSdiTocEntry& TocEntry, unsigned short* pVideo,
								 int& NumSamples);
	DTAPI_RESULT  GetActiveVideo(unsigned short* pVideo, int& NumSamples, int Field,
								 int Stride=-1);
	DTAPI_RESULT  GetAncillaryData(const DtSdiTocEntry& TocEntry, unsigned short* pData,
								   int& NumSamples);
	DTAPI_RESULT  GetAudio(int AudioGroup, int& Channel, unsigned short* pAudio,
						   int& NumSamples);
	DTAPI_RESULT  GetTableOfContents(const DtSdiTocEntry** ppToc, int& NumTocEntries);
	DTAPI_RESULT  ParseFrame(const unsigned int* pFrame, int FrameSize, int FrameFormat,
							 int ParseFlags, const DtSdiTocEntry** ppToc,
							 int& NumTocEntries);	
	// Internal methods
protected:

	// Encapsulated data
protected:
	DtSdiUtility*  m_pSdiUtil;	// Internal utility class
	
	// Constructor, destructor
public:
	DtSdi();
	virtual ~DtSdi();
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtSdi constants -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Ancillary data packet types -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define DTAPI_SDI_ANC_TYPE1			1		// Type 1 packet
#define DTAPI_SDI_ANC_TYPE2			2		// Type 2 packet

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Parse flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define DTAPI_SDI_PARSE_ACTVID		0x0001	// Parse active video
#define DTAPI_SDI_PARSE_HBLANK		0x0002	// Parse horizontal blanking
#define DTAPI_SDI_PARSE_VBLANK		0x0004	// Parse vertical blanking

#define DTAPI_SDI_PARSE_BLANK		(  DTAPI_SDI_PARSE_HBLANK | DTAPI_SDI_PARSE_VBLANK )
#define DTAPI_SDI_PARSE_ALL			(  DTAPI_SDI_PARSE_ACTVID | DTAPI_SDI_PARSE_BLANK )

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Field flags -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define DTAPI_SDI_FIELD1			1
#define DTAPI_SDI_FIELD2			2

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Audio groups -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
#define DTAPI_SDI_AUDIO_GROUP1		0x2FF
#define DTAPI_SDI_AUDIO_GROUP2		0x1FD
#define DTAPI_SDI_AUDIO_GROUP3		0x1FB
#define DTAPI_SDI_AUDIO_GROUP4		0x2F9

#define DTAPI_SDI_AUDIO_CHAN1		0x01
#define DTAPI_SDI_AUDIO_CHAN2		0x02
#define DTAPI_SDI_AUDIO_CHAN3		0x04
#define DTAPI_SDI_AUDIO_CHAN4		0x08

#define DTAPI_SDI_AUDIO_CH_PAIR1	(DTAPI_SDI_AUDIO_CHAN1 | DTAPI_SDI_AUDIO_CHAN2)
#define DTAPI_SDI_AUDIO_CH_PAIR2	(DTAPI_SDI_AUDIO_CHAN3 | DTAPI_SDI_AUDIO_CHAN4)

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Conversions format -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define DTAPI_SDI_FULL				0x01
#define DTAPI_SDI_ACTVID			0x02
#define DTAPI_SDI_HUFFMAN			0x04
#define DTAPI_SDI_625				0x08
#define DTAPI_SDI_525				0x10
#define DTAPI_SDI_8B				0x20
#define DTAPI_SDI_10B				0x40
#define DTAPI_SDI_16B				0x80

#define DTAPI_SDI_BIT_MASK			0xE0


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ Global DTAPI Functions +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

DTAPI_API DTAPI_RESULT  DtapiCheckDeviceDriverVersion(void);
DTAPI_API DTAPI_RESULT  DtapiCheckDeviceDriverVersion(int DeviceCategory);
DTAPI_API DTAPI_RESULT  DtapiDeviceScan(int NumEntries, int& NumEntriesResult,
										DtDeviceDesc* DvcDescArr, bool InclIpDvcs=false);
DTAPI_API DTAPI_RESULT  DtapiDtHwFuncDesc2String(DtHwFuncDesc* pHwFunc, int StringType, 
												 char* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiDtHwFuncDesc2String(DtHwFuncDesc* pHwFunc, int StringType, 
												 wchar_t* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiGetDeviceDriverVersion(int&, int&, int&, int&);
DTAPI_API DTAPI_RESULT  DtapiGetDeviceDriverVersion(int, int&, int&, int&, int&);
DTAPI_API DTAPI_RESULT  DtapiGetVersion(int& LibVersion, int& LibVersionMinor);
DTAPI_API DTAPI_RESULT  DtapiGetVersion(int& LibVersion, int& LibVersionMinor,
										int& LibVersionBugFix, int& LibVersionBuild);
DTAPI_API DTAPI_RESULT  DtapiHwFuncScan(int NumEntries, int& NumEntriesResult,
										DtHwFuncDesc* pHwFuncs, bool InclIpDvcs=false);
DTAPI_API DTAPI_RESULT  DtapiModPars2Bandwidth(int& ModBandwidth, int& TotalBandwidth,
									int ModType, int ParXtra0, int ParXtra1, int ParXtra2,
									DtIsdbtPars&, void* pModPars, int SymRate);
DTAPI_API DTAPI_RESULT  DtapiModPars2SymRate(int& SymRate, int ModType,
											 int ParXtra0, int ParXtra1, int ParXtra2,
											 int TsRate);
DTAPI_API DTAPI_RESULT  DtapiModPars2TsRate(int& TsRate, int ModType,
											int ParXtra0, int ParXtra1, int ParXtra2,
											int SymRate = -1);
DTAPI_API DTAPI_RESULT  DtapiModPars2TsRate(int&  TsRate, DtDvbT2Pars& T2Pars);

DTAPI_API DTAPI_RESULT  DtapiVoltage2Power(int dBmV, int& dBm, bool Is50Ohm=false);
DTAPI_API DTAPI_RESULT  DtapiPower2Voltage(int dBm, int& dBmV, bool Is50Ohm=false);
DTAPI_API const char*  DtapiResult2Str(DTAPI_RESULT DtapiResult);
DTAPI_API DTAPI_RESULT  DtapiDtDeviceDesc2String(DtDeviceDesc* pDvcDesc, int StringType, 
												 char* pString, int StringLength);
DTAPI_API DTAPI_RESULT  DtapiDtDeviceDesc2String(DtDeviceDesc* pDvcDesc, int StringType, 
												 wchar_t* pString, int StringLength);

// IP address conversion
DTAPI_API DTAPI_RESULT  DtapiInitDtTsIpParsFromIpString(DtTsIpPars& TsIpPars,
														const char* pDstIp,
														const char* pSrcIp);
DTAPI_API DTAPI_RESULT  DtapiInitDtTsIpParsFromIpString(DtTsIpPars& TsIpPars,
														const wchar_t* pDstIp,
														const wchar_t* pSrcIp);
DTAPI_API DTAPI_RESULT  DtapiIpAddr2Str(char* pStr, int StrLen, unsigned char* pIpAddr);
DTAPI_API DTAPI_RESULT  DtapiIpAddr2Str(wchar_t* pStr, int StrLen,
										unsigned char* pIpAddr);
DTAPI_API DTAPI_RESULT  DtapiStr2IpAddr(unsigned char* pIpAddr, const char* pStr);
DTAPI_API DTAPI_RESULT  DtapiStr2IpAddr(unsigned char* pIpAddr, const wchar_t* pStr);


//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ Return Codes +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// NOTE: ERROR CODES 0x1100-0x12FF ARE RESERVED FOR USE IN THE DTAPIplus

#define  DTAPI_OK					0
#define  DTAPI_OK_FAILSAFE			1
#define  DTAPI_E					0x1000
#define  DTAPI_E_ATTACHED			(DTAPI_E + 0)
#define  DTAPI_E_BUF_TOO_SMALL		(DTAPI_E + 1)
#define  DTAPI_E_DEV_DRIVER			(DTAPI_E + 2)
#define  DTAPI_E_EEPROM_FULL		(DTAPI_E + 3)
#define  DTAPI_E_EEPROM_READ		(DTAPI_E + 4)
#define  DTAPI_E_EEPROM_WRITE		(DTAPI_E + 5)
#define  DTAPI_E_EEPROM_FORMAT		(DTAPI_E + 6)
#define  DTAPI_E_FIFO_FULL			(DTAPI_E + 7)
#define  DTAPI_E_IN_USE				(DTAPI_E + 8)
#define  DTAPI_E_INVALID_BUF		(DTAPI_E + 9)
#define  DTAPI_E_INVALID_CGMODE		(DTAPI_E + 10)
#define  DTAPI_E_INVALID_FLAGS		(DTAPI_E + 11)
#define  DTAPI_E_INVALID_MODE		(DTAPI_E + 12)
#define  DTAPI_E_INVALID_RATE		(DTAPI_E + 13)
#define  DTAPI_E_INVALID_SIZE		(DTAPI_E + 14)
#define  DTAPI_E_KEYWORD			(DTAPI_E + 15)
#define  DTAPI_E_NO_DEVICE			(DTAPI_E + 16)
#define  DTAPI_E_NO_DTA_CARD		(DTAPI_E + 16)	// BACKWARD COMPATIBILITY
#define  DTAPI_E_NO_LOOPBACK		(DTAPI_E + 17)
#define  DTAPI_E_NO_SUCH_DEVICE		(DTAPI_E + 18)
#define  DTAPI_E_NO_SUCH_OUTPUT		(DTAPI_E + 19)
#define  DTAPI_E_NO_DT_OUTPUT		(DTAPI_E + 20)
#define  DTAPI_E_NO_TS_OUTPUT		(DTAPI_E + 20)
#define  DTAPI_E_NOT_ATTACHED		(DTAPI_E + 21)
#define  DTAPI_E_NOT_FOUND			(DTAPI_E + 22)
#define  DTAPI_E_NOT_SUPPORTED		(DTAPI_E + 23)
#define  DTAPI_E_PCICARD			(DTAPI_E + 24)	// BACKWARD COMPATIBILITY
#define  DTAPI_E_DEVICE				(DTAPI_E + 24)
#define  DTAPI_E_TOO_LONG			(DTAPI_E + 25)
#define  DTAPI_E_UNDERFLOW			(DTAPI_E + 26)
#define  DTAPI_E_NO_SUCH_INPUT		(DTAPI_E + 27)
#define  DTAPI_E_NO_DT_INPUT		(DTAPI_E + 28)
#define  DTAPI_E_NO_TS_INPUT		(DTAPI_E + 28)
#define  DTAPI_E_DRIVER_INCOMP		(DTAPI_E + 29)
#define  DTAPI_E_INTERNAL			(DTAPI_E + 30)
#define  DTAPI_E_OUT_OF_MEM			(DTAPI_E + 31)
#define  DTAPI_E_INVALID_J83ANNEX	(DTAPI_E + 32)
#define  DTAPI_E_INVALID_ROLLOFF	(DTAPI_E + 32)
#define  DTAPI_E_IDLE				(DTAPI_E + 33)
#define  DTAPI_E_INSUF_LOAD			(DTAPI_E + 34)
#define  DTAPI_E_INVALID_BANDWIDTH	(DTAPI_E + 35)
#define  DTAPI_E_INVALID_CONSTEL	(DTAPI_E + 36)
#define  DTAPI_E_INVALID_GUARD		(DTAPI_E + 37)
#define  DTAPI_E_INVALID_INTERLVNG	(DTAPI_E + 38)
#define  DTAPI_E_INVALID_TRANSMODE	(DTAPI_E + 39)
#define  DTAPI_E_INVALID_TSTYPE		(DTAPI_E + 40)
#define  DTAPI_E_NO_IPPARS			(DTAPI_E + 41)
#define  DTAPI_E_NO_TSRATE			(DTAPI_E + 42)
#define  DTAPI_E_NOT_IDLE			(DTAPI_E + 43)
#define  DTAPI_E_INVALID_ARG		(DTAPI_E + 44)
#define  DTAPI_E_NDIS_DRIVER		(DTAPI_E + 45)	// Legacy
#define  DTAPI_E_NW_DRIVER			(DTAPI_E + 45)
#define  DTAPI_E_DST_MAC_ADDR		(DTAPI_E + 46)
#define  DTAPI_E_NO_SUCH_PORT		(DTAPI_E + 47)
#define  DTAPI_E_WINSOCK			(DTAPI_E + 48)
#define  DTAPI_E_MULTICASTJOIN		(DTAPI_E + 49)
#define  DTAPI_E_EMBEDDED			(DTAPI_E + 50)
#define  DTAPI_E_LOCKED				(DTAPI_E + 51)
#define  DTAPI_E_NO_VALID_CALDATA	(DTAPI_E + 52)
#define  DTAPI_E_NO_LINK			(DTAPI_E + 53)
#define  DTAPI_E_INVALID_HEADER		(DTAPI_E + 54)
#define  DTAPI_E_INVALID_PARS		(DTAPI_E + 55)
#define  DTAPI_E_NOT_SDI_MODE		(DTAPI_E + 56)
#define  DTAPI_E_INCOMP_FRAME		(DTAPI_E + 57)
#define  DTAPI_E_UNSUP_CONV			(DTAPI_E + 58)
#define  DTAPI_E_OUTBUF_TOO_SMALL   (DTAPI_E + 59)
#define  DTAPI_E_CONFIG				(DTAPI_E + 60)
#define  DTAPI_E_TIMEOUT			(DTAPI_E + 61)
#define  DTAPI_E_INVALID_TIMEOUT	(DTAPI_E + 62)
#define  DTAPI_E_INVALID_FHMODE		(DTAPI_E + 63)
#define  DTAPI_E_INVALID_PILOTS		(DTAPI_E + 64)
#define  DTAPI_E_INVALID_USEFRAMENO	(DTAPI_E + 65)
#define  DTAPI_E_SYMRATE_REQD		(DTAPI_E + 66)
#define  DTAPI_E_NO_SYMRATE			(DTAPI_E + 67)
#define  DTAPI_E_INVALID_NUMSEGM	(DTAPI_E + 68)
#define  DTAPI_E_INVALID_NUMTAPS	(DTAPI_E + 69)
#define  DTAPI_E_COMMUNICATION		(DTAPI_E + 70)
#define  DTAPI_E_BIND				(DTAPI_E + 71)
#define  DTAPI_E_FRAME_INTERVAL		(DTAPI_E + 72)
#define  DTAPI_E_INVALID_BWT_EXT	(DTAPI_E + 73)
#define  DTAPI_E_INVALID_FFTMODE	(DTAPI_E + 74)
#define  DTAPI_E_INVALID_NUMDTSYM	(DTAPI_E + 75)
#define  DTAPI_E_INVALID_NUMT2FRM	(DTAPI_E + 76)
#define  DTAPI_E_INVALID_SUBCH		(DTAPI_E + 77)
#define  DTAPI_E_INVALID_TIME_IL	(DTAPI_E + 78)
#define  DTAPI_E_NUM_PLP			(DTAPI_E + 79)
#define  DTAPI_E_PLP_NUMBLOCKS		(DTAPI_E + 80)
#define  DTAPI_E_NUMPLPS_MUSTBE_1	(DTAPI_E + 81)
#define  DTAPI_E_INBAND				(DTAPI_E + 82)
#define  DTAPI_E_ISSY				(DTAPI_E + 83)
#define  DTAPI_E_OTHER_PLP_IN_BAND	(DTAPI_E + 84)
#define  DTAPI_E_CM_NUMPATHS		(DTAPI_E + 85)
#define  DTAPI_E_PILOT_PATTERN		(DTAPI_E + 86)
#define  DTAPI_E_SUBSLICES			(DTAPI_E + 87)
#define  DTAPI_E_NO_GENREF			(DTAPI_E + 88)
#define  DTAPI_E_TI_MEM_OVF			(DTAPI_E + 89)
#define  DTAPI_E_FEF				(DTAPI_E + 90)
#define  DTAPI_E_UNSUP_FORMAT		(DTAPI_E + 91)
#define  DTAPI_E_OUT_OF_SYNC		(DTAPI_E + 92)
#define  DTAPI_E_NO_FRAME			(DTAPI_E + 93)
#define  DTAPI_E_NO_SUCH_DATA		(DTAPI_E + 94)
#define  DTAPI_E_INVALID_TYPE		(DTAPI_E + 95)

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ IP Tx / Rx Constants +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- IP Tx / Rx Protocol -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_PROTO_UDP			0
#define  DTAPI_PROTO_RTP			1
#define  DTAPI_PROTO_AUTO			2
#define  DTAPI_PROTO_UNKN			2

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- IP Tx / Rx Fec mode -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_FEC_DISABLE			0
#define  DTAPI_FEC_2D				1		// FEC reconstruction
#define  DTAPI_FEC_2D_M1			1		// Mode1: FECdT = DVBdT + .5 * DVBdT
#define  DTAPI_FEC_2D_M2			2		// Mode2: FECdT = DVBdT


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+ General Network Constants +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Ethernet Speed -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
#define  DTAPI_NWSPEED_AUTO			0		// Set
#define  DTAPI_NWSPEED_NOLINK		0		// Get
#define  DTAPI_NWSPEED_10MB_HALF	1
#define  DTAPI_NWSPEED_10MB_FULL	2
#define  DTAPI_NWSPEED_100MB_HALF	3
#define  DTAPI_NWSPEED_100MB_FULL	4
#define  DTAPI_NWSPEED_1GB_MASTER	5
#define  DTAPI_NWSPEED_1GB_SLAVE	6


//=+=+=+=+=+=+=+=+=+=+=+=+=+=+ BACKWARD COMPATIBILITY SECTION +=+=+=+=+=+=+=+=+=+=+=+=+=+=

// Map old PciCard class on DtDevice
typedef class DtDevice  PciCard;

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- DtapiHwFunc -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
//
// Structure that describes a hardware function on a PCI card.
//
struct DtapiHwFunc {		
	int  m_nPciBusNumber;		// PCI-bus number
	int  m_nSlotNumber;			// PCI-slot number
	int  m_nTypeNumber;			// PCI-card type number
	int  m_nDeviceId;			// Device ID of PCI card
	int  m_nVendorId;			// Vendor ID of PCI card
	int  m_nSubsystemId;		// Subsystem ID of PCI card
	int  m_nSubVendorId;		// Subsystem Vendor ID of PCI card
	int  m_nHwFuncType;			// Hardware-function type
	int  m_nHwFuncFlags;		// Hardware-function flags
	int  m_nIndexOnCard;		// Index of hardware function
};

//-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Global functions -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
DTAPI_API DTAPI_RESULT  DtapiPciScan(int NumFuncEntries, int& NumFuncEntriesResult,
									 DtapiHwFunc* pHwFuncs);

//=+=+=+=+=+=+=+=+=+=+=+=+ END OF BACKWARD COMPATIBILITY SECTION +=+=+=+=+=+=+=+=+=+=+=+=+

// If namespace usage is required
#ifdef _USE_NAMESPACE
} // End of namespace
#endif

#endif //#ifndef __DTAPI_H
