#pragma once
#include "../Util/BasicTypes.h"
#include "../Util/Compiler.h"
#include "../Util/Concepts/Empty.h"
#include "../Util/DataStructures/Optional.h"
#include "../Util/DataStructures/Span.h"
#include "../Util/DataStructures/StringView.h"
#include "../Util/DataStructures/TString.h"
#include "../Util/EnumImpl.h"
#include "../Util/Inline.h"
#include "../Util/Reference.h"
#include "Structs/PeStructs.h"
#include "TibTebPeb.h"
#include "WinSock2_NoMinMax.h"
namespace Windows
{
	struct MappedPeView
	{
		static constexpr u8 Bits = BITNESS;
	private:
		const u8* m_base;
	public:

		template<class T>INLINE const T& get(usize offset)const noexcept { return *reinterpret_cast<const T*>(m_base + offset); }
		template<class T>INLINE const T* getPointer(usize offset)const noexcept { return reinterpret_cast<const T*>(m_base + offset); }

		INLINE MappedPeView(const void* base)noexcept :m_base{ reinterpret_cast<const u8*>(base) } { }

		INLINE const u8* pBase()const noexcept { return m_base; }

		INLINE const ImageDosHeader& dosHeader()const noexcept { return get<ImageDosHeader>(0); }
		INLINE const ImageNtHeader<Bits>& ntHeader()const noexcept { return get<ImageNtHeader<Bits>>(dosHeader().e_lfanew); }
		INLINE const ImageFileHeader& fileHeader()const noexcept { return ntHeader().FileHeader; }
		INLINE const ImageOptionalHeader<Bits>& optHeader()const noexcept { return ntHeader().OptionalHeader; }

		INLINE u32 size()const noexcept { return optHeader().SizeOfImage; }
		INLINE Span<const u8> bytes()const noexcept { return { m_base, size() }; }

		INLINE const ImageDataDirectory& dataDirectory(u8 index)const noexcept { return optHeader().DataDirectory[index]; }
		INLINE const ImageDataDirectory& dataDirectoryExport()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT); }
		INLINE const ImageDataDirectory& dataDirectoryImport()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT); }
		INLINE const ImageDataDirectory& dataDirectoryResource()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_RESOURCE); }
		INLINE const ImageDataDirectory& dataDirectoryException()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_EXCEPTION); }
		INLINE const ImageDataDirectory& dataDirectorySecurity()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_SECURITY); }
		INLINE const ImageDataDirectory& dataDirectoryReloc()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC); }
		INLINE const ImageDataDirectory& dataDirectoryDebug()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_DEBUG); }
		INLINE const ImageDataDirectory& dataDirectoryArchitecture()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_ARCHITECTURE); }
		INLINE const ImageDataDirectory& dataDirectoryGlobalptr()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_GLOBALPTR); }
		INLINE const ImageDataDirectory& dataDirectoryTls()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_TLS); }
		INLINE const ImageDataDirectory& dataDirectoryLoadConfig()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG); }
		INLINE const ImageDataDirectory& dataDirectoryBoundImport()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT); }
		INLINE const ImageDataDirectory& dataDirectoryIat()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_IAT); }
		INLINE const ImageDataDirectory& dataDirectoryDelayImport()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT); }
		INLINE const ImageDataDirectory& dataDirectoryComDescriptor()const noexcept { return dataDirectory(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR); }
		INLINE const ImageDataDirectory& dataDirectoryUnknown()const noexcept { return dataDirectory(IMAGE_NUMBEROF_DIRECTORY_ENTRIES - 1); }

		INLINE const ImageExportDirectory& exportDir()const noexcept { return get<ImageExportDirectory>(dataDirectoryExport().VirtualAddress); }
		INLINE const ImageImportDescriptor& importDescriptorFirst()const noexcept { return get<ImageImportDescriptor>(optHeader().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress); }

		template<class T, u8 type>
		INLINE Opt<Ref<const T>> directory()const noexcept
		{
			if (dataDirectory(type).Size == 0)
				return {};
			return *reinterpret_cast<const T*>(m_base + dataDirectory(type).VirtualAddress);
		}

		INLINE Opt<Ref<const ImageExportDirectory>> exportDirectory()const noexcept { return directory<ImageExportDirectory, IMAGE_DIRECTORY_ENTRY_EXPORT>(); }
		INLINE Opt<Ref<const ImageDebugDirectory>> debugDirectory()const noexcept { return directory<ImageDebugDirectory, IMAGE_DIRECTORY_ENTRY_DEBUG>(); }
		INLINE Opt<Ref<const ImageResourceDirectory>> resourceDirectory()const noexcept { return directory<ImageResourceDirectory, IMAGE_DIRECTORY_ENTRY_RESOURCE>(); }
		INLINE Opt<Ref<const ImageLoadConfigDirectory<BITNESS>>> loadConfigDirectory()const noexcept { return directory<ImageLoadConfigDirectory<BITNESS>, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG>(); }

		INLINE Span<const ImageImportDescriptor> importDescriptors()const noexcept
		{
			if (optHeader().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
				return {};
			const ImageImportDescriptor* pBegin{ &importDescriptorFirst() };
			const ImageImportDescriptor* pEnd{ pBegin };
			while (pEnd->Name)++pEnd;
			return { pBegin, pEnd };
		}

		INLINE void pagein()const noexcept
		{
			for (const ImageSectionHeader& secHeader : sectionHeaders())
				if(!(secHeader.Characteristics & ImageSectionFlag::mem_discardable))
				{
					const u8* pBegin{secHeader.VirtualAddress + m_base};
					const u8* pEnd{pBegin + secHeader.VirtualSize};
					for (const u8* p{ pBegin }; p < pEnd; p += 0x1000)
						[[maybe_unused]] const u8 _{ *const_cast<const volatile u8*>(p) };
				}
		}

		INLINE u16 sectionCount()const noexcept { return fileHeader().NumberOfSections; }
		INLINE u32 functionNamesCount()const noexcept { return dataDirectoryExport().Size? exportDir().NumberOfNames:0; }
		INLINE u32 ordinalBase()const noexcept { return exportDir().Base; }

		INLINE StringView8 dllName()const noexcept { return getPointer<const c8>(exportDir().Name); }
		INLINE const u32* pFunctionTable()const noexcept { return getPointer<u32>(exportDir().AddressOfFunctions); }
		INLINE const u16* pOrdinalTable()const noexcept { return getPointer<u16>(exportDir().AddressOfNameOrdinals); }
		INLINE const u32* pNameTable()const noexcept { return getPointer<u32>(exportDir().AddressOfNames); }

		INLINE Span<const ImageSectionHeader> sectionHeaders()const noexcept { return { reinterpret_cast<const ImageSectionHeader*>(reinterpret_cast<const u8*>(&optHeader()) + fileHeader().SizeOfOptionalHeader), sectionCount() }; }

		template<auto scanner>
		INLINE const u8* searchExecutable()const noexcept
		{
			for (const ImageSectionHeader& secHeader : sectionHeaders())
				if (secHeader.Characteristics & ImageSectionFlag::mem_execute)
				{
					const u8* pBegin{ secHeader.VirtualAddress + m_base };
					const u8* pEnd{ pBegin + secHeader.VirtualSize };
					if (const u8* pResult{ scanner.search(pBegin, pEnd) }; pResult)
						return pResult;
				}
			return nullptr;
		}

		//const ImageSectionHeader & sectionHeader(usize index)const noexcept { return IMAGE_FIRST_SECTION(&ntHeader())[index]; }
		INLINE u32 functionOrdinal(usize index)const noexcept { return ordinalBase() + pOrdinalTable()[index]; }
		INLINE const c8* functionName(usize index)const noexcept { return getPointer<const c8>(pNameTable()[index]); }
		INLINE const void* functionAddress(usize index)const noexcept { return getPointer<void>(pFunctionTable()[pOrdinalTable()[index]]); }

		struct FunctionForwardInfo
		{
			StringView8 dllName;
			StringView8 funcName;

			FunctionForwardInfo(const c8* names)noexcept :dllName{}, funcName{}
			{
				const StringView8 dllNameAndFunctionName{ names };
				const c8* pDot{ dllNameAndFunctionName.find('.') };//todo
				dllName = { dllNameAndFunctionName.begin(), pDot };
				funcName = { pDot + 1,dllNameAndFunctionName.end() };
			}

			bool valid()const noexcept { return !dllName.empty() && !funcName.empty(); }
		};

		INLINE bool isFunctionAddressForwarded(const void* addr)const noexcept
		{
			constexpr auto is_between{ [](auto min, auto max, auto v)STATIC_LAMBDA_INLINE{return (min <= v && max > v); } };
			return is_between((usize)&exportDir(), (usize)&exportDir() + dataDirectoryExport().Size, (usize)addr);//TODO
		}

		template<class Function>
		INLINE Function getFunctionTrueAddressFromLocalAddress(Function localAddr)const noexcept
		{
			if (!isFunctionAddressForwarded(reinterpret_cast<const void*>(localAddr)))
				return localAddr;
			else
			{
				const FunctionForwardInfo forwardInfo{ reinterpret_cast<const c8*>(localAddr) };

				if (!forwardInfo.valid())
					return nullptr;

				const void* forwardModuleBase{ LocalModules{}.getModuleBase(forwardInfo.dllName) };
				if (!forwardModuleBase || forwardModuleBase == reinterpret_cast<const void*>(m_base))
					return nullptr;

				return MappedPeView{ forwardModuleBase }.getFunctionTrueAddressByName<Function>(forwardInfo.funcName);
			}
		}

		INLINE Opt<u32> getFunctionIndexByNameEx($Sig<bool(const c8*)> auto&& match)const noexcept
		{
			const u32 count{ functionNamesCount() };
			for (u32 i{}; i != count; i++)
				if (match(functionName(i)))
					return i;
			return {};
		}

		template<class Function>
		INLINE Function getFunctionLocalAddressByNameEx($Sig<bool(const c8*)> auto&& match)const noexcept
		{
			const Opt<u32> index{ getFunctionIndexByNameEx(match) };
			return index ? reinterpret_cast<Function>(const_cast<void*>(functionAddress(*index))) : Function{};
		}

		template<class Function>
		INLINE Function getFunctionLocalAddressByName(auto&& name)const noexcept
		{
			return getFunctionLocalAddressByNameEx<Function>([&](const c8* funcName)LAMBDA_INLINE{ return name == StringView8{funcName}; });
		}

		template<class Function>
		INLINE Function getFunctionTrueAddressByName(auto&& name)const noexcept
		{
			const Function localAddr{ getFunctionLocalAddressByName<Function>(name) };
			const Function trueAddr{ getFunctionTrueAddressFromLocalAddress(localAddr) };
			return trueAddr;
		}

		struct ExportFunction
		{
			u32 ordinal;
			const u8* localAddress;
			const u8* forwardedAddress;
			Opt<FunctionForwardInfo> forwardInfo;
			StringView8 name;
			ExportFunction(u32 ord, const u8* localAddr, const u8* forwardAddr, const Opt<FunctionForwardInfo>& forwInfo, const c8* text)noexcept :
				ordinal{ ord }, localAddress{ localAddr }, forwardedAddress{ forwardAddr }, forwardInfo{ forwInfo }, name{ text }{}

			template<class Stream>
			friend Stream& operator<<(Stream& os, const ExportFunction& func)noexcept
			{
				os << TS("Ordinal = ") << Hex{ func.ordinal };
				os << TS(", Address = ") << func.localAddress;
				os << TS(", ForwardAddress = ");
				if (func.localAddress != func.forwardedAddress)
					os << hex(func.forwardedAddress);
				else
					for (u8 i{}; i != sizeof(void*) * 2; ++i)os << ' ';
				os << TS(", Name = ") << func.name;
				if (func.forwardInfo && func.forwardInfo->valid())
				{
					os << "\t, Forward DLL = " << &func.forwardInfo->dllName[0];
					os << "\t, Forward Name = " << &func.forwardInfo->funcName[0];
				}
				return os;
			}
		};

		INLINE void exports($Sig<void(const u8*, const c8*)> auto&& action)const noexcept
		{
			const u32 count{ functionNamesCount() };
			for (u32 i{}; i != count; ++i)
			{
				const u8* localAddr{ reinterpret_cast<const u8*>(functionAddress(i)) };
				const c8* name{ functionName(i) };
				action(localAddr, name);
			}
		}

		INLINE void exports($Sig<void(const ExportFunction)> auto&& action)const noexcept
		{
			const u32 count{ functionNamesCount() };
			for (u32 i{}; i != count; ++i)
			{
				const u32 ordinal{ functionOrdinal(i) };
				const u8* localAddr{ reinterpret_cast<const u8*>(functionAddress(i)) };
				const u8* forwardAddr{ getFunctionTrueAddressFromLocalAddress(localAddr) };
				Opt<FunctionForwardInfo> forwardInfo{ localAddr == forwardAddr ? Opt<FunctionForwardInfo>{} : Opt<FunctionForwardInfo>{ reinterpret_cast<const c8*>(localAddr)} };
				const c8* name{ functionName(i) };
				const ExportFunction exportFunction{ ordinal, localAddr, forwardAddr, forwardInfo, name };
				action(exportFunction);
			}
		}

		struct ImportFunction
		{
			StringView8 moduleName;
			usize address;
			Opt<usize> ordinal;
			Opt<u16> hint;
			Opt<StringView8> name;
			//constexpr ImportFunction(StringView8 m, usize a, Opt<usize> o, Opt<u16> h, Opt<StringView8> n)noexcept :moduleName{ m }, address{ a }, ordinal{ o }, hint{ h }, name{ n }{}
			template<class Stream>
			friend Stream& operator<<(Stream& os, const ImportFunction& func)noexcept
			{
				os << func.moduleName;
				os << TS(": addr = ") << hex(func.address);
				if (func.ordinal)os << TS(", ordinal = ") << Hex{ *func.ordinal };
				if (func.hint)os << TS(", hint = ") << hex(*func.hint);
				if (func.name)os << TS(", name = ") << *func.name;
				return os;
			}
		};

		INLINE void imports($Sig<void(const ImportFunction)> auto&& action)const noexcept
		{
			for (const ImageImportDescriptor& descriptor : importDescriptors())
			{
				const StringView8 moduleName{ reinterpret_cast<const c8*>(descriptor.Name + pBase()) };
				const usize* pThunkRef;
				if (descriptor.OriginalFirstThunk)pThunkRef = reinterpret_cast<const usize*>(pBase() + descriptor.OriginalFirstThunk);
				else if (descriptor.FirstThunk)pThunkRef = reinterpret_cast<const usize*>(pBase() + descriptor.FirstThunk);
				else continue;
				u32 index{};
				for (; *pThunkRef; ++pThunkRef)
				{
					const usize addr{ *(reinterpret_cast<const usize*>(pBase() + descriptor.FirstThunk) + index) };
					if ((*pThunkRef & imageOridinalFlag<BITNESS>) != 0)
					{
						const usize ordinal{ *pThunkRef & 0xFFFF };
						const ImportFunction importFunction{ moduleName, addr, ordinal, {}, {} };
						action(importFunction);
					}
					else
					{
						const IMAGE_IMPORT_BY_NAME& import{ *reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(pBase() + *pThunkRef) };
						const ImportFunction importFunction{ moduleName, addr, {}, import.Hint, StringView8{ &import.Name[0] } };
						action(importFunction);
					}
					++index;
				}
			}
		}

		//struct Section
		//{
		//	Span<const u8> span;
		//	StringView8 name;
		//	ImageSectionFlag characteristics;
		//	Section(const u8* pBegin, const ImageSectionHeader& header)noexcept :span{ pBegin,pBegin + header.SizeOfRawData }, name{ reinterpret_cast<const c8*>(header.Name.data()) }, characteristics{ header.Characteristics }{}
		//	template<class Stream>
		//	friend Stream& operator<<(Stream& os, const Section& section)noexcept
		//	{
		//		using char_t = typename Stream::char_type;
		//		{
		//			std::array<char_t, 8> buffer{ ' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' };
		//			u8 i{};
		//			for (c8 c : section.name)
		//				buffer[i++] = c;
		//			os << TS("Name = ") << Span{ buffer };
		//		}
		//		os << TS(", Begin = ") << section.span.begin() << TS(", End = ") << section.span.end() << TS(", Size = ") << hex(section.span.size()) << TS(", Flags = ") << section.characteristics;
		//		return os;
		//	}
		//};

		static INLINE MappedPeView exe()noexcept { return LocalModules::exeBase(); }
		static INLINE MappedPeView ntdll()noexcept { return LocalModules::ntdllBase(); }

		//GrowableArray<Section> sections()const noexcept
		//{
		//	GrowableArray<Section> result;
		//	result.reserve(sectionCount());
		//	for (usize i{}; i != sectionCount(); ++i)
		//	{
		//		const ImageSectionHeader& header{ sectionHeaders()[i] };
		//		result.emplace_back(header.VirtualAddress + pBase(), header);
		//	}
		//	return result;
		//}

		//template<class Stream>
		//friend Stream& operator<<(Stream& os, const MappedPeView& mod)noexcept//TODO
		//{
		//	if (mod.isDLL())
		//	{
		//		os << TS("Base = ") << mod.pBase() << '\n';
		//		os << TS("DLL Name = ") << mod.dllName() << '\n';
		//		os << TS("Export Functions:\n");
		//		for (const ExportFunction& f : mod.exports<1>())
		//			os << f << '\n';
		//		os << TS("Import Functions:\n");
		//		for (const ImportFunction& f : mod.imports())
		//			os << f << '\n';
		//	}
		//	os << TS("Sections:\n");
		//	for (const Section& sec : mod.sections())
		//		os << sec << '\n';
		//	return os;
		//}
	};
}