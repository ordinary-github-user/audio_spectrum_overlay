#pragma once
#include "DateTime.h"
#include "DirectoryMonitor.h"
#include "DynamicArray.h"
#include "IndexTypeOf.h"
#include "ValueList.h"
namespace Windows
{
	namespace details
	{
		template<FixedString path>
		constexpr bool is_valid_absolute_nt_path{ [] static
		{
			if(!$StringOfCharSize<decltype(path), 2>)return false;
			if(path.size() < 7)return false;
			if(!path.template starts_with<LR"(\??\)">())return false;
			if(!((path[4] >= 'a' && path[4] <= 'z') || (path[4] >= 'A' && path[4] <= 'Z')))return false;
			if(path[5] != ':')return false;
			if(path[6] != '\\')return false;
			if(path.back() == '\\')return false;
			for (const auto i: iota<7, path.size()>)
				if (path[i] == '\\' && path[i - 1] == '\\')
					return false;
			return true;
		}() };

		template<FixedString path>
		requires(is_valid_absolute_nt_path<path>)
		constexpr usize nt_path_ancestor_count{ [] static
		{
			usize result{ 1 };
			for (const c16 c : StringView16{ path.begin() + 7, path.end() })
				if (c == '\\')
					++result;
			return result;
		}()};

		template<FixedString path>
		requires(is_valid_absolute_nt_path<path>)
		using NtPathAncestors = std::remove_cvref_t<decltype([] <auto...I> (std::index_sequence<I...>)static
		{
			using Index = IndexOf<decltype(path)>;
			static constexpr std::array<Index, nt_path_ancestor_count<path>> delimiter_indexes{ [] static
			{
				std::array<Index, nt_path_ancestor_count<path>> result;
				Index* p{result.data()};
				for (const Index i : iota<6, path.size()>)
					if (path[i] == '\\')
						*p++ = i;
				return result;
			}() };
			return ValueList<path.template substr<0, delimiter_indexes[I] + 1>()...>{};
		}(std::make_index_sequence<nt_path_ancestor_count<path>>{}))>;

		template<FixedString path>
		requires(is_valid_absolute_nt_path<path>)
		using NtPathComponents = std::remove_cvref_t<decltype([] <auto...I> (std::index_sequence<I...>)static
		{
			using Index = IndexOf<decltype(path)>;
			static constexpr std::array<Index, nt_path_ancestor_count<path>> delimiter_indexes{ [] static
			{
				std::array<Index, nt_path_ancestor_count<path>> result;
				Index* p{result.data()};
				for (const Index i : iota<6, path.size()>)
					if (path[i] == '\\')
						*p++ = i;
				return result;
			}() };
			return ValueList<path.template substr<delimiter_indexes[I] + 1, (I + 1 == sizeof...(I)) ? path.size() : delimiter_indexes[I + 1]>()...>{};
		}(std::make_index_sequence<nt_path_ancestor_count<path>>{}))>;

		template<FixedString, u32, class, class, class>struct StaticFileMonitorImpl;

		template<FixedString path, u32 buffer_size, FixedString...ancestor, FixedString...component, usize...I>
		struct StaticFileMonitorImpl<path, buffer_size, ValueList<ancestor...>, ValueList<component...>, std::index_sequence<I...>>
		{
			NO_COPY_MOVE(StaticFileMonitorImpl)
			DEFINE_ENUM_(Status, u8, no_access, modified, not_modified);
			using Record = FileNotifyInformation;
			using Monitor = DirectoryMonitor<Record, false, buffer_size>;
		private:
			FileTime m_timeWrite{};
			FileTime m_timeChange{};
			Monitor m_monitors[sizeof...(I)]{};
			Status m_status{ Status::no_access };
			template<bool prevValid>
			INLINE Status sync()noexcept
			{
				File file;
				if (!file.openToRead(NtPathView{ path })) [[unlikely]]
					return Status::no_access;
				const Opt<FILE_BASIC_INFO> basicInfo{ file.basicInfo() };
				if (!basicInfo) [[unlikely]]
					return Status::no_access;
				if constexpr (prevValid)
				{
					const Status result{ (__builtin_bit_cast(FileTime, basicInfo->LastWriteTime) != m_timeWrite || __builtin_bit_cast(FileTime, basicInfo->ChangeTime) != m_timeChange) ? Status::modified : Status::not_modified };
					m_timeWrite  = __builtin_bit_cast(FileTime, basicInfo->LastWriteTime);
					m_timeChange = __builtin_bit_cast(FileTime, basicInfo->ChangeTime);
					return result;
				}
				else 
				{
					m_timeWrite  = __builtin_bit_cast(FileTime, basicInfo->LastWriteTime);
					m_timeChange = __builtin_bit_cast(FileTime, basicInfo->ChangeTime);
					return Status::modified;
				}
			}
		public:
			INLINE StaticFileMonitorImpl()noexcept = default;
			INLINE Status update()noexcept
			{
				if (m_status == Status::no_access) [[unlikely]]
				{
					(..., m_monitors[I].update(NtPathView{ ancestor }));
					return (m_status = sync<0>());
				}
				else [[likely]]
				{
					bool found{};
					if ((...&(Monitor::Status::pending == m_monitors[I].update(NtPathView{ ancestor }, [&](const Record& record)LAMBDA_INLINE
						{
							if (Wtf16::iequal<component>(record.name())) [[unlikely]]
							{
								found = true;
								return IterationControl::stop;
							}
							else
								return IterationControl::resume;
						}))) && !found) [[likely]]
						return (m_status = Status::not_modified);
					else [[unlikely]]
						return (m_status = sync<1>());
				}
			}
		};
	}

	template<FixedString path, u32 buffer_size = 0x4000>
	using StaticFileMonitor = details::StaticFileMonitorImpl<path, buffer_size, details::NtPathAncestors<path>, details::NtPathComponents<path>, std::make_index_sequence<details::nt_path_ancestor_count<path>>>;

	template<$AnyOf<NtPath, NtPathView> Path, u32 buffer_size = 0x4000>
	struct DynamicFileMonitor
	{
		NO_COPY_MOVE(DynamicFileMonitor)
		DEFINE_ENUM_(Status, u8, no_access, modified, not_modified);
		using Record = FileNotifyInformation;
		using Monitor = DirectoryMonitor<Record, false, buffer_size>;
		using Index = IndexOf<Path>;
	private:
		struct MonitorImpl
		{
			StringView16 ancestor;
			StringView16 component;
			Monitor monitor;
		};
		Path m_path;
		DynamicArray<MonitorImpl, Index> m_monitors;//using Index<Path> because m_monitors.size() < m_path.size()
		FileTime m_timeWrite;
		FileTime m_timeChange;
		Status m_status;
		template<bool prevValid>
		INLINE Status sync()noexcept
		{
			File file;
			if (!file.openToRead(m_path)) [[unlikely]]
				return Status::no_access;
			const Opt<FILE_BASIC_INFO> basicInfo{ file.basicInfo() };
			if (!basicInfo) [[unlikely]]
				return Status::no_access;
			if constexpr (prevValid)
			{
				const Status result{ (__builtin_bit_cast(FileTime, basicInfo->LastWriteTime) != m_timeWrite || __builtin_bit_cast(FileTime, basicInfo->ChangeTime) != m_timeChange) ? Status::modified : Status::not_modified };
				m_timeWrite = __builtin_bit_cast(FileTime, basicInfo->LastWriteTime);
				m_timeChange = __builtin_bit_cast(FileTime, basicInfo->ChangeTime);
				return result;
			}
			else
			{
				m_timeWrite = __builtin_bit_cast(FileTime, basicInfo->LastWriteTime);
				m_timeChange = __builtin_bit_cast(FileTime, basicInfo->ChangeTime);
				return Status::modified;
			}
		}
	public:
		INLINE DynamicFileMonitor()noexcept = default;
		INLINE bool init(auto&&...args)noexcept requires(requires{Path{ FWD(args)... }; })
		{
			DTOR(m_path);
			CTOR(m_path) { FWD(args)... };
			m_monitors.clear();
			m_timeWrite = {};
			m_timeChange = {};
			m_status = Status::no_access;
			if (m_path.size() < 7 ||
				m_path[0] != '\\' ||
				m_path[1] != '?' ||
				m_path[2] != '?' ||
				m_path[3] != '\\' ||
				!((m_path[4] >= 'a' && m_path[4] <= 'z') || (m_path[4] >= 'A' && m_path[4] <= 'Z')) ||
				m_path[5] != ':' ||
				m_path[6] != '\\' ||
				m_path.back() == '\\')
				return false;
			Index monitorCount{ 1 };
			for (Index i{ 7 }; i != m_path.size(); ++i)
				if (m_path[i] == '\\')
				{
					if (m_path[i - 1] == '\\')
						return false;
					++monitorCount;
				}
			m_monitors.resize_uninitialized(monitorCount);
			MonitorImpl* pMonitor{ m_monitors.data() };
			for (Index iDelimiter{ 6 };; ++pMonitor)
			{
				Index iNextDelimiter{ iDelimiter + 1 };
				while (iNextDelimiter != m_path.size() && m_path[iNextDelimiter] != '\\')
					++iNextDelimiter;
				CTOR_AT(pMonitor)
				{
					.ancestor{ m_path.data(), m_path.data() + iDelimiter + 1 },
					.component{ m_path.data() + iDelimiter + 1, m_path.data() + iNextDelimiter },
					.monitor{}
				};
				if (iNextDelimiter == m_path.size())
					break;
				iDelimiter = iNextDelimiter;
			}
			return true;
		}
		INLINE Status update()noexcept
		{
			if (m_status == Status::no_access) [[unlikely]]
			{
				for (MonitorImpl& monitor : m_monitors)
					monitor.monitor.update(monitor.ancestor);
				return (m_status = sync<0>());
			}
			else [[likely]]
			{
				bool found{};
				bool monitor_success{ true };
				for (MonitorImpl& monitor : m_monitors)
					monitor_success &= (Monitor::Status::pending == monitor.monitor.update(monitor.ancestor, [&](const Record& record)LAMBDA_INLINE
					{
						if (Wtf16::iequal(monitor.component, record.name())) [[unlikely]]
						{
							found = true;
							return IterationControl::stop;
						}
						else
							return IterationControl::resume;
					}));
				if (monitor_success && !found) [[likely]]
					return (m_status = Status::not_modified);
				else [[unlikely]]
					return (m_status = sync<1>());
			}
		}
	};
}