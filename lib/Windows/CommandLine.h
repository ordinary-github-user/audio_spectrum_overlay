#pragma once
#include "TibTebPeb.h"
namespace CommandLine
{
	INLINE UnicodeString const& all()noexcept { return Windows::thisPeb().pProcessParameters->CommandLine; }

	INLINE StringView16 excludeExePath() noexcept
	{
		const StringView16 cmd{ all() };
		if (cmd.empty())return {};
		else
		{
			const c16* pStart;
			if (cmd[0] == '"')
			{
				//"exe.exe"     mhw.exe
				//"exe.exe"     "mhw.exe"
				const c16* pSecondQuotationMark{ cmd.substr(1).find('"') };
				if (!pSecondQuotationMark)[[unlikely]]return {};//"exe.exe second quote missing
				pStart = pSecondQuotationMark + 1;
			}
			else
			{
				//exe.exe     mhw.exe
				//exe.exe     "mhw.exe"
				const c16* pSpace{ cmd.find(' ') };
				if (!pSpace)return {};//exe.exe
				pStart = pSpace + 1;
			}
			while (pStart != cmd.end() && *pStart == ' ')++pStart;
			const StringView16 result{ pStart, cmd.end() };
			if (result.size() > 2 && result.front() == '"' && result.back() == '"')return result.substr(1, result.size() - 2);
			else return result;
		}
	}
};