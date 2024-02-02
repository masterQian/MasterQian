#include "../include/MasterQian.Meta.h"
#include "include/hpdf.h"
#include <string>
#ifdef _DEBUG
#pragma comment(lib, "hpdfd.lib")
#else
#pragma comment(lib, "hpdf.lib")
#endif
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_Storage_PDF_##name
META_EXPORT_API_VERSION(20240131ULL)

static std::string WTA(mqcstr str_w) noexcept {
	auto len{ api::WideCharToMultiByte(0U, 0, str_w, -1, nullptr, 0, nullptr, nullptr) };
	std::string str_a;
	if (len > 0) {
		str_a.resize(static_cast<mqui64>(len) - 1ULL);
		api::WideCharToMultiByte(0U, 0, str_w, -1, str_a.data(), len, nullptr, nullptr);
	}
	return str_a;
}

static void SetPageConfig(HPDF_Page page, mqui32 width, mqui32 height) noexcept {
	if (page) {
		HPDF_Page_SetWidth(page, static_cast<HPDF_REAL>(width));
		HPDF_Page_SetHeight(page, static_cast<HPDF_REAL>(height));
	}
}

META_EXPORT_API(HPDF_Doc, Create, mqui32 compress_mode, mqui32 page_show_mode, mqui32 page_layout_mode, mqui32 permission, mqcstr password_owner, mqcstr password_user) {
	HPDF_Doc pdf{ HPDF_New(nullptr, nullptr) };
	if (pdf) {
		HPDF_SetCompressionMode(pdf, compress_mode);
		HPDF_SetPageMode(pdf, static_cast<HPDF_PageMode>(page_show_mode));
		HPDF_SetPageLayout(pdf, static_cast<HPDF_PageLayout>(page_layout_mode));
		if (permission != static_cast<mqui32>(-1)) {
			HPDF_SetEncryptionMode(pdf, HPDF_ENCRYPT_R3, 16);
			HPDF_SetPermission(pdf, permission);
		}
		if (*password_owner) {
			auto password_owner_ansi{ WTA(password_owner) };
			auto password_user_ansi{ WTA(password_user) };
			HPDF_SetPassword(pdf, password_owner_ansi.data(), password_user_ansi.data());
		}
		HPDF_UseCNSEncodings(pdf);
		HPDF_UseCNSFonts(pdf);
	}
	return pdf;
}

META_EXPORT_API(void, Delete, HPDF_Doc pdf) {
	HPDF_Free(pdf);
}

META_EXPORT_API(mqui32, GetError, HPDF_Doc pdf) {
	return HPDF_GetError(pdf);
}

META_EXPORT_API(void, ResetError, HPDF_Doc pdf) {
	HPDF_ResetError(pdf);
}

META_EXPORT_API(mqui32, SaveToMemory, HPDF_Doc pdf) {
	if (HPDF_SaveToStream(pdf) == HPDF_OK) {
		return HPDF_GetStreamSize(pdf);
	}
	return 0U;
}

META_EXPORT_API(void, ReadFromMemory, HPDF_Doc pdf, mqbytes mem, mqui32 size) {
	HPDF_ReadFromStream(pdf, mem, &size);
	HPDF_ResetStream(pdf);
}

META_EXPORT_API(void, SaveToFile, HPDF_Doc pdf, mqcstr filename) {
	std::string filename_ansi{ WTA(filename) };
	HPDF_SaveToFile(pdf, filename_ansi.data());
}

META_EXPORT_API(HPDF_Page, CurrentPage, HPDF_Doc pdf) {
	return HPDF_GetCurrentPage(pdf);
}

META_EXPORT_API(HPDF_Page, AddPage, HPDF_Doc pdf, mqui32 width, mqui32 height) {
	HPDF_Page page{ HPDF_AddPage(pdf) };
	SetPageConfig(page, width, height);
	return page;
}

META_EXPORT_API(HPDF_Page, InsertPage, HPDF_Doc pdf, HPDF_Page pos, mqui32 width, mqui32 height) {
	HPDF_Page page{ HPDF_InsertPage(pdf, pos) };
	SetPageConfig(page, width, height);
	return page;
}

META_EXPORT_API(HPDF_Font, GetSimSunFont, HPDF_Doc pdf) {
	return HPDF_GetFont(pdf, "SimSun", "GB-EUC-H");
}

META_EXPORT_API(HPDF_Font, GetSimHeiFont, HPDF_Doc pdf) {
	return HPDF_GetFont(pdf, "SimHei", "GB-EUC-H");
}

META_EXPORT_API(HPDF_Font, LoadFont, HPDF_Doc pdf, mqcstr filename, mqbool embedding) {
	std::string filename_ansi{ WTA(filename) };
	mqcstra font_name{ };
	if (!filename_ansi.empty()) {
		if (filename_ansi.back() == 'f') { // ttf
			font_name = HPDF_LoadTTFontFromFile(pdf, filename_ansi.data(), embedding);
		}
		else { // ttc
			font_name = HPDF_LoadTTFontFromFile2(pdf, filename_ansi.data(), 0U, embedding);
		}
	}
	return HPDF_GetFont(pdf, font_name, "GB-EUC-H");
}

META_EXPORT_API(HPDF_Image, LoadJPGImageFromFile, HPDF_Doc pdf, mqcstr filename) {
	std::string filename_ansi{ WTA(filename) };
	return HPDF_LoadJpegImageFromFile(pdf, filename_ansi.data());
}

META_EXPORT_API(HPDF_Image, LoadPNGImageFromFile, HPDF_Doc pdf, mqcstr filename) {
	std::string filename_ansi{ WTA(filename) };
	return HPDF_LoadPngImageFromFile(pdf, filename_ansi.data());
}

META_EXPORT_API(HPDF_Image, LoadJPGImageFromMemory, HPDF_Doc pdf, mqcbytes mem, mqui32 size) {
	return HPDF_LoadJpegImageFromMem(pdf, mem, size);
}

META_EXPORT_API(HPDF_Image, LoadPNGImageFromMemory, HPDF_Doc pdf, mqcbytes mem, mqui32 size) {
	return HPDF_LoadPngImageFromMem(pdf, mem, size);
}

META_EXPORT_API(void, BeginAddText, HPDF_Page page, HPDF_Font font, mqui32 font_size) {
	HPDF_Page_SetFontAndSize(page, font, static_cast<HPDF_REAL>(font_size));
	HPDF_Page_BeginText(page);
}

META_EXPORT_API(void, EndAddText, HPDF_Page page) {
	HPDF_Page_EndText(page);
}

META_EXPORT_API(void, AddText, HPDF_Page page, mqcstr content, mqpoint point) {
	std::string content_ansi{ WTA(content) };
	HPDF_Page_TextOut(page, static_cast<HPDF_REAL>(point.x),
		HPDF_Page_GetHeight(page) - static_cast<HPDF_REAL>(point.y) - HPDF_Page_GetCurrentFontSize(page),
		content_ansi.data());
}

META_EXPORT_API(mqsize, GetTextSize, HPDF_Page page, mqcstr content) {
	std::string content_ansi{ WTA(content) };
	return { static_cast<mqui32>(HPDF_Page_TextWidth(page, content_ansi.data())),
		static_cast<mqui32>(HPDF_Page_GetCurrentFontSize(page)) };
}

META_EXPORT_API(void, AddImage, HPDF_Page page, HPDF_Image image, mqpoint point, mqsize size) {
	HPDF_REAL width{ }, height{ };
	if (size == mqsize{ }) {
		width = static_cast<HPDF_REAL>(HPDF_Image_GetWidth(image));
		height = static_cast<HPDF_REAL>(HPDF_Image_GetHeight(image));
	}
	else {
		width = static_cast<HPDF_REAL>(size.width);
		height = static_cast<HPDF_REAL>(size.height);
	}
	HPDF_Page_DrawImage(page, image, static_cast<HPDF_REAL>(point.x),
		HPDF_Page_GetHeight(page) - static_cast<HPDF_REAL>(point.y) - height, width, height);
}