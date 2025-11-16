#include <system_error>
#include <exception>
#include <print>
#include <Windows.h>

int wmain(int argc, wchar_t *argv[])
{
	try
	{
		SYSTEMTIME sys_time;

		GetSystemTime(&sys_time);

		FILETIME file_time;

		if (SystemTimeToFileTime(&sys_time, &file_time) == 0)
			throw std::system_error(GetLastError(), std::system_category(), "SystemTimeToFileTime");

		for (int i = 1; i < argc; ++i)
		{
			auto attr = GetFileAttributesW(argv[i]);

			if (attr == INVALID_FILE_ATTRIBUTES)
				throw std::system_error(GetLastError(), std::system_category(), "GetFileAttributesW");

			auto file_h = CreateFileW(argv[i], GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | (attr & FILE_ATTRIBUTE_DIRECTORY ? FILE_FLAG_BACKUP_SEMANTICS : 0), NULL);

			if (file_h == INVALID_HANDLE_VALUE)
				throw std::system_error(GetLastError(), std::system_category(), "CreateFileW");

			auto res = SetFileTime(file_h, &file_time, &file_time, &file_time);
			auto err = GetLastError();

			CloseHandle(file_h);

			if (res == 0)
				throw std::system_error(err, std::system_category(), "SetFileTime");
		}
	}
	catch (const std::exception &ex)
	{
		std::println("{}", ex.what());
	}

	return 0;
}