#pragma once
#include "AlgorithmCopy.h"
#include "Compiler.h"
#include "ConceptSame.h"
#include "ConceptSig.h"
#include "DynamicArray.h"
#include "Filepath.h"
#include "IterationControl.h"
#include "MacroMovForward.h"
#include "MacroNoCopyMove.h"
#include "NTDLL.h"
#include "NtStatus.h"
#include "WinStructs.h"
namespace Windows
{
	struct FileDirInfo//FILE_DIRECTORY_INFORMATION
	{
		u32 NextEntryOffset;
		u32 FileIndex;
		u64 CreationTime;
		u64 LastAccessTime;
		u64 LastWriteTime;
		u64 ChangeTime;
		u64 EndOfFile;
		u64 AllocationSize;
		FileAttributeFlag FileAttributes;
		u32 FileNameLength;
		c16 FileName[1];

		INLINE bool valid()const noexcept
		{
			return FileNameLength != 0
				&& !(FileNameLength == 2 && FileName[0] == '.')
				&& !(FileNameLength == 4 && FileName[0] == '.' && FileName[1] == '.');
		}

		INLINE bool isReadonly()/*              */const noexcept { return FileAttributes & FileAttributeFlag::readonly; }
		INLINE bool isHidden()/*                */const noexcept { return FileAttributes & FileAttributeFlag::hidden; }
		INLINE bool isSystem()/*                */const noexcept { return FileAttributes & FileAttributeFlag::system; }
		INLINE bool isDirectory()/*             */const noexcept { return FileAttributes & FileAttributeFlag::directory; }
		INLINE bool isArchive()/*               */const noexcept { return FileAttributes & FileAttributeFlag::archive; }
		INLINE bool isDevice()/*                */const noexcept { return FileAttributes & FileAttributeFlag::device; }
		INLINE bool isNormal()/*                */const noexcept { return FileAttributes & FileAttributeFlag::normal; }
		INLINE bool isTemporary()/*             */const noexcept { return FileAttributes & FileAttributeFlag::temporary; }
		INLINE bool isSparseFile()/*            */const noexcept { return FileAttributes & FileAttributeFlag::sparse_file; }
		INLINE bool isReparsePoint()/*          */const noexcept { return FileAttributes & FileAttributeFlag::reparse_point; }
		INLINE bool isCompressed()/*            */const noexcept { return FileAttributes & FileAttributeFlag::compressed; }
		INLINE bool isOffline()/*               */const noexcept { return FileAttributes & FileAttributeFlag::offline; }
		INLINE bool isNotContentIndexed()/*     */const noexcept { return FileAttributes & FileAttributeFlag::not_content_indexed; }
		INLINE bool isEncrypted()/*             */const noexcept { return FileAttributes & FileAttributeFlag::encrypted; }
		INLINE bool isIntegrityStream()/*       */const noexcept { return FileAttributes & FileAttributeFlag::integrity_stream; }
		INLINE bool isVirtual()/*               */const noexcept { return FileAttributes & FileAttributeFlag::virtual_; }
		INLINE bool isNoScrubData()/*           */const noexcept { return FileAttributes & FileAttributeFlag::no_scrub_data; }
		INLINE bool isEa()/*                    */const noexcept { return FileAttributes & FileAttributeFlag::ea; }
		INLINE bool isPinned()/*                */const noexcept { return FileAttributes & FileAttributeFlag::pinned; }
		INLINE bool isUnpinned()/*              */const noexcept { return FileAttributes & FileAttributeFlag::unpinned; }
		INLINE bool isRecallOnOpen()/*          */const noexcept { return FileAttributes & FileAttributeFlag::recall_on_open; }
		INLINE bool isRecallOnDataAccess()/*    */const noexcept { return FileAttributes & FileAttributeFlag::recall_on_data_access; }

		INLINE FileDirInfo* pNext()const noexcept
		{
			if (!NextEntryOffset)return nullptr;
			return reinterpret_cast<FileDirInfo*>(reinterpret_cast<usize>(this) + NextEntryOffset);
		}

		INLINE StringView16 name()const noexcept { return { &FileName[0],FileNameLength / 2 }; }

		template<class Stream>
		requires($Same<c16, typename Stream::char_type>)
		friend Stream& operator<<(Stream& os, const FileDirInfo& info)noexcept
		{
			if (info.isDirectory())
				os << TS("Directory = \"");
			else
				os << TS("File      = \"");
			os << info.name() << '"';
			return os;
		}
	};

	struct BasicFile
	{
		NO_COPY_MOVE(BasicFile)
	protected:
		HANDLE m_handle{};
		INLINE constexpr BasicFile()noexcept = default;
		template<FileInformationClass InfoClass, class InfoType>
		Opt<InfoType> getInfo()const noexcept
		{
			IO_STATUS_BLOCK statusBlock;
			InfoType result;
			if (NT_SUCCESS(NTDLL::NtQueryInformationFile(
				m_handle,
				&statusBlock,
				&result,
				sizeof(InfoType),
				InfoClass)))
				return result;
			return {};
		}

	public:
		~BasicFile()noexcept { dispose(); }

		HANDLE handle()const noexcept { return m_handle; }
		
		bool dispose()noexcept
		{
			if (!m_handle)return true;
			const bool result{ NT_SUCCESS(NTDLL::NtClose(m_handle)) };
			m_handle = {};
			return result;
		}
	};

	struct File: BasicFile
	{
		bool openDirImpl(const auto& path, bool createIfNotExist, ACCESS_MASK desiredAccess = FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE) noexcept
		{
			return createImpl(path, desiredAccess, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, createIfNotExist ? FILE_OPEN_IF : FILE_OPEN, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
		}

		bool readImpl(u64 offset, u32 size, u8* pBuffer, IO_STATUS_BLOCK& statusBlock)const noexcept
		{
			return NT_SUCCESS(NTDLL::NtReadFile(
				m_handle,
				nullptr,
				nullptr,
				nullptr,
				&statusBlock,
				pBuffer,
				size,
				&offset,
				nullptr));
		}

		bool writeImpl(u64 offset, u32 size, const void* pBuffer, IO_STATUS_BLOCK& statusBlock)const noexcept
		{
			return NT_SUCCESS(NTDLL::NtWriteFile(
				m_handle,
				nullptr,
				nullptr,
				nullptr,
				&statusBlock,
				const_cast<void*>(pBuffer),
				size,
				&offset,
				nullptr));
		}

		bool createImpl(NtPathView ntPath, ACCESS_MASK desiredAccess, u32 fileAttributes, u32 shareAccess, u32 createDisposition, u32 createOptions)noexcept
		{
			dispose();
			UnicodeString pathUnicodeString{ ntPath };
			ObjectAttributes attributes{ ObjectAttributes::create(pathUnicodeString) };
			IO_STATUS_BLOCK ioStatus;
			const NtStatus result{ NTDLL::NtCreateFile(
				&m_handle,														//OUT HANDLE* FileHandle,
				desiredAccess,													//IN ACCESS_MASK DesiredAccess,
				&attributes,													//IN ObjectAttributes* ObjectAttributes,
				&ioStatus,														//OUT IO_STATUS_BLOCK* IoStatusBlock,
				nullptr,														//IN LARGE_INTEGER* AllocationSize OPTIONAL,
				fileAttributes,													//IN u32 FileAttributes,
				shareAccess,													//IN u32 ShareAccess,
				createDisposition,												//IN u32 CreateDisposition,
				createOptions,													//IN u32 CreateOptions,
				nullptr,														//IN void* EaBuffer OPTIONAL,
				0																//IN u32 EaLength
			) };
			return NT_SUCCESS(result);
		}
		bool openImpl(NtPathView ntPath, ACCESS_MASK desiredAccess, u32 shareAccess, u32 openOptions)noexcept
		{
			dispose();
			UnicodeString pathUnicodeString{ ntPath };
			ObjectAttributes attributes{ ObjectAttributes::create(pathUnicodeString) };
			IO_STATUS_BLOCK ioStatus;
			const NtStatus result{ NTDLL::NtOpenFile(
				&m_handle,														//OUT HANDLE* FileHandle,
				desiredAccess,													//IN ACCESS_MASK DesiredAccess,
				&attributes,													//IN ObjectAttributes* ObjectAttributes,
				&ioStatus,														//OUT IO_STATUS_BLOCK* IoStatusBlock,
				shareAccess,													//u32 shareAccess
				openOptions														//u32 openOptions
			) };
			return NT_SUCCESS(result);
		}

		bool createImpl(DosPathView path, ACCESS_MASK desiredAccess, u32 fileAttributes, u32 shareAccess, u32 createDisposition, u32 createOptions)noexcept { return createImpl(Windows::NtPath{ path }, desiredAccess, fileAttributes, shareAccess, createDisposition, createOptions); }
		bool openImpl(DosPathView path, ACCESS_MASK desiredAccess, u32 shareAccess, u32 openOptions)noexcept { return openImpl(Windows::NtPath{ path }, desiredAccess, shareAccess, openOptions); }

		bool createToRead(const auto& path)noexcept
		{
			return createImpl(path, FILE_GENERIC_READ, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);
		}

		bool openToReadImpl(const auto& path) noexcept
		{
			return openImpl(path, FILE_GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
		}

		bool createToWriteImpl(const auto& path, ACCESS_MASK desiredAccess, bool createIfNotExist) noexcept
		{
			return createImpl(path, desiredAccess, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, createIfNotExist ? FILE_OVERWRITE_IF : FILE_OVERWRITE, FILE_SYNCHRONOUS_IO_NONALERT);
		}

		bool openFileToDeleteImpl(const auto& path) noexcept
		{
			return openImpl(path, DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_NON_DIRECTORY_FILE);
		}

		bool openSymlinkFileToDeleteImpl(const auto& path) noexcept
		{
			return openImpl(path, DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_NON_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT);
		}

		bool openSymlinkDirectoryToDeleteImpl(const auto& path) noexcept
		{
			return openImpl(path, DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT);
		}

		//bool createToWriteTransactedImpl(const Transaction& transaction, const auto& path, ACCESS_MASK desiredAccess, bool createIfNotExist) noexcept
		//{
		//	transaction.start();
		//	const bool result{ createToWriteImpl(path, desiredAccess, createIfNotExist) };
		//	transaction.end();
		//	return result;
		//}
	public:

		Opt<FILE_STANDARD_INFO> standardInfo()const noexcept { return getInfo<FileInformationClass::FileStandardInformation, FILE_STANDARD_INFO>(); }
		Opt<FILE_BASIC_INFO> basicInfo()const noexcept { return getInfo<FileInformationClass::FileBasicInformation, FILE_BASIC_INFO>(); }
		//Opt<FILE_NAME_INFO> nameInfo()const noexcept { return getInfo<FileInformationClass::FileNameInformation, FILE_NAME_INFO>(); }//TODO

		constexpr File()noexcept {}

		bool openToRead(DosPathView path) noexcept { return openToReadImpl(path); /*return createToRead(path);*/ }
		bool openToRead(NtPathView path)  noexcept { return openToReadImpl(path); /*return createToRead(path);*/ }

		bool createToWrite(DosPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE, bool createIfNotExist = true) noexcept { return createToWriteImpl(path, desiredAccess, createIfNotExist); }
		bool createToWrite(NtPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE, bool createIfNotExist = true) noexcept { return createToWriteImpl(path, desiredAccess, createIfNotExist); }

		bool createToWriteDelete(DosPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE | DELETE, bool createIfNotExist = true) noexcept { return createToWriteImpl(path, desiredAccess, createIfNotExist); }
		bool createToWriteDelete(NtPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE | DELETE, bool createIfNotExist = true) noexcept { return createToWriteImpl(path, desiredAccess, createIfNotExist); }

		//bool createToWriteTransacted(const Transaction& transaction, DosPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE, bool createIfNotExist = true) noexcept { return createToWriteTransactedImpl(transaction, path, desiredAccess, createIfNotExist); }
		//bool createToWriteTransacted(const Transaction& transaction, NtPathView path, ACCESS_MASK desiredAccess = FILE_GENERIC_WRITE, bool createIfNotExist = true) noexcept { return createToWriteTransactedImpl(transaction, path, desiredAccess, createIfNotExist); }

		bool openFileToDelete(DosPathView path) noexcept { return openFileToDeleteImpl(path); }
		bool openFileToDelete(NtPathView path) noexcept { return openFileToDeleteImpl(path); }

		bool openDirToDelete(NtPathView path)noexcept { return openDirImpl(path, false, DELETE | FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE); }
		bool openDirToDelete(DosPathView path)noexcept { return openDirImpl(path, false, DELETE | FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE); }

		bool openSymlinkFileToDelete(DosPathView path) noexcept { return openSymlinkFileToDeleteImpl(path); }
		bool openSymlinkFileToDelete(NtPathView path) noexcept { return openSymlinkFileToDeleteImpl(path); }

		bool openSymlinkDirectoryToDelete(DosPathView path) noexcept { return openSymlinkDirectoryToDeleteImpl(path); }
		bool openSymlinkDirectoryToDelete(NtPathView path) noexcept { return openSymlinkDirectoryToDeleteImpl(path); }
		
		bool createDevice(NtPathView path, ACCESS_MASK desiredAccess = FILE_READ_DATA | FILE_WRITE_DATA, u32 fileAttributes = 0, u32 shareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE, u32 createDisposition = FILE_OPEN | FILE_CREATE, u32 createOptions = 0)noexcept
		{
			return createImpl(path, desiredAccess, fileAttributes, shareAccess, createDisposition, createOptions);
		}

		template<FixedString Path>
		requires(sizeof(Path[0]) == 2)
		bool createDevice(ACCESS_MASK desiredAccess = FILE_READ_DATA | FILE_WRITE_DATA, u32 fileAttributes = 0, u32 shareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE, u32 createDisposition = FILE_OPEN | FILE_CREATE, u32 createOptions = 0)noexcept
		{
			const ObfuString path{ tstring<Path> };
			return createDevice(NtPathView{path}, desiredAccess, fileAttributes, shareAccess, createDisposition, createOptions);
		}

		bool openDir(DosPathView path)noexcept { return openDirImpl(path, false); }
		bool openDir(NtPathView path)noexcept { return openDirImpl(path, false); }

		bool createDir(DosPathView path)noexcept { return openDirImpl(path, true); }
		bool createDir(NtPathView path)noexcept { return openDirImpl(path, true); }

		template<u32 BufferSize = 0x4000, $SigAnyOf<void(const FileDirInfo), IterationControl(const FileDirInfo)> Action>
		bool enumContainedFiles(Action&& action) const noexcept
		{
			u8 buffer[BufferSize];
			IO_STATUS_BLOCK statusBlock;
			NtStatus status{ NTDLL::NtQueryDirectoryFileEx(m_handle,nullptr,nullptr,nullptr,&statusBlock,&buffer,BufferSize,FileInformationClass::FileDirectoryInformation,QueryDirectoryFlag::restart_scan,nullptr) };
			for (; NT_SUCCESS(status); status = NTDLL::NtQueryDirectoryFileEx(m_handle, nullptr, nullptr, nullptr, &statusBlock, &buffer, BufferSize, FileInformationClass::FileDirectoryInformation, {}, nullptr))
			{
				FileDirInfo* pFileInfo{ reinterpret_cast<FileDirInfo*>(&buffer) };
				while (pFileInfo)
				{
					if (pFileInfo->valid())
					{
						if constexpr($Sig<Action, void(const FileDirInfo)>)
							action(*pFileInfo);
						else if (action(*pFileInfo) == IterationControl::stop)
							return true;
					}
					pFileInfo = pFileInfo->pNext();
				}
			}
			return status == NtStatus::no_more_files;
		}

		Opt<u64> size()const noexcept
		{
			if (const Opt<FILE_STANDARD_INFO> optInfo{ standardInfo() }; optInfo)
				return static_cast<u64>(optInfo->EndOfFile.QuadPart);
			else
				return {};
		}

		Opt<DynamicArray<u8>> read()const noexcept
		{
			const Opt<u64> optSize{ size() };
			if (!optSize)
				return {};
		#if BITNESS == 64
			DynamicArray<u8> result{ uninitialized, *optSize };
			usize currentOffset{};
			usize bytesLeft{ *optSize };
			IO_STATUS_BLOCK statusBlock;
			while (bytesLeft)
			{
				const u32 chunkSize{ static_cast<u32>(std::min(static_cast<u64>(std::numeric_limits<u32>::max()),bytesLeft)) };
				bytesLeft -= chunkSize;
				if (!readImpl(currentOffset, chunkSize, result.data() + currentOffset, statusBlock))
					return {};
				currentOffset += chunkSize;
			}
			return result;
		#else
			if (*optSize > std::numeric_limits<usize>::max())
				return {};
			DynamicArray<u8> result{ uninitialized, static_cast<usize>(*optSize) };
			IO_STATUS_BLOCK statusBlock;
			if (!readImpl(0, static_cast<usize>(*optSize), result.data(), statusBlock))
				return {};
			return result;
		#endif
		}

		bool write(Span<const u8> buffer)const noexcept
		{
		#if BITNESS == 64
			IO_STATUS_BLOCK statusBlock;
			usize currentOffset{};
			usize bytesLeft{ buffer.size() };
			while (bytesLeft)
			{
				const u32 chunkSize{ static_cast<u32>(std::min(static_cast<u64>(std::numeric_limits<u32>::max()),bytesLeft)) };
				bytesLeft -= chunkSize;
				if (!writeImpl(currentOffset, chunkSize, buffer.data() + currentOffset, statusBlock))
					return false;
				currentOffset += chunkSize;
			}
			return true;
		#else
			IO_STATUS_BLOCK statusBlock;
			return writeImpl(0, buffer.size(), buffer.data(), statusBlock);
		#endif
		}

		bool deleteFileOrEmptyDirOrSymlink()const noexcept
		{
			IO_STATUS_BLOCK ioStatusBlock;
#if 0
			FILE_DISPOSITION_INFORMATION_EX deleteInfo{ FILE_DISPOSITION_INFORMATION_EX::Flags::FILE_DISPOSITION_DELETE | FILE_DISPOSITION_INFORMATION_EX::Flags::FILE_DISPOSITION_POSIX_SEMANTICS };
			const NtStatus result{ NTDLL::NtSetInformationFile(m_handle, &ioStatusBlock, &deleteInfo, sizeof(deleteInfo), FileInformationClass::FileDispositionInformationEx) };
			if (NT_SUCCESS(result))
				return true;
			else
				return false;
#else
			FILE_DISPOSITION_INFORMATION deleteInfo{ true };
			return NT_SUCCESS(NTDLL::NtSetInformationFile(m_handle, &ioStatusBlock, &deleteInfo, sizeof(deleteInfo), FileInformationClass::FileDispositionInformation));
#endif
		}

		template<$LinearContainerOf<c16> Name>
		bool renameFileOrDir(const Name& filename, FileRenameFlag flag)const noexcept
		{
			if constexpr ($FixSized<Name>)
			{
				FILE_RENAME_INFORMATION_EX<static_size<Name>> info;
				info.Flags = flag;
				info.RootDirectory = nullptr;
				info.FileNameLength = static_size<Name> * sizeof(c16);
				Algorithm::copy_assign(filename, &info.FileName[0]);
				IO_STATUS_BLOCK ioStatusBlock;
				return NT_SUCCESS(NTDLL::NtSetInformationFile(m_handle, &ioStatusBlock, &info, sizeof(info), FileInformationClass::FileRenameInformationEx));
			}
			else
			{
				DynamicArray<u8> buffer{ uninitialized, __builtin_offsetof(FILE_RENAME_INFORMATION_EX<1>, FileName) + sizeof(c16) * filename.size() };
				reinterpret_cast<FILE_RENAME_INFORMATION_EX<1>*>(buffer.data())->Flags = flag;
				reinterpret_cast<FILE_RENAME_INFORMATION_EX<1>*>(buffer.data())->RootDirectory = nullptr;
				reinterpret_cast<FILE_RENAME_INFORMATION_EX<1>*>(buffer.data())->FileNameLength = static_cast<u32>(filename.size() * sizeof(c16));
				Algorithm::copy_assign(filename, &reinterpret_cast<FILE_RENAME_INFORMATION_EX<1>*>(buffer.data())->FileName[0]);
				IO_STATUS_BLOCK ioStatusBlock;
				return NT_SUCCESS(NTDLL::NtSetInformationFile(m_handle, &ioStatusBlock, buffer.data(), static_cast<u32>(buffer.size()), FileInformationClass::FileRenameInformationEx));
			}
		}
	};
}

namespace details
{
	inline Opt<DynamicArray<u8>> readFile($AnyOf<Windows::DosPathView, Windows::NtPathView> auto const& path)noexcept
	{
		Windows::File file;
		if (!file.openToRead(path))
			return {};
		return file.read();
	}

	inline bool writeFile($AnyOf<Windows::DosPathView, Windows::NtPathView> auto const& path, Span<const u8> buffer)noexcept
	{
		Windows::File file;
		return file.createToWrite(path) && file.write(buffer);
	}
}

INLINE Opt<DynamicArray<u8>> readFile(Windows::DosPathView file_name)noexcept { return details::readFile(file_name); }
INLINE Opt<DynamicArray<u8>> readFile(Windows::NtPathView file_name)noexcept { return details::readFile(file_name); }
INLINE bool writeFile(Windows::DosPathView filename, Span<const u8> buffer)noexcept { return details::writeFile(filename, buffer); }
INLINE bool writeFile(Windows::DosPathView filename, Span<const c8> buffer)noexcept { return details::writeFile(filename, { (const u8*)buffer.data(), (const u8*)buffer.data() + buffer.size() }); }
INLINE bool writeFile(Windows::NtPathView filename, Span<const u8> buffer)noexcept { return details::writeFile(filename, buffer); }
INLINE bool writeFile(Windows::NtPathView filename, Span<const c8> buffer)noexcept { return details::writeFile(filename, { (const u8*)buffer.data(), (const u8*)buffer.data() + buffer.size() }); }

inline bool renameFile(const auto& filename, $LinearContainerOf<c16> auto const& name, Windows::FileRenameFlag flag = Windows::FileRenameFlag::replace_if_exists)noexcept { Windows::File file; return file.openFileToDelete(filename) && file.renameFileOrDir(name, flag); }
inline bool renameDir(const auto& filename, $LinearContainerOf<c16> auto const& name, Windows::FileRenameFlag flag = Windows::FileRenameFlag::replace_if_exists)noexcept { Windows::File file; return file.openDirToDelete(filename) && file.renameFileOrDir(name, flag); }
template<class Char, Char...cs>inline bool renameFile(const auto& filename, TString<Char, cs...>, Windows::FileRenameFlag flag = Windows::FileRenameFlag::replace_if_exists)noexcept { Windows::File file; return file.openFileToDelete(filename) && file.renameFileOrDir(ObfuString{ TString<c16, cs...>{} }, flag); }
template<class Char, Char...cs>inline bool renameDir(const auto& filename, TString<Char, cs...>, Windows::FileRenameFlag flag = Windows::FileRenameFlag::replace_if_exists)noexcept { Windows::File file; return file.openDirToDelete(filename) && file.renameFileOrDir(ObfuString{ TString<c16, cs...>{} }, flag); }

inline bool deleteSymlinkFile(const auto& filename)noexcept
{
	Windows::File file;
	return file.openSymlinkFileToDelete(filename) && file.deleteFileOrEmptyDirOrSymlink();
}

inline bool deleteSymlinkDirectory(const auto& filename)noexcept
{
	Windows::File file;
	return file.openSymlinkDirectoryToDelete(filename) && file.deleteFileOrEmptyDirOrSymlink();
}

inline bool deleteFile(Windows::NtPathView path)noexcept
{
	UnicodeString pathUnicodeString{ path };
	ObjectAttributes attributes{ ObjectAttributes::create(pathUnicodeString) };
	const NtStatus status{ NTDLL::NtDeleteFile(&attributes) };
	//LOG(status);
	return NT_SUCCESS(status);
}

inline bool deleteFile(Windows::DosPathView path)noexcept { return deleteFile(Windows::NtPath{ path }); }

inline bool deleteEmptyDir(const auto& filename)noexcept
{
	Windows::File file; 
	return file.openDirToDelete(filename) && file.deleteFileOrEmptyDirOrSymlink();
}

template<u32 BufferSize = 0x4000>
inline bool enumFiles(Windows::$Path auto const& dirname, $SigAnyOf<void(const Windows::FileDirInfo), IterationControl(const Windows::FileDirInfo)> auto&& action)noexcept
{
	Windows::File file;
	if (!file.openDir(dirname))
		return false;
	return file.enumContainedFiles<BufferSize>(action);
}

template<u32 BufferSize = 0x4000, Windows::$Path Path>
inline bool enumFilesRecursive(const Path& dirname, $SigAnyOf<void(const Path, const Windows::FileDirInfo), IterationControl(const Path, const Windows::FileDirInfo)> auto&& action)noexcept
{
	Windows::File file;
	if (!file.openDir(dirname))
		return false;
	bool stop{};
	return file.enumContainedFiles<BufferSize>([&](const Windows::FileDirInfo& info)LAMBDA_INLINE
	{
		const auto path{ dirname / info.name() };
		const bool isSymlink{ info.FileAttributes & Windows::FileAttributeFlag::reparse_point };
		const bool isDirectory{ info.FileAttributes & Windows::FileAttributeFlag::directory };
		if (!isDirectory)
		{
			if constexpr ($Sig<decltype(action), void(const Path, const Windows::FileDirInfo)>)
			{
				action(path, info);
				return IterationControl::resume;
			}
			else
			{
				if (action(path, info) == IterationControl::stop)
				{
					stop = true;
					return IterationControl::stop;
				}
				else return IterationControl::resume;
			}
		}
		else if (!isSymlink && !enumFilesRecursive<BufferSize>(path, FWD(action)))
		{
			stop = true;
			return IterationControl::stop;
		}
		else return IterationControl::resume;
	}) && !stop;
}

//todo:check if file exist, if not exist return true
template<u32 BufferSize = 0x4000>
inline bool deleteDir(const auto& dirname)noexcept
{
	Windows::File file;
	if (!file.openDirToDelete(dirname))
		return false;
	bool failedToDeleteSubFileOrDir{ false };
	return file.enumContainedFiles<BufferSize>([&](const Windows::FileDirInfo& info)LAMBDA_INLINE
	{
		const auto path{ dirname / info.name() };
		const bool isSymlink{ info.FileAttributes & Windows::FileAttributeFlag::reparse_point };
		const bool isDirectory{ info.FileAttributes & Windows::FileAttributeFlag::directory };
		if (isSymlink)
		{
			if (isDirectory)
			{
				if (!deleteSymlinkDirectory(path))
				{
					failedToDeleteSubFileOrDir = true;
					return IterationControl::stop;
				}
			}
			else
			{
				if (!deleteSymlinkFile(path))
				{
					failedToDeleteSubFileOrDir = true;
					return IterationControl::stop;
				}
			}
		}
		else
		{
			if (isDirectory)
			{
				if (!deleteDir<BufferSize>(path))
				{
					failedToDeleteSubFileOrDir = true;
					return IterationControl::stop;
				}
			}
			else
			{
				if (!deleteFile(path))
				{
					failedToDeleteSubFileOrDir = true;
					return IterationControl::stop;
				}
			}
		}
		return IterationControl::resume;
	}) && !failedToDeleteSubFileOrDir && deleteEmptyDir(dirname);
}

inline bool createDir(const auto& path)noexcept
{
	Windows::File file;
	return file.createDir(path);
}