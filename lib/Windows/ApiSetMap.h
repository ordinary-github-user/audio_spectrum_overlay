#pragma once
#include "../Util/BasicTypes.h"
#include "../Util/DataStructures/Span.h"
#include "../Util/DataStructures/StringView.h"
#include "../Util/StringFormat/HexToStream.h"
namespace Windows::ApiSetMap
{
	//https://github.com/lucasg/Dependencies/blob/master/ClrPhlib/src/managed/Phlib.cpp
	namespace V2
	{
		struct Namespace;
		struct ValueEntryRedirection
		{
			u32 NameOffset;
			u16 NameLength;
			u32 ValueOffset;
			u16 ValueLength;

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + ValueOffset) , ValueLength / sizeof(c16) };
			}

			StringView16 alias(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}
		};

		struct ValueEntry
		{
			u32 NumberOfRedirections;
			ValueEntryRedirection Redirections[1];

			Span<const ValueEntryRedirection> redirections()const noexcept { return { &Redirections[0], NumberOfRedirections }; }
		};

		struct NamespaceEntry
		{
			u32 NameOffset;
			u32 NameLength;
			u32 DataOffset; // ===> ValueEntry

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}

			const ValueEntry& entry(const Namespace& nameSpace)const noexcept { return *reinterpret_cast<const ValueEntry*>(reinterpret_cast<const u8*>(&nameSpace) + DataOffset); }
		};

		struct Namespace
		{
			u32 Version;
			u32 Count;
			NamespaceEntry Array[1];

			Span<const NamespaceEntry> entries()const noexcept { return { &Array[0], Count }; }

			template<class Stream>
			friend Stream& operator<<(Stream& os, const Namespace& nameSpace)noexcept
			{
				os << TS("Version = ") << nameSpace.Version;
				os << TS("\nCount = ") << nameSpace.Count;
				os << TS("\nEntries: ");
				for (const NamespaceEntry& namespaceEntry : nameSpace.entries())
				{
					os << TS("\n\tName = ") << namespaceEntry.name(nameSpace);
					const ValueEntry& valueEntry{ namespaceEntry.entry(nameSpace) };
					for (const ValueEntryRedirection& redirection : valueEntry.redirections())
					{
						os << TS("\n\t\tName = ") << redirection.name(nameSpace);
						if (const StringView16 alias{ redirection.alias(nameSpace) }; alias.size())os << '(' << alias << ')';
					}
				}
				return os;
			}
		};
	}

	namespace V4
	{
		struct Namespace;
		struct ValueEntryRedirection
		{
			u32 Flags;
			u32 NameOffset;
			u32 NameLength;
			u32 ValueOffset;
			u32 ValueLength;

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + ValueOffset) , ValueLength / sizeof(c16) };
			}

			StringView16 alias(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}
		};

		struct ValueEntry
		{
			u32 Flags;
			u32 NumberOfRedirections;
			ValueEntryRedirection Redirections[1];

			Span<const ValueEntryRedirection> redirections()const noexcept { return { &Redirections[0], NumberOfRedirections }; }
		};

		struct NamespaceEntry
		{
			u32 Flags;
			u32 NameOffset;
			u32 NameLength;
			u32 AliasOffset;
			u32 AliasLength;
			u32 DataOffset; // ===> ValueEntry

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}

			StringView16 alias(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + AliasOffset) , AliasLength / sizeof(c16) };
			}

			const ValueEntry& entry(const Namespace& nameSpace)const noexcept { return *reinterpret_cast<const ValueEntry*>(reinterpret_cast<const u8*>(&nameSpace) + DataOffset); }
		};

		struct Namespace
		{
			u32 Version;
			u32 Size;
			u32 Flags;
			u32 Count;
			NamespaceEntry Array[1];

			Span<const NamespaceEntry> entries()const noexcept { return { &Array[0], Count }; }

			template<class Stream>
			friend Stream& operator<<(Stream& os, const Namespace& nameSpace)noexcept
			{
				os << TS("Version = ") << nameSpace.Version;
				os << TS("\nSize = ") << nameSpace.Size;
				os << TS("\nFlags = ") << nameSpace.Flags;
				os << TS("\nCount = ") << nameSpace.Count;
				os << TS("\nEntries: ");
				for (const NamespaceEntry& namespaceEntry : nameSpace.entries())
				{
					os << TS("\n\tName = ") << namespaceEntry.name(nameSpace);
					os << TS("\n\tAlias = ") << namespaceEntry.alias(nameSpace);
					const ValueEntry& valueEntry{ namespaceEntry.entry(nameSpace) };
					for (const ValueEntryRedirection& redirection : valueEntry.redirections())
					{
						os << TS("\n\t\tFlag = ") << Hex{ redirection.Flags };
						if (const StringView16 name{ redirection.name(nameSpace) }; name.size())os << TS(", Name = ") << name;
						if (const StringView16 alias{ redirection.alias(nameSpace) }; alias.size())os << '(' << alias << ')';
					}
				}
				return os;
			}
		};
	}

	namespace V6
	{
		struct Namespace;
		struct HashEntry
		{
			u32 Hash;
			u32 Index;
		};

		struct ValueEntry
		{
			u32 Flags;
			u32 NameOffset;
			u32 NameLength;
			u32 ValueOffset;
			u32 ValueLength;

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + ValueOffset) , ValueLength / sizeof(c16) };
			}

			StringView16 alias(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}
		};

		struct NamespaceEntry
		{
			u32 Flags;
			u32 NameOffset;
			u32 NameLength;
			u32 HashedLength;
			u32 ValueOffset;
			u32 ValueCount;

			StringView16 name(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const c16*>(reinterpret_cast<const u8*>(&nameSpace) + NameOffset) , NameLength / sizeof(c16) };
			}

			Span<const ValueEntry> entries(const Namespace& nameSpace)const noexcept
			{
				return { reinterpret_cast<const ValueEntry*>(reinterpret_cast<const u8*>(&nameSpace) + ValueOffset) , ValueCount };
			}
		};

		struct Namespace
		{
			u32 Version;
			u32 Size;
			u32 Flags;
			u32 Count;
			u32 EntryOffset;
			u32 HashOffset;
			u32 HashFactor;

			Span<const NamespaceEntry> entries()const noexcept
			{
				return { reinterpret_cast<const NamespaceEntry*>(reinterpret_cast<const u8*>(this) + EntryOffset) , Count };
			}

			template<class Stream>
			friend Stream& operator<<(Stream& os, const Namespace& nameSpace)noexcept
			{
				os << TS("Version = ") << nameSpace.Version;
				os << TS("\nSize = ") << nameSpace.Size;
				os << TS("\nFlags = ") << nameSpace.Flags;
				os << TS("\nCount = ") << nameSpace.Count;
				os << TS("\nEntryOffset = ") << nameSpace.EntryOffset;
				os << TS("\nHashOffset = ") << nameSpace.HashOffset;
				os << TS("\nHashFactor = ") << nameSpace.HashFactor;
				os << TS("\nEntries: ");
				for (const NamespaceEntry& namespaceEntry : nameSpace.entries())
				{
					os << TS("\n\tName = ") << namespaceEntry.name(nameSpace);
					for (const ValueEntry& valueEntry : namespaceEntry.entries(nameSpace))
					{
						os << TS("\n\t\tFlag = ") << Hex{ valueEntry.Flags };
						if (const StringView16 name{ valueEntry.name(nameSpace) }; name.size())os << TS(", Name = ") << name;
						if (const StringView16 alias{ valueEntry.alias(nameSpace) }; alias.size())os << '(' << alias << ')';
					}
				}
				return os;
			}
		};
	}
}