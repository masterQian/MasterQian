#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"
#include "MasterQian.Media.Image.h"

#define MasterQianModuleName(_) MasterQian_Media_GDI_
#define MasterQianModuleNameString "MasterQian_Media_GDI_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Media.GDI.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Media.GDI.dll"
#endif
#define MasterQianModuleVersion 20240109ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::Media::GDI {
	namespace details {
		META_IMPORT_API(void, StartupGDI);
		META_IMPORT_API(void, ShutdownGDI);
		META_IMPORT_API(mqhandle, CreateImageFromSize, ImageSize, Color);
		META_IMPORT_API(mqhandle, CreateImageFromMemory, mqcbytes, mqui32);
		META_IMPORT_API(mqhandle, CreateImageFromFile, mqcstr);
		META_IMPORT_API(mqhandle, CreateImageFromFileIcon, mqcstr, mqui32);
		META_IMPORT_API(mqhandle, CreateImageFromClone, mqhandle);
		META_IMPORT_API(void, DeleteImage, mqhandle);
		META_IMPORT_API(mqbool, OK, mqhandle);
		META_IMPORT_API(mqbool, SaveToFile, mqhandle, mqcstr, ImageFormat);
		META_IMPORT_API(mqhandle, SaveToStream, mqhandle, ImageFormat, mqui32*);
		META_IMPORT_API(void, StreamReadRelease, mqhandle, mqbytes, mqui32);
		META_IMPORT_API(ImageSize, Size, mqhandle);
		META_IMPORT_API(ImageDPI, GetDPI, mqhandle);
		META_IMPORT_API(void, SetDPI, mqhandle, ImageDPI);
		META_IMPORT_API(mqui32, BitDepth, mqhandle);
		META_IMPORT_API(mqhandle, Thumbnail, mqhandle, ImageSize);
		META_IMPORT_API(mqhandle, Crop, mqhandle, ImageRect, AlgorithmModes);
		META_IMPORT_API(mqhandle, Border, mqhandle, ImagePosition, Color, AlgorithmModes);
		META_IMPORT_API(mqhandle, Rotate, mqhandle, mqf64, Color, AlgorithmModes);
		META_IMPORT_API(void, RotateLeft, mqhandle);
		META_IMPORT_API(void, RotateRight, mqhandle);
		META_IMPORT_API(void, FlipX, mqhandle);
		META_IMPORT_API(void, FlipY, mqhandle);
		META_IMPORT_API(void, GrayScale, mqhandle, AlgorithmModes);
		META_IMPORT_API(mqhandle, Resample, mqhandle, ImageSize, AlgorithmModes);
		META_IMPORT_API(mqhandle, Combine, mqhandle, mqhandle, ImageRect, ImageRect, mqbool, AlgorithmModes);
		META_IMPORT_API(void, DrawLine, mqhandle, ImageRect, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawRectangle, mqhandle, ImageRect, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawRectangles, mqhandle, ImageRect const*, mqui32, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawCircle, mqhandle, ImagePoint, ImageSize, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawString, mqhandle, ImagePoint, mqcstr, mqui32, Color, mqcstr, AlgorithmModes);
		META_IMPORT_API(void, DrawImage, mqhandle, mqhandle, ImagePoint, ImageSize, AlgorithmModes);
		META_IMPORT_API(void, FillRectangle, mqhandle, ImageRect, Color, AlgorithmModes);
		META_IMPORT_API(void, FillRectangles, mqhandle, ImageRect const*, mqui32, Color, AlgorithmModes);
		META_IMPORT_API(void, FillCircle, mqhandle, ImagePoint, ImageSize, Color, AlgorithmModes);
		META_IMPORT_API(mqhandle, StartPrinterFromName, mqcstr);
		META_IMPORT_API(void, EndPrinter, mqhandle);
		META_IMPORT_API(void, DrawImageToPrinter, mqhandle, mqhandle, ImagePoint, ImageSize, AlgorithmModes);

		META_MODULE_BEGIN
			META_PROC_API(StartupGDI);
			META_PROC_API(ShutdownGDI);
			META_PROC_API(CreateImageFromSize);
			META_PROC_API(CreateImageFromMemory);
			META_PROC_API(CreateImageFromFile);
			META_PROC_API(CreateImageFromFileIcon);
			META_PROC_API(CreateImageFromClone);
			META_PROC_API(DeleteImage);
			META_PROC_API(OK);
			META_PROC_API(SaveToFile);
			META_PROC_API(SaveToStream);
			META_PROC_API(StreamReadRelease);
			META_PROC_API(Size);
			META_PROC_API(GetDPI);
			META_PROC_API(SetDPI);
			META_PROC_API(BitDepth);
			META_PROC_API(Thumbnail);
			META_PROC_API(Crop);
			META_PROC_API(Border);
			META_PROC_API(Rotate);
			META_PROC_API(RotateLeft);
			META_PROC_API(RotateRight);
			META_PROC_API(FlipX);
			META_PROC_API(FlipY);
			META_PROC_API(GrayScale);
			META_PROC_API(Resample);
			META_PROC_API(Combine);
			META_PROC_API(DrawLine);
			META_PROC_API(DrawRectangle);
			META_PROC_API(DrawRectangles);
			META_PROC_API(DrawCircle);
			META_PROC_API(DrawString);
			META_PROC_API(DrawImage);
			META_PROC_API(FillRectangle);
			META_PROC_API(FillRectangles);
			META_PROC_API(FillCircle);
			META_PROC_API(StartPrinterFromName);
			META_PROC_API(EndPrinter);
			META_PROC_API(DrawImageToPrinter);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 开启GDI
	/// </summary>
	inline void StartupGDI() noexcept {
		details::MasterQian_Media_GDI_StartupGDI();
	}

	/// <summary>
	/// 关闭GDI
	/// </summary>
	inline void ShutdownGDI() noexcept {
		details::MasterQian_Media_GDI_ShutdownGDI();
	}

	/// <summary>
	/// 图像
	/// </summary>
	struct Image {
		friend struct Printer;
	private:
		mqhandle handle{ };

		static constexpr struct _placeholder {}_unused;

		Image(mqhandle h, _placeholder) noexcept {
			handle = h;
		}

	public:
		/// <summary>
		/// 构造纯色图像
		/// </summary>
		/// <param name="size">尺寸</param>
		/// <param name="color">颜色</param>
		Image(ImageSize size, Color color) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromSize(size, color);
		}

		/// <summary>
		/// 从文件构造
		/// </summary>
		/// <param name="fn">文件名</param>
		explicit Image(std::wstring_view fn) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromFile(fn.data());
		}

		/// <summary>
		/// 从程序图标构造
		/// </summary>
		/// <param name="fileIconPath">可执行程序或DLL路径</param>
		/// <param name="index">图标索引</param>
		Image(std::wstring_view fileIconPath, mqui32 index) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromFileIcon(fileIconPath.data(), index);
		}

		/// <summary>
		/// 从内存构造
		/// </summary>
		/// <param name="bv">字节集</param>
		explicit Image(BinView bv) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromMemory(bv.data(), bv.size32());
		}

		Image(Image const& image) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromClone(image.handle);
		}

		Image(Image&& image) noexcept {
			std::swap(handle, image.handle);
		}

		Image& operator = (Image const& image) noexcept {
			if (&image != this) {
				details::MasterQian_Media_GDI_DeleteImage(handle);
				handle = details::MasterQian_Media_GDI_CreateImageFromClone(image.handle);
			}
			return *this;
		}

		Image& operator = (Image&& image) noexcept {
			std::swap(handle, image.handle);
			return *this;
		}

		~Image() noexcept {
			details::MasterQian_Media_GDI_DeleteImage(handle);
		}

		/// <summary>
		/// 保存到文件
		/// </summary>
		/// <param name="fn">文件名</param>
		/// <param name="format">图像格式</param>
		bool Save(std::wstring_view fn, ImageFormat format) const noexcept {
			return details::MasterQian_Media_GDI_SaveToFile(handle, fn.data(), format);
		}

		/// <summary>
		/// 保存到内存
		/// </summary>
		/// <param name="format">图像格式</param>
		[[nodiscard]] Bin Save(ImageFormat format) const noexcept {
			Bin bin;
			mqui32 size{ };
			if (auto hStream{ details::MasterQian_Media_GDI_SaveToStream(handle, format, &size) }) {
				bin.resize(size);
				details::MasterQian_Media_GDI_StreamReadRelease(hStream, bin.data(), size);
			}
			return bin;
		}

		/// <summary>
		/// 保存到流，此方法危险，保存的COM流为IStream*，应由用户自己主动Release
		/// </summary>
		/// <param name="format">图像格式</param>
		[[nodiscard]] mqhandle SaveToUnsafeStream(ImageFormat format) const noexcept {
			mqui32 size{ };
			return details::MasterQian_Media_GDI_SaveToStream(handle, format, &size);
		}

		[[nodiscard]] bool OK() const noexcept {
			return details::MasterQian_Media_GDI_OK(handle);
		}

		/// <summary>
		/// 取宽度
		/// </summary>
		[[nodiscard]] mqui32 Width() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle).width;
		}

		/// <summary>
		/// 取高度
		/// </summary>
		[[nodiscard]] mqui32 Height() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle).height;
		}

		/// <summary>
		/// 取尺寸
		/// </summary>
		[[nodiscard]] ImageSize Size() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle);
		}

		/// <summary>
		/// 取分辨率
		/// </summary>
		[[nodiscard]] ImageDPI DPI() const noexcept {
			return details::MasterQian_Media_GDI_GetDPI(handle);
		}

		/// <summary>
		/// 置分辨率
		/// </summary>
		/// <param name="dpi">分辨率</param>
		void DPI(ImageDPI dpi) noexcept {
			details::MasterQian_Media_GDI_SetDPI(handle, dpi);
		}

		/// <summary>
		/// 取位深度
		/// </summary>
		[[nodiscard]] mqui32 BitDepth() const noexcept {
			return details::MasterQian_Media_GDI_BitDepth(handle);
		}

		/// <summary>
		/// 取缩略图
		/// </summary>
		/// <param name="size">尺寸</param>
		[[nodiscard]] Image Thumbnail(ImageSize size) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Thumbnail(handle, size), _unused };
		}

		/// <summary>
		/// 裁剪
		/// </summary>
		/// <param name="rect">裁剪矩形</param>
		/// <param name="mode">算法</param>
		[[nodiscard]] Image Crop(ImageRect rect, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Crop(handle, rect, mode), _unused };
		}

		/// <summary>
		/// 边框
		/// </summary>
		/// <param name="pos">边框宽度</param>
		/// <param name="color">边框颜色</param>
		/// <param name="mode">算法</param>
		[[nodiscard]] Image Border(ImagePosition pos, Color color, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Border(handle, pos, color, mode), _unused };
		}

		/// <summary>
		/// 旋转
		/// </summary>
		/// <param name="angle">旋转角度，角度制</param>
		/// <param name="color">新区域颜色，默认是透明</param>
		/// <param name="mode">算法</param>
		[[nodiscard]] Image Rotate(mqf64 angle, Color color = Colors::Transparency, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Rotate(handle, angle, color, mode), _unused };
		}

		/// <summary>
		/// 左旋转90度
		/// </summary>
		Image& RotateLeft() noexcept {
			details::MasterQian_Media_GDI_RotateLeft(handle);
			return *this;
		}

		/// <summary>
		/// 右旋转90度
		/// </summary>
		Image& RotateRight() noexcept {
			details::MasterQian_Media_GDI_RotateRight(handle);
			return *this;
		}

		/// <summary>
		/// 水平翻转
		/// </summary>
		Image& FlipX() noexcept {
			details::MasterQian_Media_GDI_FlipX(handle);
			return *this;
		}

		/// <summary>
		/// 垂直翻转
		/// </summary>
		Image& FlipY() noexcept {
			details::MasterQian_Media_GDI_FlipY(handle);
			return *this;
		}

		/// <summary>
		/// 灰度图
		/// </summary>
		/// <param name="mode">算法</param>
		Image& GrayScale(AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_GrayScale(handle, mode);
			return *this;
		}

		/// <summary>
		/// 重新采样
		/// </summary>
		/// <param name="size">尺寸</param>
		/// <param name="mode">算法</param>
		[[nodiscard]] Image Resample(ImageSize size, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Resample(handle, size, mode), _unused };
		}

		/// <summary>
		/// 合并，将两个图像拼接
		/// </summary>
		/// <param name="image">图像2</param>
		/// <param name="rect1">图像1的拼接区域</param>
		/// <param name="rect2">图像2的拼接区域</param>
		/// <param name="mode">算法</param>
		/// <typeparam name="isHorizontal">是否左右拼接，反之上下拼接</typeparam>
		template<bool isHorizontal = true>
		[[nodiscard]] Image Combine(Image const& image, ImageRect rect1 = { }, ImageRect rect2 = { }, AlgorithmModes mode = FAST_MODE) const noexcept {
			if (rect1 == ImageRect{ }) {
				auto [width, height] = Size();
				rect1.width = width;
				rect1.height = height;
			}
			if (rect2 == ImageRect{ }) {
				auto [width, height] = image.Size();
				rect2.width = width;
				rect2.height = height;
			}
			return Image{ details::MasterQian_Media_GDI_Combine(handle, image.handle, rect1, rect2, isHorizontal, mode), _unused };
		}

		/// <summary>
		/// 画线
		/// </summary>
		/// <param name="rect">线所在矩形区域</param>
		/// <param name="color">线条颜色</param>
		/// <param name="thickness">线条粗细</param>
		/// <param name="mode">算法</param>
		Image& DrawLine(ImageRect rect, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawLine(handle, rect, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// 画矩形
		/// </summary>
		/// <param name="rect">矩形区域</param>
		/// <param name="color">线条颜色</param>
		/// <param name="thickness">线条粗细</param>
		/// <param name="mode">算法</param>
		Image& DrawRectangle(ImageRect rect, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawRectangle(handle, rect, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// 画多个矩形
		/// </summary>
		/// <param name="rects">矩形区域集合</param>
		/// <param name="color">线条颜色</param>
		/// <param name="thickness">线条粗细</param>
		/// <param name="mode">算法</param>
		Image& DrawRectangles(std::vector<ImageRect> const& rects, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawRectangles(handle, rects.data(), static_cast<mqui32>(rects.size()), color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// 画椭圆
		/// </summary>
		/// <param name="point">圆心位置</param>
		/// <param name="r">长轴与短轴半径</param>
		/// <param name="color">线条颜色</param>
		/// <param name="thickness">线条粗细</param>
		/// <param name="mode">算法</param>
		Image& DrawCircle(ImagePoint point, ImageSize r, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawCircle(handle, point, r, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// 写文本
		/// </summary>
		/// <param name="point">文本中心位置</param>
		/// <param name="textInfo">文本信息</param>
		/// <param name="mode">算法</param>
		Image& DrawString(ImagePoint point, ImageText const& textInfo, AlgorithmModes mode = FAST_MODE) noexcept {
			mode.compositing = AlgorithmMode::QUALITY;
			details::MasterQian_Media_GDI_DrawString(handle, point, textInfo.content,
				textInfo.size, textInfo.color, textInfo.font, mode);
			return *this;
		}

		/// <summary>
		/// 画图像
		/// </summary>
		/// <param name="image">图像2</param>
		/// <param name="point">图像2在原图像位置</param>
		/// <param name="size">图像2尺寸</param>
		/// <param name="mode">算法</param>
		Image& DrawImage(Image const& image, ImagePoint point, ImageSize size = { }, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawImage(handle, image.handle, point, size, mode);
			return *this;
		}

		/// <summary>
		/// 填充矩形
		/// </summary>
		/// <param name="rect">矩形区域</param>
		/// <param name="color">矩形颜色</param>
		/// <param name="mode">算法</param>
		Image& FillRectangle(ImageRect rect, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillRectangle(handle, rect, color, mode);
			return *this;
		}

		/// <summary>
		/// 填充多个矩形
		/// </summary>
		/// <param name="rects">矩形区域集合</param>
		/// <param name="color">填充颜色</param>
		/// <param name="mode">算法</param>
		Image& FillRectangles(std::vector<ImageRect> const& rects, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillRectangles(handle, rects.data(), static_cast<mqui32>(rects.size()), color, mode);
			return *this;
		}

		/// <summary>
		/// 填充椭圆
		/// </summary>
		/// <param name="point">圆心位置</param>
		/// <param name="r">长轴与短轴半径</param>
		/// <param name="color">填充颜色</param>
		/// <param name="mode">算法</param>
		Image& FillCircle(ImagePoint point, ImageSize r, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillCircle(handle, point, r, color, mode);
			return *this;
		}
	};

	/// <summary>
	/// 打印机
	/// <para>此对象创建时开启打印机，析构时完成打印</para>
	/// </summary>
	struct Printer {
	private:
		mqhandle handle{ };
	public:
		/// <summary>
		/// 调用打印机
		/// </summary>
		/// <param name="name">打印机名称</param>
		explicit Printer(std::wstring_view name) noexcept {
			handle = details::MasterQian_Media_GDI_StartPrinterFromName(name.data());
		}

		Printer(Printer const&) noexcept = delete;
		Printer& operator = (Printer const&) noexcept = delete;

		Printer(Printer&& printer) noexcept {
			std::swap(handle, printer.handle);
		}
		
		Printer& operator = (Printer&& printer) noexcept {
			std::swap(handle, printer.handle);
			return *this;
		}

		~Printer() noexcept {
			details::MasterQian_Media_GDI_EndPrinter(handle);
		}

		[[nodiscard]] bool OK() const noexcept {
			return handle != nullptr;
		}

		/// <summary>
		/// 画图像
		/// </summary>
		/// <param name="image">图像</param>
		/// <param name="point">位置</param>
		/// <param name="size">图像尺寸，默认按源尺寸</param>
		/// <param name="mode">算法</param>
		Printer& DrawImage(Image const& image, ImagePoint point, ImageSize size = { }, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawImageToPrinter(handle, image.handle, point, size, mode);
			return *this;
		}
	};
}