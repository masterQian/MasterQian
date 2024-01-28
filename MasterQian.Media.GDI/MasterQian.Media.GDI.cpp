#include "../include/MasterQian.Meta.h"
#include "../include/MasterQian.Media.Image.h"

#include <cmath>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define MasterQianModuleName(_) MasterQian_Media_GDI_
META_EXPORT_API_VERSION(20240109ULL)

using namespace MasterQian::Media;

namespace details {
	mqui64 GDIHANDLE;

	static void SetGraphicsMode(Gdiplus::Graphics& graphics, AlgorithmModes mode) {
		graphics.SetCompositingMode(mode.compositing == AlgorithmMode::FAST ?
			Gdiplus::CompositingMode::CompositingModeSourceCopy :
			Gdiplus::CompositingMode::CompositingModeSourceOver);
		graphics.SetCompositingQuality(mode.compositing == AlgorithmMode::FAST ?
			Gdiplus::CompositingQuality::CompositingQualityHighSpeed :
			Gdiplus::CompositingQuality::CompositingQualityHighQuality);
		graphics.SetPixelOffsetMode(mode.pixeloffset == AlgorithmMode::FAST ?
			Gdiplus::PixelOffsetMode::PixelOffsetModeHighSpeed :
			Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
		graphics.SetSmoothingMode(mode.smoothing == AlgorithmMode::FAST ?
			Gdiplus::SmoothingMode::SmoothingModeHighSpeed :
			Gdiplus::SmoothingMode::SmoothingModeHighQuality);
		graphics.SetInterpolationMode(mode.interpolation == AlgorithmMode::FAST ?
			Gdiplus::InterpolationMode::InterpolationModeLowQuality :
			Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	}

	static Gdiplus::Bitmap* CreateEmptyBitmap() {
		return new Gdiplus::Bitmap{ 0, 0 };
	}
}

META_EXPORT_API(void, StartupGDI) {
	Gdiplus::GdiplusStartupInput StartUpInput;
	Gdiplus::GdiplusStartup(&details::GDIHANDLE, &StartUpInput, nullptr);
}

META_EXPORT_API(void, ShutdownGDI) {
	Gdiplus::GdiplusShutdown(details::GDIHANDLE);
}

META_EXPORT_API(Gdiplus::Bitmap*, CreateImageFromSize, ImageSize imageSize, Color color) {
	auto image{ new Gdiplus::Bitmap(imageSize.width, imageSize.height) };
	image->SetResolution(72, 72);
	Gdiplus::Graphics graphics{ image };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	graphics.FillRectangle(&brush, 0, 0, static_cast<mqi32>(imageSize.width), static_cast<mqi32>(imageSize.height));
	return image;
}

META_EXPORT_API(Gdiplus::Bitmap*, CreateImageFromMemory, mqcbytes mem, mqui32 size) {
	IStream* stream{ };
	if (HGLOBAL hGlobal{ GlobalAlloc(GMEM_MOVEABLE, size) }) {
		if (auto hMem{ GlobalLock(hGlobal) }) {
			memcpy(hMem, mem, size);
			(void)CreateStreamOnHGlobal(hGlobal, true, &stream);
		}
	}
	auto image{ new Gdiplus::Bitmap(stream) };
	stream->Release();
	return image;
}

META_EXPORT_API(Gdiplus::Bitmap*, CreateImageFromFile, mqcstr fn) {
	Gdiplus::Bitmap* image{ };
	if (auto hFile{ CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		if (auto hFileMap{ CreateFileMappingW(hFile, nullptr,
			PAGE_READONLY, 0, 0, nullptr) }) {
			if (auto data{ MapViewOfFile(hFileMap, FILE_MAP_READ,
				0, 0, 0) }) {
				LARGE_INTEGER size{ };
				GetFileSizeEx(hFile, &size);
				image = MasterQian_Media_GDI_CreateImageFromMemory(static_cast<mqcbytes>(data), size.LowPart);
				UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
	if (image == nullptr) image = details::CreateEmptyBitmap();
	return image;
}

META_EXPORT_API(Gdiplus::Bitmap*, CreateImageFromFileIcon, mqcstr fn, mqui32 index) {
	auto hIcon{ ExtractIconW(nullptr, fn, index) };
	auto image{ new Gdiplus::Bitmap(hIcon) };
	DestroyIcon(hIcon);
	return image;
}

META_EXPORT_API(Gdiplus::Bitmap*, CreateImageFromClone, Gdiplus::Bitmap* image) {
	auto clone{ image->Clone(0, 0, static_cast<mqi32>(image->GetWidth()), static_cast<mqi32>(image->GetHeight()), PixelFormatDontCare) };
	if (clone == nullptr) return details::CreateEmptyBitmap();
	clone->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	return clone;
}

META_EXPORT_API(void, DeleteImage, Gdiplus::Bitmap* image) {
	delete image;
}

META_EXPORT_API(BOOL, OK, Gdiplus::Bitmap* image) {
	return image->GetLastStatus() == Gdiplus::Status::Ok;
}

META_EXPORT_API(BOOL, SaveToFile, Gdiplus::Bitmap* image, mqcstr fn, ImageFormat ifmt) {
	if (ImageCLSID::_BuiltinFormat(ifmt)) {
		LONG lQuality{ 100 };
		Gdiplus::EncoderParameters paras = { 1U, { Gdiplus::EncoderQuality, 1, Gdiplus::EncoderParameterValueTypeLong, &lQuality } };
		return image->Save(fn, reinterpret_cast<CLSID const*>(&ImageCLSID::_GetEncoder(ifmt)), &paras);
	}
	return false;
}

META_EXPORT_API(IStream*, SaveToStream, Gdiplus::Bitmap* image, ImageFormat ifmt, mqui32* pSize) {
	IStream* stream{ };
	*pSize = 0U;
	if (ImageCLSID::_BuiltinFormat(ifmt)) {
		LONG lQuality{ 100 };
		Gdiplus::EncoderParameters paras{ 1U, { Gdiplus::EncoderQuality, 1, Gdiplus::EncoderParameterValueTypeLong, &lQuality } };
		(void)CreateStreamOnHGlobal(nullptr, true, &stream);
		image->Save(stream, reinterpret_cast<CLSID const*>(&ImageCLSID::_GetEncoder(ifmt)), &paras);
		ULARGE_INTEGER size{ };
		stream->Seek({ }, STREAM_SEEK_END, &size);
		if (size.QuadPart) {
			stream->Seek({ }, STREAM_SEEK_SET, nullptr);
			*pSize = size.LowPart;
		}
		else {
			stream->Release();
			stream = nullptr;
		}
	}
	return stream;
}

META_EXPORT_API(void, StreamReadRelease, IStream* stream, LPBYTE mem, mqui32 size) {
	stream->Read(mem, size, nullptr);
	stream->Release();
}

META_EXPORT_API(ImageSize, Size, Gdiplus::Bitmap* image) {
	return { image->GetWidth(), image->GetHeight() };
}

META_EXPORT_API(ImageDPI, GetDPI, Gdiplus::Bitmap* image) {
	return { static_cast<mqui32>(image->GetHorizontalResolution()),
		static_cast<mqui32>(image->GetVerticalResolution()) };
}

META_EXPORT_API(void, SetDPI, Gdiplus::Bitmap* image, ImageDPI dpi) {
	image->SetResolution(static_cast<Gdiplus::REAL>(dpi.horizontal), static_cast<Gdiplus::REAL>(dpi.vertical));
}

META_EXPORT_API(mqui32, BitDepth, Gdiplus::Bitmap* image) {
	return static_cast<mqui32>((image->GetPixelFormat() >> 8) & 0xFF);
}

META_EXPORT_API(Gdiplus::Bitmap*, Thumbnail, Gdiplus::Bitmap* image, ImageSize imageSize) {
	auto thumbnail{ static_cast<Gdiplus::Bitmap*>(image->GetThumbnailImage(
		imageSize.width, imageSize.height, nullptr, nullptr)) };
	if (thumbnail == nullptr) return details::CreateEmptyBitmap();
	thumbnail->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	return thumbnail;
}

META_EXPORT_API(Gdiplus::Bitmap*, Crop, Gdiplus::Bitmap* image, ImageRect rect, AlgorithmModes mode) {
	auto cropImage{ new Gdiplus::Bitmap(rect.width, rect.height) };
	cropImage->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	Gdiplus::Graphics graphics{ cropImage };
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawImage(image, 0, 0, rect.left, rect.top, rect.width, rect.height, Gdiplus::UnitPixel);
	return cropImage;
}

META_EXPORT_API(Gdiplus::Bitmap*, Border, Gdiplus::Bitmap* image, ImagePosition pos, Color color, AlgorithmModes mode) {
	auto width{ image->GetWidth() }, height{ image->GetHeight() };
	auto new_width{ width + pos.left + pos.right }, new_height{ height + pos.top + pos.bottom };
	auto borderImage{ new Gdiplus::Bitmap(new_width, new_height) };
	borderImage->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	Gdiplus::Graphics graphics{ borderImage };
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	Gdiplus::Rect rects[]{
		{ 0, 0, static_cast<mqi32>(new_width), static_cast<mqi32>(pos.top) },
		{ 0, static_cast<mqi32>(height + pos.top), static_cast<mqi32>(new_width), static_cast<mqi32>(pos.bottom) },
		{ 0, static_cast<mqi32>(pos.top), static_cast<mqi32>(pos.left), static_cast<mqi32>(height) },
		{ static_cast<mqi32>(width + pos.left), static_cast<mqi32>(pos.top), static_cast<mqi32>(pos.right), static_cast<mqi32>(height) }
	};
	details::SetGraphicsMode(graphics, mode);
	graphics.FillRectangles(&brush, rects, 4);
	graphics.DrawImage(image, static_cast<mqi32>(pos.left), static_cast<mqi32>(pos.top));
	return borderImage;
}

META_EXPORT_API(Gdiplus::Bitmap*, Rotate, Gdiplus::Bitmap* image, mqf64 angle, Color color, AlgorithmModes mode) {
	mqi32 old_width{ static_cast<mqi32>(image->GetWidth()) };
	mqi32 old_height{ static_cast<mqi32>(image->GetHeight()) };

	mqf32 angle_f{ static_cast<mqf32>(angle - static_cast<mqi32>(angle) + static_cast<mqi32>(angle) % 360) };
	if (angle_f < 0.0f) angle_f = angle_f + 360.0f;
	mqf32 angle_r{ angle_f * 3.1415926f / 180.0f };
	mqf32 cos_angle_r{ std::cosf(angle_f) }, sin_angle_f{ std::sinf(angle_f) };

	mqf32 dx1{ old_width * cos_angle_r }, dx2{ old_height * sin_angle_f };
	mqf32 dy1{ old_height * cos_angle_r }, dy2{ old_width * sin_angle_f };
	if (dx1 < 0.0f) dx1 = -dx1;
	if (dx2 < 0.0f) dx2 = -dx2;
	if (dy1 < 0.0f) dy1 = -dy1;
	if (dy2 < 0.0f) dy2 = -dy2;
	mqi32 new_width{ static_cast<mqi32>(dx1 + dx2) }, new_height{ static_cast<mqi32>(dy1 + dy2) };

	mqf32 tL{ }, tT{ };
	if (0.0f <= angle_f && angle_f <= 90.0f) {
		tL = dx2;
		tT = 0.0f;
	}
	else if (90.0f <= angle_f && angle_f <= 180.0f) {
		tL = static_cast<mqf32>(new_width);
		tT = dy1;
	}
	else if (180.0f <= angle_f && angle_f <= 270.0f) {
		tL = dx1;
		tT = static_cast<mqf32>(new_height);
	}
	else if (270.0f <= angle_f && angle_f <= 360.0f) {
		tL = 0.0f;
		tT = dy2;
	}

	auto rotateImage{ new Gdiplus::Bitmap{ new_width, new_height } };
	rotateImage->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	Gdiplus::Graphics graphics(rotateImage);
	details::SetGraphicsMode(graphics, mode);
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	graphics.FillRectangle(&brush, 0, 0, new_width, new_height);

	Gdiplus::Matrix matrix;
	matrix.Rotate(angle_f);
	graphics.SetTransform(&matrix);
	graphics.TranslateTransform(tL, tT, Gdiplus::MatrixOrderAppend);
	graphics.DrawImage(image, 0, 0);
	return rotateImage;
}

META_EXPORT_API(void, RotateLeft, Gdiplus::Bitmap* image) {
	image->RotateFlip(Gdiplus::Rotate270FlipNone);
}

META_EXPORT_API(void, RotateRight, Gdiplus::Bitmap* image) {
	image->RotateFlip(Gdiplus::Rotate90FlipNone);
}

META_EXPORT_API(void, FlipX, Gdiplus::Bitmap* image) {
	image->RotateFlip(Gdiplus::RotateNoneFlipX);
}

META_EXPORT_API(void, FlipY, Gdiplus::Bitmap* image) {
	image->RotateFlip(Gdiplus::RotateNoneFlipY);
}

META_EXPORT_API(void, GrayScale, Gdiplus::Bitmap* image, AlgorithmModes mode) {
	Gdiplus::ColorMatrix matrix{
		0.299f, 0.299f, 0.299f, 0.000f, 0.000f,
		0.587f, 0.587f, 0.587f, 0.000f, 0.000f,
		0.114f, 0.114f, 0.114f, 0.000f, 0.000f,
		0.000f, 0.000f, 0.000f, 1.000f, 0.000f,
		0.000f, 0.000f, 0.000f, 0.000f, 1.000f
	};
	auto width{ static_cast<mqi32>(image->GetWidth()) }, height{ static_cast<mqi32>(image->GetHeight()) };
	Gdiplus::ImageAttributes attr;
	attr.SetColorMatrix(&matrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
	Gdiplus::Graphics graphics(image);
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawImage(image, { 0, 0, width, height }, 0, 0, width, height, Gdiplus::UnitPixel, &attr);
}

META_EXPORT_API(Gdiplus::Bitmap*, Resample, Gdiplus::Bitmap* image, ImageSize size, AlgorithmModes mode) {
	auto width{ static_cast<mqi32>(size.width) }, height{ static_cast<mqi32>(size.height) };
	auto resampleImage{ new Gdiplus::Bitmap{ width, height } };
	resampleImage->SetResolution(image->GetHorizontalResolution(), image->GetVerticalResolution());
	Gdiplus::Graphics graphics(resampleImage);
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawImage(image, 0, 0, width, height);
	return resampleImage;
}

META_EXPORT_API(Gdiplus::Bitmap*, Combine, Gdiplus::Bitmap* image1, Gdiplus::Bitmap* image2, ImageRect rect1, ImageRect rect2, BOOL isHorizontal, AlgorithmModes mode) {
	mqi32 new_width{ }, new_height{ };
	if (isHorizontal) {
		new_width = static_cast<mqi32>(rect1.width + rect2.width);
		new_height = rect1.height > rect2.height ? rect1.height : rect2.height;
	}
	else {
		new_width = rect1.width > rect2.width ? rect1.width : rect2.width;
		new_height = static_cast<mqi32>(rect1.height + rect2.height);
	}
	auto combineImage{ new Gdiplus::Bitmap{ new_width, new_height } };
	combineImage->SetResolution(image1->GetHorizontalResolution(), image1->GetVerticalResolution());
	Gdiplus::Graphics graphics(combineImage);
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawImage(image1, 0, 0, static_cast<mqi32>(rect1.left),
		static_cast<mqi32>(rect1.top), static_cast<mqi32>(rect1.width),
		static_cast<mqi32>(rect1.height), Gdiplus::UnitPixel);
	if (isHorizontal) {
		graphics.DrawImage(image2, static_cast<mqi32>(rect1.width), 0, static_cast<mqi32>(rect2.left),
			static_cast<mqi32>(rect2.top), static_cast<mqi32>(rect2.width),
			static_cast<mqi32>(rect2.height), Gdiplus::UnitPixel);
	}
	else {
		graphics.DrawImage(image2, 0, static_cast<mqi32>(rect1.height), static_cast<mqi32>(rect2.left),
			static_cast<mqi32>(rect2.top), static_cast<mqi32>(rect2.width),
			static_cast<mqi32>(rect2.height), Gdiplus::UnitPixel);
	}
	return combineImage;
}

META_EXPORT_API(void, DrawLine, Gdiplus::Bitmap* image, ImageRect rect, Color color, mqf64 thickness, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::Pen pen{ Gdiplus::Color{ color }, static_cast<mqf32>(thickness) };
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawLine(&pen, static_cast<mqi32>(rect.left), static_cast<mqi32>(rect.top),
		static_cast<mqi32>(rect.left + rect.width),
		static_cast<mqi32>(rect.top + rect.height));
}

META_EXPORT_API(void, DrawRectangle, Gdiplus::Bitmap* image, ImageRect rect, Color color, mqf64 thickness, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::Pen pen{ Gdiplus::Color{ color }, static_cast<mqf32>(thickness) };
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawRectangle(&pen, static_cast<mqi32>(rect.left), static_cast<mqi32>(rect.top),
		static_cast<mqi32>(rect.width), static_cast<mqi32>(rect.height));
}

META_EXPORT_API(void, DrawRectangles, Gdiplus::Bitmap* image, ImageRect const* rects, mqui32 count, Color color, mqf64 thickness, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::Pen pen{ Gdiplus::Color{ color }, static_cast<mqf32>(thickness) };
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawRectangles(&pen, reinterpret_cast<Gdiplus::Rect const*>(rects), count);
}

META_EXPORT_API(void, DrawCircle, Gdiplus::Bitmap* image, ImagePoint point, ImageSize r, Color color, mqf64 thickness, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::Pen pen{ Gdiplus::Color{ color }, static_cast<mqf32>(thickness) };
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawEllipse(&pen, static_cast<mqf32>(point.x - r.width), static_cast<mqf32>(point.y - r.height),
		static_cast<mqf32>(r.width * 2), static_cast<mqf32>(r.height * 2));
}

META_EXPORT_API(void, DrawString, Gdiplus::Bitmap* image, ImagePoint point, mqcstr content, mqui32 size, Color color, mqcstr fontname, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	Gdiplus::Font font{ fontname, static_cast<mqf32>(size) };
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	details::SetGraphicsMode(graphics, mode);
	graphics.DrawString(content, -1, &font, { static_cast<mqf32>(point.x), static_cast<mqf32>(point.y) }, &format, &brush);
}

META_EXPORT_API(void, DrawImage, Gdiplus::Bitmap* image1, Gdiplus::Bitmap* image2, ImagePoint point, ImageSize size, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image1);
	details::SetGraphicsMode(graphics, mode);
	if (size == ImageSize{ }) {
		graphics.DrawImage(image2, Gdiplus::Point{ static_cast<mqi32>(point.x), static_cast<mqi32>(point.y) });
	}
	else {
		graphics.DrawImage(image2, Gdiplus::Rect{ static_cast<mqi32>(point.x), static_cast<mqi32>(point.y),
			static_cast<mqi32>(size.width), static_cast<mqi32>(size.height) });
	}
}

META_EXPORT_API(void, FillRectangle, Gdiplus::Bitmap* image, ImageRect rect, Color color, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	details::SetGraphicsMode(graphics, mode);
	graphics.FillRectangle(&brush, static_cast<mqi32>(rect.left), static_cast<mqi32>(rect.top),
		static_cast<mqi32>(rect.width), static_cast<mqi32>(rect.height));
}

META_EXPORT_API(void, FillRectangles, Gdiplus::Bitmap* image, ImageRect const* rects, mqui32 count, Color color, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	details::SetGraphicsMode(graphics, mode);
	graphics.FillRectangles(&brush, reinterpret_cast<Gdiplus::Rect const*>(rects), count);
}

META_EXPORT_API(void, FillCircle, Gdiplus::Bitmap* image, ImagePoint point, ImageSize r, Color color, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(image);
	Gdiplus::SolidBrush brush{ Gdiplus::Color{ color } };
	details::SetGraphicsMode(graphics, mode);
	graphics.FillEllipse(&brush, static_cast<mqf32>(point.x - r.width), static_cast<mqf32>(point.y - r.height),
		static_cast<mqf32>(r.width * 2), static_cast<mqf32>(r.height * 2));
}

META_EXPORT_API(HDC, StartPrinterFromName, mqcstr name) {
	HDC hdcPrint{ CreateDCW(nullptr, name, nullptr, nullptr) };
	if (hdcPrint) {
		DOCINFO docInfo{ };
		docInfo.cbSize = sizeof(docInfo);
		docInfo.lpszDocName = L"GDIPrint";
		StartDocW(hdcPrint, &docInfo);
		StartPage(hdcPrint);
	}
	return hdcPrint;
}

META_EXPORT_API(void, EndPrinter, HDC hdcPrint) {
	if (hdcPrint) {
		EndPage(hdcPrint);
		EndDoc(hdcPrint);
		DeleteDC(hdcPrint);
	}
}

META_EXPORT_API(void, DrawImageToPrinter, HDC hdcPrint, Gdiplus::Bitmap* image, ImagePoint point, ImageSize size, AlgorithmModes mode) {
	Gdiplus::Graphics graphics(hdcPrint);
	details::SetGraphicsMode(graphics, mode);
	if (size == ImageSize{ }) {
		graphics.DrawImage(image, Gdiplus::Point{ static_cast<mqi32>(point.x), static_cast<mqi32>(point.y) });
	}
	else {
		graphics.DrawImage(image, Gdiplus::Rect{ static_cast<mqi32>(point.x), static_cast<mqi32>(point.y),
			static_cast<mqi32>(size.width), static_cast<mqi32>(size.height) });
	}
}