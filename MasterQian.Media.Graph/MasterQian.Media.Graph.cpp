#include "../include/MasterQian.Meta.h"
#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
import MasterQian.freestanding;
import MasterQian.Media.Color;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_Media_Graph_##name
META_EXPORT_API_VERSION(20240205ULL)

template<typename T1, typename T2>
requires (sizeof(T1) == sizeof(T2))
inline T1 cast(T2 const& t2) noexcept {
	T1 t1{ };
	freestanding::copy(&t1, &t2, sizeof(T1));
	return t1;
}

template<>
inline Gdiplus::PointF cast<Gdiplus::PointF, mqpoint>(mqpoint const& t2) noexcept {
	return { static_cast<mqf32>(t2.x), static_cast<mqf32>(t2.y) };
}

struct TextFormatShim {
	mqui32 mSize;
	Media::Color mColor;
	mqcstr mFont;
};

struct PenFormatShim {
	Media::Color mColor;
	mqui32 mSize;
};

// Line
static void __stdcall Line_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto& obj{ *static_cast<PenFormatShim const*>(shim) };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ obj.mColor } };
	Gdiplus::Pen pen{ &brush, static_cast<mqf32>(obj.mSize) };
	g->DrawLine(&pen, static_cast<mqf32>(rect->left), static_cast<mqf32>(rect->top),
		static_cast<mqf32>(rect->left + rect->width), static_cast<mqf32>(rect->top + rect->height));
}

// Rectangle
static void __stdcall Rectangle_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto& obj{ *static_cast<PenFormatShim const*>(shim) };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ obj.mColor } };
	if (obj.mSize) {
		Gdiplus::Pen pen{ &brush, static_cast<mqf32>(obj.mSize) };
		g->DrawRectangle(&pen, cast<Gdiplus::Rect>(*rect));
	}
	else {
		g->FillRectangle(&brush, cast<Gdiplus::Rect>(*rect));
	}
}

// Circle
static void __stdcall Circle_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto& obj{ *static_cast<PenFormatShim const*>(shim) };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ obj.mColor } };
	if (obj.mSize) {
		Gdiplus::Pen pen{ &brush, static_cast<mqf32>(obj.mSize) };
		g->DrawEllipse(&pen, cast<Gdiplus::Rect>(*rect));
	}
	else {
		g->FillEllipse(&brush, cast<Gdiplus::Rect>(*rect));
	}
}

// Text

struct TextShim {
	mqcstr mContent;
	TextFormatShim mFormat;
};

static void __stdcall Text_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto& obj{ *static_cast<TextShim const*>(shim) };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ obj.mFormat.mColor } };
	Gdiplus::Font font{ obj.mFormat.mFont, static_cast<mqf32>(obj.mFormat.mSize) };
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	Gdiplus::PointF point{ static_cast<mqf32>(rect->left + rect->width / 2), static_cast<mqf32>(rect->top + rect->height / 2) };
	g->DrawString(obj.mContent, -1, &font, point, &format, &brush);
}

// Picture

struct PictureShim {
	Gdiplus::Bitmap* mImage;
};

static void __stdcall Picture_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto obj{ *static_cast<PictureShim const*>(shim) };
	if (obj.mImage) {
		obj.mImage->SetResolution(72.0f, 72.0f);
		g->DrawImage(obj.mImage, cast<Gdiplus::Rect>(*rect));
	}
}

// TrilineChart

struct TrilineChartShim {
	mqarray<mqcstr> mTitles;
	mqarray<mqarray<mqcstr>> mContents;
	TextFormatShim mTitleFormat;
	TextFormatShim mContentFormat;
};

static void __stdcall TrilineChart_Render(Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim) noexcept {
	auto& obj{ *static_cast<TrilineChartShim const*>(shim) };
	// 行列数
	auto col{ obj.mTitles.size() }, row{ obj.mContents.size() };
	if (col == 0ULL) return;
	// 线条格式
	Gdiplus::SolidBrush line_brush{ Gdiplus::Color::Black };
	Gdiplus::Pen bold_pen{ &line_brush, 3U }, thin_pen{ &line_brush, 1U };
	// 标题格式
	Gdiplus::SolidBrush title_brush{ Gdiplus::Color{ obj.mTitleFormat.mColor } };
	Gdiplus::Font title_font{ obj.mTitleFormat.mFont, static_cast<mqf32>(obj.mTitleFormat.mSize), Gdiplus::FontStyle::FontStyleBold };
	// 内容格式
	Gdiplus::SolidBrush content_brush{ Gdiplus::Color{ obj.mContentFormat.mColor } };
	Gdiplus::Font content_font{ obj.mContentFormat.mFont, static_cast<mqf32>(obj.mContentFormat.mSize) };
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	// 边距计算
	auto padding_x{ rect->width / 32.0f }, padding_y{ rect->height / 32.0f };
	auto width{ rect->width * 15.0f / 16 }, height{ rect->height * 15.0f / 16 };
	auto title_height{ title_font.GetHeight(g) + 4 };
	auto item_width{ width / col }, item_height{ content_font.GetHeight(g) + 4 };
	if (auto show_row{ static_cast<mqui64>((height - 4U - title_height) / item_height) }; show_row < row) {
		row = show_row;
	}
	mqf32 x1{ padding_x + rect->left }, y0{ padding_y + rect->top }, x2{ x1 + width }, x{ }, y{ };
	// 画1线
	g->DrawLine(&bold_pen, x1, y0, x2, y0);
	y0 += 3U;
	// 画标题
	x = x1;
	y = y0;
	for (auto& title : obj.mTitles) {
		g->DrawString(title, -1, &title_font, { x + item_width / 2, y + title_height / 2 + 2 }, &format, &title_brush);
		x += item_width;
	}
	y0 += title_height;
	// 画2线
	g->DrawLine(&thin_pen, x1, y0, x2, y0);
	y0 += 1U;
	// 画内容
	y = y0;
	for (mqui64 i{ }; i < row; ++i) {
		x = x1;
		auto& content_row{ obj.mContents[i] };
		for (mqui64 j{ }; j < col; ++j) {
			g->DrawString(content_row[j], -1, &content_font, { x + item_width / 2, y + item_height / 2 + 2 }, &format, &content_brush);
			x += item_width;
		}
		y += item_height;
	}
	y0 = y;
	// 画3线
	g->DrawLine(&bold_pen, x1, y0, x2, y0);
}


META_EXPORT_API(Gdiplus::Bitmap*, Canvas_Create, mqsize size, Media::Color background) {
	auto context{ new Gdiplus::Bitmap{ static_cast<mqi32>(size.width), static_cast<mqi32>(size.height) } };
	context->SetResolution(72.0f, 72.0f);
	Gdiplus::Graphics g{ context };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ background } };
	g.FillRectangle(&brush, 0, 0, static_cast<mqi32>(size.width), static_cast<mqi32>(size.height));
	return context;
}

META_EXPORT_API(void, Canvas_Delete, Gdiplus::Bitmap* context) {
	delete context;
}

META_EXPORT_API(Gdiplus::Graphics*, Canvas_BeginRender, Gdiplus::Bitmap* context) {
	auto g{ new Gdiplus::Graphics{ context } };
	g->SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceOver);
	g->SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
	g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
	g->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	g->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
	return g;
}

META_EXPORT_API(void, Canvas_EndRender, Gdiplus::Graphics* g) {
	delete g;
}

META_EXPORT_API(void, Canvas_Render, Gdiplus::Graphics* g, mqrect const* rect, mqcmem shim, mqenum type) {
	using RenderFunc = void(__stdcall*)(Gdiplus::Graphics*, mqrect const*, mqcmem) noexcept;
	constexpr RenderFunc type_map[] = {
		&Line_Render,
		&Rectangle_Render,
		&Circle_Render,
		&Text_Render,
		&Picture_Render,
		&TrilineChart_Render,
	};
	if (auto render_func{ type_map[type] }) {
		render_func(g, rect, shim);
	}
}

META_EXPORT_API(IStream*, Canvas_SaveToStream, Gdiplus::Bitmap* context, mqui32* pSize) {
	constexpr mqguid PNG_E{ 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
	IStream* stream{ };
	LONG lQuality{ 100 };
	Gdiplus::EncoderParameters paras{ 1U, { Gdiplus::EncoderQuality, 1, Gdiplus::EncoderParameterValueTypeLong, &lQuality } };
	(void)CreateStreamOnHGlobal(nullptr, true, &stream);
	context->Save(stream, reinterpret_cast<CLSID const*>(&PNG_E), &paras);
	ULARGE_INTEGER size{ };
	stream->Seek({ }, STREAM_SEEK_END, &size);
	if (size.QuadPart) {
		stream->Seek({ }, STREAM_SEEK_SET, nullptr);
		*pSize = size.LowPart;
	}
	else {
		stream->Release();
		stream = nullptr;
		*pSize = 0U;
	}
	return stream;
}

META_EXPORT_API(void, Canvas_StreamReadRelease, IStream* stream, mqbytes mem, mqui32 size) {
	stream->Read(mem, size, nullptr);
	stream->Release();
}