#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Media.Color.h"

#define MasterQianModuleVersion 20240108ULL
#undef MasterQianModuleVersion

namespace MasterQian::Media {
	/// <summary>
	/// ͼ���ʽ
	/// </summary>
	enum class ImageFormat : mqenum {
		BMP = 0, JPG = 1, GIF = 2, TIFF = 5, PNG = 6,
		ICO = 10,
		TGA = 20,
		MNG = 30,
		RAW = 40,
		PSD = 50,
		UNKNOWN = static_cast<mqenum>(-1),
	};

	/// <summary>
	/// �㷨ģʽ
	/// </summary>
	enum class AlgorithmMode : bool {
		FAST, QUALITY
	};

	/// <summary>
	/// �㷨ģʽ��
	/// </summary>
	struct AlgorithmModes {
		/// <summary>
		/// ����㷨
		/// </summary>
		AlgorithmMode compositing;
		/// <summary>
		/// ����ƫ���㷨
		/// </summary>
		AlgorithmMode pixeloffset;
		/// <summary>
		/// ������㷨
		/// </summary>
		AlgorithmMode smoothing;
		/// <summary>
		/// ��ֵ�㷨
		/// </summary>
		AlgorithmMode interpolation;
	};

	/// <summary>
	/// ����ģʽ
	/// </summary>
	inline constexpr AlgorithmModes FAST_MODE{ AlgorithmMode::FAST, AlgorithmMode::FAST, AlgorithmMode::FAST, AlgorithmMode::FAST };
	/// <summary>
	/// ������ģʽ
	/// </summary>
	inline constexpr AlgorithmModes QUALITY_MODE{ AlgorithmMode::QUALITY, AlgorithmMode::QUALITY, AlgorithmMode::QUALITY, AlgorithmMode::QUALITY };

	/// <summary>
	/// ͼ��GUID
	/// </summary>
	struct ImageCLSID {
	private:
		static constexpr mqguid BMP_E{ 0x557cf400, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
		static constexpr mqguid JPG_E{ 0x557cf401, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
		static constexpr mqguid GIF_E{ 0x557cf402, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
		static constexpr mqguid TIFF_E{ 0x557cf405, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
		static constexpr mqguid PNG_E{ 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };

		static constexpr auto FormatCountMaxValue{ 7U };
		static constexpr mqguid _Encoders[FormatCountMaxValue] = { BMP_E, JPG_E, GIF_E, { }, { }, TIFF_E, PNG_E };

	public:
		ImageCLSID() = delete;
		[[nodiscard]] static constexpr bool _BuiltinFormat(ImageFormat ifmt) noexcept {
			return static_cast<mqui32>(ifmt) < FormatCountMaxValue;
		}

		[[nodiscard]] static constexpr const mqguid& _GetEncoder(ImageFormat ifmt) noexcept {
			return _Encoders[static_cast<mqui32>(ifmt)];
		}
	};

	/// <summary>
	/// ͼ��λ��
	/// </summary>
	struct ImagePoint {
		mqui32 x, y;

		[[nodiscard]] bool operator == (ImagePoint const& other) const noexcept {
			return x == other.x && y == other.y;
		}
	};

	/// <summary>
	/// ͼ��ߴ�
	/// </summary>
	struct ImageSize {
		mqui32 width, height;

		[[nodiscard]] bool operator == (ImageSize const& other) const noexcept {
			return width == other.width && height == other.height;
		}
	};

	/// <summary>
	/// ͼ��ֱ���
	/// </summary>
	struct ImageDPI {
		mqui32 horizontal, vertical;

		[[nodiscard]] bool operator == (ImageDPI const& other) const noexcept {
			return horizontal == other.horizontal && vertical == other.vertical;
		}
	};

	/// <summary>
	/// ͼ�����
	/// </summary>
	struct ImageRect {
		mqui32 left, top, width, height;

		[[nodiscard]] bool operator == (ImageRect const& other) const noexcept {
			return left == other.left && top == other.top && width == other.width && height == other.height;
		}
	};

	/// <summary>
	/// ͼ��Χ
	/// </summary>
	struct ImagePosition {
		mqui32 left, top, right, bottom;

		[[nodiscard]] bool operator == (ImagePosition const& other) const noexcept {
			return left == other.left && top == other.top && right == other.right && bottom == other.bottom;
		}
	};

	/// <summary>
	/// ͼ���ı�
	/// </summary>
	struct ImageText {
		mqcstr content;
		mqui32 size;
		Color color;
		mqcstr font;
	};
}